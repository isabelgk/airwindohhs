#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::lrconvolve2 {

constexpr std::string_view k_name{ "LRConvolve2" };
constexpr std::string_view k_short_description{
    "LRConvolve2 multiplies each channel by the other, plus Soar!"
};
constexpr std::string_view k_long_description{
    "We've had this (along with its strange demonstration audio) and you've already seen this 'Soar' control on the recent 'SquareRoot' plugin, which is going to be a lot more generally useful.But have you heard the essence of the Soar control turned into a plugin, to most drastically demonstrate what it does?Nope, because SquareRoot typically sounds fine however you set it. But now you can!I don't remove plugins that work, so this doesn't replace LRConvolve (I'm not sure who would care if it did, but it's the principle of the thing). But you can get the LRConvolve sound out of it easily enough by turning Soar up ALL the way. So there's a start: the 'crazy' can be dialed back through turning down the Soar control. Both channels still completely convolve and phase-flip each other, and it still results in an intense mess of audio.But when you run more typical sounds against each other (rather than using something predictable like a full-amplitude sine), suddenly a whole new behavior emerges. Turns out, if you reduce Soar on this algorithm and have two sounds convolving each other, there's a full range of behavior between 'mostly choked out' and 'hypercompressed and exaggerated'! This will also apply to some extent to SquareRoot, except that LRConvolve2 highlights the behavior because more sounds will be forced into the 'quiet' zone where Soar operates.For that reason, it might come in handy just sussing out how the algorithm works as it's used in SquareRoot: or, using it like the original LRConvolve but with additional tonal control that's unique to what that plugin is. I'm working on other plugins such as the Bezier-curve compressor for ConsoleH, plus a wild project (sort of an art project) around making literal buotique stompboxes, so if this isn't the plugin for you, wait a week and I'll make another :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class LRConvolve2 final : public Effect<T>
{
    float A;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    LRConvolve2()
    {
        A = 0.0;
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
            case kParamA: return "soar"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Soar"; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double soar = 0.3 - (A * 0.3);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // blame Jannik Asfaig (BoyXx76) for this (and me) :D
            double out = 0.0;
            double inL = fabs(inputSampleL) + (soar * soar);
            double inR = fabs(inputSampleR) + (soar * soar);
            if (inputSampleL > 0.0 && inputSampleR > 0.0) {
                out = fmax((sqrt(inR / inL) * inL) - soar, 0.0);
            }
            if (inputSampleL < 0.0 && inputSampleR > 0.0) {
                out = fmin((-sqrt(inR / inL) * inL) + soar, 0.0);
            }
            if (inputSampleL > 0.0 && inputSampleR < 0.0) {
                out = fmin((-sqrt(inR / inL) * inL) + soar, 0.0);
            }
            if (inputSampleL < 0.0 && inputSampleR < 0.0) {
                out = fmax((sqrt(inR / inL) * inL) - soar, 0.0);
            }
            inputSampleL = inputSampleR = out;
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
} // namespace airwindohhs::lrconvolve2
