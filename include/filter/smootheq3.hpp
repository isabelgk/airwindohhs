#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::smootheq3 {

constexpr std::string_view k_name{ "SmoothEQ3" };
constexpr std::string_view k_short_description{
    "SmoothEQ3 is the most approachable EQ."
};
constexpr std::string_view k_long_description{
    "By request from a patron, here's the EQ section out of ConsoleH, to use standalone in whatever way you like.What makes this special? It's a combination of tech things and design things. The idea for ConsoleH is to combine very tricky and complicated things like a four-band version of HipCrush, with a very basic approachable EQ of unbeatable quality so you can fall back on something that'll always sound good and work in an obvious way.SmoothEQ uses the technique from AngleEQ of constantly reconstructing the original sound from crossover filtering, which I originally came up with to deal with the weird phase behavior of AngleFilter. When you use that on more well-behaved filters like biquad filters, you're sort of protecting from issues that aren't there… but people noticed pretty quickly that the sound was there. SmoothEQ2, which is the backbone of the upcoming ConsoleX2, ran with this technology to allow for much steeper filtering, and created shelving filters that act like sweepable parametrics but still converge on ultimately clear, accurate EQ.SmoothEQ3 steps right back again to simplicity, approachability, with another twist: it goes for the combination of low CPU and steep crossovers. Working on other filters, I did a livestream where mathematically inclined viewers pitched in to set me up with the most accurate way to calculate the perfect IIR filter to match a Butterworth biquad filter. This takes the simplest form of configurable filter without extra resonance, and adds one layer of an even simpler filter at the exact frequency you need to make it steeper without it being obvious. Combining the two filter types kind of blends their sounds, between 'Butterworth' and a softer cutoff that's better for transients, to optimize both.Since it's not crazy steep it's a little bit like my Baxandall filters (a slow transition where transients are great but it's hard to tell where it transitions), so I was able to pick fixed frequencies to cross over at. Since the cutoff frequency is a little ambiguous thanks to the IIR section, it doesn't stand out, but since it's also steeper than a regular Butterworth filter you get to latch onto bass or treble and really boost or cut it without interfering with midrange. You end up with your basic three bands, immediately accessible, and they just do what you want without fuss.And since the IIR filter is very simple, SmoothEQ3 ends up being the most efficient way you can get a steeper-than-Butterworth EQ of this quality. Nearly anything else you could do, would cost more CPU to do the job. And that's also part of using it for ConsoleH: I've done everything I can to let people get big ConsoleH mixes on a potato if they have to, and SmoothEQ3 is the way to get ultimate EQ tone quality under those conditions. Something like SmoothEQ2, for ConsoleX2, is still not that expensive, but it's still doing 192 operations per sample per channel to do its four sophisticated bands of EQ.SmoothEQ3 does 59, including assignments (equals). That's more than three times as efficient than SmoothEQ2. It's partly because if you make a second-order filter this way one of the multiplies ends up being by 1.0 and you don't have to do it… the point is, not only does this EQ sound great but it's also incredibly efficient. And now that it's out in generic-plugin form, it's in Consolidated and in the VCV Rack version and you get to run it on whatever is even weaker than a potato: a lump of coal, maybe?Have fun with the EQ and I'll see ya in 2026 :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class SmoothEQ3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    }; // coefficient interpolating bessel filter, stereo
    double highFast[biq_total];
    double lowFast[biq_total];
    double highFastLIIR;
    double highFastRIIR;
    double lowFastLIIR;
    double lowFastRIIR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SmoothEQ3()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < biq_total; x++) {
            highFast[x] = 0.0;
            lowFast[x] = 0.0;
        }
        highFastLIIR = 0.0;
        highFastRIIR = 0.0;
        lowFastLIIR = 0.0;
        lowFastRIIR = 0.0;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "mid"; break;
            case kParamC: return "bass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "Mid"; break;
            case kParamC: return "Bass"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;

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

        double trebleGain = (A - 0.5) * 2.0;
        trebleGain = 1.0 + (trebleGain * fabs(trebleGain) * fabs(trebleGain));
        double midGain = (B - 0.5) * 2.0;
        midGain = 1.0 + (midGain * fabs(midGain) * fabs(midGain));
        double bassGain = (C - 0.5) * 2.0;
        bassGain = 1.0 + (bassGain * fabs(bassGain) * fabs(bassGain));
        // separate from filtering stage, this is amplitude, centered on 1.0 unity gain
        // SmoothEQ3 is how to get 3rd order steepness at very low CPU.
        // because sample rate varies, you could also vary the crossovers
        // you can't vary Q because math is simplified to take advantage of
        // how the accurate Q value for this filter is always exactly 1.0.
        highFast[biq_freq] = (4000.0 / Effect<T>::getSampleRate());
        double omega = 2.0 * M_PI * (4000.0 / Effect<T>::getSampleRate()); // mid-high crossover freq
        double K = 2.0 - cos(omega);
        double highCoef = -sqrt(K * K - 1.0) + K;
        lowFast[biq_freq] = (200.0 / Effect<T>::getSampleRate());
        omega = 2.0 * M_PI * (200.0 / Effect<T>::getSampleRate()); // low-mid crossover freq
        K = 2.0 - cos(omega);
        double lowCoef = -sqrt(K * K - 1.0) + K;
        // exponential IIR filter as part of an accurate 3rd order Butterworth filter
        K = tan(M_PI * highFast[biq_freq]);
        double norm = 1.0 / (1.0 + K + K * K);
        highFast[biq_a0] = K * K * norm;
        highFast[biq_a1] = 2.0 * highFast[biq_a0];
        highFast[biq_a2] = highFast[biq_a0];
        highFast[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        highFast[biq_b2] = (1.0 - K + K * K) * norm;
        K = tan(M_PI * lowFast[biq_freq]);
        norm = 1.0 / (1.0 + K + K * K);
        lowFast[biq_a0] = K * K * norm;
        lowFast[biq_a1] = 2.0 * lowFast[biq_a0];
        lowFast[biq_a2] = lowFast[biq_a0];
        lowFast[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        lowFast[biq_b2] = (1.0 - K + K * K) * norm;
        // custom biquad setup with Q = 1.0 gets to omit some divides
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double trebleFastL = inputSampleL;
            double outSample = (trebleFastL * highFast[biq_a0]) + highFast[biq_sL1];
            highFast[biq_sL1] = (trebleFastL * highFast[biq_a1]) - (outSample * highFast[biq_b1]) + highFast[biq_sL2];
            highFast[biq_sL2] = (trebleFastL * highFast[biq_a2]) - (outSample * highFast[biq_b2]);
            double midFastL = outSample;
            trebleFastL -= midFastL;
            outSample = (midFastL * lowFast[biq_a0]) + lowFast[biq_sL1];
            lowFast[biq_sL1] = (midFastL * lowFast[biq_a1]) - (outSample * lowFast[biq_b1]) + lowFast[biq_sL2];
            lowFast[biq_sL2] = (midFastL * lowFast[biq_a2]) - (outSample * lowFast[biq_b2]);
            double bassFastL = outSample;
            midFastL -= bassFastL;
            trebleFastL = (bassFastL * bassGain) + (midFastL * midGain) + (trebleFastL * trebleGain);
            // first stage of two crossovers is biquad of exactly 1.0 Q
            highFastLIIR = (highFastLIIR * highCoef) + (trebleFastL * (1.0 - highCoef));
            midFastL = highFastLIIR;
            trebleFastL -= midFastL;
            lowFastLIIR = (lowFastLIIR * lowCoef) + (midFastL * (1.0 - lowCoef));
            bassFastL = lowFastLIIR;
            midFastL -= bassFastL;
            inputSampleL = (bassFastL * bassGain) + (midFastL * midGain) + (trebleFastL * trebleGain);
            // second stage of two crossovers is the exponential filters
            // this produces a slightly steeper Butterworth filter very cheaply
            double trebleFastR = inputSampleR;
            outSample = (trebleFastR * highFast[biq_a0]) + highFast[biq_sR1];
            highFast[biq_sR1] = (trebleFastR * highFast[biq_a1]) - (outSample * highFast[biq_b1]) + highFast[biq_sR2];
            highFast[biq_sR2] = (trebleFastR * highFast[biq_a2]) - (outSample * highFast[biq_b2]);
            double midFastR = outSample;
            trebleFastR -= midFastR;
            outSample = (midFastR * lowFast[biq_a0]) + lowFast[biq_sR1];
            lowFast[biq_sR1] = (midFastR * lowFast[biq_a1]) - (outSample * lowFast[biq_b1]) + lowFast[biq_sR2];
            lowFast[biq_sR2] = (midFastR * lowFast[biq_a2]) - (outSample * lowFast[biq_b2]);
            double bassFastR = outSample;
            midFastR -= bassFastR;
            trebleFastR = (bassFastR * bassGain) + (midFastR * midGain) + (trebleFastR * trebleGain);
            // first stage of two crossovers is biquad of exactly 1.0 Q
            highFastRIIR = (highFastRIIR * highCoef) + (trebleFastR * (1.0 - highCoef));
            midFastR = highFastRIIR;
            trebleFastR -= midFastR;
            lowFastRIIR = (lowFastRIIR * lowCoef) + (midFastR * (1.0 - lowCoef));
            bassFastR = lowFastRIIR;
            midFastR -= bassFastR;
            inputSampleR = (bassFastR * bassGain) + (midFastR * midGain) + (trebleFastR * trebleGain);
            // second stage of two crossovers is the exponential filters
            // this produces a slightly steeper Butterworth filter very cheaply
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
} // namespace airwindohhs::smootheq3
