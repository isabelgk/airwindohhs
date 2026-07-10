# airwindohhs

[Airwindows](https://github.com/airwindows/airwindows) VST plugins modified as headers you can include in any C++ project.

## Build

Consume via CMake's FetchContent:

```cmake
include(FetchContent)
FetchContent_Declare(
    airwindohhs
    GIT_REPOSITORY https://github.com/isabelgk/airwindohhs.git
    GIT_TAG main
)
FetchContent_MakeAvailable(airwindohhs)

target_link_libraries(your_target PRIVATE Airwindohhs::Airwindohhs)
```

`Airwindohhs` is a header-only `INTERFACE` library requiring C++17, propagated
automatically to consumers. `version.txt` at the repo root pins the upstream
`airwindows/airwindows` commit the headers were generated from -- it is
unrelated to this project's own version.

## Usage

Every plugin is a header under `include/<category>/<plugin>.hpp`, in namespace
`airwindohhs::<plugin>`, categorized to match
[Airwindopedia](https://github.com/airwindows/airwindows)'s grouping (dynamics,
reverb, tape, etc).

```cpp
#include "dynamics/buttercomp.hpp"

airwindohhs::buttercomp::ButterComp<float> comp;
comp.setSampleRate(44100.0f);
comp.set_parameter_value(airwindohhs::buttercomp::ButterComp<float>::kParamA, 0.5f);
comp.process(inputs, outputs, sampleFrames);
```

## Generating headers

Everything under `include/` is generated from the upstream Airwindows VST
source by `scripts/grab`, a small [uv](https://docs.astral.sh/uv/)-managed CLI.
It parses each plugin's `.cpp`/`.h` source with libclang and fills in
`scripts/res/template.hpp` per plugin.

```
uv run grab
```

By default this clones the upstream `airwindows/airwindows` repo (shallow,
sparse-checked-out to just the plugin sources and `Airwindopedia.txt`) at the
commit pinned in `version.txt`, and regenerates every header under `include/`.

Useful flags:

- `--plugin NAME` / `--category NAME` -- only regenerate specific plugins or
  categories (repeatable).
- `--airwindows-src PATH` -- skip cloning and read from a local airwindows
  checkout instead -- the root of a `git clone
  https://github.com/airwindows/airwindows` (containing `Airwindopedia.txt`
  and `plugins/LinuxVST/src/`). The fastest way to iterate locally.
- `--airwindows-ref SHA` -- fetch a different upstream commit than the one
  pinned in `version.txt`.
- `--update-pin` -- rewrite `version.txt` to the fetched ref after a successful
  run (only valid with `--airwindows-ref`).
- `--keep-temp` -- don't delete the temporary clone directory afterward.

Run `uv run grab --help` for the full list.

Requires a `clang`/`clang++` on `PATH` -- the generator shells out to it once
per run to locate system header search paths for libclang, which ships without
any headers of its own.
