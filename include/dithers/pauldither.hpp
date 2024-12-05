#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pauldither {

constexpr std::string_view k_name{ "PaulDither" };
constexpr std::string_view k_short_description{
    "PaulDither is a highpassed TPDF dither. (quieter, airier)"
};
constexpr std::string_view k_long_description{
    "As long as we’re making TPDF dithers, here’s something worth noticing, and a shout-out to a great person.In a public Facebook discussion on dither, Paul Frindle (Sony Oxford, and the DSM 2 ‘prismatic compressor’) suggested his own preferred solution, in general terms: “The one we use most is triangular single pole high pass dither. It not freq bent enough sound odd, but is slightly less audible that flat dither. It can also be easily made by taking one sample of dither away from the previous one – this gives you the triangular PDF and the filtering in one go :-) “The great thing about this is, we don’t have to get his code to be able to do that. In fact, I’m not: I’m using a sample of dither, storing it to be the previous one, then taking it away from the next sample of dither (which is backwards from what he suggests). However, the effect is the same: TPDF single pole high pass dither.The coolest thing about this is, it’s actually twice as CPU efficient as normal TPDF! You store a dither sample (random generation is a pretty CPU-hungry process when done properly, and it sounds better when you don’t half-ass it) and then you use it again for the highpass! So not only is it just as good as regular TPDF, it’s cheaper to use.Thank Paul for that, not me. (though I do have some ideas about ways to tweak it: I think I can put a cancellation node right where the ear is most sensitive and make the noise ‘quieter’. Paul’s no doubt already tried this and didn’t like it as well, but hey, I’ll try it too and let you all try it, for free. Paul would know exactly what I’m proposing to do as soon as I mentioned a ‘node’, and it’s nearly as cheap to run as his highpassed dither, but not quite)So what do you get? Well, this is still a TPDF dither, so you get mathematically correct dither that doesn’t fluctuate in volume. You don’t get ‘the Sony Oxford’ dither, because I don’t have Paul’s code. But you do get the Airwindows implementation of this general concept, and I probably have it sounding pretty good in my own right.The tone is brighter because it’s highpassed. That makes it a quieter bed of noise, and there’s a sort of silky, not-harsh quality to it that’s nice. I think it does affect perceptions of brightness and the tonal quality of the mix, so it’s a choice, not ‘the automatic correct option’. It’ll give a ‘sound’, and focus your attention differently, towards detail and a subtle revoicing of the track. If you mix through it, your choices will be conditioned by this way of hearing (remember, use 24-bit dither like this when monitoring on a DAC that takes 24 bit input, and your 24 bit files will also match what you hear: putting dither only on mixdowns is silly and misleading)If I was going to use just a TPDF dither, it would be this one every time, because it’s not just a TPDF dither, it’s silky and sweet and a bit quieter than the usual kind. And just as Paul told us freely what the basic concept was, so Airwindows PaulDither is free. Thanks, Paul :)(followup: I’ve put in a switch between 24 bit and 16 bit, and added the DeRez control)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class PaulDither final : public Effect<T>
{
    double previousDitherL;
    double previousDitherR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    PaulDither()
    {
        A = 1.0;
        B = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        previousDitherL = 0.0;
        previousDitherR = 0.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "quant"; break;
            case kParamB: return "derez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Quant"; break;
            case kParamB: return "DeRez"; break;

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

        double currentDitherL;
        double currentDitherR;
        int processing = (VstInt32)(A * 1.999);
        bool highres = false;
        if (processing == 1) {
            highres = true;
        }
        float scaleFactor;
        if (highres) {
            scaleFactor = 8388608.0;
        }
        else {
            scaleFactor = 32768.0;
        }
        float derez = B;
        if (derez > 0.0) {
            scaleFactor *= pow(1.0 - derez, 6);
        }
        if (scaleFactor < 0.0001) {
            scaleFactor = 0.0001;
        }
        float outScale = scaleFactor;
        if (outScale < 8.0) {
            outScale = 8.0;
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            // 0-1 is now one bit, now we dither
            currentDitherL = (double(fpdL) / UINT32_MAX);
            currentDitherR = (double(fpdR) / UINT32_MAX);
            inputSampleL += currentDitherL;
            inputSampleR += currentDitherR;
            inputSampleL -= previousDitherL;
            inputSampleR -= previousDitherR;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            previousDitherL = currentDitherL;
            previousDitherR = currentDitherR;
            // Paul Frindle: It's true that the dither itself can sound different
            // if it's given a different freq response and you get to hear it.
            // The one we use most is triangular single pole high pass dither.
            // It not freq bent enough sound odd, but is slightly less audible than
            // flat dither. It can also be easily made by taking one sample of dither
            // away from the previous one - this gives you the triangular PDF and the
            // filtering in one go :-)
            inputSampleL /= outScale;
            inputSampleR /= outScale;
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::pauldither
