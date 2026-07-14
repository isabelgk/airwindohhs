#pragma once

// Single include for programmatic discovery of and runtime dispatch across
// every plugin in this repo. Pulls in all 495 plugin headers, so expect a
// real compile-time cost -- this is an opt-in facade, not something to
// include from a translation unit that only needs one or two plugins
// directly.
#include "meta/host.hpp"
#include "meta/iplugin.hpp"
#include "meta/plugin_adapter.hpp"
#include "meta/plugin_entry.hpp"
#include "meta/registry.hpp"
