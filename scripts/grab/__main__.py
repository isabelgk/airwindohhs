import argparse
from pathlib import Path

from scripts.grab.grabber import Grabber, find_missing_plugins, parse_categories
from scripts.grab.source_fetch import UpstreamSource, VersionPin, read_version_pin, write_version_pin


def parse_args(argv=None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="grab",
        description="Generate include/ headers from the upstream airwindows VST source.",
    )
    parser.add_argument(
        "--project-root", type=Path, default=Path.cwd(),
        help="Root of this repo (default: current directory).",
    )
    parser.add_argument(
        "--airwindows-ref", default=None,
        help="Upstream airwindows commit SHA to fetch (default: the ref pinned in airwindows-version.txt).",
    )
    parser.add_argument(
        "--airwindows-src", type=Path, default=None,
        help="Use a local airwindows checkout instead of cloning upstream -- the root of "
             "a `git clone https://github.com/airwindows/airwindows` (containing "
             "Airwindopedia.txt and plugins/LinuxVST/src/).",
    )
    parser.add_argument(
        "--update-pin", action="store_true",
        help="Rewrite airwindows-version.txt to the fetched ref after a fully successful run. "
             "Only valid together with --airwindows-ref.",
    )
    parser.add_argument(
        "--plugin", action="append", default=None,
        help="Only generate this plugin (repeatable).",
    )
    parser.add_argument(
        "--category", action="append", default=None,
        help="Only generate plugins in this category (repeatable).",
    )
    parser.add_argument(
        "--keep-temp", action="store_true",
        help="Don't delete the temporary clone directory after the run.",
    )
    parser.add_argument(
        "--list-missing", action="store_true",
        help="List plugin titles (from Airwindopedia.txt) that don't have a generated header "
             "yet, one per line, and exit without generating anything.",
    )
    parser.add_argument(
        "--pin-only", action="store_true",
        help="Only write airwindows-version.txt (with --update-pin) and exit, without "
             "generating any plugins.",
    )
    args = parser.parse_args(argv)

    if args.update_pin and not args.airwindows_ref:
        parser.error("--update-pin requires --airwindows-ref")
    if args.airwindows_src and args.update_pin:
        parser.error("--update-pin has no effect with --airwindows-src (nothing was fetched to pin)")
    if args.pin_only and not args.update_pin:
        parser.error("--pin-only requires --update-pin")
    if args.list_missing and args.pin_only:
        parser.error("--list-missing and --pin-only can't be used together")
    if args.list_missing and args.plugin:
        parser.error("--list-missing can't be used with --plugin")
    if args.pin_only and (args.plugin or args.category):
        parser.error("--pin-only can't be used with --plugin or --category")

    return args


def resolve_source(args: argparse.Namespace, version_file: Path) -> tuple[UpstreamSource, VersionPin]:
    pin = read_version_pin(version_file)

    if args.airwindows_src:
        return UpstreamSource.from_local_path(args.airwindows_src), pin

    ref = args.airwindows_ref or pin.ref
    source = UpstreamSource.from_clone(pin.repo_url, ref, keep_temp=args.keep_temp)
    return source, VersionPin(repo_url=pin.repo_url, ref=ref)


def main(argv=None):
    args = parse_args(argv)
    project_root = args.project_root.resolve()
    version_file = project_root / "airwindows-version.txt"

    source, resolved_pin = resolve_source(args, version_file)
    try:
        if args.list_missing:
            categories, _ = parse_categories(source.airwindopedia_path)
            category_filter = set(args.category) if args.category else None
            if category_filter:
                categories = {p: c for p, c in categories.items() if c in category_filter}
            for title in find_missing_plugins(project_root, categories):
                print(title)
            return

        if args.pin_only:
            write_version_pin(version_file, resolved_pin)
            return

        plugin_filter = set(args.plugin) if args.plugin else None
        category_filter = set(args.category) if args.category else None
        Grabber(project_root, source.plugin_source_dir, source.airwindopedia_path, plugin_filter, category_filter)

        if args.update_pin:
            write_version_pin(version_file, resolved_pin)
    finally:
        source.cleanup()


if __name__ == "__main__":
    main()
