import os.path
import re
import string
from pathlib import Path


class GrabError(RuntimeError):
    pass


def escape_cpp_string(s: str) -> str:
    """Escape text destined for a C++ string literal in the template --
    descriptions and parameter names/labels are free text pulled from
    Airwindopedia.txt/upstream source and can contain a literal '"'."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def naive_line_filter(lines: list[str]) -> list[str]:
    result = []
    ignore = {"{\n", "}\n", "\n"}
    for line in lines:
        if line not in ignore \
                and "_canDo" not in line \
                and "private:" not in line \
                and "_programName" not in line \
                and line.strip() != "":
            result.append(line)
    return result


# Plugins whose Airwindopedia.txt entry is headed by a different name than
# the one its category list uses.
_DESCRIPTION_TITLE_ALIASES = {
    "C5RawChannel": "C5RawConsole",
}

UNCATEGORIZED = "uncategorized"


def resolve_source_title(airwindows_root: Path, title: str) -> str:
    """Return the upstream source directory name matching `title` ignoring case,
    since Airwindopedia.txt's spelling can differ from it ("Derez5" vs "DeRez5")."""
    # Scan rather than test the path: a case-insensitive filesystem would accept
    # the wrong spelling and leak it into the class name.
    matches = [e.name for e in Path(airwindows_root).iterdir()
               if e.is_dir() and e.name.casefold() == title.casefold()]
    if title in matches:
        return title
    if matches:
        return matches[0]
    raise GrabError(f"no source directory for {title} under {airwindows_root}")


def make_missing_directory(path: Path):
    if not path.exists():
        os.mkdir(path)


# Airwindopedia.txt's "XYZ Filters" category line has a typo joining two
# distinct plugins with a period instead of a comma ("XBandpass. XHighpass").
# Both exist as real, separate upstream plugins -- split them back apart.
_CATEGORY_ELEMENT_FIXUPS = {
    "XBandpass. XHighpass": ["XBandpass", "XHighpass"],
}


def _split_category(s: str):
    category = s.split(":")[0].strip().lower().replace(" ", "-")
    raw_elements = s.split(":")[1].split(",")
    elements = []
    for raw_element in raw_elements:
        stripped = raw_element.strip()
        elements.extend(_CATEGORY_ELEMENT_FIXUPS.get(stripped, [stripped]))
    return category, elements


def parse_categories(airwindopedia_path: Path) -> tuple[dict[str, str], dict[str, list[str]]]:
    """Parse the `# Categories` section of Airwindopedia.txt into
    (plugin title -> category) and (category -> [plugin titles])."""
    with open(airwindopedia_path, "r") as f:
        lines = f.readlines()

    categories = dict()
    categories_transpose = dict()
    in_categories = False
    for line in lines:
        if line.startswith("#####"):
            in_categories = False
        if in_categories:
            stripped = line.strip()
            if stripped != "":
                category, elements = _split_category(stripped)
                for element in elements:
                    categories[element] = category
                if category in categories_transpose.keys():
                    categories_transpose[category] += elements
                else:
                    categories_transpose[category] = elements
        if line.startswith("# Categories"):
            in_categories = True
    return categories, categories_transpose


def plugin_categories(airwindopedia_path: Path, airwindows_root: Path) -> dict[str, str]:
    """Plugin title -> category for every plugin to generate: Airwindopedia's
    categories, plus UNCATEGORIZED for each upstream source directory that
    no Airwindopedia title claims."""
    categories, _ = parse_categories(airwindopedia_path)
    claimed = {t.replace(" ", "").casefold() for t in categories}
    for entry in sorted(Path(airwindows_root).iterdir(), key=lambda e: e.name):
        if entry.is_dir() and entry.name.casefold() not in claimed:
            categories[entry.name] = UNCATEGORIZED
    return categories


def header_path(project_root: Path, title: str, category: str) -> Path:
    return project_root / "include" / category / f"{title.replace(' ', '').lower()}.hpp"


