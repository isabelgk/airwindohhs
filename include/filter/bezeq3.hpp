#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::bezeq3 {

constexpr std::string_view k_name{ "BezEQ3" };
constexpr std::string_view k_short_description{
    "BezEQ3 refines the concept into four vivid, unified bands."
};
constexpr std::string_view k_long_description{
    "Persistence has its rewards…The idea was to see if BezEQ could be expanded into something I can use in a Console, for which I like having four bands and frequency sweep controls for each. But the delay lines built into BezEQ2 had to go, because I'd also want zero latency… and that would produce a phasier response, perhaps interactions between bands that would make it behave less normally. But normal's easy to come by, SmoothEQ2 exists, so what if we could get something worth having that's less normal?Enter BezEQ3. There's more improvements on getting it to filter smoothly, there's that needed 4th band, and the sweepable middle positions to the bands, but it's still very much BezEQ. It's refined that unusual tonality down, and is still using a filter topology where, set flat, it's literally perfectly transparent.And then it's perfectly transparent for any other setting, no matter how extreme… but how you GET to those settings is something else.Because removing the internal delay lines and making it be zero latency, derailed the filter bands in some peculiar way. If you boost a frequency, you're cutting something else. Everything's kind of interactive. It's not going to behave nicely if you think you can treat it like shelving filters with neat markings. No point EVEN marking it. This is not SmoothEQ2, which runs on DF1 biquad filters like a normal plugin.Instead, you voice it by using the bands and sweeps as suggestions, and you listen to what it's doing. Nothing will get a frequency band to separate out and act like a 'frequency band', and curiously nothing will get frequency bands to come forward or fall back (like FatEQ) either. It acts, more than any other EQ I've ever heard or made, like no EQ had been done, and like the sound was that way on recording.It's just that you can revoice things to a ridiculous degree and still have it sound like one coherent piece.This plugin will be very polarizing. You will likely love it or absolutely hate it. I suspect some of the other old-schoolers will like what they hear. I'm almost sure anybody with extensive modern-production expertise will be frustrated and confused. So very many people think in terms of taking all the sounds and decomposing them into distinct pieces and fitting them together like some mixing jigsaw puzzle. What are you going to do with an EQ that refuses to separate out anything, where you can never turn a sound into this band and that band and holes where the bad audio used to be?Meanwhile, the old school practitioners of mixing black arts are licking their chops and can't wait to turn a much smaller number of tracks into something huge and compelling… sneakily heightening rawness and energy, without ever seeming like they are EQing. It's more like voicing. Nothing needs to be aggressive, just voice everything until it hits right and sits in the right places, and the top-to-bottom, left-to-right, mixing jigsaw puzzle can stay in the box. With suitable use of compression and saturation you could use this to mix a whole thing in mono, sounding unequalized, and it'd still hit like a ton of bricks.BezEQ3 is currently on track to be the primary EQ in ConsoleX3. Many people will just get mad at it, most likely. I'm just gonna have to show people what this is about… but if something happens when you try this plugin, and it gets exactly the sound you crave and you want to use it on everything… well, you know who you are, and you're welcome :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class BezEQ3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    enum
    {
        bez_HMAL,
        bez_HMBL,
        bez_HMCL,
        bez_HMDL,
        bez_MAL,
        bez_MBL,
        bez_MCL,
        bez_MDL,
        bez_LMAL,
        bez_LMBL,
        bez_LMCL,
        bez_LMDL,
        bez_HMAR,
        bez_HMBR,
        bez_HMCR,
        bez_HMDR,
        bez_HMcycle,
        bez_MAR,
        bez_MBR,
        bez_MCR,
        bez_MDR,
        bez_Mcycle,
        bez_LMAR,
        bez_LMBR,
        bez_LMCR,
        bez_LMDR,
        bez_LMcycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezEQ[bez_total][3];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BezEQ3()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        for (int x = 0; x < bez_total; x++) {
            for (int y = 0; y < 3; y++) {
                bezEQ[x][y] = 0.0;
            }
        }
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
        kNumParameters = 8
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;
            case kParamH: return H; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "hmid"; break;
            case kParamC: return "lmid"; break;
            case kParamD: return "bass"; break;
            case kParamE: return "highf"; break;
            case kParamF: return "hmidf"; break;
            case kParamG: return "lmidf"; break;
            case kParamH: return "bassf"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "HMid"; break;
            case kParamC: return "LMid"; break;
            case kParamD: return "Bass"; break;
            case kParamE: return "HighF"; break;
            case kParamF: return "HMidF"; break;
            case kParamG: return "LMidF"; break;
            case kParamH: return "BassF"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return ""; break;

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
        // begin BezEQ3 control setup
        double highGain = (pow(A + 0.5, 3.0) + 3.0) * 0.25;
        double hmidGain = (pow(B + 0.5, 3.0) + 3.0) * 0.25;
        double lmidGain = (pow(C + 0.5, 3.0) + 3.0) * 0.25;
        double bassGain = (pow(D + 0.5, 3.0) + 3.0) * 0.25;
        double trebleRef = (1.0 - pow(1.0 - E, 2.0)) - 0.5;
        double highmidRef = F - 0.5;
        double lowmidRef = G - 0.5;
        double bassRef = pow(H, 2.0) - 0.5;
        double HMderez = pow(0.75 + ((trebleRef + trebleRef + trebleRef + highmidRef) * 0.125), 1.618) / overallscale;
        double LMderez = pow(0.25 + ((lowmidRef + bassRef + bassRef + bassRef) * 0.125), 1.618) / overallscale;
        double Mderez = pow((HMderez * 0.5) + (LMderez * 0.5) + ((highmidRef + lowmidRef) * 0.125), 1.618) / overallscale;
        int stepped = 999999;
        if (HMderez > 0.000001) {
            stepped = (int)(1.0 / HMderez);
        }
        HMderez = 1.0 / stepped;
        double HMtrim = 1.0 - (HMderez * ((double)stepped / (stepped + 1.0)));
        stepped = 999999;
        if (Mderez > 0.000001) {
            stepped = (int)(1.0 / Mderez);
        }
        Mderez = 1.0 / stepped;
        double Mtrim = 1.0 - (Mderez * ((double)stepped / (stepped + 1.0)));
        stepped = 999999;
        if (LMderez > 0.000001) {
            stepped = (int)(1.0 / LMderez);
        }
        LMderez = 1.0 / stepped;
        double LMtrim = 1.0 - (LMderez * ((double)stepped / (stepped + 1.0)));
        // end BezEQ3 control setup
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // begin BezEQ3
            for (int p = 0; p < 3; p++) {
                double highL = inputSampleL;
                double highR = inputSampleR;
                bezEQ[bez_HMcycle][p] += HMderez;
                bezEQ[bez_HMAL][p] += (highL * HMderez);
                bezEQ[bez_HMAR][p] += (highR * HMderez);
                if (bezEQ[bez_HMcycle][p] > 1.0) {
                    bezEQ[bez_HMDL][p] = bezEQ[bez_HMCL][p];
                    bezEQ[bez_HMCL][p] = bezEQ[bez_HMBL][p];
                    bezEQ[bez_HMBL][p] = bezEQ[bez_HMAL][p] * (0.5 - (HMderez * 0.082));
                    bezEQ[bez_HMAL][p] = 0.0;
                    bezEQ[bez_HMcycle][p] = 0.0;
                    bezEQ[bez_HMDR][p] = bezEQ[bez_HMCR][p];
                    bezEQ[bez_HMCR][p] = bezEQ[bez_HMBR][p];
                    bezEQ[bez_HMBR][p] = bezEQ[bez_HMAR][p] * (0.5 - (HMderez * 0.082));
                    bezEQ[bez_HMAR][p] = 0.0;
                    bezEQ[bez_HMcycle][p] = 0.0;
                }
                double X = bezEQ[bez_HMcycle][p] * HMtrim;
                double hmidL = bezEQ[bez_HMCL][p] + (bezEQ[bez_HMDL][p] * (1.0 - X) * (1.0 - X));
                hmidL += (bezEQ[bez_HMCL][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBL][p] * X * X);
                highL -= hmidL; // final high and hmid
                double hmidR = bezEQ[bez_HMCR][p] + (bezEQ[bez_HMDR][p] * (1.0 - X) * (1.0 - X));
                hmidR += (bezEQ[bez_HMCR][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_HMBR][p] * X * X);
                highR -= hmidR; // final high and hmid
                bezEQ[bez_Mcycle][p] += Mderez;
                bezEQ[bez_MAL][p] += (hmidL * Mderez);
                bezEQ[bez_MAR][p] += (hmidR * Mderez);
                if (bezEQ[bez_Mcycle][p] > 1.0) {
                    bezEQ[bez_MDL][p] = bezEQ[bez_MCL][p];
                    bezEQ[bez_MCL][p] = bezEQ[bez_MBL][p];
                    bezEQ[bez_MBL][p] = bezEQ[bez_MAL][p] * (0.5 - (Mderez * 0.082));
                    bezEQ[bez_MAL][p] = 0.0;
                    bezEQ[bez_Mcycle][p] = 0.0;
                    bezEQ[bez_MDR][p] = bezEQ[bez_MCR][p];
                    bezEQ[bez_MCR][p] = bezEQ[bez_MBR][p];
                    bezEQ[bez_MBR][p] = bezEQ[bez_MAR][p] * (0.5 - (Mderez * 0.082));
                    bezEQ[bez_MAR][p] = 0.0;
                    bezEQ[bez_Mcycle][p] = 0.0;
                }
                X = bezEQ[bez_Mcycle][p] * Mtrim;
                double lmidL = bezEQ[bez_MCL][p] + (bezEQ[bez_MDL][p] * (1.0 - X) * (1.0 - X));
                lmidL += (bezEQ[bez_MCL][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_MBL][p] * X * X);
                hmidL -= lmidL; // final hmid and lmid
                double lmidR = bezEQ[bez_MCR][p] + (bezEQ[bez_MDR][p] * (1.0 - X) * (1.0 - X));
                lmidR += (bezEQ[bez_MCR][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_MBR][p] * X * X);
                hmidR -= lmidR; // final hmid and lmid
                bezEQ[bez_LMcycle][p] += LMderez;
                bezEQ[bez_LMAL][p] += (lmidL * LMderez);
                bezEQ[bez_LMAR][p] += (lmidR * LMderez);
                if (bezEQ[bez_LMcycle][p] > 1.0) {
                    bezEQ[bez_LMDL][p] = bezEQ[bez_LMCL][p];
                    bezEQ[bez_LMCL][p] = bezEQ[bez_LMBL][p];
                    bezEQ[bez_LMBL][p] = bezEQ[bez_LMAL][p] * (0.5 - (LMderez * 0.082));
                    bezEQ[bez_LMAL][p] = 0.0;
                    bezEQ[bez_LMcycle][p] = 0.0;
                    bezEQ[bez_LMDR][p] = bezEQ[bez_LMCR][p];
                    bezEQ[bez_LMCR][p] = bezEQ[bez_LMBR][p];
                    bezEQ[bez_LMBR][p] = bezEQ[bez_LMAR][p] * (0.5 - (LMderez * 0.082));
                    bezEQ[bez_LMAR][p] = 0.0;
                    bezEQ[bez_LMcycle][p] = 0.0;
                }
                X = bezEQ[bez_LMcycle][p] * LMtrim;
                double bassL = bezEQ[bez_LMCL][p] + (bezEQ[bez_LMDL][p] * (1.0 - X) * (1.0 - X));
                bassL += (bezEQ[bez_LMCL][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBL][p] * X * X);
                lmidL -= bassL; // final lmid and bass
                double bassR = bezEQ[bez_LMCR][p] + (bezEQ[bez_LMDR][p] * (1.0 - X) * (1.0 - X));
                bassR += (bezEQ[bez_LMCR][p] * 2.0 * (1.0 - X) * X) + (bezEQ[bez_LMBR][p] * X * X);
                lmidR -= bassR; // final lmid and bass
                inputSampleL = (highL * highGain) + (hmidL * hmidGain) + (lmidL * lmidGain) + (bassL * bassGain);
                inputSampleR = (highR * highGain) + (hmidR * hmidGain) + (lmidR * lmidGain) + (bassR * bassGain);
            }
            // end BezEQ3
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
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::bezeq3
