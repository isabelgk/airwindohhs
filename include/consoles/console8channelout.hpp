#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console8channelout {

constexpr std::string_view k_name{ "Console8ChannelOut" };
constexpr std::string_view k_short_description{
    "Console8ChannelOut moves to a channel/submix/buss topology and adds analog simulation."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console8ChannelOut final : public Effect<T>
{
    double inTrimA;
    double inTrimB;
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
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

    float A;

  public:
    Console8ChannelOut()
    {
        A = 0.5;
        inTrimA = 0.5;
        inTrimB = 0.5;
        for (int x = 0; x < fix_total; x++) {
            fix[x] = 0.0;
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
        kParamA = 0,
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "fader"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        if (Effect<T>::getSampleRate() > 49000.0) {
            hsr = true;
        }
        else {
            hsr = false;
        }
        fix[fix_freq] = 24000.0 / Effect<T>::getSampleRate();
        fix[fix_reso] = 3.51333709;
        double K = tan(M_PI * fix[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fix[fix_reso] + K * K);
        fix[fix_a0] = K * K * norm;
        fix[fix_a1] = 2.0 * fix[fix_a0];
        fix[fix_a2] = fix[fix_a0];
        fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fix[fix_b2] = (1.0 - K / fix[fix_reso] + K * K) * norm;
        // this is the fixed biquad distributed anti-aliasing filter
        inTrimA = inTrimB;
        inTrimB = A * 2.0;
        // 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
        // into softclipping overdrive.
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
            double position = (double)sampleFrames / inFramesToProcess;
            double inTrim = (inTrimA * position) + (inTrimB * (1.0 - position));
            // input trim smoothed to cut out zipper noise
            inputSampleL *= inTrim;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            // Console8 gain stage clips at exactly 1.0 post-sin()
            inputSampleR *= inTrim;
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // Console8 gain stage clips at exactly 1.0 post-sin()
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
            inputSampleL *= inTrim;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            // Console8 gain stage clips at exactly 1.0 post-sin()
            inputSampleR *= inTrim;
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // Console8 gain stage clips at exactly 1.0 post-sin()
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
} // namespace airwindohhs::console8channelout
