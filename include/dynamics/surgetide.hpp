#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SurgeTide final : public Effect<T>
{
    std::string m_name{ "SurgeTide" };

    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    double chaseA;
    double chaseB;
    double chaseC;
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
    SurgeTide()
    {
        A = 0.5;
        B = 0.3;
        C = 1.0;
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
            case kParamA: return "SurgeNode";
            case kParamB: return "SurgeRate";
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
            case kParamA: return " ";
            case kParamB: return " ";
            case kParamC: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
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
        double intensity = A;
        double attack = ((B + 0.1) * 0.0005) / overallscale;
        double decay = ((B + 0.001) * 0.00005) / overallscale;
        double wet = C;
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
            if (chaseC < inputSense) {
                chaseA += attack;
            }
            if (chaseC > inputSense) {
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
            if (chaseC > 1.0) {
                chaseC = 1.0;
            }
            if (chaseC < 0.0) {
                chaseC = 0.0;
            }
            inputSampleL *= chaseC;
            inputSampleL = drySampleL - (inputSampleL * intensity);
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
            inputSampleR *= chaseC;
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
