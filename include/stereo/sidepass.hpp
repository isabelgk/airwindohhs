#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sidepass {

constexpr std::string_view k_name{ "Sidepass" };
constexpr std::string_view k_short_description{
    "Sidepass is a simple utility plugin, a highpass on the side channel. Mono-maker."
};
constexpr std::string_view k_long_description{
    "This one was by request. It’s not very fancy, but I hope it’ll come in handy. If it is, spread the word!I’ve got a plugin called ToVinyl, which is rather fancy. It’s the Airwindows attempt at a vinyl mastering suite: elliptical EQ, highpass on the mid to help get levels, an acceleration limiter to tame sudden energy spikes without really cutting brightness otherwise, and even a groove wear modeler that was the precursor to Airwindows Aura. That’s one of the Kagi for-pay plugins, number 9 on the list when I reach $800 a month and begin doing those.But, I got asked for something much simpler. A highpass filter on just the side channel, and one that would go up the full range from subs to highs, so at full crank it’d be purely mono.Well, I’ve altered that a teeny bit: Sidepass is aware of sample rates, so if you’re at a 0.1 setting at 44.1K, it ought to be doing the same thing at 0.1 at 96K. But apart from that, here’s a little utility for AU, Mac and PC VST that just highpasses the side, from DC to 22K. It’s an Airwindows filter, same as what’s in Density and Drive as a highpass, and it ought to do nicely."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class Sidepass final : public Effect<T>
{
    double iirSampleA;
    double iirSampleB;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Sidepass()
    {
        A = 0.0;
        iirSampleA = 0.0;
        iirSampleB = 0.0;
        flip = true;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
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
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "cutoff"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Cutoff"; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double iirAmount = pow(A, 3) / overallscale;
        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
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
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            if (flip)
            {
                iirSampleA = (iirSampleA * (1 - iirAmount)) + (side * iirAmount);
                side -= iirSampleA;
            }
            else
            {
                iirSampleB = (iirSampleB * (1 - iirAmount)) + (side * iirAmount);
                side -= iirSampleB;
            }
            // highpass section
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            // end 64 bit stereo floating point dither
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::sidepass
