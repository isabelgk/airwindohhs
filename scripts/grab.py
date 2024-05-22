import argparse
import os.path
from pathlib import Path


def _arg_parser():
    parser = argparse.ArgumentParser(description="Grab an airwindows plugin source")
    parser.add_argument("srcdir", type=str, help="The path to the directory containing the main airwindows plugin code")
    parser.add_argument("category", type=str, help="The plugin category")
    parser.add_argument("dstdir", type=str, help="The path to the include/ directory")
    parser.add_argument("template_header", type=str, help="The header template path")
    args = parser.parse_args()
    return args


def _check_args(args):
    if args["srcdir"]:
        srcdir = Path(args["srcdir"])
        if not srcdir.is_dir():
            raise RuntimeError("srcdir must exist")
    else:
        raise RuntimeError("srcdir must exist")

    dstdir = Path(args["dstdir"])
    if not os.path.exists(dstdir):
        os.mkdir(dstdir)

    category_dir = Path(os.path.join(dstdir, args["category"]))
    if category_dir.exists():
        output_filepath = Path(os.path.join(category_dir, os.path.basename(srcdir).lower() + ".hpp"))
        if output_filepath.exists():
            pass
        else:
            output_filepath.touch()
    else:
        os.mkdir(category_dir)

    return args


def _split_category(s: str):
    category = s.split(":")[0].strip().lower().replace(" ", "-")
    elements = s.split(":")[1].split(",")
    for i in range(len(elements)):
        new_elem = elements[i].strip()
        elements[i] = new_elem
    return category, elements


def get_category_lines(filepath: Path):
    ls = []
    with open(filepath, "r") as f:
        in_categories = False
        for line in f.readlines():
            if line.startswith("#####"):
                in_categories = False
            if in_categories:
                stripped = line.strip()
                if stripped != "":
                    ls.append(_split_category(stripped))
            if line.startswith("# Categories"):
                in_categories = True
    return ls


def _filter_naive(lines):
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


def _get_name_from_srcpath(srcdir: Path) -> str:
    return os.path.basename(srcdir).lower()


def _get_path_from_srcpath(srcdir: Path, postfix: str) -> Path:
    name = _get_name_from_srcpath(srcdir) + postfix
    return Path(os.path.join(srcdir, name))


def _get_proc(srcdir: Path):
    path = _get_path_from_srcpath(srcdir, "Proc.cpp")
    with open(path, "r") as f:
        lines = f.readlines()

    count = 0
    start_index = 0
    for line in lines:
        count += 1
        if "processDoubleReplacing(" in line:
            start_index = count + 5  # skipping the input and output part
    return _filter_naive(lines[start_index:-1].copy())


def _get_private_vars(srcdir: Path):
    result = []
    fname = _get_path_from_srcpath(srcdir, ".h")
    within = False

    with open(fname) as f:
        lines = f.readlines()

    for line in lines:
        if "private:" in line:
            within = True
        if "};" == line.rstrip():
            within = False

        if within:
            result.append(line)

    return _filter_naive(result)


def _get_initialization(srcdir: Path):
    fname = _get_path_from_srcpath(srcdir, ".cpp")
    with open(fname) as f:
        lines = f.readlines()

    start_index = 0
    end_index = 0
    count = 0
    for line in lines:
        count += 1
        if "AudioEffectX(" in line:
            start_index = count + 2
        if "_canDo.insert(" in line:
            end_index = count - 1
            break

    return _filter_naive(lines[start_index - 2:end_index])


def _get_num_params(srcdir: Path):
    fname = _get_path_from_srcpath(srcdir, ".h")
    num_params = 0
    with open(fname) as f:
        lines = f.readlines()
        for line in lines:
            if "kNumParameters" in line:
                num_params = int(line.split(" ")[-1])

    return num_params


def _get_chunk(srcdir: Path, src_postfix: str, num_params: int, match: str):
    fname = _get_path_from_srcpath(srcdir, src_postfix)
    chunk = []
    with open(fname) as f:
        lines = f.readlines()
        for index, line in enumerate(lines):
            if match in line:
                i = index + 2
                chunk = lines[i:i + num_params]
    return chunk


def _get_default_param_values(srcdir: Path, num_params: int):
    chunk = _get_chunk(srcdir, ".cpp", num_params, "AudioEffectX(")
    result = []
    for line in chunk:
        initializer = line.split("=")
        var = initializer[0].strip()
        val = initializer[1].split(";")[0].strip()
        result.append({"variable": var, "initial_val": val, "enum_str": f"kParam{var}"})
    return result


def _get_param_names(srcdir: Path, num_params: int):
    chunk = _get_chunk(srcdir, ".cpp", num_params, "::getParameterName(")
    return [x.split("\"")[1] for x in chunk]


