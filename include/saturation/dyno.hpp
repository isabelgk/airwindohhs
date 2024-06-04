#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dyno {

constexpr std::string_view k_name{ "Dyno" };
constexpr std::string_view k_short_description{
    "Dyno is a lot like Mojo, but for intensity instead of loudness."
};
constexpr std::string_view k_long_description{
    "Dyno is similar in some ways to Mojo, but Dyno has a completely different character. Instead of loudenating, it ‘intensenates’. It’s a little bit like Remap in that way, but it’s not the same as Remap. You won’t get a volume boost out of it really, nor will it let you slam it for fatter peaks.Instead, it brings fire and intensity to the audio and reshapes the waveform in a Mojo-like ‘evolution of Spiral’ way. Because it doesn’t take to slamming in the same way, it’s going to be a more subtle effect, but for those who got excited by Remap, this one deserves your attention."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Dyno final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Dyno()
    {
        A = 0.5;
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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24.0) - 12.0);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double gain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            double dyno = pow(fabs(inputSampleL), 4);
            if (dyno > 0.0) {
                inputSampleL = (sin(inputSampleL * dyno) / dyno) * 1.1654321;
            }
            // dyno is the one that tries to raise peak energy
            dyno = pow(fabs(inputSampleR), 4);
            if (dyno > 0.0) {
                inputSampleR = (sin(inputSampleR * dyno) / dyno) * 1.1654321;
            }
            // dyno is the one that tries to raise peak energy
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
} // namespace airwindohhs::dyno
