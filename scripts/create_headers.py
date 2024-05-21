import os.path
from pathlib import Path
from dotenv import load_dotenv


def _split_category(s: str):
    category = s.split(":")[0].strip().lower().replace(" ", "-")
    elements = s.split(":")[1].split(",")
    for i in range(len(elements)):
        new_elem = elements[i].strip().lower()
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


def make_headers(base: Path, category: str, elements):
    category_path = Path(os.path.join(base, category))
    if not category_path.exists():
        os.mkdir(category_path)

    for category in elements:
        header_path = Path(os.path.join(category_path, f"{category}.hpp"))
        if not header_path.exists():
            header_path.touch(mode=0o660, exist_ok=False)


if __name__ == "__main__":
    if not load_dotenv():
        raise RuntimeError("Please set up a .env file")
    info_path = os.environ["INFO_PATH"]
    include_path = os.environ["INCLUDE_PATH"]

    lines = get_category_lines(Path(info_path))
    for c, e in lines:
        make_headers(Path(include_path), c, e)
