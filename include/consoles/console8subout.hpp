#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console8subout {

constexpr std::string_view k_name{ "Console8SubOut" };
constexpr std::string_view k_short_description{
    "Console8SubOut moves to a channel/submix/buss topology and adds analog simulation."
};
constexpr std::string_view k_long_description{
    "Yep. Told you I had stuff in the works! ;)These are subtle things, not meant to be wildly wildly more colored and different from regular digital mix buss. It’s not like an amp sim. BUT, all the same, if you start to seriously listen to the texture and depth and how stuff communicates through Console8 relative to even Console7 much less a straight DAW mix, Console8 pretty much obliterates the rest. I made it because I needed it. I’m going to be using it, lots, and you can use it too: my plugins and digital things are free, Patreon-supported. These are tough times but that’s still working and has become all the more important to commit to.But what makes Console8 that much better?Ultrasonic filtering at 24k, distributed among more stages than ever before in a Console version, for extended highs (and it switches off at lower sample rates when it wouldn’t be helping, for lower CPU and more direct sound)Distributed highpass filtering emulating real analog gear. This is a new filter that doesn’t exist anywhere else and is only part of Console8. It’s more a texture/attitude filter than a sound-shaping filter, and is quite low but its effect is strongly felt.Distributed, gradiated treble softening across the whole mix topology. The idea is that your inputs are clean and low level, and as we go deeper into the console the levels rise as does the energy level, until the final mix buss is very powerful and pushing any electronics, transformers etc. quite hard. This gives Console8 the ability to do airy subtle sparkle and big roaring powerful energy all at the same time. It should just naturally happen through appropriate mixing. Think big, grand scale.A new mix topology with much simpler rules for how to set it up, using six dedicated plugins in six places.ChannelIn, SubIn, and BussIn go FIRST on your channels, submixes, and master buss.ChannelOut, SubOut, and BussOut go LAST on your channels, submixes, and master buss.All channels must go to a Submix with unity gain between the Out plugins and the In plugin on the submix.All submixes must go to the Master Buss with unity gain between the Out plugins and the In plugin on the master buss.The output of the BussOut goes directly to your converter, your file etc. and you’re done :)Don’t use any added processing between BussOut and the file or converter. It uses a version of ClipOnly2 plus a new wordlength reducer that doesn’t exist anywhere else and is a hybrid between my old Ten Nines process and Airwindows Dark. It only does 24 bit and if you must make 16 bit you can add a 16 bit dither (but you don’t need to make 16 bit, this is 2022, come on). This wordlength reducer is specially optimized for Console8, so if you’re using one of my existing Monitoring plugins that applies a wordlength reducer, switch it off when not in use (I’ll produce a ‘plain’ Monitoring that doesn’t interfere, though Monitoring2 is probably going to end up arriving at the same output as Console8BussOut)I realize this is a lot to take in but in practice it should all just work and fall into place very naturally. The Out plugins all have faders, which are good to assign to midi controllers so you can adjust them with physical controls and not the mouse. I have mine working with a TX-6 :) They’re designed so that 0.5 is unity: less than that fades down to zero, and more than that gives you about 12dB of boost in each stage. That’s also true for the master buss, except it also drives into the ClipOnly2 so it gets a punchier ‘slam’ than the channels and submixes get if you push those.It’s not really intended for ‘everything is louder than everything else’ overloudenated nonsense but will probably do that better than anything else you can have, especially if you’re trying to get a grand scale on things and not just ear-maiming grit :)Again, ‘In’ goes first. ‘Out’ goes last. All Channels go to a Sub, all Subs go to the Buss, and you’re done. Easy to explain :)"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console8SubOut final : public Effect<T>
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
    Console8SubOut()
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
        inTrimA = inTrimB;
        inTrimB = A * 2.0;
        // 0.5 is unity gain, and we can attenuate to silence or boost slightly over 12dB
        // into softclipping overdrive.
        if (Effect<T>::getSampleRate() > 49000.0) {
            hsr = true;
        }
        else {
            hsr = false;
        }
        fix[fix_freq] = 24000.0 / Effect<T>::getSampleRate();
        fix[fix_reso] = 1.20361562;
        double K = tan(M_PI * fix[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fix[fix_reso] + K * K);
        fix[fix_a0] = K * K * norm;
        fix[fix_a1] = 2.0 * fix[fix_a0];
        fix[fix_a2] = fix[fix_a0];
        fix[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fix[fix_b2] = (1.0 - K / fix[fix_reso] + K * K) * norm;
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
} // namespace airwindohhs::console8subout
