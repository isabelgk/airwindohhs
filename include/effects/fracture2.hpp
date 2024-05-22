#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Fracture2 final : public Effect<T>
{
    const std::string m_name{ "Fracture2" };

    double densityA;
    double densityB;
    double thresholdA;
    double thresholdB;
    double outputA;
    double outputB;
    double wetA;
    double wetB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5

    };

  public:
    Fracture2()
    {
        A = 0.1;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        densityA = 1.0;
        densityB = 1.0;
        thresholdA = 0.5;
        thresholdB = 0.5;
        outputA = 1.0;
        outputB = 1.0;
        wetA = 1.0;
        wetB = 1.0;
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
            case kParamE: E = value; break;

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
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive";
            case kParamB: return "Fractre";
            case kParamC: return "Thresh";
            case kParamD: return "Output";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string((B * 8.0));
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

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
            case kParamD: return "";
            case kParamE: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        densityA = densityB;
        densityB = A * 10.0; // 0.0 to 10.0
        int stages = B * 8.0;
        thresholdA = thresholdB;
        thresholdB = C;
        outputA = outputB;
        outputB = D;
        wetA = wetB;
        wetB = E;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double density = (densityA * temp) + (densityB * (1.0 - temp));
            double threshold = (thresholdA * temp) + (thresholdB * (1.0 - temp));
            double output = (outputA * temp) + (outputB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            inputSampleL *= density;
            inputSampleR *= density;
            for (int x = 0; x < stages; x++) {
                inputSampleL *= (fabs(inputSampleL) + 1.0);
                inputSampleR *= (fabs(inputSampleR) + 1.0);
            }
            if (inputSampleL > M_PI_2) {
                inputSampleL = (sin(inputSampleL) * threshold) + (1.0 * (1.0 - threshold));
            }
            else if (inputSampleL < -M_PI_2) {
                inputSampleL = (sin(inputSampleL) * threshold) + (-1.0 * (1.0 - threshold));
            }
            else {
                inputSampleL = sin(inputSampleL);
            }
            if (inputSampleR > M_PI_2) {
                inputSampleR = (sin(inputSampleR) * threshold) + (1.0 * (1.0 - threshold));
            }
            else if (inputSampleR < -M_PI_2) {
                inputSampleR = (sin(inputSampleR) * threshold) + (-1.0 * (1.0 - threshold));
            }
            else {
                inputSampleR = sin(inputSampleR);
            }
            inputSampleL *= output;
            inputSampleR *= output;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
