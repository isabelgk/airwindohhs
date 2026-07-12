#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::derez4 {

constexpr std::string_view k_name{ "DeRez4" };
constexpr std::string_view k_short_description{
    "DeRez4 perfects retro digital tones."
};
constexpr std::string_view k_long_description{
    "Still working like mad on Meter, but I've got something of a special treat.What if, rather than go to great lengths to emulate something super vintage and unobtainable, you just… did something else, that ended up pulling a lot of weight and being great at exactly what the vintage thing did, at its best?DeRez4 is the first retro digital tone-maker to do what it does without ANY bitcrushing. Seriously. It runs everything at full floating point resolution… double precision, where that's available such as in Reaper using the VST build. It's doing it all with a sort of frequency crushing… but not like any kind I've tried before. And while it makes use of my Bezier filters, the guts of it are totally made out of hard, pointy line segments laid end to end, which I'm not sure if that's been used for this purpose before (after all, everyone trying to get the retro digital thing starts with bitcrushing and goes from there to layer 'analog color' over it…)This one's radically different from all that. The underlying frequency crush can be used to resynthesize pointy transients on upsampled audio, in line with some of the scarier high end audiophile designs. It'll throw a layer of glitter onto anything you give it, but very much 'fake digital' in nature, not anything natural. That's the underlying engine. At much lower frequency crushes, it adds a characteristic edge.The other two controls, Bright and Bassis, are each double Bezier filters. One in front of the frequency crush, one behind. Bright is sort of able to resist aliasing but not really: its real strength lies in drawing neat little radiuses to round off every point the frequency-crush creates. Doesn't matter what sample rate you're at, it just helps it work better. Then Bassis functions as a Bezier highpass… and layered the way that it is, it brings a curious resonant quality to whatever you put through DeRez4. Then there's an output trim, because these things can lead to peaks becoming more punchy and energetic than the original sound.DeRez4 is able to make a strong claim for 'better at retro digital vibe than even modeling the heck out of real retro machines'. It's always possible to get real live raw hardware (at great cost) to go do this sort of thing for real and all… but in the absence of that, or if it's a lot of trouble to run with that authentic workflow, it turns out there's another path to consider.That's DeRez4: instead of emulating, do an entirely other thing, done at the highest possible resolution but generating a whole other kind of sound that turns out to do a startlingly similar job. Conveniently, and for free."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeRez4 final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'drz4';
    float A;
    float B;
    float C;
    float D;
    enum
    {
        bez_HMAL,
        bez_HMBL,
        bez_HMCL,
        bez_HMDL,
        bez_LMAL,
        bez_LMBL,
        bez_LMCL,
        bez_LMDL,
        bez_HMAR,
        bez_HMBR,
        bez_HMCR,
        bez_HMDR,
        bez_HMcycle,
        bez_LMAR,
        bez_LMBR,
        bez_LMCR,
        bez_LMDR,
        bez_LMcycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezEQ[bez_total][2];
    double pointCycle, pointAL, pointBL, pointAR, pointBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeRez4()
    {
        A = 0.618;
        B = 0.618;
        C = 0.382;
        D = 1.0;
        for (int x = 0; x < bez_total; x++) {
            for (int y = 0; y < 2; y++) {
                bezEQ[x][y] = 0.0;
            }
        }
        pointCycle = pointAL = pointBL = pointAR = pointBR = 0.0;
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
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.618; break;
            case kParamB: return 0.618; break;
            case kParamC: return 0.382; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "downrez"; break;
            case kParamB: return "bright"; break;
            case kParamC: return "bassis"; break;
            case kParamD: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "DownRez"; break;
            case kParamB: return "Bright"; break;
            case kParamC: return "Bassis"; break;
            case kParamD: return "Output"; break;

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
        double Sderez = pow(A, 2.0 + overallscale);
        int stepped = 999999;
        if (Sderez > 0.000001) {
            stepped = (int)(1.0 / Sderez);
        }
        if (Sderez > 0.0) {
            Sderez = 0.99999999 / stepped;
        }
        double Strim = 1.0 - (Sderez * ((double)stepped / (stepped + 1.0)));
        double brightDerez = (1.0 - pow(B, 2.0)) * 4.0 * overallscale;
        stepped = 999999;
        if (brightDerez > 1.0) {
            stepped = (int)brightDerez;
            brightDerez = 0.99999999 / stepped;
        }
        else {
            brightDerez = 1.1;
        }
        double brightTrim = 1.0 - (brightDerez * ((double)stepped / (stepped + 1.0)));
        double bassRef = pow(C, 4.0);
        double bassisDerez = (bassRef * 0.25) / overallscale;
        stepped = 999999;
        if (bassisDerez > 0.000001) {
            stepped = (int)(1.0 / bassisDerez);
        }
        if (bassisDerez > 0.0) {
            bassisDerez = 0.99999999 / stepped;
        }
        double bassisTrim = 1.0 - (bassisDerez * ((double)stepped / (stepped + 1.0)));
        double output = D;
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
            if (brightDerez < 1.0) {
                bezEQ[bez_HMcycle][0] += brightDerez;
                bezEQ[bez_HMAL][0] += (inputSampleL * 0.5 * brightDerez);
                bezEQ[bez_HMAR][0] += (inputSampleR * 0.5 * brightDerez);
                if (bezEQ[bez_HMcycle][0] > brightTrim) {
                    bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0];
                    bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0];
                    bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0] * (0.5 - (brightDerez * 0.082));
                    bezEQ[bez_HMAL][0] = 0.0;
                    bezEQ[bez_HMDR][0] = bezEQ[bez_HMCR][0];
                    bezEQ[bez_HMCR][0] = bezEQ[bez_HMBR][0];
                    bezEQ[bez_HMBR][0] = bezEQ[bez_HMAR][0] * (0.5 - (brightDerez * 0.082));
                    bezEQ[bez_HMAR][0] = 0.0;
                    bezEQ[bez_HMcycle][0] = 0.0;
                }
                double X = bezEQ[bez_HMcycle][0];
                inputSampleL = bezEQ[bez_HMCL][0] + (bezEQ[bez_HMDL][0] * (1.0 - X) * (1.0 - X));
                inputSampleL += (bezEQ[bez_HMCL][0] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBL][0] * X * X);
                inputSampleL /= brightTrim;
                inputSampleR = bezEQ[bez_HMCR][0] + (bezEQ[bez_HMDR][0] * (1.0 - X) * (1.0 - X));
                inputSampleR += (bezEQ[bez_HMCR][0] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBR][0] * X * X);
                inputSampleR /= brightTrim;
            }
            if (bassisDerez > 0.0) {
                bezEQ[bez_LMcycle][0] += bassisDerez;
                bezEQ[bez_LMAL][0] += (inputSampleL * bassisDerez);
                bezEQ[bez_LMAR][0] += (inputSampleR * bassisDerez);
                if (bezEQ[bez_LMcycle][0] > bassisTrim) {
                    bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0];
                    bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0];
                    bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0] * (0.5 - (bassisDerez * 0.082));
                    bezEQ[bez_LMAL][0] = 0.0;
                    bezEQ[bez_LMDR][0] = bezEQ[bez_LMCR][0];
                    bezEQ[bez_LMCR][0] = bezEQ[bez_LMBR][0];
                    bezEQ[bez_LMBR][0] = bezEQ[bez_LMAR][0] * (0.5 - (bassisDerez * 0.082));
                    bezEQ[bez_LMAR][0] = 0.0;
                    bezEQ[bez_LMcycle][0] = 0.0;
                }
                double X = bezEQ[bez_LMcycle][0];
                inputSampleL -= (bezEQ[bez_LMCL][0] + (bezEQ[bez_LMDL][0] * (1.0 - X) * (1.0 - X))) * 0.5;
                inputSampleL -= ((bezEQ[bez_LMCL][0] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBL][0] * X * X)) * 0.5;
                inputSampleL /= bassisTrim;
                inputSampleR -= (bezEQ[bez_LMCR][0] + (bezEQ[bez_LMDR][0] * (1.0 - X) * (1.0 - X))) * 0.5;
                inputSampleR -= ((bezEQ[bez_LMCR][0] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBR][0] * X * X)) * 0.5;
                inputSampleR /= bassisTrim;
            }
            if (Sderez < 1.0) {
                pointCycle += Sderez;
                if (pointCycle > 0.9999999) {
                    pointAL = pointBL;
                    pointBL = inputSampleL;
                    pointAR = pointBR;
                    pointBR = inputSampleR;
                    pointCycle = 0.0;
                }
                double X = pointCycle * Strim;
                if (Sderez > 0.0) {
                    inputSampleL = pointAL + ((pointBL - pointAL) * X);
                    inputSampleL /= Strim;
                    inputSampleR = pointAR + ((pointBR - pointAR) * X);
                    inputSampleR /= Strim;
                }
                else {
                    inputSampleL = 0.0;
                    inputSampleR = 0.0;
                }
            }
            if (brightDerez < 1.0) {
                bezEQ[bez_HMcycle][1] += brightDerez;
                bezEQ[bez_HMAL][1] += (inputSampleL * brightDerez);
                bezEQ[bez_HMAR][1] += (inputSampleR * brightDerez);
                if (bezEQ[bez_HMcycle][1] > brightTrim) {
                    bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1];
                    bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1];
                    bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1] * (0.5 - (brightDerez * 0.082));
                    bezEQ[bez_HMAL][1] = 0.0;
                    bezEQ[bez_HMDR][1] = bezEQ[bez_HMCR][1];
                    bezEQ[bez_HMCR][1] = bezEQ[bez_HMBR][1];
                    bezEQ[bez_HMBR][1] = bezEQ[bez_HMAR][1] * (0.5 - (brightDerez * 0.082));
                    bezEQ[bez_HMAR][1] = 0.0;
                    bezEQ[bez_HMcycle][1] = 0.0;
                }
                double X = bezEQ[bez_HMcycle][1];
                inputSampleL = bezEQ[bez_HMCL][1] + (bezEQ[bez_HMDL][1] * (1.0 - X) * (1.0 - X));
                inputSampleL += (bezEQ[bez_HMCL][1] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBL][1] * X * X);
                inputSampleL /= brightTrim;
                inputSampleR = bezEQ[bez_HMCR][1] + (bezEQ[bez_HMDR][1] * (1.0 - X) * (1.0 - X));
                inputSampleR += (bezEQ[bez_HMCR][1] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBR][1] * X * X);
                inputSampleR /= brightTrim;
            }
            if (bassisDerez > 0.0) {
                bezEQ[bez_LMcycle][1] += bassisDerez;
                bezEQ[bez_LMAL][1] += (inputSampleL * bassisDerez);
                bezEQ[bez_LMAR][1] += (inputSampleR * bassisDerez);
                if (bezEQ[bez_LMcycle][1] > bassisTrim) {
                    bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1];
                    bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1];
                    bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1] * (0.5 - (bassisDerez * 0.082));
                    bezEQ[bez_LMAL][1] = 0.0;
                    bezEQ[bez_LMDR][1] = bezEQ[bez_LMCR][1];
                    bezEQ[bez_LMCR][1] = bezEQ[bez_LMBR][1];
                    bezEQ[bez_LMBR][1] = bezEQ[bez_LMAR][1] * (0.5 - (bassisDerez * 0.082));
                    bezEQ[bez_LMAR][1] = 0.0;
                    bezEQ[bez_LMcycle][1] = 0.0;
                }
                double X = bezEQ[bez_LMcycle][1];
                inputSampleL -= (bezEQ[bez_LMCL][1] + (bezEQ[bez_LMDL][1] * (1.0 - X) * (1.0 - X))) * 0.5;
                inputSampleL -= ((bezEQ[bez_LMCL][1] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBL][1] * X * X)) * 0.5;
                inputSampleL /= bassisTrim;
                inputSampleR -= (bezEQ[bez_LMCR][1] + (bezEQ[bez_LMDR][1] * (1.0 - X) * (1.0 - X))) * 0.5;
                inputSampleR -= ((bezEQ[bez_LMCR][1] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBR][1] * X * X)) * 0.5;
                inputSampleR /= bassisTrim;
            }
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::derez4
