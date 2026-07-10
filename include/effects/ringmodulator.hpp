#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ringmodulator {

constexpr std::string_view k_name{ "RingModulator" };
constexpr std::string_view k_short_description{
    "RingModulator repitches sounds mathematically, not harmonically."
};
constexpr std::string_view k_long_description{
    "Here's an effect not for every day or the master buss! (pause, for fans to immediately insist that they use it on their master buss and it changed their life)So, this is probably the major offshoot from SquareRoot apart from its use as an overdrive. RingModulator is like LRConvolve, except you always have the most polite possible convolver (a sine) and you get to control it to do whatever you like, from subsonic LFO throbs to very high pitches. In fact, when you're in stereo you get two independent sines to play with!The rest is simply a ring modulator, the device that makes voices into Daleks or electric pianos into oddly clangy discordant inharmonic sounds. Ring modulators can produce mathematically, not harmonically, related sounds. That means it 'tracks' quickly to whatever your raw sound is, but the notes it adds are out of tune: going off, or even going in the reverse of your original note's direction.That's because if you take a note, and convolve it by a nearby note, you'll produce a higher note but also a strong subharmonic. Since the ring modulator is flipping phase at musical frequencies, it can produce an apparent note way lower than itself or the source note, through that interference. It'll also tend to cut the lows in the source audio if it's at a high frequency, because if you're constantly flipping the phase of a bass note it kinda goes away on you.Then to top it off, RingModulator has the Soar control… so you can wildly alter the texture of the additional notes (including stereo added notes when you've got the Freq controls set to different settings) by either reducing Soar for a gatey, thin sound, or boosting it for a dense and lively sound! The reason Soar's important here is because convolving stuff is multiplying, and if you square something (multiply it by itself) and then take a square root you get the original thing back. So Soar is my way of restoring the density of the original sound coming in, except that it opens up a new way to alter the tone of things.Hope you like it!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class RingModulator final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double sinePosL;
    double sinePosR;
    double incLA;
    double incLB;
    double incRA;
    double incRB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    RingModulator()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        D = 0.0;
        sinePosL = 0.0;
        sinePosR = 0.0;
        incLA = 0.0;
        incLB = 0.0;
        incRA = 0.0;
        incRB = 0.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "freq"; break;
            case kParamB: return "freq"; break;
            case kParamC: return "soar"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Freq"; break;
            case kParamB: return "Freq"; break;
            case kParamC: return "Soar"; break;
            case kParamD: return "Dry/Wet"; break;

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
        incLA = incLB;
        incLB = pow(A, 5) / overallscale;
        incRA = incRB;
        incRB = pow(B, 5) / overallscale;
        double soar = 0.3 - (C * 0.3);
        double wet = pow(D, 2);
        while (--sampleFrames >= 0) {
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double incL = (incLA * temp) + (incLB * (1.0 - temp));
            double incR = (incRA * temp) + (incRB * (1.0 - temp));
            sinePosL += incL;
            if (sinePosL > 6.283185307179586) {
                sinePosL -= 6.283185307179586;
            }
            double sinResultL = sin(sinePosL);
            sinePosR += incR;
            if (sinePosR > 6.283185307179586) {
                sinePosR -= 6.283185307179586;
            }
            double sinResultR = sin(sinePosR);
            double out = 0.0;
            double snM = fabs(sinResultL) + (soar * soar);
            double inM = fabs(inputSampleL);
            if (inM < snM) {
                inM = fabs(sinResultL);
                snM = fabs(inputSampleL) + (soar * soar);
            }
            if (inputSampleL > 0.0 && sinResultL > 0.0) {
                out = fmax((sqrt(inM / snM) * snM) - soar, 0.0);
            }
            if (inputSampleL < 0.0 && sinResultL > 0.0) {
                out = fmin((-sqrt(inM / snM) * snM) + soar, 0.0);
            }
            if (inputSampleL > 0.0 && sinResultL < 0.0) {
                out = fmin((-sqrt(inM / snM) * snM) + soar, 0.0);
            }
            if (inputSampleL < 0.0 && sinResultL < 0.0) {
                out = fmax((sqrt(inM / snM) * snM) - soar, 0.0);
            }
            inputSampleL = out;
            out = 0.0;
            snM = fabs(sinResultR) + (soar * soar);
            inM = fabs(inputSampleR);
            if (inM < snM) {
                inM = fabs(sinResultR);
                snM = fabs(inputSampleR) + (soar * soar);
            }
            if (inputSampleR > 0.0 && sinResultR > 0.0) {
                out = fmax((sqrt(inM / snM) * snM) - soar, 0.0);
            }
            if (inputSampleR < 0.0 && sinResultR > 0.0) {
                out = fmin((-sqrt(inM / snM) * snM) + soar, 0.0);
            }
            if (inputSampleR > 0.0 && sinResultR < 0.0) {
                out = fmin((-sqrt(inM / snM) * snM) + soar, 0.0);
            }
            if (inputSampleR < 0.0 && sinResultR < 0.0) {
                out = fmax((sqrt(inM / snM) * snM) - soar, 0.0);
            }
            inputSampleR = out;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::ringmodulator