def existing_plugins_in_dirs(project_root: Path, categories: dict[str, str], source_dirs: set[str]) -> set[str]:
    """Titles from `categories` that already have a header and whose upstream
    source directory is one of `source_dirs`."""
    wanted = {d.casefold() for d in source_dirs}
    return {
        title for title, category in categories.items()
        if title.replace(" ", "").casefold() in wanted
        and header_path(project_root, title, category).exists()
    }


def find_headers(project_root: Path, title: str) -> list[Path]:
    """Every generated header for `title`, under whichever category directory it was written to."""
    slug = title.replace(" ", "").lower()
    return [p for p in (project_root / "include").glob(f"*/{slug}.hpp") if p.parent.name != "meta"]


def find_missing_plugins(project_root: Path, categories: dict[str, str]) -> list[str]:
    """Plugin titles from `categories` that don't have a generated header in any category yet."""
    return [title for title in categories if not find_headers(project_root, title)]


def find_recategorized_headers(project_root: Path, categories: dict[str, str]) -> dict[str, list[Path]]:
    """Titles whose header sits under a category other than the one `categories`
    now assigns, mapped to those stale headers."""
    stale = {}
    for title, category in categories.items():
        current = header_path(project_root, title, category)
        others = [p for p in find_headers(project_root, title) if p != current]
        if others:
            stale[title] = others
    return stale


def num_to_alpha(num: int) -> str:
    """Spreadsheet-style column letters: 0 -> A, 25 -> Z, 26 -> AA."""
    letters = ""
    num += 1
    while num:
        num, rem = divmod(num - 1, 26)
        letters = chr(65 + rem) + letters
    return letters


def warning(plugin: str, error: str):
    print(f"{plugin} (warning): {error}")


_SHADOW_WARNINGS_PUSH = """#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4458)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
#endif
"""

_SHADOW_WARNINGS_POP = """#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
"""


# A plugin namespace with one of these names (Floor) would hide the math
# function from every plugin namespace in airwindohhs that calls it unqualified.
_C_MATH_FUNCTIONS = {
    "abs", "acos", "acosh", "asin", "asinh", "atan", "atan2", "atanh", "cbrt", "ceil", "copysign",
    "cos", "cosh", "erf", "erfc", "exp", "exp2", "expm1", "fabs", "fdim", "floor", "fma", "fmax",
    "fmin", "fmod", "frexp", "hypot", "ilogb", "ldexp", "lgamma", "llrint", "llround", "log",
    "log10", "log1p", "log2", "logb", "lrint", "lround", "modf", "nan", "nearbyint", "nextafter",
    "nexttoward", "pow", "remainder", "remquo", "rint", "round", "scalbln", "scalbn", "sin", "sinh",
    "sqrt", "tan", "tanh", "tgamma", "trunc",
}


class Parameter:
    """A parameter represents a VST parameter, a variable that can change how the plugin does the processing"""

    variable: str
    """A variable name, usually a capital letter"""
    member: str
    """The real C++ member variable backing this parameter -- usually the
    same as `variable`, but some plugins use descriptive member names
    instead of the A/B/C convention (e.g. `consoletype` for kParamA)"""
    enum_name: str
    """An enum name, usually something like kParamA"""
    title: str
    """The title-case version of the parameter name"""
    slug: str
    """The slug-ified version of the parameter name"""
    default_value: float
    """The default parameter value"""

    def __init__(self, variable: str, title: str, default_value: float, member: str = None):
        self.variable = variable
        self.member = member if member is not None else variable
        self.enum_name = f"kParam{variable}"
        self.title = title
        self.slug = self._slug()
        self.default_value = default_value

    def _slug(self):
        return re.sub(r"[^a-z0-9]", "", self.title.lower())


