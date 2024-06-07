#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::platinumslew {

constexpr std::string_view k_name{ "PlatinumSlew" };
constexpr std::string_view k_short_description{
    "PlatinumSlew is the best easy-to-use slew clipper."
};
constexpr std::string_view k_long_description{
    "This is something I've specialized in for a while: types of processing that aren't common. Slew clipping is like what you see out of certain op-amps, circuits that can't keep up with high frequencies and are given semi-magical qualities having to do with just the right IC and so on. And you can program that, and I have (for instance my Channel plugins, or Guitar Conditioner).And then I discovered, in GoldenSlew, that you could cascade slew clippings and it'd actually give you a unique sound, unlike just using one slew clipping.And then it went beyond, as I tried to incorporate another previous sample and bring in the rate of change of the direction of the slew, and ran into strange audio bugs that were the digital equivalent of my circuits blowing up or short-circuiting, unable to handle what I was asking of them……and came out the other end with PlatinumSlew (and a plan for a more complicated, control-laden plugin where I can get really detailed control over everything that happens in the plugin).The original Slew, and GoldenSlew after it, is a bit of an audio chainsaw. For instance, if you slam it to full crank it acts as a sample and hold, because that's literally what you get when you cut slew to 0.0: permanent holding of whatever voltage you had. Good for use in the VCV Rack port, not so good for subwoofers. So, don't use those for swooping the audio down to bass and then silence, especially not fast, or you'll trap energy.PlatinumSlew explores a new space in slew clipping where its algorithms don't hold energy in the same way. But what does it sound like? It sounds like a record. It sounds retro. What happens is, you can control the transients of the high-end, but then as you push further, as it gets darker, it also begins to alter the subsonics, the bass… which is something I've never been able to do, not like this. It's not a filter, this is a clipper. But it's not clipping the bass… it's hard to explain.Crank it way up and hear the odd things it does when you push it way too far… and know that you can bring in just tiny amounts of that very unusual, very retro, very vibey flavor. This tech is central to where I'm going with the new Console designs, as I come up with stuff that really truly sounds like classic desks. It immediately found its way into my music, taking my legit analog gear that last little distance it needed as post processing. It's mastering friendly if your tweeters and ears are good enough to hear its subtlest effects. And unlike the swiss army knife version I'm still developing, this one is one slider which can't be set wrong, and it's free and open source, and it's yours. Hope you like it :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class PlatinumSlew final : public Effect<T>
{
    enum
    {
        prevSampL1,
        prevSampR1,
        invSampL1,
        invSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        invSampL2,
        invSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        invSampL3,
        invSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        invSampL4,
        invSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        invSampL5,
        invSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        invSampL6,
        invSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        invSampL7,
        invSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        invSampL8,
        invSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        invSampL9,
        invSampR9,
        threshold9,
        prevSampL10,
        prevSampR10,
        invSampL10,
        invSampR10,
        threshold10,
        gslew_total

        public :
            PlatinumSlew(){
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
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
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
            return "slew";
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
            return "Slew";
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
        overallscale *= getSampleRate();
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
            for (int x = 0; x < gslew_total; x += 5) {
                if (((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * 0.156)) + (gslew[x + 4] * 0.844);
                }
                if (-((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * 0.2)) - (gslew[x + 4] * 0.8);
                }
                gslew[x + 2] = gslew[x] * 0.844;
                gslew[x] = inputSampleL;
                if (((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * 0.156)) + (gslew[x + 4] * 0.844);
                }
                if (-((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * 0.2)) - (gslew[x + 4] * 0.8);
                }
                gslew[x + 3] = gslew[x + 1] * 0.844;
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
} // namespace airwindohhs::platinumslew
