#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::galactic3 {

constexpr std::string_view k_name{ "Galactic3" };
constexpr std::string_view k_short_description{
    "Galactic3 is exactly Galactic again, but with better undersampling."
};
constexpr std::string_view k_long_description{
    "This is always a bit tricky: I try to revisit my biggest hits and make them better. The last time I did that with Galactic, not only was it merely different, it had a bug that Galactic doesn't have: four variables that didn't get initialized properly. Galactic2 is now fixed and presumably bugless, and is still just 'different', a variation on what Galactic does.Galactic3 is straight up better. Because it's exactly Galactic again… but instead of the linear interpolation it used to use (like CrunchCoat) for high sample rates, it's now using my new Bezier curve technique to interpolate, like kCathedral3 and CreamCoat.First, that means it should have better tone at high sample rates. Second, in theory it should gracefully handle even really outrageous sample rates (Galactic will eventually start scaling the verb space down in size, Galactic3 won't). But most importantly, that means you have a Derez control like CreamCoat, except on the reverb engine of Galactic. So you have exactly the sounds of Galactic, but you can further undersample them for a cool 'reduce sample rate' effect… and in doing so, increase the reverb space in size. How much? To unreasonable size: galaxy-sized.To use the new tone with the same apparent room size, decrease Bigness until the giant space is back to what you want again.Galactic3 takes in audio (dry/wet control available) and uses the Replace control to determine how much of the new sound coming in should replace the space that’s currently there. Detune shifts the pitch for both channels (in a quadrature pitch shift arrangement that means maximum widening for each sound) and Brightness controls both the brightness going into, and coming out of, the reverb. Replace, Brightness and Detune are designed to be playable on the fly to make your ambient spaces or evolve them. Bigness is the reverb buffers, so you can still alter that but it will make crashing noises when you do (that will then become more infinite spaces). And Derez, the new addition, will snap the whole pitch of the reverb space up and down without otherwise glitching it, so in theory it's playable as well.I think this one is really fun! As you can see it fits with my experimental-music aesthetic (didn’t even have to add Srsly2 on the end of it to make it superwide… though of course I could, and so can you). If you’re not quite that abstract, you can still use it on pads for more normal things. Just set Replace to a lot higher, set the Brightness and Bigness appropriately, and use Detune to chorus out your new huge enormous synth pad, or whatever else needs to have an unreasonably huge and wide stereo field. I’m pretty sure this can become your go-to for epic fields of reverb, no matter what’s meant to be causing them. And Derez adds darkening abilities beyond the scope of original Galactic. Honestly, I think I finally managed to make it better rather than just different. Let me know what you think :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class Galactic3 final : public Effect<T>
{
    double iirAL;
    double iirBL;
    double aIL[6480];
    double aJL[3660];
    double aKL[1720];
    double aLL[680];
    double aAL[9700];
    double aBL[6000];
    double aCL[2320];
    double aDL[940];
    double aEL[15220];
    double aFL[8460];
    double aGL[4540];
    double aHL[3200];
    double aML[3111];
    double aMR[3111];
    double vibML, vibMR, depthM, oldfpd;
    double feedbackAL;
    double feedbackBL;
    double feedbackCL;
    double feedbackDL;
    double iirAR;
    double iirBR;
    double aIR[6480];
    double aJR[3660];
    double aKR[1720];
    double aLR[680];
    double aAR[9700];
    double aBR[6000];
    double aCR[2320];
    double aDR[940];
    double aER[15220];
    double aFR[8460];
    double aGR[4540];
    double aHR[3200];
    double feedbackAR;
    double feedbackBR;
    double feedbackCR;
    double feedbackDR;
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
    int countM, delayM; // all these ints are shared across channels, not duplicated
    double vibM;
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

    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Galactic3()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        F = 1.0;
        iirAL = 0.0;
        iirAR = 0.0;
        iirBL = 0.0;
        iirBR = 0.0;
        for (int count = 0; count < 6479; count++) {
            aIL[count] = 0.0;
            aIR[count] = 0.0;
        }
        for (int count = 0; count < 3659; count++) {
            aJL[count] = 0.0;
            aJR[count] = 0.0;
        }
        for (int count = 0; count < 1719; count++) {
            aKL[count] = 0.0;
            aKR[count] = 0.0;
        }
        for (int count = 0; count < 679; count++) {
            aLL[count] = 0.0;
            aLR[count] = 0.0;
        }
        for (int count = 0; count < 9699; count++) {
            aAL[count] = 0.0;
            aAR[count] = 0.0;
        }
        for (int count = 0; count < 5999; count++) {
            aBL[count] = 0.0;
            aBR[count] = 0.0;
        }
        for (int count = 0; count < 2319; count++) {
            aCL[count] = 0.0;
            aCR[count] = 0.0;
        }
        for (int count = 0; count < 939; count++) {
            aDL[count] = 0.0;
            aDR[count] = 0.0;
        }
        for (int count = 0; count < 15219; count++) {
            aEL[count] = 0.0;
            aER[count] = 0.0;
        }
        for (int count = 0; count < 8459; count++) {
            aFL[count] = 0.0;
            aFR[count] = 0.0;
        }
        for (int count = 0; count < 4539; count++) {
            aGL[count] = 0.0;
            aGR[count] = 0.0;
        }
        for (int count = 0; count < 3199; count++) {
            aHL[count] = 0.0;
            aHR[count] = 0.0;
        }
        for (int count = 0; count < 3110; count++) {
            aML[count] = aMR[count] = 0.0;
        }
        feedbackAL = 0.0;
        feedbackAR = 0.0;
        feedbackBL = 0.0;
        feedbackBR = 0.0;
        feedbackCL = 0.0;
        feedbackCR = 0.0;
        feedbackDL = 0.0;
        feedbackDR = 0.0;
        countI = 1;
        countJ = 1;
        countK = 1;
        countL = 1;
        countA = 1;
        countB = 1;
        countC = 1;
        countD = 1;
        countE = 1;
        countF = 1;
        countG = 1;
        countH = 1;
        countM = 1;
        // the predelay
        vibM = 3.0;
        oldfpd = 429496.7295;
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
        kParamF = 5,
        kNumParameters = 6
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "replace"; break;
            case kParamB: return "brightns"; break;
            case kParamC: return "detune"; break;
            case kParamD: return "derez"; break;
            case kParamE: return "bigness"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Replace"; break;
            case kParamB: return "Brightns"; break;
            case kParamC: return "Detune"; break;
            case kParamD: return "Derez"; break;
            case kParamE: return "Bigness"; break;
            case kParamF: return "Dry/Wet"; break;

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
        double regen = 0.0625 + ((1.0 - A) * 0.0625);
        double attenuate = (1.0 - (regen / 0.125)) * 1.333;
        double lowpass = pow(1.00001 - (1.0 - B), 2.0) / sqrt(overallscale);
        double drift = pow(C, 3) * 0.001;
        double derez = D / overallscale;
        if (derez < 0.0005) {
            derez = 0.0005;
        }
        if (derez > 1.0) {
            derez = 1.0;
        }
        derez = 1.0 / ((int)(1.0 / derez));
        // this hard-locks derez to exact subdivisions of 1.0
        double size = (E * 1.77) + 0.1;
        double wet = 1.0 - (pow(1.0 - F, 3));
        delayI = 3407.0 * size;
        delayJ = 1823.0 * size;
        delayK = 859.0 * size;
        delayL = 331.0 * size;
        delayA = 4801.0 * size;
        delayB = 2909.0 * size;
        delayC = 1153.0 * size;
        delayD = 461.0 * size;
        delayE = 7607.0 * size;
        delayF = 4217.0 * size;
        delayG = 2269.0 * size;
        delayH = 1597.0 * size;
        delayM = 256;
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
            vibM += (oldfpd * drift);
            if (vibM > (3.141592653589793238 * 2.0)) {
                vibM = 0.0;
                oldfpd = 0.4294967295 + (fpdL * 0.0000000000618);
            }
            aML[countM] = inputSampleL * attenuate;
            aMR[countM] = inputSampleR * attenuate;
            countM++;
            if (countM < 0 || countM > delayM) {
                countM = 0;
            }
            double offsetML = (sin(vibM) + 1.0) * 127;
            double offsetMR = (sin(vibM + (3.141592653589793238 / 2.0)) + 1.0) * 127;
            int workingML = countM + offsetML;
            int workingMR = countM + offsetMR;
            double interpolML = (aML[workingML - ((workingML > delayM) ? delayM + 1 : 0)] * (1 - (offsetML - floor(offsetML))));
            interpolML += (aML[workingML + 1 - ((workingML + 1 > delayM) ? delayM + 1 : 0)] * ((offsetML - floor(offsetML))));
            double interpolMR = (aMR[workingMR - ((workingMR > delayM) ? delayM + 1 : 0)] * (1 - (offsetMR - floor(offsetMR))));
            interpolMR += (aMR[workingMR + 1 - ((workingMR + 1 > delayM) ? delayM + 1 : 0)] * ((offsetMR - floor(offsetMR))));
            inputSampleL = interpolML;
            inputSampleR = interpolMR;
            // predelay that applies vibrato
            // want vibrato speed AND depth like in MatrixVerb
            iirAL = (iirAL * (1.0 - lowpass)) + (inputSampleL * lowpass);
            inputSampleL = iirAL;
            iirAR = (iirAR * (1.0 - lowpass)) + (inputSampleR * lowpass);
            inputSampleR = iirAR;
            // initial filter
            bez[bez_cycle] += derez;
            bez[bez_SampL] += ((inputSampleL + bez[bez_InL]) * derez);
            bez[bez_SampR] += ((inputSampleR + bez[bez_InR]) * derez);
            bez[bez_InL] = inputSampleL;
            bez[bez_InR] = inputSampleR;
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bez[bez_cycle] = 0.0;
                aIL[countI] = (bez[bez_SampL] + bez[bez_UnInL]) + (feedbackAR * regen);
                aJL[countJ] = (bez[bez_SampL] + bez[bez_UnInL]) + (feedbackBR * regen);
                aKL[countK] = (bez[bez_SampL] + bez[bez_UnInL]) + (feedbackCR * regen);
                aLL[countL] = (bez[bez_SampL] + bez[bez_UnInL]) + (feedbackDR * regen);
                bez[bez_UnInL] = bez[bez_SampL];
                aIR[countI] = (bez[bez_SampR] + bez[bez_UnInR]) + (feedbackAL * regen);
                aJR[countJ] = (bez[bez_SampR] + bez[bez_UnInR]) + (feedbackBL * regen);
                aKR[countK] = (bez[bez_SampR] + bez[bez_UnInR]) + (feedbackCL * regen);
                aLR[countL] = (bez[bez_SampR] + bez[bez_UnInR]) + (feedbackDL * regen);
                bez[bez_UnInR] = bez[bez_SampR];
                countI++;
                if (countI < 0 || countI > delayI) {
                    countI = 0;
                }
                countJ++;
                if (countJ < 0 || countJ > delayJ) {
                    countJ = 0;
                }
                countK++;
                if (countK < 0 || countK > delayK) {
                    countK = 0;
                }
                countL++;
                if (countL < 0 || countL > delayL) {
                    countL = 0;
                }
                double outIL = aIL[countI - ((countI > delayI) ? delayI + 1 : 0)];
                double outJL = aJL[countJ - ((countJ > delayJ) ? delayJ + 1 : 0)];
                double outKL = aKL[countK - ((countK > delayK) ? delayK + 1 : 0)];
                double outLL = aLL[countL - ((countL > delayL) ? delayL + 1 : 0)];
                double outIR = aIR[countI - ((countI > delayI) ? delayI + 1 : 0)];
                double outJR = aJR[countJ - ((countJ > delayJ) ? delayJ + 1 : 0)];
                double outKR = aKR[countK - ((countK > delayK) ? delayK + 1 : 0)];
                double outLR = aLR[countL - ((countL > delayL) ? delayL + 1 : 0)];
                // first block: now we have four outputs
                aAL[countA] = (outIL - (outJL + outKL + outLL));
                aBL[countB] = (outJL - (outIL + outKL + outLL));
                aCL[countC] = (outKL - (outIL + outJL + outLL));
                aDL[countD] = (outLL - (outIL + outJL + outKL));
                aAR[countA] = (outIR - (outJR + outKR + outLR));
                aBR[countB] = (outJR - (outIR + outKR + outLR));
                aCR[countC] = (outKR - (outIR + outJR + outLR));
                aDR[countD] = (outLR - (outIR + outJR + outKR));
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
                double outAL = aAL[countA - ((countA > delayA) ? delayA + 1 : 0)];
                double outBL = aBL[countB - ((countB > delayB) ? delayB + 1 : 0)];
                double outCL = aCL[countC - ((countC > delayC) ? delayC + 1 : 0)];
                double outDL = aDL[countD - ((countD > delayD) ? delayD + 1 : 0)];
                double outAR = aAR[countA - ((countA > delayA) ? delayA + 1 : 0)];
                double outBR = aBR[countB - ((countB > delayB) ? delayB + 1 : 0)];
                double outCR = aCR[countC - ((countC > delayC) ? delayC + 1 : 0)];
                double outDR = aDR[countD - ((countD > delayD) ? delayD + 1 : 0)];
                // second block: four more outputs
                aEL[countE] = (outAL - (outBL + outCL + outDL));
                aFL[countF] = (outBL - (outAL + outCL + outDL));
                aGL[countG] = (outCL - (outAL + outBL + outDL));
                aHL[countH] = (outDL - (outAL + outBL + outCL));
                aER[countE] = (outAR - (outBR + outCR + outDR));
                aFR[countF] = (outBR - (outAR + outCR + outDR));
                aGR[countG] = (outCR - (outAR + outBR + outDR));
                aHR[countH] = (outDR - (outAR + outBR + outCR));
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
                double outEL = aEL[countE - ((countE > delayE) ? delayE + 1 : 0)];
                double outFL = aFL[countF - ((countF > delayF) ? delayF + 1 : 0)];
                double outGL = aGL[countG - ((countG > delayG) ? delayG + 1 : 0)];
                double outHL = aHL[countH - ((countH > delayH) ? delayH + 1 : 0)];
                double outER = aER[countE - ((countE > delayE) ? delayE + 1 : 0)];
                double outFR = aFR[countF - ((countF > delayF) ? delayF + 1 : 0)];
                double outGR = aGR[countG - ((countG > delayG) ? delayG + 1 : 0)];
                double outHR = aHR[countH - ((countH > delayH) ? delayH + 1 : 0)];
                // third block: final outputs
                feedbackAL = (outEL - (outFL + outGL + outHL));
                feedbackBL = (outFL - (outEL + outGL + outHL));
                feedbackCL = (outGL - (outEL + outFL + outHL));
                feedbackDL = (outHL - (outEL + outFL + outGL));
                feedbackAR = (outER - (outFR + outGR + outHR));
                feedbackBR = (outFR - (outER + outGR + outHR));
                feedbackCR = (outGR - (outER + outFR + outHR));
                feedbackDR = (outHR - (outER + outFR + outGR));
                // which we need to feed back into the input again, a bit
                inputSampleL = (outEL + outFL + outGL + outHL) / 8.0;
                inputSampleR = (outER + outFR + outGR + outHR) / 8.0;
                // and take the final combined sum of outputs
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
            iirBL = (iirBL * (1.0 - lowpass)) + (inputSampleL * lowpass);
            inputSampleL = iirBL;
            iirBR = (iirBR * (1.0 - lowpass)) + (inputSampleR * lowpass);
            inputSampleR = iirBR;
            // end filter
            if (wet < 1.0) {
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::galactic3
