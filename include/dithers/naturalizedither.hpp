#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::naturalizedither {

constexpr std::string_view k_name{ "NaturalizeDither" };
constexpr std::string_view k_short_description{
    "NaturalizeDither is deterministic dither that uses Benford Realness calculations for each sample."
};
constexpr std::string_view k_long_description{
    "Last and definitely not least in the dither-fest: Naturalize! This dither is often considered best by listeners. It works on every genre, at every sample rate. It adds no tonal color to the audio, and the noise it produces is quiet… but more than quiet, the noise of Naturalize has a fugitive quality where it seems to ‘hide’ behind the reverb tails, and once you start listening to the music it’s difficult to hear there’s any noise present. The sense of reality produced by the music is compelling enough that it doesn’t draw attention to the noise floor at all.How can this be? I kept the secret for eight years but now that Airwindows is supported by a Patreon, I’m free to communicate how I did it, because my compensation is basically ‘the number of people out there who think I ought to keep working in this field’, and the more of those the better. I don’t have to keep Naturalize just to myself anymore, because it helps the Patreon when I’m generous. So, go ahead and download and use it for free (it’s running at 24 bit, use BitShiftGain to get 16 bit out of it) and here’s how Naturalize works.Benford’s Law is a statistical technique: basically, it says that in lists of natural data, if you get rid of all the leading zeroes, the first digit other than zero is most likely to be ‘one’. That’s no matter what scale the number is at: no matter how many zeroes you have to take away to get to the first number, it’s more likely to be ‘one’ than, say, ‘nine’.In fact, it’s ‘one’ thirty percent of the time, and the other leading digits are progressively rarer. How much rarer? I’ll give you a piece of the Naturalize code and the numbers I’m using.byn[0] = 1000;byn[1] = 301;byn[2] = 176;byn[3] = 125;byn[4] = 97;byn[5] = 79;byn[6] = 67;byn[7] = 58;byn[8] = 51;byn[9] = 46;This works on sample data that varies in logarithmic scale: fortunately, audio is already that way. Something like a reverb tail is perfect for Benford realness calculations! As the sound dies away, it SHOULD fall into just this sort of pattern: millions of sample numbers, scaling down to tiny near-zero values. It ought to be the perfect picture of Benford’s law.Here’s what Naturalize is. Every sample can be truncated in two directions: up or down, to the nearest value. So we do both, and then we run the Benford’s Law calculation on each, and we see which choice will get us closer to that perfect ‘realness’ distribution.Then we just choose that direction :) that’s it! Each sample, we always choose the direction that will bring us closer to Benford’s Law.This works on a statistical basis, to produce a collection of sample values that hold to what you’d get if you directly sampled ‘reality’. It erodes unnatural qualities in the digital audio, and it’s pleasing to listen to. There’s no excess energy to worry about: it’s always within one least significant bit of the target audio. And I might be able to improve upon it (I initially remembered it as having no noise sources at all, but it’s using some noise to shake up the values a bit for better distributing) but as 2017 begins, doing this gives you the best sounding dither in the world. I’d be happy to see if I can make it even better: support my Patreon, after all I came up with this thing in the first place, stands to reason I can develop it even further. Maybe I’ll make it entirely deterministic, with no noise at all. I’m sure I tried that already, but sometimes persistence pays off.(followup: Naturalize is the first example of what then became NJAD and went through a couple of revisions: the most current and best version of NJAD/Naturalize is the NJAD that is in the plugin Monitoring."
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class NaturalizeDither final : public Effect<T>
{
    double bynL[13];
    double bynR[13];
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    NaturalizeDither()
    {
        bynL[0] = 1000;
        bynL[1] = 301;
        bynL[2] = 176;
        bynL[3] = 125;
        bynL[4] = 97;
        bynL[5] = 79;
        bynL[6] = 67;
        bynL[7] = 58;
        bynL[8] = 51;
        bynL[9] = 46;
        bynL[10] = 1000;
        bynR[0] = 1000;
        bynR[1] = 301;
        bynR[2] = 176;
        bynR[3] = 125;
        bynR[4] = 97;
        bynR[5] = 79;
        bynR[6] = 67;
        bynR[7] = 58;
        bynR[8] = 51;
        bynR[9] = 46;
        bynR[10] = 1000;
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

        double inputSampleL;
        double inputSampleR;
        double benfordize;
        int hotbinA;
        int hotbinB;
        double totalA;
        double totalB;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            if (inputSampleL > 0) {
                inputSampleL += (0.3333333333);
            }
            if (inputSampleL < 0) {
                inputSampleL -= (0.3333333333);
            }
            inputSampleL += (double(fpdL) / UINT32_MAX) * 0.6666666666;
            if (inputSampleR > 0) {
                inputSampleR += (0.3333333333);
            }
            if (inputSampleR < 0) {
                inputSampleR -= (0.3333333333);
            }
            inputSampleR += (double(fpdR) / UINT32_MAX) * 0.6666666666;
            // begin L
            benfordize = floor(inputSampleL);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynL[hotbinA] += 1;
                totalA += (301 - bynL[1]);
                totalA += (176 - bynL[2]);
                totalA += (125 - bynL[3]);
                totalA += (97 - bynL[4]);
                totalA += (79 - bynL[5]);
                totalA += (67 - bynL[6]);
                totalA += (58 - bynL[7]);
                totalA += (51 - bynL[8]);
                totalA += (46 - bynL[9]);
                bynL[hotbinA] -= 1;
            }
            else {
                hotbinA = 10;
            }
            // produce total number- smaller is closer to Benford real
            benfordize = ceil(inputSampleL);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynL[hotbinB] += 1;
                totalB += (301 - bynL[1]);
                totalB += (176 - bynL[2]);
                totalB += (125 - bynL[3]);
                totalB += (97 - bynL[4]);
                totalB += (79 - bynL[5]);
                totalB += (67 - bynL[6]);
                totalB += (58 - bynL[7]);
                totalB += (51 - bynL[8]);
                totalB += (46 - bynL[9]);
                bynL[hotbinB] -= 1;
            }
            else {
                hotbinB = 10;
            }
            // produce total number- smaller is closer to Benford real
            if (totalA < totalB)
            {
                bynL[hotbinA] += 1;
                inputSampleL = floor(inputSampleL);
            }
            else
            {
                bynL[hotbinB] += 1;
                inputSampleL = ceil(inputSampleL);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            totalA = bynL[1] + bynL[2] + bynL[3] + bynL[4] + bynL[5] + bynL[6] + bynL[7] + bynL[8] + bynL[9];
            totalA /= 1000;
            totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
            bynL[1] /= totalA;
            bynL[2] /= totalA;
            bynL[3] /= totalA;
            bynL[4] /= totalA;
            bynL[5] /= totalA;
            bynL[6] /= totalA;
            bynL[7] /= totalA;
            bynL[8] /= totalA;
            bynL[9] /= totalA;
            bynL[10] /= 2; // catchall for garbage data
            // end L
            // begin R
            benfordize = floor(inputSampleR);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynR[hotbinA] += 1;
                totalA += (301 - bynR[1]);
                totalA += (176 - bynR[2]);
                totalA += (125 - bynR[3]);
                totalA += (97 - bynR[4]);
                totalA += (79 - bynR[5]);
                totalA += (67 - bynR[6]);
                totalA += (58 - bynR[7]);
                totalA += (51 - bynR[8]);
                totalA += (46 - bynR[9]);
                bynR[hotbinA] -= 1;
            }
            else {
                hotbinA = 10;
            }
            // produce total number- smaller is closer to Benford real
            benfordize = ceil(inputSampleR);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            if (benfordize < 1.0) {
                benfordize *= 10;
            }
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynR[hotbinB] += 1;
                totalB += (301 - bynR[1]);
                totalB += (176 - bynR[2]);
                totalB += (125 - bynR[3]);
                totalB += (97 - bynR[4]);
                totalB += (79 - bynR[5]);
                totalB += (67 - bynR[6]);
                totalB += (58 - bynR[7]);
                totalB += (51 - bynR[8]);
                totalB += (46 - bynR[9]);
                bynR[hotbinB] -= 1;
            }
            else {
                hotbinB = 10;
            }
            // produce total number- smaller is closer to Benford real
            if (totalA < totalB)
            {
                bynR[hotbinA] += 1;
                inputSampleR = floor(inputSampleR);
            }
            else
            {
                bynR[hotbinB] += 1;
                inputSampleR = ceil(inputSampleR);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            totalA = bynR[1] + bynR[2] + bynR[3] + bynR[4] + bynR[5] + bynR[6] + bynR[7] + bynR[8] + bynR[9];
            totalA /= 1000;
            totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
            bynR[1] /= totalA;
            bynR[2] /= totalA;
            bynR[3] /= totalA;
            bynR[4] /= totalA;
            bynR[5] /= totalA;
            bynR[6] /= totalA;
            bynR[7] /= totalA;
            bynR[8] /= totalA;
            bynR[9] /= totalA;
            bynR[10] /= 2; // catchall for garbage data
            // end R
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::naturalizedither
