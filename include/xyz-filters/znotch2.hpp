#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::znotch2 {

constexpr std::string_view k_name{ "ZNotch2" };
constexpr std::string_view k_short_description{
    "ZNotch2 acts more like the Emu e6400 Ultra phaser in motion, with coefficient interpolation."
};
constexpr std::string_view k_long_description{
    "This completes the reboot of my original Z-style filters! These are all based on study of a real hardware Emu e6400 sampler. (If my Patreon does well, I’ll be able to study more interesting hardware gear like I did this one). ZNotch is based on the ‘phaser’ settings, and ZNotch2 brings the coefficient interpolation people longed to have in this filter.Bear in mind, ZNotch also has its place. It’s meant to sound exactly the same (I may possibly have improved the tone a tiny bit with ZNotch2, not sure of that) but the original ZNotch does NOT have coefficient smoothing. That means it started out less than the real hardware filter, ‘not as good yet’, BUT if you’re not modulating the controls, you can get the same tone with a lot less CPU by choosing the Z filter that is not the 2 version. The originals will use less CPU because they’re not recalculating so much every sample, and that means you should probably have both installed if you like this type of filter. I demonstrate ZNotch2 on an electronic kick drum to great effect, and there’s nothing I’m doing there which I couldn’t do with the original ZNotch at lower CPU cost.That said, this one will move more fluidly, and the ‘Phaser’ sound is very special in motion! I hope you like it. We’re starting 2022 with the full DnB arsenal available ITB: Mackity, MackEQ, and the Z-style filters! I hope people have a lot of fun with this."
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class ZNotch2 final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleAR;
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_aA0,
        biq_aA1,
        biq_aA2,
        biq_bA1,
        biq_bA2,
        biq_aB0,
        biq_aB1,
        biq_aB2,
        biq_bB1,
        biq_bB2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    }; // coefficient interpolating biquad filter, stereo
    double biquadA[biq_total];
    double biquadB[biq_total];
    double biquadC[biq_total];
    double biquadD[biq_total];
    double inTrimA;
    double inTrimB;
    double outTrimA;
    double outTrimB;
    double wetA;
    double wetB;
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
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    ZNotch2()
    {
        A = 0.1;
        B = 0.5;
        C = 1.0;
        D = 0.5;
        iirSampleAL = 0.0;
        iirSampleAR = 0.0;
        for (int x = 0; x < biq_total; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
        }
        inTrimA = 0.1;
        inTrimB = 0.1;
        outTrimA = 1.0;
        outTrimB = 1.0;
        wetA = 0.5;
        wetB = 0.5;
        for (int x = 0; x < fix_total; x++) {
            fixA[x] = 0.0;
            fixB[x] = 0.0;
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
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.1;
            case kParamB: return 0.5;
            case kParamC: return 1.0;
            case kParamD: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input";
            case kParamB: return "freq";
            case kParamC: return "output";
            case kParamD: return "poles";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";
            case kParamB: return "Freq";
            case kParamC: return "Output";
            case kParamD: return "Poles";

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
            case kParamD: return std::to_string(D);

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
            case kParamD: return "";
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
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        biquadA[biq_freq] = ((pow(B, 3) * 4700.0) / getSampleRate()) + 0.0009963;
        double clipFactor = 0.91 - ((1.0 - B) * 0.15);
        biquadA[biq_reso] = 0.618033988749894848204586;
        biquadA[biq_aA0] = biquadA[biq_aB0];
        biquadA[biq_aA1] = biquadA[biq_aB1];
        biquadA[biq_aA2] = biquadA[biq_aB2];
        biquadA[biq_bA1] = biquadA[biq_bB1];
        biquadA[biq_bA2] = biquadA[biq_bB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        double K = tan(M_PI * biquadA[biq_freq]);
        double norm = 1.0 / (1.0 + K / biquadA[biq_reso] + K * K);
        biquadA[biq_aB0] = (1.0 + K * K) * norm;
        biquadA[biq_aB1] = 2.0 * (K * K - 1.0) * norm;
        biquadA[biq_aB2] = biquadA[biq_aB0];
        biquadA[biq_bB1] = biquadA[biq_aB1];
        biquadA[biq_bB2] = (1.0 - K / biquadA[biq_reso] + K * K) * norm;
        // opamp stuff
        inTrimA = inTrimB;
        inTrimB = A * 10.0;
        inTrimB *= inTrimB;
        inTrimB *= inTrimB;
        outTrimA = outTrimB;
        outTrimB = C * 10.0;
        wetA = wetB;
        wetB = pow(D, 2);
        double iirAmountA = 0.00069 / overallscale;
        fixA[fix_freq] = fixB[fix_freq] = 15500.0 / getSampleRate();
        fixA[fix_reso] = fixB[fix_reso] = 0.935;
        K = tan(M_PI * fixB[fix_freq]); // lowpass
        norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
        fixA[fix_a0] = fixB[fix_a0] = K * K * norm;
        fixA[fix_a1] = fixB[fix_a1] = 2.0 * fixB[fix_a0];
        fixA[fix_a2] = fixB[fix_a2] = fixB[fix_a0];
        fixA[fix_b1] = fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
        // end opamp stuff
        double trim = 0.1 + (3.712 * biquadA[biq_freq]);
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
            double overallDrySampleL = inputSampleL;
            double overallDrySampleR = inputSampleR;
            double outSample = (double)sampleFrames / inFramesToProcess;
            biquadA[biq_a0] = (biquadA[biq_aA0] * outSample) + (biquadA[biq_aB0] * (1.0 - outSample));
            biquadA[biq_a1] = (biquadA[biq_aA1] * outSample) + (biquadA[biq_aB1] * (1.0 - outSample));
            biquadA[biq_a2] = (biquadA[biq_aA2] * outSample) + (biquadA[biq_aB2] * (1.0 - outSample));
            biquadA[biq_b1] = (biquadA[biq_bA1] * outSample) + (biquadA[biq_bB1] * (1.0 - outSample));
            biquadA[biq_b2] = (biquadA[biq_bA2] * outSample) + (biquadA[biq_bB2] * (1.0 - outSample));
            for (int x = 0; x < 7; x++) {
                biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x];
            }
            // this is the interpolation code for the biquad
            double inTrim = (inTrimA * outSample) + (inTrimB * (1.0 - outSample));
            double outTrim = (outTrimA * outSample) + (outTrimB * (1.0 - outSample));
            double wet = (wetA * outSample) + (wetB * (1.0 - outSample));
            double aWet = 1.0;
            double bWet = 1.0;
            double cWet = 1.0;
            double dWet = wet * 4.0;
            // four-stage wet/dry control using progressive stages that bypass when not engaged
            if (dWet < 1.0) {
                aWet = dWet;
                bWet = 0.0;
                cWet = 0.0;
                dWet = 0.0;
            }
            else if (dWet < 2.0) {
                bWet = dWet - 1.0;
                cWet = 0.0;
                dWet = 0.0;
            }
            else if (dWet < 3.0) {
                cWet = dWet - 2.0;
                dWet = 0.0;
            }
            else {
                dWet -= 3.0;
            }
            // this is one way to make a little set of dry/wet stages that are successively added to the
            // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
            // beyond that point: this is a way to progressively add a 'black box' sound processing
            // which lets you fall through to simpler processing at lower settings.
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleL *= trim;
            inputSampleR *= trim;
            inputSampleL /= clipFactor;
            inputSampleR /= clipFactor;
            outSample = (inputSampleL * biquadA[biq_a0]) + biquadA[biq_sL1];
            if (outSample > 1.0) {
                outSample = 1.0;
            }
            if (outSample < -1.0) {
                outSample = -1.0;
            }
            biquadA[biq_sL1] = (inputSampleL * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sL2];
            biquadA[biq_sL2] = (inputSampleL * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
            drySampleL = inputSampleL = outSample;
            outSample = (inputSampleR * biquadA[biq_a0]) + biquadA[biq_sR1];
            if (outSample > 1.0) {
                outSample = 1.0;
            }
            if (outSample < -1.0) {
                outSample = -1.0;
            }
            biquadA[biq_sR1] = (inputSampleR * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sR2];
            biquadA[biq_sR2] = (inputSampleR * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
            drySampleR = inputSampleR = outSample;
            if (bWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = (inputSampleL * biquadB[biq_a0]) + biquadB[biq_sL1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadB[biq_sL1] = (inputSampleL * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sL2];
                biquadB[biq_sL2] = (inputSampleL * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
                drySampleL = inputSampleL = (outSample * bWet) + (drySampleL * (1.0 - bWet));
                inputSampleR /= clipFactor;
                outSample = (inputSampleR * biquadB[biq_a0]) + biquadB[biq_sR1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadB[biq_sR1] = (inputSampleR * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sR2];
                biquadB[biq_sR2] = (inputSampleR * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
                drySampleR = inputSampleR = (outSample * bWet) + (drySampleR * (1.0 - bWet));
            }
            if (cWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = (inputSampleL * biquadC[biq_a0]) + biquadC[biq_sL1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadC[biq_sL1] = (inputSampleL * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sL2];
                biquadC[biq_sL2] = (inputSampleL * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
                drySampleL = inputSampleL = (outSample * cWet) + (drySampleL * (1.0 - cWet));
                inputSampleR /= clipFactor;
                outSample = (inputSampleR * biquadC[biq_a0]) + biquadC[biq_sR1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadC[biq_sR1] = (inputSampleR * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sR2];
                biquadC[biq_sR2] = (inputSampleR * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
                drySampleR = inputSampleR = (outSample * cWet) + (drySampleR * (1.0 - cWet));
            }
            if (dWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = (inputSampleL * biquadD[biq_a0]) + biquadD[biq_sL1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadD[biq_sL1] = (inputSampleL * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sL2];
                biquadD[biq_sL2] = (inputSampleL * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
                drySampleL = inputSampleL = (outSample * dWet) + (drySampleL * (1.0 - dWet));
                inputSampleR /= clipFactor;
                outSample = (inputSampleR * biquadD[biq_a0]) + biquadD[biq_sR1];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadD[biq_sR1] = (inputSampleR * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sR2];
                biquadD[biq_sR2] = (inputSampleR * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
                drySampleR = inputSampleR = (outSample * dWet) + (drySampleR * (1.0 - dWet));
            }
            inputSampleL /= clipFactor;
            inputSampleR /= clipFactor;
            // opamp stage
            if (fabs(iirSampleAL) < 1.18e-37) {
                iirSampleAL = 0.0;
            }
            iirSampleAL = (iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
            inputSampleL -= iirSampleAL;
            if (fabs(iirSampleAR) < 1.18e-37) {
                iirSampleAR = 0.0;
            }
            iirSampleAR = (iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
            inputSampleR -= iirSampleAR;
            outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL -= (inputSampleL * inputSampleL * inputSampleL * inputSampleL * inputSampleL * 0.1768);
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR -= (inputSampleR * inputSampleR * inputSampleR * inputSampleR * inputSampleR * 0.1768);
            outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
            fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
            fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
            fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
            fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            if (outTrim != 1.0) {
                inputSampleL *= outTrim;
                inputSampleR *= outTrim;
            }
            // end opamp stage
            if (aWet != 1.0) {
                inputSampleL = (inputSampleL * aWet) + (overallDrySampleL * (1.0 - aWet));
                inputSampleR = (inputSampleR * aWet) + (overallDrySampleR * (1.0 - aWet));
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
} // namespace airwindohhs::znotch2
