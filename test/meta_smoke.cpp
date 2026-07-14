// Compiles, links, and runs a couple of plugins through PluginHost to prove
// the generated factory wiring and virtual dispatch actually work, not just
// parse. scripts/check_compile.py's checks are syntax-only per header and
// never exercise cross-plugin runtime dispatch.
#include "meta.hpp"

#include <cassert>
#include <cstdio>
#include <string>

template <typename T>
void run_smoke_test()
{
    airwindohhs::meta::PluginHost<T> host;
    const auto& plugins = host.all_plugins();
    assert(!plugins.empty());

    bool ok = host.load_by_index(0);
    assert(ok);
    assert(host.current() != nullptr);

    host.setSampleRate(static_cast<T>(44100.0));
    assert(host.getSampleRate() == static_cast<T>(44100.0));

    T inL[8]{};
    T inR[8]{};
    T outL[8]{};
    T outR[8]{};
    T* ins[2]{ inL, inR };
    T* outs[2]{ outL, outR };
    host.process(ins, outs, 8);

    // Sample rate must survive a swap to a different plugin.
    std::string last_name(plugins.back().name);
    ok = host.load_by_name(last_name);
    assert(ok);
    assert(host.getSampleRate() == static_cast<T>(44100.0));
    host.process(ins, outs, 8);

    ok = host.load_by_name("Definitely Not A Real Plugin Name");
    assert(!ok);

    ok = host.load_by_index(plugins.size());
    assert(!ok);

    std::printf("PluginHost<%s>: %zu plugins, ran '%.*s' and '%s' ok\n",
                std::is_same<T, float>::value ? "float" : "double", plugins.size(),
                static_cast<int>(plugins.front().name.size()), plugins.front().name.data(), last_name.c_str());
}

int main()
{
    run_smoke_test<float>();
    run_smoke_test<double>();
    return 0;
}
