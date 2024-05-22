#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Overheads final : public Effect<T>
{
    const std::string m_name{ "Overheads" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    double ovhGain;
    double ovhL[130];
    double ovhR[130];
    int ovhCount;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    Overheads()
    {
        A = 0.0;
        B = 0.5;
        C = 1.0;
        ovhGain = 1.0;
        for (int count = 0; count < 129; count++) {
            ovhL[count] = 0.0;
            ovhR[count] = 0.0;
        }
        ovhCount = 0;
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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
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

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Compr";
            case kParamB: return "Sharp";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
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

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double ovhTrim = pow(A, 3);
        ovhTrim += 1.0;
        ovhTrim *= ovhTrim;
        int offset = (pow(B, 7) * 16.0 * overallscale) + 1;
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
            // begin Overheads compressor
            inputSampleL *= ovhTrim;
            inputSampleR *= ovhTrim;
            ovhCount--;
            if (ovhCount < 0 || ovhCount > 128) {
                ovhCount = 128;
            }
            ovhL[ovhCount] = inputSampleL;
            ovhR[ovhCount] = inputSampleR;
            double ovhClamp = sin(fabs(inputSampleL - ovhL[(ovhCount + offset) - (((ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
            ovhGain *= (1.0 - ovhClamp);
            ovhGain += ((1.0 - ovhClamp) * ovhClamp);
            ovhClamp = sin(fabs(inputSampleR - ovhR[(ovhCount + offset) - (((ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
            ovhGain *= (1.0 - ovhClamp);
            ovhGain += ((1.0 - ovhClamp) * ovhClamp);
            if (ovhGain > 1.0) {
                ovhGain = 1.0;
            }
            if (ovhGain < 0.0) {
                ovhGain = 0.0;
            }
            inputSampleL *= ovhGain;
            inputSampleR *= ovhGain;
            // end Overheads compressor
            if (wet != 1.0) {
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs
