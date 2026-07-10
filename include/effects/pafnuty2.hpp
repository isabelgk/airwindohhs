#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::pafnuty2 {

constexpr std::string_view k_name{ "Pafnuty2" };
constexpr std::string_view k_short_description{
    "Pafnuty2 is a Chebyshev filter, that adds harmonics, and fits in the VCV Rack port."
};
constexpr std::string_view k_long_description{
    "Pafnuty is a Chebyshev filter. What are those? Well, it’s not much like your usual filter: you don’t use this to roll off highs or lows (though under some conditions you might be able to do any of those things). A Chebyshev filter is like a mathematical formula. It works like this: if you feed it a sine wave (at exactly 0dB, or barely-clipping) it can generate entirely new sine waves to add to your sine wave. Which ones? Harmonically related ones. You can have twice, three, four times the frequency, all the way up to thirteenth harmonic.What do you get when you run music into this sine-multiplying filter? If your audio has no frequencies that, multiplied, go higher than the sampling rate, you get perfect aliasing-free harmonic enhancement. The way the filter works, it absolutely doesn’t generate anything higher than the multipliers it works with. It’s a sort of color-adding harmonic enhancement where you can pick what kind of coloration you add (or subtract, since all the controls go both ways). If the frequencies do go higher than the sampling rate then they do alias, but the way Pafnuty resists adding extra harmonics helps it to resist aliasing and if you don’t add lots of higher harmonics you can go very high in frequency, cleanly.Now that it fits into the VCV Rack port, you can run a sine LFO into it, and then all sorts of other LFOs into all the parameters, to produce a bizarre modular hyper-LFO, and that's why I knew this one needed updating :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Pafnuty2 final : public Effect<T>
{
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
    float H;
    float I;
    float J;

  public:
    Pafnuty2()
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
        J = 0.5;
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "second"; break;
            case kParamB: return "third"; break;
            case kParamC: return "fourth"; break;
            case kParamD: return "fifth"; break;
            case kParamE: return "sixth"; break;
            case kParamF: return "seventh"; break;
            case kParamG: return "eighth"; break;
            case kParamH: return "ninth"; break;
            case kParamI: return "tenth"; break;
            case kParamJ: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Second"; break;
            case kParamB: return "Third"; break;
            case kParamC: return "Fourth"; break;
            case kParamD: return "Fifth"; break;
            case kParamE: return "Sixth"; break;
            case kParamF: return "Seventh"; break;
            case kParamG: return "Eighth"; break;
            case kParamH: return "Ninth"; break;
            case kParamI: return "Tenth"; break;
            case kParamJ: return "Inv/Wet"; break;

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
            case kParamI: return std::to_string(I); break;
            case kParamJ: return std::to_string(J); break;

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

        double chebyshev;
        double effxct; // this highlighted when spelled 'effect', might be a reserved word for VST
        double inP2;
        double inP3;
        double inP4;
        double inP5;
        double inP6;
        double inP7;
        double inP8;
        double inP9;
        double inP10;
        double second = (((A * 2.0) - 1.0) * 1.0);
        second = second * fabs(second);
        double third = -(((B * 2.0) - 1.0) * 0.60);
        third = third * fabs(third);
        double fourth = -(((C * 2.0) - 1.0) * 0.60);
        fourth = fourth * fabs(fourth);
        double fifth = (((D * 2.0) - 1.0) * 0.45);
        fifth = fifth * fabs(fifth);
        double sixth = (((E * 2.0) - 1.0) * 0.45);
        sixth = sixth * fabs(sixth);
        double seventh = -(((F * 2.0) - 1.0) * 0.38);
        seventh = seventh * fabs(seventh);
        double eighth = -(((G * 2.0) - 1.0) * 0.38);
        eighth = eighth * fabs(eighth);
        double ninth = (((H * 2.0) - 1.0) * 0.35);
        ninth = ninth * fabs(ninth);
        double tenth = (((I * 2.0) - 1.0) * 0.35);
        tenth = tenth * fabs(tenth);
        double amount = (J * 2.0) - 1.0;
        amount = amount * fabs(amount);
        // setting up
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // left channel
            effxct = 0.0;
            inP2 = inputSampleL * inputSampleL;
            inP3 = inP2 * inputSampleL;
            inP4 = inP3 * inputSampleL;
            inP5 = inP4 * inputSampleL;
            inP6 = inP5 * inputSampleL;
            inP7 = inP6 * inputSampleL;
            inP8 = inP7 * inputSampleL;
            inP9 = inP8 * inputSampleL;
            inP10 = inP9 * inputSampleL;
            // let's do the powers ahead of time and see how we do.
            if (second != 0.0) {
                chebyshev = (2 * inP2);
                effxct += (chebyshev * second);
            }
            if (third != 0.0) {
                chebyshev = (4 * inP3) - (3 * inputSampleL);
                effxct += (chebyshev * third);
            }
            if (fourth != 0.0) {
                chebyshev = (8 * inP4) - (8 * inP2);
                effxct += (chebyshev * fourth);
            }
            if (fifth != 0.0) {
                chebyshev = (16 * inP5) - (20 * inP3) + (5 * inputSampleL);
                effxct += (chebyshev * fifth);
            }
            if (sixth != 0.0) {
                chebyshev = (32 * inP6) - (48 * inP4) + (18 * inP2);
                effxct += (chebyshev * sixth);
            }
            if (seventh != 0.0) {
                chebyshev = (64 * inP7) - (112 * inP5) + (56 * inP3) - (7 * inputSampleL);
                effxct += (chebyshev * seventh);
            }
            if (eighth != 0.0) {
                chebyshev = (128 * inP8) - (256 * inP6) + (160 * inP4) - (32 * inP2);
                effxct += (chebyshev * eighth);
            }
            if (ninth != 0.0) {
                chebyshev = (256 * inP9) - (576 * inP7) + (432 * inP5) - (120 * inP3) + (9 * inputSampleL);
                effxct += (chebyshev * ninth);
            }
            if (tenth != 0.0) {
                chebyshev = (512 * inP10) - (1280 * inP8) + (1120 * inP6) - (400 * inP4) + (50 * inP2);
                effxct += (chebyshev * tenth);
            }
            // Yowza! Aren't we glad we're testing to see if we can skip these little bastards?
            inputSampleL += (effxct * amount);
            // You too can make a horrible graunch and then SUBTRACT it leaving only the refreshing smell of pine...
            // end left channel
            // right channel
            effxct = 0.0;
            inP2 = inputSampleR * inputSampleR;
            inP3 = inP2 * inputSampleR;
            inP4 = inP3 * inputSampleR;
            inP5 = inP4 * inputSampleR;
            inP6 = inP5 * inputSampleR;
            inP7 = inP6 * inputSampleR;
            inP8 = inP7 * inputSampleR;
            inP9 = inP8 * inputSampleR;
            inP10 = inP9 * inputSampleR;
            // let's do the powers ahead of time and see how we do.
            if (second != 0.0) {
                chebyshev = (2 * inP2);
                effxct += (chebyshev * second);
            }
            if (third != 0.0) {
                chebyshev = (4 * inP3) - (3 * inputSampleR);
                effxct += (chebyshev * third);
            }
            if (fourth != 0.0) {
                chebyshev = (8 * inP4) - (8 * inP2);
                effxct += (chebyshev * fourth);
            }
            if (fifth != 0.0) {
                chebyshev = (16 * inP5) - (20 * inP3) + (5 * inputSampleR);
                effxct += (chebyshev * fifth);
            }
            if (sixth != 0.0) {
                chebyshev = (32 * inP6) - (48 * inP4) + (18 * inP2);
                effxct += (chebyshev * sixth);
            }
            if (seventh != 0.0) {
                chebyshev = (64 * inP7) - (112 * inP5) + (56 * inP3) - (7 * inputSampleR);
                effxct += (chebyshev * seventh);
            }
            if (eighth != 0.0) {
                chebyshev = (128 * inP8) - (256 * inP6) + (160 * inP4) - (32 * inP2);
                effxct += (chebyshev * eighth);
            }
            if (ninth != 0.0) {
                chebyshev = (256 * inP9) - (576 * inP7) + (432 * inP5) - (120 * inP3) + (9 * inputSampleR);
                effxct += (chebyshev * ninth);
            }
            if (tenth != 0.0) {
                chebyshev = (512 * inP10) - (1280 * inP8) + (1120 * inP6) - (400 * inP4) + (50 * inP2);
                effxct += (chebyshev * tenth);
            }
            // Yowza! Aren't we glad we're testing to see if we can skip these little bastards?
            inputSampleR += (effxct * amount);
            // You too can make a horrible graunch and then SUBTRACT it leaving only the refreshing smell of pine...
            // end right channel
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
} // namespace airwindohhs::pafnuty2
