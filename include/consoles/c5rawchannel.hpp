#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::c5rawchannel {

constexpr std::string_view k_name{ "C5RawChannel" };
constexpr std::string_view k_short_description{
    ""
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class C5RawChannel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastFXChannelL;
    double lastSampleChannelL;
    double lastFXChannelR;
    double lastSampleChannelR;
    float A;

  public:
    C5RawChannel()
    {
        A = 0.0;
        lastFXChannelL = 0.0;
        lastSampleChannelL = 0.0;
        lastFXChannelR = 0.0;
        lastSampleChannelR = 0.0;
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
            case kParamA: return "center"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Center"; break;

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

        double centering = A * 0.5;
        centering = 1.0 - pow(centering, 5);
        // we can set our centering force from zero to rather high, but
        // there's a really intense taper on it forcing it to mostly be almost 1.0.
        // If it's literally 1.0, we don't even apply it, and you get the original
        // Xmas Morning bugged-out Console5, which is the default setting for Raw Console5.
        double differenceL;
        double differenceR;
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
            differenceL = lastSampleChannelL - inputSampleL;
            lastSampleChannelL = inputSampleL;
            // derive slew part off direct sample measurement + from last time
            differenceR = lastSampleChannelR - inputSampleR;
            lastSampleChannelR = inputSampleR;
            // derive slew part off direct sample measurement + from last time
            if (differenceL > 1.0) {
                differenceL = 1.0;
            }
            if (differenceL < -1.0) {
                differenceL = -1.0;
            }
            if (differenceR > 1.0) {
                differenceR = 1.0;
            }
            if (differenceR < -1.0) {
                differenceR = -1.0;
            }
            inputSampleL = lastFXChannelL + asin(differenceL);
            lastFXChannelL = inputSampleL;
            if (centering < 1.0) {
                lastFXChannelL *= centering;
            }
            // if we're using the crude centering force, it's applied here
            inputSampleR = lastFXChannelR + asin(differenceR);
            lastFXChannelR = inputSampleR;
            if (centering < 1.0) {
                lastFXChannelR *= centering;
            }
            // if we're using the crude centering force, it's applied here
            if (lastFXChannelL > 1.0) {
                lastFXChannelL = 1.0;
            }
            if (lastFXChannelL < -1.0) {
                lastFXChannelL = -1.0;
            }
            if (lastFXChannelR > 1.0) {
                lastFXChannelR = 1.0;
            }
            if (lastFXChannelR < -1.0) {
                lastFXChannelR = -1.0;
            }
            // build new signal off what was present in output last time
            // slew aspect
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
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
} // namespace airwindohhs::c5rawchannel
