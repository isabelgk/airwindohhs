#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SingleEndedTriode final : public Effect<T>
{
    const std::string m_name{ "SingleEndedTriode" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double postsine;
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    SingleEndedTriode()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        postsine = sin(0.5);
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
            case kParamA: return "Triode";
            case kParamB: return "Clas AB";
            case kParamC: return "Clas B";
            case kParamD: return "Dry/Wet";

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
            case kParamD: return std::to_string(D);

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double intensity = pow(A, 2) * 8.0;
        double triode = intensity;
        intensity += 0.001;
        double softcrossover = pow(B, 3) / 8.0;
        double hardcrossover = pow(C, 7) / 8.0;
        double wet = D;
        // removed extra dry variable
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
            if (triode > 0.0)
            {
                inputSampleL *= intensity;
                inputSampleR *= intensity;
                inputSampleL -= 0.5;
                inputSampleR -= 0.5;
                double bridgerectifier = fabs(inputSampleL);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleL > 0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleR > 0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
                inputSampleL += postsine;
                inputSampleR += postsine;
                inputSampleL /= intensity;
                inputSampleR /= intensity;
            }
            if (softcrossover > 0.0)
            {
                double bridgerectifier = fabs(inputSampleL);
                if (bridgerectifier > 0.0) {
                    bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
                }
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0;
                }
                if (inputSampleL > 0.0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                if (bridgerectifier > 0.0) {
                    bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
                }
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0;
                }
                if (inputSampleR > 0.0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
            }
            if (hardcrossover > 0.0)
            {
                double bridgerectifier = fabs(inputSampleL);
                bridgerectifier -= hardcrossover;
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0.0;
                }
                if (inputSampleL > 0.0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                bridgerectifier -= hardcrossover;
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0.0;
                }
                if (inputSampleR > 0.0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
            }
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
