#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::avermatrix {

constexpr std::string_view k_name{ "AverMatrix" };
constexpr std::string_view k_short_description{
    "AverMatrix turns averaging into a full-featured EQ."
};
constexpr std::string_view k_long_description{
    "My Average plugin is pretty handy. It gives you from one to ten taps of simple averaging, as a continuously variable thing. At higher settings, there are comb-filtery artifacts that come in as a result of how averaging works, but you can tune them with the filter setting and they sound pretty natural… and averaging has very good time response because it doesn’t have an IIR ‘tail’, so it’s very clean-sounding in ways normal filters aren’t.AverMatrix takes that, and gives you one to ten POLES of filtering like in the first Average. That’s continuous too: you can have three and a half poles, no trouble (it just generates the third pole and then fades halfway into it) so the subtlety of adjustment is great.AND, also by request, AverMatrix uses the Inv/Dry/Wet trick some of my other plugins use, and it’s a great idea to include. Go to full ‘inverse’ and AverMatrix is made to keep the dry and subtract the inverted wet from it… which means, now it’s a highpass. An amazing-sounding highpass, with great clarity and airy-ness and just as clean transient behavior, and just like applying dry/wet on the regular averaging, you can apply dry/inverse to give only a subtle treble lift. I think this is going to work real well for people. And one more thing: since native averaging at high numbers of taps gives a funny notch that you can tune, and since this is inverse, you can use this behavior to give a highs boost with a funny, tuneable peak. Set it wrongly and maybe it’ll make your mic sound nasty (don’t reinforce existing spikes in the mic’s response) but place it just right and you can get treble lift that neatly avoids the resonances of your mic… and enjoy the response of a much fancier microphone. (your settings will be sample-rate dependent so tune it again if you change sample rate)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class AverMatrix final : public Effect<T>
{
    double bL[11][11];
    double bR[11][11];
    double f[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    AverMatrix()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        for (int x = 0; x < 11; x++) {
            f[x] = 0.0;
            for (int y = 0; y < 11; y++) {
                bL[x][y] = 0.0;
                bR[x][y] = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "average"; break;
            case kParamB: return "depth"; break;
            case kParamC: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Average"; break;
            case kParamB: return "Depth"; break;
            case kParamC: return "Inv/Wet"; break;

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
            case kParamA: return "taps"; break;
            case kParamB: return "poles"; break;
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

        double overalltaps = (A * 9.0) + 1.0;
        double taps = overalltaps;
        // this is our averaging, which is not integer but continuous
        double overallpoles = (B * 9.0) + 1.0;
        // this is the poles of the filter, also not integer but continuous
        int yLimit = floor(overallpoles) + 1;
        double yPartial = overallpoles - floor(overallpoles);
        // now we can do a for loop, and also apply the final pole continuously
        double wet = (C * 2.0) - 1.0;
        double dry = (1.0 - wet);
        if (dry > 1.0) {
            dry = 1.0;
        }
        int xLimit = 1;
        for (int x = 0; x < 11; x++) {
            if (taps > 1.0) {
                f[x] = 1.0;
                taps -= 1.0;
                xLimit++;
            }
            else {
                f[x] = taps;
                taps = 0.0;
            }
        } // there, now we have a neat little moving average with remainders
        if (xLimit > 9) {
            xLimit = 9;
        }
        if (overalltaps < 1.0) {
            overalltaps = 1.0;
        }
        for (int x = 0; x < xLimit; x++) {
            f[x] /= overalltaps;
        } // and now it's neatly scaled, too
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            double previousPoleL = 0;
            double previousPoleR = 0;
            for (int y = 0; y < yLimit; y++) {
                for (int x = xLimit; x >= 0; x--) {
                    bL[x + 1][y] = bL[x][y];
                    bR[x + 1][y] = bR[x][y];
                }
                bL[0][y] = previousPoleL = inputSampleL;
                bR[0][y] = previousPoleR = inputSampleR;
                inputSampleL = 0.0;
                inputSampleR = 0.0;
                for (int x = 0; x < xLimit; x++) {
                    inputSampleL += (bL[x][y] * f[x]);
                    inputSampleR += (bR[x][y] * f[x]);
                }
            }
            inputSampleL = (previousPoleL * (1.0 - yPartial)) + (inputSampleL * yPartial);
            inputSampleR = (previousPoleR * (1.0 - yPartial)) + (inputSampleR * yPartial);
            // in this way we can blend in the final pole
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            // wet can be negative, in which case dry is always full volume and they cancel
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
} // namespace airwindohhs::avermatrix
