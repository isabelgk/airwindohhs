#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::biquadhilo {

constexpr std::string_view k_name{ "BiquadHiLo" };
constexpr std::string_view k_short_description{
    "BiquadHiLo is the highpass and lowpass filter in ConsoleX."
};
constexpr std::string_view k_long_description{
    "Pretty straightforward: this is effectively the same thing as the highpass and lowpass used in ConsoleX. Except, in practice it's absolutely not, because in ConsoleX both of these are distributed filters. That means as signal hits the Stonefire section with dynamics, it will have hit some of the highpassing and lowpassing, but not all of it: some of it will happen after the dynamics. Some of it will happen after the four-band parametric EQ, which is somewhat nonlinear. The lowpass in particular gets to work as a distributed filter against aliasing, especially if you're running at high sample rate.But here it's just those filters as a one-piece unit.That also means you can use it as those filters, but in a much more lightweight form than in ConsoleX. I'm hoping ConsoleX is working out for people (it will be a while before I'm finished explaining all that, and getting it working on everybody's DAW, if that's even possible!). But though it is prettier and a lot fancier, it's way more complicated.In the video I made, I demonstrate how you might be running something like a guitar into virtual tape (ToTape8 in this case) and from there into ConsoleX. But there are some things you simply can't do when processing the sound AFTER the tape. Sometimes there's a reason to shape the sound going in front of the tape, so it can hit those harmonics harder with less extra frequencies flying around… and BiquadHiLo can work for 'trapping in' a sound like that so it can hit tape even harder and produce a really direct, clear sound.And of course you can use ConsoleXPre for exactly that purpose and have all the EQs or even dynamics going, both in front of and after the tape, but much like you have access to three bands of the parametric EQ in 'Parametric', and have the dynamics in 'StoneFireComp', you have the additional filters in BiquadHiLo.If all goes well I can have the 'mastering' (a very airwindowsized take on mastering) plugin by next week, but while I work on more fixes for ConsoleX, here's a spare filter to have :)"
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class BiquadHiLo final : public Effect<T>
{
    float A;
    float B;
    enum
    {
        hilp_freq,
        hilp_temp,
        hilp_a0,
        hilp_a1,
        hilp_b1,
        hilp_b2,
        hilp_c0,
        hilp_c1,
        hilp_d1,
        hilp_d2,
        hilp_e0,
        hilp_e1,
        hilp_f1,
        hilp_f2,
        hilp_aL1,
        hilp_aL2,
        hilp_aR1,
        hilp_aR2,
        hilp_cL1,
        hilp_cL2,
        hilp_cR1,
        hilp_cR2,
        hilp_eL1,
        hilp_eL2,
        hilp_eR1,
        hilp_eR2,
        hilp_total
    };
    double highpass[hilp_total];
    double lowpass[hilp_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BiquadHiLo()
    {
        A = 0.0;
        B = 0.0;
        for (int x = 0; x < hilp_total; x++) {
            highpass[x] = 0.0;
            lowpass[x] = 0.0;
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
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "highpas"; break;
            case kParamB: return "lowpass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Highpas"; break;
            case kParamB: return "Lowpass"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        highpass[hilp_freq] = ((A * 330.0) + 20.0) / Effect<T>::getSampleRate();
        bool highpassEngage = true;
        if (A == 0.0) {
            highpassEngage = false;
        }
        lowpass[hilp_freq] = ((pow(1.0 - B, 2) * 17000.0) + 3000.0) / Effect<T>::getSampleRate();
        bool lowpassEngage = true;
        if (B == 0.0) {
            lowpassEngage = false;
        }
        double K = tan(M_PI * highpass[hilp_freq]); // highpass
        double norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
        highpass[hilp_a0] = norm;
        highpass[hilp_a1] = -2.0 * highpass[hilp_a0];
        highpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
        highpass[hilp_c0] = norm;
        highpass[hilp_c1] = -2.0 * highpass[hilp_c0];
        highpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
        highpass[hilp_e0] = norm;
        highpass[hilp_e1] = -2.0 * highpass[hilp_e0];
        highpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
        K = tan(M_PI * lowpass[hilp_freq]); // lowpass
        norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
        lowpass[hilp_a0] = K * K * norm;
        lowpass[hilp_a1] = 2.0 * lowpass[hilp_a0];
        lowpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
        lowpass[hilp_c0] = K * K * norm;
        lowpass[hilp_c1] = 2.0 * lowpass[hilp_c0];
        lowpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
        lowpass[hilp_e0] = K * K * norm;
        lowpass[hilp_e1] = 2.0 * lowpass[hilp_e0];
        lowpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (highpassEngage) { // begin Stacked Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_a0]) + highpass[hilp_aL1];
                highpass[hilp_aL1] = (inputSampleL * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aL2];
                highpass[hilp_aL2] = (inputSampleL * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_c0]) + highpass[hilp_cL1];
                highpass[hilp_cL1] = (inputSampleL * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cL2];
                highpass[hilp_cL2] = (inputSampleL * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_e0]) + highpass[hilp_eL1];
                highpass[hilp_eL1] = (inputSampleL * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eL2];
                highpass[hilp_eL2] = (inputSampleL * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_a0]) + highpass[hilp_aR1];
                highpass[hilp_aR1] = (inputSampleR * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aR2];
                highpass[hilp_aR2] = (inputSampleR * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleR = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_c0]) + highpass[hilp_cR1];
                highpass[hilp_cR1] = (inputSampleR * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cR2];
                highpass[hilp_cR2] = (inputSampleR * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleR = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_e0]) + highpass[hilp_eR1];
                highpass[hilp_eR1] = (inputSampleR * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eR2];
                highpass[hilp_eR2] = (inputSampleR * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_aL1] = highpass[hilp_aL2] = highpass[hilp_aR1] = highpass[hilp_aR2] = 0.0;
            } // end Stacked Highpass
            // rest of control strip goes here
            if (lowpassEngage) { // begin Stacked Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_a0]) + lowpass[hilp_aL1];
                lowpass[hilp_aL1] = (inputSampleL * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aL2];
                lowpass[hilp_aL2] = (inputSampleL * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_c0]) + lowpass[hilp_cL1];
                lowpass[hilp_cL1] = (inputSampleL * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cL2];
                lowpass[hilp_cL2] = (inputSampleL * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_e0]) + lowpass[hilp_eL1];
                lowpass[hilp_eL1] = (inputSampleL * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eL2];
                lowpass[hilp_eL2] = (inputSampleL * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_a0]) + lowpass[hilp_aR1];
                lowpass[hilp_aR1] = (inputSampleR * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aR2];
                lowpass[hilp_aR2] = (inputSampleR * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleR = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_c0]) + lowpass[hilp_cR1];
                lowpass[hilp_cR1] = (inputSampleR * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cR2];
                lowpass[hilp_cR2] = (inputSampleR * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleR = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_e0]) + lowpass[hilp_eR1];
                lowpass[hilp_eR1] = (inputSampleR * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eR2];
                lowpass[hilp_eR2] = (inputSampleR * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_aL1] = lowpass[hilp_aL2] = lowpass[hilp_aR1] = lowpass[hilp_aR2] = 0.0;
            } // end Stacked Lowpass
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
} // namespace airwindohhs::biquadhilo
