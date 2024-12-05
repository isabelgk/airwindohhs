#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hermetrim {

constexpr std::string_view k_name{ "HermeTrim" };
constexpr std::string_view k_short_description{
    "HermeTrim is Very Fine Adjustments, otherwise just like EveryTrim."
};
constexpr std::string_view k_long_description{
    "If you’ve seen EveryTrim, you know how this works: it’s the all-possible-trims trim. +- 12dB of left, right, mid, side, and master volume. By request, and a handy little tool for all manner of stereo tweaking. It’s very nearly as clear and accurate as PurestGain, which itself is very nearly as accurate as BitShiftGain (and most people will find any of these more than acceptable, because this is uber-pickiness land and none of you are likely to be able to tell these from your standard utility gain plugin from your DAW, ten out of ten. I just know I’ve implemented mine with extra rigorousness)Thing is, the person who’d requested this is the same one who requested Hermepass… and in this case, I fumbled it a bit. On many DAWs, and all my Mac Audio Unit stuff, you can hold option to get tiny fine adjustments. But my user with the enlightening (and accessible) requests isn’t using a host that can do that… and needed to have much, much finer adjustments on tap.So, now Hermepass has a companion, HermeTrim. This is exactly the same as EveryTrim, only it’s 1.5 dB plus and minus. It’s more a mastering tool: make tiny little tweaks to get the master just right. EveryTrim will sound just as good, but this one’s geared towards non-mixing purposes. Hope you like it."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class HermeTrim final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    HermeTrim()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
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
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "left"; break;
            case kParamB: return "right"; break;
            case kParamC: return "mid"; break;
            case kParamD: return "side"; break;
            case kParamE: return "master"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Left"; break;
            case kParamB: return "Right"; break;
            case kParamC: return "Mid"; break;
            case kParamD: return "Side"; break;
            case kParamE: return "Master"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB"; break;
            case kParamB: return "dB"; break;
            case kParamC: return "dB"; break;
            case kParamD: return "dB"; break;
            case kParamE: return "dB"; break;

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

        double leftgain = pow(10.0, ((A * 3.0) - 1.5) / 20.0);
        double rightgain = pow(10.0, ((B * 3.0) - 1.5) / 20.0);
        double midgain = pow(10.0, ((C * 3.0) - 1.5) / 20.0);
        double sidegain = pow(10.0, ((D * 3.0) - 1.5) / 20.0);
        double mastergain = pow(10.0, ((E * 3.0) - 1.5) / 20.0) * 0.5;
        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
        leftgain *= mastergain;
        rightgain *= mastergain;
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
            mid *= midgain;
            side *= sidegain;
            inputSampleL = (mid + side) * leftgain;
            inputSampleR = (mid - side) * rightgain;
            // contains mastergain and the gain trim fixing the mid/side
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
} // namespace airwindohhs::hermetrim
