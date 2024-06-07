#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::infinity2 {

constexpr std::string_view k_name{ "Infinity2" };
constexpr std::string_view k_short_description{
    "Infinity2 expands Infinity with more tone shaping and a much-requested kill-switch!"
};
constexpr std::string_view k_long_description{
    "Here’s a much-requested upgrade! Infinity has been working out surprisingly well for people (surprising me, anyway, I got so excited about the feedforward reverb topologies that I thought Infinity was far too old school) except there’s quite a few DAWs that keep the plugin ‘live’ permanently if you’ve got it loaded. That, I didn’t expect. And so, there’s folks who are still sitting there waiting for their Infinity to clear out so they can add some new sounds to it.And since it’s Infinity, they’ll be waiting a very very long time.Until now!Infinity2 is carefully kept to allow exactly the same tones as before. No undersampling, no changes, just the addition of a Feedback control as well as the Damping control, so you can shut the thing off (or turn it to a traditional dual-mono reverb if you wish).But of course that would be too easy: so, more controls! Damping now works on all the reverb paths so to get the previous amount, set it to 0.125 or so. Zero is still full brightness, and now turning it up to 1 gives you a more significant treble fall-off effect. It’s possible that this can interact with frequencies to cause feedback reinforcement, so keep an eye out for that.The allpasses at the beginning (which spread and smooth incoming audio) can now be bypassed with another control, for brighter, more transient-rich input sounds.The maximum verb size (tied to sample rate like the original) is still exactly the same, but Infinity2 now lets you crank the pitch up to a LOT higher for when you’re doing weird effects with the plugin. That seemed like a nice tweak: if you don’t like it, try to not move the slider that far to the left :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class Infinity2 final : public Effect<T>
{
    double biquadA[11];
    double biquadB[11];
    double biquadC[11];
    double aAL[8111];
    double aBL[7511];
    double aCL[7311];
    double aDL[6911];
    double aEL[6311];
    double aFL[6111];
    double aGL[5511];
    double aHL[4911];
    double aIL[4511];
    double aJL[4311];
    double aKL[3911];
    double aLL[3311];
    double aML[3111];
    double aAR[8111];
    double aBR[7511];
    double aCR[7311];
    double aDR[6911];
    double aER[6311];
    double aFR[6111];
    double aGR[5511];
    double aHR[4911];
    double aIR[4511];
    double aJR[4311];
    double aKR[3911];
    double aLR[3311];
    double aMR[3111];
    int countA, delayA;
    int countB, delayB;
    int countC, delayC;
    int countD, delayD;
    int countE, delayE;
    int countF, delayF;
    int countG, delayG;
    int countH, delayH;
    int countI, delayI;
    int countJ, delayJ;
    int countK, delayK;
    int countL, delayL;
    int countM, delayM;
    double feedbackAL;
    double feedbackBL;
    double feedbackCL;
    double feedbackDL;
    double feedbackEL;
    double feedbackFL;
    double feedbackGL;
    double feedbackHL;
    double feedbackAR;
    double feedbackBR;
    double feedbackCR;
    double feedbackDR;
    double feedbackER;
    double feedbackFR;
    double feedbackGR;
    double feedbackHR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;

  public:
    Infinity2()
    {
        for (int x = 0; x < 11; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
        }
        feedbackAL = feedbackAR = 0.0;
        feedbackBL = feedbackBR = 0.0;
        feedbackCL = feedbackCR = 0.0;
        feedbackDL = feedbackDR = 0.0;
        feedbackEL = feedbackER = 0.0;
        feedbackFL = feedbackFR = 0.0;
        feedbackGL = feedbackGR = 0.0;
        feedbackHL = feedbackHR = 0.0;
        int count;
        for (count = 0; count < 8110; count++) {
            aAL[count] = aAR[count] = 0.0;
        }
        for (count = 0; count < 7510; count++) {
            aBL[count] = aBR[count] = 0.0;
        }
        for (count = 0; count < 7310; count++) {
            aCL[count] = aCR[count] = 0.0;
        }
        for (count = 0; count < 6910; count++) {
            aDL[count] = aDR[count] = 0.0;
        }
        for (count = 0; count < 6310; count++) {
            aEL[count] = aER[count] = 0.0;
        }
        for (count = 0; count < 6110; count++) {
            aFL[count] = aFR[count] = 0.0;
        }
        for (count = 0; count < 5510; count++) {
            aGL[count] = aGR[count] = 0.0;
        }
        for (count = 0; count < 4910; count++) {
            aHL[count] = aHR[count] = 0.0;
        }
        // maximum value needed will be delay * 100, plus 206 (absolute max vibrato depth)
        for (count = 0; count < 4510; count++) {
            aIL[count] = aIR[count] = 0.0;
        }
        for (count = 0; count < 4310; count++) {
            aJL[count] = aJR[count] = 0.0;
        }
        for (count = 0; count < 3910; count++) {
            aKL[count] = aKR[count] = 0.0;
        }
        for (count = 0; count < 3310; count++) {
            aLL[count] = aLR[count] = 0.0;
        }
        // maximum value will be delay * 100
        countA = 1;
        delayA = 79;
        countB = 1;
        delayB = 73;
        countC = 1;
        delayC = 71;
        countD = 1;
        delayD = 67;
        countE = 1;
        delayE = 61;
        countF = 1;
        delayF = 59;
        countG = 1;
        delayG = 53;
        countH = 1;
        delayH = 47;
        // the householder matrices
        countI = 1;
        delayI = 43;
        countJ = 1;
        delayJ = 41;
        countK = 1;
        delayK = 37;
        countL = 1;
        delayL = 31;
        // the allpasses
        A = 1.0;
        B = 0.5;
        C = 0.0;
        D = 1.0;
        E = 1.0;
        F = 1.0;
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
        kParamF = 5,
        kNumParameters = 6
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
        kParamF:
            F = value;
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
        kParamF:
            return F;
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
        kParamB:
            return 1.0;
            break;
        kParamC:
            return 1.0;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
            return 1.0;
            break;
        kParamF:
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
            return "filter";
            break;
        kParamB:
            return "size";
            break;
        kParamC:
            return "damping";
            break;
        kParamD:
            return "allpass";
            break;
        kParamE:
            return "feedback";
            break;
        kParamF:
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
            return "Filter";
            break;
        kParamB:
            return "Size";
            break;
        kParamC:
            return "Damping";
            break;
        kParamD:
            return "Allpass";
            break;
        kParamE:
            return "Feedback";
            break;
        kParamF:
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
        kParamF:
            return std::to_string(F);
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
        kParamF:
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

        biquadC[0] = biquadB[0] = biquadA[0] = ((pow(A, 2) * 9900.0) + 100.0) / getSampleRate();
        biquadA[1] = 0.618033988749894848204586;
        biquadB[1] = (A * 0.5) + 0.118033988749894848204586;
        biquadC[1] = 0.5;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        double size = (pow(B, 2) * 99.0) + 1.0;
        double damping = pow(C, 2) * 0.5;
        double rawPass = D;
        double feedback = 1.0 - (pow(1.0 - E, 4));
        double wet = F;
        delayA = 79 * size;
        delayB = 73 * size;
        delayC = 71 * size;
        delayD = 67 * size;
        delayE = 61 * size;
        delayF = 59 * size;
        delayG = 53 * size;
        delayH = 47 * size;
        delayI = 43 * size;
        delayJ = 41 * size;
        delayK = 37 * size;
        delayL = 31 * size;
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
            double tempSampleL = (inputSampleL * biquadA[2]) + biquadA[7];
            biquadA[7] = (inputSampleL * biquadA[3]) - (tempSampleL * biquadA[5]) + biquadA[8];
            biquadA[8] = (inputSampleL * biquadA[4]) - (tempSampleL * biquadA[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquadA[2]) + biquadA[9];
            biquadA[9] = (inputSampleR * biquadA[3]) - (tempSampleR * biquadA[5]) + biquadA[10];
            biquadA[10] = (inputSampleR * biquadA[4]) - (tempSampleR * biquadA[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
            double allpassIL = inputSampleL;
            double allpassJL = inputSampleL;
            double allpassKL = inputSampleL;
            double allpassLL = inputSampleL;
            double allpassIR = inputSampleR;
            double allpassJR = inputSampleR;
            double allpassKR = inputSampleR;
            double allpassLR = inputSampleR;
            int allpasstemp = countI + 1;
            if (allpasstemp < 0 || allpasstemp > delayI) {
                allpasstemp = 0;
            }
            allpassIL -= aIL[allpasstemp] * 0.5;
            aIL[countI] = allpassIL;
            allpassIL *= 0.5;
            allpassIR -= aIR[allpasstemp] * 0.5;
            aIR[countI] = allpassIR;
            allpassIR *= 0.5;
            countI++;
            if (countI < 0 || countI > delayI) {
                countI = 0;
            }
            allpassIL += (aIL[countI]);
            allpassIR += (aIR[countI]);
            allpasstemp = countJ + 1;
            if (allpasstemp < 0 || allpasstemp > delayJ) {
                allpasstemp = 0;
            }
            allpassJL -= aJL[allpasstemp] * 0.5;
            aJL[countJ] = allpassJL;
            allpassJL *= 0.5;
            allpassJR -= aJR[allpasstemp] * 0.5;
            aJR[countJ] = allpassJR;
            allpassJR *= 0.5;
            countJ++;
            if (countJ < 0 || countJ > delayJ) {
                countJ = 0;
            }
            allpassJL += (aJL[countJ]);
            allpassJR += (aJR[countJ]);
            allpasstemp = countK + 1;
            if (allpasstemp < 0 || allpasstemp > delayK) {
                allpasstemp = 0;
            }
            allpassKL -= aKL[allpasstemp] * 0.5;
            aKL[countK] = allpassKL;
            allpassKL *= 0.5;
            allpassKR -= aKR[allpasstemp] * 0.5;
            aKR[countK] = allpassKR;
            allpassKR *= 0.5;
            countK++;
            if (countK < 0 || countK > delayK) {
                countK = 0;
            }
            allpassKL += (aKL[countK]);
            allpassKR += (aKR[countK]);
            allpasstemp = countL + 1;
            if (allpasstemp < 0 || allpasstemp > delayL) {
                allpasstemp = 0;
            }
            allpassLL -= aLL[allpasstemp] * 0.5;
            aLL[countL] = allpassLL;
            allpassLL *= 0.5;
            allpassLR -= aLR[allpasstemp] * 0.5;
            aLR[countL] = allpassLR;
            allpassLR *= 0.5;
            countL++;
            if (countL < 0 || countL > delayL) {
                countL = 0;
            }
            allpassLL += (aLL[countL]);
            allpassLR += (aLR[countL]);
            // the big allpass in front of everything
            if (rawPass != 1.0) {
                allpassIL = (allpassIL * rawPass) + (drySampleL * (1.0 - rawPass));
                allpassJL = (allpassJL * rawPass) + (drySampleL * (1.0 - rawPass));
                allpassKL = (allpassKL * rawPass) + (drySampleL * (1.0 - rawPass));
                allpassLL = (allpassLL * rawPass) + (drySampleL * (1.0 - rawPass));
                allpassIR = (allpassIR * rawPass) + (drySampleR * (1.0 - rawPass));
                allpassJR = (allpassJR * rawPass) + (drySampleR * (1.0 - rawPass));
                allpassKR = (allpassKR * rawPass) + (drySampleR * (1.0 - rawPass));
                allpassLR = (allpassLR * rawPass) + (drySampleR * (1.0 - rawPass));
            }
            aAL[countA] = allpassIL + (feedbackAL * feedback);
            aBL[countB] = allpassJL + (feedbackBL * feedback);
            aCL[countC] = allpassKL + (feedbackCL * feedback);
            aDL[countD] = allpassLL + (feedbackDL * feedback);
            aEL[countE] = allpassIL + (feedbackEL * feedback);
            aFL[countF] = allpassJL + (feedbackFL * feedback);
            aGL[countG] = allpassKL + (feedbackGL * feedback);
            aHL[countH] = allpassLL + (feedbackHL * feedback); // L
            aAR[countA] = allpassIR + (feedbackAR * feedback);
            aBR[countB] = allpassJR + (feedbackBR * feedback);
            aCR[countC] = allpassKR + (feedbackCR * feedback);
            aDR[countD] = allpassLR + (feedbackDR * feedback);
            aER[countE] = allpassIR + (feedbackER * feedback);
            aFR[countF] = allpassJR + (feedbackFR * feedback);
            aGR[countG] = allpassKR + (feedbackGR * feedback);
            aHR[countH] = allpassLR + (feedbackHR * feedback); // R
            countA++;
            if (countA < 0 || countA > delayA) {
                countA = 0;
            }
            countB++;
            if (countB < 0 || countB > delayB) {
                countB = 0;
            }
            countC++;
            if (countC < 0 || countC > delayC) {
                countC = 0;
            }
            countD++;
            if (countD < 0 || countD > delayD) {
                countD = 0;
            }
            countE++;
            if (countE < 0 || countE > delayE) {
                countE = 0;
            }
            countF++;
            if (countF < 0 || countF > delayF) {
                countF = 0;
            }
            countG++;
            if (countG < 0 || countG > delayG) {
                countG = 0;
            }
            countH++;
            if (countH < 0 || countH > delayH) {
                countH = 0;
            }
            // the Householder matrices (shared between channels, offset is stereo)
            double infiniteAL = (aAL[countA - ((countA > delayA) ? delayA + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteAL += (aAL[countA + 1 - ((countA + 1 > delayA) ? delayA + 1 : 0)] * ((damping - floor(damping))));
            double infiniteBL = (aBL[countB - ((countB > delayB) ? delayB + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteBL += (aBL[countB + 1 - ((countB + 1 > delayB) ? delayB + 1 : 0)] * ((damping - floor(damping))));
            double infiniteCL = (aCL[countC - ((countC > delayC) ? delayC + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteCL += (aCL[countC + 1 - ((countC + 1 > delayC) ? delayC + 1 : 0)] * ((damping - floor(damping))));
            double infiniteDL = (aDL[countD - ((countD > delayD) ? delayD + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteDL += (aDL[countD + 1 - ((countD + 1 > delayD) ? delayD + 1 : 0)] * ((damping - floor(damping))));
            double infiniteAR = (aAR[countA - ((countA > delayA) ? delayA + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteAR += (aAR[countA + 1 - ((countA + 1 > delayA) ? delayA + 1 : 0)] * ((damping - floor(damping))));
            double infiniteBR = (aBR[countB - ((countB > delayB) ? delayB + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteBR += (aBR[countB + 1 - ((countB + 1 > delayB) ? delayB + 1 : 0)] * ((damping - floor(damping))));
            double infiniteCR = (aCR[countC - ((countC > delayC) ? delayC + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteCR += (aCR[countC + 1 - ((countC + 1 > delayC) ? delayC + 1 : 0)] * ((damping - floor(damping))));
            double infiniteDR = (aDR[countD - ((countD > delayD) ? delayD + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteDR += (aDR[countD + 1 - ((countD + 1 > delayD) ? delayD + 1 : 0)] * ((damping - floor(damping))));
            double infiniteEL = (aEL[countE - ((countE > delayE) ? delayE + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteEL += (aEL[countE + 1 - ((countE + 1 > delayE) ? delayE + 1 : 0)] * ((damping - floor(damping))));
            double infiniteFL = (aFL[countF - ((countF > delayF) ? delayF + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteFL += (aFL[countF + 1 - ((countF + 1 > delayF) ? delayF + 1 : 0)] * ((damping - floor(damping))));
            double infiniteGL = (aGL[countG - ((countG > delayG) ? delayG + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteGL += (aGL[countG + 1 - ((countG + 1 > delayG) ? delayG + 1 : 0)] * ((damping - floor(damping))));
            double infiniteHL = (aHL[countH - ((countH > delayH) ? delayH + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteHL += (aHL[countH + 1 - ((countH + 1 > delayH) ? delayH + 1 : 0)] * ((damping - floor(damping))));
            double infiniteER = (aER[countE - ((countE > delayE) ? delayE + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteER += (aER[countE + 1 - ((countE + 1 > delayE) ? delayE + 1 : 0)] * ((damping - floor(damping))));
            double infiniteFR = (aFR[countF - ((countF > delayF) ? delayF + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteFR += (aFR[countF + 1 - ((countF + 1 > delayF) ? delayF + 1 : 0)] * ((damping - floor(damping))));
            double infiniteGR = (aGR[countG - ((countG > delayG) ? delayG + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteGR += (aGR[countG + 1 - ((countG + 1 > delayG) ? delayG + 1 : 0)] * ((damping - floor(damping))));
            double infiniteHR = (aHR[countH - ((countH > delayH) ? delayH + 1 : 0)] * (1 - (damping - floor(damping))));
            infiniteHR += (aHR[countH + 1 - ((countH + 1 > delayH) ? delayH + 1 : 0)] * ((damping - floor(damping))));
            double dialBackAL = 0.5;
            double dialBackEL = 0.5;
            double dialBackDryL = 0.5;
            if (fabs(infiniteAL) > 0.4) {
                dialBackAL -= ((fabs(infiniteAL) - 0.4) * 0.2);
            }
            if (fabs(infiniteEL) > 0.4) {
                dialBackEL -= ((fabs(infiniteEL) - 0.4) * 0.2);
            }
            if (fabs(drySampleL) > 0.4) {
                dialBackDryL -= ((fabs(drySampleL) - 0.4) * 0.2);
            }
            // we're compressing things subtly so we can feed energy in and not overload
            double dialBackAR = 0.5;
            double dialBackER = 0.5;
            double dialBackDryR = 0.5;
            if (fabs(infiniteAR) > 0.4) {
                dialBackAR -= ((fabs(infiniteAR) - 0.4) * 0.2);
            }
            if (fabs(infiniteER) > 0.4) {
                dialBackER -= ((fabs(infiniteER) - 0.4) * 0.2);
            }
            if (fabs(drySampleR) > 0.4) {
                dialBackDryR -= ((fabs(drySampleR) - 0.4) * 0.2);
            }
            // we're compressing things subtly so we can feed energy in and not overload
            feedbackAL = (infiniteAL - (infiniteBL + infiniteCL + infiniteDL)) * dialBackAL;
            feedbackBL = (infiniteBL - (infiniteAL + infiniteCL + infiniteDL)) * dialBackDryL;
            feedbackCL = (infiniteCL - (infiniteAL + infiniteBL + infiniteDL)) * dialBackDryL;
            feedbackDL = (infiniteDL - (infiniteAL + infiniteBL + infiniteCL)) * dialBackDryL; // Householder feedback matrix, L
            feedbackEL = (infiniteEL - (infiniteFL + infiniteGL + infiniteHL)) * dialBackEL;
            feedbackFL = (infiniteFL - (infiniteEL + infiniteGL + infiniteHL)) * dialBackDryL;
            feedbackGL = (infiniteGL - (infiniteEL + infiniteFL + infiniteHL)) * dialBackDryL;
            feedbackHL = (infiniteHL - (infiniteEL + infiniteFL + infiniteGL)) * dialBackDryL; // Householder feedback matrix, L
            feedbackAR = (infiniteAR - (infiniteBR + infiniteCR + infiniteDR)) * dialBackAR;
            feedbackBR = (infiniteBR - (infiniteAR + infiniteCR + infiniteDR)) * dialBackDryR;
            feedbackCR = (infiniteCR - (infiniteAR + infiniteBR + infiniteDR)) * dialBackDryR;
            feedbackDR = (infiniteDR - (infiniteAR + infiniteBR + infiniteCR)) * dialBackDryR; // Householder feedback matrix, R
            feedbackER = (infiniteER - (infiniteFR + infiniteGR + infiniteHR)) * dialBackER;
            feedbackFR = (infiniteFR - (infiniteER + infiniteGR + infiniteHR)) * dialBackDryR;
            feedbackGR = (infiniteGR - (infiniteER + infiniteFR + infiniteHR)) * dialBackDryR;
            feedbackHR = (infiniteHR - (infiniteER + infiniteFR + infiniteGR)) * dialBackDryR; // Householder feedback matrix, R
            inputSampleL = (infiniteAL + infiniteBL + infiniteCL + infiniteDL + infiniteEL + infiniteFL + infiniteGL + infiniteHL) / 8.0;
            inputSampleR = (infiniteAR + infiniteBR + infiniteCR + infiniteDR + infiniteER + infiniteFR + infiniteGR + infiniteHR) / 8.0;
            tempSampleL = (inputSampleL * biquadB[2]) + biquadB[7];
            biquadB[7] = (inputSampleL * biquadB[3]) - (tempSampleL * biquadB[5]) + biquadB[8];
            biquadB[8] = (inputSampleL * biquadB[4]) - (tempSampleL * biquadB[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadB[2]) + biquadB[9];
            biquadB[9] = (inputSampleR * biquadB[3]) - (tempSampleR * biquadB[5]) + biquadB[10];
            biquadB[10] = (inputSampleR * biquadB[4]) - (tempSampleR * biquadB[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            tempSampleL = (inputSampleL * biquadC[2]) + biquadC[7];
            biquadC[7] = (inputSampleL * biquadC[3]) - (tempSampleL * biquadC[5]) + biquadC[8];
            biquadC[8] = (inputSampleL * biquadC[4]) - (tempSampleL * biquadC[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadC[2]) + biquadC[9];
            biquadC[9] = (inputSampleR * biquadC[3]) - (tempSampleR * biquadC[5]) + biquadC[10];
            biquadC[10] = (inputSampleR * biquadC[4]) - (tempSampleR * biquadC[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
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
} // namespace airwindohhs::infinity2
