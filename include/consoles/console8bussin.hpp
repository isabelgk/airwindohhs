#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console8bussin {

constexpr std::string_view k_name{ "Console8BussIn" };
constexpr std::string_view k_short_description{
    "Console8BussIn moves to a channel/submix/buss topology and adds analog simulation."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console8BussIn final : public Effect<T>
{
    double iirAL;
    double iirBL;
    double iirAR;
    double iirBR;
    bool hsr;
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
    double fix[fix_total];
    double softL[11];
    double softR[11];
    int cycleEnd;
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    Console8BussIn()
    {
        iirAL = 0.0;
        iirBL = 0.0;
        iirAR = 0.0;
        iirBR = 0.0;
        for (int x = 0; x < fix_total; x++) {
            fix[x] = 0.0;
        }
        for (int x = 0; x < 10; x++) {
            softL[x] = 0.0;
            softR[x] = 0.0;
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

    enum params
    {
        kNumParameters = 0
    };

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

    T get_parameter_default(int index)
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

    constexpr std::string_view get_parameter_title(int index)
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

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double iirAmountA = 12.66 / Effect<T>::getSampleRate();
        // this is our distributed unusual highpass, which is
        // adding subtle harmonics to the really deep stuff to define it
        if (fabs(iirAL) < 1.18e-37) {
            iirAL = 0.0;
        }
        if (fabs(iirBL) < 1.18e-37) {
            iirBL = 0.0;
        }
        if (fabs(iirAR) < 1.18e-37) {
            iirAR = 0.0;
        }
        if (fabs(iirBR) < 1.18e-37) {
            iirBR = 0.0;
        }
        // catch denormals early and only check once per buffer
        if (Effect<T>::getSampleRate() > 49000.0) {
            hsr = true;
        }
        else {
            hsr = false;
        }
        fix[fix_freq] = 24000.0 / Effect<T>::getSampleRate();
        fix[fix_reso] = 0.5;
        double K = tan(M_PI * fix[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fix[fix_reso] + K * K);
        fix[fix_a0] = K * K * norm;
        fix[fix_a1] = 2.0 * fix[fix_a0];
        fix[fix_a2] = fix[fix_a0];
        fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fix[fix_b2] = (1.0 - K / fix[fix_reso] + K * K) * norm;
        // this is the fixed biquad distributed anti-aliasing filter
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd == 3) {
            cycleEnd = 4;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
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
            iirAL = (iirAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
            double iirAmountBL = fabs(iirAL) + 0.00001;
            iirBL = (iirBL * (1.0 - iirAmountBL)) + (iirAL * iirAmountBL);
            inputSampleL -= iirBL;
            iirAR = (iirAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
            double iirAmountBR = fabs(iirAR) + 0.00001;
            iirBR = (iirBR * (1.0 - iirAmountBR)) + (iirAR * iirAmountBR);
            inputSampleR -= iirBR;
            // Console8 highpass
            if (cycleEnd == 4) {
                softL[8] = softL[7];
                softL[7] = softL[6];
                softL[6] = softL[5];
                softL[5] = softL[4];
                softL[4] = softL[3];
                softL[3] = softL[2];
                softL[2] = softL[1];
                softL[1] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[7];
                softR[7] = softR[6];
                softR[6] = softR[5];
                softR[5] = softR[4];
                softR[4] = softR[3];
                softR[3] = softR[2];
                softR[2] = softR[1];
                softR[1] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 2) {
                softL[8] = softL[6];
                softL[6] = softL[4];
                softL[4] = softL[2];
                softL[2] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[6];
                softR[6] = softR[4];
                softR[4] = softR[2];
                softR[2] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 1) {
                softL[8] = softL[4];
                softL[4] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[4];
                softR[4] = softR[0];
                softR[0] = inputSampleR;
            }
            softL[9] = ((softL[0] - softL[4]) - (softL[4] - softL[8]));
            if (softL[9] > 0.56852180) {
                inputSampleL = softL[4] + (fabs(softL[4]) * sin(softL[9] - 0.56852180) * 0.4314782);
            }
            if (-softL[9] > 0.56852180) {
                inputSampleL = softL[4] - (fabs(softL[4]) * sin(-softL[9] - 0.56852180) * 0.4314782);
            }
            // Console8 slew soften: must be clipped or it can generate NAN out of the full system
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            softR[9] = ((softR[0] - softR[4]) - (softR[4] - softR[8]));
            if (softR[9] > 0.56852180) {
                inputSampleR = softR[4] + (fabs(softR[4]) * sin(softR[9] - 0.56852180) * 0.4314782);
            }
            if (-softR[9] > 0.56852180) {
                inputSampleR = softR[4] - (fabs(softR[4]) * sin(-softR[9] - 0.56852180) * 0.4314782);
            }
            // Console8 slew soften: must be clipped or it can generate NAN out of the full system
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            if (hsr) {
                double outSample = (inputSampleL * fix[fix_a0]) + fix[fix_sL1];
                fix[fix_sL1] = (inputSampleL * fix[fix_a1]) - (outSample * fix[fix_b1]) + fix[fix_sL2];
                fix[fix_sL2] = (inputSampleL * fix[fix_a2]) - (outSample * fix[fix_b2]);
                inputSampleL = outSample;
                outSample = (inputSampleR * fix[fix_a0]) + fix[fix_sR1];
                fix[fix_sR1] = (inputSampleR * fix[fix_a1]) - (outSample * fix[fix_b1]) + fix[fix_sR2];
                fix[fix_sR2] = (inputSampleR * fix[fix_a2]) - (outSample * fix[fix_b2]);
                inputSampleR = outSample;
            } // fixed biquad filtering ultrasonics
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL = asin(inputSampleL); // Console8 decode
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR = asin(inputSampleR); // Console8 decode
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
} // namespace airwindohhs::console8bussin
