#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::totape5 {

constexpr std::string_view k_name{ "ToTape5" };
constexpr std::string_view k_short_description{
    "ToTape5 is Airwindows analog tape emulation."
};
constexpr std::string_view k_long_description{
    "ToTape5 is the best Airwindows analog tape emulation. It builds upon the previous four versions (which have been some of my best sellers) and incorporates everything learned from the Purest series of plugins, to produce a tape emulation that does what analog does. (Analog tape does really good things for mixes, and it’s very difficult to get it right without sinking into a morass of overprocessing and digital blandness)It’s better than Iron Oxide, always was. Iron Oxide is for ‘slamming tape for effect’, for putting on individual tracks, not realism. ToTape is for realism and quality: for ‘mixing to tape’, in the box. I don’t think there is anything else that can stand as much scrutiny as ToTape will: it’s developed on mastering-grade gear and when used in its most optimized state, it’s not a toy. It should be more transparent and musical than most plugins (never mind ‘tape emulation’ plugins, which are generally not even as transparent and musical as a good digital EQ plugin).It has six controls: four if you don’t count Output and Dry/Wet, which are pretty obvious.Louder defaults to 0.25 because the tape emulation soaks up some level. You can set it to 0 for added purity (it removes a gain trim stage if you do) and if you do that, you can plainly hear that the emulated tape ‘soaks up’ some of the audio, noticeably dropping the level while not seeming to alter the tone at all. There are no gain adjustments making that happen, it’s entirely tape saturation which is very transparent. (There is no compression, either.)Softer is the treble softening. Defaults to 0, which is still on but very hi-fi and subtle. You can turn it up to get a more ‘old’ tape machine tone, and like the other controls if you need to finetune the effect by ear, your adjustments will probably be around 0.1 to 0.2 if you mean to retain the full fidelity of the mix.Fatter is the head bump. Defaults to 0, which is still on, but subtle. It can be cranked up to silly/stupid levels if you like. Again, if you want to emphasize the extra roundness and fullness of tape, you might be tweaking this to 0.1 or 0.2. If you don’t have fantastic subwoofers or monitoring that can handle deep bass, leave this at 0! Boosting it will introduce deep lows very cleanly and you might not hear them unless your monitoring is up to scratch. You will also have to turn down Output if you boost this a lot.Flutter is the tape flutter. Defaults to 0 which is OFF, see comment on ‘dry/wet’. The most amazingly awesome tape recorders did NOT have loads of flutter, but if you want a little ‘spaciness’ or ‘atmosphere’ you can put in small amounts of this, like 0.1 or so. Go by feel, if you can hear it fluttering it’s kind of too much. If you’re using this, please don’t use Dry/Wet to combine the result with dry: you’ll create a flangey effect and it’ll be more obvious than it should be. By design, Flutter is made so you can increase it until it’s too much, so please remember that realistic levels are more like 0.1: too subtle to immediately hear. Go by feel, or pretend you have a really terrific tape machine and leave it off entirely, set Flutter to 0. (For instance, anyone who’s mastering and intentionally adds flutter ought to think hard about whether that’s really helping.)"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class ToTape5 final : public Effect<T>
{
    int gcount;
    double rateof;
    double sweep;
    double nextmax;
    int hcount;
    int flip;
    double dL[1000];
    double eL[1000];
    double dR[1000];
    double eR[1000];
    double iirMidRollerAL;
    double iirMidRollerBL;
    double iirMidRollerCL;
    double iirHeadBumpAL;
    double iirHeadBumpBL;
    double iirHeadBumpCL;
    double iirMinHeadBumpL;
    double iirMidRollerAR;
    double iirMidRollerBR;
    double iirMidRollerCR;
    double iirHeadBumpAR;
    double iirHeadBumpBR;
    double iirHeadBumpCR;
    double iirMinHeadBumpR;
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
    float F;

  public:
    ToTape5()
    {
        A = 0.25;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        E = 1.0;
        F = 1.0;
        iirMidRollerAL = 0.0;
        iirMidRollerBL = 0.0;
        iirMidRollerCL = 0.0;
        iirHeadBumpAL = 0.0;
        iirHeadBumpBL = 0.0;
        iirHeadBumpCL = 0.0;
        iirMinHeadBumpL = 0.0;
        iirMidRollerAR = 0.0;
        iirMidRollerBR = 0.0;
        iirMidRollerCR = 0.0;
        iirHeadBumpAR = 0.0;
        iirHeadBumpBR = 0.0;
        iirHeadBumpCR = 0.0;
        iirMinHeadBumpR = 0.0;
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
        for (int temp = 0; temp < 999; temp++) {
            dL[temp] = 0.0;
            eL[temp] = 0.0;
            dR[temp] = 0.0;
            eR[temp] = 0.0;
        }
        gcount = 0;
        rateof = 0.5;
        sweep = 0.0;
        nextmax = 0.5;
        hcount = 0;
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
            case kParamA: return 0.25; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.0; break;
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
            case kParamA: return "louder"; break;
            case kParamB: return "softer"; break;
            case kParamC: return "fatter"; break;
            case kParamD: return "flutter"; break;
            case kParamE: return "output"; break;
            case kParamF: return "dry/wet"; break;

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
            case kParamC: return "Fatter"; break;
            case kParamD: return "Flutter"; break;
            case kParamE: return "Output"; break;
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
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double inputgain = pow(A + 1.0, 3);
        double outputgain = E;
        double wet = F;
        // removed extra dry variable
        double trim = 0.211324865405187117745425;
        double SoftenControl = pow(B, 2);
        double tempRandy = 0.06 + (SoftenControl / 10.0);
        double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
        double HeadBumpControl = pow(C, 2);
        int allpasstemp;
        int maxdelay = (int)(floor(((HeadBumpControl + 0.3) * 2.2) * overallscale));
        HeadBumpControl *= fabs(HeadBumpControl);
        double HeadBumpFreq = 0.044 / overallscale;
        double iirAmount = 0.000001 / overallscale;
        double altAmount = 1.0 - iirAmount;
        double iirHBoostAmount = 0.0001 / overallscale;
        double altHBoostAmount = 1.0 - iirAmount;
        double depth = pow(D, 2) * overallscale;
        double fluttertrim = 0.005 / overallscale;
        double sweeptrim = (0.0006 * depth) / overallscale;
        double offset;
        double tupi = 3.141592653589793238 * 2.0;
        double newrate = 0.005 / overallscale;
        double oldrate = 1.0 - newrate;
        double flutterrandy;
        double randy;
        double invrandy;
        int count;
        double HighsSampleL = 0.0;
        double NonHighsSampleL = 0.0;
        double HeadBumpL = 0.0;
        double SubtractL;
        double bridgerectifierL;
        double tempSampleL;
        double drySampleL;
        double HighsSampleR = 0.0;
        double NonHighsSampleR = 0.0;
        double HeadBumpR = 0.0;
        double SubtractR;
        double bridgerectifierR;
        double tempSampleR;
        double drySampleR;
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
            flutterrandy = (double(fpdL) / UINT32_MAX);
            randy = flutterrandy * tempRandy; // for soften
            invrandy = (1.0 - randy);
            randy /= 2.0;
            // we've set up so that we dial in the amount of the alt sections (in pairs) with invrandy being the source section
            // now we've got a random flutter, so we're messing with the pitch before tape effects go on
            if (gcount < 0 || gcount > 300) {
                gcount = 300;
            }
            count = gcount;
            dL[count + 301] = dL[count] = inputSampleL;
            dR[count + 301] = dR[count] = inputSampleR;
            gcount--;
            // we will also keep the buffer going, even when not in use
            if (depth != 0.0) {
                offset = (1.0 + sin(sweep)) * depth;
                count += (int)floor(offset);
                bridgerectifierL = (dL[count] * (1 - (offset - floor(offset))));
                bridgerectifierL += (dL[count + 1] * (offset - floor(offset)));
                bridgerectifierL -= ((dL[count + 2] * (offset - floor(offset))) * trim);
                bridgerectifierR = (dR[count] * (1 - (offset - floor(offset))));
                bridgerectifierR += (dR[count + 1] * (offset - floor(offset)));
                bridgerectifierR -= ((dR[count + 2] * (offset - floor(offset))) * trim);
                rateof = (nextmax * newrate) + (rateof * oldrate);
                sweep += rateof * fluttertrim;
                sweep += sweep * sweeptrim;
                if (sweep >= tupi) {
                    sweep = 0.0;
                    nextmax = 0.02 + (flutterrandy * 0.98);
                }
                inputSampleL = bridgerectifierL;
                inputSampleR = bridgerectifierR;
                // apply to input signal only when flutter is present, interpolate samples
            }
            if (inputgain != 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            }
            if (flip < 1 || flip > 3) {
                flip = 1;
            }
            switch (flip)
            {
                case 1:
                    iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerAL = (invrandy * iirMidRollerAL) + (randy * iirMidRollerBL) + (randy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerAL;
                    NonHighsSampleL = iirMidRollerAL;
                    iirHeadBumpAL += (inputSampleL * 0.05);
                    iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
                    iirHeadBumpAL = (invrandy * iirHeadBumpAL) + (randy * iirHeadBumpBL) + (randy * iirHeadBumpCL);
                    iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerAR = (invrandy * iirMidRollerAR) + (randy * iirMidRollerBR) + (randy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerAR;
                    NonHighsSampleR = iirMidRollerAR;
                    iirHeadBumpAR += (inputSampleR * 0.05);
                    iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
                    iirHeadBumpAR = (invrandy * iirHeadBumpAR) + (randy * iirHeadBumpBR) + (randy * iirHeadBumpCR);
                    break;
                case 2:
                    iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerBL = (randy * iirMidRollerAL) + (invrandy * iirMidRollerBL) + (randy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerBL;
                    NonHighsSampleL = iirMidRollerBL;
                    iirHeadBumpBL += (inputSampleL * 0.05);
                    iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
                    iirHeadBumpBL = (randy * iirHeadBumpAL) + (invrandy * iirHeadBumpBL) + (randy * iirHeadBumpCL);
                    iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerBR = (randy * iirMidRollerAR) + (invrandy * iirMidRollerBR) + (randy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerBR;
                    NonHighsSampleR = iirMidRollerBR;
                    iirHeadBumpBR += (inputSampleR * 0.05);
                    iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
                    iirHeadBumpBR = (randy * iirHeadBumpAR) + (invrandy * iirHeadBumpBR) + (randy * iirHeadBumpCR);
                    break;
                case 3:
                    iirMidRollerCL = (iirMidRollerCL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                    iirMidRollerCL = (randy * iirMidRollerAL) + (randy * iirMidRollerBL) + (invrandy * iirMidRollerCL);
                    HighsSampleL = inputSampleL - iirMidRollerCL;
                    NonHighsSampleL = iirMidRollerCL;
                    iirHeadBumpCL += (inputSampleL * 0.05);
                    iirHeadBumpCL -= (iirHeadBumpCL * iirHeadBumpCL * iirHeadBumpCL * HeadBumpFreq);
                    iirHeadBumpCL = (randy * iirHeadBumpAL) + (randy * iirHeadBumpBL) + (invrandy * iirHeadBumpCL);
                    iirMidRollerCR = (iirMidRollerCR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                    iirMidRollerCR = (randy * iirMidRollerAR) + (randy * iirMidRollerBR) + (invrandy * iirMidRollerCR);
                    HighsSampleR = inputSampleR - iirMidRollerCR;
                    NonHighsSampleR = iirMidRollerCR;
                    iirHeadBumpCR += (inputSampleR * 0.05);
                    iirHeadBumpCR -= (iirHeadBumpCR * iirHeadBumpCR * iirHeadBumpCR * HeadBumpFreq);
                    iirHeadBumpCR = (randy * iirHeadBumpAR) + (randy * iirHeadBumpBR) + (invrandy * iirHeadBumpCR);
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
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifierL;
            }
            if (inputSampleL < 0) {
                inputSampleL = -bridgerectifierL;
            }
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifierR;
            }
            if (inputSampleR < 0) {
                inputSampleR = -bridgerectifierR;
            }
            // drive section: the tape sound includes a very gentle saturation curve, which is always an attenuation.
            // we cut back on highs before hitting this, and then we're going to subtract highs a second time after.
            HeadBumpL = iirHeadBumpAL + iirHeadBumpBL + iirHeadBumpCL;
            HeadBumpR = iirHeadBumpAR + iirHeadBumpBR + iirHeadBumpCR;
            // begin PhaseNudge
            allpasstemp = hcount - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay) {
                allpasstemp = maxdelay;
            }
            HeadBumpL -= eL[allpasstemp] * fpOld;
            eL[hcount] = HeadBumpL;
            inputSampleL *= fpOld;
            HeadBumpR -= eR[allpasstemp] * fpOld;
            eR[hcount] = HeadBumpR;
            inputSampleR *= fpOld;
            hcount--;
            if (hcount < 0 || hcount > maxdelay) {
                hcount = maxdelay;
            }
            HeadBumpL += (eL[hcount]);
            HeadBumpR += (eR[hcount]);
            // end PhaseNudge on head bump in lieu of delay.
            SubtractL -= (HeadBumpL * (HeadBumpControl + iirMinHeadBumpL));
            SubtractR -= (HeadBumpR * (HeadBumpControl + iirMinHeadBumpR));
            // makes a second soften and a single head bump after saturation.
            // we are going to retain this, and then feed it into the highpass filter. That way, we can skip a subtract.
            // Head Bump retains a trace which is roughly as large as what the highpass will do.
            tempSampleL = inputSampleL;
            tempSampleR = inputSampleR;
            iirMinHeadBumpL = (iirMinHeadBumpL * altHBoostAmount) + (fabs(inputSampleL) * iirHBoostAmount);
            if (iirMinHeadBumpL > 0.01) {
                iirMinHeadBumpL = 0.01;
            }
            iirMinHeadBumpR = (iirMinHeadBumpR * altHBoostAmount) + (fabs(inputSampleR) * iirHBoostAmount);
            if (iirMinHeadBumpR > 0.01) {
                iirMinHeadBumpR = 0.01;
            }
            // we want this one rectified so that it's a relatively steady positive value. Boosts can cause it to be
            // greater than 1 so we clamp it in that case.
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
} // namespace airwindohhs::totape5
