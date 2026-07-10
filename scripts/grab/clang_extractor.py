import functools
import os.path
import subprocess
from pathlib import Path

import clang.cindex as cindex

from scripts.grab.grabber import GrabError, naive_line_filter, num_to_alpha

STUB_HEADER = Path(__file__).resolve().parent.parent / "res" / "vst_stub.hpp"


@functools.lru_cache(maxsize=1)
def _toolchain_args() -> tuple:
    """Auto-detect the system C++ compiler's header search paths.

    The libclang wheel this generator depends on ships only a shared library,
    no headers -- without these, even compiler-builtin headers like stdarg.h
    fail to resolve, which (like an unresolved base class) causes libclang to
    silently drop AST nodes instead of raising a usable diagnostic.
    """
    resource_dir = subprocess.run(
        ["clang", "-print-resource-dir"], check=True, capture_output=True, text=True,
    ).stdout.strip()

    verbose = subprocess.run(
        ["clang++", "-x", "c++", "-E", "-v", "-"],
        input="", check=True, capture_output=True, text=True,
    ).stderr

    isystem_paths = []
    in_search_list = False
    for line in verbose.splitlines():
        if "#include <...> search starts here" in line:
            in_search_list = True
            continue
        if line.startswith("End of search list"):
            break
        if in_search_list:
            isystem_paths.append(line.strip().split(" ")[0])

    args = ["-resource-dir", resource_dir]
    for path in isystem_paths:
        args += ["-isystem", path]
    return tuple(args)


def parse_source(path: Path) -> cindex.TranslationUnit:
    """Parse a plugin source file with a synthetic VST SDK stub so the AST
    reflects the real source structure without the (unavailable) real SDK."""
    index = cindex.Index.create()
    args = [
        "-std=c++14", "-x", "c++",
        "-D__audioeffect__", "-include", str(STUB_HEADER),
        *_toolchain_args(),
    ]
    tu = index.parse(str(path), args=args)
    errors = [d for d in tu.diagnostics if d.severity >= cindex.Diagnostic.Error]
    if errors:
        messages = "; ".join(f"{d.location}: {d.spelling}" for d in errors)
        raise GrabError(f"libclang could not cleanly parse {path.name}: {messages}")
    return tu


def find_all(cursor: cindex.Cursor, predicate, file: Path = None) -> list:
    results = []

    def walk(c):
        if file is not None and c.location.file and str(c.location.file) != str(file):
            return
        if predicate(c):
            results.append(c)
        for child in c.get_children():
            walk(child)

    walk(cursor)
    return results


def find_first(cursor: cindex.Cursor, predicate, file: Path = None, description: str = ""):
    results = find_all(cursor, predicate, file=file)
    if not results:
        raise GrabError(f"Could not find {description or 'matching cursor'}")
    return results[0]


def _line_start_offset(source: bytes, offset: int) -> int:
    """Byte offset of the start of the physical line containing `offset`,
    so slicing includes that line's leading indentation."""
    newline = source.rfind(b"\n", 0, offset)
    return newline + 1


def _extend_over_leading_comments(source: bytes, start_offset: int) -> int:
    """Comments aren't part of any statement's AST extent, so a boundary
    anchored to a statement's own start silently drops any comment lines
    immediately preceding it. Walk backward over blank/line-comment lines
    to include them."""
    cursor = start_offset
    while cursor > 0:
        prev_line_start = source.rfind(b"\n", 0, cursor - 1) + 1
        prev_line = source[prev_line_start:cursor - 1].decode("utf-8", errors="replace").strip()
        if prev_line == "" or prev_line.startswith("//"):
            cursor = prev_line_start
        else:
            break
    return cursor


def _slice_and_filter(source: bytes, start_offset: int, end_offset: int) -> str:
    raw = source[start_offset:end_offset].decode("utf-8", errors="replace")
    return "".join(naive_line_filter(raw.splitlines(keepends=True)))


def _references_any(cursor: cindex.Cursor, names: set) -> bool:
    return bool(find_all(cursor, lambda c: c.kind == cindex.CursorKind.DECL_REF_EXPR and c.spelling in names))


def _case_variable(case: cindex.Cursor) -> str:
    """The parameter letter (A, B, ...) a `case` in a parameter switch
    corresponds to, resolved by the case value's actual enum position --
    not by string-matching the enum constant's name -- so this works
    regardless of naming convention (kParamA, kSlewParam, kParamTRF, ...).
    Returns None for cases (e.g. `default:`) that don't resolve to an index."""
    case_value = list(case.get_children())[0]

    enum_refs = find_all(case_value, lambda c: c.kind == cindex.CursorKind.DECL_REF_EXPR)
    if enum_refs:
        referenced = enum_refs[0].referenced
        if referenced is not None and referenced.kind == cindex.CursorKind.ENUM_CONSTANT_DECL:
            return num_to_alpha(referenced.enum_value)

    literals = find_all(case_value, lambda c: c.kind == cindex.CursorKind.INTEGER_LITERAL)
    if literals:
        return num_to_alpha(int(next(literals[0].get_tokens()).spelling))

    return None


