#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestwarm2 {

constexpr std::string_view k_name{ "PurestWarm2" };
constexpr std::string_view k_short_description{
    "PurestWarm2 adds dual dry/wet controls for each side of the waveform."
};
constexpr std::string_view k_long_description{
    "PurestWarm2 is PurestWarm, but better. I was asked for a version of PurestWarm with a dry/wet (which would make it even more subtle and gentle) and then I thought, why not TWO dry/wets, one for either side of the waveform? That way you could massage both the positive and negative sides of the wave (seen briefly in the video: I wiggle the cursor around to show which side’s being reined in by which slider) to taste, or both at once.And so that’s what PurestWarm2 is (plus, ultrasonic filtering added for use at high sample rates). It’s also using a performance tweak I hit on, defining the filter coefficients when the plugin’s instantiated, rather than once every sample buffer. Not a complicated thing, but for anyone who made use of PurestWarm, this should count as a nice upgrade. And it’s a direct request: since I was asked for PurestWarm but with a dry/wet, how could I not?"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class PurestWarm2 final : public Effect<T>
{
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

        public :
            PurestWarm2(){
                A = 0.0;
                B = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
                // this is reset: values being initialized only once. Startup values, whatever they are.

            }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
            break;
        kParamB:
            return 0.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "dry/pos";
            break;
        kParamB:
            return "dry/neg";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Dry/Pos";
            break;
        kParamB:
            return "Dry/Neg";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;

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
} // namespace airwindohhs::purestwarm2
