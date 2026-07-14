// Dumps full per-plugin metadata as JSON: everything scripts/gen_meta.py's
// static-analysis pass can't see, because parameter names/titles/labels and
// the k_*_description text only exist as real values once a plugin is actually
// instantiated through the registry. Downstream consumers can use this instead
// of re-deriving metadata from headers.
#include "meta.hpp"

#include <cstdio>
#include <string>
#include <string_view>

namespace {

void print_json_string(std::string_view s)
{
    std::putchar('"');
    for (unsigned char c : s) {
        switch (c) {
            case '"': std::fputs("\\\"", stdout); break;
            case '\\': std::fputs("\\\\", stdout); break;
            case '\n': std::fputs("\\n", stdout); break;
            case '\r': std::fputs("\\r", stdout); break;
            case '\t': std::fputs("\\t", stdout); break;
            default:
                if (c < 0x20) {
                    std::printf("\\u%04x", c);
                }
                else {
                    std::putchar(c);
                }
        }
    }
    std::putchar('"');
}

} // namespace

int main()
{
    const auto& plugins = airwindohhs::meta::all_plugins<double>();

    std::printf("[\n");
    for (std::size_t i = 0; i < plugins.size(); ++i) {
        const auto& entry = plugins[i];
        auto plugin = entry.make();

        std::printf("  {\n    \"name\": ");
        print_json_string(entry.name);
        std::printf(",\n    \"slug\": ");
        print_json_string(entry.slug);
        std::printf(",\n    \"category\": ");
        print_json_string(entry.category);
        std::printf(",\n    \"tags\": ");
        print_json_string(entry.tags);
        std::printf(",\n    \"short_description\": ");
        print_json_string(entry.short_description);
        std::printf(",\n    \"long_description\": ");
        print_json_string(entry.long_description);
        std::printf(",\n    \"num_parameters\": %d,\n    \"parameters\": [\n", entry.num_parameters);

        for (int p = 0; p < entry.num_parameters; ++p) {
            std::printf("      {\"name\": ");
            print_json_string(plugin->parameter_name(p));
            std::printf(", \"title\": ");
            print_json_string(plugin->parameter_title(p));
            std::printf(", \"label\": ");
            print_json_string(plugin->parameter_label(p));
            std::printf(", \"default\": %.17g}%s\n", plugin->get_parameter_default(p),
                        p + 1 < entry.num_parameters ? "," : "");
        }

        std::printf("    ]\n  }%s\n", i + 1 < plugins.size() ? "," : "");
    }
    std::printf("]\n");

    return 0;
}
