#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::rawglitters {

constexpr std::string_view k_name{ "RawGlitters" };
constexpr std::string_view k_short_description{
    "RawGlitters is just the quantization from Dither Me Timbers… and the opposite, as a brightener."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class RawGlitters final : public Effect<T>
{
    double lastSampleL;
    double lastSample2L;
    double lastSampleR;
    double lastSample2R;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    RawGlitters()
    {
        A = 1.0;
        B = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        lastSampleL = 0.0;
        lastSample2L = 0.0;
        lastSampleR = 0.0;
        lastSample2R = 0.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "quant"; break;
            case kParamB: return "derez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Quant"; break;
            case kParamB: return "DeRez"; break;

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

        int processing = (VstInt32)(A * 1.999);
        bool highres = false;
        if (processing == 1) {
            highres = true;
        }
        float scaleFactor;
        if (highres) {
            scaleFactor = 8388608.0;
        }
        else {
            scaleFactor = 32768.0;
        }
        float derez = B;
        if (derez > 0.0) {
            scaleFactor *= pow(1.0 - derez, 6);
        }
        if (scaleFactor < 0.0001) {
            scaleFactor = 0.0001;
        }
        float outScale = scaleFactor;
        if (outScale < 8.0) {
            outScale = 8.0;
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            double outputSampleL;
            double outputSampleR;
            inputSampleL += 0.381966011250105;
            inputSampleR += 0.381966011250105;
            if ((lastSampleL + lastSampleL) <= (inputSampleL + lastSample2L)) {
                outputSampleL = floor(lastSampleL);
            }
            else {
                outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
            }
            if ((lastSampleR + lastSampleR) <= (inputSampleR + lastSample2R)) {
                outputSampleR = floor(lastSampleR);
            }
            else {
                outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
            }
            lastSample2L = lastSampleL;
            lastSampleL = inputSampleL; // we retain three samples in a row
            lastSample2R = lastSampleR;
            lastSampleR = inputSampleR; // we retain three samples in a row
            *out1 = outputSampleL / outScale;
            *out2 = outputSampleR / outScale;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::rawglitters
