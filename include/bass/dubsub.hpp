#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubsub {

constexpr std::string_view k_name{ "DubSub" };
constexpr std::string_view k_short_description{
    "DubSub is a fully featured bass doctor."
};
constexpr std::string_view k_long_description{
    "Be careful what you wish for. I like making plugins with very few controls, but when you have to use ALL the controls…Here’s how it works. The top section, Treble Grind, is like a bass guitar presence circuit. You shouldn’t use that for hi-fi purposes, if you want clean pass-through use the Dry/Wet. Treble Grind works like a distortion, and has an Inverse/Out control allowing you to subtract it as well as add it. ‘zero’ is in the middle.Crossover determines what goes to the Treble Grind, and what goes to the bass sections. To make it track bass better, set the crossover low.Bass Drive is how hard you’re pushing the main bass section. It’s essentially an adjustable Head Bump control like in ToTape. Bass Voicing controls the depth of the bass boost: setting it higher up sounds more like overdriving a bass amp. Bass Inv/Out is the same as in the Treble Grind section, an ‘attenuverter’ like in certain Eurorack modules: it lets you subtract the bass, not just add it.Sub Drive, Sub Voicing, and Sub Inv/Out are much like the bass section, except they work on an octave-divided version of the bass section. This helps it get good octave-down sounds some of the time, but it’s not anything like a digital suboctave synthesizer: it’s working crudely like an analog octave divider, which means it can make horrible noises if it doesn’t have clean signal to work with. This is of course intentional :) if you want it to do a recognizable sub-note, feed it a carefully controlled signal off a single track. Or, you can set it very deep and not mix in all that much of it, and get an interesting effect sound. For cleaner deep bass, work with the main bass section, or voice both of them very deep in hopes of cleaning up the sub-bass section a little by refusing to let it have more complicated signals."
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class DubSub final : public Effect<T>
{
    double iirDriveSampleAL;
    double iirDriveSampleBL;
    double iirDriveSampleCL;
    double iirDriveSampleDL;
    double iirDriveSampleEL;
    double iirDriveSampleFL;
    double iirDriveSampleAR;
    double iirDriveSampleBR;
    double iirDriveSampleCR;
    double iirDriveSampleDR;
    double iirDriveSampleER;
    double iirDriveSampleFR;
    bool flip; // drive things
    int bflip;
    bool WasNegativeL;
    bool SubOctaveL;
    bool WasNegativeR;
    bool SubOctaveR;
    double iirHeadBumpAL;
    double iirHeadBumpBL;
    double iirHeadBumpCL;
    double iirHeadBumpAR;
    double iirHeadBumpBR;
    double iirHeadBumpCR;
    double iirSubBumpAL;
    double iirSubBumpBL;
    double iirSubBumpCL;
    double iirSubBumpAR;
    double iirSubBumpBR;
    double iirSubBumpCR;
    double lastHeadBumpL;
    double lastSubBumpL;
    double lastHeadBumpR;
    double lastSubBumpR;
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleGL;
    double iirSampleHL;
    double iirSampleIL;
    double iirSampleJL;
    double iirSampleKL;
    double iirSampleLL;
    double iirSampleML;
    double iirSampleNL;
    double iirSampleOL;
    double iirSamplePL;
    double iirSampleQL;
    double iirSampleRL;
    double iirSampleSL;
    double iirSampleTL;
    double iirSampleUL;
    double iirSampleVL;
    double iirSampleWL;
    double iirSampleXL;
    double iirSampleYL;
    double iirSampleZL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double iirSampleGR;
    double iirSampleHR;
    double iirSampleIR;
    double iirSampleJR;
    double iirSampleKR;
    double iirSampleLR;
    double iirSampleMR;
    double iirSampleNR;
    double iirSampleOR;
    double iirSamplePR;
    double iirSampleQR;
    double iirSampleRR;
    double iirSampleSR;
    double iirSampleTR;
    double iirSampleUR;
    double iirSampleVR;
    double iirSampleWR;
    double iirSampleXR;
    double iirSampleYR;
    double iirSampleZR;
    double oscGateL;
    double oscGateR;
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
    float H;
    float I;
    float J;

  public:
    DubSub()
    {
        A = 0.9;
        B = 0.5;
        C = 0.74;
        D = 1.0;
        E = 0.95;
        F = 0.5;
        G = 0.2;
        H = 0.2;
        I = 0.5;
        J = 1.0;
        WasNegativeL = false;
        SubOctaveL = false;
        WasNegativeR = false;
        SubOctaveR = false;
        flip = false;
        bflip = 0;
        iirDriveSampleAL = 0.0;
        iirDriveSampleBL = 0.0;
        iirDriveSampleCL = 0.0;
        iirDriveSampleDL = 0.0;
        iirDriveSampleEL = 0.0;
        iirDriveSampleFL = 0.0;
        iirDriveSampleAR = 0.0;
        iirDriveSampleBR = 0.0;
        iirDriveSampleCR = 0.0;
        iirDriveSampleDR = 0.0;
        iirDriveSampleER = 0.0;
        iirDriveSampleFR = 0.0;
        iirHeadBumpAL = 0.0;
        iirHeadBumpBL = 0.0;
        iirHeadBumpCL = 0.0;
        iirHeadBumpAR = 0.0;
        iirHeadBumpBR = 0.0;
        iirHeadBumpCR = 0.0;
        iirSubBumpAL = 0.0;
        iirSubBumpBL = 0.0;
        iirSubBumpCL = 0.0;
        iirSubBumpAR = 0.0;
        iirSubBumpBR = 0.0;
        iirSubBumpCR = 0.0;
        lastHeadBumpL = 0.0;
        lastSubBumpL = 0.0;
        lastHeadBumpR = 0.0;
        lastSubBumpR = 0.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleGL = 0.0;
        iirSampleHL = 0.0;
        iirSampleIL = 0.0;
        iirSampleJL = 0.0;
        iirSampleKL = 0.0;
        iirSampleLL = 0.0;
        iirSampleML = 0.0;
        iirSampleNL = 0.0;
        iirSampleOL = 0.0;
        iirSamplePL = 0.0;
        iirSampleQL = 0.0;
        iirSampleRL = 0.0;
        iirSampleSL = 0.0;
        iirSampleTL = 0.0;
        iirSampleUL = 0.0;
        iirSampleVL = 0.0;
        iirSampleWL = 0.0;
        iirSampleXL = 0.0;
        iirSampleYL = 0.0;
        iirSampleZL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        iirSampleGR = 0.0;
        iirSampleHR = 0.0;
        iirSampleIR = 0.0;
        iirSampleJR = 0.0;
        iirSampleKR = 0.0;
        iirSampleLR = 0.0;
        iirSampleMR = 0.0;
        iirSampleNR = 0.0;
        iirSampleOR = 0.0;
        iirSamplePR = 0.0;
        iirSampleQR = 0.0;
        iirSampleRR = 0.0;
        iirSampleSR = 0.0;
        iirSampleTR = 0.0;
        iirSampleUR = 0.0;
        iirSampleVR = 0.0;
        iirSampleWR = 0.0;
        iirSampleXR = 0.0;
        iirSampleYR = 0.0;
        iirSampleZR = 0.0;
        oscGateL = 1.0;
        oscGateR = 1.0;
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
        kParamH = 7,
        kParamI = 8,
        kParamJ = 9,
        kNumParameters = 10
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
            case kParamH: H = value; break;
            case kParamI: I = value; break;
            case kParamJ: J = value; break;

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
            case kParamH: return H; break;
            case kParamI: return I; break;
            case kParamJ: return J; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.9; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.74; break;
            case kParamD: return 1.0; break;
            case kParamE: return 0.95; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.2; break;
            case kParamH: return 0.2; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "tgrind"; break;
            case kParamB: return "grd/out"; break;
            case kParamC: return "xover"; break;
            case kParamD: return "bsdrive"; break;
            case kParamE: return "bsvoice"; break;
            case kParamF: return "bassout"; break;
            case kParamG: return "sbdrive"; break;
            case kParamH: return "sbvoice"; break;
            case kParamI: return "subout"; break;
            case kParamJ: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "TGrind"; break;
            case kParamB: return "Grd/Out"; break;
            case kParamC: return "XOver"; break;
            case kParamD: return "BsDrive"; break;
            case kParamE: return "BsVoice"; break;
            case kParamF: return "BassOut"; break;
            case kParamG: return "SbDrive"; break;
            case kParamH: return "SbVoice"; break;
            case kParamI: return "SubOut"; break;
            case kParamJ: return "Dry/Wet"; break;

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
            case kParamH: return std::to_string(H); break;
            case kParamI: return std::to_string(I); break;
            case kParamJ: return std::to_string(J); break;

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
            case kParamH: return ""; break;
            case kParamI: return ""; break;
            case kParamJ: return ""; break;

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
        double driveone = pow(A * 3.0, 2);
        double driveoutput = pow(B, 2);
        double iirAmount = ((C * 0.33) + 0.1) / overallscale;
        double ataLowpassL;
        double ataLowpassR;
        double randyL;
        double invrandyL;
        double randyR;
        double invrandyR;
        double HeadBumpL = 0.0;
        double HeadBumpR = 0.0;
        double BassGain = D * 0.1;
        double HeadBumpFreq = ((E * 0.1) + 0.0001) / overallscale;
        double iirBmount = HeadBumpFreq / 44.1;
        double altBmount = 1.0 - iirBmount;
        double BassOutGain = pow(F, 2) * 0.5;
        double SubBumpL = 0.0;
        double SubBumpR = 0.0;
        double SubGain = G * 0.1;
        double SubBumpFreq = ((H * 0.1) + 0.0001) / overallscale;
        double iirCmount = SubBumpFreq / 44.1;
        double altCmount = 1.0 - iirCmount;
        double SubOutGain = pow(I, 2) * 0.45;
        double clampL = 0.0;
        double clampR = 0.0;
        double out;
        double fuzz = 0.111;
        double wet = J;
        double glitch = 0.60;
        double tempSampleL;
        double tempSampleR;
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            noisesourceL = noisesourceL % 1700021;
            noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
                inputSampleL -= applyresidue;
            }
            noisesourceR = noisesourceR % 1700021;
            noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
                inputSampleR -= applyresidue;
            }
            // for live air, we always apply the dither noise. Then, if our result is
            // effectively digital black, we'll subtract it aDubSub. We want a 'air' hiss
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            // here's the plan.
            // Grind Boost
            // Grind Output Level
            // Bass Split Freq
            // Bass Drive
            // Bass Voicing
            // Bass Output Level
            // Sub Oct Drive
            // Sub Voicing
            // Sub Output Level
            // Dry/Wet
            oscGateL += fabs(inputSampleL * 10.0);
            oscGateL -= 0.001;
            if (oscGateL > 1.0) {
                oscGateL = 1.0;
            }
            if (oscGateL < 0) {
                oscGateL = 0;
            }
            oscGateR += fabs(inputSampleR * 10.0);
            oscGateR -= 0.001;
            if (oscGateR > 1.0) {
                oscGateR = 1.0;
            }
            if (oscGateR < 0) {
                oscGateR = 0;
            }
            // got a value that only goes down low when there's silence or near silence on input
            clampL = 1.0 - oscGateL;
            clampL *= 0.00001;
            clampR = 1.0 - oscGateR;
            clampR *= 0.00001;
            // set up the thing to choke off oscillations- belt and suspenders affair
            if (flip)
            {
                tempSampleL = inputSampleL;
                iirDriveSampleAL = (iirDriveSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleAL;
                iirDriveSampleCL = (iirDriveSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleCL;
                iirDriveSampleEL = (iirDriveSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleEL;
                ataLowpassL = tempSampleL - inputSampleL;
                tempSampleR = inputSampleR;
                iirDriveSampleAR = (iirDriveSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleAR;
                iirDriveSampleCR = (iirDriveSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleCR;
                iirDriveSampleER = (iirDriveSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleER;
                ataLowpassR = tempSampleR - inputSampleR;
            }
            else
            {
                tempSampleL = inputSampleL;
                iirDriveSampleBL = (iirDriveSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleBL;
                iirDriveSampleDL = (iirDriveSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleDL;
                iirDriveSampleFL = (iirDriveSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleFL;
                ataLowpassL = tempSampleL - inputSampleL;
                tempSampleR = inputSampleR;
                iirDriveSampleBR = (iirDriveSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleBR;
                iirDriveSampleDR = (iirDriveSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleDR;
                iirDriveSampleFR = (iirDriveSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleFR;
                ataLowpassR = tempSampleR - inputSampleR;
            }
            // highpass section
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
            out = driveone;
            while (out > glitch)
            {
                out -= glitch;
                inputSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch));
                inputSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch));
                inputSampleL *= (1.0 + glitch);
                inputSampleR *= (1.0 + glitch);
            }
            // that's taken care of the really high gain stuff
            inputSampleL -= (inputSampleL * (fabs(inputSampleL) * out) * (fabs(inputSampleL) * out));
            inputSampleR -= (inputSampleR * (fabs(inputSampleR) * out) * (fabs(inputSampleR) * out));
            inputSampleL *= (1.0 + out);
            inputSampleR *= (1.0 + out);
            if (ataLowpassL > 0)
            {
                if (WasNegativeL) {
                    SubOctaveL = !SubOctaveL;
                }
                WasNegativeL = false;
            }
            else {
                WasNegativeL = true;
            }
            if (ataLowpassR > 0)
            {
                if (WasNegativeR) {
                    SubOctaveR = !SubOctaveR;
                }
                WasNegativeR = false;
            }
            else {
                WasNegativeR = true;
            }
            // set up polarities for sub-bass version
            randyL = (double(fpdL) / UINT32_MAX) * fuzz; // 0 to 1 the noise, may not be needed
            invrandyL = (1.0 - randyL);
            randyL /= 2.0;
            randyR = (double(fpdR) / UINT32_MAX) * fuzz; // 0 to 1 the noise, may not be needed
            invrandyR = (1.0 - randyR);
            randyR /= 2.0;
            // set up the noise
            iirSampleAL = (iirSampleAL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleAL;
            iirSampleBL = (iirSampleBL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleBL;
            iirSampleCL = (iirSampleCL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleCL;
            iirSampleDL = (iirSampleDL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleDL;
            iirSampleEL = (iirSampleEL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleEL;
            iirSampleFL = (iirSampleFL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleFL;
            iirSampleGL = (iirSampleGL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleGL;
            iirSampleHL = (iirSampleHL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleHL;
            iirSampleIL = (iirSampleIL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleIL;
            iirSampleJL = (iirSampleJL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleJL;
            iirSampleKL = (iirSampleKL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleKL;
            iirSampleLL = (iirSampleLL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleLL;
            iirSampleML = (iirSampleML * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleML;
            iirSampleNL = (iirSampleNL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleNL;
            iirSampleOL = (iirSampleOL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleOL;
            iirSamplePL = (iirSamplePL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSamplePL;
            iirSampleQL = (iirSampleQL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleQL;
            iirSampleRL = (iirSampleRL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleRL;
            iirSampleSL = (iirSampleSL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleSL;
            iirSampleTL = (iirSampleTL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleTL;
            iirSampleUL = (iirSampleUL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleUL;
            iirSampleVL = (iirSampleVL * altBmount) + (ataLowpassL * iirBmount);
            ataLowpassL -= iirSampleVL;
            iirSampleAR = (iirSampleAR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleAR;
            iirSampleBR = (iirSampleBR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleBR;
            iirSampleCR = (iirSampleCR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleCR;
            iirSampleDR = (iirSampleDR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleDR;
            iirSampleER = (iirSampleER * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleER;
            iirSampleFR = (iirSampleFR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleFR;
            iirSampleGR = (iirSampleGR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleGR;
            iirSampleHR = (iirSampleHR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleHR;
            iirSampleIR = (iirSampleIR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleIR;
            iirSampleJR = (iirSampleJR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleJR;
            iirSampleKR = (iirSampleKR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleKR;
            iirSampleLR = (iirSampleLR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleLR;
            iirSampleMR = (iirSampleMR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleMR;
            iirSampleNR = (iirSampleNR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleNR;
            iirSampleOR = (iirSampleOR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleOR;
            iirSamplePR = (iirSamplePR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSamplePR;
            iirSampleQR = (iirSampleQR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleQR;
            iirSampleRR = (iirSampleRR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleRR;
            iirSampleSR = (iirSampleSR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleSR;
            iirSampleTR = (iirSampleTR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleTR;
            iirSampleUR = (iirSampleUR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleUR;
            iirSampleVR = (iirSampleVR * altBmount) + (ataLowpassR * iirBmount);
            ataLowpassR -= iirSampleVR;
            switch (bflip)
            {
                case 1:
                    iirHeadBumpAL += (ataLowpassL * BassGain);
                    iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
                    iirHeadBumpAL = (invrandyL * iirHeadBumpAL) + (randyL * iirHeadBumpBL) + (randyL * iirHeadBumpCL);
                    if (iirHeadBumpAL > 0) {
                        iirHeadBumpAL -= clampL;
                    }
                    if (iirHeadBumpAL < 0) {
                        iirHeadBumpAL += clampL;
                    }
                    HeadBumpL = iirHeadBumpAL;
                    iirHeadBumpAR += (ataLowpassR * BassGain);
                    iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
                    iirHeadBumpAR = (invrandyR * iirHeadBumpAR) + (randyR * iirHeadBumpBR) + (randyR * iirHeadBumpCR);
                    if (iirHeadBumpAR > 0) {
                        iirHeadBumpAR -= clampR;
                    }
                    if (iirHeadBumpAR < 0) {
                        iirHeadBumpAR += clampR;
                    }
                    HeadBumpR = iirHeadBumpAR;
                    break;
                case 2:
                    iirHeadBumpBL += (ataLowpassL * BassGain);
                    iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
                    iirHeadBumpBL = (randyL * iirHeadBumpAL) + (invrandyL * iirHeadBumpBL) + (randyL * iirHeadBumpCL);
                    if (iirHeadBumpBL > 0) {
                        iirHeadBumpBL -= clampL;
                    }
                    if (iirHeadBumpBL < 0) {
                        iirHeadBumpBL += clampL;
                    }
                    HeadBumpL = iirHeadBumpBL;
                    iirHeadBumpBR += (ataLowpassR * BassGain);
                    iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
                    iirHeadBumpBR = (randyR * iirHeadBumpAR) + (invrandyR * iirHeadBumpBR) + (randyR * iirHeadBumpCR);
                    if (iirHeadBumpBR > 0) {
                        iirHeadBumpBR -= clampR;
                    }
                    if (iirHeadBumpBR < 0) {
                        iirHeadBumpBR += clampR;
                    }
                    HeadBumpR = iirHeadBumpBR;
                    break;
                case 3:
                    iirHeadBumpCL += (ataLowpassL * BassGain);
                    iirHeadBumpCL -= (iirHeadBumpCL * iirHeadBumpCL * iirHeadBumpCL * HeadBumpFreq);
                    iirHeadBumpCL = (randyL * iirHeadBumpAL) + (randyL * iirHeadBumpBL) + (invrandyL * iirHeadBumpCL);
                    if (iirHeadBumpCL > 0) {
                        iirHeadBumpCL -= clampL;
                    }
                    if (iirHeadBumpCL < 0) {
                        iirHeadBumpCL += clampL;
                    }
                    HeadBumpL = iirHeadBumpCL;
                    iirHeadBumpCR += (ataLowpassR * BassGain);
                    iirHeadBumpCR -= (iirHeadBumpCR * iirHeadBumpCR * iirHeadBumpCR * HeadBumpFreq);
                    iirHeadBumpCR = (randyR * iirHeadBumpAR) + (randyR * iirHeadBumpBR) + (invrandyR * iirHeadBumpCR);
                    if (iirHeadBumpCR > 0) {
                        iirHeadBumpCR -= clampR;
                    }
                    if (iirHeadBumpCR < 0) {
                        iirHeadBumpCR += clampR;
                    }
                    HeadBumpR = iirHeadBumpCR;
                    break;
            }
            iirSampleWL = (iirSampleWL * altBmount) + (HeadBumpL * iirBmount);
            HeadBumpL -= iirSampleWL;
            iirSampleXL = (iirSampleXL * altBmount) + (HeadBumpL * iirBmount);
            HeadBumpL -= iirSampleXL;
            iirSampleWR = (iirSampleWR * altBmount) + (HeadBumpR * iirBmount);
            HeadBumpR -= iirSampleWR;
            iirSampleXR = (iirSampleXR * altBmount) + (HeadBumpR * iirBmount);
            HeadBumpR -= iirSampleXR;
            SubBumpL = HeadBumpL;
            iirSampleYL = (iirSampleYL * altCmount) + (SubBumpL * iirCmount);
            SubBumpL -= iirSampleYL;
            SubBumpR = HeadBumpR;
            iirSampleYR = (iirSampleYR * altCmount) + (SubBumpR * iirCmount);
            SubBumpR -= iirSampleYR;
            SubBumpL = fabs(SubBumpL);
            if (SubOctaveL == false) {
                SubBumpL = -SubBumpL;
            }
            SubBumpR = fabs(SubBumpR);
            if (SubOctaveR == false) {
                SubBumpR = -SubBumpR;
            }
            switch (bflip)
            {
                case 1:
                    iirSubBumpAL += (SubBumpL * SubGain);
                    iirSubBumpAL -= (iirSubBumpAL * iirSubBumpAL * iirSubBumpAL * SubBumpFreq);
                    iirSubBumpAL = (invrandyL * iirSubBumpAL) + (randyL * iirSubBumpBL) + (randyL * iirSubBumpCL);
                    if (iirSubBumpAL > 0) {
                        iirSubBumpAL -= clampL;
                    }
                    if (iirSubBumpAL < 0) {
                        iirSubBumpAL += clampL;
                    }
                    SubBumpL = iirSubBumpAL;
                    iirSubBumpAR += (SubBumpR * SubGain);
                    iirSubBumpAR -= (iirSubBumpAR * iirSubBumpAR * iirSubBumpAR * SubBumpFreq);
                    iirSubBumpAR = (invrandyR * iirSubBumpAR) + (randyR * iirSubBumpBR) + (randyR * iirSubBumpCR);
                    if (iirSubBumpAR > 0) {
                        iirSubBumpAR -= clampR;
                    }
                    if (iirSubBumpAR < 0) {
                        iirSubBumpAR += clampR;
                    }
                    SubBumpR = iirSubBumpAR;
                    break;
                case 2:
                    iirSubBumpBL += (SubBumpL * SubGain);
                    iirSubBumpBL -= (iirSubBumpBL * iirSubBumpBL * iirSubBumpBL * SubBumpFreq);
                    iirSubBumpBL = (randyL * iirSubBumpAL) + (invrandyL * iirSubBumpBL) + (randyL * iirSubBumpCL);
                    if (iirSubBumpBL > 0) {
                        iirSubBumpBL -= clampL;
                    }
                    if (iirSubBumpBL < 0) {
                        iirSubBumpBL += clampL;
                    }
                    SubBumpL = iirSubBumpBL;
                    iirSubBumpBR += (SubBumpR * SubGain);
                    iirSubBumpBR -= (iirSubBumpBR * iirSubBumpBR * iirSubBumpBR * SubBumpFreq);
                    iirSubBumpBR = (randyR * iirSubBumpAR) + (invrandyR * iirSubBumpBR) + (randyR * iirSubBumpCR);
                    if (iirSubBumpBR > 0) {
                        iirSubBumpBR -= clampR;
                    }
                    if (iirSubBumpBR < 0) {
                        iirSubBumpBR += clampR;
                    }
                    SubBumpR = iirSubBumpBR;
                    break;
                case 3:
                    iirSubBumpCL += (SubBumpL * SubGain);
                    iirSubBumpCL -= (iirSubBumpCL * iirSubBumpCL * iirSubBumpCL * SubBumpFreq);
                    iirSubBumpCL = (randyL * iirSubBumpAL) + (randyL * iirSubBumpBL) + (invrandyL * iirSubBumpCL);
                    if (iirSubBumpCL > 0) {
                        iirSubBumpCL -= clampL;
                    }
                    if (iirSubBumpCL < 0) {
                        iirSubBumpCL += clampL;
                    }
                    SubBumpL = iirSubBumpCL;
                    iirSubBumpCR += (SubBumpR * SubGain);
                    iirSubBumpCR -= (iirSubBumpCR * iirSubBumpCR * iirSubBumpCR * SubBumpFreq);
                    iirSubBumpCR = (randyR * iirSubBumpAR) + (randyR * iirSubBumpBR) + (invrandyR * iirSubBumpCR);
                    if (iirSubBumpCR > 0) {
                        iirSubBumpCR -= clampR;
                    }
                    if (iirSubBumpCR < 0) {
                        iirSubBumpCR += clampR;
                    }
                    SubBumpR = iirSubBumpCR;
                    break;
            }
            iirSampleZL = (iirSampleZL * altCmount) + (SubBumpL * iirCmount);
            SubBumpL -= iirSampleZL;
            iirSampleZR = (iirSampleZR * altCmount) + (SubBumpR * iirCmount);
            SubBumpR -= iirSampleZR;
            inputSampleL *= driveoutput; // start with the drive section then add lows and subs
            inputSampleR *= driveoutput; // start with the drive section then add lows and subs
            inputSampleL += ((HeadBumpL + lastHeadBumpL) * BassOutGain);
            inputSampleL += ((SubBumpL + lastSubBumpL) * SubOutGain);
            inputSampleR += ((HeadBumpR + lastHeadBumpR) * BassOutGain);
            inputSampleR += ((SubBumpR + lastSubBumpR) * SubOutGain);
            lastHeadBumpL = HeadBumpL;
            lastSubBumpL = SubBumpL;
            lastHeadBumpR = HeadBumpR;
            lastSubBumpR = SubBumpR;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
            flip = !flip;
            bflip++;
            if (bflip < 1 || bflip > 3) {
                bflip = 1;
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
} // namespace airwindohhs::dubsub
