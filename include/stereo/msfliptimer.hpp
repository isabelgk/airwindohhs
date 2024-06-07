#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::msfliptimer {

constexpr std::string_view k_name{ "MSFlipTimer" };
constexpr std::string_view k_short_description{
    "MSFlipTimer is a utility that swaps stereo with mono every few (1-10) minutes."
};
constexpr std::string_view k_long_description{
    "Here's a request I got, a variation on one that's in the utility category. While I ramp up to more interesting stuff I can do some of the background work while putting out the thing that someone asked me to make :)Every few minutes (as in, one minute to ten minutes) MSFlipTimer switches from stereo to mono. It does this in about a tenth of a second, to prevent any sort of pop or anything. When it’s in stereo it is 100% direct pass-through of the audio data word, so this is as clean as stuff like LeftToMono: it’s one of those ones that just copies the data over, not even touching it. That said, this doesn’t belong in your mix: the idea is that if you’re mixing and you tend to screw up the mono mix by doing too much crazy stuff with stereo, this'll repeatedly force you to grapple with it in its mono form."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class MSFlipTimer final : public Effect<T>
{
    uint32_t tick;
    float A;

  public:
    MSFlipTimer()
    {
        A = 0.0;
        tick = 0;
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
        kParamA:
            A = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "minutes";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Minutes";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;

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
                    // 0 to 1 from beginning mono to end as direct stereo out
                    double mono = (inputSampleL + inputSampleR) * 0.5;
                    if (crossfade < 0.5) {
                        outputSampleL = (inputSampleL * crossfade) + (mono * (1.0 - crossfade));
                        outputSampleR = (inputSampleR * crossfade) + (mono * (1.0 - crossfade));
                    }
                    else {
                        crossfade = fabs(crossfade - 1.0);
                        outputSampleL = (mono * crossfade) + (inputSampleL * (1.0 - crossfade));
                        outputSampleR = (mono * crossfade) + (inputSampleR * (1.0 - crossfade));
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
                    // 0 to 1 from beginning direct to end as mono
                    double mono = (inputSampleL + inputSampleR) * 0.5;
                    if (crossfade < 0.5) {
                        outputSampleL = (mono * crossfade) + (inputSampleL * (1.0 - crossfade));
                        outputSampleR = (mono * crossfade) + (inputSampleR * (1.0 - crossfade));
                    }
                    else {
                        crossfade = fabs(crossfade - 1.0);
                        outputSampleL = (inputSampleL * crossfade) + (mono * (1.0 - crossfade));
                        outputSampleR = (inputSampleR * crossfade) + (mono * (1.0 - crossfade));
                    }
                }
                else {
                    double mono = (inputSampleL + inputSampleR) * 0.5;
                    outputSampleL = outputSampleR = mono;
                    // mono out
                }
            }
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::msfliptimer
