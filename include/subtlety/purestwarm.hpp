#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestwarm {

constexpr std::string_view k_name{ "PurestWarm" };
constexpr std::string_view k_short_description{
    "PurestWarm is a subtle tone shaper and warmth adder."
};
constexpr std::string_view k_long_description{
    "PurestWarm is a little bit like PurestDrive (anxiously awaited by quite a few people) but not quite as sophisticated… however, in its simplicity is part of its usefulness. It applies an Airwindows softest-possible-saturation effect (like what you can get out of Density, or Channel) but it’s polarized: you pick which side of the waveform the saturation applies to. For the other half of the waveform, it’ll pass through the exact bits that came in. So it’s half a plugin: I’m not sure there’s ever been anything quite like that, on one half of the wave a full Airwindows (noise shaped to floating point) ultra-quality saturation, and on the other half of the wave, ‘bypass’ and literal bit-identical pass-through. I made sure even the noise shaping wasn’t applying for the ‘bypass’ half.The result is this: waveforms that need a little sweetening, can have that texture (not frequency or EQ, texture) subtly added to the track, in a way that utterly and totally maintains the integrity of the signal. In some ways this is ‘purer’ than any of the other Purest line, since by its nature it’s true bypass for literally half the time. It’s also one of the minimal ones, with just a switch: there’s no way to get the perfect integration of bypass and effected halves (they switch off like a push-pull power amp) otherwise, and it produces an obvious enough effect but one that’s harmonious and doesn’t get in the way. In some circumstances it hints at being like a ‘bass optimizer’ since it’ll push some second harmonic by its nature. You’ll probably find that for any given sound (especially an electric bass guitar) there’ll be one polarity that’s clearly better: I think for basses where the string swings near the pickup and produces extra voltage on that side of the wave, PurestWarm will saturate that side of the wave really well and it’ll give you an ideal bass tone with no extra fiddling or processing required."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class PurestWarm final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    PurestWarm()
    {
        A = 0.0;
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
            case kParamA: return "polarity"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Polarity"; break;

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

        int polarity = (int)(A * 1.999);
        double inputSampleL;
        double inputSampleR;
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
            if (polarity == 1)
            {
                if (inputSampleL < 0)
                {
                    inputSampleL = -(sin(-inputSampleL * 1.57079634) / 1.57079634);
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
                }
                if (inputSampleR < 0)
                {
                    inputSampleR = -(sin(-inputSampleR * 1.57079634) / 1.57079634);
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
                }
            }
            else {
                if (inputSampleL > 0)
                {
                    inputSampleL = sin(inputSampleL * 1.57079634) / 1.57079634;
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
                }
                if (inputSampleR > 0)
                {
                    inputSampleR = sin(inputSampleR * 1.57079634) / 1.57079634;
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
                }
            }
            // that's it. Only applies on one half of the waveform, other half is passthrough untouched.
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::purestwarm
