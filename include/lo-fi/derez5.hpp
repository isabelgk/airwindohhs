#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::derez5 {

constexpr std::string_view k_name{ "DeRez5" };
constexpr std::string_view k_short_description{
    "DeRez5 adds new forms of derezzing."
};
constexpr std::string_view k_long_description{
    "Interestingly, the last time I worked on DeRez I was also putting a lot of time into Meter. Maybe I turn to imaginary digital noises when struggling to unearth the secrets of the great analog noises. DeRez5 builds on everything DeRez4 can do: in fact if you set Square to 0 and Trim to 1, you have DeRez4 again, complete with the frequency crush, linear point-to-point interpolation, and Bezier filters to turn any corners into neat little rounded places without altering the straight parts of the lines. That is of course weird and has nothing to do with musical or harmonic behavior, but we're doing something different so it doesn't matter :) Add the Square control, and now we can go from the original point-to-point undersampling behavior, to full zero-order-hold behavior: meaning, squarewaves! The control progressively untilts the angles of the point to point waves until they're fully square in all their cornery glory. You can still use the Bright control to add neat little round parts to where the corners are. The Trim control sounds innocent, but it's anything but. If you're heavily derezzed, and you increase Trim, it'll sound like you're raising the DeRez control back up again… kinda. Except it's grungier, weirder, less predictable, and it stops imposing a clashing note onto the sound, because the frequency's less regular. What it is doing is allowing angle-reversals in the underlying sound to determine the derez frequency… so if you crank it, you can definitely get more highs in there and save deeper crunchiness for when bass is happening, but it goes beyond that. You can sneak Trim up just enough to interfere with the steadiness of the underlying derez frequency, producing an altered sound with a little more 'mulch' to it but without dragging in as many highs. You can crank it and see at what point DeRez starts to step on the real highs. You can shape all that with Bright and Bassis, and heighten the edge with Square. And you should tune all this to the sounds you're working with, because more than any other DeRez, the whole sound of this plugin interacts with the sound that's, in theory, being destroyed. Hope you enjoy melding this weird thing with your tracks! :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeRez5 final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'drz5';
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
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
    double pointCycleL, pointAL, pointBL, sDerezL;
    double pointCycleR, pointAR, pointBR, sDerezR;
    double lastSampleL, directionCountL;
    double lastSampleR, directionCountR;
    bool goingUpL, goingUpR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeRez5()
    {
        A = 1.0;
        B = 1.0;
        C = 0.0;
        D = 1.0;
        E = 0.0;
        F = 1.0;
        for (int x = 0; x < bez_total; x++) {
            for (int y = 0; y < 2; y++) {
                bezEQ[x][y] = 0.0;
            }
        }
        pointCycleL = pointAL = pointBL = 0.0;
        sDerezL = 1.0;
        pointCycleR = pointAR = pointBR = 0.0;
        sDerezR = 1.0;
        lastSampleL = 0.0;
        directionCountL = 1.0;
        goingUpL = false;
        lastSampleR = 0.0;
        directionCountR = 1.0;
        goingUpR = false;
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
        switch (index) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            default: break; // unknown parameter, shouldn't happen!
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;
            case kParamB: return 1.0; break;
            case kParamC: return 0.0; break;
            case kParamD: return 1.0; break;
            case kParamE: return 0.0; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "derez"; break;
            case kParamB: return "trim"; break;
            case kParamC: return "square"; break;
            case kParamD: return "bright"; break;
            case kParamE: return "bassis"; break;
            case kParamF: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Derez"; break;
            case kParamB: return "Trim"; break;
            case kParamC: return "Square"; break;
            case kParamD: return "Bright"; break;
            case kParamE: return "Bassis"; break;
            case kParamF: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(A, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(B, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(C, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(D, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(E, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(F, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
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
        double applyDerez = 1.0 - pow(A, 2.0 + overallscale);
        double trimDerez = (1.0 - B) * 16.0 * overallscale;
        double square = 1.0 - pow(C, 3.0);
        double brightBez = (1.0 - pow(D, 2.0)) * 4.0 * overallscale;
        int stepped = 999999;
        if (brightBez > 1.0) {
            stepped = (int)brightBez;
            brightBez = 0.99999999 / stepped;
        }
        else {
            brightBez = 1.1;
        }
        double brightTrim = 1.0 - (brightBez * ((double)stepped / (stepped + 1.0)));
        double bassRef = pow(E, 4.0);
        double bassisBez = (bassRef * 0.25) / overallscale;
        stepped = 999999;
        if (bassisBez > 0.000001) {
            stepped = (int)(1.0 / bassisBez);
        }
        if (bassisBez > 0.0) {
            bassisBez = 0.99999999 / stepped;
        }
        double bassisTrim = 1.0 - (bassisBez * ((double)stepped / (stepped + 1.0)));
        double output = F;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (inputSampleL > lastSampleL == goingUpL) {
                directionCountL += (applyDerez + trimDerez);
            }
            else {
                directionCountL = trimDerez + 1.0;
            }
            goingUpL = (inputSampleL > lastSampleL);
            lastSampleL = inputSampleL;
            if (inputSampleR > lastSampleR == goingUpR) {
                directionCountR += (applyDerez + trimDerez);
            }
            else {
                directionCountR = trimDerez + 1.0;
            }
            goingUpR = (inputSampleR > lastSampleR);
            lastSampleR = inputSampleR;
            if (brightBez < 1.0) {
                bezEQ[bez_HMcycle][0] += brightBez;
                bezEQ[bez_HMAL][0] += (inputSampleL * 0.5 * brightBez);
                bezEQ[bez_HMAR][0] += (inputSampleR * 0.5 * brightBez);
                if (bezEQ[bez_HMcycle][0] > brightTrim) {
                    bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0];
                    bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0];
                    bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0] * (0.5 - (brightBez * 0.082));
                    bezEQ[bez_HMAL][0] = 0.0;
                    bezEQ[bez_HMDR][0] = bezEQ[bez_HMCR][0];
                    bezEQ[bez_HMCR][0] = bezEQ[bez_HMBR][0];
                    bezEQ[bez_HMBR][0] = bezEQ[bez_HMAR][0] * (0.5 - (brightBez * 0.082));
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
            if (bassisBez > 0.0) {
                bezEQ[bez_LMcycle][0] += bassisBez;
                bezEQ[bez_LMAL][0] += (inputSampleL * bassisBez);
                bezEQ[bez_LMAR][0] += (inputSampleR * bassisBez);
                if (bezEQ[bez_LMcycle][0] > bassisTrim) {
                    bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0];
                    bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0];
                    bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0] * (0.5 - (bassisBez * 0.082));
                    bezEQ[bez_LMAL][0] = 0.0;
                    bezEQ[bez_LMDR][0] = bezEQ[bez_LMCR][0];
                    bezEQ[bez_LMCR][0] = bezEQ[bez_LMBR][0];
                    bezEQ[bez_LMBR][0] = bezEQ[bez_LMAR][0] * (0.5 - (bassisBez * 0.082));
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
            if (applyDerez + trimDerez > 0.0) {
                pointCycleL += sDerezL;
                if (pointCycleL > 0.9999999) {
                    pointAL = pointBL;
                    pointBL = inputSampleL;
                    pointCycleL = 0.0; // waveform point values
                    stepped = 999999;
                    if (applyDerez < 0.999999) {
                        stepped = (int)(1.0 / (1.0 - applyDerez));
                    }
                    if (stepped > directionCountL) {
                        stepped = directionCountL;
                    }
                    if (stepped < 1) {
                        stepped = 1;
                    }
                    sDerezL = 0.99999999 / stepped;
                }
                double X = pointCycleL * square;
                if (sDerezL > 0.0) {
                    inputSampleL = pointAL + ((pointBL - pointAL) * X);
                }
                else {
                    inputSampleL = 0.0;
                }
                pointCycleR += sDerezR;
                if (pointCycleR > 0.9999999) {
                    pointAR = pointBR;
                    pointBR = inputSampleR;
                    pointCycleR = 0.0; // waveform point values
                    stepped = 999999;
                    if (applyDerez < 0.999999) {
                        stepped = (int)(1.0 / (1.0 - applyDerez));
                    }
                    if (stepped > directionCountR) {
                        stepped = directionCountR;
                    }
                    if (stepped < 1) {
                        stepped = 1;
                    }
                    sDerezR = 0.99999999 / stepped;
                }
                X = pointCycleR * square;
                if (sDerezR > 0.0) {
                    inputSampleR = pointAR + ((pointBR - pointAR) * X);
                }
                else {
                    inputSampleR = 0.0;
                }
            }
            if (brightBez < 1.0) {
                bezEQ[bez_HMcycle][1] += brightBez;
                bezEQ[bez_HMAL][1] += (inputSampleL * brightBez);
                bezEQ[bez_HMAR][1] += (inputSampleR * brightBez);
                if (bezEQ[bez_HMcycle][1] > brightTrim) {
                    bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1];
                    bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1];
                    bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1] * (0.5 - (brightBez * 0.082));
                    bezEQ[bez_HMAL][1] = 0.0;
                    bezEQ[bez_HMDR][1] = bezEQ[bez_HMCR][1];
                    bezEQ[bez_HMCR][1] = bezEQ[bez_HMBR][1];
                    bezEQ[bez_HMBR][1] = bezEQ[bez_HMAR][1] * (0.5 - (brightBez * 0.082));
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
            if (bassisBez > 0.0) {
                bezEQ[bez_LMcycle][1] += bassisBez;
                bezEQ[bez_LMAL][1] += (inputSampleL * bassisBez);
                bezEQ[bez_LMAR][1] += (inputSampleR * bassisBez);
                if (bezEQ[bez_LMcycle][1] > bassisTrim) {
                    bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1];
                    bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1];
                    bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1] * (0.5 - (bassisBez * 0.082));
                    bezEQ[bez_LMAL][1] = 0.0;
                    bezEQ[bez_LMDR][1] = bezEQ[bez_LMCR][1];
                    bezEQ[bez_LMCR][1] = bezEQ[bez_LMBR][1];
                    bezEQ[bez_LMBR][1] = bezEQ[bez_LMAR][1] * (0.5 - (bassisBez * 0.082));
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
} // namespace airwindohhs::derez5
