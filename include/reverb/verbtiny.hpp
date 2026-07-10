#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::verbtiny {

constexpr std::string_view k_name{ "VerbTiny" };
constexpr std::string_view k_short_description{
    "VerbTiny is a classic artificial reverb that expands reverb shape."
};
constexpr std::string_view k_long_description{
    "So, why this much simpler reverb? Instead of the more elaborate ones I'm developing starting with the letter k?Because those ones (like kStation, kGuitarHall2 etc) are meant to simulate an acoustic space of various sorts. And that's interesting, and usually what I want if I'm using reverb for things. And there'll be more of those (for instance, kWoodRoom).But VerbTiny's made for different purposes. Rather than place sounds in a convincing acoustic environment, it's about merging itself with sounds to alter their texture. I was listening to some old dub techno and noticed that the reverb was… naive. Much like in the 80s and 90s, I was hearing a lot of fake reverb run without any predelay or anything sophisticated, just the 'tssshhhh' of an early reverb device.Then, experimenting with 4x4 matrices, I hit on one that was kind of special.It was just another 4x4 matrix, but run through my testing, it made a sound that was weirdly intense with peak energy, beyond anything else I'd ever created. Just a lucky break (through spending days and weeks and months using genetic algorithm to evolve millions and billions of possible reverb matrices, so it's not like it was only an accident). And that's what's in VerbTiny: that one algorithm for making a simple reverb.Twice.Because I'd had another idea: yes, it was gonna be 'VerbTiny' because it could sound good but the code would be way simpler than the main ones I'm developing. An example of super-low-CPU digital reverb, complete with my Bezier undersampling so it'd work on all sample rates, and also a Bezier filter so you could make it darker. But what if I ran another copy of it (since it's so simple) and made that one dual-mono?That would mean a wideness control, because it started out seeming kind of narrow (I can't control this, it's part of the algorithm). If there was a dual mono instance, each side would just feed back into the same side, meaning that stereo would stay as VERY wide stereo, at the cost of destroying the normal stereo image the reverb would have. And running two 4x4 reverbs isn't that demanding.And then, since the 'normal' stereo reverb was so narrow, that means it had a lot of energy in the mid channel even though it's two stereo channels. So, what if the wideness went from 'normal' to the dual mono one at the center (0.5) and then as you went beyond that, you brought in the original reverb again, but with one channel phase flipped? Then it'd all be side channel energy, against the dual mono.And so I did.So most of this reverb acts normally (the regeneration control is a Galactic-style 'replace', the Derez and Filter controls are strictly 'good sounding' Bezier filtering, and the dry wet is as you'd expect). It's designed to be largely an 'old school' reverb, and is in my Basic category in Consolidated because of how simple that is.And then the Wider control is still easy, but contains subtleties. If you have it exactly at 0, 0.5, or 1, you're wasting half the reverb (of course, you can do that if you want, I'm just saying). But adjust it, and listen to the shape of the 'space'. You'll find 0 is a normal stereo reverb, and then as you bring in the dual-mono version (set up so it blends nicely), the texture changes and becomes richer and it's like turning up a 'stereo wide' control. At 0.5 you have a weird wideness effect done in reverb alone. It's unnatural, but can be neat-sounding. And then as you continue to turn up Wider, you get into a HYPER-wide effect that also produces that richer tone, and it'll really accentuate the stereo space like nothing I've made (short of the Srsly plugins, which you could put on the end of this just for overkill).I'm looking at making one of these which is the opposite tone: dense, thick, and soupy, for adding body to the sound, rather than depth and spaciousness. This one is for artificial depth and space, but in a way that's more retro and less realistic.Hope you like it! Again, I would look for 'blends' of normal and dual-mono, either between 0 and 0.5, or between 0.5 and 1. That way you're using both of the reverbs inside. But any way you use this is fine. I'm looking forward to playing with it myself once I've finished some more plugins, perhaps on my Bastl Kastle-based jams. It's intentionally not like my other reverbs, but for some things it is just what I need :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class VerbTiny final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'vtny';
    static constexpr int d4A = 136;
    static constexpr int d4B = 52;
    static constexpr int d4C = 53;
    static constexpr int d4D = 1261;
    static constexpr int d4E = 209;
    static constexpr int d4F = 473;
    static constexpr int d4G = 549;
    static constexpr int d4H = 29;
    static constexpr int d4I = 92;
    static constexpr int d4J = 1137;
    static constexpr int d4K = 1406;
    static constexpr int d4L = 994;
    static constexpr int d4M = 1314;
    static constexpr int d4N = 191;
    static constexpr int d4O = 1263;
    static constexpr int d4P = 103;
    float A;
    float B;
    float C;
    float D;
    float E;
    double a4AL[d4A + 5];
    double a4BL[d4B + 5];
    double a4CL[d4C + 5];
    double a4DL[d4D + 5];
    double a4EL[d4E + 5];
    double a4FL[d4F + 5];
    double a4GL[d4G + 5];
    double a4HL[d4H + 5];
    double a4IL[d4I + 5];
    double a4JL[d4J + 5];
    double a4KL[d4K + 5];
    double a4LL[d4L + 5];
    double a4ML[d4M + 5];
    double a4NL[d4N + 5];
    double a4OL[d4O + 5];
    double a4PL[d4P + 5];
    double a4AR[d4A + 5];
    double a4BR[d4B + 5];
    double a4CR[d4C + 5];
    double a4DR[d4D + 5];
    double a4ER[d4E + 5];
    double a4FR[d4F + 5];
    double a4GR[d4G + 5];
    double a4HR[d4H + 5];
    double a4IR[d4I + 5];
    double a4JR[d4J + 5];
    double a4KR[d4K + 5];
    double a4LR[d4L + 5];
    double a4MR[d4M + 5];
    double a4NR[d4N + 5];
    double a4OR[d4O + 5];
    double a4PR[d4P + 5];
    int c4AL, c4BL, c4CL, c4DL, c4EL, c4FL, c4GL, c4HL;
    int c4IL, c4JL, c4KL, c4LL, c4ML, c4NL, c4OL, c4PL;
    int c4AR, c4BR, c4CR, c4DR, c4ER, c4FR, c4GR, c4HR;
    int c4IR, c4JR, c4KR, c4LR, c4MR, c4NR, c4OR, c4PR;
    double f4AL, f4BL, f4CL, f4DL, f4DR, f4HR, f4LR, f4PR;
    // base stereo reverb
    double b4AL[d4A + 5];
    double b4BL[d4B + 5];
    double b4CL[d4C + 5];
    double b4DL[d4D + 5];
    double b4EL[d4E + 5];
    double b4FL[d4F + 5];
    double b4GL[d4G + 5];
    double b4HL[d4H + 5];
    double b4IL[d4I + 5];
    double b4JL[d4J + 5];
    double b4KL[d4K + 5];
    double b4LL[d4L + 5];
    double b4ML[d4M + 5];
    double b4NL[d4N + 5];
    double b4OL[d4O + 5];
    double b4PL[d4P + 5];
    double b4AR[d4A + 5];
    double b4BR[d4B + 5];
    double b4CR[d4C + 5];
    double b4DR[d4D + 5];
    double b4ER[d4E + 5];
    double b4FR[d4F + 5];
    double b4GR[d4G + 5];
    double b4HR[d4H + 5];
    double b4IR[d4I + 5];
    double b4JR[d4J + 5];
    double b4KR[d4K + 5];
    double b4LR[d4L + 5];
    double b4MR[d4M + 5];
    double b4NR[d4N + 5];
    double b4OR[d4O + 5];
    double b4PR[d4P + 5];
    int e4AL, e4BL, e4CL, e4DL, e4EL, e4FL, e4GL, e4HL;
    int e4IL, e4JL, e4KL, e4LL, e4ML, e4NL, e4OL, e4PL;
    int e4AR, e4BR, e4CR, e4DR, e4ER, e4FR, e4GR, e4HR;
    int e4IR, e4JR, e4KR, e4LR, e4MR, e4NR, e4OR, e4PR;
    double g4AL, g4BL, g4CL, g4DL, g4DR, g4HR, g4LR, g4PR;
    // changed letter is the dual mono, with rearranged grid
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];
    double bezF[bez_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    VerbTiny()
    {
        A = 0.5;
        B = 1.0;
        C = 1.0;
        D = 0.0;
        E = 1.0;
        for (int x = 0; x < d4A + 2; x++) {
            a4AL[x] = 0.0;
            a4AR[x] = 0.0;
        }
        for (int x = 0; x < d4B + 2; x++) {
            a4BL[x] = 0.0;
            a4BR[x] = 0.0;
        }
        for (int x = 0; x < d4C + 2; x++) {
            a4CL[x] = 0.0;
            a4CR[x] = 0.0;
        }
        for (int x = 0; x < d4D + 2; x++) {
            a4DL[x] = 0.0;
            a4DR[x] = 0.0;
        }
        for (int x = 0; x < d4E + 2; x++) {
            a4EL[x] = 0.0;
            a4ER[x] = 0.0;
        }
        for (int x = 0; x < d4F + 2; x++) {
            a4FL[x] = 0.0;
            a4FR[x] = 0.0;
        }
        for (int x = 0; x < d4G + 2; x++) {
            a4GL[x] = 0.0;
            a4GR[x] = 0.0;
        }
        for (int x = 0; x < d4H + 2; x++) {
            a4HL[x] = 0.0;
            a4HR[x] = 0.0;
        }
        for (int x = 0; x < d4I + 2; x++) {
            a4IL[x] = 0.0;
            a4IR[x] = 0.0;
        }
        for (int x = 0; x < d4J + 2; x++) {
            a4JL[x] = 0.0;
            a4JR[x] = 0.0;
        }
        for (int x = 0; x < d4K + 2; x++) {
            a4KL[x] = 0.0;
            a4KR[x] = 0.0;
        }
        for (int x = 0; x < d4L + 2; x++) {
            a4LL[x] = 0.0;
            a4LR[x] = 0.0;
        }
        for (int x = 0; x < d4M + 2; x++) {
            a4ML[x] = 0.0;
            a4MR[x] = 0.0;
        }
        for (int x = 0; x < d4N + 2; x++) {
            a4NL[x] = 0.0;
            a4NR[x] = 0.0;
        }
        for (int x = 0; x < d4O + 2; x++) {
            a4OL[x] = 0.0;
            a4OR[x] = 0.0;
        }
        for (int x = 0; x < d4P + 2; x++) {
            a4PL[x] = 0.0;
            a4PR[x] = 0.0;
        }
        c4AL = c4BL = c4CL = c4DL = c4EL = c4FL = c4GL = c4HL = 1;
        c4IL = c4JL = c4KL = c4LL = c4ML = c4NL = c4OL = c4PL = 1;
        c4AR = c4BR = c4CR = c4DR = c4ER = c4FR = c4GR = c4HR = 1;
        c4IR = c4JR = c4KR = c4LR = c4MR = c4NR = c4OR = c4PR = 1;
        f4AL = f4BL = f4CL = f4DL = 0.0;
        f4DR = f4HR = f4LR = f4PR = 0.0;
        for (int x = 0; x < d4A + 2; x++) {
            b4AL[x] = 0.0;
            b4AR[x] = 0.0;
        }
        for (int x = 0; x < d4B + 2; x++) {
            b4BL[x] = 0.0;
            b4BR[x] = 0.0;
        }
        for (int x = 0; x < d4C + 2; x++) {
            b4CL[x] = 0.0;
            b4CR[x] = 0.0;
        }
        for (int x = 0; x < d4D + 2; x++) {
            b4DL[x] = 0.0;
            b4DR[x] = 0.0;
        }
        for (int x = 0; x < d4E + 2; x++) {
            b4EL[x] = 0.0;
            b4ER[x] = 0.0;
        }
        for (int x = 0; x < d4F + 2; x++) {
            b4FL[x] = 0.0;
            b4FR[x] = 0.0;
        }
        for (int x = 0; x < d4G + 2; x++) {
            b4GL[x] = 0.0;
            b4GR[x] = 0.0;
        }
        for (int x = 0; x < d4H + 2; x++) {
            b4HL[x] = 0.0;
            b4HR[x] = 0.0;
        }
        for (int x = 0; x < d4I + 2; x++) {
            b4IL[x] = 0.0;
            b4IR[x] = 0.0;
        }
        for (int x = 0; x < d4J + 2; x++) {
            b4JL[x] = 0.0;
            b4JR[x] = 0.0;
        }
        for (int x = 0; x < d4K + 2; x++) {
            b4KL[x] = 0.0;
            b4KR[x] = 0.0;
        }
        for (int x = 0; x < d4L + 2; x++) {
            b4LL[x] = 0.0;
            b4LR[x] = 0.0;
        }
        for (int x = 0; x < d4M + 2; x++) {
            b4ML[x] = 0.0;
            b4MR[x] = 0.0;
        }
        for (int x = 0; x < d4N + 2; x++) {
            b4NL[x] = 0.0;
            b4NR[x] = 0.0;
        }
        for (int x = 0; x < d4O + 2; x++) {
            b4OL[x] = 0.0;
            b4OR[x] = 0.0;
        }
        for (int x = 0; x < d4P + 2; x++) {
            b4PL[x] = 0.0;
            b4PR[x] = 0.0;
        }
        e4AL = e4BL = e4CL = e4DL = e4EL = e4FL = e4GL = e4HL = 1;
        e4IL = e4JL = e4KL = e4LL = e4ML = e4NL = e4OL = e4PL = 1;
        e4AR = e4BR = e4CR = e4DR = e4ER = e4FR = e4GR = e4HR = 1;
        e4IR = e4JR = e4KR = e4LR = e4MR = e4NR = e4OR = e4PR = 1;
        g4AL = g4BL = g4CL = g4DL = 0.0;
        g4DR = g4HR = g4LR = g4PR = 0.0;
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
            bezF[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
        bezF[bez_cycle] = 1.0;
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
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "replace"; break;
            case kParamB: return "derez"; break;
            case kParamC: return "filter"; break;
            case kParamD: return "wider"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Replace"; break;
            case kParamB: return "Derez"; break;
            case kParamC: return "Filter"; break;
            case kParamD: return "Wider"; break;
            case kParamE: return "Dry/Wet"; break;

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
        double reg4n = 0.03125 + ((1.0 - pow(1.0 - A, 2.0)) * 0.03125);
        double attenuate = 1.0 - (1.0 - pow(1.0 - A, 2.0));
        double derez = pow(B, 2.0);
        derez = fmin(fmax(derez / overallscale, 0.0001), 1.0);
        int bezFraction = (int)(1.0 / derez);
        double bezTrim = (double)bezFraction / (bezFraction + 1.0);
        derez = 1.0 / bezFraction;
        bezTrim = 1.0 - (derez * bezTrim);
        // the revision more accurately connects the bezier curves
        double derezFreq = pow(C, 2.0);
        derezFreq = fmin(fmax(derezFreq / overallscale, 0.0001), 1.0);
        int bezFreqFraction = (int)(1.0 / derezFreq);
        double bezFreqTrim = (double)bezFreqFraction / (bezFreqFraction + 1.0);
        derezFreq = 1.0 / bezFreqFraction;
        bezFreqTrim = 1.0 - (derezFreq * bezFreqTrim);
        // the revision more accurately connects the bezier curves
        double wider = D * 2.0;
        double wet = E;
        while (--sampleFrames >= 0) {
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
            bez[bez_cycle] += derez;
            bez[bez_SampL] += (inputSampleL * attenuate * derez);
            bez[bez_SampR] += (inputSampleR * attenuate * derez);
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bez[bez_cycle] = 0.0;
                double mainSampleL = bez[bez_SampL];
                double dualmonoSampleL = bez[bez_SampR];
                // workaround involves keeping the cross-matrix system,
                // but for initial layering, each side gets each version
                // making blends never quite line up as exactly the same.
                // left verbs
                a4AL[c4AL] = mainSampleL + (f4DR * reg4n);
                a4BL[c4BL] = mainSampleL + (f4HR * reg4n);
                a4CL[c4CL] = mainSampleL + (f4LR * reg4n);
                a4DL[c4DL] = mainSampleL + (f4PR * reg4n);
                b4AL[c4AL] = dualmonoSampleL + (g4AL * reg4n);
                b4BL[c4BL] = dualmonoSampleL + (g4BL * reg4n);
                b4CL[c4CL] = dualmonoSampleL + (g4CL * reg4n);
                b4DL[c4DL] = dualmonoSampleL + (g4DL * reg4n);
                c4AL++;
                if (c4AL < 0 || c4AL > d4A) {
                    c4AL = 0;
                }
                c4BL++;
                if (c4BL < 0 || c4BL > d4B) {
                    c4BL = 0;
                }
                c4CL++;
                if (c4CL < 0 || c4CL > d4C) {
                    c4CL = 0;
                }
                c4DL++;
                if (c4DL < 0 || c4DL > d4D) {
                    c4DL = 0;
                }
                double hA = a4AL[c4AL - ((c4AL > d4A) ? d4A + 1 : 0)];
                double hB = a4BL[c4BL - ((c4BL > d4B) ? d4B + 1 : 0)];
                double hC = a4CL[c4CL - ((c4CL > d4C) ? d4C + 1 : 0)];
                double hD = a4DL[c4DL - ((c4DL > d4D) ? d4D + 1 : 0)];
                a4EL[c4EL] = hA - (hB + hC + hD);
                a4FL[c4FL] = hB - (hA + hC + hD);
                a4GL[c4GL] = hC - (hA + hB + hD);
                a4HL[c4HL] = hD - (hA + hB + hC);
                hA = b4AL[c4AL - ((c4AL > d4A) ? d4A + 1 : 0)];
                hB = b4BL[c4BL - ((c4BL > d4B) ? d4B + 1 : 0)];
                hC = b4CL[c4CL - ((c4CL > d4C) ? d4C + 1 : 0)];
                hD = b4DL[c4DL - ((c4DL > d4D) ? d4D + 1 : 0)];
                b4EL[c4EL] = hA - (hB + hC + hD);
                b4FL[c4FL] = hB - (hA + hC + hD);
                b4GL[c4GL] = hC - (hA + hB + hD);
                b4HL[c4HL] = hD - (hA + hB + hC);
                c4EL++;
                if (c4EL < 0 || c4EL > d4E) {
                    c4EL = 0;
                }
                c4FL++;
                if (c4FL < 0 || c4FL > d4F) {
                    c4FL = 0;
                }
                c4GL++;
                if (c4GL < 0 || c4GL > d4G) {
                    c4GL = 0;
                }
                c4HL++;
                if (c4HL < 0 || c4HL > d4H) {
                    c4HL = 0;
                }
                hA = a4EL[c4EL - ((c4EL > d4E) ? d4E + 1 : 0)];
                hB = a4FL[c4FL - ((c4FL > d4F) ? d4F + 1 : 0)];
                hC = a4GL[c4GL - ((c4GL > d4G) ? d4G + 1 : 0)];
                hD = a4HL[c4HL - ((c4HL > d4H) ? d4H + 1 : 0)];
                a4IL[c4IL] = hA - (hB + hC + hD);
                a4JL[c4JL] = hB - (hA + hC + hD);
                a4KL[c4KL] = hC - (hA + hB + hD);
                a4LL[c4LL] = hD - (hA + hB + hC);
                hA = b4EL[c4EL - ((c4EL > d4E) ? d4E + 1 : 0)];
                hB = b4FL[c4FL - ((c4FL > d4F) ? d4F + 1 : 0)];
                hC = b4GL[c4GL - ((c4GL > d4G) ? d4G + 1 : 0)];
                hD = b4HL[c4HL - ((c4HL > d4H) ? d4H + 1 : 0)];
                b4IL[c4IL] = hA - (hB + hC + hD);
                b4JL[c4JL] = hB - (hA + hC + hD);
                b4KL[c4KL] = hC - (hA + hB + hD);
                b4LL[c4LL] = hD - (hA + hB + hC);
                c4IL++;
                if (c4IL < 0 || c4IL > d4I) {
                    c4IL = 0;
                }
                c4JL++;
                if (c4JL < 0 || c4JL > d4J) {
                    c4JL = 0;
                }
                c4KL++;
                if (c4KL < 0 || c4KL > d4K) {
                    c4KL = 0;
                }
                c4LL++;
                if (c4LL < 0 || c4LL > d4L) {
                    c4LL = 0;
                }
                hA = a4IL[c4IL - ((c4IL > d4I) ? d4I + 1 : 0)];
                hB = a4JL[c4JL - ((c4JL > d4J) ? d4J + 1 : 0)];
                hC = a4KL[c4KL - ((c4KL > d4K) ? d4K + 1 : 0)];
                hD = a4LL[c4LL - ((c4LL > d4L) ? d4L + 1 : 0)];
                a4ML[c4ML] = hA - (hB + hC + hD);
                a4NL[c4NL] = hB - (hA + hC + hD);
                a4OL[c4OL] = hC - (hA + hB + hD);
                a4PL[c4PL] = hD - (hA + hB + hC);
                hA = b4IL[c4IL - ((c4IL > d4I) ? d4I + 1 : 0)];
                hB = b4JL[c4JL - ((c4JL > d4J) ? d4J + 1 : 0)];
                hC = b4KL[c4KL - ((c4KL > d4K) ? d4K + 1 : 0)];
                hD = b4LL[c4LL - ((c4LL > d4L) ? d4L + 1 : 0)];
                b4ML[c4ML] = hA - (hB + hC + hD);
                b4NL[c4NL] = hB - (hA + hC + hD);
                b4OL[c4OL] = hC - (hA + hB + hD);
                b4PL[c4PL] = hD - (hA + hB + hC);
                c4ML++;
                if (c4ML < 0 || c4ML > d4M) {
                    c4ML = 0;
                }
                c4NL++;
                if (c4NL < 0 || c4NL > d4N) {
                    c4NL = 0;
                }
                c4OL++;
                if (c4OL < 0 || c4OL > d4O) {
                    c4OL = 0;
                }
                c4PL++;
                if (c4PL < 0 || c4PL > d4P) {
                    c4PL = 0;
                }
                hA = a4ML[c4ML - ((c4ML > d4M) ? d4M + 1 : 0)];
                hB = a4NL[c4NL - ((c4NL > d4N) ? d4N + 1 : 0)];
                hC = a4OL[c4OL - ((c4OL > d4O) ? d4O + 1 : 0)];
                hD = a4PL[c4PL - ((c4PL > d4P) ? d4P + 1 : 0)];
                f4AL = hA - (hB + hC + hD);
                f4BL = hB - (hA + hC + hD);
                f4CL = hC - (hA + hB + hD);
                f4DL = hD - (hA + hB + hC); // not actually crosschannel yet
                mainSampleL = (hA + hB + hC + hD) * 0.125;
                hA = b4ML[c4ML - ((c4ML > d4M) ? d4M + 1 : 0)];
                hB = b4NL[c4NL - ((c4NL > d4N) ? d4N + 1 : 0)];
                hC = b4OL[c4OL - ((c4OL > d4O) ? d4O + 1 : 0)];
                hD = b4PL[c4PL - ((c4PL > d4P) ? d4P + 1 : 0)];
                g4AL = hA - (hB + hC + hD);
                g4BL = hB - (hA + hC + hD);
                g4CL = hC - (hA + hB + hD);
                g4DL = hD - (hA + hB + hC);
                dualmonoSampleL = (hA + hB + hC + hD) * 0.125;
                double mainSampleR = bez[bez_SampR]; // begin primary reverb
                double dualmonoSampleR = bez[bez_SampL];
                // right verbs
                a4DR[c4DR] = mainSampleR + (f4AL * reg4n);
                a4HR[c4HR] = mainSampleR + (f4BL * reg4n);
                a4LR[c4LR] = mainSampleR + (f4CL * reg4n);
                a4PR[c4PR] = mainSampleR + (f4DL * reg4n);
                b4DR[c4DR] = dualmonoSampleR + (g4DR * reg4n);
                b4HR[c4HR] = dualmonoSampleR + (g4HR * reg4n);
                b4LR[c4LR] = dualmonoSampleR + (g4LR * reg4n);
                b4PR[c4PR] = dualmonoSampleR + (g4PR * reg4n);
                c4DR++;
                if (c4DR < 0 || c4DR > d4D) {
                    c4DR = 0;
                }
                c4HR++;
                if (c4HR < 0 || c4HR > d4H) {
                    c4HR = 0;
                }
                c4LR++;
                if (c4LR < 0 || c4LR > d4L) {
                    c4LR = 0;
                }
                c4PR++;
                if (c4PR < 0 || c4PR > d4P) {
                    c4PR = 0;
                }
                hA = a4DR[c4DR - ((c4DR > d4D) ? d4D + 1 : 0)];
                hB = a4HR[c4HR - ((c4HR > d4H) ? d4H + 1 : 0)];
                hC = a4LR[c4LR - ((c4LR > d4L) ? d4L + 1 : 0)];
                hD = a4PR[c4PR - ((c4PR > d4P) ? d4P + 1 : 0)];
                a4CR[c4CR] = hA - (hB + hC + hD);
                a4GR[c4GR] = hB - (hA + hC + hD);
                a4KR[c4KR] = hC - (hA + hB + hD);
                a4OR[c4OR] = hD - (hA + hB + hC);
                hA = b4DR[c4DR - ((c4DR > d4D) ? d4D + 1 : 0)];
                hB = b4HR[c4HR - ((c4HR > d4H) ? d4H + 1 : 0)];
                hC = b4LR[c4LR - ((c4LR > d4L) ? d4L + 1 : 0)];
                hD = b4PR[c4PR - ((c4PR > d4P) ? d4P + 1 : 0)];
                b4CR[c4CR] = hA - (hB + hC + hD);
                b4GR[c4GR] = hB - (hA + hC + hD);
                b4KR[c4KR] = hC - (hA + hB + hD);
                b4OR[c4OR] = hD - (hA + hB + hC);
                c4CR++;
                if (c4CR < 0 || c4CR > d4C) {
                    c4CR = 0;
                }
                c4GR++;
                if (c4GR < 0 || c4GR > d4G) {
                    c4GR = 0;
                }
                c4KR++;
                if (c4KR < 0 || c4KR > d4K) {
                    c4KR = 0;
                }
                c4OR++;
                if (c4OR < 0 || c4OR > d4O) {
                    c4OR = 0;
                }
                hA = a4CR[c4CR - ((c4CR > d4C) ? d4C + 1 : 0)];
                hB = a4GR[c4GR - ((c4GR > d4G) ? d4G + 1 : 0)];
                hC = a4KR[c4KR - ((c4KR > d4K) ? d4K + 1 : 0)];
                hD = a4OR[c4OR - ((c4OR > d4O) ? d4O + 1 : 0)];
                a4BR[c4BR] = hA - (hB + hC + hD);
                a4FR[c4FR] = hB - (hA + hC + hD);
                a4JR[c4JR] = hC - (hA + hB + hD);
                a4NR[c4NR] = hD - (hA + hB + hC);
                hA = b4CR[c4CR - ((c4CR > d4C) ? d4C + 1 : 0)];
                hB = b4GR[c4GR - ((c4GR > d4G) ? d4G + 1 : 0)];
                hC = b4KR[c4KR - ((c4KR > d4K) ? d4K + 1 : 0)];
                hD = b4OR[c4OR - ((c4OR > d4O) ? d4O + 1 : 0)];
                b4BR[c4BR] = hA - (hB + hC + hD);
                b4FR[c4FR] = hB - (hA + hC + hD);
                b4JR[c4JR] = hC - (hA + hB + hD);
                b4NR[c4NR] = hD - (hA + hB + hC);
                c4BR++;
                if (c4BR < 0 || c4BR > d4B) {
                    c4BR = 0;
                }
                c4FR++;
                if (c4FR < 0 || c4FR > d4F) {
                    c4FR = 0;
                }
                c4JR++;
                if (c4JR < 0 || c4JR > d4J) {
                    c4JR = 0;
                }
                c4NR++;
                if (c4NR < 0 || c4NR > d4N) {
                    c4NR = 0;
                }
                hA = a4BR[c4BR - ((c4BR > d4B) ? d4B + 1 : 0)];
                hB = a4FR[c4FR - ((c4FR > d4F) ? d4F + 1 : 0)];
                hC = a4JR[c4JR - ((c4JR > d4J) ? d4J + 1 : 0)];
                hD = a4NR[c4NR - ((c4NR > d4N) ? d4N + 1 : 0)];
                a4AR[c4AR] = hA - (hB + hC + hD);
                a4ER[c4ER] = hB - (hA + hC + hD);
                a4IR[c4IR] = hC - (hA + hB + hD);
                a4MR[c4MR] = hD - (hA + hB + hC);
                hA = b4BR[c4BR - ((c4BR > d4B) ? d4B + 1 : 0)];
                hB = b4FR[c4FR - ((c4FR > d4F) ? d4F + 1 : 0)];
                hC = b4JR[c4JR - ((c4JR > d4J) ? d4J + 1 : 0)];
                hD = b4NR[c4NR - ((c4NR > d4N) ? d4N + 1 : 0)];
                b4AR[c4AR] = hA - (hB + hC + hD);
                b4ER[c4ER] = hB - (hA + hC + hD);
                b4IR[c4IR] = hC - (hA + hB + hD);
                b4MR[c4MR] = hD - (hA + hB + hC);
                c4AR++;
                if (c4AR < 0 || c4AR > d4A) {
                    c4AR = 0;
                }
                c4ER++;
                if (c4ER < 0 || c4ER > d4E) {
                    c4ER = 0;
                }
                c4IR++;
                if (c4IR < 0 || c4IR > d4I) {
                    c4IR = 0;
                }
                c4MR++;
                if (c4MR < 0 || c4MR > d4M) {
                    c4MR = 0;
                }
                hA = a4AR[c4AR - ((c4AR > d4A) ? d4A + 1 : 0)];
                hB = a4ER[c4ER - ((c4ER > d4E) ? d4E + 1 : 0)];
                hC = a4IR[c4IR - ((c4IR > d4I) ? d4I + 1 : 0)];
                hD = a4MR[c4MR - ((c4MR > d4M) ? d4M + 1 : 0)];
                f4DR = hA - (hB + hC + hD);
                f4HR = hB - (hA + hC + hD);
                f4LR = hC - (hA + hB + hD);
                f4PR = hD - (hA + hB + hC);
                mainSampleR = (hA + hB + hC + hD) * 0.125;
                hA = b4AR[c4AR - ((c4AR > d4A) ? d4A + 1 : 0)];
                hB = b4ER[c4ER - ((c4ER > d4E) ? d4E + 1 : 0)];
                hC = b4IR[c4IR - ((c4IR > d4I) ? d4I + 1 : 0)];
                hD = b4MR[c4MR - ((c4MR > d4M) ? d4M + 1 : 0)];
                g4DR = hA - (hB + hC + hD);
                g4HR = hB - (hA + hC + hD);
                g4LR = hC - (hA + hB + hD);
                g4PR = hD - (hA + hB + hC);
                dualmonoSampleR = (hA + hB + hC + hD) * 0.125;
                // dual mono version is wider = 1.0 at the center
                // with mainsample 0.0 and 2.0 (only at the edges)
                // with mainsample out of phase when over 1.0
                // couldn't re-do the arrays perfectly, so instead
                // we keep exactly the same cross-matrix,
                // but we flip the sides we're using for initial reverb.
                // then, dualmono remains totally dualmono, and blend a bit in for wideness.
                if (wider < 1.0) {
                    inputSampleL = (dualmonoSampleR * wider) + (mainSampleL * (1.0 - wider));
                    inputSampleR = (dualmonoSampleL * wider) + (mainSampleR * (1.0 - wider));
                }
                else {
                    inputSampleL = (dualmonoSampleR * (2.0 - wider)) + (mainSampleL * (wider - 1.0));
                    inputSampleR = (dualmonoSampleL * (2.0 - wider)) + (-mainSampleR * (wider - 1.0));
                }
                bez[bez_CL] = bez[bez_BL];
                bez[bez_BL] = bez[bez_AL];
                bez[bez_AL] = inputSampleL;
                bez[bez_SampL] = 0.0;
                bez[bez_CR] = bez[bez_BR];
                bez[bez_BR] = bez[bez_AR];
                bez[bez_AR] = inputSampleR;
                bez[bez_SampR] = 0.0;
            }
            double X = bez[bez_cycle] * bezTrim;
            double CBL = (bez[bez_CL] * (1.0 - X)) + (bez[bez_BL] * X);
            double CBR = (bez[bez_CR] * (1.0 - X)) + (bez[bez_BR] * X);
            double BAL = (bez[bez_BL] * (1.0 - X)) + (bez[bez_AL] * X);
            double BAR = (bez[bez_BR] * (1.0 - X)) + (bez[bez_AR] * X);
            inputSampleL = (bez[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * -0.25;
            inputSampleR = (bez[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * -0.25;
            bezF[bez_cycle] += derezFreq;
            bezF[bez_SampL] += (inputSampleL * derezFreq);
            bezF[bez_SampR] += (inputSampleR * derezFreq);
            if (bezF[bez_cycle] > 1.0) { // hit the end point and we do a filter sample
                bezF[bez_cycle] = 0.0;
                bezF[bez_CL] = bezF[bez_BL];
                bezF[bez_BL] = bezF[bez_AL];
                bezF[bez_AL] = bezF[bez_SampL];
                bezF[bez_SampL] = 0.0;
                bezF[bez_CR] = bezF[bez_BR];
                bezF[bez_BR] = bezF[bez_AR];
                bezF[bez_AR] = bezF[bez_SampR];
                bezF[bez_SampR] = 0.0;
            }
            X = bezF[bez_cycle] * bezFreqTrim;
            double CBLfreq = (bezF[bez_CL] * (1.0 - X)) + (bezF[bez_BL] * X);
            double BALfreq = (bezF[bez_BL] * (1.0 - X)) + (bezF[bez_AL] * X);
            inputSampleL = (bezF[bez_BL] + (CBLfreq * (1.0 - X)) + (BALfreq * X)) * 0.25;
            double CBRfreq = (bezF[bez_CR] * (1.0 - X)) + (bezF[bez_BR] * X);
            double BARfreq = (bezF[bez_BR] * (1.0 - X)) + (bezF[bez_AR] * X);
            inputSampleR = (bezF[bez_BR] + (CBRfreq * (1.0 - X)) + (BARfreq * X)) * 0.25;
            // filtering the reverb separately, after making it
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::verbtiny
