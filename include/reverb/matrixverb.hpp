#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::matrixverb {

constexpr std::string_view k_name{ "MatrixVerb" };
constexpr std::string_view k_short_description{
    "MatrixVerb is a wildly adjustable, strangely colorful reverb for deep and flexible spaces."
};
constexpr std::string_view k_long_description{
    "First, the Swiss Army Knife reverb! (a more cooperative one is to follow)This is the result of some deep diving into Householder reverb algorithms (a way of taking four delay lines and turning them into infinite reverb). It’s different from anything I’ve done before, reverb-wise: extremely flexible, and incorporating some neat new tricks (for instance, the highs fall away at the same rate they would in a giant concrete cavern, allowing for REALLY huge-sounding spaces as well as convincing smaller spaces). It’s actually two parallel Householder reverbs in the place of one.But what would happen if you had them feed back into each other, not just into themselves?Turns out two different things can happen. One is a twisting and distorting of the sonic space into a distinctly… SPRING-like tonality. If you push the flavor knob towards 1.0, you increasingly get that clangy spring reverb thing, either subtly or overbearingly.The other is this: apparently the opposite of a spring is a plate. Because when the Householder tanks feed back into each other inverted, they cancel out those same things and produce a whonging booming dense solidness that I remember, very well, from building a REAL plate reverb out of a big sheet of steel hanging from springs. I should say that real one I built was not a GOOD plate reverb… but I remember what it was like, and I can bring back the feel of it with this strange beast and its inverted broken feedback thing. This is one of those Airwindows plugins that lets you cut off your own foot with giddy abandon: all the bad settings are totally available. But if you know how to tune it, you might get something quite magical.You get damping (from ‘almost’ infinite reverb down to very very damped), an overall tone control to handle whether the verb is bright or not, the ‘flavor’ control that leans either platey or springy or neutral, a room size that will go unreasonably huge even at 96k, and dry/wet. Stay tuned for stuff that’s more ‘preset’ and always gives you useful/good settings: that’s coming too. But this one is the reverb of doom: the most wild range of settings and tonalities and spaces, and the neat thing is apart from the global tone control at top, it’s all about manipulating the heart of the algorithm in significant ways. None of the adjustments are arbitrary: for the range of useful tonality, this reverb is very simple to operate."
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class MatrixVerb final : public Effect<T>
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
    double feedbackAL, vibAL, depthA;
    double feedbackBL, vibBL, depthB;
    double feedbackCL, vibCL, depthC;
    double feedbackDL, vibDL, depthD;
    double feedbackEL, vibEL, depthE;
    double feedbackFL, vibFL, depthF;
    double feedbackGL, vibGL, depthG;
    double feedbackHL, vibHL, depthH;
    double feedbackAR, vibAR;
    double feedbackBR, vibBR;
    double feedbackCR, vibCR;
    double feedbackDR, vibDR;
    double feedbackER, vibER;
    double feedbackFR, vibFR;
    double feedbackGR, vibGR;
    double feedbackHR, vibHR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;

  public:
    MatrixVerb()
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
        for (count = 0; count < 3110; count++) {
            aML[count] = aMR[count] = 0.0;
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
        countM = 1;
        delayM = 29;
        // the predelay
        depthA = 0.003251;
        depthB = 0.002999;
        depthC = 0.002917;
        depthD = 0.002749;
        depthE = 0.002503;
        depthF = 0.002423;
        depthG = 0.002146;
        depthH = 0.002088;
        // the individual vibrato rates for the delays
        vibAL = rand() * -2147483647;
        vibBL = rand() * -2147483647;
        vibCL = rand() * -2147483647;
        vibDL = rand() * -2147483647;
        vibEL = rand() * -2147483647;
        vibFL = rand() * -2147483647;
        vibGL = rand() * -2147483647;
        vibHL = rand() * -2147483647;
        vibAR = rand() * -2147483647;
        vibBR = rand() * -2147483647;
        vibCR = rand() * -2147483647;
        vibDR = rand() * -2147483647;
        vibER = rand() * -2147483647;
        vibFR = rand() * -2147483647;
        vibGR = rand() * -2147483647;
        vibHR = rand() * -2147483647;
        A = 1.0;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        E = 0.5;
        F = 0.5;
        G = 1.0;
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
        kParamG = 6,
        kNumParameters = 7
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;
            case kParamF: return 1.0; break;
            case kParamG: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "filter"; break;
            case kParamB: return "damping"; break;
            case kParamC: return "speed"; break;
            case kParamD: return "vibrato"; break;
            case kParamE: return "rmsize"; break;
            case kParamF: return "flavor"; break;
            case kParamG: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Filter"; break;
            case kParamB: return "Damping"; break;
            case kParamC: return "Speed"; break;
            case kParamD: return "Vibrato"; break;
            case kParamE: return "RmSize"; break;
            case kParamF: return "Flavor"; break;
            case kParamG: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;

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
        biquadC[0] = biquadB[0] = biquadA[0] = ((A * 9000.0) + 1000.0) / Effect<T>::getSampleRate();
        biquadA[1] = 1.618033988749894848204586;
        biquadB[1] = 0.618033988749894848204586;
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
        double vibSpeed = 0.06 + C;
        double vibDepth = (0.027 + pow(D, 3)) * 100.0;
        double size = (pow(E, 2) * 90.0) + 10.0;
        double depthFactor = 1.0 - pow((1.0 - (0.82 - ((B * 0.5) + (size * 0.002)))), 4);
        double blend = 0.955 - (size * 0.007);
        double crossmod = (F - 0.5) * 2.0;
        crossmod = pow(crossmod, 3) * 0.5;
        double regen = depthFactor * (0.5 - (fabs(crossmod) * 0.031));
        double wet = G;
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
        delayM = (29 * size) - (56 * size * fabs(crossmod));
        // predelay for natural spaces, gets cut back for heavily artificial spaces
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
            aML[countM] = inputSampleL;
            aMR[countM] = inputSampleR;
            countM++;
            if (countM < 0 || countM > delayM) {
                countM = 0;
            }
            inputSampleL = aML[countM];
            inputSampleR = aMR[countM];
            // predelay
            double tempSampleL = (inputSampleL * biquadA[2]) + biquadA[7];
            biquadA[7] = (inputSampleL * biquadA[3]) - (tempSampleL * biquadA[5]) + biquadA[8];
            biquadA[8] = (inputSampleL * biquadA[4]) - (tempSampleL * biquadA[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquadA[2]) + biquadA[9];
            biquadA[9] = (inputSampleR * biquadA[3]) - (tempSampleR * biquadA[5]) + biquadA[10];
            biquadA[10] = (inputSampleR * biquadA[4]) - (tempSampleR * biquadA[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
            inputSampleL *= wet;
            inputSampleR *= wet;
            // we're going to use this as a kind of balance since the reverb buildup can be so large
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
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
            aAL[countA] = allpassLL + feedbackAL;
            aBL[countB] = allpassKL + feedbackBL;
            aCL[countC] = allpassJL + feedbackCL;
            aDL[countD] = allpassIL + feedbackDL;
            aEL[countE] = allpassIL + feedbackEL;
            aFL[countF] = allpassJL + feedbackFL;
            aGL[countG] = allpassKL + feedbackGL;
            aHL[countH] = allpassLL + feedbackHL; // L
            aAR[countA] = allpassLR + feedbackAR;
            aBR[countB] = allpassKR + feedbackBR;
            aCR[countC] = allpassJR + feedbackCR;
            aDR[countD] = allpassIR + feedbackDR;
            aER[countE] = allpassIR + feedbackER;
            aFR[countF] = allpassJR + feedbackFR;
            aGR[countG] = allpassKR + feedbackGR;
            aHR[countH] = allpassLR + feedbackHR; // R
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
            vibAL += (depthA * vibSpeed);
            vibBL += (depthB * vibSpeed);
            vibCL += (depthC * vibSpeed);
            vibDL += (depthD * vibSpeed);
            vibEL += (depthE * vibSpeed);
            vibFL += (depthF * vibSpeed);
            vibGL += (depthG * vibSpeed);
            vibHL += (depthH * vibSpeed); // L
            vibAR += (depthA * vibSpeed);
            vibBR += (depthB * vibSpeed);
            vibCR += (depthC * vibSpeed);
            vibDR += (depthD * vibSpeed);
            vibER += (depthE * vibSpeed);
            vibFR += (depthF * vibSpeed);
            vibGR += (depthG * vibSpeed);
            vibHR += (depthH * vibSpeed); // R
            // Depth is shared, but each started at a random position
            double offsetAL = (sin(vibAL) + 1.0) * vibDepth;
            double offsetBL = (sin(vibBL) + 1.0) * vibDepth;
            double offsetCL = (sin(vibCL) + 1.0) * vibDepth;
            double offsetDL = (sin(vibDL) + 1.0) * vibDepth;
            double offsetEL = (sin(vibEL) + 1.0) * vibDepth;
            double offsetFL = (sin(vibFL) + 1.0) * vibDepth;
            double offsetGL = (sin(vibGL) + 1.0) * vibDepth;
            double offsetHL = (sin(vibHL) + 1.0) * vibDepth; // L
            double offsetAR = (sin(vibAR) + 1.0) * vibDepth;
            double offsetBR = (sin(vibBR) + 1.0) * vibDepth;
            double offsetCR = (sin(vibCR) + 1.0) * vibDepth;
            double offsetDR = (sin(vibDR) + 1.0) * vibDepth;
            double offsetER = (sin(vibER) + 1.0) * vibDepth;
            double offsetFR = (sin(vibFR) + 1.0) * vibDepth;
            double offsetGR = (sin(vibGR) + 1.0) * vibDepth;
            double offsetHR = (sin(vibHR) + 1.0) * vibDepth; // R
            int workingAL = countA + offsetAL;
            int workingBL = countB + offsetBL;
            int workingCL = countC + offsetCL;
            int workingDL = countD + offsetDL;
            int workingEL = countE + offsetEL;
            int workingFL = countF + offsetFL;
            int workingGL = countG + offsetGL;
            int workingHL = countH + offsetHL; // L
            int workingAR = countA + offsetAR;
            int workingBR = countB + offsetBR;
            int workingCR = countC + offsetCR;
            int workingDR = countD + offsetDR;
            int workingER = countE + offsetER;
            int workingFR = countF + offsetFR;
            int workingGR = countG + offsetGR;
            int workingHR = countH + offsetHR; // R
            double interpolAL = (aAL[workingAL - ((workingAL > delayA) ? delayA + 1 : 0)] * (1 - (offsetAL - floor(offsetAL))));
            interpolAL += (aAL[workingAL + 1 - ((workingAL + 1 > delayA) ? delayA + 1 : 0)] * ((offsetAL - floor(offsetAL))));
            double interpolBL = (aBL[workingBL - ((workingBL > delayB) ? delayB + 1 : 0)] * (1 - (offsetBL - floor(offsetBL))));
            interpolBL += (aBL[workingBL + 1 - ((workingBL + 1 > delayB) ? delayB + 1 : 0)] * ((offsetBL - floor(offsetBL))));
            double interpolCL = (aCL[workingCL - ((workingCL > delayC) ? delayC + 1 : 0)] * (1 - (offsetCL - floor(offsetCL))));
            interpolCL += (aCL[workingCL + 1 - ((workingCL + 1 > delayC) ? delayC + 1 : 0)] * ((offsetCL - floor(offsetCL))));
            double interpolDL = (aDL[workingDL - ((workingDL > delayD) ? delayD + 1 : 0)] * (1 - (offsetDL - floor(offsetDL))));
            interpolDL += (aDL[workingDL + 1 - ((workingDL + 1 > delayD) ? delayD + 1 : 0)] * ((offsetDL - floor(offsetDL))));
            double interpolEL = (aEL[workingEL - ((workingEL > delayE) ? delayE + 1 : 0)] * (1 - (offsetEL - floor(offsetEL))));
            interpolEL += (aEL[workingEL + 1 - ((workingEL + 1 > delayE) ? delayE + 1 : 0)] * ((offsetEL - floor(offsetEL))));
            double interpolFL = (aFL[workingFL - ((workingFL > delayF) ? delayF + 1 : 0)] * (1 - (offsetFL - floor(offsetFL))));
            interpolFL += (aFL[workingFL + 1 - ((workingFL + 1 > delayF) ? delayF + 1 : 0)] * ((offsetFL - floor(offsetFL))));
            double interpolGL = (aGL[workingGL - ((workingGL > delayG) ? delayG + 1 : 0)] * (1 - (offsetGL - floor(offsetGL))));
            interpolGL += (aGL[workingGL + 1 - ((workingGL + 1 > delayG) ? delayG + 1 : 0)] * ((offsetGL - floor(offsetGL))));
            double interpolHL = (aHL[workingHL - ((workingHL > delayH) ? delayH + 1 : 0)] * (1 - (offsetHL - floor(offsetHL))));
            interpolHL += (aHL[workingHL + 1 - ((workingHL + 1 > delayH) ? delayH + 1 : 0)] * ((offsetHL - floor(offsetHL))));
            // L
            double interpolAR = (aAR[workingAR - ((workingAR > delayA) ? delayA + 1 : 0)] * (1 - (offsetAR - floor(offsetAR))));
            interpolAR += (aAR[workingAR + 1 - ((workingAR + 1 > delayA) ? delayA + 1 : 0)] * ((offsetAR - floor(offsetAR))));
            double interpolBR = (aBR[workingBR - ((workingBR > delayB) ? delayB + 1 : 0)] * (1 - (offsetBR - floor(offsetBR))));
            interpolBR += (aBR[workingBR + 1 - ((workingBR + 1 > delayB) ? delayB + 1 : 0)] * ((offsetBR - floor(offsetBR))));
            double interpolCR = (aCR[workingCR - ((workingCR > delayC) ? delayC + 1 : 0)] * (1 - (offsetCR - floor(offsetCR))));
            interpolCR += (aCR[workingCR + 1 - ((workingCR + 1 > delayC) ? delayC + 1 : 0)] * ((offsetCR - floor(offsetCR))));
            double interpolDR = (aDR[workingDR - ((workingDR > delayD) ? delayD + 1 : 0)] * (1 - (offsetDR - floor(offsetDR))));
            interpolDR += (aDR[workingDR + 1 - ((workingDR + 1 > delayD) ? delayD + 1 : 0)] * ((offsetDR - floor(offsetDR))));
            double interpolER = (aER[workingER - ((workingER > delayE) ? delayE + 1 : 0)] * (1 - (offsetER - floor(offsetER))));
            interpolER += (aER[workingER + 1 - ((workingER + 1 > delayE) ? delayE + 1 : 0)] * ((offsetER - floor(offsetER))));
            double interpolFR = (aFR[workingFR - ((workingFR > delayF) ? delayF + 1 : 0)] * (1 - (offsetFR - floor(offsetFR))));
            interpolFR += (aFR[workingFR + 1 - ((workingFR + 1 > delayF) ? delayF + 1 : 0)] * ((offsetFR - floor(offsetFR))));
            double interpolGR = (aGR[workingGR - ((workingGR > delayG) ? delayG + 1 : 0)] * (1 - (offsetGR - floor(offsetGR))));
            interpolGR += (aGR[workingGR + 1 - ((workingGR + 1 > delayG) ? delayG + 1 : 0)] * ((offsetGR - floor(offsetGR))));
            double interpolHR = (aHR[workingHR - ((workingHR > delayH) ? delayH + 1 : 0)] * (1 - (offsetHR - floor(offsetHR))));
            interpolHR += (aHR[workingHR + 1 - ((workingHR + 1 > delayH) ? delayH + 1 : 0)] * ((offsetHR - floor(offsetHR))));
            // R
            interpolAL = ((1.0 - blend) * interpolAL) + (aAL[workingAL - ((workingAL > delayA) ? delayA + 1 : 0)] * blend);
            interpolBL = ((1.0 - blend) * interpolBL) + (aBL[workingBL - ((workingBL > delayB) ? delayB + 1 : 0)] * blend);
            interpolCL = ((1.0 - blend) * interpolCL) + (aCL[workingCL - ((workingCL > delayC) ? delayC + 1 : 0)] * blend);
            interpolDL = ((1.0 - blend) * interpolDL) + (aDL[workingDL - ((workingDL > delayD) ? delayD + 1 : 0)] * blend);
            interpolEL = ((1.0 - blend) * interpolEL) + (aEL[workingEL - ((workingEL > delayE) ? delayE + 1 : 0)] * blend);
            interpolFL = ((1.0 - blend) * interpolFL) + (aFL[workingFL - ((workingFL > delayF) ? delayF + 1 : 0)] * blend);
            interpolGL = ((1.0 - blend) * interpolGL) + (aGL[workingGL - ((workingGL > delayG) ? delayG + 1 : 0)] * blend);
            interpolHL = ((1.0 - blend) * interpolHL) + (aHL[workingHL - ((workingHL > delayH) ? delayH + 1 : 0)] * blend); // L
            interpolAR = ((1.0 - blend) * interpolAR) + (aAR[workingAR - ((workingAR > delayA) ? delayA + 1 : 0)] * blend);
            interpolBR = ((1.0 - blend) * interpolBR) + (aBR[workingBR - ((workingBR > delayB) ? delayB + 1 : 0)] * blend);
            interpolCR = ((1.0 - blend) * interpolCR) + (aCR[workingCR - ((workingCR > delayC) ? delayC + 1 : 0)] * blend);
            interpolDR = ((1.0 - blend) * interpolDR) + (aDR[workingDR - ((workingDR > delayD) ? delayD + 1 : 0)] * blend);
            interpolER = ((1.0 - blend) * interpolER) + (aER[workingER - ((workingER > delayE) ? delayE + 1 : 0)] * blend);
            interpolFR = ((1.0 - blend) * interpolFR) + (aFR[workingFR - ((workingFR > delayF) ? delayF + 1 : 0)] * blend);
            interpolGR = ((1.0 - blend) * interpolGR) + (aGR[workingGR - ((workingGR > delayG) ? delayG + 1 : 0)] * blend);
            interpolHR = ((1.0 - blend) * interpolHR) + (aHR[workingHR - ((workingHR > delayH) ? delayH + 1 : 0)] * blend); // R
            interpolAL = (interpolAL * (1.0 - fabs(crossmod))) + (interpolEL * crossmod);
            interpolEL = (interpolEL * (1.0 - fabs(crossmod))) + (interpolAL * crossmod); // L
            interpolAR = (interpolAR * (1.0 - fabs(crossmod))) + (interpolER * crossmod);
            interpolER = (interpolER * (1.0 - fabs(crossmod))) + (interpolAR * crossmod); // R
            feedbackAL = (interpolAL - (interpolBL + interpolCL + interpolDL)) * regen;
            feedbackBL = (interpolBL - (interpolAL + interpolCL + interpolDL)) * regen;
            feedbackCL = (interpolCL - (interpolAL + interpolBL + interpolDL)) * regen;
            feedbackDL = (interpolDL - (interpolAL + interpolBL + interpolCL)) * regen; // Householder feedback matrix, L
            feedbackEL = (interpolEL - (interpolFL + interpolGL + interpolHL)) * regen;
            feedbackFL = (interpolFL - (interpolEL + interpolGL + interpolHL)) * regen;
            feedbackGL = (interpolGL - (interpolEL + interpolFL + interpolHL)) * regen;
            feedbackHL = (interpolHL - (interpolEL + interpolFL + interpolGL)) * regen; // Householder feedback matrix, L
            feedbackAR = (interpolAR - (interpolBR + interpolCR + interpolDR)) * regen;
            feedbackBR = (interpolBR - (interpolAR + interpolCR + interpolDR)) * regen;
            feedbackCR = (interpolCR - (interpolAR + interpolBR + interpolDR)) * regen;
            feedbackDR = (interpolDR - (interpolAR + interpolBR + interpolCR)) * regen; // Householder feedback matrix, R
            feedbackER = (interpolER - (interpolFR + interpolGR + interpolHR)) * regen;
            feedbackFR = (interpolFR - (interpolER + interpolGR + interpolHR)) * regen;
            feedbackGR = (interpolGR - (interpolER + interpolFR + interpolHR)) * regen;
            feedbackHR = (interpolHR - (interpolER + interpolFR + interpolGR)) * regen; // Householder feedback matrix, R
            inputSampleL = (interpolAL + interpolBL + interpolCL + interpolDL + interpolEL + interpolFL + interpolGL + interpolHL) / 8.0;
            inputSampleR = (interpolAR + interpolBR + interpolCR + interpolDR + interpolER + interpolFR + interpolGR + interpolHR) / 8.0;
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
                inputSampleL += (drySampleL * (1.0 - wet));
                inputSampleR += (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::matrixverb