class Plugin:
    source_dir: Path
    output_path: Path

    title: str
    slug: str
    category: str

    short_description: str
    long_description: str

    initialization_code: str
    private_vars: str
    processing_code: str
    set_parameter_code: str
    display_code: str
    label_code: str
    parameters: dict[str, Parameter]

    def __init__(self, project_root: Path, airwindows_root: Path, airwindopedia_path: Path, title: str,
                 category: str):
        self.project_root = project_root
        self.airwindopedia_path = airwindopedia_path
        listed_title = title.replace(" ", "")
        source_title = resolve_source_title(airwindows_root, listed_title)
        self.title = source_title
        self.slug = self.title.lower()
        self.category = category
        self.short_description, self.long_description = self._init_descriptions(
            _DESCRIPTION_TITLE_ALIASES.get(listed_title, listed_title))

        self.source_dir = Path(os.path.join(airwindows_root, source_title))
        self.output_path = Path(os.path.join(project_root, "include", self.category, self.slug + ".hpp"))

        # deferred import: clang_extractor imports GrabError/naive_line_filter from this module
        from scripts.grab.clang_extractor import PluginAst
        ast = PluginAst(self.source_dir, source_title)

        self.initialization_code = ast.initialization_code()
        self.processing_code = ast.processing_code()
        self.private_vars = ast.private_vars()
        self.set_parameter_code = ast.method_body("setParameter")
        self.display_code = ast.method_body("getParameterDisplay")
        self.label_code = ast.method_body("getParameterLabel")
        self.shadows_members = ast.shadows_members()
        self.parameters = self._init_parameters(ast)

        injected, injected_names = ast.uninitialized_members()
        if injected:
            warning(self.title, f"constructor never assigns {', '.join(injected_names)}; initializing to defaults")
            if not self.initialization_code.endswith("\n"):
                self.initialization_code += "\n"
            self.initialization_code += injected

        # Only macros the emitted code references, so plugins that define but
        # never test them don't grow dead directives.
        emitted = (self.private_vars + self.initialization_code + self.processing_code
                   + self.set_parameter_code + self.display_code + self.label_code)
        self.namespace = f"{self.slug}effect" if self.slug in _C_MATH_FUNCTIONS else self.slug
        self.macros = {
            name: line for name, line in ast.file_scope_macros().items()
            if re.search(rf"\b{re.escape(name)}\b", emitted)
        }

    def _init_parameters(self, ast):
        num_parameters = ast.num_parameters()
        if num_parameters == 0:
            return dict()

        name_map = ast.parameter_strings("getParameterName")
        member_map = ast.parameter_members()

        result = dict()
        for i in range(num_parameters):
            variable = num_to_alpha(i)

            title = name_map.get(variable)
            if title is None:
                warning(self.title, "Could not find parameter name; using a default instead")
                title = variable

            member = member_map.get(variable)
            if member is None:
                warning(self.title, f"Could not find member variable for {variable}; assuming it's named '{variable}'")
                member = variable

            default_value = ast.default_value(member)
            result[variable] = Parameter(variable, title, default_value, member=member)
        return result

    def _init_descriptions(self, description_title: str):
        with open(self.airwindopedia_path, "r") as f:
            lines = f.readlines()

        heading = re.compile(rf"^#+ {re.escape(description_title)}(?![\w-])", re.IGNORECASE)
        begin_index = next((i for i, line in enumerate(lines) if heading.match(line)), None)
        if begin_index is None:
            warning(self.title, "could not find descriptions; using empty strings")
            return "", ""
        next_index = next((i for i in range(begin_index + 1, len(lines)) if lines[i].startswith("#")), len(lines))
        short_description = lines[begin_index].replace("#", "").strip()

        chunk = [line.replace("\n", "") for line in lines[begin_index + 1:next_index]]
        long_description = " ".join(line.strip() for line in naive_line_filter(chunk))
        return escape_cpp_string(short_description), escape_cpp_string(long_description)

    def _to_dict(self) -> dict:
        d = {
            "namespace": self.namespace,
            "short_description": self.short_description,
            "long_description": self.long_description,
            "tags": self.category,
            "class_name": self.title,
            "private_variables": self.private_vars,
            "initialization": self.initialization_code,
            "process": self.processing_code,
            "macro_defines": "".join(f"{line}\n" for line in self.macros.values()) + ("\n" if self.macros else ""),
            # undefined again so one header's topology can't switch on blocks in another
            "macro_undefs": ("\n" if self.macros else "") + "".join(f"#undef {name}\n" for name in self.macros),
            # param-specific:
            "param_enum": self._param_enum(),
            "set_parameter_value_body": self.set_parameter_code,
            "get_parameter_value_switch": self._param_get_value_switch(),
            "get_parameter_default_switch": self._param_default_switch(),
            "get_parameter_name_switch": self._param_name_switch(),
            "get_parameter_title_switch": self._param_title_switch(),
            "get_parameter_display_body": self.display_code,
            "get_parameter_label_body": self.label_code,
            "shadow_warnings_push": _SHADOW_WARNINGS_PUSH if self.shadows_members else "",
            "shadow_warnings_pop": _SHADOW_WARNINGS_POP if self.shadows_members else "",
        }
        return d

    def _param_enum(self) -> str:
        lines = []
        for index, param in enumerate(self.parameters.values()):
            line = f"{param.enum_name} = {index},\n"
            lines.append(line)
        lines.append(f"kNumParameters = {len(self.parameters)}")
        return "".join(lines)

    def _param_get_value_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return {param.member}; break;\n"
            lines.append(line)
        return "".join(lines)

    def _param_default_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return {param.default_value}; break;\n"
            lines.append(line)
        return "".join(lines)

    def _param_name_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return \"{escape_cpp_string(param.slug)}\"; break;\n"
            lines.append(line)
        return "".join(lines)

    def _param_title_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return \"{escape_cpp_string(param.title)}\"; break;\n"
            lines.append(line)
        return "".join(lines)

    def write(self, project_root: Path, template_header: Path):
        with open(template_header, "r") as f:
            template = f.read()
        template = string.Template(template)

        try:
            filled_template = template.substitute(self._to_dict())
        except KeyError:
            raise GrabError("Could not substitute template")

        make_missing_directory(Path(os.path.join(project_root, "include")))
        make_missing_directory(Path(os.path.join(project_root, "include", self.category)))

        output_path = Path(os.path.join(project_root, "include", self.category, f"{self.slug}.hpp"))
        with open(output_path, "w") as f:
            f.write(filled_template)


