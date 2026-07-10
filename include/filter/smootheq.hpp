#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::smootheq {

constexpr std::string_view k_name{ "SmoothEQ" };
constexpr std::string_view k_short_description{
    "SmoothEQ is a sharp, accurate, transparent three-band filter."
};
constexpr std::string_view k_long_description{
    "This follows AngleEQ, so it follows an extremely weird, colorful, poorly behaved EQ with a super-clean, accurate, well-behaved EQ, just for variety. Not in the sense that you dial in exact dBs and frequencies: it's still a 'by ear' filter like the great analog EQs, and I'm probably not the person you go to for 'add exactly two and a half dB of 3026 hz shelving and that's the important part, all EQs sound the same'.When I say accurate, I mean that unlike AngleEQ if you set SmoothEQ flat you get EXACTLY what you put into it. It uses a subtractive technique that I like, not sure if other people do this, where the sound is assembled out of sub-bands that are always 'one band is the filtered, and the other is that subtracted from dry'. It's that which I couldn't do with AngleEQ, and which has been tricky with everything short of simple biquads (for which it works excellently).And this is the breakthrough where I worked out how to do this thing I like, which I'm not sure anyone cares about but me, with ANY degree or order of filters so long as they're biquads. Maybe even beyond that, but turns out biquad filters are very cooperative with this.SmoothEQ uses eighth-order crossovers: steep! They isolate real well. But so powerful is the technique that I can use Bessel filters instead of Butterworth, for nicer phase behavior. There are limits: I don't think I can use this for speaker crossovers, it's strictly for setting relative volumes of EQ bands and listening to a combined output. The trick is, if I apply cascaded filter crossovers and try to subtract it from dry, I get what I got in AngleEQ, a phasey mess.But if I reconstitute the original signal from the 'sections' between EVERY stage of filtering, I get the degree of filter steepness I wanted, AND all the sums still reconstitute to a perfect, bit-identical input if set flat.If you wanted an Airwindows five-band, or seven-band, or 31-band EQ this is how I'd have to go about it. I don't know if it would hold up at 31 bands of EQ, or how well that would work, but mathematically I know that if you set it flat you'd have ultimate, bit-identical, perfectly transparent sonics, because that's the only way the technique CAN work. There are obvious applications in making, for instance, a Mesa Boogie 5-band guitar EQ and having it sound amazing without analog modeling: sometimes what you want is the merits of analog processing, not just to imitate everything. It'll make EQs that are perfectly accurate set flat, and then you're shelving up and down bands of frequencies with extremely clean boundaries, as steep as you like.SmoothEQ is a simple three-band version of this. Now that I know I can do it, I can do a future ConsoleO (orchestral) and really do a good job of keeping the tone even through filtering. This is not a linear phase EQ but does not need to be, since everything is either perfect or some degree of the tone produced by extreme EQ isolations: whatever that sound, if you add only a tiny amount of filtering you automatically get only a tiny amount of that tone color. That's why I build it in that peculiar way, to get that result.It's going to be fun running with this technique and making stuff sound better, and I hope you like SmoothEQ :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class SmoothEQ final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
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
    double besselA[biq_total];
    double besselB[biq_total];
    double besselC[biq_total];
    double besselD[biq_total];
    double besselE[biq_total];
    double besselF[biq_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SmoothEQ()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.6;
        E = 0.4;
        for (int x = 0; x < biq_total; x++) {
            besselA[x] = 0.0;
            besselB[x] = 0.0;
            besselC[x] = 0.0;
            besselD[x] = 0.0;
            besselE[x] = 0.0;
            besselF[x] = 0.0;
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
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

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
            case kParamD: return 0.6; break;
            case kParamE: return 0.4; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "mid"; break;
            case kParamC: return "low"; break;
            case kParamD: return "xoverh"; break;
            case kParamE: return "xoverl"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "Mid"; break;
            case kParamC: return "Low"; break;
            case kParamD: return "XoverH"; break;
            case kParamE: return "XoverL"; break;

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
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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
        double trebleGain = A * 2.0;
        double midGain = B * 2.0;
        double bassGain = C * 2.0; // amount ends up being pow(gain,3)
        // simple three band to adjust
        besselA[biq_freq] = pow(D, 2) * (0.25 / overallscale) * 1.9047076123;
        if (besselA[biq_freq] < 0.00025) {
            besselA[biq_freq] = 0.00025;
        }
        if (besselA[biq_freq] > 0.4999) {
            besselA[biq_freq] = 0.4999;
        }
        besselA[biq_reso] = 1.02331395383;
        besselB[biq_freq] = pow(D, 2) * (0.25 / overallscale) * 1.68916826762;
        if (besselB[biq_freq] < 0.00025) {
            besselB[biq_freq] = 0.00025;
        }
        if (besselB[biq_freq] > 0.4999) {
            besselB[biq_freq] = 0.4999;
        }
        besselB[biq_reso] = 0.611194546878;
        besselC[biq_freq] = pow(D, 2) * (0.25 / overallscale) * 1.60391912877;
        if (besselC[biq_freq] < 0.00025) {
            besselC[biq_freq] = 0.00025;
        }
        if (besselC[biq_freq] > 0.4999) {
            besselC[biq_freq] = 0.4999;
        }
        besselC[biq_reso] = 0.510317824749;
        besselD[biq_freq] = pow(E, 4) * (0.25 / overallscale) * 1.9047076123;
        if (besselD[biq_freq] < 0.00025) {
            besselD[biq_freq] = 0.00025;
        }
        if (besselD[biq_freq] > 0.4999) {
            besselD[biq_freq] = 0.4999;
        }
        besselD[biq_reso] = 1.02331395383;
        besselE[biq_freq] = pow(E, 4) * (0.25 / overallscale) * 1.68916826762;
        if (besselE[biq_freq] < 0.00025) {
            besselE[biq_freq] = 0.00025;
        }
        if (besselE[biq_freq] > 0.4999) {
            besselE[biq_freq] = 0.4999;
        }
        besselE[biq_reso] = 0.611194546878;
        besselF[biq_freq] = pow(E, 4) * (0.25 / overallscale) * 1.60391912877;
        if (besselF[biq_freq] < 0.00025) {
            besselF[biq_freq] = 0.00025;
        }
        if (besselF[biq_freq] > 0.4999) {
            besselF[biq_freq] = 0.4999;
        }
        besselF[biq_reso] = 0.510317824749;
        double K = tan(M_PI * besselA[biq_freq]);
        double norm = 1.0 / (1.0 + K / besselA[biq_reso] + K * K);
        besselA[biq_a0] = K * K * norm;
        besselA[biq_a1] = 2.0 * besselA[biq_a0];
        besselA[biq_a2] = besselA[biq_a0];
        besselA[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselA[biq_b2] = (1.0 - K / besselA[biq_reso] + K * K) * norm;
        K = tan(M_PI * besselB[biq_freq]);
        norm = 1.0 / (1.0 + K / besselB[biq_reso] + K * K);
        besselB[biq_a0] = K * K * norm;
        besselB[biq_a1] = 2.0 * besselB[biq_a0];
        besselB[biq_a2] = besselB[biq_a0];
        besselB[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselB[biq_b2] = (1.0 - K / besselB[biq_reso] + K * K) * norm;
        K = tan(M_PI * besselC[biq_freq]);
        norm = 1.0 / (1.0 + K / besselC[biq_reso] + K * K);
        besselC[biq_a0] = K * K * norm;
        besselC[biq_a1] = 2.0 * besselC[biq_a0];
        besselC[biq_a2] = besselC[biq_a0];
        besselC[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselC[biq_b2] = (1.0 - K / besselC[biq_reso] + K * K) * norm;
        K = tan(M_PI * besselD[biq_freq]);
        norm = 1.0 / (1.0 + K / besselD[biq_reso] + K * K);
        besselD[biq_a0] = K * K * norm;
        besselD[biq_a1] = 2.0 * besselD[biq_a0];
        besselD[biq_a2] = besselD[biq_a0];
        besselD[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselD[biq_b2] = (1.0 - K / besselD[biq_reso] + K * K) * norm;
        K = tan(M_PI * besselE[biq_freq]);
        norm = 1.0 / (1.0 + K / besselE[biq_reso] + K * K);
        besselE[biq_a0] = K * K * norm;
        besselE[biq_a1] = 2.0 * besselE[biq_a0];
        besselE[biq_a2] = besselE[biq_a0];
        besselE[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselE[biq_b2] = (1.0 - K / besselE[biq_reso] + K * K) * norm;
        K = tan(M_PI * besselF[biq_freq]);
        norm = 1.0 / (1.0 + K / besselF[biq_reso] + K * K);
        besselF[biq_a0] = K * K * norm;
        besselF[biq_a1] = 2.0 * besselF[biq_a0];
        besselF[biq_a2] = besselF[biq_a0];
        besselF[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        besselF[biq_b2] = (1.0 - K / besselF[biq_reso] + K * K) * norm;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double trebleL = inputSampleL;
            double outSample = (trebleL * besselA[biq_a0]) + besselA[biq_sL1];
            besselA[biq_sL1] = (trebleL * besselA[biq_a1]) - (outSample * besselA[biq_b1]) + besselA[biq_sL2];
            besselA[biq_sL2] = (trebleL * besselA[biq_a2]) - (outSample * besselA[biq_b2]);
            double midL = outSample;
            trebleL -= midL;
            outSample = (midL * besselD[biq_a0]) + besselD[biq_sL1];
            besselD[biq_sL1] = (midL * besselD[biq_a1]) - (outSample * besselD[biq_b1]) + besselD[biq_sL2];
            besselD[biq_sL2] = (midL * besselD[biq_a2]) - (outSample * besselD[biq_b2]);
            double bassL = outSample;
            midL -= bassL;
            trebleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            outSample = (trebleL * besselB[biq_a0]) + besselB[biq_sL1];
            besselB[biq_sL1] = (trebleL * besselB[biq_a1]) - (outSample * besselB[biq_b1]) + besselB[biq_sL2];
            besselB[biq_sL2] = (trebleL * besselB[biq_a2]) - (outSample * besselB[biq_b2]);
            midL = outSample;
            trebleL -= midL;
            outSample = (midL * besselE[biq_a0]) + besselE[biq_sL1];
            besselE[biq_sL1] = (midL * besselE[biq_a1]) - (outSample * besselE[biq_b1]) + besselE[biq_sL2];
            besselE[biq_sL2] = (midL * besselE[biq_a2]) - (outSample * besselE[biq_b2]);
            bassL = outSample;
            midL -= bassL;
            trebleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            outSample = (trebleL * besselC[biq_a0]) + besselC[biq_sL1];
            besselC[biq_sL1] = (trebleL * besselC[biq_a1]) - (outSample * besselC[biq_b1]) + besselC[biq_sL2];
            besselC[biq_sL2] = (trebleL * besselC[biq_a2]) - (outSample * besselC[biq_b2]);
            midL = outSample;
            trebleL -= midL;
            outSample = (midL * besselF[biq_a0]) + besselF[biq_sL1];
            besselF[biq_sL1] = (midL * besselF[biq_a1]) - (outSample * besselF[biq_b1]) + besselF[biq_sL2];
            besselF[biq_sL2] = (midL * besselF[biq_a2]) - (outSample * besselF[biq_b2]);
            bassL = outSample;
            midL -= bassL;
            inputSampleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            double trebleR = inputSampleR;
            outSample = (trebleR * besselA[biq_a0]) + besselA[biq_sR1];
            besselA[biq_sR1] = (trebleR * besselA[biq_a1]) - (outSample * besselA[biq_b1]) + besselA[biq_sR2];
            besselA[biq_sR2] = (trebleR * besselA[biq_a2]) - (outSample * besselA[biq_b2]);
            double midR = outSample;
            trebleR -= midR;
            outSample = (midR * besselD[biq_a0]) + besselD[biq_sR1];
            besselD[biq_sR1] = (midR * besselD[biq_a1]) - (outSample * besselD[biq_b1]) + besselD[biq_sR2];
            besselD[biq_sR2] = (midR * besselD[biq_a2]) - (outSample * besselD[biq_b2]);
            double bassR = outSample;
            midR -= bassR;
            trebleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
            outSample = (trebleR * besselB[biq_a0]) + besselB[biq_sR1];
            besselB[biq_sR1] = (trebleR * besselB[biq_a1]) - (outSample * besselB[biq_b1]) + besselB[biq_sR2];
            besselB[biq_sR2] = (trebleR * besselB[biq_a2]) - (outSample * besselB[biq_b2]);
            midR = outSample;
            trebleR -= midR;
            outSample = (midR * besselE[biq_a0]) + besselE[biq_sR1];
            besselE[biq_sR1] = (midR * besselE[biq_a1]) - (outSample * besselE[biq_b1]) + besselE[biq_sR2];
            besselE[biq_sR2] = (midR * besselE[biq_a2]) - (outSample * besselE[biq_b2]);
            bassR = outSample;
            midR -= bassR;
            trebleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
            outSample = (trebleR * besselC[biq_a0]) + besselC[biq_sR1];
            besselC[biq_sR1] = (trebleR * besselC[biq_a1]) - (outSample * besselC[biq_b1]) + besselC[biq_sR2];
            besselC[biq_sR2] = (trebleR * besselC[biq_a2]) - (outSample * besselC[biq_b2]);
            midR = outSample;
            trebleR -= midR;
            outSample = (midR * besselF[biq_a0]) + besselF[biq_sR1];
            besselF[biq_sR1] = (midR * besselF[biq_a1]) - (outSample * besselF[biq_b1]) + besselF[biq_sR2];
            besselF[biq_sR2] = (midR * besselF[biq_a2]) - (outSample * besselF[biq_b2]);
            bassR = outSample;
            midR -= bassR;
            inputSampleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
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
} // namespace airwindohhs::smootheq
