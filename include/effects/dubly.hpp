#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubly {

constexpr std::string_view k_name{ "Dubly" };
constexpr std::string_view k_short_description{
    "Dubly is a retro vibe and atmosphere maker."
};
constexpr std::string_view k_long_description{
    "Some people will recognize this instantly… even if you don't do heavy metal in Dubly, you know :)Here's what this is. There's a famous sort of noise reduction, with variations, that's based around the idea of taking the audio, filtering it, compressing it 2:1 and adding that TO the signal. You then record that to tape. And when you play it back, you take that combination, compress it and filter it AGAIN, but this time subtract it from the signal. And hey presto, perfect sound!Well… nope. Doesn't add up. Changes the sound. The thing is… that changed sound is sort of magical.How come? For starters, when you distort tape you get harmonics. If you do that to the sound you start getting harmonics a lot sooner, over a broader range of sounds… but then you're taking the added harmonics you just made, and you're subtracting them again. You're turning them down. You're making the dynamics of your sound come with dynamically varying harmonics, a real color adder, but without the dynamic squashing of just plain saturating. And that's not even getting into when you've got the machines misaligned… or tuned by ear to bring out a particular magical sparkliness and ambience that's NOT really accurate, but maybe is the special sauce for a particular studio's recording machines… we may never know, we know only that these old noise reduction things were fiddly beasts…So what if you strip all that down, Airwindows-style, to only the basic principle?Meet Dubly. It uses uLaw technology (really!) for good, not evil! It does the most general form of that processing, NOT like any specific very proprietary tech, and uses a simple distortion inside rather than any tricky tape emulation (that can come later!). It defaults to 0.5, where it will just bring a delicate aura of retro magic to the sound. But, the way it's calibrated (very tricky, by nature) you can crank it out and mis-calibrate it. And when you do, you'll get more and more into a crazy, hyped, intensified zone that adds a lot of treble energy. But this doesn't make it into a 'single ended' version of the processing (even though that's a known secret weapon). It remains the double-ended, full chain, noise-reducing system (that is, if the single distortion stage was adding noise, which it isn't). Just… twisted.There will obviously be more of this, but for now, you can do your heavy metal in Dubly, you know. It will get a little lighter and spacier and more atmospheric, which is probably why this is Not Done. But who knows, maybe you'll like a little Dubly on your record. Hope you like it :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Dubly final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Dubly()
    {
        A = 0.5;
        B = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dubly"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Dubly"; break;
            case kParamB: return "Dry/Wet"; break;

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
        double dublyAmount = pow(A, 3) * 0.16;
        double outlyAmount = pow(A, 3) * 0.160618;
        double gain = outlyAmount + 1.0;
        double iirAmount = 0.4275 / overallscale;
        double wet = B;
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
            iirSampleAL = (iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            double doubly = inputSampleL - iirSampleAL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL += doubly * dublyAmount;
            iirSampleAR = (iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            doubly = inputSampleR - iirSampleAR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR += doubly * dublyAmount;
            // end Dubly encode
            inputSampleL *= gain;
            inputSampleL = sin(inputSampleL);
            inputSampleL /= gain;
            inputSampleR *= gain;
            inputSampleR = sin(inputSampleR);
            inputSampleR /= gain;
            iirSampleBL = (iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            doubly = inputSampleL - iirSampleBL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL -= doubly * outlyAmount;
            iirSampleBR = (iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            doubly = inputSampleR - iirSampleBR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR -= doubly * outlyAmount;
            // end Dubly decode
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::dubly
