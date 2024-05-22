#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class LRFlipTimer final : public Effect<T>
{
    std::string m_name{ "LRFlipTimer" };

    uint32_t tick;
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    LRFlipTimer()
    {
        A = 0.0;
        tick = 0;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    constexpr std::string_view name()
    {
        return m_name;
    }

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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Minutes";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 9.0) + 1.0);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        uint32_t time = (uint32_t)(((A * 9.0) + 1.0) * 60.0 * getSampleRate());
        uint32_t transition = (uint32_t)(getSampleRate() * 0.1);
        // number of minutes * 60 seconds * samples per second
        // transition: tenth of a second crossfade
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            double outputSampleL = inputSampleL;
            double outputSampleR = inputSampleR;
            tick++;
            if (tick > time * 2) {
                tick = 0;
            }
            if (tick < time) {
                if (tick < transition) {
                    double crossfade;
                    crossfade = tick;
                    crossfade /= transition;
                    // 0 to 1 from beginning swapped to end as direct stereo out
                    if (crossfade < 0.5) {
                        outputSampleL = (inputSampleL * crossfade) + (inputSampleR * (1.0 - crossfade));
                        outputSampleR = (inputSampleR * crossfade) + (inputSampleL * (1.0 - crossfade));
                    }
                    else {
                        crossfade = fabs(crossfade - 1.0);
                        outputSampleL = (inputSampleR * crossfade) + (inputSampleL * (1.0 - crossfade));
                        outputSampleR = (inputSampleL * crossfade) + (inputSampleR * (1.0 - crossfade));
                    }
                }
                else {
                    outputSampleL = inputSampleL;
                    outputSampleR = inputSampleR;
                    // direct stereo out
                }
            }
            else {
                if ((tick - time) < transition) {
                    double crossfade;
                    crossfade = tick - time;
                    crossfade /= transition;
                    // 0 to 1 from beginning direct to end as swapped stereo
                    if (crossfade < 0.5) {
                        outputSampleL = (inputSampleR * crossfade) + (inputSampleL * (1.0 - crossfade));
                        outputSampleR = (inputSampleL * crossfade) + (inputSampleR * (1.0 - crossfade));
                    }
                    else {
                        crossfade = fabs(crossfade - 1.0);
                        outputSampleL = (inputSampleL * crossfade) + (inputSampleR * (1.0 - crossfade));
                        outputSampleR = (inputSampleR * crossfade) + (inputSampleL * (1.0 - crossfade));
                    }
                }
                else {
                    outputSampleL = inputSampleR;
                    outputSampleR = inputSampleL;
                    // swapped stereo out
                }
            }
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
