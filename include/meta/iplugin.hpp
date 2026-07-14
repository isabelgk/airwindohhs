#pragma once
#include <string>
#include <string_view>

namespace airwindohhs::meta {

template <typename T>
class IPlugin
{
  public:
    virtual ~IPlugin() = default;

    virtual void process(T** inputs, T** outputs, long sampleFrames) = 0;

    virtual void set_parameter_value(int index, float value) = 0;
    virtual float get_parameter_value(int index) = 0;
    virtual T get_parameter_default(int index) = 0;
    virtual int num_parameters() = 0;
    virtual std::string_view parameter_name(int index) = 0;
    virtual std::string_view parameter_title(int index) = 0;
    virtual std::string get_parameter_display(int index) = 0;
    virtual std::string_view parameter_label(int index) = 0;

    virtual void setSampleRate(T sampleRate) = 0;
    virtual T getSampleRate() = 0;

    // Self-description, captured at construction time -- never touches the
    // wrapped instance, so these are safe to call regardless of what's
    // currently loaded.
    virtual std::string_view name() const = 0;
    virtual std::string_view tags() const = 0;
    virtual std::string_view short_description() const = 0;
    virtual std::string_view long_description() const = 0;
};

} // namespace airwindohhs::meta
