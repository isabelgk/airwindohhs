"""Locating generated plugin headers and the plugin class each one defines."""

import re
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
INCLUDE_DIR = REPO_ROOT / "include"
META_DIR = INCLUDE_DIR / "meta"

CLASS_RE = re.compile(
    r"namespace\s+airwindohhs::(\w+)"
    r".*?"
    r"template\s*<\s*typename\s+(\w+)\s*>\s*class\s+(\w+)\s+final\s*:\s*public\s+Effect<\2>",
    re.DOTALL,
)


def find_headers() -> list[Path]:
    """Every plugin header under include/, excluding effect.hpp and the meta registry."""
    return sorted(
        p
        for p in INCLUDE_DIR.rglob("*.hpp")
        if p.name != "effect.hpp" and p != INCLUDE_DIR / "meta.hpp" and META_DIR not in p.parents
    )


def plugin_class(header: Path) -> tuple[str, str] | None:
    """(namespace, class_name) of the plugin `header` defines, or None if CLASS_RE doesn't match."""
    match = CLASS_RE.search(header.read_text())
    if not match:
        return None
    namespace, _template_param, class_name = match.groups()
    return namespace, class_name
