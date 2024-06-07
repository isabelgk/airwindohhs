#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::powersag2 {

constexpr std::string_view k_name{ "PowerSag2" };
constexpr std::string_view k_short_description{
    "PowerSag2 is my improved circuit-starve plugin, now with inverse effect!"
};
constexpr std::string_view k_long_description{
    "This is PowerSag (my circuit-power-supply-starve plugin), but the internals are coded differently for more efficiency on modern CPUs, it gets twice as much maximum effect range (which will help if you’re using it at high sample rates) and it now has an inverse/wet control. That means that you can hear what’s being taken AWAY (which is typically a grungey, gatey effect) and fade into that if it helps you get more meat into some of your sounds."
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class PowerSag2 final : public Effect<T>
{
    double dL[16386];
    double dR[16386];
    double controlL;
    double controlR;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    PowerSag2()
    {
        for (int count = 0; count < 16385; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        controlL = 0;
        controlR = 0;
        gcount = 0;
        A = 0.3;
        B = 0.5;
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
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.3; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "range"; break;
            case kParamB: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Range"; break;
            case kParamB: return "Inv/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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

        double depth = pow(A, 4);
        int offset = (int)(depth * 16383) + 1;
        double wet = (B * 2.0) - 1.0;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (gcount < 0 || gcount > 16384) {
                gcount = 16384;
            }
            dL[gcount] = fabs(inputSampleL);
            dR[gcount] = fabs(inputSampleR);
            controlL += dL[gcount];
            controlR += dR[gcount];
            controlL -= dL[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
            controlR -= dR[gcount + offset - ((gcount + offset > 16384) ? 16384 : 0)];
            gcount--;
            if (controlL > offset) {
                controlL = offset;
            }
            if (controlL < 0.0) {
                controlL = 0.0;
            }
            if (controlR > offset) {
                controlR = offset;
            }
            if (controlR < 0.0) {
                controlR = 0.0;
            }
            double burst = inputSampleL * (controlL / sqrt(offset));
            double clamp = inputSampleL / ((burst == 0.0) ? 1.0 : burst);
            if (clamp > 1.0) {
                clamp = 1.0;
            }
            if (clamp < 0.0) {
                clamp = 0.0;
            }
            inputSampleL *= clamp;
            double difference = drySampleL - inputSampleL;
            if (wet < 0.0) {
                drySampleL *= (wet + 1.0);
            }
            inputSampleL = drySampleL - (difference * wet);
            burst = inputSampleR * (controlR / sqrt(offset));
            clamp = inputSampleR / ((burst == 0.0) ? 1.0 : burst);
            if (clamp > 1.0) {
                clamp = 1.0;
            }
            if (clamp < 0.0) {
                clamp = 0.0;
            }
            inputSampleR *= clamp;
            difference = drySampleR - inputSampleR;
            if (wet < 0.0) {
                drySampleR *= (wet + 1.0);
            }
            inputSampleR = drySampleR - (difference * wet);
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
} // namespace airwindohhs::powersag2
