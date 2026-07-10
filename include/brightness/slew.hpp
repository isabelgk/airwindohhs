#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::slew {

constexpr std::string_view k_name{ "Slew" };
constexpr std::string_view k_short_description{
    "Slew is a slew clipper, which darkens treble in an unusual way."
};
constexpr std::string_view k_long_description{
    "Slew is the very first plugin I made for Airwindows (or at least the first one listed on this website).Slew is a simple plugin to let you do slew clipping. You can use it to morph and transform percussive sounds or hats, you can give a unique and interesting ‘grind’ to instruments or glue stuff together into a retro, old-school-sampler, grungey grind, or you can use it on things like reverb sends to really amp up the sense of distant loud sound in a room or space."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Slew final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'slew';
    float gain;
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    Slew()
    {
        gain = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        // TODO: uncomment canDo entries according to your plugin's capabilities
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
            case kParamA: gain = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return gain; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "clamping"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Clamping"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(gain); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double inputSampleL;
        double inputSampleR;
        double outputSampleL;
        double outputSampleR;
        double clamp;
        double threshold = pow((1 - gain), 4) / overallscale;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            clamp = inputSampleL - lastSampleL;
            outputSampleL = inputSampleL;
            if (clamp > threshold) {
                outputSampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                outputSampleL = lastSampleL - threshold;
            }
            lastSampleL = outputSampleL;
            clamp = inputSampleR - lastSampleR;
            outputSampleR = inputSampleR;
            if (clamp > threshold) {
                outputSampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                outputSampleR = lastSampleR - threshold;
            }
            lastSampleR = outputSampleR;
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::slew
