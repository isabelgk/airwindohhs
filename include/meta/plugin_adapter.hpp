#pragma once
#include <string>
#include <string_view>

#include "meta/iplugin.hpp"

namespace airwindohhs::meta {

// Wraps any generated plugin class (they're all structurally identical --
// same method names/signatures, just different bodies) so it can be reached
// through the IPlugin<T> interface. One generic template covers all 495
// plugins; only the tiny per-plugin factory functions in registry.hpp need
// to be generated.
template <typename T, template <typename> class PluginT>
class PluginAdapter final : public IPlugin<T>
{
    PluginT<T> impl_;
    std::string_view name_;
    std::string_view tags_;
    std::string_view short_description_;
    std::string_view long_description_;

  public:
    PluginAdapter(std::string_view name, std::string_view tags, std::string_view short_description,
                  std::string_view long_description)
        : name_(name), tags_(tags), short_description_(short_description), long_description_(long_description)
    {
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        impl_.process(inputs, outputs, sampleFrames);
    }

    void set_parameter_value(int index, float value) override
    {
        impl_.set_parameter_value(index, value);
    }

    float get_parameter_value(int index) override
    {
        return impl_.get_parameter_value(index);
    }

    T get_parameter_default(int index) override
    {
        return impl_.get_parameter_default(index);
    }

    int num_parameters() override
    {
        return static_cast<int>(PluginT<T>::kNumParameters);
    }

    std::string_view parameter_name(int index) override
    {
        return impl_.get_parameter_name(index);
    }

    std::string_view parameter_title(int index) override
    {
        return impl_.get_parameter_title(index);
    }

    std::string get_parameter_display(int index) override
    {
        return impl_.get_parameter_display(index);
    }

    std::string_view parameter_label(int index) override
    {
        return impl_.get_parameter_label(index);
    }

    void setSampleRate(T sampleRate) override
    {
        impl_.setSampleRate(sampleRate);
    }

    T getSampleRate() override
    {
        return impl_.getSampleRate();
    }

    std::string_view name() const override
    {
        return name_;
    }

    std::string_view tags() const override
    {
        return tags_;
    }

    std::string_view short_description() const override
    {
        return short_description_;
    }

    std::string_view long_description() const override
    {
        return long_description_;
    }
};

} // namespace airwindohhs::meta
