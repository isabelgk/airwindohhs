#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::parametric {

constexpr std::string_view k_name{ "Parametric" };
constexpr std::string_view k_short_description{
    "Parametric is three bands of ConsoleX EQ in advance."
};
constexpr std::string_view k_long_description{
    "Parametric is basically 3/4 of the EQ section of ConsoleX, except that it's half of the EQ section because ConsoleX has a dedicated highpass and lowpass per channel (and a special one for the buss). Except that it's one third of the EQ section, because ConsoleX also has Stonefire per channel (and on the buss), but you actually already have Stonefire! So you can run that into this and begin to get a handle on what ConsoleX will allow.Except you actually won't, because in ConsoleX proper, Stonefire is also a multiband compressor/gate (an extension of what you get in big SSL consoles) in which Parametric (like this, but with a dedicated bass filter) runs parallel around the Stonefire and dynamics. So in that, everything in Parametric will be used as ways of bringing energy and power AROUND the dynamics so the sound opens up way more than you'd get in a real SSL, and then you apply Discontinuity (which you also already have now) to set the overall loudness cues.I promise I will explain all this when it's done. It seems I've been working real hard on all this and a lot happens and I'm sure it's a lot to keep up with. Them's the risks when you're trying to not imitate, but outdo the classics. For the time being, Parametric is roughly SSL-style EQ for very detailed tone shaping, in three bands designed to be recognizable to SSL fans except the Low Mid extends into the bass (so I could make sure Parametric works in Airwindows Consolidated, and in the VCV Rack version). The sound is Airwindows-style and I hope it's useful, but the ranges and resonances of the filters are designed to act something like a big SSL console so if you know to grab for the High Mid control and tighten the bandwidth by turning it left, this acts the same way without 'emulating' someone else's property."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Parametric final : public Effect<T>
{
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
    enum
    {
        biqs_freq,
        biqs_reso,
        biqs_level,
        biqs_nonlin,
        biqs_temp,
        biqs_dis,
        biqs_a0,
        biqs_a1,
        biqs_b1,
        biqs_b2,
        biqs_c0,
        biqs_c1,
        biqs_d1,
        biqs_d2,
        biqs_e0,
        biqs_e1,
        biqs_f1,
        biqs_f2,
        biqs_aL1,
        biqs_aL2,
        biqs_aR1,
        biqs_aR2,
        biqs_cL1,
        biqs_cL2,
        biqs_cR1,
        biqs_cR2,
        biqs_eL1,
        biqs_eL2,
        biqs_eR1,
        biqs_eR2,
        biqs_outL,
        biqs_outR,
        biqs_total
    };
    double high[biqs_total];
    double hmid[biqs_total];
    double lmid[biqs_total];

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Parametric()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 1.0;
        for (int x = 0; x < biqs_total; x++) {
            high[x] = 0.0;
            hmid[x] = 0.0;
            lmid[x] = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
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
            case kParamA: return "tr freq"; break;
            case kParamB: return "treble"; break;
            case kParamC: return "tr reso"; break;
            case kParamD: return "hm freq"; break;
            case kParamE: return "highmid"; break;
            case kParamF: return "hm reso"; break;
            case kParamG: return "lm freq"; break;
            case kParamH: return "lowmid"; break;
            case kParamI: return "lm reso"; break;
            case kParamJ: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Tr Freq"; break;
            case kParamB: return "Treble"; break;
            case kParamC: return "Tr Reso"; break;
            case kParamD: return "HM Freq"; break;
            case kParamE: return "HighMid"; break;
            case kParamF: return "HM Reso"; break;
            case kParamG: return "LM Freq"; break;
            case kParamH: return "LowMid"; break;
            case kParamI: return "LM Reso"; break;
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
        high[biqs_freq] = (((pow(A, 3) * 14500.0) + 1500.0) / Effect<T>::getSampleRate());
        if (high[biqs_freq] < 0.0001) {
            high[biqs_freq] = 0.0001;
        }
        high[biqs_nonlin] = B;
        high[biqs_level] = (high[biqs_nonlin] * 2.0) - 1.0;
        if (high[biqs_level] > 0.0) {
            high[biqs_level] *= 2.0;
        }
        high[biqs_reso] = ((0.5 + (high[biqs_nonlin] * 0.5) + sqrt(high[biqs_freq])) - (1.0 - pow(1.0 - C, 2.0))) + 0.5 + (high[biqs_nonlin] * 0.5);
        double K = tan(M_PI * high[biqs_freq]);
        double norm = 1.0 / (1.0 + K / (high[biqs_reso] * 1.93185165) + K * K);
        high[biqs_a0] = K / (high[biqs_reso] * 1.93185165) * norm;
        high[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_b2] = (1.0 - K / (high[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 0.70710678) + K * K);
        high[biqs_c0] = K / (high[biqs_reso] * 0.70710678) * norm;
        high[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_d2] = (1.0 - K / (high[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 0.51763809) + K * K);
        high[biqs_e0] = K / (high[biqs_reso] * 0.51763809) * norm;
        high[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_f2] = (1.0 - K / (high[biqs_reso] * 0.51763809) + K * K) * norm;
        // high
        hmid[biqs_freq] = (((pow(D, 3) * 6400.0) + 600.0) / Effect<T>::getSampleRate());
        if (hmid[biqs_freq] < 0.0001) {
            hmid[biqs_freq] = 0.0001;
        }
        hmid[biqs_nonlin] = E;
        hmid[biqs_level] = (hmid[biqs_nonlin] * 2.0) - 1.0;
        if (hmid[biqs_level] > 0.0) {
            hmid[biqs_level] *= 2.0;
        }
        hmid[biqs_reso] = ((0.5 + (hmid[biqs_nonlin] * 0.5) + sqrt(hmid[biqs_freq])) - (1.0 - pow(1.0 - F, 2.0))) + 0.5 + (hmid[biqs_nonlin] * 0.5);
        K = tan(M_PI * hmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 1.93185165) + K * K);
        hmid[biqs_a0] = K / (hmid[biqs_reso] * 1.93185165) * norm;
        hmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_b2] = (1.0 - K / (hmid[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 0.70710678) + K * K);
        hmid[biqs_c0] = K / (hmid[biqs_reso] * 0.70710678) * norm;
        hmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_d2] = (1.0 - K / (hmid[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 0.51763809) + K * K);
        hmid[biqs_e0] = K / (hmid[biqs_reso] * 0.51763809) * norm;
        hmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_f2] = (1.0 - K / (hmid[biqs_reso] * 0.51763809) + K * K) * norm;
        // hmid
        lmid[biqs_freq] = (((pow(G, 3) * 2200.0) + 20.0) / Effect<T>::getSampleRate());
        if (lmid[biqs_freq] < 0.00001) {
            lmid[biqs_freq] = 0.00001;
        }
        lmid[biqs_nonlin] = H;
        lmid[biqs_level] = (lmid[biqs_nonlin] * 2.0) - 1.0;
        if (lmid[biqs_level] > 0.0) {
            lmid[biqs_level] *= 2.0;
        }
        lmid[biqs_reso] = ((0.5 + (lmid[biqs_nonlin] * 0.5) + sqrt(lmid[biqs_freq])) - (1.0 - pow(1.0 - I, 2.0))) + 0.5 + (lmid[biqs_nonlin] * 0.5);
        K = tan(M_PI * lmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 1.93185165) + K * K);
        lmid[biqs_a0] = K / (lmid[biqs_reso] * 1.93185165) * norm;
        lmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_b2] = (1.0 - K / (lmid[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 0.70710678) + K * K);
        lmid[biqs_c0] = K / (lmid[biqs_reso] * 0.70710678) * norm;
        lmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_d2] = (1.0 - K / (lmid[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 0.51763809) + K * K);
        lmid[biqs_e0] = K / (lmid[biqs_reso] * 0.51763809) * norm;
        lmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_f2] = (1.0 - K / (lmid[biqs_reso] * 0.51763809) + K * K) * norm;
        // lmid
        double wet = J;
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
            // begin Stacked Biquad With Reversed Neutron Flow L
            high[biqs_outL] = inputSampleL * fabs(high[biqs_level]);
            high[biqs_dis] = fabs(high[biqs_a0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_aL1];
            high[biqs_aL1] = high[biqs_aL2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_c0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_cL1];
            high[biqs_cL1] = high[biqs_cL2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_e0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_eL1];
            high[biqs_eL1] = high[biqs_eL2] - (high[biqs_temp] * high[biqs_f1]);
            high[biqs_eL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_f2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_outL] *= high[biqs_level];
            if (high[biqs_level] > 1.0) {
                high[biqs_outL] *= high[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            hmid[biqs_outL] = inputSampleL * fabs(hmid[biqs_level]);
            hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_aL1];
            hmid[biqs_aL1] = hmid[biqs_aL2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_cL1];
            hmid[biqs_cL1] = hmid[biqs_cL2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_eL1];
            hmid[biqs_eL1] = hmid[biqs_eL2] - (hmid[biqs_temp] * hmid[biqs_f1]);
            hmid[biqs_eL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_f2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_outL] *= hmid[biqs_level];
            if (hmid[biqs_level] > 1.0) {
                hmid[biqs_outL] *= hmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            lmid[biqs_outL] = inputSampleL * fabs(lmid[biqs_level]);
            lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_aL1];
            lmid[biqs_aL1] = lmid[biqs_aL2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_cL1];
            lmid[biqs_cL1] = lmid[biqs_cL2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_eL1];
            lmid[biqs_eL1] = lmid[biqs_eL2] - (lmid[biqs_temp] * lmid[biqs_f1]);
            lmid[biqs_eL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_f2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_outL] *= lmid[biqs_level];
            if (lmid[biqs_level] > 1.0) {
                lmid[biqs_outL] *= lmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow R
            high[biqs_outR] = inputSampleR * fabs(high[biqs_level]);
            high[biqs_dis] = fabs(high[biqs_a0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_aR1];
            high[biqs_aR1] = high[biqs_aR2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_c0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_cR1];
            high[biqs_cR1] = high[biqs_cR2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_e0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_eR1];
            high[biqs_eR1] = high[biqs_eR2] - (high[biqs_temp] * high[biqs_f1]);
            high[biqs_eR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_f2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_outR] *= high[biqs_level];
            if (high[biqs_level] > 1.0) {
                high[biqs_outR] *= high[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            hmid[biqs_outR] = inputSampleR * fabs(hmid[biqs_level]);
            hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_aR1];
            hmid[biqs_aR1] = hmid[biqs_aR2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_cR1];
            hmid[biqs_cR1] = hmid[biqs_cR2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_eR1];
            hmid[biqs_eR1] = hmid[biqs_eR2] - (hmid[biqs_temp] * hmid[biqs_f1]);
            hmid[biqs_eR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_f2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_outR] *= hmid[biqs_level];
            if (hmid[biqs_level] > 1.0) {
                hmid[biqs_outR] *= hmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            lmid[biqs_outR] = inputSampleR * fabs(lmid[biqs_level]);
            lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_aR1];
            lmid[biqs_aR1] = lmid[biqs_aR2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_cR1];
            lmid[biqs_cR1] = lmid[biqs_cR2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_eR1];
            lmid[biqs_eR1] = lmid[biqs_eR2] - (lmid[biqs_temp] * lmid[biqs_f1]);
            lmid[biqs_eR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_f2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_outR] *= lmid[biqs_level];
            if (lmid[biqs_level] > 1.0) {
                lmid[biqs_outR] *= lmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            double parametric = high[biqs_outL] + hmid[biqs_outL] + lmid[biqs_outL];
            inputSampleL += (parametric * wet); // purely a parallel filter stage here
            parametric = high[biqs_outR] + hmid[biqs_outR] + lmid[biqs_outR];
            inputSampleR += (parametric * wet); // purely a parallel filter stage here
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
} // namespace airwindohhs::parametric
