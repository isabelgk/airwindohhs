#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::fastdistance2 {

constexpr std::string_view k_name{ "FastDistance2" };
constexpr std::string_view k_short_description{
    ""
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class FastDistance2 final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'fds2';
    float A;
    float fdlA, fdlB, fdlC, fdlD, fdlE, fdlF, fdlG, fdlH;
    float fdrA, fdrB, fdrC, fdrD, fdrE, fdrF, fdrG, fdrH;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    FastDistance2()
    {
        A = 0.0;
        fdlA = fdlB = fdlC = fdlD = fdlE = fdlF = fdlG = fdlH = 0.0;
        fdrA = fdrB = fdrC = fdrD = fdrE = fdrF = fdrG = fdrH = 0.0;
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
        switch (index) {
            case kParamA: A = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "distnce"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Distnce"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(A, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
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
        float distance = pow(1.0f - A, overallscale + 1.0f);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= 0.5f; // gain stage as part of the design
            inputSampleR *= 0.5f; // gain stage as part of the design
            // this can be omitted in a game engine and then you scale the
            // entire output by -6dB at the end, rather than constantly
            // having to make everything *0.5f
            fdlE = distance * 0.875f * (inputSampleL - fdlA - fdlE) + fdlE;
            fdlF = distance * (fdlA - fdlB - fdlF) + fdlF;
            fdlG = distance * (fdlB - fdlC - fdlG) + fdlG;
            fdlH = distance * (fdlC - fdlD - fdlH) + fdlH;
            fdlA = distance * 0.875f * fdlE + fdlA;
            fdlB = distance * fdlF + fdlB;
            fdlC = distance * fdlG + fdlC;
            fdlD = distance * fdlH + fdlD;
            inputSampleL = fdlC + fdlD;
            fdrE = distance * 0.875f * (inputSampleR - fdrA - fdrE) + fdrE;
            fdrF = distance * (fdrA - fdrB - fdrF) + fdrF;
            fdrG = distance * (fdrB - fdrC - fdrG) + fdrG;
            fdrH = distance * (fdrC - fdrD - fdrH) + fdrH;
            fdrA = distance * 0.875f * fdrE + fdrA;
            fdrB = distance * fdrF + fdrB;
            fdrC = distance * fdrG + fdrC;
            fdrD = distance * fdrH + fdrD;
            inputSampleR = fdrC + fdrD;
            // somewhere, scale by -6dB
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
            // end 64 bit stereo floating point dither
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::fastdistance2
