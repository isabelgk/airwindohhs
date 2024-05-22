#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Dubly final : public Effect<T>
{
    const std::string m_name{ "Dubly" };

    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    Dubly()
    {
        A = 0.5;
        B = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Dubly";
            case kParamB: return "Dry/Wet";

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
        double dublyAmount = pow(A, 3) * 0.16;
        double outlyAmount = pow(A, 3) * 0.160618;
        double gain = outlyAmount + 1.0;
        double iirAmount = 0.4275 / overallscale;
        double wet = B;
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
            iirSampleAL = (iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            double doubly = inputSampleL - iirSampleAL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL += doubly * dublyAmount;
            iirSampleAR = (iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            doubly = inputSampleR - iirSampleAR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR += doubly * dublyAmount;
            // end Dubly encode
            inputSampleL *= gain;
            inputSampleL = sin(inputSampleL);
            inputSampleL /= gain;
            inputSampleR *= gain;
            inputSampleR = sin(inputSampleR);
            inputSampleR /= gain;
            iirSampleBL = (iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            doubly = inputSampleL - iirSampleBL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL -= doubly * outlyAmount;
            iirSampleBR = (iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            doubly = inputSampleR - iirSampleBR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR -= doubly * outlyAmount;
            // end Dubly decode
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