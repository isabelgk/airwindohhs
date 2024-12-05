#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dcvoltage {

constexpr std::string_view k_name{ "DCVoltage" };
constexpr std::string_view k_short_description{
    "DCVoltage is literally a DC control voltage, in a plugin."
};
constexpr std::string_view k_long_description{
    "This is exactly what it says on the tin. Do NOT just put this in a mix and crank it up to see what it’ll do. If your whole system is DC-coupled you will blow your woofers, just like that, after a big ‘whump’. I will not take responsibility for damage caused by misusing unusual tools.What SHOULD you do? Here are some ideas.There is no DC offset filter nearly as good as applying an opposite offset. Use metering, perhaps option/alt-dragging on the slider if your DAW permits it, to cancel out a DC offset without any sonic penalty at all. This is called a ‘DC servo’, but digitally. If you can get it perfect and then bounce files so you can work with a center corrected section (so you don’t have to get a pop from turning it on or off) this would be the highest quality way to get rid of a FIXED DC offset without altering any of the bass at all. It’ll retain right down to 0.001 hz or whatever, and only kill what is totally unvarying DC.If you have a converter that’s DC coupled, and analog modular synthesizers, you can use this to create and modulate control voltages. Use it as a voltage source and then mix stuff together using DAW routing much like you use patchcords on your synthesizers, and be careful not to route control voltages to your monitors! I know there are people who’ve done odd things to get DC voltages inside their DAWs. Now it’s a lot simpler :)This plugin may not be any use to you, and don’t play with it if you don’t know what it is. If it is useful to you, you already know exactly what you’ll do with it, so go right ahead, now you’ve got DAW control voltages out of a simple plugin."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class DCVoltage final : public Effect<T>
{
    float A;

  public:
    DCVoltage()
    {
        A = 0.5;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "voltage"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Voltage"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double voltage = (A * 2.0) - 1.0;
        while (--sampleFrames >= 0)
        {
            *out1 = *in1 + voltage;
            *out2 = *in2 + voltage;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::dcvoltage
