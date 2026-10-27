#!/bin/bash
# Formats the given files and directories (default: include/) with the
# clang-format version pinned in pyproject.toml.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
if [ $# -eq 0 ]; then
    set -- "$repo_root/include"
fi

find "$@" -type f \( -name "*.c" -o -name "*.cpp" -o -name "*.h" -o -name "*.hpp" \) -print0 \
    | xargs -0 uv run --project "$repo_root" clang-format -i
