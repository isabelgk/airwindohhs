#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ironoxide5 {

constexpr std::string_view k_name{ "IronOxide5" };
constexpr std::string_view k_short_description{
    "IronOxide5 is the old school, heavily colored Airwindows tape emulation."
};
constexpr std::string_view k_long_description{
    "The legacy of this plugin goes way back. Many years ago, I was coding some of my first AU plugins, and some friends of mine were having bad experiences with a company that sold the big tape emulation plugin of the day. Outraged, I charged into the fray: I would code a replacement for them, one that did the same things and sounded better and sold for $60 (later $50). And that was Iron Oxide. It had one ‘ips’ control, a Drive, and an output level.Then, I expanded on that with Iron Oxide 2. That one split the ‘ips’ top and bottom cutoffs, so you could vary the ‘bandpassy’ quality it had. It used the same unusual algorithm, but made it more flexible. It also incorporated an unusual sort of anti-aliasing in the form of a ‘tape noise’ factor that blurred slew.Iron Oxide 3 added flutter. At this point, we stepped away from strictly zero latency: instead, the plugin declares zero latency but produces a fuzzy smear across one or two samples, the range the flutter covers. That persists with Iron Oxide 4 and 5, and is how the current free VST Iron Oxide works.Iron Oxide 4 added something else that (come to find out) is also present in the Delta Labs Effectron: inv/dry/wet control. That persists with Iron Oxide 5. The way you use it is, set up an Iron Oxide tone that accentuates a frequency range (like mids). Drive it, or leave it clean… but begin setting the control to inv (the inverted position). You’ll subtract it from dry, causing a dip rather than a boost, but if you’re saturating the ‘tape’ then the dip will leave dynamic energy in the area being cancelled: it will cut out fat, leaving punch. Overdrive the ‘tape’ section harder (and turn down ‘inv’) to get more punch out, or leave it clean and use it just to cancel out the area. It’s an unusual effect, but it works.Iron Oxide 5 is all of this, plus lessons from the ‘Purest’ line of plugins (mostly still in line to be released later) to produce the same thing as Iron Oxide 4, but even more pure and resonant and intense. None of these are really ‘mix buss’ plugins (though I’m not the boss of you): they’re far too intensely colored and distorted. They’re more about ‘make that snare really bark’ and so on. Though of course, since I’m not the boss of you, I can’t prevent you from trying to use it on the full mix. All I can do is say that ToTape is coming, and that’s the MODERN tape emulation. This is the old school, rowdy, obvious tape emulation, full of grunge and bark :)"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class IronOxide5 final : public Effect<T>
{
    double iirSamplehAL;
    double iirSamplehBL;
    double iirSampleAL;
    double iirSampleBL;
    double dL[264];
    double fastIIRAL;
    double fastIIRBL;
    double slowIIRAL;
    double slowIIRBL;
    double fastIIHAL;
    double fastIIHBL;
    double slowIIHAL;
    double slowIIHBL;
    double prevInputSampleL;
    double iirSamplehAR;
    double iirSamplehBR;
    double iirSampleAR;
    double iirSampleBR;
    double dR[264];
    double fastIIRAR;
    double fastIIRBR;
    double slowIIRAR;
    double slowIIRBR;
    double fastIIHAR;
    double fastIIHBR;
    double slowIIHAR;
    double slowIIHBR;
    double prevInputSampleR;
    int gcount;
    bool flip;
    double flL[100];
    double flR[100];
    int fstoredcount;
    double rateof;
    double sweep;
    double nextmax;
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
    IronOxide5()
    {
        A = 0.5; // 0.0 input trim in dB -18 to +18, default 0 ((A*36.0)-18.0)
        B = 0.562341325190349; // 15.0 ips 1.5 to 150.0 logarithmic. B*B
        C = 0.562341325190349; // (C*C) * (C*C) * 150 gives 15 ips (clamp to 1.5 minimum)
        // 15.0  (0.316227766016838)squared * 150 gives 15 ips
        D = 0.5; // 0.5 flutter 0 to 1
        E = 0.5; // 0.5 noise 0 to 1
        F = 0.5; // 0.0 output trim in dB -18 to +18, default 0 ((E*36.0)-18.0)
        G = 1.0; // 1.0 inv/dry/wet -1 0 1 ((F*2.0)-1.0)
        // needs very fussy defaults to comply with unusual defaults
        for (int temp = 0; temp < 263; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        gcount = 0;
        fastIIRAL = fastIIRBL = slowIIRAL = slowIIRBL = 0.0;
        fastIIHAL = fastIIHBL = slowIIHAL = slowIIHBL = 0.0;
        iirSamplehAL = iirSamplehBL = 0.0;
        iirSampleAL = iirSampleBL = 0.0;
        prevInputSampleL = 0.0;
        fastIIRAR = fastIIRBR = slowIIRAR = slowIIRBR = 0.0;
        fastIIHAR = fastIIHBR = slowIIHAR = slowIIHBR = 0.0;
        iirSamplehAR = iirSamplehBR = 0.0;
        iirSampleAR = iirSampleBR = 0.0;
        prevInputSampleR = 0.0;
        flip = false;
        for (int temp = 0; temp < 99; temp++) {
            flL[temp] = 0.0;
            flR[temp] = 0.0;
        }
        fstoredcount = 0;
        sweep = 0.0;
        rateof = 0.5;
        nextmax = 0.5;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.562341325190349; break;
            case kParamC: return 0.562341325190349; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input trim"; break;
            case kParamB: return "tape high"; break;
            case kParamC: return "tape low"; break;
            case kParamD: return "flutter"; break;
            case kParamE: return "noise"; break;
            case kParamF: return "output trim"; break;
            case kParamG: return "inv/dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Trim"; break;
            case kParamB: return "Tape High"; break;
            case kParamC: return "Tape Low"; break;
            case kParamD: return "Flutter"; break;
            case kParamE: return "Noise"; break;
            case kParamF: return "Output Trim"; break;
            case kParamG: return "Inv/Dry/Wet"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return "ips"; break;
            case kParamC: return "ips"; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return "dB"; break;
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
        double inputgain = pow(10.0, ((A * 36.0) - 18.0) / 20.0);
        double outputgain = pow(10.0, ((F * 36.0) - 18.0) / 20.0);
        double ips = (((B * B) * (B * B) * 148.5) + 1.5) * 1.1;
        // slight correction to dial in convincing ips settings
        if (ips < 1 || ips > 200) {
            ips = 33.0;
        }
        // sanity checks are always key
        double tempRandy = 0.04 + (0.11 / sqrt(ips));
        double randy;
        double lps = (((C * C) * (C * C) * 148.5) + 1.5) * 1.1;
        // slight correction to dial in convincing ips settings
        if (lps < 1 || lps > 200) {
            lps = 33.0;
        }
        // sanity checks are always key
        double iirAmount = lps / 430.0; // for low leaning
        double bridgerectifierL;
        double bridgerectifierR;
        double fastTaper = ips / 15.0;
        double slowTaper = 2.0 / (lps * lps);
        double lowspeedscale = (5.0 / ips);
        int count;
        int flutcount;
        double flutterrandy;
        double temp;
        double depth = pow(D, 2) * overallscale;
        double fluttertrim = 0.00581 / overallscale;
        double sweeptrim = (0.0005 * depth) / overallscale;
        double offset;
        double flooroffset;
        double tupi = 3.141592653589793238 * 2.0;
        double newrate = 0.006 / overallscale;
        double oldrate = 1.0 - newrate;
        if (overallscale == 0) {
            fastTaper += 1.0;
            slowTaper += 1.0;
        }
        else
        {
            iirAmount /= overallscale;
            lowspeedscale *= overallscale;
            fastTaper = 1.0 + (fastTaper / overallscale);
            slowTaper = 1.0 + (slowTaper / overallscale);
        }
        double noise = E * 0.5;
        double invdrywet = ((G * 2.0) - 1.0);
        double dry = 1.0;
        if (invdrywet > 0.0) {
            dry -= invdrywet;
        }
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            flutterrandy = (double(fpdL) / UINT32_MAX);
            // part of flutter section
            // now we've got a random flutter, so we're messing with the pitch before tape effects go on
            if (fstoredcount < 0 || fstoredcount > 30) {
                fstoredcount = 30;
            }
            flutcount = fstoredcount;
            flL[flutcount + 31] = flL[flutcount] = inputSampleL;
            flR[flutcount + 31] = flR[flutcount] = inputSampleR;
            offset = (1.0 + sin(sweep)) * depth;
            flooroffset = offset - floor(offset);
            flutcount += (int)floor(offset);
            bridgerectifierL = (flL[flutcount] * (1 - flooroffset));
            bridgerectifierR = (flR[flutcount] * (1 - flooroffset));
            bridgerectifierL += (flL[flutcount + 1] * flooroffset);
            bridgerectifierR += (flR[flutcount + 1] * flooroffset);
            rateof = (nextmax * newrate) + (rateof * oldrate);
            sweep += rateof * fluttertrim;
            sweep += sweep * sweeptrim;
            if (sweep >= tupi) {
                sweep = 0.0;
                nextmax = 0.02 + (flutterrandy * 0.98);
            }
            fstoredcount--;
            inputSampleL = bridgerectifierL;
            inputSampleR = bridgerectifierR;
            // apply to input signal, interpolate samples
            // all the funky renaming is just trying to fix how I never reassigned the control numbers
            if (flip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // do IIR highpass for leaning out
            inputSampleL *= inputgain;
            inputSampleR *= inputgain;
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // preliminary gain stage using antialiasing
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // preliminary gain stage using antialiasing
            // over to the Iron Oxide shaping code using inputsample
            if (gcount < 0 || gcount > 131) {
                gcount = 131;
            }
            count = gcount;
            // increment the counter
            dL[count + 131] = dL[count] = inputSampleL;
            dR[count + 131] = dR[count] = inputSampleR;
            if (flip)
            {
                fastIIRAL = fastIIRAL / fastTaper;
                slowIIRAL = slowIIRAL / slowTaper;
                fastIIRAL += dL[count];
                // scale stuff down
                fastIIRAR = fastIIRAR / fastTaper;
                slowIIRAR = slowIIRAR / slowTaper;
                fastIIRAR += dR[count];
                // scale stuff down
                count += 3;
                temp = dL[count + 127];
                temp += dL[count + 113];
                temp += dL[count + 109];
                temp += dL[count + 107];
                temp += dL[count + 103];
                temp += dL[count + 101];
                temp += dL[count + 97];
                temp += dL[count + 89];
                temp += dL[count + 83];
                temp /= 2;
                temp += dL[count + 79];
                temp += dL[count + 73];
                temp += dL[count + 71];
                temp += dL[count + 67];
                temp += dL[count + 61];
                temp += dL[count + 59];
                temp += dL[count + 53];
                temp += dL[count + 47];
                temp += dL[count + 43];
                temp += dL[count + 41];
                temp += dL[count + 37];
                temp += dL[count + 31];
                temp += dL[count + 29];
                temp /= 2;
                temp += dL[count + 23];
                temp += dL[count + 19];
                temp += dL[count + 17];
                temp += dL[count + 13];
                temp += dL[count + 11];
                temp /= 2;
                temp += dL[count + 7];
                temp += dL[count + 5];
                temp += dL[count + 3];
                temp /= 2;
                temp += dL[count + 2];
                temp += dL[count + 1]; // end L
                slowIIRAL += (temp / 128);
                temp = dR[count + 127];
                temp += dR[count + 113];
                temp += dR[count + 109];
                temp += dR[count + 107];
                temp += dR[count + 103];
                temp += dR[count + 101];
                temp += dR[count + 97];
                temp += dR[count + 89];
                temp += dR[count + 83];
                temp /= 2;
                temp += dR[count + 79];
                temp += dR[count + 73];
                temp += dR[count + 71];
                temp += dR[count + 67];
                temp += dR[count + 61];
                temp += dR[count + 59];
                temp += dR[count + 53];
                temp += dR[count + 47];
                temp += dR[count + 43];
                temp += dR[count + 41];
                temp += dR[count + 37];
                temp += dR[count + 31];
                temp += dR[count + 29];
                temp /= 2;
                temp += dR[count + 23];
                temp += dR[count + 19];
                temp += dR[count + 17];
                temp += dR[count + 13];
                temp += dR[count + 11];
                temp /= 2;
                temp += dR[count + 7];
                temp += dR[count + 5];
                temp += dR[count + 3];
                temp /= 2;
                temp += dR[count + 2];
                temp += dR[count + 1]; // end R
                slowIIRAR += (temp / 128);
                inputSampleL = fastIIRAL - (slowIIRAL / slowTaper);
                inputSampleR = fastIIRAR - (slowIIRAR / slowTaper);
            }
            else
            {
                fastIIRBL = fastIIRBL / fastTaper;
                slowIIRBL = slowIIRBL / slowTaper;
                fastIIRBL += dL[count];
                // scale stuff down
                fastIIRBR = fastIIRBR / fastTaper;
                slowIIRBR = slowIIRBR / slowTaper;
                fastIIRBR += dR[count];
                // scale stuff down
                count += 3;
                temp = dL[count + 127];
                temp += dL[count + 113];
                temp += dL[count + 109];
                temp += dL[count + 107];
                temp += dL[count + 103];
                temp += dL[count + 101];
                temp += dL[count + 97];
                temp += dL[count + 89];
                temp += dL[count + 83];
                temp /= 2;
                temp += dL[count + 79];
                temp += dL[count + 73];
                temp += dL[count + 71];
                temp += dL[count + 67];
                temp += dL[count + 61];
                temp += dL[count + 59];
                temp += dL[count + 53];
                temp += dL[count + 47];
                temp += dL[count + 43];
                temp += dL[count + 41];
                temp += dL[count + 37];
                temp += dL[count + 31];
                temp += dL[count + 29];
                temp /= 2;
                temp += dL[count + 23];
                temp += dL[count + 19];
                temp += dL[count + 17];
                temp += dL[count + 13];
                temp += dL[count + 11];
                temp /= 2;
                temp += dL[count + 7];
                temp += dL[count + 5];
                temp += dL[count + 3];
                temp /= 2;
                temp += dL[count + 2];
                temp += dL[count + 1];
                slowIIRBL += (temp / 128);
                temp = dR[count + 127];
                temp += dR[count + 113];
                temp += dR[count + 109];
                temp += dR[count + 107];
                temp += dR[count + 103];
                temp += dR[count + 101];
                temp += dR[count + 97];
                temp += dR[count + 89];
                temp += dR[count + 83];
                temp /= 2;
                temp += dR[count + 79];
                temp += dR[count + 73];
                temp += dR[count + 71];
                temp += dR[count + 67];
                temp += dR[count + 61];
                temp += dR[count + 59];
                temp += dR[count + 53];
                temp += dR[count + 47];
                temp += dR[count + 43];
                temp += dR[count + 41];
                temp += dR[count + 37];
                temp += dR[count + 31];
                temp += dR[count + 29];
                temp /= 2;
                temp += dR[count + 23];
                temp += dR[count + 19];
                temp += dR[count + 17];
                temp += dR[count + 13];
                temp += dR[count + 11];
                temp /= 2;
                temp += dR[count + 7];
                temp += dR[count + 5];
                temp += dR[count + 3];
                temp /= 2;
                temp += dR[count + 2];
                temp += dR[count + 1];
                slowIIRBR += (temp / 128);
                inputSampleL = fastIIRBL - (slowIIRBL / slowTaper);
                inputSampleR = fastIIRBR - (slowIIRBR / slowTaper);
            }
            inputSampleL /= fastTaper;
            inputSampleR /= fastTaper;
            inputSampleL /= lowspeedscale;
            inputSampleR /= lowspeedscale;
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            // can use as an output limiter
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            // can use as an output limiter
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            randy = (0.55 + tempRandy + ((double(fpdR) / UINT32_MAX) * tempRandy)) * noise; // 0 to 2
            inputSampleL *= (1.0 - randy);
            inputSampleL += (prevInputSampleL * randy);
            prevInputSampleL = drySampleL;
            inputSampleR *= (1.0 - randy);
            inputSampleR += (prevInputSampleR * randy);
            prevInputSampleR = drySampleR;
            // a slew-based noise generator: noise is only relative to how much change between samples there is.
            // This will cause high sample rates to be a little 'smoother' and clearer. I see this mechanic as something that
            // interacts with the sample rate. The 'dust' is scaled to the size of the samples.
            flip = !flip;
            // begin invdrywet block with outputgain
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
            if (invdrywet != 1.0) {
                inputSampleL *= invdrywet;
                inputSampleR *= invdrywet;
            }
            if (dry != 1.0) {
                drySampleL *= dry;
                drySampleR *= dry;
            }
            if (fabs(drySampleL) > 0.0) {
                inputSampleL += drySampleL;
            }
            if (fabs(drySampleR) > 0.0) {
                inputSampleR += drySampleR;
            }
            // end invdrywet block with outputgain
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
} // namespace airwindohhs::ironoxide5
