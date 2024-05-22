#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class HermeTrim final : public Effect<T>
{
    std::string m_name{ "HermeTrim" };

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
    HermeTrim()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
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
            case kParamD: D = value; break;
            case kParamE: E = value; break;

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
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Left";
            case kParamB: return "Right";
            case kParamC: return "Mid";
            case kParamD: return "Side";
            case kParamE: return "Master";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 3.0) - 1.5);
            case kParamB: return std::to_string((B * 3.0) - 1.5);
            case kParamC: return std::to_string((C * 3.0) - 1.5);
            case kParamD: return std::to_string((D * 3.0) - 1.5);
            case kParamE: return std::to_string((E * 3.0) - 1.5);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return "dB";
            case kParamC: return "dB";
            case kParamD: return "dB";
            case kParamE: return "dB";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double leftgain = pow(10.0, ((A * 3.0) - 1.5) / 20.0);
        double rightgain = pow(10.0, ((B * 3.0) - 1.5) / 20.0);
        double midgain = pow(10.0, ((C * 3.0) - 1.5) / 20.0);
        double sidegain = pow(10.0, ((D * 3.0) - 1.5) / 20.0);
        double mastergain = pow(10.0, ((E * 3.0) - 1.5) / 20.0) * 0.5;
        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
        leftgain *= mastergain;
        rightgain *= mastergain;
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
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            mid *= midgain;
            side *= sidegain;
            inputSampleL = (mid + side) * leftgain;
            inputSampleR = (mid - side) * rightgain;
            // contains mastergain and the gain trim fixing the mid/side
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
