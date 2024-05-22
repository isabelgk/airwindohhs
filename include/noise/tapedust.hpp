#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class TapeDust final : public Effect<T>
{
    std::string m_name{ "TapeDust" };

    uint32_t fpdL;
    uint32_t fpdR;
    bool fpFlip;
    // default stuff
    double bL[11];
    double fL[11];
    double bR[11];
    double fR[11];
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    TapeDust()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            fL[count] = 0.0;
            bR[count] = 0.0;
            fR[count] = 0.0;
        }
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        fpFlip = true;
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Dust";
            case kParamB: return "Dry/Wet";

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double rRange = pow(A, 2) * 5.0;
        double xfuzz = rRange * 0.002;
        double rOffset = (rRange * 0.4) + 1.0;
        double rDepthL; // the randomly fluctuating value
        double rDepthR; // the randomly fluctuating value
        double gainL;
        double gainR;
        double wet = B;
        // removed extra dry variable
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
            for (int count = 9; count < 0; count--) {
                bL[count + 1] = bL[count];
                bR[count + 1] = bR[count];
            }
            bL[0] = inputSampleL;
            bR[0] = inputSampleR;
            inputSampleL = rand() / (double)RAND_MAX;
            inputSampleR = rand() / (double)RAND_MAX;
            gainL = rDepthL = (inputSampleL * rRange) + rOffset;
            gainR = rDepthR = (inputSampleR * rRange) + rOffset;
            inputSampleL *= ((1.0 - fabs(bL[0] - bL[1])) * xfuzz);
            inputSampleR *= ((1.0 - fabs(bR[0] - bR[1])) * xfuzz);
            if (fpFlip) {
                inputSampleL = -inputSampleL;
                inputSampleR = -inputSampleR;
            }
            fpFlip = !fpFlip;
            for (int count = 0; count < 9; count++) {
                if (gainL > 1.0) {
                    fL[count] = 1.0;
                    gainL -= 1.0;
                }
                else {
                    fL[count] = gainL;
                    gainL = 0.0;
                }
                if (gainR > 1.0) {
                    fR[count] = 1.0;
                    gainR -= 1.0;
                }
                else {
                    fR[count] = gainR;
                    gainR = 0.0;
                }
                fL[count] /= rDepthL;
                fR[count] /= rDepthR;
                inputSampleL += (bL[count] * fL[count]);
                inputSampleR += (bR[count] * fR[count]);
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