def generate(project_root: Path, airwindows_root: Path, airwindopedia_path: Path,
             plugin_filter: set[str] = None, category_filter: set[str] = None) -> list[tuple[str, str]]:
    """Write a header for each selected plugin, returning (title, message) for each one that failed."""
    if not os.path.exists(airwindows_root) or not os.path.exists(os.path.join(airwindows_root, "Aura")):
        raise FileNotFoundError(f"Could not find airwindows plugin source at {airwindows_root} "
                                "(expected a subdirectory per plugin, e.g. `Aura/`)")
    if not os.path.exists(airwindopedia_path):
        raise FileNotFoundError(f"Could not find Airwindopedia.txt at {airwindopedia_path}")

    template_header_path = Path(os.path.join(project_root, "scripts", "res", "template.hpp"))
    failures = []
    for plugin, category in plugin_categories(airwindopedia_path, airwindows_root).items():
        if plugin_filter and plugin not in plugin_filter:
            continue
        if category_filter and category not in category_filter:
            continue
        try:
            print(f"Writing {plugin} =====")
            plug = Plugin(project_root, airwindows_root, airwindopedia_path, plugin, category)
            plug.write(project_root, template_header_path)
        except GrabError as e:
            print(f">>> Failure... {e}\n")
            failures.append((plugin, str(e)))
    return failures
