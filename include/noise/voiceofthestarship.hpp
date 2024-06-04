#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::voiceofthestarship {

constexpr std::string_view k_name{ "VoiceOfTheStarship" };
constexpr std::string_view k_short_description{
    "VoiceOfTheStarship is a deep noise tone source."
};
constexpr std::string_view k_long_description{
    "This is the core of Noise, in maybe a more approachable form. The algorithm works like this: you do a random noise wander, where the audio output drifts around randomly with random amounts being added and subtracted from it. This is known as a random walk, and even if the amounts are equally positive and negative, the result will do two things: it'll give you more of a bassy rumble, and the loudest part of this bass will be a DC component. In other words, you can't use this directly for audio, because it won't make frequencies.Voice Of The Starship uses a flipping variable to alternately add and subtract the randomness, which is what makes the amounts equally positive and negative, but it does one more thing: it runs another variable so that, periodically, it'll take a moment to check whether the output is above or below zero… and at that moment it will ALWAYS use the randomness to move toward zero.This is enough to fix the DC problem, without adding a filter. (mind you, VOTS does also have lowpass filtering to help with the dark tonalities.) When you adjust Voice Of The Starship to have a less bassy noise, what you're doing is making it revert to zero more often, still with the simple randomness it's using.This lets you go from regular noise to deep dark noise to purely subsonic rumble. I also used this algorithm for background ambience in my game Counterpart. Now it’s open source under the MIT license, so other game projects can have algorithmic noise (better and more flexible than wave files)"
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class VoiceOfTheStarship final : public Effect<T>
{
    double noiseAL;
    double noiseBL;
    double noiseCL;
    double noiseAR;
    double noiseBR;
    double noiseCR;
    int position;
    int quadratic;
    bool flipL;
    bool flipR;
    bool filterflip;
    double bL[11];
    double bR[11];
    double f[11];
    int lastAlgorithm;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    VoiceOfTheStarship()
    {
        A = 0.5;
        B = 0.0;
        position = 99999999;
        quadratic = 0;
        noiseAL = 0.0;
        noiseBL = 0.0;
        noiseCL = 0.0;
        flipL = false;
        noiseAR = 0.0;
        noiseBR = 0.0;
        noiseCR = 0.0;
        flipR = false;
        filterflip = false;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
        }
        lastAlgorithm = 0;
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
        kNumParameters = 2

    };

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

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "filter";
            case kParamB: return "algrthm";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Filter";
            case kParamB: return "Algrthm";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(floor);

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

        double cutoff = pow((A * 0.89) + 0.1, 3);
        if (cutoff > 1.0) {
            cutoff = 1.0;
        }
        double invcutoff = 1.0 - cutoff;
        // this is the lowpass
        double overallscale = ((1.0 - A) * 9.0) + 1.0;
        double gain = overallscale;
        if (gain > 1.0) {
            f[0] = 1.0;
            gain -= 1.0;
        }
        else {
            f[0] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[1] = 1.0;
            gain -= 1.0;
        }
        else {
            f[1] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[2] = 1.0;
            gain -= 1.0;
        }
        else {
            f[2] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[3] = 1.0;
            gain -= 1.0;
        }
        else {
            f[3] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[4] = 1.0;
            gain -= 1.0;
        }
        else {
            f[4] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[5] = 1.0;
            gain -= 1.0;
        }
        else {
            f[5] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[6] = 1.0;
            gain -= 1.0;
        }
        else {
            f[6] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[7] = 1.0;
            gain -= 1.0;
        }
        else {
            f[7] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[8] = 1.0;
            gain -= 1.0;
        }
        else {
            f[8] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[9] = 1.0;
            gain -= 1.0;
        }
        else {
            f[9] = gain;
            gain = 0.0;
        }
        // this is the moving average with remainders
        if (overallscale < 1.0) {
            overallscale = 1.0;
        }
        f[0] /= overallscale;
        f[1] /= overallscale;
        f[2] /= overallscale;
        f[3] /= overallscale;
        f[4] /= overallscale;
        f[5] /= overallscale;
        f[6] /= overallscale;
        f[7] /= overallscale;
        f[8] /= overallscale;
        f[9] /= overallscale;
        // and now it's neatly scaled, too
        int lowcut = floor(B * 16.9);
        if (lastAlgorithm != lowcut) {
            noiseAL = 0.0;
            noiseBL = 0.0;
            noiseCL = 0.0;
            noiseAR = 0.0;
            noiseBR = 0.0;
            noiseCR = 0.0;
            for (int count = 0; count < 11; count++) {
                bL[count] = 0.0;
                bR[count] = 0.0;
            }
            lastAlgorithm = lowcut;
        }
        // cuts the noise back to 0 if we are changing algorithms,
        // because that also changes gains and can make loud pops.
        // We still get pops, but they'd be even worse
        int dcut;
        if (lowcut > 15) {
            lowcut = 1151;
            dcut = 11517;
        }
        if (lowcut == 15) {
            lowcut = 113;
            dcut = 1151;
        }
        if (lowcut == 14) {
            lowcut = 71;
            dcut = 719;
        }
        if (lowcut == 13) {
            lowcut = 53;
            dcut = 541;
        }
        if (lowcut == 12) {
            lowcut = 31;
            dcut = 311;
        }
        if (lowcut == 11) {
            lowcut = 23;
            dcut = 233;
        }
        if (lowcut == 10) {
            lowcut = 19;
            dcut = 191;
        }
        if (lowcut == 9) {
            lowcut = 17;
            dcut = 173;
        }
        if (lowcut == 8) {
            lowcut = 13;
            dcut = 131;
        }
        if (lowcut == 7) {
            lowcut = 11;
            dcut = 113;
        }
        if (lowcut == 6) {
            lowcut = 7;
            dcut = 79;
        }
        if (lowcut == 5) {
            lowcut = 6;
            dcut = 67;
        }
        if (lowcut == 4) {
            lowcut = 5;
            dcut = 59;
        }
        if (lowcut == 3) {
            lowcut = 4;
            dcut = 43;
        }
        if (lowcut == 2) {
            lowcut = 3;
            dcut = 37;
        }
        if (lowcut == 1) {
            lowcut = 2;
            dcut = 23;
        }
        if (lowcut < 1) {
            lowcut = 1;
            dcut = 11;
        }
        // this is the mechanism for cutting back subs without filtering
        double rumbletrim = sqrt(lowcut);
        // this among other things is just to give volume compensation
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            // we then ignore this!
            quadratic -= 1;
            if (quadratic < 0)
            {
                position += 1;
                quadratic = position * position;
                quadratic = quadratic % 170003; //% is C++ mod operator
                quadratic *= quadratic;
                quadratic = quadratic % 17011; //% is C++ mod operator
                quadratic *= quadratic;
                quadratic = quadratic % 1709; //% is C++ mod operator
                quadratic *= quadratic;
                quadratic = quadratic % dcut; //% is C++ mod operator
                quadratic *= quadratic;
                quadratic = quadratic % lowcut;
                // sets density of the centering force
                if (noiseAL < 0) {
                    flipL = true;
                }
                else {
                    flipL = false;
                }
                if (noiseAR < 0) {
                    flipR = true;
                }
                else {
                    flipR = false;
                }
                // every time we come here, we force the random walk to be
                // toward the center of the waveform. Without this,
                // it's a pure random walk that will generate DC.
            }
            if (flipL) {
                noiseAL += (double(fpdL) / UINT32_MAX);
            }
            else {
                noiseAL -= (double(fpdL) / UINT32_MAX);
            }
            if (flipR) {
                noiseAR += (double(fpdR) / UINT32_MAX);
            }
            else {
                noiseAR -= (double(fpdR) / UINT32_MAX);
            }
            // here's the guts of the random walk
            if (filterflip)
            {
                noiseBL *= invcutoff;
                noiseBL += (noiseAL * cutoff);
                inputSampleL = noiseBL;
                noiseBR *= invcutoff;
                noiseBR += (noiseAR * cutoff);
                inputSampleR = noiseBR;
            }
            else
            {
                noiseCL *= invcutoff;
                noiseCL += (noiseAL * cutoff);
                inputSampleL = noiseCL;
                noiseCR *= invcutoff;
                noiseCR += (noiseAR * cutoff);
                inputSampleR = noiseCR;
            }
            // now we have the output of the filter as inputSample.
            // this filter is shallower than a straight IIR: it's interleaved
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = inputSampleL;
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = inputSampleR;
            inputSampleL *= f[0];
            inputSampleL += (bL[1] * f[1]);
            inputSampleL += (bL[2] * f[2]);
            inputSampleL += (bL[3] * f[3]);
            inputSampleL += (bL[4] * f[4]);
            inputSampleL += (bL[5] * f[5]);
            inputSampleL += (bL[6] * f[6]);
            inputSampleL += (bL[7] * f[7]);
            inputSampleL += (bL[8] * f[8]);
            inputSampleL += (bL[9] * f[9]);
            inputSampleR *= f[0];
            inputSampleR += (bR[1] * f[1]);
            inputSampleR += (bR[2] * f[2]);
            inputSampleR += (bR[3] * f[3]);
            inputSampleR += (bR[4] * f[4]);
            inputSampleR += (bR[5] * f[5]);
            inputSampleR += (bR[6] * f[6]);
            inputSampleR += (bR[7] * f[7]);
            inputSampleR += (bR[8] * f[8]);
            inputSampleR += (bR[9] * f[9]);
            inputSampleL *= 0.1;
            inputSampleR *= 0.1;
            inputSampleL *= invcutoff;
            inputSampleR *= invcutoff;
            inputSampleL /= rumbletrim;
            inputSampleR /= rumbletrim;
            flipL = !flipL;
            flipR = !flipR;
            filterflip = !filterflip;
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::voiceofthestarship
