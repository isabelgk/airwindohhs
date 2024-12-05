#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::wolfbot {

constexpr std::string_view k_name{ "Wolfbot" };
constexpr std::string_view k_short_description{
    "Wolfbot is an aggressive Kalman bandpass with evil in its heart."
};
constexpr std::string_view k_long_description{
    "So it's possible that I simply went too far.This was inspired by, and specifically modelled after, the famous transformer DI 'Wolfbox', which I don't have. I had only a youtube video made by somebody who did have a real Wolfbox DI (and at that, not a vintage one) and who was A/Bing it against a different DI. I'm putting this out because I was asked to, but understand that it's a science experiment.And by that I mean, literal science experiment. It answers the question, 'what happens when you use two Kalman filters to mimic the bandpassing of a vintage transformer DI box?' plus there's a bit of saturation on the end. The saturation is not the aggressive distortion you hear: that's the Kalman filters acting as comes naturally! It's possible that DI bass is the single worst signal you could try to put through this plugin that was designed to work on bass, and voiced using examples of bass and DI guitar. It's possible something has gone quite horribly wrong.Or.It's possible that this will come in handy more than anybody imagined. The thing is, I worked real hard to get the 'voice' of this dialed in just right, and THEN checked to see what it was really doing. It has a bit of saturation (should rein in snaps and pops nicely) but the grind it delivers isn't from that, it's from specifically the highpass Kalman filter, which is turning the whole sound into a sort of bass horn! I was shocked to see how much this strange plugin turns otherwise normal DI signals into sort of fat beefy pulse waves. Wolfbot is not gentle, and doesn't have any more controls than the original Wolfbox did, and while some kinds of sounds (drums, snares etc) get voiced in a convincingly 'bandpass' way, other sounds like the DIs it was designed to do, get utterly transformed.It acts more like a bass amp than a transformer DI, but more like some strange new invention than either… and the specific way it retains the hammering, brutal directness of bass low-mids, while wiping out irritating string-gloss (even on a Rickenbacker bridge pickup) and nuking ALL the real bass to make room for kick drums and sub-synths, means in a strange way Wolfbot entirely succeeded. I have a pile of amp-sims, multiple DIs and transformer DIs, and real amps, and none of them are even close to doing this, whatever 'this' is.I need to try it in some mixes, and so can you. I bet I can add deep-bass boosts and get something else out of it, but even just as it is, I can immediately hear how it would fill in a spot where bass guitars are supposed to go. It's just that rather than going crazy on the channel EQ, or running a bass amp and going crazy on that, it will just do that sound right away and it'll sit in an otherwise full-range mix reinforcing exactly what I want the bass guitar to reinforce.I've looked at the output in a wave editor and it's terrifying to think that this monstrous thing is probably my new go-to DI bass plugin, but here we are. Oh, and I bet you anything this makes basses project better on a phone. Have fun :)"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class Wolfbot final : public Effect<T>
{
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
    double kHP[kal_total];
    double kLP[kal_total];

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Wolfbot()
    {
        for (int x = 0; x < kal_total; x++) {
            kHP[x] = 0.0;
            kLP[x] = 0.0;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double kalHP = 1.0 - (0.004225 / overallscale);
        double kalLP = 1.0 - (0.954529 / overallscale);
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
            // begin Kalman Filter L
            double dryKal = inputSampleL = inputSampleL * (1.0 - kalHP) * 0.777;
            inputSampleL *= (1.0 - kalHP);
            // set up gain levels to control the beast
            kHP[prevSlewL3] += kHP[prevSampL3] - kHP[prevSampL2];
            kHP[prevSlewL3] *= 0.5;
            kHP[prevSlewL2] += kHP[prevSampL2] - kHP[prevSampL1];
            kHP[prevSlewL2] *= 0.5;
            kHP[prevSlewL1] += kHP[prevSampL1] - inputSampleL;
            kHP[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kHP[accSlewL2] += kHP[prevSlewL3] - kHP[prevSlewL2];
            kHP[accSlewL2] *= 0.5;
            kHP[accSlewL1] += kHP[prevSlewL2] - kHP[prevSlewL1];
            kHP[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kHP[accSlewL3] += (kHP[accSlewL2] - kHP[accSlewL1]);
            kHP[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kHP[kalOutL] += kHP[prevSampL1] + kHP[prevSlewL2] + kHP[accSlewL3];
            kHP[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kHP[kalGainL] += fabs(dryKal - kHP[kalOutL]) * kalHP * 8.0;
            kHP[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kHP[kalGainL] > kalHP * 0.5) {
                kHP[kalGainL] = kalHP * 0.5;
            }
            // attempts to avoid explosions
            kHP[kalOutL] += (dryKal * (1.0 - (0.68 + (kalHP * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kHP[prevSampL3] = kHP[prevSampL2];
            kHP[prevSampL2] = kHP[prevSampL1];
            kHP[prevSampL1] = (kHP[kalGainL] * kHP[kalOutL]) + ((1.0 - kHP[kalGainL]) * dryKal);
            // feed the chain of previous samples
            if (kHP[prevSampL1] > 1.0) {
                kHP[prevSampL1] = 1.0;
            }
            if (kHP[prevSampL1] < -1.0) {
                kHP[prevSampL1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleL = drySampleL + (kHP[kalOutL] * -0.777); // highpass
            // begin Kalman Filter L
            dryKal = inputSampleL = inputSampleL * (1.0 - kalLP) * 0.777;
            inputSampleL *= (1.0 - kalLP);
            // set up gain levels to control the beast
            kLP[prevSlewL3] += kLP[prevSampL3] - kLP[prevSampL2];
            kLP[prevSlewL3] *= 0.5;
            kLP[prevSlewL2] += kLP[prevSampL2] - kLP[prevSampL1];
            kLP[prevSlewL2] *= 0.5;
            kLP[prevSlewL1] += kLP[prevSampL1] - inputSampleL;
            kLP[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kLP[accSlewL2] += kLP[prevSlewL3] - kLP[prevSlewL2];
            kLP[accSlewL2] *= 0.5;
            kLP[accSlewL1] += kLP[prevSlewL2] - kLP[prevSlewL1];
            kLP[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kLP[accSlewL3] += (kLP[accSlewL2] - kLP[accSlewL1]);
            kLP[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kLP[kalOutL] += kLP[prevSampL1] + kLP[prevSlewL2] + kLP[accSlewL3];
            kLP[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kLP[kalGainL] += fabs(dryKal - kLP[kalOutL]) * kalLP * 8.0;
            kLP[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kLP[kalGainL] > kalLP * 0.5) {
                kLP[kalGainL] = kalLP * 0.5;
            }
            // attempts to avoid explosions
            kLP[kalOutL] += (dryKal * (1.0 - (0.68 + (kalLP * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kLP[prevSampL3] = kLP[prevSampL2];
            kLP[prevSampL2] = kLP[prevSampL1];
            kLP[prevSampL1] = (kLP[kalGainL] * kLP[kalOutL]) + ((1.0 - kLP[kalGainL]) * dryKal);
            // feed the chain of previous samples
            if (kLP[prevSampL1] > 1.0) {
                kLP[prevSampL1] = 1.0;
            }
            if (kLP[prevSampL1] < -1.0) {
                kLP[prevSampL1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleL = sin(kLP[kalOutL] * 0.7943) * 1.2589; // lowpass
            // begin Kalman Filter R
            dryKal = inputSampleR = inputSampleR * (1.0 - kalHP) * 0.777;
            inputSampleR *= (1.0 - kalHP);
            // set up gain levels to control the beast
            kHP[prevSlewR3] += kHP[prevSampR3] - kHP[prevSampR2];
            kHP[prevSlewR3] *= 0.5;
            kHP[prevSlewR2] += kHP[prevSampR2] - kHP[prevSampR1];
            kHP[prevSlewR2] *= 0.5;
            kHP[prevSlewR1] += kHP[prevSampR1] - inputSampleR;
            kHP[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kHP[accSlewR2] += kHP[prevSlewR3] - kHP[prevSlewR2];
            kHP[accSlewR2] *= 0.5;
            kHP[accSlewR1] += kHP[prevSlewR2] - kHP[prevSlewR1];
            kHP[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kHP[accSlewR3] += (kHP[accSlewR2] - kHP[accSlewR1]);
            kHP[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kHP[kalOutR] += kHP[prevSampR1] + kHP[prevSlewR2] + kHP[accSlewR3];
            kHP[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kHP[kalGainR] += fabs(dryKal - kHP[kalOutR]) * kalHP * 8.0;
            kHP[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kHP[kalGainR] > kalHP * 0.5) {
                kHP[kalGainR] = kalHP * 0.5;
            }
            // attempts to avoid explosions
            kHP[kalOutR] += (dryKal * (1.0 - (0.68 + (kalHP * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kHP[prevSampR3] = kHP[prevSampR2];
            kHP[prevSampR2] = kHP[prevSampR1];
            kHP[prevSampR1] = (kHP[kalGainR] * kHP[kalOutR]) + ((1.0 - kHP[kalGainR]) * dryKal);
            // feed the chain of previous samples
            if (kHP[prevSampR1] > 1.0) {
                kHP[prevSampR1] = 1.0;
            }
            if (kHP[prevSampR1] < -1.0) {
                kHP[prevSampR1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleR = drySampleR + (kHP[kalOutR] * -0.777); // highpass
            // begin Kalman Filter R
            dryKal = inputSampleR = inputSampleR * (1.0 - kalLP) * 0.777;
            inputSampleR *= (1.0 - kalLP);
            // set up gain levels to control the beast
            kLP[prevSlewR3] += kLP[prevSampR3] - kLP[prevSampR2];
            kLP[prevSlewR3] *= 0.5;
            kLP[prevSlewR2] += kLP[prevSampR2] - kLP[prevSampR1];
            kLP[prevSlewR2] *= 0.5;
            kLP[prevSlewR1] += kLP[prevSampR1] - inputSampleR;
            kLP[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kLP[accSlewR2] += kLP[prevSlewR3] - kLP[prevSlewR2];
            kLP[accSlewR2] *= 0.5;
            kLP[accSlewR1] += kLP[prevSlewR2] - kLP[prevSlewR1];
            kLP[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kLP[accSlewR3] += (kLP[accSlewR2] - kLP[accSlewR1]);
            kLP[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kLP[kalOutR] += kLP[prevSampR1] + kLP[prevSlewR2] + kLP[accSlewR3];
            kLP[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kLP[kalGainR] += fabs(dryKal - kLP[kalOutR]) * kalLP * 8.0;
            kLP[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kLP[kalGainR] > kalLP * 0.5) {
                kLP[kalGainR] = kalLP * 0.5;
            }
            // attempts to avoid explosions
            kLP[kalOutR] += (dryKal * (1.0 - (0.68 + (kalLP * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kLP[prevSampR3] = kLP[prevSampR2];
            kLP[prevSampR2] = kLP[prevSampR1];
            kLP[prevSampR1] = (kLP[kalGainR] * kLP[kalOutR]) + ((1.0 - kLP[kalGainR]) * dryKal);
            // feed the chain of previous samples
            if (kLP[prevSampR1] > 1.0) {
                kLP[prevSampR1] = 1.0;
            }
            if (kLP[prevSampR1] < -1.0) {
                kLP[prevSampR1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleR = sin(kLP[kalOutR] * 0.7943) * 1.2589; // lowpass
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
} // namespace airwindohhs::wolfbot
