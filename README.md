# airwindohhs

[Airwindows](https://github.com/airwindows/airwindows) VST plugins modified as headers you can include in any C++ project.

## Generating headers

Everything under `include/` is generated from the upstream Airwindows VST
source by `scripts/grab`, a small [uv](https://docs.astral.sh/uv/)-managed CLI.
It parses each plugin's `.cpp`/`.h` source with libclang and fills in
`scripts/res/template.hpp` per plugin.

```
uv run grab
```

By default this clones the upstream `airwindows/airwindows` repo (shallow,
sparse-checked-out to just the plugin sources) at the commit pinned in
`version.txt`, and regenerates every header under `include/`.

Useful flags:

- `--plugin NAME` / `--category NAME` -- only regenerate specific plugins or
  categories (repeatable).
- `--airwindows-src PATH` -- skip cloning and read plugin sources from a local
  directory instead (a directory containing one subdirectory per plugin, e.g.
  `Aura/`). The fastest way to iterate locally.
- `--airwindows-ref SHA` -- fetch a different upstream commit than the one
  pinned in `version.txt`.
- `--update-pin` -- rewrite `version.txt` to the fetched ref after a successful
  run (only valid with `--airwindows-ref`).
- `--keep-temp` -- don't delete the temporary clone directory afterward.

Run `uv run grab --help` for the full list.

Requires a `clang`/`clang++` on `PATH` -- the generator shells out to it once
per run to locate system header search paths for libclang, which ships without
any headers of its own.
