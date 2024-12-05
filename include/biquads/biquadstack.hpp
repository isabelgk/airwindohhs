#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquadstack {

constexpr std::string_view k_name{ "BiquadStack" };
constexpr std::string_view k_short_description{
    "BiquadStack is a way of making a parametric EQ out of stacked biquad filters."
};
constexpr std::string_view k_long_description{
    "There's already a plugin called BiquadTriple, and it's three biquads simply cascaded to do what they do with steeper roll-offs before resonance. I'd said 'this is to let you mock up effects where you're going to be using filters' and mentioned 'ways to make the Q factors more specific and staggered' but didn't really go there, at the time.That time arrived in a hurry when I started really trying to work out what was so special about SSL channel strips, so I could use similar parametric bands in ConsoleX.So, BiquadStack is out to let you use this right away, without waiting, and see how that goes. The way it works is, it has the same technique used to make very steep Butterworth filters. This is using specific Q factors in combination, so they end up doing a very accurate highpass or lowpass without resonance or irregularities. Typically, when you find this stuff it's carefully designed to behave correctly, and you can make steep Butterworth filters of whatever order you like, this way.Using it for bandpasses instead, and adding nonlinearity, gives you a really interesting response: it's not a narrowing spike as a normal resonant filter would be, instead it's a little region of intensity that you can bring in or remove. The nonlinearity increases as you add more boost, or stays subtle at lower settings. The edges of the region develop little 'moats' to accentuate the effect: some sort of phase interference.The result is what I wanted: tight and effective parametric EQ which is not 'analog modeled', it's designed to do what I'd WANT to do with analog modeling. It's about letting through the energy and sonority (or suppressing energy you don't want) rather than trying to duplicate tone colors of some hardware and muddying things up. This one is full-range, and smoothed, because there's only one of it: if you want to do sweepy automation things keep it around. There will be more, like a three-band that can fit in Airwindows Consolidated, and the full SSL-style four-band that goes in ConsoleX.If you want to get a head start on what that will bring you, play with BiquadStack. If you want four bands of it on every channel in your whole mix… well so do I, and I'm working on it :)"
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class BiquadStack final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        biqs_freq,
        biqs_reso,
        biqs_level,
        biqs_levelA,
        biqs_levelB,
        biqs_nonlin,
        biqs_temp,
        biqs_dis,
        biqs_a0,
        biqs_aA0,
        biqs_aB0,
        biqs_b1,
        biqs_bA1,
        biqs_bB1,
        biqs_b2,
        biqs_bA2,
        biqs_bB2,
        biqs_c0,
        biqs_cA0,
        biqs_cB0,
        biqs_d1,
        biqs_dA1,
        biqs_dB1,
        biqs_d2,
        biqs_dA2,
        biqs_dB2,
        biqs_e0,
        biqs_eA0,
        biqs_eB0,
        biqs_f1,
        biqs_fA1,
        biqs_fB1,
        biqs_f2,
        biqs_fA2,
        biqs_fB2,
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
    double biqs[biqs_total];

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BiquadStack()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < biqs_total; x++) {
            biqs[x] = 0.0;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "freq"; break;
            case kParamB: return "level"; break;
            case kParamC: return "reso"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Freq"; break;
            case kParamB: return "Level"; break;
            case kParamC: return "Reso"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        biqs[biqs_levelA] = biqs[biqs_levelB];
        biqs[biqs_aA0] = biqs[biqs_aB0];
        biqs[biqs_bA1] = biqs[biqs_bB1];
        biqs[biqs_bA2] = biqs[biqs_bB2];
        biqs[biqs_cA0] = biqs[biqs_cB0];
        biqs[biqs_dA1] = biqs[biqs_dB1];
        biqs[biqs_dA2] = biqs[biqs_dB2];
        biqs[biqs_eA0] = biqs[biqs_eB0];
        biqs[biqs_fA1] = biqs[biqs_fB1];
        biqs[biqs_fA2] = biqs[biqs_fB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        biqs[biqs_freq] = (((pow(A, 4) * 19980.0) + 20.0) / Effect<T>::getSampleRate());
        biqs[biqs_nonlin] = B;
        biqs[biqs_levelB] = (biqs[biqs_nonlin] * 2.0) - 1.0;
        if (biqs[biqs_levelB] > 0.0) {
            biqs[biqs_levelB] *= 2.0;
        }
        biqs[biqs_reso] = ((0.5 + (biqs[biqs_nonlin] * 0.5) + sqrt(biqs[biqs_freq])) - (1.0 - pow(1.0 - C, 2.0))) + 0.5 + (biqs[biqs_nonlin] * 0.5);
        double K = tan(M_PI * biqs[biqs_freq]);
        double norm = 1.0 / (1.0 + K / (biqs[biqs_reso] * 1.93185165) + K * K);
        biqs[biqs_aB0] = K / (biqs[biqs_reso] * 1.93185165) * norm;
        biqs[biqs_bB1] = 2.0 * (K * K - 1.0) * norm;
        biqs[biqs_bB2] = (1.0 - K / (biqs[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (biqs[biqs_reso] * 0.70710678) + K * K);
        biqs[biqs_cB0] = K / (biqs[biqs_reso] * 0.70710678) * norm;
        biqs[biqs_dB1] = 2.0 * (K * K - 1.0) * norm;
        biqs[biqs_dB2] = (1.0 - K / (biqs[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (biqs[biqs_reso] * 0.51763809) + K * K);
        biqs[biqs_eB0] = K / (biqs[biqs_reso] * 0.51763809) * norm;
        biqs[biqs_fB1] = 2.0 * (K * K - 1.0) * norm;
        biqs[biqs_fB2] = (1.0 - K / (biqs[biqs_reso] * 0.51763809) + K * K) * norm;
        if (biqs[biqs_aA0] == 0.0) { // if we have just started, start directly with raw info
            biqs[biqs_levelA] = biqs[biqs_levelB];
            biqs[biqs_aA0] = biqs[biqs_aB0];
            biqs[biqs_bA1] = biqs[biqs_bB1];
            biqs[biqs_bA2] = biqs[biqs_bB2];
            biqs[biqs_cA0] = biqs[biqs_cB0];
            biqs[biqs_dA1] = biqs[biqs_dB1];
            biqs[biqs_dA2] = biqs[biqs_dB2];
            biqs[biqs_eA0] = biqs[biqs_eB0];
            biqs[biqs_fA1] = biqs[biqs_fB1];
            biqs[biqs_fA2] = biqs[biqs_fB2];
        }
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
            double buf = (double)sampleFrames / inFramesToProcess;
            biqs[biqs_level] = (biqs[biqs_levelA] * buf) + (biqs[biqs_levelB] * (1.0 - buf));
            biqs[biqs_a0] = (biqs[biqs_aA0] * buf) + (biqs[biqs_aB0] * (1.0 - buf));
            biqs[biqs_b1] = (biqs[biqs_bA1] * buf) + (biqs[biqs_bB1] * (1.0 - buf));
            biqs[biqs_b2] = (biqs[biqs_bA2] * buf) + (biqs[biqs_bB2] * (1.0 - buf));
            biqs[biqs_c0] = (biqs[biqs_cA0] * buf) + (biqs[biqs_cB0] * (1.0 - buf));
            biqs[biqs_d1] = (biqs[biqs_dA1] * buf) + (biqs[biqs_dB1] * (1.0 - buf));
            biqs[biqs_d2] = (biqs[biqs_dA2] * buf) + (biqs[biqs_dB2] * (1.0 - buf));
            biqs[biqs_e0] = (biqs[biqs_eA0] * buf) + (biqs[biqs_eB0] * (1.0 - buf));
            biqs[biqs_f1] = (biqs[biqs_fA1] * buf) + (biqs[biqs_fB1] * (1.0 - buf));
            biqs[biqs_f2] = (biqs[biqs_fA2] * buf) + (biqs[biqs_fB2] * (1.0 - buf));
            // begin Stacked Biquad With Reversed Neutron Flow L
            biqs[biqs_outL] = inputSampleL * fabs(biqs[biqs_level]);
            biqs[biqs_dis] = fabs(biqs[biqs_a0] * (1.0 + (biqs[biqs_outL] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_aL1];
            biqs[biqs_aL1] = biqs[biqs_aL2] - (biqs[biqs_temp] * biqs[biqs_b1]);
            biqs[biqs_aL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_b2]);
            biqs[biqs_outL] = biqs[biqs_temp];
            biqs[biqs_dis] = fabs(biqs[biqs_c0] * (1.0 + (biqs[biqs_outL] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_cL1];
            biqs[biqs_cL1] = biqs[biqs_cL2] - (biqs[biqs_temp] * biqs[biqs_d1]);
            biqs[biqs_cL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_d2]);
            biqs[biqs_outL] = biqs[biqs_temp];
            biqs[biqs_dis] = fabs(biqs[biqs_e0] * (1.0 + (biqs[biqs_outL] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outL] * biqs[biqs_dis]) + biqs[biqs_eL1];
            biqs[biqs_eL1] = biqs[biqs_eL2] - (biqs[biqs_temp] * biqs[biqs_f1]);
            biqs[biqs_eL2] = (biqs[biqs_outL] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_f2]);
            biqs[biqs_outL] = biqs[biqs_temp];
            biqs[biqs_outL] *= biqs[biqs_level];
            if (biqs[biqs_level] > 1.0) {
                biqs[biqs_outL] *= biqs[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow R
            biqs[biqs_outR] = inputSampleR * fabs(biqs[biqs_level]);
            biqs[biqs_dis] = fabs(biqs[biqs_a0] * (1.0 + (biqs[biqs_outR] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outR] * biqs[biqs_dis]) + biqs[biqs_aR1];
            biqs[biqs_aR1] = biqs[biqs_aR2] - (biqs[biqs_temp] * biqs[biqs_b1]);
            biqs[biqs_aR2] = (biqs[biqs_outR] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_b2]);
            biqs[biqs_outR] = biqs[biqs_temp];
            biqs[biqs_dis] = fabs(biqs[biqs_c0] * (1.0 + (biqs[biqs_outR] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outR] * biqs[biqs_dis]) + biqs[biqs_cR1];
            biqs[biqs_cR1] = biqs[biqs_cR2] - (biqs[biqs_temp] * biqs[biqs_d1]);
            biqs[biqs_cR2] = (biqs[biqs_outR] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_d2]);
            biqs[biqs_outR] = biqs[biqs_temp];
            biqs[biqs_dis] = fabs(biqs[biqs_e0] * (1.0 + (biqs[biqs_outR] * biqs[biqs_nonlin])));
            if (biqs[biqs_dis] > 1.0) {
                biqs[biqs_dis] = 1.0;
            }
            biqs[biqs_temp] = (biqs[biqs_outR] * biqs[biqs_dis]) + biqs[biqs_eR1];
            biqs[biqs_eR1] = biqs[biqs_eR2] - (biqs[biqs_temp] * biqs[biqs_f1]);
            biqs[biqs_eR2] = (biqs[biqs_outR] * -biqs[biqs_dis]) - (biqs[biqs_temp] * biqs[biqs_f2]);
            biqs[biqs_outR] = biqs[biqs_temp];
            biqs[biqs_outR] *= biqs[biqs_level];
            if (biqs[biqs_level] > 1.0) {
                biqs[biqs_outR] *= biqs[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            inputSampleL += biqs[biqs_outL]; // purely a parallel filter stage here
            inputSampleR += biqs[biqs_outR]; // purely a parallel filter stage here
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
} // namespace airwindohhs::biquadstack
