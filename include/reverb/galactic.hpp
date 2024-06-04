#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::galactic {

constexpr std::string_view k_name{ "Galactic" };
constexpr std::string_view k_short_description{
    "GalacticVibe is the stereo vibrato from the original Galactic reverb."
};
constexpr std::string_view k_long_description{
    "Been working on this for a while on Monday coding-streams! Galactic is an extension of my Verbity reverb, designed for ultimate deep space ambient music. It’s a combination feedback and feedforward reverb designed to make wide stereo verb-spaces out of anything, even mono test tones if you like.It takes in audio (dry/wet control available) and uses the Replace control to determine how much of the new sound coming in should replace the space that’s currently there. Detune shifts the pitch for both channels (in a quadrature pitch shift arrangement that means maximum widening for each sound) and Brightness controls both the brightness going into, and coming out of, the reverb. Replace, Brightness and Detune are designed to be playable on the fly to make your ambient spaces or evolve them. Bigness is the reverb buffers, so you can still alter that but it will make crashing noises when you do (that will then become more infinite spaces).I think this one is really fun! As you can see it fits with my experimental-music aesthetic (didn’t even have to add Srsly2 on the end of it to make it superwide… though of course I could, and so can you). If you’re not quite that abstract, you can still use it on pads for more normal things. Just set Replace to a lot higher, set the Brightness and Bigness appropriately, and use Detune to chorus out your new huge enormous synth pad, or whatever else needs to have an unreasonably huge and wide stereo field. I’m pretty sure this can become your go-to for epic fields of reverb, no matter what’s meant to be causing them.Galactic has been a huge success, no pun intended. It might be the most popular of my first batch of feedforward reverbs, and it's still available, from its banks of 4-wide householder matrix delay lines, to its pitch shift stereo widening.So, why revisit Galactic? What is there to even improve? Well, it's a matter of taste. My newer big reverbs and realistic reverbs go for a bigger reverb matrix with a distinct, more organic texture to it. And I wasn't really satisfied with how you worked with Galactic: I'd imagined a thing where you'd have reverb space going, and then you'd 'paint' in other sound, perhaps with a slider, or just by overloading the existing space with other stuff.And so, here we are. Galactic2. Drive, Sustain, Darken and Wetness. And a tone that owes more to the K series plate reverbs, than Infinity or Verbity. Sustain starts to feed back forever a little ways past two-thirds up, with halfway being long decays, and very short sustain being your kill switch. It'll creep up in gain at full crank, so be careful with that. Drive is how you feed in new audio while it's going, Wetness is probably going to be full crank for most use cases, and Darken goes from totally, scintillatingly bright to mellow and distant-sounding, both as part of the total output of the reverb, and as part of the feedback loop (so if you have bright stuff going, and you Darken, your sustain will start to get darker as it continues, which can add realism).I keep saying it's like the kPlate reverbs, but I checked the code and it's still 4-wide Householder matrixes, like the original. So what makes this so different? Its handling of stereo, is what. Either you'll love or hate it. This is why you should love it. Unlike the original Galactic, this isn't a pair of matched reverbs using pitch bends to stereo-ize them for mono things. Galactic2 is a SINGLE giant stereo reverb that uses all the same delay values, but uses them crossways: one channel accesses the matrix vertically, the other accesses the matrix horizontally. It's 'the same', but all the echoes are arranged differently. These are way harder to come up with but it's what was used on the kPlate reverbs, except those are 5x5 matrixes and this is still a 4x4. There's just a teeny bit of crossmodulation to round it out.What this means is, if you put stuff in the left, it makes a sort of cloud of reverb sound that's also mostly on the left. If you put stuff in on the right, likewise: you'll get 'space' happening mix right.And if you put stuff in mono… you get WIDE center content. The whole texture is wider than what original Galactic can do, without use of chorusing or pitch shifting to widen it. That gives it a completely different character, which is why you've still got original Galactic to use if you want. This is a second bite of the apple. It's about trying to get even deeper into space, and I hope you enjoy it :)GalacticVibe is about taking the code I used to make the Galactic reverb super wide, and breaking it out into a chorus-like effect… at which it turns out to be great!Used in Galactic, it's always full wet. It's a dual vibrato that's 'quadrature', meaning the channels are two different offsets on the same LFO. Except it's not exactly an LFO in the sense of low frequency oscillator, because it's slightly irregular. You don't hear it as being super off, but it's a somewhat organic quality whatever speed you choose.This produces two stereo outputs (even if you're just giving it mono) and the way they modulate is a bit like a miked-up Leslie: the pitch wavers across both channels. But then, if you add dry (which isn't part of Galactic's modulation) that blends against both stereo channels… and you end up with a nice little vibey chorus, with definite Leslie-like qualities, but without any of the speaker emulation and without an accurate pitch modulation on what would be the treble horns (instead, it's something else, a little smoother).So it turns out the modulation inside Galactic (Galactic2 is different) is quite nice just by itself! In particular, it seems very nice giving slow swirly effects, and ramping up to a quicker speed has a really striking 'leslie, but not leslie' quality. A happy accident that is now yours, because this is an open source, free plugin, so enjoy adding this tool to your stereo toolkit.All this is paid for by my Patreon, and the better that does the more I can do with it: as promised, the Bricasti reverb is here. Like I said I am not going to make a clone, but I am certain I can both get some useful k-series reverbs inspired by classic Bricasti patches, and design future reverb algorithms that incorporate more Bricast-isms, just from being able to hear it properly and make use of it in the studio. There's also the new Console versions coming along, currently in study mode as I explore a world of classic vinyl records heard in the fullest fidelity, and develop channel EQs that will go along with the new Console versions.Thanks, and hope you like GalacticVibe!"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class Galactic final : public Effect<T>
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
    double lastRefL[7];
    double thunderL;
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
    double lastRefR[7];
    double thunderR;
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
    int cycle; // all these ints are shared across channels, not duplicated
    double vibM;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Galactic()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        E = 1.0;
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
        for (int count = 0; count < 6; count++) {
            lastRefL[count] = 0.0;
            lastRefR[count] = 0.0;
        }
        thunderL = 0;
        thunderR = 0;
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
        cycle = 0;
        vibM = 3.0;
        oldfpd = 429496.7295;
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
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;
            case kParamC: return 0.5;
            case kParamD: return 1.0;
            case kParamE: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "replace";
            case kParamB: return "brightness";
            case kParamC: return "detune";
            case kParamD: return "bigness";
            case kParamE: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Replace";
            case kParamB: return "Brightness";
            case kParamC: return "Detune";
            case kParamD: return "Bigness";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
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
        double regen = 0.0625 + ((1.0 - A) * 0.0625);
        double attenuate = (1.0 - (regen / 0.125)) * 1.333;
        double lowpass = pow(1.00001 - (1.0 - B), 2.0) / sqrt(overallscale);
        double drift = pow(C, 3) * 0.001;
        double size = (D * 1.77) + 0.1;
        double wet = 1.0 - (pow(1.0 - E, 3));
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
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do a reverb sample
                aIL[countI] = inputSampleL + (feedbackAR * regen);
                aJL[countJ] = inputSampleL + (feedbackBR * regen);
                aKL[countK] = inputSampleL + (feedbackCR * regen);
                aLL[countL] = inputSampleL + (feedbackDR * regen);
                aIR[countI] = inputSampleR + (feedbackAL * regen);
                aJR[countJ] = inputSampleR + (feedbackBL * regen);
                aKR[countK] = inputSampleR + (feedbackCL * regen);
                aLR[countL] = inputSampleR + (feedbackDL * regen);
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::galactic
