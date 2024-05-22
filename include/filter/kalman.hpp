#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Kalman final : public Effect<T>
{
    std::string m_name{ "Kalman" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        prevSampL1,
        prevSlewL1,
        accSlewL1,
        prevSampL2,
        prevSlewL2,
        accSlewL2,
        prevSampL3,
        prevSlewL3,
        accSlewL3,
        kalGainL,
        kalOutL,
        prevSampR1,
        prevSlewR1,
        accSlewR1,
        prevSampR2,
        prevSlewR2,
        accSlewR2,
        prevSampR3,
        prevSlewR3,
        accSlewR3,
        kalGainR,
        kalOutR,
        kal_total
    };
    double kal[kal_total];
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    Kalman()
    {
        A = 0.5;
        B = 1.0;
        for (int x = 0; x < kal_total; x++) {
            kal[x] = 0.0;
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
            case kParamA: return "Kalman";
            case kParamB: return "Inv/Wet";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double kalman = 1.0 - pow(A, 2);
        double wet = (B * 2.0) - 1.0; // inv-dry-wet for highpass
        double dry = 2.0 - (B * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
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
            // begin Kalman Filter
            double dryKal = inputSampleL = inputSampleL * (1.0 - kalman) * 0.777;
            inputSampleL *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2];
            kal[prevSlewL3] *= 0.5;
            kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1];
            kal[prevSlewL2] *= 0.5;
            kal[prevSlewL1] += kal[prevSampL1] - inputSampleL;
            kal[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2];
            kal[accSlewL2] *= 0.5;
            kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1];
            kal[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]);
            kal[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3];
            kal[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainL] += fabs(dryKal - kal[kalOutL]) * kalman * 8.0;
            kal[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainL] > kalman * 0.5) {
                kal[kalGainL] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutL] += (dryKal * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampL3] = kal[prevSampL2];
            kal[prevSampL2] = kal[prevSampL1];
            kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0 - kal[kalGainL]) * dryKal);
            // feed the chain of previous samples
            if (kal[prevSampL1] > 1.0) {
                kal[prevSampL1] = 1.0;
            }
            if (kal[prevSampL1] < -1.0) {
                kal[prevSampL1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleL = (drySampleL * dry) + (kal[kalOutL] * wet * 0.777);
            // now the right channel
            dryKal = inputSampleR = inputSampleR * (1.0 - kalman) * 0.777;
            inputSampleR *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2];
            kal[prevSlewR3] *= 0.5;
            kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1];
            kal[prevSlewR2] *= 0.5;
            kal[prevSlewR1] += kal[prevSampR1] - inputSampleR;
            kal[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2];
            kal[accSlewR2] *= 0.5;
            kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1];
            kal[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]);
            kal[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3];
            kal[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainR] += fabs(dryKal - kal[kalOutR]) * kalman * 8.0;
            kal[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainR] > kalman * 0.5) {
                kal[kalGainR] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutR] += (dryKal * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampR3] = kal[prevSampR2];
            kal[prevSampR2] = kal[prevSampR1];
            kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0 - kal[kalGainR]) * dryKal);
            // feed the chain of previous samples
            if (kal[prevSampR1] > 1.0) {
                kal[prevSampR1] = 1.0;
            }
            if (kal[prevSampR1] < -1.0) {
                kal[prevSampR1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleR = (drySampleR * dry) + (kal[kalOutR] * wet * 0.777);
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
