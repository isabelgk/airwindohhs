import argparse
from pathlib import Path

from scripts.grab.grabber import Grabber
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
        help="Upstream airwindows commit SHA to fetch (default: the ref pinned in version.txt).",
    )
    parser.add_argument(
        "--airwindows-src", type=Path, default=None,
        help="Use a local directory of plugin sources instead of cloning upstream "
             "(a directory containing one subdirectory per plugin, e.g. `Aura/`).",
    )
    parser.add_argument(
        "--update-pin", action="store_true",
        help="Rewrite version.txt to the fetched ref after a fully successful run. "
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
    args = parser.parse_args(argv)

    if args.update_pin and not args.airwindows_ref:
        parser.error("--update-pin requires --airwindows-ref")
    if args.airwindows_src and args.update_pin:
        parser.error("--update-pin has no effect with --airwindows-src (nothing was fetched to pin)")

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
    version_file = project_root / "version.txt"

    source, resolved_pin = resolve_source(args, version_file)
    try:
        plugin_filter = set(args.plugin) if args.plugin else None
        category_filter = set(args.category) if args.category else None
        Grabber(project_root, source.airwindows_root, plugin_filter, category_filter)

        if args.update_pin:
            write_version_pin(version_file, resolved_pin)
    finally:
        source.cleanup()


if __name__ == "__main__":
    main()
