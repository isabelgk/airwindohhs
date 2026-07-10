#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::kplate140 {

constexpr std::string_view k_name{ "kPlate140" };
constexpr std::string_view k_short_description{
    "kPlate140 is a next-generation Airwindows plate reverb."
};
constexpr std::string_view k_long_description{
    "Time for the big plate!kPlate140 is a lot like the smaller version, kPlate240, to the point that you might ask, what's up with that? Can't you simply make the 240 version sustain a little longer, maybe dial in some EQ, and not need to have a whole separate plugin?Much like kPlate240, kPlate140 is a 5x5 Householder matrix with a plate-style delay density, Pear filters, and the use of SubTight. Where a lot of reverbs out there will be different EQ settings etc. on one basic algorithm (or a small number of them), it's true that it's got the same sliders doing the same things and if you can use one you can use the other… but the way these are designed involves generating thousands or hundreds of thousands of possible Householder matrices and testing them to try and work out what would give the best sound. That means they've got better during the time I've developed the technique… but it also means each new 'best algorithm' is unique. Sometimes there's a lucky break, like the original Galactic one (a 4x4) that was used again for Galactic3, sometimes not so much like the original kCathedral that people felt was metallic.And then when you're trying to do a literal plate which very much has its own sound qualities… and some years ago I did build a DIY real-world physical plate reverb, so I'm familiar with the sound though I couldn't get it up to EMT140 quality and didn't keep it… well, dialing in the sound involves getting all that stuff tuned up uniquely to the plugin in question.kPlate140 isn't meant to duplicate kPlate240. My take on the gold foil reverb is that it's cloudier, more understated: from reports of people who use both sorts, it's got a thing of its own but it's the big 375 pound monster (600 pounds packed for shipping) that really gets people's attention. And so, kPlate140 takes a different angle, with all of its parts tuned for that flashier, deeper, more fiery sound that's not part of the more subdued 240.140 plates come in all sounds and varieties: much like with the earlier attempt at plate reverbs, kPlateA through D, each one will sound different. kPlate140's the one that is yours, much like kPlate240 is. I'm hoping this will come in handy in situations where you're looking for a plate reverb plugin, but are going for that BIG sound. You get to have both! Have fun bathing in reverb :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class kPlate140 final : public Effect<T>
{
    static constexpr int predelay = 16000;
    static constexpr int vlfpredelay = 16000;
    static constexpr int delayA = 466;
    static constexpr int delayB = 233;
    static constexpr int delayC = 747;
    static constexpr int delayD = 70;
    static constexpr int delayE = 776;
    static constexpr int delayF = 719;
    static constexpr int delayG = 130;
    static constexpr int delayH = 511;
    static constexpr int delayI = 148;
    static constexpr int delayJ = 84;
    static constexpr int delayK = 787;
    static constexpr int delayL = 124;
    static constexpr int delayM = 72;
    static constexpr int delayN = 667;
    static constexpr int delayO = 773;
    static constexpr int delayP = 29;
    static constexpr int delayQ = 710;
    static constexpr int delayR = 309;
    static constexpr int delayS = 773;
    static constexpr int delayT = 487;
    static constexpr int delayU = 11;
    static constexpr int delayV = 73;
    static constexpr int delayW = 756;
    static constexpr int delayX = 627;
    static constexpr int delayY = 328;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'kpls';
    float A;
    float B;
    float C;
    float D;
    float E;
    double iirAL;
    double iirBL;
    double gainIn;
    double gainOutL;
    double gainOutR;
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
    double feedblurAL;
    double feedblurBL;
    double feedblurCL;
    double feedblurDL;
    double feedblurEL;
    double feedblurER;
    double feedblurJR;
    double feedblurOR;
    double feedblurTR;
    double feedblurYR;
    double sbAL;
    double sbBL;
    double sbCL;
    double sbDL;
    double sbEL;
    double sbER;
    double sbJR;
    double sbOR;
    double sbTR;
    double sbYR;
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
    }; // fixed frequency pear filter for ultrasonics, stereo
    double pearA[pear_total]; // probably worth just using a number here
    double pearB[pear_total]; // probably worth just using a number here
    double pearC[pear_total]; // probably worth just using a number here
    double pearD[pear_total]; // probably worth just using a number here
    double pearE[pear_total]; // probably worth just using a number here
    double pearF[pear_total]; // probably worth just using a number here
    double vibratoL;
    double vibratoR;
    double vibAL;
    double vibAR;
    double vibBL;
    double vibBR;
    double subAL;
    double subAR;
    double subBL;
    double subBR;
    double subCL;
    double subCR;
    double previousAL;
    double previousBL;
    double previousCL;
    double previousDL;
    double previousEL;
    double iirAR;
    double iirBR;
    double previousAR;
    double previousBR;
    double previousCR;
    double previousDR;
    double previousER;
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
    double prevMulchBL;
    double prevMulchBR;
    double prevMulchCL;
    double prevMulchCR;
    double prevMulchDL;
    double prevMulchDR;
    double prevMulchEL;
    double prevMulchER;
    double prevOutDL;
    double prevOutDR;
    double prevOutEL;
    double prevOutER;
    double prevInDL;
    double prevInDR;
    double prevInEL;
    double prevInER;
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_InL,
        bez_InR,
        bez_UnInL,
        bez_UnInR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    kPlate140()
    {
        A = 1.0;
        B = 0.5;
        C = 1.0;
        D = 0.0;
        E = 0.25;
        gainIn = gainOutL = gainOutR = 1.0;
        for (int count = 0; count < delayA + 2; count++) {
            aAL[count] = 0.0;
            aAR[count] = 0.0;
        }
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
        previousAL = 0.0;
        previousBL = 0.0;
        previousCL = 0.0;
        previousDL = 0.0;
        previousEL = 0.0;
        feedbackER = 0.0;
        feedbackJR = 0.0;
        feedbackOR = 0.0;
        feedbackTR = 0.0;
        feedbackYR = 0.0;
        previousAR = 0.0;
        previousBR = 0.0;
        previousCR = 0.0;
        previousDR = 0.0;
        previousER = 0.0;
        feedblurAL = 0.0;
        feedblurBL = 0.0;
        feedblurCL = 0.0;
        feedblurDL = 0.0;
        feedblurEL = 0.0;
        feedblurER = 0.0;
        feedblurJR = 0.0;
        feedblurOR = 0.0;
        feedblurTR = 0.0;
        feedblurYR = 0.0;
        sbAL = 0.0;
        sbBL = 0.0;
        sbCL = 0.0;
        sbDL = 0.0;
        sbEL = 0.0;
        sbER = 0.0;
        sbJR = 0.0;
        sbOR = 0.0;
        sbTR = 0.0;
        sbYR = 0.0;
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
        for (int x = 0; x < pear_total; x++) {
            pearA[x] = 0.0;
            pearB[x] = 0.0;
            pearC[x] = 0.0;
            pearD[x] = 0.0;
            pearE[x] = 0.0;
            pearF[x] = 0.0;
        }
        // from PearEQ
        vibratoL = vibAL = vibAR = vibBL = vibBR = 0.0;
        vibratoR = M_PI_4;
        subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
        // from SubTight
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.25; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "regen"; break;
            case kParamC: return "derez"; break;
            case kParamD: return "predlay"; break;
            case kParamE: return "wetness"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Regen"; break;
            case kParamC: return "DeRez"; break;
            case kParamD: return "Predlay"; break;
            case kParamE: return "Wetness"; break;

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
        double inputPad = A;
        double sbScale = pow(1.0 - B, 3) * -0.0000001;
        double sbRebound = (pow(B, 2) * 24.448) + 39.552;
        double blur = (1.0 - B) * 0.5;
        double regen = 1.0 - pow(1.0 - B, 2);
        regen = (regen * 0.0001) + 0.000195;
        double derez = C / overallscale;
        if (derez < 0.0005) {
            derez = 0.0005;
        }
        if (derez > 1.0) {
            derez = 1.0;
        }
        derez = 1.0 / ((int)(1.0 / derez));
        // this hard-locks derez to exact subdivisions of 1.0
        int adjPredelay = predelay * D * derez;
        int adjSubDelay = vlfpredelay * derez;
        double wet = E * 2.0;
        double dry = 2.0 - wet;
        if (wet > 1.0) {
            wet = 1.0;
        }
        else {
            wet *= wet;
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
            if (inputPad < 1.0) {
                inputSampleL *= inputPad;
                inputSampleR *= inputPad;
            }
            bez[bez_cycle] += derez;
            bez[bez_SampL] += ((inputSampleL + bez[bez_InL]) * derez);
            bez[bez_SampR] += ((inputSampleR + bez[bez_InR]) * derez);
            bez[bez_InL] = inputSampleL;
            bez[bez_InR] = inputSampleR;
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bez[bez_cycle] = 0.0;
                // predelay
                aZL[countZ] = bez[bez_SampL];
                aZR[countZ] = bez[bez_SampR];
                countZ++;
                if (countZ < 0 || countZ > adjPredelay) {
                    countZ = 0;
                }
                bez[bez_SampL] = aZL[countZ - ((countZ > adjPredelay) ? adjPredelay + 1 : 0)];
                bez[bez_SampR] = aZR[countZ - ((countZ > adjPredelay) ? adjPredelay + 1 : 0)];
                // end predelay
                double avgSampL = (bez[bez_SampL] + bez[bez_UnInL]) * 0.125;
                double avgSampR = (bez[bez_SampR] + bez[bez_UnInR]) * 0.125;
                bez[bez_UnInL] = bez[bez_SampL];
                bez[bez_UnInR] = bez[bez_SampR];
                // begin SubTight section
                double outSampleL = avgSampL * 0.00187;
                double outSampleR = avgSampR * 0.00187;
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
                avgSampL -= outSampleL;
                avgSampR -= outSampleR;
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
                avgSampL += outSampleL;
                avgSampR += outSampleR;
                // having re-added our VLF delayed channel we can now re-use outSample
                aAL[countAL] = avgSampL + (feedbackAL * regen);
                aBL[countBL] = avgSampL + (feedbackBL * regen);
                aCL[countCL] = avgSampL + (feedbackCL * regen);
                aDL[countDL] = avgSampL + (feedbackDL * regen);
                aEL[countEL] = avgSampL + (feedbackEL * regen);
                aER[countER] = avgSampR + (feedbackER * regen);
                aJR[countJR] = avgSampR + (feedbackJR * regen);
                aOR[countOR] = avgSampR + (feedbackOR * regen);
                aTR[countTR] = avgSampR + (feedbackTR * regen);
                aYR[countYR] = avgSampR + (feedbackYR * regen);
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
                //-------- four
                aVL[countVL] = ((outQL * 3.0) - ((outPL + outRL + outSL + outTL) * 2.0));
                aWL[countWL] = ((outRL * 3.0) - ((outPL + outQL + outSL + outTL) * 2.0));
                aXL[countXL] = ((outSL * 3.0) - ((outPL + outQL + outRL + outTL) * 2.0));
                aYL[countYL] = ((outTL * 3.0) - ((outPL + outQL + outRL + outSL) * 2.0));
                aAR[countAR] = ((outBR * 3.0) - ((outGR + outLR + outQR + outVR) * 2.0));
                aFR[countFR] = ((outGR * 3.0) - ((outBR + outLR + outQR + outVR) * 2.0));
                aKR[countKR] = ((outLR * 3.0) - ((outBR + outGR + outQR + outVR) * 2.0));
                aPR[countPR] = ((outQR * 3.0) - ((outBR + outGR + outLR + outVR) * 2.0));
                double outUL = ((outPL * 3.0) - ((outQL + outRL + outSL + outTL) * 2.0)) - (aUL[(countUL + 1) - ((countUL + 1 > delayU) ? delayU + 1 : 0)] * 0.618033988749894848204586);
                aUL[countUL] = outUL;
                outUL *= 0.618033988749894848204586;
                countUL++;
                if (countUL < 0 || countUL > delayU) {
                    countUL = 0;
                }
                outUL += aUL[countUL - ((countUL > delayU) ? delayU + 1 : 0)];
                // a delay slot becomes an allpass
                vibBL = vibAL;
                vibAL = outUL; // tiny two sample delay chains
                vibratoL += fpdL * 0.5e-13;
                if (vibratoL > M_PI * 2.0) {
                    vibratoL -= M_PI * 2.0;
                }
                double quadL = sin(vibratoL) + 1.0;
                if (quadL < 1.0) {
                    outUL = (outUL * (1.0 - quadL)) + (vibAL * quadL);
                }
                else {
                    outUL = (vibAL * (1.0 - (quadL - 1.0))) + (vibBL * (quadL - 1.0));
                }
                // also, pitch drift this allpass slot for very subtle motion
                double outUR = ((outVR * 3.0) - ((outBR + outGR + outLR + outQR) * 2.0)) - (aUR[(countUR + 1) - ((countUR + 1 > delayU) ? delayU + 1 : 0)] * 0.618033988749894848204586);
                aUR[countUR] = outUR;
                outUR *= 0.618033988749894848204586;
                countUR++;
                if (countUR < 0 || countUR > delayU) {
                    countUR = 0;
                }
                outUR += aUR[countUR - ((countUR > delayU) ? delayU + 1 : 0)];
                // a delay slot becomes an allpass
                vibBR = vibAR;
                vibAR = outUR; // tiny two sample delay chains
                vibratoR += fpdR * 0.5e-13;
                if (vibratoR > M_PI * 2.0) {
                    vibratoR -= M_PI * 2.0;
                }
                double quadR = sin(vibratoR) + 1.0;
                if (quadR < 1.0) {
                    outUR = (outUR * (1.0 - quadR)) + (vibAR * quadR);
                }
                else {
                    outUR = (vibAR * (1.0 - (quadR - 1.0))) + (vibBR * (quadR - 1.0));
                }
                // also, pitch drift this allpass slot for very subtle motion
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
                double outVL = aVL[countVL - ((countVL > delayV) ? delayV + 1 : 0)];
                double outWL = aWL[countWL - ((countWL > delayW) ? delayW + 1 : 0)];
                double outXL = aXL[countXL - ((countXL > delayX) ? delayX + 1 : 0)];
                double outYL = aYL[countYL - ((countYL > delayY) ? delayY + 1 : 0)];
                double outAR = aAR[countAR - ((countAR > delayA) ? delayA + 1 : 0)];
                double outFR = aFR[countFR - ((countFR > delayF) ? delayF + 1 : 0)];
                double outKR = aKR[countKR - ((countKR > delayK) ? delayK + 1 : 0)];
                double outPR = aPR[countPR - ((countPR > delayP) ? delayP + 1 : 0)];
                //-------- five
                feedbackER = ((outUL * 3.0) - ((outVL + outWL + outXL + outYL) * 2.0));
                feedbackAL = ((outAR * 3.0) - ((outFR + outKR + outPR + outUR) * 2.0));
                feedbackJR = ((outVL * 3.0) - ((outUL + outWL + outXL + outYL) * 2.0));
                feedbackBL = ((outFR * 3.0) - ((outAR + outKR + outPR + outUR) * 2.0));
                feedbackOR = ((outWL * 3.0) - ((outUL + outVL + outXL + outYL) * 2.0));
                feedbackCL = ((outKR * 3.0) - ((outAR + outFR + outPR + outUR) * 2.0));
                feedbackTR = ((outXL * 3.0) - ((outUL + outVL + outWL + outYL) * 2.0));
                feedbackDL = ((outPR * 3.0) - ((outAR + outFR + outKR + outUR) * 2.0));
                feedbackYR = ((outYL * 3.0) - ((outUL + outVL + outWL + outXL) * 2.0));
                feedbackEL = ((outUR * 3.0) - ((outAR + outFR + outKR + outPR) * 2.0));
                // which we need to feed back into the input again, a bit
                if (fabs(feedbackER) < 2000.0) {
                    feedbackER += (2.0 * feedbackER * feedbackER) * sbER;
                    sbER += ((feedbackER - sin(feedbackER)) * sbScale);
                }
                sbER = sin(sbER * 0.015625) * sbRebound;
                if (fabs(feedbackAL) < 2000.0) {
                    feedbackAL += (2.0 * feedbackAL * feedbackAL) * sbAL;
                    sbAL += ((feedbackAL - sin(feedbackAL)) * sbScale);
                }
                sbAL = sin(sbAL * 0.015625) * sbRebound;
                if (fabs(feedbackJR) < 2000.0) {
                    feedbackJR += (2.0 * feedbackJR * feedbackJR) * sbJR;
                    sbJR += ((feedbackJR - sin(feedbackJR)) * sbScale);
                }
                sbJR = sin(sbJR * 0.015625) * sbRebound;
                if (fabs(feedbackBL) < 2000.0) {
                    feedbackBL += (2.0 * feedbackBL * feedbackBL) * sbBL;
                    sbBL += ((feedbackBL - sin(feedbackBL)) * sbScale);
                }
                sbBL = sin(sbBL * 0.015625) * sbRebound;
                if (fabs(feedbackOR) < 2000.0) {
                    feedbackOR += (2.0 * feedbackOR * feedbackOR) * sbOR;
                    sbOR += ((feedbackOR - sin(feedbackOR)) * sbScale);
                }
                sbOR = sin(sbOR * 0.015625) * sbRebound;
                if (fabs(feedbackCL) < 2000.0) {
                    feedbackCL += (2.0 * feedbackCL * feedbackCL) * sbCL;
                    sbCL += ((feedbackCL - sin(feedbackCL)) * sbScale);
                }
                sbCL = sin(sbCL * 0.015625) * sbRebound;
                if (fabs(feedbackTR) < 2000.0) {
                    feedbackTR += (2.0 * feedbackTR * feedbackTR) * sbTR;
                    sbTR += ((feedbackTR - sin(feedbackTR)) * sbScale);
                }
                sbTR = sin(sbTR * 0.015625) * sbRebound;
                if (fabs(feedbackDL) < 2000.0) {
                    feedbackDL += (2.0 * feedbackDL * feedbackDL) * sbDL;
                    sbDL += ((feedbackDL - sin(feedbackDL)) * sbScale);
                }
                sbDL = sin(sbDL * 0.015625) * sbRebound;
                if (fabs(feedbackYR) < 2000.0) {
                    feedbackYR += (2.0 * feedbackYR * feedbackYR) * sbYR;
                    sbYR += ((feedbackYR - sin(feedbackYR)) * sbScale);
                }
                sbYR = sin(sbYR * 0.015625) * sbRebound;
                if (fabs(feedbackEL) < 2000.0) {
                    feedbackEL += (2.0 * feedbackEL * feedbackEL) * sbEL;
                    sbEL += ((feedbackEL - sin(feedbackEL)) * sbScale);
                }
                sbEL = sin(sbEL * 0.015625) * sbRebound;
                double temp;
                temp = ((feedbackER * (1.0 - blur)) + (feedblurER * blur));
                feedblurER = feedbackER;
                feedbackER = temp;
                temp = ((feedbackAL * (1.0 - blur)) + (feedblurAL * blur));
                feedblurAL = feedbackAL;
                feedbackAL = temp;
                temp = ((feedbackJR * (1.0 - blur)) + (feedblurJR * blur));
                feedblurJR = feedbackJR;
                feedbackJR = temp;
                temp = ((feedbackBL * (1.0 - blur)) + (feedblurBL * blur));
                feedblurBL = feedbackBL;
                feedbackBL = temp;
                temp = ((feedbackOR * (1.0 - blur)) + (feedblurOR * blur));
                feedblurOR = feedbackOR;
                feedbackOR = temp;
                temp = ((feedbackCL * (1.0 - blur)) + (feedblurCL * blur));
                feedblurCL = feedbackCL;
                feedbackCL = temp;
                temp = ((feedbackTR * (1.0 - blur)) + (feedblurTR * blur));
                feedblurTR = feedbackTR;
                feedbackTR = temp;
                temp = ((feedbackDL * (1.0 - blur)) + (feedblurDL * blur));
                feedblurDL = feedbackDL;
                feedbackDL = temp;
                temp = ((feedbackYR * (1.0 - blur)) + (feedblurYR * blur));
                feedblurYR = feedbackYR;
                feedbackYR = temp;
                temp = ((feedbackEL * (1.0 - blur)) + (feedblurEL * blur));
                feedblurEL = feedbackEL;
                feedbackEL = temp;
                inputSampleL = (outUL + outVL + outWL + outXL + outYL) * 0.0016;
                inputSampleR = (outAR + outFR + outKR + outPR + outUR) * 0.0016;
                // and take the final combined sum of outputs, corrected for Householder gain
                bez[bez_CL] = bez[bez_BL];
                bez[bez_BL] = bez[bez_AL];
                bez[bez_AL] = inputSampleL;
                bez[bez_SampL] = 0.0;
                bez[bez_CR] = bez[bez_BR];
                bez[bez_BR] = bez[bez_AR];
                bez[bez_AR] = inputSampleR;
                bez[bez_SampR] = 0.0;
            }
            double CBL = (bez[bez_CL] * (1.0 - bez[bez_cycle])) + (bez[bez_BL] * bez[bez_cycle]);
            double CBR = (bez[bez_CR] * (1.0 - bez[bez_cycle])) + (bez[bez_BR] * bez[bez_cycle]);
            double BAL = (bez[bez_BL] * (1.0 - bez[bez_cycle])) + (bez[bez_AL] * bez[bez_cycle]);
            double BAR = (bez[bez_BR] * (1.0 - bez[bez_cycle])) + (bez[bez_AR] * bez[bez_cycle]);
            double CBAL = (bez[bez_BL] + (CBL * (1.0 - bez[bez_cycle])) + (BAL * bez[bez_cycle])) * 0.125;
            double CBAR = (bez[bez_BR] + (CBR * (1.0 - bez[bez_cycle])) + (BAR * bez[bez_cycle])) * 0.125;
            inputSampleL = CBAL;
            inputSampleR = CBAR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
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
} // namespace airwindohhs::kplate140
