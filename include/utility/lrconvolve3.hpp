#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::lrconvolve3 {

constexpr std::string_view k_name{ "LRConvolve3" };
constexpr std::string_view k_short_description{
    "LRConvolve3 gates one channel by the other, roughly or cleanly."
};
constexpr std::string_view k_long_description{
    "So the LRConvolve plugins are all audio manglers, capable of doing awful things to a sound, or turning an innocent voice into the Swedish Chef just by inverting that voice at audio rates. It's inherent to the convolution that you can flip the phase of a sound with another sound, and this is where things get most gnarly, even if you're convolving stuff with simple sine tones.But what if you wanted to do that, but cleanly?Well, there's degrees of 'cleanly', but LRConvolve3 exists to sit alongside the other two, looking cute, for when the full convolution is just too intense. And it does just that: the same, but 'cleanly'. How? You pick one of the sides, and full-wave rectify it. There's another control simply named 'smooth' (not the same as the Bezier control-smoothers I've been coming up with, this is from the same time I was doing the original LRConvolve) and all it does is add a time constant, turning your sound from full-wave rectified into basically an envelope that decays.And that's all you need to get the effect of 'one side turns into the gate for the other', either responding very crisply with Smooth turned all the way down, or using Smooth to make it a trigger input for the gating of the other. I demo it on a guitar taking the shape of drums, and then on a racecar fly-by also taking the shape of drums. You'll probably look to more percussive sounds to supply the gating, but you can do whatever you like: for instance, do something extra through passing a sustained chord on the 'sound' side, and then routing a bass part or whatever into the gate, and you've got some extra overtones that are attached to the bass part and follow its attacks and dynamics. Or a drum spot mic, or a literal timing click: if you have to stretch a too-brief sound, that's what Smooth is for.While I work on more glamorous and dramatic plugins, it's nice to drop a little widget that just does what it's supposed to. Hope if it ever finds a need, it comes in handy. Like many utilities, LRConvolve3 could find many surprising uses, and it's nice how it ties its effects so completely to other sounds you have around. That helps make each use unique :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class LRConvolve3 final : public Effect<T>
{
    float A;
    float B;
    double iirSample;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    LRConvolve3()
    {
        A = 0.0;
        B = 0.0;
        iirSample = 0.0;
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
            case kParamA: return "smooth"; break;
            case kParamB: return "channel"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Smooth"; break;
            case kParamB: return "Channel"; break;

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
        double smooth = pow(A, 4) * (0.5 / overallscale);
        double channel = B;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double carrier = inputSampleL;
            double modulate = fabs(inputSampleR);
            if (channel > 0.5) {
                carrier = inputSampleR;
                modulate = fabs(inputSampleL);
            }
            if (iirSample < modulate) {
                iirSample = modulate;
            }
            modulate = (iirSample * smooth) + (modulate * (1.0 - smooth));
            if (carrier > 0.0 && modulate > 0.0) {
                carrier = sqrt(carrier / modulate) * modulate;
            }
            if (carrier < 0.0 && modulate > 0.0) {
                carrier = -sqrt(-carrier / modulate) * modulate;
            }
            inputSampleL = inputSampleR = carrier;
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::lrconvolve3
