#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SoftGate final : public Effect<T>
{
    std::string m_name{ "SoftGate" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedL[2];
    double diffL;
    double storedR[2];
    double diffR;
    double gate;
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
    SoftGate()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        storedL[0] = storedL[1] = 0.0;
        diffL = 0.0;
        storedR[0] = storedR[1] = 0.0;
        diffR = 0.0;
        gate = 1.1;
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
            case kParamA: return "Thresh";
            case kParamB: return "Darken";
            case kParamC: return "Silence";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double threshold = pow(A, 6);
        double recovery = pow((B * 0.5), 6);
        recovery /= overallscale;
        double baseline = pow(C, 6);
        double invrec = 1.0 - recovery;
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
            storedL[1] = storedL[0];
            storedL[0] = inputSampleL;
            diffL = storedL[0] - storedL[1];
            storedR[1] = storedR[0];
            storedR[0] = inputSampleR;
            diffR = storedR[0] - storedR[1];
            if (gate > 0) {
                gate = ((gate - baseline) * invrec) + baseline;
            }
            if ((fabs(diffR) > threshold) || (fabs(diffL) > threshold)) {
                gate = 1.1;
            }
            else {
                gate = (gate * invrec);
                if (threshold > 0) {
                    gate += ((fabs(inputSampleL) / threshold) * recovery);
                    gate += ((fabs(inputSampleR) / threshold) * recovery);
                }
            }
            if (gate < 0) {
                gate = 0;
            }
            if (gate < 1.0)
            {
                storedL[0] = storedL[1] + (diffL * gate);
                storedR[0] = storedR[1] + (diffR * gate);
            }
            if (gate < 1) {
                inputSampleL = (inputSampleL * gate) + (storedL[0] * (1.0 - gate));
                inputSampleR = (inputSampleR * gate) + (storedR[0] * (1.0 - gate));
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
