#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::lrfliptimer {

constexpr std::string_view k_name{ "LRFlipTimer" };
constexpr std::string_view k_short_description{
    "LRFlipTimer is a utility that swaps Left with Right every few (1-10) minutes."
};
constexpr std::string_view k_long_description{
    "I don’t know how useful this’ll be for you: a person asked me for it, and I was able to do it. This just does one thing.Every few minutes (as in, one minute to ten minutes) it swaps the L and R channels. It sweeps them across in about a tenth of a second, to prevent any sort of pop or anything. When it’s in LR or RL mode, it is 100% direct pass-through of the audio data word, so this is as clean as stuff like LeftToMono: it’s one of those ones that just copies the data over, not even touching it. That said, this doesn’t belong in your mix: the idea is that if you’re mixing and you do stuff asymmetrically, it’s like visual arts: you might need to flip the canvas left-to-right to see if things are off balance. That’s all this plugin does. You set how many minutes will elapse before it flips or re-flips."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class LRFlipTimer final : public Effect<T>
{
    uint32_t tick;
    float A;

  public:
    LRFlipTimer()
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

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "minutes";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
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
} // namespace airwindohhs::lrfliptimer
