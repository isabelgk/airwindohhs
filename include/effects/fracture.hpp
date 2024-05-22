#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Fracture final : public Effect<T>
{
    const std::string m_name{ "Fracture" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    Fracture()
    {
        A = 0.25; // 1 from 0 to 4: A*4
        B = 0.5; // 2 from 1 to 3: (B*2.999)+1
        C = 1.0;
        D = 1.0;
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
            case kParamD: D = value; break;

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
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive";
            case kParamB: return "Fracture";
            case kParamC: return "Out Lvl";
            case kParamD: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 4));
            case kParamB: return std::to_string((B * 2.999) + 1);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " ";
            case kParamB: return " ";
            case kParamC: return "dB";
            case kParamD: return " ";
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
        double density = A * 4;
        double fracture = (((B * 2.999) + 1) * 3.14159265358979);
        double output = C;
        double wet = D;
        double dry = 1.0 - wet;
        double bridgerectifier;
        density = density * fabs(density);
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL *= density;
            inputSampleR *= density;
            bridgerectifier = fabs(inputSampleL) * fracture;
            if (bridgerectifier > fracture) {
                bridgerectifier = fracture;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // blend according to density control
            bridgerectifier = fabs(inputSampleR) * fracture;
            if (bridgerectifier > fracture) {
                bridgerectifier = fracture;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // blend according to density control
            inputSampleL *= output;
            inputSampleR *= output;
            inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
            inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
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
