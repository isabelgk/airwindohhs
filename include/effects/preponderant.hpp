#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::preponderant {

constexpr std::string_view k_name{ "Preponderant" };
constexpr std::string_view k_short_description{
    "Preponderant is Airwindows Anti-Soothe, a strange tone intensifier and balancer."
};
constexpr std::string_view k_long_description{
    "Sometimes, it’s just… strange.Preponderant was created in an attempt to combine the old ResEQ concept with Soothe, in such a way that it’d give you three tightly controlled bands of emphasis (one thing Airwindows is all about is maximizing resonant sonority and intensity, not blindly removing it) and then also balance them on the fly so that all the bands would tend to be constantly active. This would bring up the high band, for instance, giving that ‘hyping of detail’ effect. In theory, it’d work.And maybe it does. But I don’t blame you if this one just drives you crazy and makes you mad. I had to scramble to get rid of an extra 24 dB of gain on tap for each band because when I started making the video, the plugin went insane and started blowing up: I thought I could let it throw in extra energy but I was sorely mistaken. One hasty bugfix later, we have… something? We have Preponderant. There is thankfully nothing else like it anywhere :DTurn up Resonance and Wet to hear what it’s focussing on. Use the Narrow, Medium and Wide controls to dial in frequency bands: each is twice as wide as the previous, so using Narrow will give you the tightest focus on a frequency range. Set ’em to areas in your sound (middle is the midrange). Preponderant is named because it can be tuned between areas of preponderant energy, and areas of power or areas of no energy, and it’ll compensate to give about the same output for each band you select, no matter how much energy is ‘supposed’ to be there. It’ll boost quietness, suppress intense resonant areas that are much louder than they should be, or you can simply tune to a different frequency range and avoid the resonant stuff.Also, it’s not a compressor. In no sense is it turning down louder stuff, or up quieter stuff. If you put it on drums you’ll scream in horror (unless you know exactly what you’re doing and have other drums to fill in a more continuous sonic flow). It’s rapidly balancing the bands while keeping the dynamics just as they are. Might be good on spot mics, toms or kick or something. Horrible on room mics or overall mics. Good on heavy guitar stems as it’ll retain dynamics there. It will be immediately obvious whether Preponderant is working on a track or stem.You HAVE to pick real audio for every band, or it will just make some up. Subsonics on a skinny guitar? It’ll turn down the other bands until it thinks they balance with whatever subsonics you selected. By the same token, ice-pick zone on the same guitar? It’ll make it balance with the other bands you chose.This will sound terrible. So, turn down the resonance until the sound is less insanely resonant. It’ll still sound terrible. Turn the dry/wet right back to dry (which will probably sound real boxy and dull by comparison).Then, feed in a little of the Preponderant, just until you start to hear it accentuate the stuff that you chose.THERE you go.Preponderant is one of those Airwindows audio chainsaws. The final version (without the 24 dB boosts) is intentionally made to just cut and restrict stuff, explicitly so if you don’t know what you’re doing you’ll hate it and not use it. This is intentional as you could hurt your mix, your ears, and your sensibilities.On the other hand, if you know how to dial in areas of power and focus and find three things about a sound that are useful in the fray of a mix, I’m not sure I’ve ever made a plugin that will so aggressively give you what you ask it for. (and if that’s not enough, a few of you will want to use the original version. I’m not going to encourage this, and you must choose: that or the real, volume-cutting version. They install with the same name and the same ID so you can’t have both, and shouldn’t have the boosty version, ever. But if you want to blow up your mix, then YES, you may. The rest of you, and me, please work with the one that doesn’t boost?)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Preponderant final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double bassBalanceL;
    double midBalanceL;
    double highBalanceL;
    double bassTrackL;
    double midTrackL;
    double highTrackL;
    double quicknessL;
    double bassBalanceR;
    double midBalanceR;
    double highBalanceR;
    double bassTrackR;
    double midTrackR;
    double highTrackR;
    double quicknessR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Preponderant()
    {
        A = 0.25;
        B = 0.5;
        C = 0.75;
        D = 0.5;
        E = 1.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
        }
        bassBalanceL = midBalanceL = highBalanceL = 1.0;
        bassTrackL = midTrackL = highTrackL = 0.0;
        quicknessL = 0.0001;
        bassBalanceR = midBalanceR = highBalanceR = 1.0;
        bassTrackR = midTrackR = highTrackR = 0.0;
        quicknessR = 0.0001;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
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
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;
        kParamE:
            return E;
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
            return 0.25;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.75;
            break;
        kParamD:
            return 0.5;
            break;
        kParamE:
            return 1.0;
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
            return "narrow";
            break;
        kParamB:
            return "medium";
            break;
        kParamC:
            return "wide";
            break;
        kParamD:
            return "resonan";
            break;
        kParamE:
            return "dry/wet";
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
            return "Narrow";
            break;
        kParamB:
            return "Medium";
            break;
        kParamC:
            return "Wide";
            break;
        kParamD:
            return "Resonan";
            break;
        kParamE:
            return "Dry/Wet";
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
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
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
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;
        kParamE:
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double chase = 0.00005 / overallscale;
        biquadA[0] = pow(A, (3.0 * cbrt(overallscale))) * 0.42;
        if (biquadA[0] < 0.0001) {
            biquadA[0] = 0.0001;
        }
        biquadB[0] = pow(B, (3.0 * cbrt(overallscale))) * 0.42;
        if (biquadB[0] < 0.0001) {
            biquadB[0] = 0.0001;
        }
        biquadC[0] = pow(C, (3.0 * cbrt(overallscale))) * 0.42;
        if (biquadC[0] < 0.0001) {
            biquadC[0] = 0.0001;
        }
        biquadA[1] = biquadB[1] = biquadC[1] = (pow(D, 3) * 8.0) + 0.33;
        biquadB[1] /= 2.0;
        biquadC[1] /= 4.0;
        double volumeCompensation = sqrt(biquadA[1]);
        double wet = E;
        double K = tan(M_PI * biquadA[0]);
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K / biquadA[1] * norm;
        biquadA[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
        biquadA[4] = -biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K / biquadB[1] * norm;
        biquadB[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
        biquadB[4] = -biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K / biquadC[1] * norm;
        biquadC[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
        biquadC[4] = -biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
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
            double drySampleL = *in1;
            double drySampleR = *in2;
            double tempSampleL = (inputSampleL * biquadA[2]) + biquadA[7];
            biquadA[7] = -(tempSampleL * biquadA[5]) + biquadA[8]; // BANDPASS form
            biquadA[8] = (inputSampleL * biquadA[4]) - (tempSampleL * biquadA[6]);
            double bassSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquadA[2]) + biquadA[9];
            biquadA[9] = -(tempSampleR * biquadA[5]) + biquadA[10]; // BANDPASS form
            biquadA[10] = (inputSampleR * biquadA[4]) - (tempSampleR * biquadA[6]);
            double bassSampleR = tempSampleR; // note: 9 and 10 store the R channel
            tempSampleL = (inputSampleL * biquadB[2]) + biquadB[7];
            biquadB[7] = -(tempSampleL * biquadB[5]) + biquadB[8]; // BANDPASS form
            biquadB[8] = (inputSampleL * biquadB[4]) - (tempSampleL * biquadB[6]);
            double midSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadB[2]) + biquadB[9];
            biquadB[9] = -(tempSampleR * biquadB[5]) + biquadB[10]; // BANDPASS form
            biquadB[10] = (inputSampleR * biquadB[4]) - (tempSampleR * biquadB[6]);
            double midSampleR = tempSampleR; // note: 9 and 10 store the R channel
            tempSampleL = (inputSampleL * biquadC[2]) + biquadC[7];
            biquadC[7] = -(tempSampleL * biquadC[5]) + biquadC[8]; // BANDPASS form
            biquadC[8] = (inputSampleL * biquadC[4]) - (tempSampleL * biquadC[6]);
            double highSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadC[2]) + biquadC[9];
            biquadC[9] = -(tempSampleR * biquadC[5]) + biquadC[10]; // BANDPASS form
            biquadC[10] = (inputSampleR * biquadC[4]) - (tempSampleR * biquadC[6]);
            double highSampleR = tempSampleR; // note: 9 and 10 store the R channel
            if (bassBalanceL < 0.00125) {
                bassBalanceL = 0.00125;
            }
            if (bassBalanceL > 1.0) {
                bassBalanceL = 1.0;
            }
            if (bassBalanceR < 0.00125) {
                bassBalanceR = 0.00125;
            }
            if (bassBalanceR > 1.0) {
                bassBalanceR = 1.0;
            }
            if (midBalanceL < 0.00125) {
                midBalanceL = 0.00125;
            }
            if (midBalanceL > 1.0) {
                midBalanceL = 1.0;
            }
            if (midBalanceR < 0.00125) {
                midBalanceR = 0.00125;
            }
            if (midBalanceR > 1.0) {
                midBalanceR = 1.0;
            }
            if (highBalanceL < 0.00125) {
                highBalanceL = 0.00125;
            }
            if (highBalanceL > 1.0) {
                highBalanceL = 1.0;
            }
            if (highBalanceR < 0.00125) {
                highBalanceR = 0.00125;
            }
            if (highBalanceR > 1.0) {
                highBalanceR = 1.0;
            }
            bassSampleL *= bassBalanceL;
            midSampleL *= midBalanceL;
            highSampleL *= highBalanceL;
            if (bassTrackL < fabs(bassSampleL)) {
                bassTrackL = fabs(bassSampleL);
            }
            if (midTrackL < fabs(midSampleL)) {
                midTrackL = fabs(midSampleL);
            }
            if (highTrackL < fabs(highSampleL)) {
                highTrackL = fabs(highSampleL);
            }
            if (bassTrackL > quicknessL) {
                bassTrackL -= quicknessL;
            }
            if (midTrackL > quicknessL) {
                midTrackL -= quicknessL;
            }
            if (highTrackL > quicknessL) {
                highTrackL -= quicknessL;
            }
            double disparity = 0.0;
            if (disparity < fabs(bassTrackL - midTrackL)) {
                disparity = fabs(bassTrackL - midTrackL);
            }
            if (disparity < fabs(bassTrackL - highTrackL)) {
                disparity = fabs(bassTrackL - highTrackL);
            }
            if (disparity < fabs(midTrackL - highTrackL)) {
                disparity = fabs(midTrackL - highTrackL);
            }
            quicknessL *= (1.0 - chase);
            quicknessL += ((1.0 + disparity) * 0.0001 * chase);
            if (bassTrackL > midTrackL) {
                bassBalanceL -= quicknessL;
                midBalanceL += quicknessL;
            }
            else {
                bassBalanceL += quicknessL;
                midBalanceL -= quicknessL;
            }
            if (midTrackL > highTrackL) {
                midBalanceL -= quicknessL;
                highBalanceL += quicknessL;
            }
            else {
                midBalanceL += quicknessL;
                highBalanceL -= quicknessL;
            }
            if (highTrackL > bassTrackL) {
                highBalanceL -= quicknessL;
                bassBalanceL += quicknessL;
            }
            else {
                highBalanceL += quicknessL;
                bassBalanceL -= quicknessL;
            }
            if (highBalanceL > 0.618033988749894848204586) {
                highBalanceL -= (quicknessL * 0.001);
            }
            else {
                highBalanceL += (quicknessL * 0.001);
            }
            if (midBalanceL > 0.618033988749894848204586) {
                midBalanceL -= (quicknessL * 0.001);
            }
            else {
                midBalanceL += (quicknessL * 0.001);
            }
            if (bassBalanceL > 0.618033988749894848204586) {
                bassBalanceL -= (quicknessL * 0.001);
            }
            else {
                bassBalanceL += (quicknessL * 0.001);
            }
            bassSampleR *= bassBalanceR;
            midSampleR *= midBalanceR;
            highSampleR *= highBalanceR;
            if (bassTrackR < fabs(bassSampleR)) {
                bassTrackR = fabs(bassSampleR);
            }
            if (midTrackR < fabs(midSampleR)) {
                midTrackR = fabs(midSampleR);
            }
            if (highTrackR < fabs(highSampleR)) {
                highTrackR = fabs(highSampleR);
            }
            if (bassTrackR > quicknessR) {
                bassTrackR -= quicknessR;
            }
            if (midTrackR > quicknessR) {
                midTrackR -= quicknessR;
            }
            if (highTrackR > quicknessR) {
                highTrackR -= quicknessR;
            }
            disparity = 0.0;
            if (disparity < fabs(bassTrackR - midTrackR)) {
                disparity = fabs(bassTrackR - midTrackR);
            }
            if (disparity < fabs(bassTrackR - highTrackR)) {
                disparity = fabs(bassTrackR - highTrackR);
            }
            if (disparity < fabs(midTrackR - highTrackR)) {
                disparity = fabs(midTrackR - highTrackR);
            }
            quicknessR *= (1.0 - chase);
            quicknessR += ((1.0 + disparity) * 0.0001 * chase);
            if (bassTrackR > midTrackR) {
                bassBalanceR -= quicknessR;
                midBalanceR += quicknessR;
            }
            else {
                bassBalanceR += quicknessR;
                midBalanceR -= quicknessR;
            }
            if (midTrackR > highTrackR) {
                midBalanceR -= quicknessR;
                highBalanceR += quicknessR;
            }
            else {
                midBalanceR += quicknessR;
                highBalanceR -= quicknessR;
            }
            if (highTrackR > bassTrackR) {
                highBalanceR -= quicknessR;
                bassBalanceR += quicknessR;
            }
            else {
                highBalanceR += quicknessR;
                bassBalanceR -= quicknessR;
            }
            if (highBalanceR > 0.618033988749894848204586) {
                highBalanceR -= (quicknessR * 0.001);
            }
            else {
                highBalanceR += (quicknessR * 0.001);
            }
            if (midBalanceR > 0.618033988749894848204586) {
                midBalanceR -= (quicknessR * 0.001);
            }
            else {
                midBalanceR += (quicknessR * 0.001);
            }
            if (bassBalanceR > 0.618033988749894848204586) {
                bassBalanceR -= (quicknessR * 0.001);
            }
            else {
                bassBalanceR += (quicknessR * 0.001);
            }
            inputSampleL = (bassSampleL + midSampleL + highSampleL) * volumeCompensation;
            inputSampleR = (bassSampleR + midSampleR + highSampleR) * volumeCompensation;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::preponderant
