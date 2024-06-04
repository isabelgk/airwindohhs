#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console8litebuss {

constexpr std::string_view k_name{ "Console8LiteBuss" };
constexpr std::string_view k_short_description{
    "Console8LiteBuss is simplified Console8, working with just a single mix buss."
};
constexpr std::string_view k_long_description{
    "Here's a useful update on Console8! The original Console8 was unlike any other Airwindows Console, as it's designed to be used only in a Channel/Submix/Buss configuration. Its ultrasonic filtering needs to run through all three stages in order to be set up right. Yet, Console8 also introduced other things: a custom distributed highpass algorithm for acting more like real-world analog circuits, treble softening to mimic the use of transformers, and as a result you got a distinctive sound that wasn't present in more purist Console versions.Enter Console8Lite! Bear in mind that the full three-stage Console8 will still have a slight edge in certain ways: the interactions with the ultrasonic filtering are more intense, and things like the capacitor emulation work more effectively on the larger mix topology with more stages. But there's also a place for simpler, more direct mixers, and that's where Console8Lite comes in.It's designed from the ground up to bring all the Console8 features but in simpler, traditional Airwindows Console form. You put the channel plugins last on each channel, have the faders at unity gain, and put the buss plugin first on the buss. Everything else, from the custom highpassing to the treble softening to the ability to boost the buss output into an on-board version of ClipOnly2, is there in the simpler form, easier to set up and ready to bring analog tone to your DAW mix.When using it, set it up like any pre-8 version of Console. Use it as a replacement for Console8, though each plugin is roughly equivalent to a Console8 stage's In and Out run in series: if you mix and match it with Console8 full version it'll function but the ultrasonic filtering will no longer be calibrated.Hope you like the new, simpler and more direct Console8Lite :)"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console8LiteBuss final : public Effect<T>
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
    double fixB[fix_total];
    double softL[11];
    double softR[11];
    int cycleEnd;
    double inTrimA;
    double inTrimB;
    double lastSampleL;
    double intermediateL[18];
    bool wasPosClipL;
    bool wasNegClipL; // ClipOnly2
    double lastSampleR;
    double intermediateR[18];
    bool wasPosClipR;
    bool wasNegClipR;
    int spacing;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console8LiteBuss()
    {
        A = 0.5;
        iirAL = 0.0;
        iirBL = 0.0;
        iirAR = 0.0;
        iirBR = 0.0;
        for (int x = 0; x < fix_total; x++) {
            fix[x] = 0.0;
            fixB[x] = 0.0;
        }
        for (int x = 0; x < 10; x++) {
            softL[x] = 0.0;
            softR[x] = 0.0;
        }
        inTrimA = 0.5;
        inTrimB = 0.5;
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 17; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        } // ADClip2
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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "fader";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Fader";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double iirAmountA = 12.66 / getSampleRate();
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
        if (getSampleRate() > 49000.0) {
            hsr = true;
        }
        else {
            hsr = false;
        }
        fix[fix_freq] = 24000.0 / getSampleRate();
        fix[fix_reso] = 0.55495813;
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
        overallscale *= getSampleRate();
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
        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        inTrimA = inTrimB;
        inTrimB = A * 2.0;
        // 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
        // into softclip and ADClip in case we need intense loudness bursts on transients.
        fixB[fix_freq] = 24000.0 / getSampleRate();
        fixB[fix_reso] = 0.5;
        K = tan(M_PI * fixB[fix_freq]); // lowpass
        norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
        fixB[fix_a0] = K * K * norm;
        fixB[fix_a1] = 2.0 * fixB[fix_a0];
        fixB[fix_a2] = fixB[fix_a0];
        fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
        // this is the fixed biquad distributed anti-aliasing filter
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
                double outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
                fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
                fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
                inputSampleL = outSample;
                outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
                fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
                fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
                inputSampleR = outSample;
            } // fixed biquad filtering ultrasonics
            inputSampleL *= inTrim;
            inputSampleR *= inTrim;
            // the final output fader, before ClipOnly2
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::console8litebuss
