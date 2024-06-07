#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::goldenslew {

constexpr std::string_view k_name{ "GoldenSlew" };
constexpr std::string_view k_short_description{
    "GoldenSlew is a multistage slew clipper based on the Golden Ratio."
};
constexpr std::string_view k_long_description{
    "Slew clipping is a fairly uncommon effect: it takes the bright off sounds, but replaces it with a distorty edge. You've already got Slew to do slew clipping with, and if you like Channel part of what you like is its built-in slew clipping… but what if you could expand on what slew clipping does?GoldenSlew is a chain of slew clippers becoming increasingly restrictive as they go, each one scaled by a factor related to the Golden Ratio (if it was applied to the 'slew' control). You would think this would produce a simple result: the sound would always be restricted to the smallest amount in the chain, and it'd sound just like Slew.Except it SO doesn't… because each new value's working from what the previous one was clipped to. And so, when you apply slew clipping as a chain, you get a completely different effect. And what it sounds like… is distant, roaring, big, loud, powerful in a way you just don't get from slew clipping alone.Be warned: for this plugin, like Slew, if you have audio going through it and you crank the slew clipping ALL the way to 1.0, what you're telling it is 'stop slewing completely, don't budge' which means sample and hold which means you're stuck on a DC voltage. There are reasons why you might do this, for instance if you're in VCV Rack and using it on a control voltage and you want to do sample and hold on that voltage. Don't crank it up to 1.0 on audio signals or you might hurt your speakers if they're DC coupled.This is one of a series of plugins starting with Slew and continuing with GoldenSlew, where they're refinements of a tone I like using for analog emulation. When used for that, you typically want to keep the setting fairly low, less than 0.5 certainly. It's for controlling the digital-ness of the highs without apparently making them quieter: remember this is a clipper, not an EQ, and it'll kick in only on LOUD treble. There will be more explorations of this concept because I'll need a really good test-bed to use for when I start dialing in sounds of actual classic analog consoles: it's not just the maximum restriction of slew (like in the Channel plugins), it's about the sound character as we hit that limit.You can use GoldenSlew on things like drums, final mixes: anywhere you want the effect of extreme loudness, but don't want the treble coming forward and poking at you. Hope you like GoldenSlew!"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class GoldenSlew final : public Effect<T>
{
    enum
    {
        prevSampL1,
        prevSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        threshold9,
        prevSampL10,
        prevSampR10,
        threshold10,
        gslew_total

        public :
            GoldenSlew(){
                A = 0.0;
                for (int x = 0; x < gslew_total; x++) gslew[x] = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
                // this is reset: values being initialized only once. Startup values, whatever they are.

            }

    enum params
    {
        kParamA = 0,
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "slew"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Slew"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
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
        double source = pow(1 - A, 4) / overallscale;
        gslew[threshold10] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold9] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold8] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold7] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold6] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold5] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold4] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold3] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold2] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold1] = source;
        source *= 1.618033988749894848204586;
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
            for (int x = 0; x < gslew_total; x += 3) {
                if ((inputSampleL - gslew[x]) > gslew[x + 2]) {
                    inputSampleL = gslew[x] + gslew[x + 2];
                }
                if (-(inputSampleL - gslew[x]) > gslew[x + 2]) {
                    inputSampleL = gslew[x] - gslew[x + 2];
                }
                gslew[x] = inputSampleL;
                if ((inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                    inputSampleR = gslew[x + 1] + gslew[x + 2];
                }
                if (-(inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                    inputSampleR = gslew[x + 1] - gslew[x + 2];
                }
                gslew[x + 1] = inputSampleR;
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
} // namespace airwindohhs::goldenslew