class PluginAst:
    """AST-backed replacement for the naive line/string scanning in the
    original grab.py: libclang locates the correct source byte ranges
    (constructor body, private section, method bodies), then the original
    source text between those ranges is sliced -- never re-emitted from the
    AST, so comments and formatting are preserved exactly as before."""

    def __init__(self, source_dir: Path, title: str):
        self.title = title
        self.cpp_path = Path(os.path.join(source_dir, f"{title}.cpp"))
        self.h_path = Path(os.path.join(source_dir, f"{title}.h"))
        self.proc_path = Path(os.path.join(source_dir, f"{title}Proc.cpp"))

        self.cpp_tu = parse_source(self.cpp_path)
        self.h_tu = parse_source(self.h_path)
        self.proc_tu = parse_source(self.proc_path)

        self.cpp_source = self.cpp_path.read_bytes()
        self.h_source = self.h_path.read_bytes()
        self.proc_source = self.proc_path.read_bytes()

    def _constructor_definition(self) -> cindex.Cursor:
        return find_first(
            self.cpp_tu.cursor,
            lambda c: c.kind == cindex.CursorKind.CONSTRUCTOR and c.is_definition(),
            file=self.cpp_path, description=f"{self.title}::{self.title} constructor definition",
        )

    def _method_definition(self, name: str, file_path: Path, tu: cindex.TranslationUnit) -> cindex.Cursor:
        return find_first(
            tu.cursor,
            lambda c: c.kind == cindex.CursorKind.CXX_METHOD and c.spelling == name and c.is_definition(),
            file=file_path, description=f"{self.title}::{name} definition",
        )

    def num_parameters(self) -> int:
        enum_const = find_first(
            self.h_tu.cursor,
            lambda c: c.kind == cindex.CursorKind.ENUM_CONSTANT_DECL and c.spelling == "kNumParameters",
            file=self.h_path, description="kNumParameters enum constant",
        )
        return enum_const.enum_value

    def initialization_code(self) -> str:
        ctor = self._constructor_definition()
        body = find_first(ctor, lambda c: c.kind == cindex.CursorKind.COMPOUND_STMT,
                           description="constructor body")
        stmts = list(body.get_children())

        stop_index = len(stmts)
        for i, stmt in enumerate(stmts):
            if find_all(stmt, lambda c: c.kind == cindex.CursorKind.MEMBER_REF_EXPR and c.spelling == "_canDo"):
                stop_index = i
                break

        start_offset = body.extent.start.offset + 1  # just after the opening '{'
        end_offset = stmts[stop_index].extent.start.offset if stop_index < len(stmts) else body.extent.end.offset - 1
        return _slice_and_filter(self.cpp_source, start_offset, end_offset)

    def processing_code(self) -> str:
        method = self._method_definition("processDoubleReplacing", self.proc_path, self.proc_tu)
        body = find_first(method, lambda c: c.kind == cindex.CursorKind.COMPOUND_STMT,
                           description="processDoubleReplacing body")
        stmts = list(body.get_children())

        skip = 0
        for stmt in stmts:
            if stmt.kind == cindex.CursorKind.DECL_STMT and _references_any(stmt, {"inputs", "outputs"}):
                skip += 1
            else:
                break
        if skip == 0 or skip >= len(stmts):
            raise GrabError("Could not find input/output pointer declarations in processDoubleReplacing")

        start_offset = _line_start_offset(self.proc_source, stmts[skip].extent.start.offset)
        start_offset = _extend_over_leading_comments(self.proc_source, start_offset)
        # stmts[-1].extent.end can land before a trailing ';' or comment (e.g. a bare
        # expression statement as the function's last statement) -- slice to just
        # before the function body's own closing '}' instead, like initialization_code().
        end_offset = body.extent.end.offset - 1
        result = _slice_and_filter(self.proc_source, start_offset, end_offset)
        return result.replace("getSampleRate()", "Effect<T>::getSampleRate()")

    def file_scope_constants(self) -> str:
        """Some plugins declare const globals in the .h (usually array-size
        constants like `const int kshortA = 193;`) ahead of the class, and
        reference them directly in private member declarations (e.g.
        `double aAL[kshortA + 5];`). Carrying only the class's own private
        section leaves those undefined, so pull in every top-level VAR_DECL
        too -- harmless if unused (e.g. kNumInputs), required if not."""
        decls = find_all(
            self.h_tu.cursor,
            lambda c: c.kind == cindex.CursorKind.VAR_DECL
            and c.semantic_parent.kind == cindex.CursorKind.TRANSLATION_UNIT,
            file=self.h_path,
        )
        lines = []
        for decl in decls:
            text = self.h_source[decl.extent.start.offset:decl.extent.end.offset].decode("utf-8", errors="replace")
            text = text.rstrip().rstrip(";").strip()
            # normalize away whatever qualifiers the original had (const,
            # static, or neither) -- a non-static member's initializer isn't
            # a constant-expression even when const+literal (needs `static`
            # to be usable as a sibling member's array bound), and an
            # in-class initializer for an array-typed static member needs
            # `constexpr`, not just `const`.
            for prefix in ("static constexpr ", "static const ", "constexpr ", "const ", "static "):
                if text.startswith(prefix):
                    text = text[len(prefix):]
                    break
            lines.append(f"static constexpr {text};\n")
        return "".join(lines)

    def private_vars(self) -> str:
        class_decl = find_first(
            self.h_tu.cursor,
            lambda c: c.kind == cindex.CursorKind.CLASS_DECL and c.spelling == self.title and c.is_definition(),
            file=self.h_path, description=f"class {self.title} definition",
        )
        private_spec = find_first(
            class_decl,
            lambda c: c.kind == cindex.CursorKind.CXX_ACCESS_SPEC_DECL
            and c.access_specifier == cindex.AccessSpecifier.PRIVATE,
            description=f"private: section in class {self.title}",
        )
        start_offset = private_spec.extent.end.offset
        end_offset = class_decl.extent.end.offset - 1  # exclude the class's closing '}'
        return self.file_scope_constants() + _slice_and_filter(self.h_source, start_offset, end_offset)

    def default_value(self, member: str) -> float:
        ctor = self._constructor_definition()
        assign = find_first(
            ctor,
            lambda c: c.kind == cindex.CursorKind.BINARY_OPERATOR and any(
                child.kind == cindex.CursorKind.MEMBER_REF_EXPR and child.spelling == member
                for child in c.get_children()
            ),
            description=f"assignment to {member} in constructor",
        )
        rhs = list(assign.get_children())[1]
        literal = find_first(
            rhs, lambda c: c.kind in (cindex.CursorKind.FLOATING_LITERAL, cindex.CursorKind.INTEGER_LITERAL),
            description=f"default value literal for {member}",
        )
        token = next(literal.get_tokens()).spelling
        return float(token)

    def parameter_strings(self, method_name: str) -> dict:
        """Maps a parameter's variable letter (A, B, ...) to the string
        literal argument of the call in that switch's matching case, e.g.
        getParameterName's `case kParamA: vst_strncpy(text, "Voicing", ...)`.
        Some plugins skip the switch entirely (e.g. when every label is the
        same empty string) -- that's not an error, just no strings to map."""
        method = self._method_definition(method_name, self.cpp_path, self.cpp_tu)
        switches = find_all(method, lambda c: c.kind == cindex.CursorKind.SWITCH_STMT)
        if not switches:
            return {}
        result = {}
        for case in find_all(switches[0], lambda c: c.kind == cindex.CursorKind.CASE_STMT):
            variable = _case_variable(case)
            if variable is None:
                continue
            strings = find_all(case, lambda c: c.kind == cindex.CursorKind.STRING_LITERAL)
            if not strings:
                continue
            result[variable] = strings[0].spelling.strip('"')
        return result

    def parameter_members(self) -> dict:
        """Maps a parameter's variable letter (A, B, ...) to the real C++
        member variable backing it, resolved from setParameter's switch,
        e.g. `case kParamA: consoletype = value; break;` -> {"A": "consoletype"}.
        Usually this is just the letter itself, but older plugins often use
        descriptive member names instead of the A/B/C convention."""
        method = self._method_definition("setParameter", self.cpp_path, self.cpp_tu)
        switches = find_all(method, lambda c: c.kind == cindex.CursorKind.SWITCH_STMT)
        if not switches:
            return {}
        result = {}
        for case in find_all(switches[0], lambda c: c.kind == cindex.CursorKind.CASE_STMT):
            variable = _case_variable(case)
            if variable is None:
                continue
            member_refs = find_all(case, lambda c: c.kind == cindex.CursorKind.MEMBER_REF_EXPR)
            if not member_refs:
                continue
            result[variable] = member_refs[0].spelling
        return result
