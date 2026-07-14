#pragma once
#include <memory>
#include <string_view>

#include "meta/iplugin.hpp"

namespace airwindohhs::meta {

template <typename T>
struct PluginEntry
{
    std::string_view name;
    std::string_view slug;
    std::string_view category;
    std::string_view tags;
    std::string_view short_description;
    std::string_view long_description;
    int num_parameters;
    std::unique_ptr<IPlugin<T>> (*make)();
};

} // namespace airwindohhs::meta
