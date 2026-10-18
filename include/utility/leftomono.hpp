#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::leftomono {

constexpr std::string_view k_name{ "LeftoMono" };
constexpr std::string_view k_short_description{
    "LeftoMono copies one channel to both, losslessly."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class LeftoMono final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'lfto';

  public:
    LeftoMono()
    {
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        while (--sampleFrames >= 0) {
            *out1 = *in1;
            *out2 = *in1;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::leftomono
