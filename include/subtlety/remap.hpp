#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Remap final : public Effect<T>
{
    std::string m_name{ "Remap" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    Remap()
    {
        A = 0.5;
        B = 1.0;
        C = 1.0;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Remap";
            case kParamB: return "Output";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double drive = A * 0.5;
        double gain = (drive + 0.2) * 8;
        double out = B;
        double wet = C;
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
            double gaintrimL = fabs(inputSampleL);
            double gaintrimR = fabs(inputSampleR);
            double bridgerectifierL = gaintrimL * gain;
            double bridgerectifierR = gaintrimR * gain;
            if (gaintrimL > 1.0) {
                gaintrimL = 1.0;
            }
            else {
                gaintrimL *= gaintrimL;
            }
            if (gaintrimR > 1.0) {
                gaintrimR = 1.0;
            }
            else {
                gaintrimR *= gaintrimR;
            }
            if (inputSampleL > 1.57079633) {
                bridgerectifierL = 1.0 - (1.0 - sin(bridgerectifierL));
            }
            else {
                bridgerectifierL = sin(bridgerectifierL);
            }
            if (inputSampleR > 1.57079633) {
                bridgerectifierR = 1.0 - (1.0 - sin(bridgerectifierR));
            }
            else {
                bridgerectifierR = sin(bridgerectifierR);
            }
            if (inputSampleL > 0) {
                inputSampleL -= (bridgerectifierL * gaintrimL * drive);
            }
            else {
                inputSampleL += (bridgerectifierL * gaintrimL * drive);
            }
            if (inputSampleR > 0) {
                inputSampleR -= (bridgerectifierR * gaintrimR * drive);
            }
            else {
                inputSampleR += (bridgerectifierR * gaintrimR * drive);
            }
            if (out < 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs
