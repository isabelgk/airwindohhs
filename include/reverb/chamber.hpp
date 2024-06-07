#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::chamber {

constexpr std::string_view k_name{ "Chamber" };
constexpr std::string_view k_short_description{
    "Chamber is a feedforward reverb based on the golden ratio."
};
constexpr std::string_view k_long_description{
    "I actually meant to do something completely different. Honest.It was time to dig into the feedforward reverbs again, maybe rearrange them in some interesting way. Do something with the geometry of them, have them go two-into-three-into-four or something along those lines, and we were hanging out in the Monday coding livestream, starting to experiment… and one of my crew tossed out a sequence of numbers. Fibonacci numbers. Could we hear what those sounded like as delay constants?Wasn’t bad. Chat got buried in Fibonacci numbers for a while: we are always enthusiastic at finding weird applications of things that shouldn’t be any use. After all, to get reverb tails to become seamless, the delay constants have to be set up properly. Prime numbers are key. Fibonacci sequences have nothing to do with this. And then, someone in chat observed… as the Fibonacci numbers get bigger they start approximating to the golden ratio.Yoicks, scooby! We’d better try it! AND THEN…Chamber is a feedforward reverb, using three banks of four delays each in a Householder feedback matrix, except it’s feedforward. Only the very end feeds back into the beginning again, just like Verbity, just like Galactic (it is dual-mono like Verbity, as its peculiar merits fit well with a dual-mono arrangement). And the delays go to a longest delay (maximum delay size) and each one in turn, back to the first, is exactly the golden ratio smaller than the previous. It’s like a big spiral of delay times, perfect to lots of decimal places. If you listen to just one instance of each delay (by turning Longness to zero, and Chamber lets you HAVE literally zero feedback), that’s a weird stuttery slapback. By itself, an arbitrary little chirp, a complicated slapback that doesn’t sound particularly interesting.And then if you turn the feedback up, with Longness, it stretches out into a continuous, seamless, perfect reverb tail, just as if all the delay times had been worked out to be perfect little prime ratios.This was an astounding discovery. It means you can dial in any degree of feedback or none, use any delay time (everything’s calculated out on the fly), do anything with it and it’ll adapt. It’ll always sound like a chamber, hence the name, but it’s maybe more malleable than any reverb I’ve ever made. And to make it even more malleable, Chamber’s Darkness control is tweaked so that the fall-off over time is always accurate to the sound of audio decaying in air in a theoretically ultimate room (studied from recordings of giant underground concrete cisterns) but the tone-shaping is darkened using very warm, basic IIR filters. And on top of that, a new control for the feedforward reverbs: since Chamber is such a studio tool, I gave it a highpass. So you can plunk it on any sort of buss or channel, run it mostly dry, bring in the reverb (Chamber and Verbity are designed so as you add verb, the dry remains unaltered until you get to 0.5 on the Wetness control, at which point the verb is at full volume and you start fading the dry signal) and then begin dialing back the bassiness of the reverb without touching the dry. Very useful for a chamber or plate send, and built right in!Hope you like it. Oh, and one more thing: in working on this, I found a bug that was in the Undersampling code I use. The bug was making a bit of unwanted edge, only in high sample rate stuff, only above 20kHz. I’d had someone discover this in Verbity, but I didn’t know what to do other than filter it at the time, and hadn’t done anything yet.So now (as of right now: redownload what you need) Verbity, Galactic, IronOxideClassic2, and Chamber are FIXED. Go back and redownload them, or get them out of the collections for various different platforms. They have all had the ultrasonic noise cleaned up: there is still a touch of audio there as part of the algorithm, but it turns out it’s much less than I thought and that’s reflected in cleaner sound. It should not change saved mixes significantly as it is entirely supersonic, but if it did it would only help as the traces of noise weren’t useful for any purpose, they were a bug, samples being slightly out of order at 96 and 192k. No change at 44.1 or 48k.Chamber actually goes a step farther, in that I added more code that subtly averages the supersonic samples… that can apply to new plugins going forward, but Verbity, Galactic, and IronOxideClassic2 don’t have that as it could work like a tone change. So, compare the new Verbity to Chamber if you’d like to check out the tiny amount of added depth we’ll have going forward."
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class Chamber final : public Effect<T>
{
    double iirAL;
    double iirBL;
    double iirCL;
    double aEL[20000];
    double aFL[12361];
    double aGL[7640];
    double aHL[4722];
    double aAL[2916];
    double aBL[1804];
    double aCL[1115];
    double aDL[689];
    double aIL[426];
    double aJL[264];
    double aKL[163];
    double aLL[101];
    double feedbackAL;
    double feedbackBL;
    double feedbackCL;
    double feedbackDL;
    double previousAL;
    double previousBL;
    double previousCL;
    double previousDL;
    double lastRefL[10];
    double iirAR;
    double iirBR;
    double iirCR;
    double aER[20000];
    double aFR[12361];
    double aGR[7640];
    double aHR[4722];
    double aAR[2916];
    double aBR[1804];
    double aCR[1115];
    double aDR[689];
    double aIR[426];
    double aJR[264];
    double aKR[163];
    double aLR[101];
    double feedbackAR;
    double feedbackBR;
    double feedbackCR;
    double feedbackDR;
    double previousAR;
    double previousBR;
    double previousCR;
    double previousDR;
    double lastRefR[10];
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
    int cycle; // all these ints are shared across channels, not duplicated
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Chamber()
    {
        A = 0.35;
        B = 0.35;
        C = 0.35;
        D = 0.35;
        E = 0.35;
        iirAL = 0.0;
        iirAR = 0.0;
        iirBL = 0.0;
        iirBR = 0.0;
        iirCL = 0.0;
        iirCR = 0.0;
        for (int count = 0; count < 19999; count++) {
            aEL[count] = 0.0;
            aER[count] = 0.0;
        }
        for (int count = 0; count < 12360; count++) {
            aFL[count] = 0.0;
            aFR[count] = 0.0;
        }
        for (int count = 0; count < 7639; count++) {
            aGL[count] = 0.0;
            aGR[count] = 0.0;
        }
        for (int count = 0; count < 4721; count++) {
            aHL[count] = 0.0;
            aHR[count] = 0.0;
        }
        for (int count = 0; count < 2915; count++) {
            aAL[count] = 0.0;
            aAR[count] = 0.0;
        }
        for (int count = 0; count < 1803; count++) {
            aBL[count] = 0.0;
            aBR[count] = 0.0;
        }
        for (int count = 0; count < 1114; count++) {
            aCL[count] = 0.0;
            aCR[count] = 0.0;
        }
        for (int count = 0; count < 688; count++) {
            aDL[count] = 0.0;
            aDR[count] = 0.0;
        }
        for (int count = 0; count < 425; count++) {
            aIL[count] = 0.0;
            aIR[count] = 0.0;
        }
        for (int count = 0; count < 263; count++) {
            aJL[count] = 0.0;
            aJR[count] = 0.0;
        }
        for (int count = 0; count < 162; count++) {
            aKL[count] = 0.0;
            aKR[count] = 0.0;
        }
        for (int count = 0; count < 100; count++) {
            aLL[count] = 0.0;
            aLR[count] = 0.0;
        }
        feedbackAL = 0.0;
        feedbackAR = 0.0;
        feedbackBL = 0.0;
        feedbackBR = 0.0;
        feedbackCL = 0.0;
        feedbackCR = 0.0;
        feedbackDL = 0.0;
        feedbackDR = 0.0;
        previousAL = 0.0;
        previousAR = 0.0;
        previousBL = 0.0;
        previousBR = 0.0;
        previousCL = 0.0;
        previousCR = 0.0;
        previousDL = 0.0;
        previousDR = 0.0;
        for (int count = 0; count < 9; count++) {
            lastRefL[count] = 0.0;
            lastRefR[count] = 0.0;
        }
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
        cycle = 0;
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
            return 0.35;
            break;
        kParamB:
            return 0.35;
            break;
        kParamC:
            return 0.35;
            break;
        kParamD:
            return 0.35;
            break;
        kParamE:
            return 0.35;
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
            return "bigness";
            break;
        kParamB:
            return "longness";
            break;
        kParamC:
            return "liteness";
            break;
        kParamD:
            return "darkness";
            break;
        kParamE:
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
            return "Bigness";
            break;
        kParamB:
            return "Longness";
            break;
        kParamC:
            return "Liteness";
            break;
        kParamD:
            return "Darkness";
            break;
        kParamE:
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
        double size = (pow(A, 2) * 0.9) + 0.1;
        double regen = (1.0 - (pow(1.0 - B, 6))) * 0.123;
        double highpass = (pow(C, 2.0)) / sqrt(overallscale);
        double lowpass = (1.0 - pow(D, 2.0)) / sqrt(overallscale);
        double interpolate = size * 0.381966011250105;
        double wet = E * 2.0;
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
        delayE = 19900 * size;
        delayF = delayE * 0.618033988749894848204586;
        delayG = delayF * 0.618033988749894848204586;
        delayH = delayG * 0.618033988749894848204586;
        delayA = delayH * 0.618033988749894848204586;
        delayB = delayA * 0.618033988749894848204586;
        delayC = delayB * 0.618033988749894848204586;
        delayD = delayC * 0.618033988749894848204586;
        delayI = delayD * 0.618033988749894848204586;
        delayJ = delayI * 0.618033988749894848204586;
        delayK = delayJ * 0.618033988749894848204586;
        delayL = delayK * 0.618033988749894848204586;
        // initially designed around the Fibonnaci series, Chamber uses
        // delay coefficients that are all related to the Golden Ratio,
        // Turns out that as you continue to sustain them, it turns from a
        // chunky slapback effect into a smoother reverb tail that can
        // sustain infinitely.
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
            iirCL = (iirCL * (1.0 - highpass)) + (inputSampleL * highpass);
            inputSampleL -= iirCL;
            iirCR = (iirCR * (1.0 - highpass)) + (inputSampleR * highpass);
            inputSampleR -= iirCR;
            // initial highpass
            iirAL = (iirAL * (1.0 - lowpass)) + (inputSampleL * lowpass);
            inputSampleL = iirAL;
            iirAR = (iirAR * (1.0 - lowpass)) + (inputSampleR * lowpass);
            inputSampleR = iirAR;
            // initial filter
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do a reverb sample
                feedbackAL = (feedbackAL * (1.0 - interpolate)) + (previousAL * interpolate);
                previousAL = feedbackAL;
                feedbackBL = (feedbackBL * (1.0 - interpolate)) + (previousBL * interpolate);
                previousBL = feedbackBL;
                feedbackCL = (feedbackCL * (1.0 - interpolate)) + (previousCL * interpolate);
                previousCL = feedbackCL;
                feedbackDL = (feedbackDL * (1.0 - interpolate)) + (previousDL * interpolate);
                previousDL = feedbackDL;
                feedbackAR = (feedbackAR * (1.0 - interpolate)) + (previousAR * interpolate);
                previousAR = feedbackAR;
                feedbackBR = (feedbackBR * (1.0 - interpolate)) + (previousBR * interpolate);
                previousBR = feedbackBR;
                feedbackCR = (feedbackCR * (1.0 - interpolate)) + (previousCR * interpolate);
                previousCR = feedbackCR;
                feedbackDR = (feedbackDR * (1.0 - interpolate)) + (previousDR * interpolate);
                previousDR = feedbackDR;
                aIL[countI] = inputSampleL + (feedbackAL * regen);
                aJL[countJ] = inputSampleL + (feedbackBL * regen);
                aKL[countK] = inputSampleL + (feedbackCL * regen);
                aLL[countL] = inputSampleL + (feedbackDL * regen);
                aIR[countI] = inputSampleR + (feedbackAR * regen);
                aJR[countJ] = inputSampleR + (feedbackBR * regen);
                aKR[countK] = inputSampleR + (feedbackCR * regen);
                aLR[countL] = inputSampleR + (feedbackDR * regen);
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
            switch (cycleEnd) // multi-pole average using lastRef[] variables
            {
                case 4:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[7]) * 0.5;
                    lastRefL[7] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[7]) * 0.5;
                    lastRefR[7] = lastRefR[8]; // continue, do not break
                case 3:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[6]) * 0.5;
                    lastRefL[6] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[6]) * 0.5;
                    lastRefR[6] = lastRefR[8]; // continue, do not break
                case 2:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[5]) * 0.5;
                    lastRefL[5] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[5]) * 0.5;
                    lastRefR[5] = lastRefR[8]; // continue, do not break
                case 1:
                    break; // no further averaging
            }
            iirBL = (iirBL * (1.0 - lowpass)) + (inputSampleL * lowpass);
            inputSampleL = iirBL;
            iirBR = (iirBR * (1.0 - lowpass)) + (inputSampleR * lowpass);
            inputSampleR = iirBR;
            // end filter
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::chamber
