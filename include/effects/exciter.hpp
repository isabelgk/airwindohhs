#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::exciter {

constexpr std::string_view k_name{ "Exciter" };
constexpr std::string_view k_short_description{
    "Exciter is an Aural Exciter plugin that can be both subtle and extreme."
};
constexpr std::string_view k_long_description{
    "This plugin isn't my usual jam, but I've been listening to this type of effect all my life. The Steely Dan album 'The Royal Scam' wouldn't be the same without the Exciter effect. And now it's a free Airwindows plugin!Note that this is NOT a clone of hardware, or any particular brand. As I usually do, I've extracted the guts of the effect and then adapted it so it can be used normally or exaggerated. You'll find that as you apply the effect, it'll start off sounding like it does nothing, gets more and more intense and then suddenly blows up into crazy distortion. To use it like a normal exciter, fine-tune it so that it's just barely making transients 'pop'. If you're hearing obvious crunch, you've already got it cranked up higher than real-world examples would let you do.The effect works like this: get a sharp band filter going to extract certain kinds of information. Distort it with a soft clip (I use a sin() function, and some real-world examples used a 4049 hex inverter chip, which does a very similar super soft distort when used as an audio effect: it's the chip that made up Craig Anderton's 'Tube Sound Fuzz' circuit back in the day, and I still have lots of these chips to play with :D ) Then, once you've distorted this bandpass, add just only the distortion elements back into the full bandwidth signal, by subtracting the bandpass again.Exciter lets you adjust the frequency you're using, and dial in the amount of effect you want. I'm pretty sure it'll consistently sound good (not quite natural, but this is 'late seventies heightened detail' tone here, it doesn't have to sound natural) if you're careful to not crank it too much. And of course this is Airwindows, you can crank it on stuff that doesn't have much to excite, and blast it on bright stuff to make a distinct form of gritty evil distortion for effect.A lot of the stuff I like in analog is when transistors and chips are misused and freaking out. You can get tones like that out of Exciter, if you like. You can put it in the middle of uLaw, if you like. I've not tried that so you can be the first. I hope you enjoy Exciter :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Exciter final : public Effect<T>
{
    double biquad[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Exciter()
    {
        A = 0.6;
        B = 0.0;
        for (int x = 0; x < 11; x++) {
            biquad[x] = 0.0;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.6;
            break;
        kParamB:
            return 0.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "freq";
            break;
        kParamB:
            return "excite";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Freq";
            break;
        kParamB:
            return "Excite";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;

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

        biquad[0] = ((A * 7000.0) + 8000.0) / getSampleRate();
        biquad[1] = A + B + 0.7071;
        // tighter resonance as frequency and boost increases
        double boost = pow(B, 2.0) * 16.0;
        double K = tan(M_PI * biquad[0]);
        double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
        biquad[2] = K / 0.7071 * norm;
        biquad[4] = -biquad[2];
        biquad[5] = 2.0 * (K * K - 1.0) * norm;
        biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        // bandpass to focus the intensity of the effect
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
            double outSampleL = (inputSampleL * biquad[2]) + biquad[7];
            biquad[7] = (inputSampleL * biquad[3]) - (outSampleL * biquad[5]) + biquad[8];
            biquad[8] = (inputSampleL * biquad[4]) - (outSampleL * biquad[6]);
            double outSampleR = (inputSampleR * biquad[2]) + biquad[9];
            biquad[9] = (inputSampleR * biquad[3]) - (outSampleR * biquad[5]) + biquad[10];
            biquad[10] = (inputSampleR * biquad[4]) - (outSampleR * biquad[6]);
            // so the audio we're working with is going to be a bandpassed signal: only high mids
            outSampleL *= boost;
            if (outSampleL > M_PI) {
                outSampleL = M_PI;
            }
            if (outSampleL < -M_PI) {
                outSampleL = -M_PI;
            }
            outSampleL -= sin(outSampleL);
            outSampleR *= boost;
            if (outSampleR > M_PI) {
                outSampleR = M_PI;
            }
            if (outSampleR < -M_PI) {
                outSampleR = -M_PI;
            }
            outSampleR -= sin(outSampleR);
            // so we're clipping to generate artifacts, but subtracting them, meaning that
            // our deviations from the unclipped waveform are now negative in form.
            outSampleL *= boost;
            if (outSampleL > M_PI) {
                outSampleL = M_PI;
            }
            if (outSampleL < -M_PI) {
                outSampleL = -M_PI;
            }
            outSampleL = sin(outSampleL);
            outSampleR *= boost;
            if (outSampleR > M_PI) {
                outSampleR = M_PI;
            }
            if (outSampleR < -M_PI) {
                outSampleR = -M_PI;
            }
            outSampleR = sin(outSampleR);
            // now we're clipping the result, to make the peaks less intense
            inputSampleL -= outSampleL;
            inputSampleR -= outSampleR;
            // and we apply only those deviations from distorted high mids, by subtracting them from
            // the original full bandwidth audio. And that's how analog aural exciters basically work.
            // The real ones used a 4049 chip sometimes to produce the soft saturation we're getting with sin()
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // and we'll do the harshest of hardclips to cope with how intense the new peaks can get,
            // without in any way dialing back the ruthless brightness the exciter can create.
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
} // namespace airwindohhs::exciter
