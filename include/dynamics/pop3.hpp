#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pop3 {

constexpr std::string_view k_name{ "Pop3" };
constexpr std::string_view k_short_description{
    "Pop3 is the dynamics from ConsoleX."
};
constexpr std::string_view k_long_description{
    "As I work on all the things that go into ConsoleX, now and then it becomes useful to give you a little preview. Pop3 is like that. It's the latest Airwindows compressor, but also gate, this time.I hope it's pretty simple to understand, because in ConsoleX you get two of them per channel/buss, one for the Fire band and one for the Stone band. (if needed, I'll put out a StoneFire compressor to help you practice that). In this case, it's a fullrange dynamics processor tailored to my needs with ConsoleX.For the compression section, Threshold brings down the squish. It doesn't use makeup gain, so you use it to restrain the sound. Attack and Release work as you'd expect, though Pop3 runs a new sense circuit that's very transparent in action. The Ratio control is simply a dry/wet: if you have signals peaking really hot, over 0dB, you'll have to set Ratio to 0 to truly bypass the compression. Since this'll be available in multiple places on multiple bands it's designed to be more a 'glue' type, though you can run audio really hot into it in order to get an exaggerated result. In the final version you use the Stone and Fire controls to boost post-compression if you want.The gate section had to be designed into Pop3, like it is in ConsoleX. It triggers off the uncompressed signal, so whatever you do with compression has no effect. It uses its own threshold (coming up from zero) with a release that goes from slow to brutally fast, and a handy Sustain control which stretches out the gating to control whether it's sputtery or crisp. This gate is good at tightening things up, especially with the sustain feature there as a primary control, and again: works totally independently of the compressor, so you can sculpt those dynamics (or put percussive bumps on things using the gate ratio to dial in the spike and then blend it with the regular signal) and then shape your primary audio to make it more squishy. I found that just kicking in the compressor slightly gives a huge change in texture compared to only using the gate.I wanted it to be extremely approachable because you deal with two of them in parallel in ConsoleX, and not even with a normal crossover, instead with multiple Kalman filters and a set of parametrics that bypass all the compression (but which are in fact gated, but the different parametrics and the Air band respond to different gates).The glimpse of ConsoleX interface is only a rough draft: in particular, I gotta add user control over whether that 3D effect is on the knobs at top and bottom. Some people will prefer it flat, some will like the extra room provided by some of the knobs being seen edge-on. Also I have a lot of work to do on the dynamic positioning and reflowing of controls, perhaps with some divider lines to help highlight which knobs went where. Suffice to say, all my days and all my attention are focussed on this now, and I hope it comes out (right now it's not running properly in my DAW Reaper, so there's bugfixing too)Talk to ya later, and I hope you enjoy the chance to come to grips with Pop3."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pop3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    double popCompL;
    double popCompR;
    double popGate;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Pop3()
    {
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.0;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        popCompL = 1.0;
        popCompR = 1.0;
        popGate = 1.0;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.0; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "thresld"; break;
            case kParamB: return "c ratio"; break;
            case kParamC: return "c atk"; break;
            case kParamD: return "c rls"; break;
            case kParamE: return "thresld"; break;
            case kParamF: return "g ratio"; break;
            case kParamG: return "g sust"; break;
            case kParamH: return "g rls"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thresld"; break;
            case kParamB: return "C Ratio"; break;
            case kParamC: return "C Atk"; break;
            case kParamD: return "C Rls"; break;
            case kParamE: return "Thresld"; break;
            case kParamF: return "G Ratio"; break;
            case kParamG: return "G Sust"; break;
            case kParamH: return "G Rls"; break;

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
        double compThresh = pow(A, 4);
        double compRatio = 1.0 - pow(1.0 - B, 2);
        double compAttack = 1.0 / (((pow(C, 3) * 5000.0) + 500.0) * overallscale);
        double compRelease = 1.0 / (((pow(D, 5) * 50000.0) + 500.0) * overallscale);
        double gateThresh = pow(E, 4);
        double gateRatio = 1.0 - pow(1.0 - F, 2);
        double gateSustain = M_PI_2 * pow(G + 1.0, 4.0);
        double gateRelease = 1.0 / (((pow(H, 5) * 500000.0) + 500.0) * overallscale);
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
            if (fabs(inputSampleL) > compThresh) { // compression L
                popCompL -= (popCompL * compAttack);
                popCompL += ((compThresh / fabs(inputSampleL)) * compAttack);
            }
            else {
                popCompL = (popCompL * (1.0 - compRelease)) + compRelease;
            }
            if (fabs(inputSampleR) > compThresh) { // compression R
                popCompR -= (popCompR * compAttack);
                popCompR += ((compThresh / fabs(inputSampleR)) * compAttack);
            }
            else {
                popCompR = (popCompR * (1.0 - compRelease)) + compRelease;
            }
            if (popCompL > popCompR) {
                popCompL -= (popCompL * compAttack);
            }
            if (popCompR > popCompL) {
                popCompR -= (popCompR * compAttack);
            }
            if (fabs(inputSampleL) > gateThresh) {
                popGate = gateSustain;
            }
            else if (fabs(inputSampleR) > gateThresh) {
                popGate = gateSustain;
            }
            else {
                popGate *= (1.0 - gateRelease);
            }
            if (popGate < 0.0) {
                popGate = 0.0;
            }
            popCompL = fmax(fmin(popCompL, 1.0), 0.0);
            popCompR = fmax(fmin(popCompR, 1.0), 0.0);
            inputSampleL *= ((1.0 - compRatio) + (popCompL * compRatio));
            inputSampleR *= ((1.0 - compRatio) + (popCompR * compRatio));
            if (popGate < M_PI_2) {
                inputSampleL *= ((1.0 - gateRatio) + (sin(popGate) * gateRatio));
                inputSampleR *= ((1.0 - gateRatio) + (sin(popGate) * gateRatio));
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
} // namespace airwindohhs::pop3