def _get_param_labels(srcdir: Path, num_params: int):
    chunk = _get_chunk(srcdir, ".cpp", num_params, "::getParameterLabel(")
    return [x.split("\"")[1] for x in chunk]


def _get_param_displays(srcdir: Path, num_params: int):
    chunk = _get_chunk(srcdir, ".cpp", num_params, "::getParameterDisplay(")
    result = []
    for line in chunk:
        sp = line.split("(")[1]
        if len(sp) > 1:
            result.append(sp.split(", text")[0])
        elif "text" in line:
            temp = line.split("(")[-1].split(", text")[0]
            if ")" in temp and "(" not in temp:
                result.append(f"({temp}")
            else:
                result.append(temp)
    return result


def _get_param(srcdir: Path):
    num_params = _get_num_params(srcdir)
    default_info = _get_default_param_values(srcdir, num_params)
    param_names = _get_param_names(srcdir, num_params)
    param_labels = _get_param_labels(srcdir, num_params)
    param_display = _get_param_displays(srcdir, num_params)

    result = {
        "enum": [],
        "set_switch": [],
        "get_switch": [],
        "name_switch": [],
        "label_switch": [],
        "display_switch": [],
    }

    for i in range(num_params):
        enum_str = default_info[i]["enum_str"]
        variable = default_info[i]["variable"]

        set_text = f"case {enum_str}: {variable} = value; break;\n"
        result["set_switch"].append(set_text)

        get_text = f"case {enum_str}: return {variable};\n"
        result["get_switch"].append(get_text)

        name_text = f"case {enum_str}: return \"{param_names[i]}\";\n"
        result["name_switch"].append(name_text)

        label_text = f"case {enum_str}: return \"{param_labels[i]}\";\n"
        result["label_switch"].append(label_text)

        display_text = f"case {enum_str}: return std::to_string({param_display[i]});\n"
        result["display_switch"].append(display_text)

        enum_text = f"{enum_str} = {i},\n"
        result["enum"].append(enum_text)

    result["enum"].append(f"kNumParameters = {num_params}\n")

    return result


def _parse_sections(srcdir: Path):
    return {
        "params": _get_param(srcdir),
        "init": _get_initialization(srcdir),
        "proc": _get_proc(srcdir),
        "private_vars": _get_private_vars(srcdir),
    }


def main(srcdir: Path, category: str, dstdir: Path, template_header: Path):
    args = {
        "srcdir": srcdir,
        "category": category,
        "dstdir": dstdir,
        "template_header": template_header
    }

    args = _check_args(args)
    parsed = _parse_sections(args["srcdir"])
    name = os.path.basename(args["srcdir"])
    outpath = os.path.join(args["dstdir"], args["category"], str(name).lower() + ".hpp")

    changed = []
    with open(args["template_header"], "r") as f:
        lines = f.readlines()
        for line in lines:
            if "%%CLASSNAME%%" in line:
                changed.append(line.replace("%%CLASSNAME%%", name))
            elif "%%PRIVATEVARS%%" in line:
                changed.append(line.replace("%%PRIVATEVARS%%", "".join(parsed["private_vars"])))
            elif "%%INITIALIZATION%%" in line:
                changed.append(line.replace("%%INITIALIZATION%%", "".join(parsed["init"])))
            elif "%%PARAM_ENUM%%" in line:
                changed.append(line.replace("%%PARAM_ENUM%%", "".join(parsed["params"]["enum"])))
            elif "%%SETPARAMSWITCH%%" in line:
                changed.append(line.replace("%%SETPARAMSWITCH%%", "".join(parsed["params"]["set_switch"])))
            elif "%%GETPARAMSWITCH%%" in line:
                changed.append(line.replace("%%GETPARAMSWITCH%%", "".join(parsed["params"]["get_switch"])))
            elif "%%GETPARAMLABELDISPLAY%%" in line:
                changed.append(line.replace("%%GETPARAMLABELDISPLAY%%", "".join(parsed["params"]["display_switch"])))
            elif "%%GETPARAMLABELSWITCH%%" in line:
                changed.append(line.replace("%%GETPARAMLABELSWITCH%%", "".join(parsed["params"]["label_switch"])))
            elif "%%GETPARAMNAMESWITCH%%" in line:
                changed.append(line.replace("%%GETPARAMNAMESWITCH%%", "".join(parsed["params"]["name_switch"])))
            elif "%%PROCESS%%" in line:
                changed.append(line.replace("%%PROCESS%%", "".join(parsed["proc"])))
            else:
                changed.append(line)

    with open(outpath, "w") as o:
        for line in changed:
            o.write(line)


if __name__ == "__main__":
    args = _arg_parser()

    srcdir = args["srcdir"]
    category = args["category"]
    dstdir = args["dstdir"]
    template_file = args["template_header"]

    try:
        main(srcdir, category, dstdir, template_file)
    except Exception as e:
        print(f"Exception: {e}")
