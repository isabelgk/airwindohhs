#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::discontapeity {

constexpr std::string_view k_name{ "Discontapeity" };
constexpr std::string_view k_short_description{
    "Discontapeity combines air modeling with tape-style overdrive."
};
constexpr std::string_view k_long_description{
    "TapeHack came out, and boom! Everyone immediately wanted an update to ToTape that incorporated it.I mean, fair. But have you considered taking it and making it LESS like tape?I did. Discontapeity might be a cumbersome name, but when it finds its way into new models of Airwindows Console you'll know it by the label I put on it, which is simply 'More'.No form of tape, real or modeled, does the type of air nonlinearity characteristic of loud sound over distance. That's not even a thing. It's strictly left to my plugins Discontinuity and Disintegrate for the bolder audio-wreckers, and as a permanent part of the sound of ConsoleX. It's built into both channel and buss on that, for the purpose of delivering more convincingly loud-sounding mixes, as needed.Add TapeHack to it, or to be more accurate, change both things to lean closer to each other in behavior, and an interesting effect emerges.TapeHack allows for intense overdrive, a gain boost of 10x much like my Z series filters. Discontinuity layers three instances of the effect so it can go up to extremely high SPLs and emulate unusual sounds for sound design purposes (Disintegrate can go even farther).Discontapeity pulls all that back. Less gain. Only one stage of Discontinuity, expanded slightly to compensate. It's cleaner, simpler, and though there's still a little bit of gain on tap, it doesn't try to take that very far.Instead, if you leave it at 0.0, you have a very clean subdued 'more' that is still a touch bigger and fuller than bypass. And if you bring it up to the middle of 0.5 or so, the sound blooms like it might in a real studio in the golden age of tape and giant mixing desks. If you push it to 1.0, as far as you can crank, that's where you get a touch of distortion and overload, still in a relatively polite way. For smashing drums and such, try layering it after regular TapeHack, or perhaps SquareRoot or some other unusual intensifier: Sinew, maybe?No, I didn't update the flagship tape emulator ToTape. Every day I'm learning more about what I'll do, when I do. But maybe, what you need is not me including every possible real-tape coloration and behavior… instead, try taking some of those qualities and boiling them down to ever-simpler forms so you can have some of the stuff you want while letting maximum tone and sonority through your music.Think of it as sonic bloom, or scale and bigness, or simply 'more'. You can still use TapeHack, or indeed ToTape8 or 7. Not every worthy exploration is in the direction everybody else is going. This one's designed to show up on both Console channel plugins and buss plugins, and still be good enough to permanently include in both places. I hope you like it :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Discontapeity final : public Effect<T>
{
    float A;
    double dBaL[dscBuf + 5];
    double dBaPosL;
    int dBaXL;
    double dBaR[dscBuf + 5];
    double dBaPosR;
    int dBaXR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Discontapeity()
    {
        A = 0.0;
        for (int count = 0; count < dscBuf + 2; count++) {
            dBaL[count] = 0.0;
            dBaR[count] = 0.0;
        }
        dBaPosL = 0.0;
        dBaPosR = 0.0;
        dBaXL = 1;
        dBaXR = 1;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "more"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "More"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;

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
        double moreDiscontinuity = fmax(pow(A * 0.42, 3.0) * overallscale, 0.00001);
        double moreTapeHack = (A * 1.4152481) + 1.2;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // begin Discontinuity section
            inputSampleL *= moreDiscontinuity;
            dBaL[dBaXL] = inputSampleL;
            dBaPosL *= 0.5;
            dBaPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            dBaPosL = fmin(dBaPosL, 1.0);
            int dBdly = floor(dBaPosL * dscBuf);
            double dBi = (dBaPosL * dscBuf) - dBdly;
            inputSampleL = dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXL++;
            if (dBaXL < 0 || dBaXL >= dscBuf) {
                dBaXL = 0;
            }
            inputSampleL /= moreDiscontinuity;
            // end Discontinuity section, begin TapeHack section
            inputSampleL = fmax(fmin(inputSampleL * moreTapeHack, 2.305929007734908), -2.305929007734908);
            double addtwo = inputSampleL * inputSampleL;
            double empower = inputSampleL * addtwo; // inputSampleL to the third power
            inputSampleL -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleL += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleL -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleL += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleL -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            inputSampleL *= 0.9239;
            // end TapeHack section
            // begin Discontinuity section
            inputSampleR *= moreDiscontinuity;
            dBaR[dBaXR] = inputSampleR;
            dBaPosR *= 0.5;
            dBaPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBaPosR = fmin(dBaPosR, 1.0);
            dBdly = floor(dBaPosR * dscBuf);
            dBi = (dBaPosR * dscBuf) - dBdly;
            inputSampleR = dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXR++;
            if (dBaXR < 0 || dBaXR >= dscBuf) {
                dBaXR = 0;
            }
            inputSampleR /= moreDiscontinuity;
            // end Discontinuity section, begin TapeHack section
            inputSampleR = fmax(fmin(inputSampleR * moreTapeHack, 2.305929007734908), -2.305929007734908);
            addtwo = inputSampleR * inputSampleR;
            empower = inputSampleR * addtwo; // inputSampleR to the third power
            inputSampleR -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleR += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleR -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleR += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleR -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            inputSampleR *= 0.9239;
            // end TapeHack section
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
} // namespace airwindohhs::discontapeity
