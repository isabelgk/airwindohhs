import os.path
import string
from pathlib import Path
import re


class GrabError(RuntimeError):
    pass


def first_matching_line_index(lines: list[str], match: str, line_rstrip=None) -> int:
    for index, line in enumerate(lines):
        if line_rstrip:
            line = line.rstrip()
        if match in line:
            return index
    raise GrabError(f"Could not find {match} in lines")


def next_matching_line_index(lines: list[str], offset: int, match: str) -> int:
    return first_matching_line_index(lines[offset:], match) + offset


def matching_line(lines: list[str], match: str, line_rstrip=None, offset: int = 0) -> str:
    for line in lines[offset:]:
        if line_rstrip:
            line = line.rstrip()
        if match in line:
            return line
    raise GrabError(f"Could not find {match} in lines")


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


def num_to_alpha(num: int) -> str:
    return str(chr(65 + num))


def warning(plugin: str, error: str):
    print(f"{plugin} (warning): {error}")


class Parameter:
    """A parameter represents a VST parameter, a variable that can change how the plugin does the processing"""

    variable: str
    """A variable name, usually a capital letter"""
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
    display: str
    """The parameter display is an expression that usually maps the internal parameter value
    to a value to show the user"""

    _source_code: list[str]
    """The .cpp source code to parse for info about parameters"""

    def __init__(self, variable: str, source_dir: Path):
        self._source_code = self._get_source(source_dir)
        self._plugin = os.path.basename(source_dir)
        self.variable = variable
        self.enum_name = f"kParam{variable}"
        self.title = self._title()
        self.slug = self._slug()
        self.default_value = self._default_value()
        self.label = self._label()
        self.display = self._display()

    @staticmethod
    def _get_source(source_dir):
        name = os.path.basename(source_dir)
        filepath = Path(os.path.join(source_dir, f"{name}.cpp"))
        with open(filepath, "r") as f:
            lines = f.readlines()
        return lines

    def _title(self):
        fn_index = first_matching_line_index(self._source_code, "::getParameterName")
        param_index = next_matching_line_index(self._source_code, fn_index, self.enum_name)
        line = self._source_code[param_index]
        tokens = line.split("\"")
        if len(tokens) != 3:
            warning(self._plugin, "Could not find parameter name; using a default instead")
            return f"{self.variable}"
        else:
            return tokens[1]

    def _slug(self):
        to_strip = {"-", "/", ""}
        slug = self.title.lower()
        for elem in to_strip:
            slug.replace(elem, "")
        return slug

    def _default_value(self):
        fn_index = first_matching_line_index(self._source_code, "AudioEffectX(")
        line = matching_line(self._source_code, f"{self.variable} = ", offset=fn_index + 1)
        token = line.split("= ")[1]  # we know there is "= " in the string already
        token = token.split(";")[0]
        try:
            val = float(token)
            return val
        except ValueError as e:
            warning(self._plugin, f"Could not find default float value in `{line.strip()}` -- using 0.0")
            return 0.0

    def _label(self):
        fn_index = first_matching_line_index(self._source_code, "::getParameterLabel")
        line = matching_line(self._source_code, self.enum_name, offset=fn_index + 1)
        try:
            label = line.split("\"")[1]
            return label
        except IndexError as e:
            warning(self._plugin, f"Could not find parameter label in `{line.strip()}` -- using empty string")
            return ""

    def _display(self):
        fn_index = first_matching_line_index(self._source_code, "::getParameterDisplay")
        line = matching_line(self._source_code, self.enum_name, offset=fn_index + 1)
        if self.variable not in line:
            return self.variable
        try:
            # this is pretty gross, but good enough for now
            sp = line.split("(")[1]
            if len(sp) > 1:
                return sp.split(", text")[0]
            elif "text" in line:
                temp = line.split("(")[-1].split(", text")[0]
                if ")" in temp and "(" not in temp:
                    return f"({temp}"
                else:
                    return temp
        except IndexError as e:
            warning(self._plugin, f"Could not get display in `{line.strip}` -- using {self.variable}")
            return self.variable


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

    def __init__(self, project_root: Path, title: str, category: str):
        self.project_root = project_root
        self.title = title.replace(" ", "")
        self.slug = self.title.lower()
        self.category = category
        self.short_description, self.long_description = self._init_descriptions()

        self.source_dir = Path(os.path.join(project_root, "airwindows", self.title))
        self.output_path = Path(os.path.join(project_root, "include", self.category, self.slug + ".hpp"))

        self.initialization_code = self._init_initialization_code()
        self.processing_code = self._init_processing_code()
        self.private_vars = self._init_private_vars()
        self.parameters = self._init_parameters()

    def _init_initialization_code(self):
        filepath = Path(os.path.join(self.source_dir, f"{self.title}.cpp"))
        with open(filepath) as f:
            lines = f.readlines()

        begin_index = first_matching_line_index(lines, "AudioEffectX(") + 1
        end_index = first_matching_line_index(lines, "_canDo.insert(")
        chunk = lines[begin_index:end_index]
        chunk = naive_line_filter(chunk)
        return "".join(chunk)

    def _init_processing_code(self):
        filepath = Path(os.path.join(self.source_dir, f"{self.title}Proc.cpp"))
        with open(filepath, "r") as f:
            lines = f.readlines()

        # skip input and output pointers
        begin_index = first_matching_line_index(lines, "processDoubleReplacing(") + 6
        chunk = lines[begin_index:]
        chunk = naive_line_filter(chunk)
        result = []
        for line in chunk:
            result.append(line.replace("getSampleRate()", "Effect<T>::getSampleRate()"))
        return "".join(result)

    def _init_private_vars(self):
        filepath = Path(os.path.join(self.source_dir, f"{self.title}.h"))
        with open(filepath, "r") as f:
            lines = f.readlines()

        begin_index = first_matching_line_index(lines, "private:") + 1
        end_index = first_matching_line_index(lines[begin_index:], "};", line_rstrip=True) + begin_index
        chunk = lines[begin_index:end_index]
        chunk = naive_line_filter(chunk)
        return "".join(chunk)

    def _init_parameters(self):
        filepath = Path(os.path.join(self.source_dir, f"{self.title}.h"))
        with open(filepath, "r") as f:
            lines = f.readlines()

        begin_index = first_matching_line_index(lines, "kNumParameters")
        num_parameters = int(lines[begin_index].split("kNumParameters = ")[1])
        result = dict()
        for i in range(num_parameters):
            variable = num_to_alpha(i)
            result[variable] = Parameter(variable, self.source_dir)
        return result

    def _init_descriptions(self):
        filepath = Path(os.path.join(self.project_root, "scripts", "res", "airwindopedia.txt"))
        with open(filepath, "r") as f:
            lines = f.readlines()

        try:
            begin_index = first_matching_line_index(lines, f"# {self.title}")
            next_index = next_matching_line_index(lines, begin_index + 1, "# ")
            short_description = lines[begin_index].replace("#", "").strip()

            chunk = lines[begin_index + 1:next_index]
            chunk = [line.replace("\n", "") for line in chunk]
            long_description = "".join(naive_line_filter(chunk))
            return short_description, long_description
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
            line = f"case {param.enum_name}: {param.variable} = value; break;\n"
            lines.append(line)
        return "".join(lines)

    def _param_get_value_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return {param.variable}; break;\n"
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
            line = f"case {param.enum_name}: return \"{param.slug}\"; break;\n"
            lines.append(line)
        return "".join(lines)

    def _param_title_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return \"{param.title}\"; break;\n"
            lines.append(line)
        return "".join(lines)

    def _parameter_display_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return std::to_string({param.variable}); break;\n"
            lines.append(line)
        return "".join(lines)

    def _parameter_label_switch(self) -> str:
        lines = []
        for index, param in self.parameters.items():
            line = f"case {param.enum_name}: return \"{param.label}\"; break;\n"
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
    _airwindopedia_path: Path
    _template_header_path: Path
    _categories: dict[str, str]
    _categories_transpose: dict[str, list[str]]

    def __init__(self, project_root: Path):
        self._project_root = project_root
        airwindows_root = os.path.join(project_root, "airwindows")
        if not os.path.exists(airwindows_root) or not os.path.exists(os.path.join(airwindows_root, "Aura")):
            raise FileNotFoundError("Please add a copy of the airwindows source code to the root of this repo with "
                                    "the plugin source code as subdirectories")

        self._airwindopedia_path = Path(os.path.join(self._project_root, "scripts", "res", "airwindopedia.txt"))
        self._template_header_path = Path(os.path.join(self._project_root, "scripts", "res", "template.hpp"))
        self._categories, self._categories_transpose = self._init_categories()
        self._init_plugins()

    def _init_categories(self) -> (dict[str, list[str]], dict[str, str]):
        with open(self._airwindopedia_path, "r") as f:
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
                    category, elements = self._split_category(stripped)
                    for element in elements:
                        categories[element] = category
                    if category in categories_transpose.keys():
                        categories_transpose[category] += elements
                    else:
                        categories_transpose[category] = elements
            if line.startswith("# Categories"):
                in_categories = True
        return categories, categories_transpose

    @staticmethod
    def _split_category(s: str):
        category = s.split(":")[0].strip().lower().replace(" ", "-")
        elements = s.split(":")[1].split(",")
        for i in range(len(elements)):
            new_elem = elements[i].strip()
            elements[i] = new_elem
        return category, elements

    def _init_plugins(self):
        for plugin, category in self._categories.items():
            try:
                print(f"Writing {plugin} =====")
                plug = Plugin(self._project_root, plugin, category)
                plug.write(self._project_root, self._template_header_path)
            except GrabError as e:
                print(f">>> Failure... {e}\n")


def main(root_dir):
    grabber = Grabber(Path(root_dir))


if __name__ == "__main__":
    path_to_airwindohhs = "/Users/alexvangils/repos/aria/airwindohhs"
    main(path_to_airwindohhs)
