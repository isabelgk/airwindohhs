#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Surge final : public Effect<T>
{
    const std::string m_name{ "Surge" };

    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    double chaseA;
    double chaseB;
    double chaseC;
    double chaseD;
    double chaseMax;
    float A;
    float B;

    enum params
    {
        kParamfpdL = 0,
        kParamfpdR = 1,
        kNumParameters = 2

    };

  public:
    Surge()
    {
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = true;
        chaseA = 0.0;
        chaseB = 0.0;
        chaseC = 0.0;
        chaseD = 0.0;
        chaseMax = 0.0;
        A = 0.0;
        B = 1.0;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParamfpdL: fpdL = value; break;
            case kParamfpdR: fpdR = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamfpdL: return fpdL;
            case kParamfpdR: return fpdR;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamfpdL: return "Surge";
            case kParamfpdR: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamfpdL: return std::to_string(A);
            case kParamfpdR: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamfpdL: return " ";
            case kParamfpdR: return " ";
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
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double chaseMax = 0.0;
        double intensity = (1.0 - (pow((1.0 - A), 2))) * 0.7;
        double attack = ((intensity + 0.1) * 0.0005) / overallscale;
        double decay = ((intensity + 0.001) * 0.00005) / overallscale;
        double wet = B;
        // removed extra dry variable
        double inputSense;
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
            inputSampleL *= 8.0;
            inputSampleR *= 8.0;
            inputSampleL *= intensity;
            inputSampleR *= intensity;
            inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            if (chaseMax < inputSense) {
                chaseA += attack;
            }
            if (chaseMax > inputSense) {
                chaseA -= decay;
            }
            if (chaseA > decay) {
                chaseA = decay;
            }
            if (chaseA < -attack) {
                chaseA = -attack;
            }
            chaseB += (chaseA / overallscale);
            if (chaseB > decay) {
                chaseB = decay;
            }
            if (chaseB < -attack) {
                chaseB = -attack;
            }
            chaseC += (chaseB / overallscale);
            if (chaseC > decay) {
                chaseC = decay;
            }
            if (chaseC < -attack) {
                chaseC = -attack;
            }
            chaseD += (chaseC / overallscale);
            if (chaseD > 1.0) {
                chaseD = 1.0;
            }
            if (chaseD < 0.0) {
                chaseD = 0.0;
            }
            chaseMax = chaseA;
            if (chaseMax < chaseB) {
                chaseMax = chaseB;
            }
            if (chaseMax < chaseC) {
                chaseMax = chaseC;
            }
            if (chaseMax < chaseD) {
                chaseMax = chaseD;
            }
            inputSampleL *= chaseMax;
            inputSampleL = drySampleL - (inputSampleL * intensity);
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
            inputSampleR *= chaseMax;
            inputSampleR = drySampleR - (inputSampleR * intensity);
            inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
