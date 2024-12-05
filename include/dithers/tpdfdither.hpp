#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tpdfdither {

constexpr std::string_view k_name{ "TPDFDither" };
constexpr std::string_view k_short_description{
    "TPDFDither is TPDF dither. With 16/24 bit output and a DeRez control."
};
constexpr std::string_view k_long_description{
    "TPDF is the industry standard technically correct boring dither. It does nothing strange or interesting, makes no effort to optimize the sound in any way (generally if you make it better for something it’s worse for something else: even Naturalize only has a limited amount of output bits to work with.)TPDF uses two sources of noise to make what’s called a Triangular Probability Density Function, which gives it its name. If you had only one noise source, you’d get what’s called flat dither (I’m not offering that, but it’s in Ditherbox, along with truncation.) When you only have one noise source (at the correct volume, which is one bit wide) you get dithering and sounds correctly transition into silence instead of going insane with grating gnarlyness (which is what happens with truncation, anywhere and everywhere it happens) but with only the one noise source you get an effect where the noise level flutters and shifts depending on what the audio’s doing. I demonstrate this in the video, it’s quite noticeable.With the two noise sources, TPDF transitions sounds into silence, and still keeps a totally unvarying noise profile. In a very real way, this is more analog-like. All analog circuits get some noise, and all analog circuits let sounds drop beneath that noise exactly like TPDF dither does.That’s not to say it’s the only choice you can have for dithering: I’ll be offering up different ways to dither for weeks, each with their own interesting sound signature. I’m just saying, for what it is, TPDF dither works extremely well. If you don’t have good reasons to use something fancier, or you want something guaranteed to work on everything in any situation, TPDF dither’s the one for you. Airwindows TPDF dither is a nice high-resolution well-coded implementation, one that does the truncating for you so you can compare it (for instance, with BitShiftGain like in the video) but it’s also exactly the same as any other properly done TPDF dither out there. There’s no fancy tricks, it’s just the boring but useful TPDF dither.I guess there are a few Airwindowsy things about it but they’re not sound quality related. If you’re using the AU on Macs, it’s ‘N to N’ meaning it works on quad and 5.1 channels automatically, and is more efficient on mono channels. And just like all the Airwindows dithers that are coming out, it is 24 bit only and has no controls. That means if you want 16 bit you could get the AU Ditherbox, wait for it to be ported to VST, or use two copies of BitShiftGain that I just released. (I’m discouraging emphasis on 16 bit because I think it’s obsolete and should be deprecated.) This also means if you’re using TPDFDither as intended, it’s a ‘non-fiddly’ plugin that won’t distract you. There’s no window, no GUI, no reason ever to open it in the DAW: if you’ve placed it in the correct spot you can see it there (clearly marked TPDFDither) and that’s all you need to know.Making plugins that simple and self-effacing is a very ‘Patreon-supported‘ thing to do. When you have to sell plugins to stay alive as a company, you’ve got to keep them in your users’ faces and distract people to make them think about you, lest you be forgotten. This competes with the creative urge and gets in the way. Making plugins that are ruthlessly minimalistic to the point of being almost not there, is the opposite. Your music has to be the focus, so the plugin has to have the goods sonically but also be non-fiddly, because it’s sort of ugly and boring and not fun to play with.I continue to delight in the latter. Let your music be the focus. Hope you enjoy TPDF Dither, and expect a lot more plugins of this nature, as fast as I can reasonably release them :)This got a Redux edition, meaning some added controls. Not only 16/24 bit TPDF with a DeRez control (and a clamp for how loud it gets so it doesn’t blow up your ears at super low bit depths), it’s also the Airwindows TPDF algorithm. Which is to say it’s not exactly like everyone else’s TPDF algorithm, even though it adds exactly two noise sources each one bit in loudness.That’s because when I made it, I added one and then subtracted the other and thought that gave me zero. But of course, it doesn’t… it gives me TPDF (triangular probability density function, or ‘the correct kind of dither when done by-the-book’ which decorrelates two moments of quantization noise blah blah blah technical :) )But it also gives exactly half a bit, on average, of DC offset. This is enough to make measurements weird. It’s not enough to do any harm to anything (less than a least significant bit: it’s literally in the noise floor). But it is enough to have the noise floor sit across THREE bit values instead of only two. And that is enough to make it sound nice. (it’s possible there are other values, like 0.382… that would sound better still, but I’ve not done that experiment yet, and it’d be more complicated to implement… guess I know what I’ll be doing NEXT week)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class TPDFDither final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    TPDFDither()
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
            inputSampleL -= 1.0;
            inputSampleR -= 1.0;
            inputSampleL += (double(fpdL) / UINT32_MAX);
            inputSampleR += (double(fpdR) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL += (double(fpdL) / UINT32_MAX);
            inputSampleR += (double(fpdR) / UINT32_MAX);
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            // TPDF: two 0-1 random noises
            inputSampleL /= outScale;
            inputSampleR /= outScale;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::tpdfdither
