#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console8subhype {

constexpr std::string_view k_name{ "Console8SubHype" };
constexpr std::string_view k_short_description{
    "Console8SubHype swaps out for Console8SubIn, for a brighter sparklier circuit."
};
constexpr std::string_view k_long_description{
    "The original Console8 had a treble soften that was too weird to let people have. Used in every stage, it shaped the sound plenty, but it let through this weird airy brightness and hyper-focus that was far too extreme to have as the base Console sound. I tried reining it in with a dry/wet and that only created bizarre cancellations and other problems, and so I dug deeper and came up with a treble softening that played better at widely varying audio levels and gave the impact and solidity I needed. And the weird airy first version was gone forever.Here it is :)Here’s what you need to know: the plugins Console8ChannelHype, Console8SubHype, and Console8BussHype have no controls, and you can swap them out with Console8ChannelIn, Console8SubIn, and Console8BussIn. Those are the plugins that have the treble softening at each stage, and those are the ones you replace. The more stages you replace, the more hyped and trebly the result will be: maybe not to the extent of taking an EQ and cranking it, but the whole tonality is different. The In plugins have a solidness, more tough-sounding and physical: the Hype plugins are softer, more ethereal. They’re still manipulating the sound and are actually softening high-mids and treble, but the presence band is sticking out.Treat them like they’re preamp modules with a different circuit and its own sound, which you can use in three places on the resulting console. You can have them on individual channels for the most direct effect, or on the submixes where they’ll change the tone of the whole sub, or on the buss to take the entire mix and hype it in this way. If you’ve got a sound going through two or more stages of hype, it’ll be more obvious, and using it on later stages will tend to change the shape and color of the mix as a whole, not just the individual sounds.I can’t tell you how to incorporate these: it’s too Airwindowsy for that. Console8 is the latest Console made more direct with simpler, more obvious rules, but it doesn’t tell you how to mix, and when you include the Console8Hype plugins it becomes very difficult to explain what swapping them in will do. It depends on what your mix is, what you’re trying to do with it, and it’s heavily about textural values like solidness versus ethereal spaceyness, physical versus abstract, hard versus soft, not stuff you can just boil down to frequencies and DB values. It means direct access to tone options that are distinctly different, and applying them to not only sounds but the way the mix comes together. Hype on an individual track versus Hype on the buss are the same amount of ‘hype’, but totally different things in the final mix, and I can’t tell you which are going to be better for you.So I won’t. Console8Hype is your secret weapon and where you incorporate it is your affair: even other people knowing about Console8 might not hit upon the way you integrate Hype stages into the mix. This post may well drop out of sight quickly as Console8Hype is another layer of tricky on top of the already demanding Console8… and that’s fine. I’ll be using it, and I’ll answer questions about it, but not everybody needs to ‘get’ this."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console8SubHype final : public Effect<T>
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
    // default stuff

  public:
    Console8SubHype()
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
        fix[fix_reso] = 0.59435114;
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
            if (softL[9] < -1.57079633) {
                softL[9] = -1.57079633;
            }
            if (softL[9] > 1.57079633) {
                softL[9] = 1.57079633;
            }
            inputSampleL = softL[8] + (sin(softL[9]) * 0.61803398);
            softR[9] = ((softR[0] - softR[4]) - (softR[4] - softR[8]));
            if (softR[9] < -1.57079633) {
                softR[9] = -1.57079633;
            }
            if (softR[9] > 1.57079633) {
                softR[9] = 1.57079633;
            }
            inputSampleR = softR[8] + (sin(softR[9]) * 0.61803398);
            // treble softening effect ended up being an aural exciter
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
} // namespace airwindohhs::console8subhype
