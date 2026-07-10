import os.path
import string
from pathlib import Path


class GrabError(RuntimeError):
    pass


def escape_cpp_string(s: str) -> str:
    """Escape text destined for a C++ string literal in the template --
    descriptions and parameter names/labels are free text pulled from
    Airwindopedia.txt/upstream source and can contain a literal '"'."""
    return s.replace("\\", "\\\\").replace('"', '\\"')


def first_matching_line_index(lines: list[str], match: str, line_rstrip=None) -> int:
    for index, line in enumerate(lines):
        if line_rstrip:
            line = line.rstrip()
        if match in line:
            return index
    raise GrabError(f"Could not find {match} in lines")


def next_matching_line_index(lines: list[str], offset: int, match: str) -> int:
    return first_matching_line_index(lines[offset:], match) + offset


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


def find_missing_plugins(project_root: Path, categories: dict[str, str]) -> list[str]:
    """Plugin titles from `categories` that don't have a generated header yet."""
    missing = []
    for title, category in categories.items():
        slug = title.replace(" ", "").lower()
        output_path = project_root / "include" / category / f"{slug}.hpp"
        if not output_path.exists():
            missing.append(title)
    return missing


def num_to_alpha(num: int) -> str:
    return str(chr(65 + num))


def warning(plugin: str, error: str):
    print(f"{plugin} (warning): {error}")


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
    label: str
    """The parameter label, often the units of the parameter"""

    def __init__(self, variable: str, title: str, default_value: float, label: str, member: str = None):
        self.variable = variable
        self.member = member if member is not None else variable
        self.enum_name = f"kParam{variable}"
        self.title = title
        self.slug = self._slug()
        self.default_value = default_value
        self.label = label

    def _slug(self):
        to_strip = {"-", "/", ""}
        slug = self.title.lower()
        for elem in to_strip:
            slug.replace(elem, "")
        return slug


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
    parameters: dict[str, Parameter]

    def __init__(self, project_root: Path, airwindows_root: Path, airwindopedia_path: Path, title: str,
                 category: str):
        self.project_root = project_root
        self.airwindopedia_path = airwindopedia_path
        self.title = title.replace(" ", "")
        self.slug = self.title.lower()
        self.category = category
        self.short_description, self.long_description = self._init_descriptions()

        self.source_dir = Path(os.path.join(airwindows_root, self.title))
        self.output_path = Path(os.path.join(project_root, "include", self.category, self.slug + ".hpp"))

        # deferred import: clang_extractor imports GrabError/naive_line_filter from this module
        from scripts.grab.clang_extractor import PluginAst
        ast = PluginAst(self.source_dir, self.title)

        self.initialization_code = ast.initialization_code()
        self.processing_code = ast.processing_code()
        self.private_vars = ast.private_vars()
        self.parameters = self._init_parameters(ast)

    def _init_parameters(self, ast):
        num_parameters = ast.num_parameters()
        if num_parameters == 0:
            return dict()

        name_map = ast.parameter_strings("getParameterName")
        label_map = ast.parameter_strings("getParameterLabel")
        member_map = ast.parameter_members()

        result = dict()
        for i in range(num_parameters):
            variable = num_to_alpha(i)

            title = name_map.get(variable)
            if title is None:
                warning(self.title, "Could not find parameter name; using a default instead")
                title = variable

            label = label_map.get(variable)
            if label is None:
                warning(self.title, f"Could not find parameter label for {variable} -- using empty string")
                label = ""

            member = member_map.get(variable)
            if member is None:
                warning(self.title, f"Could not find member variable for {variable}; assuming it's named '{variable}'")
                member = variable

            default_value = ast.default_value(member)
            result[variable] = Parameter(variable, title, default_value, label, member=member)
        return result

    def _init_descriptions(self):
        with open(self.airwindopedia_path, "r") as f:
            lines = f.readlines()

        try:
            begin_index = first_matching_line_index(lines, f"# {self.title}")
            next_index = next_matching_line_index(lines, begin_index + 1, "# ")
            short_description = lines[begin_index].replace("#", "").strip()

            chunk = lines[begin_index + 1:next_index]
            chunk = [line.replace("\n", "") for line in chunk]
            long_description = "".join(naive_line_filter(chunk))
            return escape_cpp_string(short_description), escape_cpp_string(long_description)
        except RuntimeError as e:
            warning(self.title, "could not find descriptions; using empty strings")
            return "", ""

    def _to_dict(self) -> dict:
        d = {
            "namespace": self.slug,
            "short_description": self.short_description,
            "long_description": self.long_description,
            "tags": self.category,
            "class_name": self.title,
            "private_variables": self.private_vars,
            "initialization": self.initialization_code,
            "process": self.processing_code,
            # param-specific:
            "param_enum": self._param_enum(),
            "set_parameter_value_switch": self._param_set_value_switch(),
            "get_parameter_value_switch": self._param_get_value_switch(),
            "get_parameter_default_switch": self._param_default_switch(),
            "get_parameter_name_switch": self._param_name_switch(),
            "get_parameter_title_switch": self._param_title_switch(),
            "get_parameter_display_switch": self._parameter_display_switch(),
            "get_parameter_label_switch": self._parameter_label_switch(),
        }
        return d

    def _param_enum(self) -> str:
        lines = []
        for index, param in enumerate(self.parameters.values()):
            line = f"{param.enum_name} = {index},\n"
            lines.append(line)
        lines.append(f"kNumParameters = {len(self.parameters)}")
        return "".join(lines)

    def _param_set_value_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: {param.member} = value; break;\n"
            lines.append(line)
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

    def _parameter_display_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return std::to_string({param.member}); break;\n"
            lines.append(line)
        return "".join(lines)

    def _parameter_label_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return \"{escape_cpp_string(param.label)}\"; break;\n"
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


class Grabber:
    _project_root: Path
    _airwindows_root: Path
    _airwindopedia_path: Path
    _template_header_path: Path
    _categories: dict[str, str]
    _categories_transpose: dict[str, list[str]]

    def __init__(self, project_root: Path, airwindows_root: Path, airwindopedia_path: Path,
                 plugin_filter: set[str] = None, category_filter: set[str] = None):
        self._project_root = project_root
        self._airwindows_root = airwindows_root
        if not os.path.exists(airwindows_root) or not os.path.exists(os.path.join(airwindows_root, "Aura")):
            raise FileNotFoundError(f"Could not find airwindows plugin source at {airwindows_root} "
                                    "(expected a subdirectory per plugin, e.g. `Aura/`)")
        if not os.path.exists(airwindopedia_path):
            raise FileNotFoundError(f"Could not find Airwindopedia.txt at {airwindopedia_path}")

        self._airwindopedia_path = airwindopedia_path
        self._template_header_path = Path(os.path.join(self._project_root, "scripts", "res", "template.hpp"))
        self._categories, self._categories_transpose = parse_categories(airwindopedia_path)
        self._init_plugins(plugin_filter, category_filter)

    def _init_plugins(self, plugin_filter: set[str] = None, category_filter: set[str] = None):
        for plugin, category in self._categories.items():
            if plugin_filter and plugin not in plugin_filter:
                continue
            if category_filter and category not in category_filter:
                continue
            try:
                print(f"Writing {plugin} =====")
                plug = Plugin(self._project_root, self._airwindows_root, self._airwindopedia_path, plugin, category)
                plug.write(self._project_root, self._template_header_path)
            except GrabError as e:
                print(f">>> Failure... {e}\n")
