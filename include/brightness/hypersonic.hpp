#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Hypersonic final : public Effect<T>
{
    std::string m_name{ "Hypersonic" };

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
    double fixB[fix_total];
    double fixC[fix_total];
    double fixD[fix_total];
    double fixE[fix_total];
    double fixF[fix_total];
    double fixG[fix_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

    enum params
    {
        kNumParameters = 0

    };

  public:
    Hypersonic()
    {
        for (int x = 0; x < fix_total; x++) {
            fixA[x] = 0.0;
            fixB[x] = 0.0;
            fixC[x] = 0.0;
            fixD[x] = 0.0;
            fixE[x] = 0.0;
            fixF[x] = 0.0;
            fixG[x] = 0.0;
        }
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double cutoff = 25000.0 / getSampleRate();
        if (cutoff > 0.49) {
            cutoff = 0.49; // don't crash if run at 44.1k
        }
        fixG[fix_freq] = fixF[fix_freq] = fixE[fix_freq] = fixD[fix_freq] = fixC[fix_freq] = fixB[fix_freq] = fixA[fix_freq] = cutoff;
        fixA[fix_reso] = 4.46570214;
        fixB[fix_reso] = 1.51387132;
        fixC[fix_reso] = 0.93979296;
        fixD[fix_reso] = 0.70710678;
        fixE[fix_reso] = 0.59051105;
        fixF[fix_reso] = 0.52972649;
        fixG[fix_reso] = 0.50316379;
        double K = tan(M_PI * fixA[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixB[fix_freq]);
        norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
        fixB[fix_a0] = K * K * norm;
        fixB[fix_a1] = 2.0 * fixB[fix_a0];
        fixB[fix_a2] = fixB[fix_a0];
        fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixC[fix_freq]);
        norm = 1.0 / (1.0 + K / fixC[fix_reso] + K * K);
        fixC[fix_a0] = K * K * norm;
        fixC[fix_a1] = 2.0 * fixC[fix_a0];
        fixC[fix_a2] = fixC[fix_a0];
        fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixC[fix_b2] = (1.0 - K / fixC[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixD[fix_freq]);
        norm = 1.0 / (1.0 + K / fixD[fix_reso] + K * K);
        fixD[fix_a0] = K * K * norm;
        fixD[fix_a1] = 2.0 * fixD[fix_a0];
        fixD[fix_a2] = fixD[fix_a0];
        fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixD[fix_b2] = (1.0 - K / fixD[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixE[fix_freq]);
        norm = 1.0 / (1.0 + K / fixE[fix_reso] + K * K);
        fixE[fix_a0] = K * K * norm;
        fixE[fix_a1] = 2.0 * fixE[fix_a0];
        fixE[fix_a2] = fixE[fix_a0];
        fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixE[fix_b2] = (1.0 - K / fixE[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixF[fix_freq]);
        norm = 1.0 / (1.0 + K / fixF[fix_reso] + K * K);
        fixF[fix_a0] = K * K * norm;
        fixF[fix_a1] = 2.0 * fixF[fix_a0];
        fixF[fix_a2] = fixF[fix_a0];
        fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixF[fix_b2] = (1.0 - K / fixF[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixG[fix_freq]);
        norm = 1.0 / (1.0 + K / fixG[fix_reso] + K * K);
        fixG[fix_a0] = K * K * norm;
        fixG[fix_a1] = 2.0 * fixG[fix_a0];
        fixG[fix_a2] = fixG[fix_a0];
        fixG[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixG[fix_b2] = (1.0 - K / fixG[fix_reso] + K * K) * norm;
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
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
            fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
            fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
            fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
            fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixC[fix_a0]) + fixC[fix_sL1];
            fixC[fix_sL1] = (inputSampleL * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
            fixC[fix_sL2] = (inputSampleL * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixC[fix_a0]) + fixC[fix_sR1];
            fixC[fix_sR1] = (inputSampleR * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sR2];
            fixC[fix_sR2] = (inputSampleR * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixD[fix_a0]) + fixD[fix_sL1];
            fixD[fix_sL1] = (inputSampleL * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
            fixD[fix_sL2] = (inputSampleL * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixD[fix_a0]) + fixD[fix_sR1];
            fixD[fix_sR1] = (inputSampleR * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sR2];
            fixD[fix_sR2] = (inputSampleR * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixE[fix_a0]) + fixE[fix_sL1];
            fixE[fix_sL1] = (inputSampleL * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
            fixE[fix_sL2] = (inputSampleL * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixE[fix_a0]) + fixE[fix_sR1];
            fixE[fix_sR1] = (inputSampleR * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sR2];
            fixE[fix_sR2] = (inputSampleR * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixF[fix_a0]) + fixF[fix_sL1];
            fixF[fix_sL1] = (inputSampleL * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
            fixF[fix_sL2] = (inputSampleL * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixF[fix_a0]) + fixF[fix_sR1];
            fixF[fix_sR1] = (inputSampleR * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sR2];
            fixF[fix_sR2] = (inputSampleR * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleL * fixG[fix_a0]) + fixG[fix_sL1];
            fixG[fix_sL1] = (inputSampleL * fixG[fix_a1]) - (outSample * fixG[fix_b1]) + fixG[fix_sL2];
            fixG[fix_sL2] = (inputSampleL * fixG[fix_a2]) - (outSample * fixG[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixG[fix_a0]) + fixG[fix_sR1];
            fixG[fix_sR1] = (inputSampleR * fixG[fix_a1]) - (outSample * fixG[fix_b1]) + fixG[fix_sR2];
            fixG[fix_sR2] = (inputSampleR * fixG[fix_a2]) - (outSample * fixG[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            // begin 64 bit stereo floating point dither
            // only relevant if buss is double
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
