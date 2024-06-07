#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::kcathedral {

constexpr std::string_view k_name{ "kCathedral" };
constexpr std::string_view k_short_description{
    "kCathedral is a giant reverby space."
};
constexpr std::string_view k_long_description{
    "Here's a neat little experiment! As always with the K reverbs, it's a dedicated sound space with its own unique algorithm… but there are some new twists making kCathedral a step along an interesting path. This is the first of the Bricasti-inspired reverbs!It uses a 5x5 Householder matrix, and a built-in crossover (a SubTight filter, so it's not even a normal type of filter) to allow for extra delay on the lows. The internal reverb filtering is Pear filters rather than the biquads used on the kPlates. (the reverb is undersampled at high sample rates, so the SubTight crossover will work the same whether at 1x, 2x, or 4x.)And for all that trouble… kCathedral does NOT sound the same as the Bricasti Cathedral preset. You won't find a clone here.But… it will produce largely the same depth and spatiality, with a different texture that is less 'rich and soft' and more 'stark and vibey'. It should be used similarly: for realism, hide it behind other sounds by keeping it quiet. There are no controls other than a dry/wet: for use on an aux, go full wet, and if you're using it inline you might end up adding just the tiniest amount (in the video, I'm using 0.03 of it on my voice, and you'll barely notice it until it switches to similarly faint ClearCoat 7, just for a sentence, after having mentioned ClearCoat.)My hope for these is distinctness, and kCathedral might not be the one to reach for as an 'all-purpose' reverb, because it's just a first steo into a larger, more echoey new world. The thing I like about it is, while it has a distinct sound, the spatiality sits in the mix roughly the same way a real Bricasti might, even while the texture is different (will be interesting to see if I can get closer to that butter tone, in future).All in all, a good day at work (okay, months) and I hope you enjoy kCathedral :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class kCathedral final : public Effect<T>
{
    double gainOutL;
    double gainOutR;
    double eAL[earlyA + 5];
    double eBL[earlyB + 5];
    double eCL[earlyC + 5];
    double eDL[earlyD + 5];
    double eEL[earlyE + 5];
    double eFL[earlyF + 5];
    double eGL[earlyG + 5];
    double eHL[earlyH + 5];
    double eIL[earlyI + 5];
    double eAR[earlyA + 5];
    double eBR[earlyB + 5];
    double eCR[earlyC + 5];
    double eDR[earlyD + 5];
    double eER[earlyE + 5];
    double eFR[earlyF + 5];
    double eGR[earlyG + 5];
    double eHR[earlyH + 5];
    double eIR[earlyI + 5];
    int earlyAL, earlyAR;
    int earlyBL, earlyBR;
    int earlyCL, earlyCR;
    int earlyDL, earlyDR;
    int earlyEL, earlyER;
    int earlyFL, earlyFR;
    int earlyGL, earlyGR;
    int earlyHL, earlyHR;
    int earlyIL, earlyIR;
    double aAL[delayA + 5];
    double aBL[delayB + 5];
    double aCL[delayC + 5];
    double aDL[delayD + 5];
    double aEL[delayE + 5];
    double aFL[delayF + 5];
    double aGL[delayG + 5];
    double aHL[delayH + 5];
    double aIL[delayI + 5];
    double aJL[delayJ + 5];
    double aKL[delayK + 5];
    double aLL[delayL + 5];
    double aML[delayM + 5];
    double aNL[delayN + 5];
    double aOL[delayO + 5];
    double aPL[delayP + 5];
    double aQL[delayQ + 5];
    double aRL[delayR + 5];
    double aSL[delayS + 5];
    double aTL[delayT + 5];
    double aUL[delayU + 5];
    double aVL[delayV + 5];
    double aWL[delayW + 5];
    double aXL[delayX + 5];
    double aYL[delayY + 5];
    double aAR[delayA + 5];
    double aBR[delayB + 5];
    double aCR[delayC + 5];
    double aDR[delayD + 5];
    double aER[delayE + 5];
    double aFR[delayF + 5];
    double aGR[delayG + 5];
    double aHR[delayH + 5];
    double aIR[delayI + 5];
    double aJR[delayJ + 5];
    double aKR[delayK + 5];
    double aLR[delayL + 5];
    double aMR[delayM + 5];
    double aNR[delayN + 5];
    double aOR[delayO + 5];
    double aPR[delayP + 5];
    double aQR[delayQ + 5];
    double aRR[delayR + 5];
    double aSR[delayS + 5];
    double aTR[delayT + 5];
    double aUR[delayU + 5];
    double aVR[delayV + 5];
    double aWR[delayW + 5];
    double aXR[delayX + 5];
    double aYR[delayY + 5];
    double aZL[predelay + 5];
    double aZR[predelay + 5];
    double aVLFL[vlfpredelay + 5];
    double aVLFR[vlfpredelay + 5];
    double feedbackAL;
    double feedbackBL;
    double feedbackCL;
    double feedbackDL;
    double feedbackEL;
    double feedbackER;
    double feedbackJR;
    double feedbackOR;
    double feedbackTR;
    double feedbackYR;
    double lastRefL[7];
    double lastRefR[7];
    int countAL;
    int countBL;
    int countCL;
    int countDL;
    int countEL;
    int countFL;
    int countGL;
    int countHL;
    int countIL;
    int countJL;
    int countKL;
    int countLL;
    int countML;
    int countNL;
    int countOL;
    int countPL;
    int countQL;
    int countRL;
    int countSL;
    int countTL;
    int countUL;
    int countVL;
    int countWL;
    int countXL;
    int countYL;
    int countAR;
    int countBR;
    int countCR;
    int countDR;
    int countER;
    int countFR;
    int countGR;
    int countHR;
    int countIR;
    int countJR;
    int countKR;
    int countLR;
    int countMR;
    int countNR;
    int countOR;
    int countPR;
    int countQR;
    int countRR;
    int countSR;
    int countTR;
    int countUR;
    int countVR;
    int countWR;
    int countXR;
    int countYR;
    int countZ;
    int countVLF;
    int cycle;
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        prevSampL7,
        prevSlewL7,
        prevSampR7,
        prevSlewR7,
        prevSampL8,
        prevSlewL8,
        prevSampR8,
        prevSlewR8,
        prevSampL9,
        prevSlewL9,
        prevSampR9,
        prevSlewR9,
        prevSampL10,
        prevSlewL10,
        prevSampR10,
        prevSlewR10,
        pear_total

        public :
            kCathedral(){
                A = 1.0;
                gainOutL = gainOutR = 1.0;
                for (int count = 0; count < delayA + 2; count++){ aAL[count] = 0.0; aAR[count] = 0.0; }
    for (int count = 0; count < delayB + 2; count++) {
        aBL[count] = 0.0;
        aBR[count] = 0.0;
    }
    for (int count = 0; count < delayC + 2; count++) {
        aCL[count] = 0.0;
        aCR[count] = 0.0;
    }
    for (int count = 0; count < delayD + 2; count++) {
        aDL[count] = 0.0;
        aDR[count] = 0.0;
    }
    for (int count = 0; count < delayE + 2; count++) {
        aEL[count] = 0.0;
        aER[count] = 0.0;
    }
    for (int count = 0; count < delayF + 2; count++) {
        aFL[count] = 0.0;
        aFR[count] = 0.0;
    }
    for (int count = 0; count < delayG + 2; count++) {
        aGL[count] = 0.0;
        aGR[count] = 0.0;
    }
    for (int count = 0; count < delayH + 2; count++) {
        aHL[count] = 0.0;
        aHR[count] = 0.0;
    }
    for (int count = 0; count < delayI + 2; count++) {
        aIL[count] = 0.0;
        aIR[count] = 0.0;
    }
    for (int count = 0; count < delayJ + 2; count++) {
        aJL[count] = 0.0;
        aJR[count] = 0.0;
    }
    for (int count = 0; count < delayK + 2; count++) {
        aKL[count] = 0.0;
        aKR[count] = 0.0;
    }
    for (int count = 0; count < delayL + 2; count++) {
        aLL[count] = 0.0;
        aLR[count] = 0.0;
    }
    for (int count = 0; count < delayM + 2; count++) {
        aML[count] = 0.0;
        aMR[count] = 0.0;
    }
    for (int count = 0; count < delayN + 2; count++) {
        aNL[count] = 0.0;
        aNR[count] = 0.0;
    }
    for (int count = 0; count < delayO + 2; count++) {
        aOL[count] = 0.0;
        aOR[count] = 0.0;
    }
    for (int count = 0; count < delayP + 2; count++) {
        aPL[count] = 0.0;
        aPR[count] = 0.0;
    }
    for (int count = 0; count < delayQ + 2; count++) {
        aQL[count] = 0.0;
        aQR[count] = 0.0;
    }
    for (int count = 0; count < delayR + 2; count++) {
        aRL[count] = 0.0;
        aRR[count] = 0.0;
    }
    for (int count = 0; count < delayS + 2; count++) {
        aSL[count] = 0.0;
        aSR[count] = 0.0;
    }
    for (int count = 0; count < delayT + 2; count++) {
        aTL[count] = 0.0;
        aTR[count] = 0.0;
    }
    for (int count = 0; count < delayU + 2; count++) {
        aUL[count] = 0.0;
        aUR[count] = 0.0;
    }
    for (int count = 0; count < delayV + 2; count++) {
        aVL[count] = 0.0;
        aVR[count] = 0.0;
    }
    for (int count = 0; count < delayW + 2; count++) {
        aWL[count] = 0.0;
        aWR[count] = 0.0;
    }
    for (int count = 0; count < delayX + 2; count++) {
        aXL[count] = 0.0;
        aXR[count] = 0.0;
    }
    for (int count = 0; count < delayY + 2; count++) {
        aYL[count] = 0.0;
        aYR[count] = 0.0;
    }
    for (int count = 0; count < earlyA + 2; count++) {
        eAL[count] = 0.0;
        eAR[count] = 0.0;
    }
    for (int count = 0; count < earlyB + 2; count++) {
        eBL[count] = 0.0;
        eBR[count] = 0.0;
    }
    for (int count = 0; count < earlyC + 2; count++) {
        eCL[count] = 0.0;
        eCR[count] = 0.0;
    }
    for (int count = 0; count < earlyD + 2; count++) {
        eDL[count] = 0.0;
        eDR[count] = 0.0;
    }
    for (int count = 0; count < earlyE + 2; count++) {
        eEL[count] = 0.0;
        eER[count] = 0.0;
    }
    for (int count = 0; count < earlyF + 2; count++) {
        eFL[count] = 0.0;
        eFR[count] = 0.0;
    }
    for (int count = 0; count < earlyG + 2; count++) {
        eGL[count] = 0.0;
        eGR[count] = 0.0;
    }
    for (int count = 0; count < earlyH + 2; count++) {
        eHL[count] = 0.0;
        eHR[count] = 0.0;
    }
    for (int count = 0; count < earlyI + 2; count++) {
        eIL[count] = 0.0;
        eIR[count] = 0.0;
    }
    for (int count = 0; count < predelay + 2; count++) {
        aZL[count] = 0.0;
        aZR[count] = 0.0;
    }
    for (int count = 0; count < vlfpredelay + 2; count++) {
        aVLFL[count] = 0.0;
        aVLFR[count] = 0.0;
    }
    feedbackAL = 0.0;
    feedbackBL = 0.0;
    feedbackCL = 0.0;
    feedbackDL = 0.0;
    feedbackEL = 0.0;
    feedbackER = 0.0;
    feedbackJR = 0.0;
    feedbackOR = 0.0;
    feedbackTR = 0.0;
    feedbackYR = 0.0;
    prevMulchBL = 0.0;
    prevMulchBR = 0.0;
    prevMulchCL = 0.0;
    prevMulchCR = 0.0;
    prevMulchDL = 0.0;
    prevMulchDR = 0.0;
    prevMulchEL = 0.0;
    prevMulchER = 0.0;
    tailL = 0.0;
    tailR = 0.0;
    prevOutEL = 0.0;
    prevOutER = 0.0;
    prevInEL = 0.0;
    prevInER = 0.0;
    for (int count = 0; count < 6; count++) {
        lastRefL[count] = 0.0;
        lastRefR[count] = 0.0;
    }
    earlyAL = 1;
    earlyBL = 1;
    earlyCL = 1;
    earlyDL = 1;
    earlyEL = 1;
    earlyFL = 1;
    earlyGL = 1;
    earlyHL = 1;
    earlyIL = 1;
    earlyAR = 1;
    earlyBR = 1;
    earlyCR = 1;
    earlyDR = 1;
    earlyER = 1;
    earlyFR = 1;
    earlyGR = 1;
    earlyHR = 1;
    earlyIR = 1;
    countAL = 1;
    countBL = 1;
    countCL = 1;
    countDL = 1;
    countEL = 1;
    countFL = 1;
    countGL = 1;
    countHL = 1;
    countIL = 1;
    countJL = 1;
    countKL = 1;
    countLL = 1;
    countML = 1;
    countNL = 1;
    countOL = 1;
    countPL = 1;
    countQL = 1;
    countRL = 1;
    countSL = 1;
    countTL = 1;
    countUL = 1;
    countVL = 1;
    countWL = 1;
    countXL = 1;
    countYL = 1;
    countAR = 1;
    countBR = 1;
    countCR = 1;
    countDR = 1;
    countER = 1;
    countFR = 1;
    countGR = 1;
    countHR = 1;
    countIR = 1;
    countJR = 1;
    countKR = 1;
    countLR = 1;
    countMR = 1;
    countNR = 1;
    countOR = 1;
    countPR = 1;
    countQR = 1;
    countRR = 1;
    countSR = 1;
    countTR = 1;
    countUR = 1;
    countVR = 1;
    countWR = 1;
    countXR = 1;
    countYR = 1;
    countZ = 1;
    countVLF = 1;
    cycle = 0;
    for (int x = 0; x < pear_total; x++) {
        pearA[x] = 0.0;
        pearB[x] = 0.0;
        pearC[x] = 0.0;
    }
    // from PearEQ
    subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
    // from SubTight
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

enum params {
    kParamA = 0,
    kNumParameters = 1
};

void set_parameter_value(int index, float value)
{
    switch (static_cast<params>(index))
    {
    kParamA:
        A = value;
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

        default: break;
    }
    return 0.0;
}

T get_parameter_default(int index)
{
    switch (static_cast<params>(index))
    {
    kParamA:
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
        return "wetness";
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
        return "Wetness";
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
    overallscale *= getSampleRate();
    int cycleEnd = floor(overallscale);
    if (cycleEnd < 1) {
        cycleEnd = 1;
    }
    if (cycleEnd > 4) {
        cycleEnd = 4;
    }
    // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
    if (cycle > cycleEnd - 1) {
        cycle = cycleEnd - 1; // sanity check
    }
    int adjPredelay = predelay;
    int adjSubDelay = vlfpredelay;
    double wet = A * 2.0;
    double dry = 2.0 - wet;
    if (wet > 1.0) {
        wet = 1.0;
    }
    if (wet < 0.0) {
        wet = 0.0;
    }
    if (dry > 1.0) {
        dry = 1.0;
    }
    if (dry < 0.0) {
        dry = 0.0;
    }
    // this reverb makes 50% full dry AND full wet, not crossfaded.
    // that's so it can be on submixes without cutting back dry channel when adjusted:
    // unless you go super heavy, you are only adjusting the added verb loudness.
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
        cycle++;
        if (cycle == cycleEnd) { // hit the end point and we do a reverb sample
            double outSample;
            outSample = (inputSampleL + prevInEL) * 0.5;
            prevInEL = inputSampleL;
            inputSampleL = outSample;
            outSample = (inputSampleR + prevInER) * 0.5;
            prevInER = inputSampleR;
            inputSampleR = outSample;
            // predelay
            aZL[countZ] = inputSampleL;
            aZR[countZ] = inputSampleR;
            countZ++;
            if (countZ < 0 || countZ > adjPredelay) {
                countZ = 0;
            }
            inputSampleL = aZL[countZ - ((countZ > adjPredelay) ? adjPredelay + 1 : 0)];
            inputSampleR = aZR[countZ - ((countZ > adjPredelay) ? adjPredelay + 1 : 0)];
            // end predelay
            // begin SubTight section
            double outSampleL = inputSampleL * 0.0026856;
            double outSampleR = inputSampleR * 0.0026856;
            double scale = 0.5 + fabs(outSampleL * 0.5);
            outSampleL = (subAL + (sin(subAL - outSampleL) * scale));
            subAL = outSampleL * scale;
            scale = 0.5 + fabs(outSampleR * 0.5);
            outSampleR = (subAR + (sin(subAR - outSampleR) * scale));
            subAR = outSampleR * scale;
            scale = 0.5 + fabs(outSampleL * 0.5);
            outSampleL = (subBL + (sin(subBL - outSampleL) * scale));
            subBL = outSampleL * scale;
            scale = 0.5 + fabs(outSampleR * 0.5);
            outSampleR = (subBR + (sin(subBR - outSampleR) * scale));
            subBR = outSampleR * scale;
            scale = 0.5 + fabs(outSampleL * 0.5);
            outSampleL = (subCL + (sin(subCL - outSampleL) * scale));
            subCL = outSampleL * scale;
            scale = 0.5 + fabs(outSampleR * 0.5);
            outSampleR = (subCR + (sin(subCR - outSampleR) * scale));
            subCR = outSampleR * scale;
            outSampleL = -outSampleL;
            outSampleR = -outSampleR;
            if (outSampleL > 0.25) {
                outSampleL = 0.25;
            }
            if (outSampleL < -0.25) {
                outSampleL = -0.25;
            }
            if (outSampleR > 0.25) {
                outSampleR = 0.25;
            }
            if (outSampleR < -0.25) {
                outSampleR = -0.25;
            }
            outSampleL *= 16.0;
            outSampleR *= 16.0;
            inputSampleL -= outSampleL;
            inputSampleR -= outSampleR;
            // end SubTight section
            // VLF predelay
            aVLFL[countVLF] = outSampleL;
            aVLFR[countVLF] = outSampleR;
            countVLF++;
            if (countVLF < 0 || countVLF > adjSubDelay) {
                countVLF = 0;
            }
            outSampleL = aVLFL[countVLF - ((countVLF > adjSubDelay) ? adjSubDelay + 1 : 0)] * 2.0;
            outSampleR = aVLFR[countVLF - ((countVLF > adjSubDelay) ? adjSubDelay + 1 : 0)] * 2.0;
            // end VLF predelay
            // begin with early reflections
            eAL[earlyAL] = inputSampleL;
            eBL[earlyBL] = inputSampleL;
            eCL[earlyCL] = inputSampleL;
            eCR[earlyCR] = inputSampleR;
            eFR[earlyFR] = inputSampleR;
            eIR[earlyIR] = inputSampleR;
            earlyAL++;
            if (earlyAL < 0 || earlyAL > earlyA) {
                earlyAL = 0;
            }
            earlyBL++;
            if (earlyBL < 0 || earlyBL > earlyB) {
                earlyBL = 0;
            }
            earlyCL++;
            if (earlyCL < 0 || earlyCL > earlyC) {
                earlyCL = 0;
            }
            earlyCR++;
            if (earlyCR < 0 || earlyCR > earlyC) {
                earlyCR = 0;
            }
            earlyFR++;
            if (earlyFR < 0 || earlyFR > earlyF) {
                earlyFR = 0;
            }
            earlyIR++;
            if (earlyIR < 0 || earlyIR > earlyI) {
                earlyIR = 0;
            }
            double oeAL = eAL[earlyAL - ((earlyAL > earlyA) ? earlyA + 1 : 0)];
            double oeBL = eBL[earlyBL - ((earlyBL > earlyB) ? earlyB + 1 : 0)];
            double oeCL = eCL[earlyCL - ((earlyCL > earlyC) ? earlyC + 1 : 0)];
            double oeCR = eCR[earlyCR - ((earlyCR > earlyC) ? earlyC + 1 : 0)];
            double oeFR = eFR[earlyFR - ((earlyFR > earlyF) ? earlyF + 1 : 0)];
            double oeIR = eIR[earlyIR - ((earlyIR > earlyI) ? earlyI + 1 : 0)];
            eDL[earlyDL] = ((oeBL + oeCL) - oeAL);
            eEL[earlyEL] = ((oeAL + oeCL) - oeBL);
            eFL[earlyFL] = ((oeAL + oeBL) - oeCL);
            eBR[earlyBR] = ((oeFR + oeIR) - oeCR);
            eER[earlyER] = ((oeCR + oeIR) - oeFR);
            eHR[earlyHR] = ((oeCR + oeFR) - oeIR);
            earlyDL++;
            if (earlyDL < 0 || earlyDL > earlyD) {
                earlyDL = 0;
            }
            earlyEL++;
            if (earlyEL < 0 || earlyEL > earlyE) {
                earlyEL = 0;
            }
            earlyFL++;
            if (earlyFL < 0 || earlyFL > earlyF) {
                earlyFL = 0;
            }
            earlyBR++;
            if (earlyBR < 0 || earlyBR > earlyB) {
                earlyBR = 0;
            }
            earlyER++;
            if (earlyER < 0 || earlyER > earlyE) {
                earlyER = 0;
            }
            earlyHR++;
            if (earlyHR < 0 || earlyHR > earlyH) {
                earlyHR = 0;
            }
            double oeDL = eDL[earlyDL - ((earlyDL > earlyD) ? earlyD + 1 : 0)];
            double oeEL = eEL[earlyEL - ((earlyEL > earlyE) ? earlyE + 1 : 0)];
            double oeFL = eFL[earlyFL - ((earlyFL > earlyF) ? earlyF + 1 : 0)];
            double oeBR = eBR[earlyBR - ((earlyBR > earlyB) ? earlyB + 1 : 0)];
            double oeER = eER[earlyER - ((earlyER > earlyE) ? earlyE + 1 : 0)];
            double oeHR = eHR[earlyHR - ((earlyHR > earlyH) ? earlyH + 1 : 0)];
            eGL[earlyGL] = ((oeEL + oeFL) - oeDL);
            eHL[earlyHL] = ((oeDL + oeFL) - oeEL);
            eIL[earlyIL] = ((oeDL + oeEL) - oeFL);
            eAR[earlyAR] = ((oeER + oeHR) - oeBR);
            eDR[earlyDR] = ((oeBR + oeHR) - oeER);
            eGR[earlyGR] = ((oeBR + oeER) - oeHR);
            earlyGL++;
            if (earlyGL < 0 || earlyGL > earlyG) {
                earlyGL = 0;
            }
            earlyHL++;
            if (earlyHL < 0 || earlyHL > earlyH) {
                earlyHL = 0;
            }
            earlyIL++;
            if (earlyIL < 0 || earlyIL > earlyI) {
                earlyIL = 0;
            }
            earlyAR++;
            if (earlyAR < 0 || earlyAR > earlyA) {
                earlyAR = 0;
            }
            earlyDR++;
            if (earlyDR < 0 || earlyDR > earlyD) {
                earlyDR = 0;
            }
            earlyGR++;
            if (earlyGR < 0 || earlyGR > earlyG) {
                earlyGR = 0;
            }
            double oeGL = eGL[earlyGL - ((earlyGL > earlyG) ? earlyG + 1 : 0)];
            double oeHL = eHL[earlyHL - ((earlyHL > earlyH) ? earlyH + 1 : 0)];
            double oeIL = eIL[earlyIL - ((earlyIL > earlyI) ? earlyI + 1 : 0)];
            double oeAR = eAR[earlyAR - ((earlyAR > earlyA) ? earlyA + 1 : 0)];
            double oeDR = eDR[earlyDR - ((earlyDR > earlyD) ? earlyD + 1 : 0)];
            double oeGR = eGR[earlyGR - ((earlyGR > earlyG) ? earlyG + 1 : 0)];
            double earlyReflectionsL = oeGL + oeHL + oeIL;
            double earlyReflectionsR = oeAR + oeDR + oeGR;
            inputSampleL += outSampleL;
            inputSampleR += outSampleR;
            // having re-added our VLF delayed channel we can now re-use outSample
            aAL[countAL] = inputSampleL + (feedbackAL * 0.000293);
            aBL[countBL] = inputSampleL + (feedbackBL * 0.000293);
            aCL[countCL] = inputSampleL + (feedbackCL * 0.000293);
            aDL[countDL] = inputSampleL + (feedbackDL * 0.000293);
            aEL[countEL] = inputSampleL + (feedbackEL * 0.000293);
            aER[countER] = inputSampleR + (feedbackER * 0.000293);
            aJR[countJR] = inputSampleR + (feedbackJR * 0.000293);
            aOR[countOR] = inputSampleR + (feedbackOR * 0.000293);
            aTR[countTR] = inputSampleR + (feedbackTR * 0.000293);
            aYR[countYR] = inputSampleR + (feedbackYR * 0.000293);
            countAL++;
            if (countAL < 0 || countAL > delayA) {
                countAL = 0;
            }
            countBL++;
            if (countBL < 0 || countBL > delayB) {
                countBL = 0;
            }
            countCL++;
            if (countCL < 0 || countCL > delayC) {
                countCL = 0;
            }
            countDL++;
            if (countDL < 0 || countDL > delayD) {
                countDL = 0;
            }
            countEL++;
            if (countEL < 0 || countEL > delayE) {
                countEL = 0;
            }
            countER++;
            if (countER < 0 || countER > delayE) {
                countER = 0;
            }
            countJR++;
            if (countJR < 0 || countJR > delayJ) {
                countJR = 0;
            }
            countOR++;
            if (countOR < 0 || countOR > delayO) {
                countOR = 0;
            }
            countTR++;
            if (countTR < 0 || countTR > delayT) {
                countTR = 0;
            }
            countYR++;
            if (countYR < 0 || countYR > delayY) {
                countYR = 0;
            }
            double outAL = aAL[countAL - ((countAL > delayA) ? delayA + 1 : 0)];
            double outBL = aBL[countBL - ((countBL > delayB) ? delayB + 1 : 0)];
            double outCL = aCL[countCL - ((countCL > delayC) ? delayC + 1 : 0)];
            double outDL = aDL[countDL - ((countDL > delayD) ? delayD + 1 : 0)];
            double outEL = aEL[countEL - ((countEL > delayE) ? delayE + 1 : 0)];
            double outER = aER[countER - ((countER > delayE) ? delayE + 1 : 0)];
            double outJR = aJR[countJR - ((countJR > delayJ) ? delayJ + 1 : 0)];
            double outOR = aOR[countOR - ((countOR > delayO) ? delayO + 1 : 0)];
            double outTR = aTR[countTR - ((countTR > delayT) ? delayT + 1 : 0)];
            double outYR = aYR[countYR - ((countYR > delayY) ? delayY + 1 : 0)];
            //-------- one
            for (int x = 0; x < 31.32; x += 4) {
                double slew = ((outAL - pearA[x]) + pearA[x + 1]) * 0.304 * 0.5;
                pearA[x] = outAL = (0.304 * outAL) + ((1.0 - 0.304) * (pearA[x] + pearA[x + 1]));
                pearA[x + 1] = slew;
                slew = ((outER - pearA[x + 2]) + pearA[x + 3]) * 0.304 * 0.5;
                pearA[x + 2] = outER = (0.304 * outER) + ((1.0 - 0.304) * (pearA[x + 2] + pearA[x + 3]));
                pearA[x + 3] = slew;
            }
            aFL[countFL] = ((outAL * 3.0) - ((outBL + outCL + outDL + outEL) * 2.0));
            aGL[countGL] = ((outBL * 3.0) - ((outAL + outCL + outDL + outEL) * 2.0));
            aHL[countHL] = ((outCL * 3.0) - ((outAL + outBL + outDL + outEL) * 2.0));
            aIL[countIL] = ((outDL * 3.0) - ((outAL + outBL + outCL + outEL) * 2.0));
            aJL[countJL] = ((outEL * 3.0) - ((outAL + outBL + outCL + outDL) * 2.0));
            aDR[countDR] = ((outER * 3.0) - ((outJR + outOR + outTR + outYR) * 2.0));
            aIR[countIR] = ((outJR * 3.0) - ((outER + outOR + outTR + outYR) * 2.0));
            aNR[countNR] = ((outOR * 3.0) - ((outER + outJR + outTR + outYR) * 2.0));
            aSR[countSR] = ((outTR * 3.0) - ((outER + outJR + outOR + outYR) * 2.0));
            aXR[countXR] = ((outYR * 3.0) - ((outER + outJR + outOR + outTR) * 2.0));
            countFL++;
            if (countFL < 0 || countFL > delayF) {
                countFL = 0;
            }
            countGL++;
            if (countGL < 0 || countGL > delayG) {
                countGL = 0;
            }
            countHL++;
            if (countHL < 0 || countHL > delayH) {
                countHL = 0;
            }
            countIL++;
            if (countIL < 0 || countIL > delayI) {
                countIL = 0;
            }
            countJL++;
            if (countJL < 0 || countJL > delayJ) {
                countJL = 0;
            }
            countDR++;
            if (countDR < 0 || countDR > delayD) {
                countDR = 0;
            }
            countIR++;
            if (countIR < 0 || countIR > delayI) {
                countIR = 0;
            }
            countNR++;
            if (countNR < 0 || countNR > delayN) {
                countNR = 0;
            }
            countSR++;
            if (countSR < 0 || countSR > delayS) {
                countSR = 0;
            }
            countXR++;
            if (countXR < 0 || countXR > delayX) {
                countXR = 0;
            }
            double outFL = aFL[countFL - ((countFL > delayF) ? delayF + 1 : 0)];
            double outGL = aGL[countGL - ((countGL > delayG) ? delayG + 1 : 0)];
            double outHL = aHL[countHL - ((countHL > delayH) ? delayH + 1 : 0)];
            double outIL = aIL[countIL - ((countIL > delayI) ? delayI + 1 : 0)];
            double outJL = aJL[countJL - ((countJL > delayJ) ? delayJ + 1 : 0)];
            double outDR = aDR[countDR - ((countDR > delayD) ? delayD + 1 : 0)];
            double outIR = aIR[countIR - ((countIR > delayI) ? delayI + 1 : 0)];
            double outNR = aNR[countNR - ((countNR > delayN) ? delayN + 1 : 0)];
            double outSR = aSR[countSR - ((countSR > delayS) ? delayS + 1 : 0)];
            double outXR = aXR[countXR - ((countXR > delayX) ? delayX + 1 : 0)];
            //-------- mulch
            for (int x = 0; x < 31.32; x += 4) {
                double slew = ((outFL - pearB[x]) + pearB[x + 1]) * 0.566 * 0.5;
                pearB[x] = outFL = (0.566 * outFL) + ((1.0 - 0.566) * (pearB[x] + pearB[x + 1]));
                pearB[x + 1] = slew;
                slew = ((outDR - pearB[x + 2]) + pearB[x + 3]) * 0.566 * 0.5;
                pearB[x + 2] = outDR = (0.566 * outDR) + ((1.0 - 0.566) * (pearB[x + 2] + pearB[x + 3]));
                pearB[x + 3] = slew;
            }
            outSample = (outGL + prevMulchBL) * 0.5;
            prevMulchBL = outGL;
            outGL = outSample;
            outSample = (outIR + prevMulchBR) * 0.5;
            prevMulchBR = outIR;
            outIR = outSample;
            //-------- two
            aKL[countKL] = ((outFL * 3.0) - ((outGL + outHL + outIL + outJL) * 2.0));
            aLL[countLL] = ((outGL * 3.0) - ((outFL + outHL + outIL + outJL) * 2.0));
            aML[countML] = ((outHL * 3.0) - ((outFL + outGL + outIL + outJL) * 2.0));
            aNL[countNL] = ((outIL * 3.0) - ((outFL + outGL + outHL + outJL) * 2.0));
            aOL[countOL] = ((outJL * 3.0) - ((outFL + outGL + outHL + outIL) * 2.0));
            aCR[countCR] = ((outDR * 3.0) - ((outIR + outNR + outSR + outXR) * 2.0));
            aHR[countHR] = ((outIR * 3.0) - ((outDR + outNR + outSR + outXR) * 2.0));
            aMR[countMR] = ((outNR * 3.0) - ((outDR + outIR + outSR + outXR) * 2.0));
            aRR[countRR] = ((outSR * 3.0) - ((outDR + outIR + outNR + outXR) * 2.0));
            aWR[countWR] = ((outXR * 3.0) - ((outDR + outIR + outNR + outSR) * 2.0));
            countKL++;
            if (countKL < 0 || countKL > delayK) {
                countKL = 0;
            }
            countLL++;
            if (countLL < 0 || countLL > delayL) {
                countLL = 0;
            }
            countML++;
            if (countML < 0 || countML > delayM) {
                countML = 0;
            }
            countNL++;
            if (countNL < 0 || countNL > delayN) {
                countNL = 0;
            }
            countOL++;
            if (countOL < 0 || countOL > delayO) {
                countOL = 0;
            }
            countCR++;
            if (countCR < 0 || countCR > delayC) {
                countCR = 0;
            }
            countHR++;
            if (countHR < 0 || countHR > delayH) {
                countHR = 0;
            }
            countMR++;
            if (countMR < 0 || countMR > delayM) {
                countMR = 0;
            }
            countRR++;
            if (countRR < 0 || countRR > delayR) {
                countRR = 0;
            }
            countWR++;
            if (countWR < 0 || countWR > delayW) {
                countWR = 0;
            }
            double outKL = aKL[countKL - ((countKL > delayK) ? delayK + 1 : 0)];
            double outLL = aLL[countLL - ((countLL > delayL) ? delayL + 1 : 0)];
            double outML = aML[countML - ((countML > delayM) ? delayM + 1 : 0)];
            double outNL = aNL[countNL - ((countNL > delayN) ? delayN + 1 : 0)];
            double outOL = aOL[countOL - ((countOL > delayO) ? delayO + 1 : 0)];
            double outCR = aCR[countCR - ((countCR > delayC) ? delayC + 1 : 0)];
            double outHR = aHR[countHR - ((countHR > delayH) ? delayH + 1 : 0)];
            double outMR = aMR[countMR - ((countMR > delayM) ? delayM + 1 : 0)];
            double outRR = aRR[countRR - ((countRR > delayR) ? delayR + 1 : 0)];
            double outWR = aWR[countWR - ((countWR > delayW) ? delayW + 1 : 0)];
            //-------- mulch
            for (int x = 0; x < 31.32; x += 4) {
                double slew = ((outKL - pearC[x]) + pearC[x + 1]) * 0.416 * 0.5;
                pearC[x] = outKL = (0.416 * outKL) + ((1.0 - 0.416) * (pearC[x] + pearC[x + 1]));
                pearC[x + 1] = slew;
                slew = ((outCR - pearC[x + 2]) + pearC[x + 3]) * 0.416 * 0.5;
                pearC[x + 2] = outCR = (0.416 * outCR) + ((1.0 - 0.416) * (pearC[x + 2] + pearC[x + 3]));
                pearC[x + 3] = slew;
            }
            outSample = (outLL + prevMulchCL) * 0.5;
            prevMulchCL = outLL;
            outLL = outSample;
            outSample = (outHR + prevMulchCR) * 0.5;
            prevMulchCR = outHR;
            outHR = outSample;
            //-------- three
            aPL[countPL] = ((outKL * 3.0) - ((outLL + outML + outNL + outOL) * 2.0));
            aQL[countQL] = ((outLL * 3.0) - ((outKL + outML + outNL + outOL) * 2.0));
            aRL[countRL] = ((outML * 3.0) - ((outKL + outLL + outNL + outOL) * 2.0));
            aSL[countSL] = ((outNL * 3.0) - ((outKL + outLL + outML + outOL) * 2.0));
            aTL[countTL] = ((outOL * 3.0) - ((outKL + outLL + outML + outNL) * 2.0));
            aBR[countBR] = ((outCR * 3.0) - ((outHR + outMR + outRR + outWR) * 2.0));
            aGR[countGR] = ((outHR * 3.0) - ((outCR + outMR + outRR + outWR) * 2.0));
            aLR[countLR] = ((outMR * 3.0) - ((outCR + outHR + outRR + outWR) * 2.0));
            aQR[countQR] = ((outRR * 3.0) - ((outCR + outHR + outMR + outWR) * 2.0));
            aVR[countVR] = ((outWR * 3.0) - ((outCR + outHR + outMR + outRR) * 2.0));
            countPL++;
            if (countPL < 0 || countPL > delayP) {
                countPL = 0;
            }
            countQL++;
            if (countQL < 0 || countQL > delayQ) {
                countQL = 0;
            }
            countRL++;
            if (countRL < 0 || countRL > delayR) {
                countRL = 0;
            }
            countSL++;
            if (countSL < 0 || countSL > delayS) {
                countSL = 0;
            }
            countTL++;
            if (countTL < 0 || countTL > delayT) {
                countTL = 0;
            }
            countBR++;
            if (countBR < 0 || countBR > delayB) {
                countBR = 0;
            }
            countGR++;
            if (countGR < 0 || countGR > delayG) {
                countGR = 0;
            }
            countLR++;
            if (countLR < 0 || countLR > delayL) {
                countLR = 0;
            }
            countQR++;
            if (countQR < 0 || countQR > delayQ) {
                countQR = 0;
            }
            countVR++;
            if (countVR < 0 || countVR > delayV) {
                countVR = 0;
            }
            double outPL = aPL[countPL - ((countPL > delayP) ? delayP + 1 : 0)];
            double outQL = aQL[countQL - ((countQL > delayQ) ? delayQ + 1 : 0)];
            double outRL = aRL[countRL - ((countRL > delayR) ? delayR + 1 : 0)];
            double outSL = aSL[countSL - ((countSL > delayS) ? delayS + 1 : 0)];
            double outTL = aTL[countTL - ((countTL > delayT) ? delayT + 1 : 0)];
            double outBR = aBR[countBR - ((countBR > delayB) ? delayB + 1 : 0)];
            double outGR = aGR[countGR - ((countGR > delayG) ? delayG + 1 : 0)];
            double outLR = aLR[countLR - ((countLR > delayL) ? delayL + 1 : 0)];
            double outQR = aQR[countQR - ((countQR > delayQ) ? delayQ + 1 : 0)];
            double outVR = aVR[countVR - ((countVR > delayV) ? delayV + 1 : 0)];
            outSample = (outQL + prevMulchDL) * 0.5;
            prevMulchDL = outQL;
            outQL = outSample;
            outSample = (outGR + prevMulchDR) * 0.5;
            prevMulchDR = outGR;
            outGR = outSample;
            //-------- four
            aUL[countUL] = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0));
            aVL[countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
            aWL[countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
            aXL[countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
            aYL[countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));
            aAR[countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
            aFR[countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
            aKR[countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
            aPR[countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
            aUR[countUR] = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0));
            countUL++;
            if (countUL < 0 || countUL > delayU) {
                countUL = 0;
            }
            countVL++;
            if (countVL < 0 || countVL > delayV) {
                countVL = 0;
            }
            countWL++;
            if (countWL < 0 || countWL > delayW) {
                countWL = 0;
            }
            countXL++;
            if (countXL < 0 || countXL > delayX) {
                countXL = 0;
            }
            countYL++;
            if (countYL < 0 || countYL > delayY) {
                countYL = 0;
            }
            countAR++;
            if (countAR < 0 || countAR > delayA) {
                countAR = 0;
            }
            countFR++;
            if (countFR < 0 || countFR > delayF) {
                countFR = 0;
            }
            countKR++;
            if (countKR < 0 || countKR > delayK) {
                countKR = 0;
            }
            countPR++;
            if (countPR < 0 || countPR > delayP) {
                countPR = 0;
            }
            countUR++;
            if (countUR < 0 || countUR > delayU) {
                countUR = 0;
            }
            double outUL = aUL[countUL - ((countUL > delayU) ? delayU + 1 : 0)];
            double outVL = aVL[countVL - ((countVL > delayV) ? delayV + 1 : 0)];
            double outWL = aWL[countWL - ((countWL > delayW) ? delayW + 1 : 0)];
            double outXL = aXL[countXL - ((countXL > delayX) ? delayX + 1 : 0)];
            double outYL = aYL[countYL - ((countYL > delayY) ? delayY + 1 : 0)];
            double outAR = aAR[countAR - ((countAR > delayA) ? delayA + 1 : 0)];
            double outFR = aFR[countFR - ((countFR > delayF) ? delayF + 1 : 0)];
            double outKR = aKR[countKR - ((countKR > delayK) ? delayK + 1 : 0)];
            double outPR = aPR[countPR - ((countPR > delayP) ? delayP + 1 : 0)];
            double outUR = aUR[countUR - ((countUR > delayU) ? delayU + 1 : 0)];
            //-------- mulch
            outSample = (outVL + prevMulchEL) * 0.5;
            prevMulchEL = outVL;
            outVL = outSample;
            outSample = (outFR + prevMulchER) * 0.5;
            prevMulchER = outFR;
            outFR = outSample;
            //-------- five
            feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
            feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
            feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
            feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
            feedbackCL = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
            feedbackOR = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
            feedbackDL = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
            feedbackTR = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
            feedbackEL = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
            feedbackYR = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
            // which we need to feed back into the input again, a bit
            inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
            inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
            // and take the final combined sum of outputs, corrected for Householder gain
            inputSampleL += (earlyReflectionsL * 0.2);
            inputSampleR += (earlyReflectionsR * 0.2);
            inputSampleL *= 0.25;
            inputSampleR *= 0.25;
            if (gainOutL < 0.0078125) {
                gainOutL = 0.0078125;
            }
            if (gainOutL > 1.0) {
                gainOutL = 1.0;
            }
            if (gainOutR < 0.0078125) {
                gainOutR = 0.0078125;
            }
            if (gainOutR > 1.0) {
                gainOutR = 1.0;
            }
            // gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
            // smaller number is maximum clamping, if too small it'll take a while to bounce back
            inputSampleL *= gainOutL;
            inputSampleR *= gainOutR;
            gainOutL += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
            gainOutR += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
            // 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
            inputSampleL *= 4.0;
            inputSampleR *= 4.0;
            // curve! To get a compressed effect that matches a certain other plugin
            // that is too overprocessed for its own good :)
            outSample = (inputSampleL + prevOutEL) * 0.5;
            prevOutEL = inputSampleL;
            inputSampleL = outSample;
            outSample = (inputSampleR + prevOutER) * 0.5;
            prevOutER = inputSampleR;
            inputSampleR = outSample;
            if (cycleEnd == 4) {
                lastRefL[0] = lastRefL[4]; // start from previous last
                lastRefL[2] = (lastRefL[0] + inputSampleL) / 2; // half
                lastRefL[1] = (lastRefL[0] + lastRefL[2]) / 2; // one quarter
                lastRefL[3] = (lastRefL[2] + inputSampleL) / 2; // three quarters
                lastRefL[4] = inputSampleL; // full
                lastRefR[0] = lastRefR[4]; // start from previous last
                lastRefR[2] = (lastRefR[0] + inputSampleR) / 2; // half
                lastRefR[1] = (lastRefR[0] + lastRefR[2]) / 2; // one quarter
                lastRefR[3] = (lastRefR[2] + inputSampleR) / 2; // three quarters
                lastRefR[4] = inputSampleR; // full
            }
            if (cycleEnd == 3) {
                lastRefL[0] = lastRefL[3]; // start from previous last
                lastRefL[2] = (lastRefL[0] + lastRefL[0] + inputSampleL) / 3; // third
                lastRefL[1] = (lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
                lastRefL[3] = inputSampleL; // full
                lastRefR[0] = lastRefR[3]; // start from previous last
                lastRefR[2] = (lastRefR[0] + lastRefR[0] + inputSampleR) / 3; // third
                lastRefR[1] = (lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
                lastRefR[3] = inputSampleR; // full
            }
            if (cycleEnd == 2) {
                lastRefL[0] = lastRefL[2]; // start from previous last
                lastRefL[1] = (lastRefL[0] + inputSampleL) / 2; // half
                lastRefL[2] = inputSampleL; // full
                lastRefR[0] = lastRefR[2]; // start from previous last
                lastRefR[1] = (lastRefR[0] + inputSampleR) / 2; // half
                lastRefR[2] = inputSampleR; // full
            }
            if (cycleEnd == 1) {
                lastRefL[0] = inputSampleL;
                lastRefR[0] = inputSampleR;
            }
            cycle = 0; // reset
            inputSampleL = lastRefL[cycle];
            inputSampleR = lastRefR[cycle];
        }
        else {
            inputSampleL = lastRefL[cycle];
            inputSampleR = lastRefR[cycle];
            // we are going through our references now
        }
        inputSampleL *= 0.25;
        inputSampleR *= 0.25;
        if (inputSampleL > 2.8) {
            inputSampleL = 2.8;
        }
        if (inputSampleL < -2.8) {
            inputSampleL = -2.8;
        }
        if (inputSampleR > 2.8) {
            inputSampleR = 2.8;
        }
        if (inputSampleR < -2.8) {
            inputSampleR = -2.8; // clip BigFastArcSin harder
        }
        if (inputSampleL > 0.0) {
            inputSampleL = (inputSampleL * 2.0) / (2.8274333882308 - inputSampleL);
        }
        else {
            inputSampleL = -(inputSampleL * -2.0) / (2.8274333882308 + inputSampleL);
        }
        if (inputSampleR > 0.0) {
            inputSampleR = (inputSampleR * 2.0) / (2.8274333882308 - inputSampleR);
        }
        else {
            inputSampleR = -(inputSampleR * -2.0) / (2.8274333882308 + inputSampleR);
        }
        // BigFastArcSin output stage
        if (cycleEnd > 1) {
            double outSample = (inputSampleL + tailL) * 0.5;
            tailL = inputSampleL;
            inputSampleL = outSample;
            outSample = (inputSampleR + tailR) * 0.5;
            tailR = inputSampleR;
            inputSampleR = outSample;
        } // let's average only at elevated sample rates
        if (wet < 1.0) {
            inputSampleL *= wet;
            inputSampleR *= wet;
        }
        if (dry < 1.0) {
            drySampleL *= dry;
            drySampleR *= dry;
        }
        inputSampleL += drySampleL;
        inputSampleR += drySampleR;
        // this is our submix verb dry/wet: 0.5 is BOTH at FULL VOLUME
        // purpose is that, if you're adding verb, you're not altering other balances
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
} // namespace airwindohhs::kcathedral
