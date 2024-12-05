#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapedust {

constexpr std::string_view k_name{ "TapeDust" };
constexpr std::string_view k_short_description{
    "TapeDust is just a special treble-erode noise, a ‘slew noise’ plugin."
};
constexpr std::string_view k_long_description{
    "Sometimes it’s good to have just a little specialty plugin that does a useful thing. Hard to do that in the commercial plugin biz, where everything has to be the biggest hype to date: but hey! Thanks to Patreon I’m free of all that, and can follow my vision.It helps that I put out a lot of plugins: it’d be weird to do this as my only plugin for the month. But, while I work on Desk4 and StarChild from the greatest-hits list, I thought I’d sprinkle a little TapeDust for you.This is slightly different from the tape noise in Iron Oxide, though it’s the same general principle. It is a slew noise plugin. What that means is, the noise ONLY hits high frequencies or anywhere the signal’s moving rapidly (there’s a teeny bit of other noise added at high settings, but it’s mostly that).Note the ‘or anywhere the signal’s moving rapidly’. This isn’t a crossover. If you put in a sine wave that’s low and loud, you’ll get very obvious noise only as it crosses through zero, and that’ll sound odd. In general, cranking this up is weird. It’ll depend hugely on what kind of signal you’ve got… but that’s the beauty of it if you can master where to use this plugin.Basses? Probably no way. Full mix? Getouttahere, no chance outside very low settings. Guitar? Hmmm. Drums, loops? A pattern emerges.TapeDust can convert ugly sharp treble attacks on pointy percussive atonal sounds to pretty much any degree of dense, noisy, natural-sounding crunch. It’s a type of noise, so it also gives analog-style variance to repeated samples that might sound over-digital. And the less tonal, or the less ‘pure clear note’ the signal is, the more TapeDust you can get away with. Since it’s a slew noise, it hits the treble of your signal HARD, but since it’s a noise, it’s not filtering or softening the sound as much as it’s just eroding it, weathering it, making it more natural. Anywhere you’ve got bright highs on a nonpure sound, you can grind them off with TapeDust.Of course, if you’re cool with using super-low settings, you can do that anywhere: it’s just important to register that this very specialized and dedicated tool is super picky about what it likes to work on. It’s a beautiful example of taking your production skills deeper: use something that can sound horrible and wrong, and find places where it’s in its element. You can do outlandish textural things, taking something like a clean electro mix with deep clean bass, and sticking heavy TapeDust on just one element in the mix to contrast with the un-grungy elements. I hope you like TapeDust. It’s the kind of plugin I love to make.(note: there has been a bug in this plugin causing it to sound different than intended)"
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class TapeDust final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    bool fpFlip;
    // default stuff
    double bL[11];
    double fL[11];
    double bR[11];
    double fR[11];
    float A;
    float B;

  public:
    TapeDust()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            fL[count] = 0.0;
            bR[count] = 0.0;
            fR[count] = 0.0;
        }
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        fpFlip = true;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dust"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Dust"; break;
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

        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double rRange = pow(A, 2) * 5.0;
        double xfuzz = rRange * 0.002;
        double rOffset = (rRange * 0.4) + 1.0;
        double rDepthL; // the randomly fluctuating value
        double rDepthR; // the randomly fluctuating value
        double gainL;
        double gainR;
        double wet = B;
        // removed extra dry variable
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            for (int count = 9; count < 0; count--) {
                bL[count + 1] = bL[count];
                bR[count + 1] = bR[count];
            }
            bL[0] = inputSampleL;
            bR[0] = inputSampleR;
            inputSampleL = rand() / (double)RAND_MAX;
            inputSampleR = rand() / (double)RAND_MAX;
            gainL = rDepthL = (inputSampleL * rRange) + rOffset;
            gainR = rDepthR = (inputSampleR * rRange) + rOffset;
            inputSampleL *= ((1.0 - fabs(bL[0] - bL[1])) * xfuzz);
            inputSampleR *= ((1.0 - fabs(bR[0] - bR[1])) * xfuzz);
            if (fpFlip) {
                inputSampleL = -inputSampleL;
                inputSampleR = -inputSampleR;
            }
            fpFlip = !fpFlip;
            for (int count = 0; count < 9; count++) {
                if (gainL > 1.0) {
                    fL[count] = 1.0;
                    gainL -= 1.0;
                }
                else {
                    fL[count] = gainL;
                    gainL = 0.0;
                }
                if (gainR > 1.0) {
                    fR[count] = 1.0;
                    gainR -= 1.0;
                }
                else {
                    fR[count] = gainR;
                    gainR = 0.0;
                }
                fL[count] /= rDepthL;
                fR[count] /= rDepthR;
                inputSampleL += (bL[count] * fL[count]);
                inputSampleR += (bR[count] * fR[count]);
            }
            if (wet < 1.0) {
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::tapedust
