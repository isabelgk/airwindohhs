#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PurestWarm2 final : public Effect<T>
{
    const std::string m_name{ "PurestWarm2" };

    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total
    }; // fixed frequency biquad filter for ultrasonics, stereo
    double fixA[fix_total];
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
    PurestWarm2()
    {
        A = 0.0;
        B = 0.0;
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
            case kParamA: return "Dry/Pos";
            case kParamB: return "Dry/Neg";

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

        double pos = A;
        double neg = B;
        double cutoff = 25000.0 / getSampleRate();
        if (cutoff > 0.49) {
            cutoff = 0.49; // don't crash if run at 44.1k
        }
        fixA[fix_freq] = cutoff;
        fixA[fix_reso] = 0.70710678; // butterworth Q
        double K = tan(M_PI * fixA[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        fixA[fix_sL1] = 0.0;
        fixA[fix_sL2] = 0.0;
        fixA[fix_sR1] = 0.0;
        fixA[fix_sR2] = 0.0;
        // define filters here: on VST you can't define them in reset 'cos getSampleRate isn't returning good information yet
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
            double outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            if (inputSampleL > 0) {
                inputSampleL = (sin(inputSampleL * 1.57079634 * pos) / 1.57079634) + (inputSampleL * (1.0 - pos));
            }
            if (inputSampleL < 0) {
                inputSampleL = (sin(inputSampleL * 1.57079634 * neg) / 1.57079634) + (inputSampleL * (1.0 - neg));
            }
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            if (inputSampleR > 0) {
                inputSampleR = (sin(inputSampleR * 1.57079634 * pos) / 1.57079634) + (inputSampleR * (1.0 - pos));
            }
            if (inputSampleR < 0) {
                inputSampleR = (sin(inputSampleR * 1.57079634 * neg) / 1.57079634) + (inputSampleR * (1.0 - neg));
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