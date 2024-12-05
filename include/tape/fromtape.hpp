#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::fromtape {

constexpr std::string_view k_name{ "FromTape" };
constexpr std::string_view k_short_description{
    "FromTape is a minimalist, cleaner analog tape emulation."
};
constexpr std::string_view k_long_description{
    "Here’s something more… refined.FromTape was originally conceived as a ‘bump-less’ ToTape. It appeared with the original ToTape, and then with ToTape3, as a stripped-down version without the head bump, intended as very much the same thing but less bass. In some ways that’s still true.But, as ToTape grew to version 5, it developed many unusual traits. There was always that untameable head bump code, and its desire to throw DC everywhere (ToTape’s head bump literally doesn’t want to settle on 0, it wants to be either a positive or negative offset voltage by preference). There was the flutter. There was the built-in highpass, coded in a curious way to get a resonant quality around the corner frequency without any actual resonance applied. ToTape grew into a rich and strange effect, with many curious qualities and many fervent fans. And it’s out.And then there was FromTape.This FromTape draws on what I’d learned from the Purest plugins. It’s like no previous FromTape: elements have been rearranged, deleted, rethought until it became just this: the ‘unusual’ highpass (which accumulates tiny alterations in a buffer and then applies them in a single add for purity reasons) and THEN the Softer control, accentuated, but ONLY the Softer code and not the ‘Airwindows saturation’ that’s a major part of ToTape and allows for the ‘tape drive’ and saturation effects. The highpass is called ‘Weight’ and wired backwards so as you increase it, more bass comes out.So, this new FromTape does the very transparent treble softening, but has no real ‘distortion level’ because it has no distortion outside of Softer. It has the highpass (over a far broader range, and adjustable) but not the head bump the highpass was designed to handle. Instead of going after the other effects, the highpass goes first, and then the Softer works on the output of that.It’s capable of clipping to a set level only if Soften is cranked totally, and then it’s not a good sound (still available, though, in case you want it). Anything else will let peaks through largely undiminished. The highpass cuts bass, but in such a way that clean unclipped bass pre-FromTape might well turn into over 0dB output after FromTape: it cuts the bass in such a way that it might end up 3dB louder. Go figure, use the output level control to buffer it. Rather than loudenating stuff by ‘slamming it with tape saturation’ it’s more likely to reshape the tonality of the sound so it sounds quieter for whatever peak level you’re reaching.It sounds amazing. It’s also way more CPU-efficient than ToTape, and eats much less in terms of delay buffers and things. You could use it everywhere, certainly on channels where ToTape would be too heavy, but even on channels plus the 2-buss. You could use it in mastering if you wanted to soften digital edge while retaining total clarity, or if you wanted to take an overlimited mix and make the bass rounder and more open, giving a little crest factor back.I got lucky. FromTape sounds amazing, it really came together in a surprising way. You might like the added thickness and fullness of ToTape, or the bells and whistles, but if you want to call FromTape superior, you won’t be seeing an argument from me. Surprise! This might be your new best tape plugin, especially if you like subtlety and have ears like a bat."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class FromTape final : public Effect<T>
{
    int flip;
    double iirMidRollerAL;
    double iirMidRollerBL;
    double iirMidRollerCL;
    double iirMidRollerAR;
    double iirMidRollerBR;
    double iirMidRollerCR;
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
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;

  public:
    FromTape()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        iirMidRollerAL = 0.0;
        iirMidRollerBL = 0.0;
        iirMidRollerCL = 0.0;
        iirMidRollerAR = 0.0;
        iirMidRollerBR = 0.0;
        iirMidRollerCR = 0.0;
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
        flip = 0;
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
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "louder"; break;
            case kParamB: return "softer"; break;
            case kParamC: return "weight"; break;
            case kParamD: return "output"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Louder"; break;
            case kParamB: return "Softer"; break;
            case kParamC: return "Weight"; break;
            case kParamD: return "Output"; break;
            case kParamE: return "Dry/Wet"; break;

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
        double inputgain = A * 2.0;
        double SoftenControl = B;
        double RollAmount = (1.0 - SoftenControl) / overallscale;
        double iirAmount = (0.004 * (1.0 - C)) / overallscale;
        double altAmount = 1.0 - iirAmount;
        double outputgain = D * 2.0;
        double wet = E;
        // removed extra dry variable
        double HighsSampleL = 0.0;
        double SubtractL;
        double bridgerectifierL;
        double tempSampleL;
        double drySampleL;
        double HighsSampleR = 0.0;
        double SubtractR;
        double bridgerectifierR;
        double tempSampleR;
        double drySampleR;
        double randy;
        double invrandy;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            if (inputgain != 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            }
            randy = (double(fpdL) / UINT32_MAX) * SoftenControl; // for soften
            invrandy = (1.0 - randy);
            randy /= 2.0;
            // we've set up so that we dial in the amount of the alt sections (in pairs) with invrandy being the source section
            SubtractL = 0.0;
            SubtractR = 0.0;
            tempSampleL = inputSampleL;
            tempSampleR = inputSampleR;
            iirSampleAL = (iirSampleAL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleAL;
            SubtractL += iirSampleAL;
            iirSampleBL = (iirSampleBL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleBL;
            SubtractL += iirSampleBL;
            iirSampleCL = (iirSampleCL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleCL;
            SubtractL += iirSampleCL;
            iirSampleDL = (iirSampleDL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleDL;
            SubtractL += iirSampleDL;
            iirSampleEL = (iirSampleEL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleEL;
            SubtractL += iirSampleEL;
            iirSampleFL = (iirSampleFL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleFL;
            SubtractL += iirSampleFL;
            iirSampleGL = (iirSampleGL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleGL;
            SubtractL += iirSampleGL;
            iirSampleHL = (iirSampleHL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleHL;
            SubtractL += iirSampleHL;
            iirSampleIL = (iirSampleIL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleIL;
            SubtractL += iirSampleIL;
            iirSampleJL = (iirSampleJL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleJL;
            SubtractL += iirSampleJL;
            iirSampleKL = (iirSampleKL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleKL;
            SubtractL += iirSampleKL;
            iirSampleLL = (iirSampleLL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleLL;
            SubtractL += iirSampleLL;
            iirSampleML = (iirSampleML * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleML;
            SubtractL += iirSampleML;
            iirSampleNL = (iirSampleNL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleNL;
            SubtractL += iirSampleNL;
            iirSampleOL = (iirSampleOL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleOL;
            SubtractL += iirSampleOL;
            iirSamplePL = (iirSamplePL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSamplePL;
            SubtractL += iirSamplePL;
            iirSampleQL = (iirSampleQL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleQL;
            SubtractL += iirSampleQL;
            iirSampleRL = (iirSampleRL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleRL;
            SubtractL += iirSampleRL;
            iirSampleSL = (iirSampleSL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleSL;
            SubtractL += iirSampleSL;
            iirSampleTL = (iirSampleTL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleTL;
            SubtractL += iirSampleTL;
            iirSampleUL = (iirSampleUL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleUL;
            SubtractL += iirSampleUL;
            iirSampleVL = (iirSampleVL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleVL;
            SubtractL += iirSampleVL;
            iirSampleWL = (iirSampleWL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleWL;
            SubtractL += iirSampleWL;
            iirSampleXL = (iirSampleXL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleXL;
            SubtractL += iirSampleXL;
            iirSampleYL = (iirSampleYL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleYL;
            SubtractL += iirSampleYL;
            iirSampleZL = (iirSampleZL * altAmount) + (tempSampleL * iirAmount);
            tempSampleL -= iirSampleZL;
            SubtractL += iirSampleZL;
            iirSampleAR = (iirSampleAR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleAR;
            SubtractR += iirSampleAR;
            iirSampleBR = (iirSampleBR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleBR;
            SubtractR += iirSampleBR;
            iirSampleCR = (iirSampleCR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleCR;
            SubtractR += iirSampleCR;
            iirSampleDR = (iirSampleDR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleDR;
            SubtractR += iirSampleDR;
            iirSampleER = (iirSampleER * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleER;
            SubtractR += iirSampleER;
            iirSampleFR = (iirSampleFR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleFR;
            SubtractR += iirSampleFR;
            iirSampleGR = (iirSampleGR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleGR;
            SubtractR += iirSampleGR;
            iirSampleHR = (iirSampleHR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleHR;
            SubtractR += iirSampleHR;
            iirSampleIR = (iirSampleIR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleIR;
            SubtractR += iirSampleIR;
            iirSampleJR = (iirSampleJR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleJR;
            SubtractR += iirSampleJR;
            iirSampleKR = (iirSampleKR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleKR;
            SubtractR += iirSampleKR;
            iirSampleLR = (iirSampleLR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleLR;
            SubtractR += iirSampleLR;
            iirSampleMR = (iirSampleMR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleMR;
            SubtractR += iirSampleMR;
            iirSampleNR = (iirSampleNR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleNR;
            SubtractR += iirSampleNR;
            iirSampleOR = (iirSampleOR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleOR;
            SubtractR += iirSampleOR;
            iirSamplePR = (iirSamplePR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSamplePR;
            SubtractR += iirSamplePR;
            iirSampleQR = (iirSampleQR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleQR;
            SubtractR += iirSampleQR;
            iirSampleRR = (iirSampleRR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleRR;
            SubtractR += iirSampleRR;
            iirSampleSR = (iirSampleSR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleSR;
            SubtractR += iirSampleSR;
            iirSampleTR = (iirSampleTR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleTR;
            SubtractR += iirSampleTR;
            iirSampleUR = (iirSampleUR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleUR;
            SubtractR += iirSampleUR;
            iirSampleVR = (iirSampleVR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleVR;
            SubtractR += iirSampleVR;
            iirSampleWR = (iirSampleWR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleWR;
            SubtractR += iirSampleWR;
            iirSampleXR = (iirSampleXR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleXR;
            SubtractR += iirSampleXR;
            iirSampleYR = (iirSampleYR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleYR;
            SubtractR += iirSampleYR;
            iirSampleZR = (iirSampleZR * altAmount) + (tempSampleR * iirAmount);
            tempSampleR -= iirSampleZR;
            SubtractR += iirSampleZR;
            // do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
            // numbers being used. Don't keep doing it against the possibly high level signal number.
            // This has been known to add a resonant quality to the cutoff, which we're using on purpose.
            inputSampleL -= SubtractL;
            inputSampleR -= SubtractR;
            // apply stored up tiny corrections.
            if (flip < 1 || flip > 3) {
                flip = 1;
            }
            switch (flip)
            {
                case 1:
                    iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerAL = (invrandy * iirMidRollerAL) + (randy * iirMidRollerBL) + (randy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerAL;
                    iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerAR = (invrandy * iirMidRollerAR) + (randy * iirMidRollerBR) + (randy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerAR;
                    break;
                case 2:
                    iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerBL = (randy * iirMidRollerAL) + (invrandy * iirMidRollerBL) + (randy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerBL;
                    iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerBR = (randy * iirMidRollerAR) + (invrandy * iirMidRollerBR) + (randy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerBR;
                    break;
                case 3:
                    iirMidRollerCL = (iirMidRollerCL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerCL = (randy * iirMidRollerAL) + (randy * iirMidRollerBL) + (invrandy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerCL;
                    iirMidRollerCR = (iirMidRollerCR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerCR = (randy * iirMidRollerAR) + (randy * iirMidRollerBR) + (invrandy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerCR;
                    break;
            }
            flip++; // increment the triplet counter
            SubtractL = HighsSampleL;
            bridgerectifierL = fabs(SubtractL) * 1.57079633;
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = 1 - cos(bridgerectifierL);
            if (SubtractL > 0) {
                SubtractL = bridgerectifierL;
            }
            if (SubtractL < 0) {
                SubtractL = -bridgerectifierL;
            }
            inputSampleL -= SubtractL;
            SubtractR = HighsSampleR;
            bridgerectifierR = fabs(SubtractR) * 1.57079633;
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = 1 - cos(bridgerectifierR);
            if (SubtractR > 0) {
                SubtractR = bridgerectifierR;
            }
            if (SubtractR < 0) {
                SubtractR = -bridgerectifierR;
            }
            inputSampleR -= SubtractR;
            // Soften works using the MidRoller stuff, defining a bright parallel channel that we apply negative Density
            // to, and then subtract from the main audio. That makes the 'highs channel subtract' hit only the loudest
            // transients, plus we are subtracting any artifacts we got from the negative Density.
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
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
} // namespace airwindohhs::fromtape
