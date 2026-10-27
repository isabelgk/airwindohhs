"""Compile every generated plugin header to catch broken templates.

Explicitly instantiates each plugin's class for float and double, since
templates only get real semantic checking of member-function bodies once
instantiated (a bare #include does not catch this).
"""

import argparse
import concurrent.futures
import subprocess
import sys
import tempfile
from pathlib import Path

from scripts.headers import INCLUDE_DIR, REPO_ROOT, find_headers, plugin_class


def check_header(header, cxx):
    extracted = plugin_class(header)
    if extracted is None:
        return header, False, "could not find 'namespace airwindohhs::X ... class Y final : public Effect<T>' pattern"

    namespace, class_name = extracted
    rel_include = header.relative_to(INCLUDE_DIR).as_posix()

    source = (
        f'#include "{rel_include}"\n'
        f"template class airwindohhs::{namespace}::{class_name}<float>;\n"
        f"template class airwindohhs::{namespace}::{class_name}<double>;\n"
    )

    with tempfile.NamedTemporaryFile(suffix=".cpp", mode="w", delete=False) as tmp:
        tmp.write(source)
        tmp_path = tmp.name

    try:
        result = subprocess.run(
            [cxx, "-std=c++17", "-fsyntax-only", "-I", str(INCLUDE_DIR), tmp_path],
            capture_output=True,
            text=True,
        )
    finally:
        Path(tmp_path).unlink(missing_ok=True)

    return header, result.returncode == 0, result.stderr


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "paths",
        nargs="*",
        help="specific header files to check (default: all of include/**/*.hpp)",
    )
    parser.add_argument(
        "--cxx", default="c++", help="C++ compiler to invoke (default: c++)"
    )
    parser.add_argument(
        "--jobs", type=int, default=None, help="parallel compiles (default: CPU count)"
    )
    args = parser.parse_args()

    headers = [Path(p).resolve() for p in args.paths] if args.paths else find_headers()
    if not headers:
        print("no headers found to check", file=sys.stderr)
        return 1

    failures = []
    unmatched = []

    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        futures = [pool.submit(check_header, h, args.cxx) for h in headers]
        for future in concurrent.futures.as_completed(futures):
            header, ok, message = future.result()
            if "could not find" in message:
                unmatched.append((header, message))
            elif not ok:
                failures.append((header, message))

    total = len(headers)
    checked = total - len(unmatched)
    print(f"{checked}/{total} headers checked, {len(failures)} failed, {len(unmatched)} unparsable")

    if unmatched:
        print("\nheaders that could not be parsed for a class to instantiate "
              "(never actually compiled -- fix the regex or the header):", file=sys.stderr)
        for header, message in sorted(unmatched):
            print(f"  {header.relative_to(REPO_ROOT)}: {message}", file=sys.stderr)

    if failures:
        print("\nheaders that failed to compile:", file=sys.stderr)
        for header, stderr in sorted(failures):
            print(f"\n=== {header.relative_to(REPO_ROOT)} ===", file=sys.stderr)
            print(stderr, file=sys.stderr)

    return 1 if (failures or unmatched) else 0


if __name__ == "__main__":
    sys.exit(main())
