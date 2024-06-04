#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::everytrim {

constexpr std::string_view k_name{ "EveryTrim" };
constexpr std::string_view k_short_description{
    "EveryTrim is Left/Right, Mid/Side, and Master in one plugin."
};
constexpr std::string_view k_long_description{
    "EveryTrim is like PurestGain only more so: it is very simply every basic stereo trim you can have (in loudness terms, anyhow!) You get left, right, mid/side, and a master level control. It works on stereo tracks only, as mid/side is meaningless without stereo.It’s also efficiently coded, suppresses denormal numbers, and uses the same noise shaping to the floating point buss you get in PurestGain. If you need a nice basic gain trim that does all those things (and nothing fancy: Wider is much more sophisticated, and I’ve got an idea for a still more sophisticated stereo-widener plugin that is in the works) then EveryTrim will come in handy. Begone, dull pan-pots! EveryTrim will also be simpler than using EdIsDim and MidSide just to adjust mid/side balances: while you can do that with that pair of plugins, they’re really for doing processing between them using another plugin (any plugin, doesn’t have to be M/S). With EveryTrim, you can tweak mid and side levels directly, in a more obvious way."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class EveryTrim final : public Effect<T>
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
    EveryTrim()
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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;
            case kParamC: return 0.5;
            case kParamD: return 0.5;
            case kParamE: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "left";
            case kParamB: return "right";
            case kParamC: return "mid";
            case kParamD: return "side";
            case kParamE: return "master";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Left";
            case kParamB: return "Right";
            case kParamC: return "Mid";
            case kParamD: return "Side";
            case kParamE: return "Master";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24.0) - 12.0);
            case kParamB: return std::to_string((B * 24.0) - 12.0);
            case kParamC: return std::to_string((C * 24.0) - 12.0);
            case kParamD: return std::to_string((D * 24.0) - 12.0);
            case kParamE: return std::to_string((E * 24.0) - 12.0);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return "dB";
            case kParamC: return "dB";
            case kParamD: return "dB";
            case kParamE: return "dB";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double leftgain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
        double rightgain = pow(10.0, ((B * 24.0) - 12.0) / 20.0);
        double midgain = pow(10.0, ((C * 24.0) - 12.0) / 20.0);
        double sidegain = pow(10.0, ((D * 24.0) - 12.0) / 20.0);
        double mastergain = pow(10.0, ((E * 24.0) - 12.0) / 20.0) * 0.5;
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
} // namespace airwindohhs::everytrim
