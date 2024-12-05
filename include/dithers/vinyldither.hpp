#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::vinyldither {

constexpr std::string_view k_name{ "VinylDither" };
constexpr std::string_view k_short_description{
    "VinylDither is a high-performance dither that converts digital noise to ‘groove noise’."
};
constexpr std::string_view k_long_description{
    "To explain this dither, I’ve got to talk about SACD (DSD) a little.There’s a super-hi-fi digital format known as DSD (direct stream digital). It uses a sample rate in the megahertz, but not as many bits (as few as one, even!) and is tricky to work with. It’s a bit like having Class D amplifiers: those also use an incredibly high frequency switching system, and produce a clear, fluid sound that totally lacks some audio flaws.DSD is like that. There are some things it does incredibly well, and other things it gets wrong. Notably, it’s incredibly good at delivering accuracy on deep bass notes, but it’s all over the place on supersonic frequencies and can produce a ‘splat’ of high-frequency overreaction if you drive it too hard there. The performance of DSD increases as frequency lowers. All its ugly is reserved for the super-highs, but nothing comes close in the midrange, low mids, bass and so on. That’s fundamental to how the format works: accuracy becomes a statistical pitch-related thing and the deeper you go, the more of a lock DSD has on the sonic truth.What if it was possible to emulate this behavior in a dither? First, you’d need to use a really intense noise shaper, not just dither noise. Then, you’d have to make it have the same frequency-related thing where lows get increased accuracy. And lastly, you’d pay the same price: it’d drive the error of the system into the highs and do a lot of unpredictable, ugly things there.Meet VinylDither! It’s the extension of an old dither I had called Ten Nines, which does exactly this. With Ten Nines I was able to get a noise floor under -200db on 16 bit audio (if I remember correctly)… but only at frequencies ten hertz and below! It also spit out loudish crackling noises at high frequencies. The experiment worked, but not everybody loved it.However, if you weren’t paying close attention, the noise behavior was kind of like record surface noise, and that gave me ideas.VinylDither is the result of pursuing those ideas. It’s a dither/noise shaper which completely removes the ‘steady white noise’ noise floor of normal dithered digital, and replaces it with a more crackly, surface-noise kind of sound. That’s produced from storing up error energy that would mess up your midrange and bass, and releasing it in bursts and sparks. VinylDither is the first of three dithers I’m releasing that are professional quality, boutique dithers and sonically better than TPDF. If it’s really important to you for your noise floor to be featureless and sound like white noise down there, you won’t want this. However, if you’re an analogophile and like vinyl records, you’ll instantly recognize the ‘vibe’ of this one. It does an incredibly good job at giving ‘vinyl’ clues down at the threshold of hearing but this is not a ‘sound effect’ being added, it’s the natural digital error being rearranged to get that effect.So, if you want ‘vinyl’ effects added to your mix so it sounds like a retro vinyl record coming off the computer, this is the single subtlest way you can do that, plus you could combine it with other things (like the Audio Unit ‘ToVinyl’ which does elliptical EQ and has an amazing groove wear emulation built in) and get a completely different vibe. Wrecking your sounds with heavy overprocessing isn’t necessary! You can just pick specific things to give a more subtle vinyl feel, and VinylDither is the perfect dither choice for it. Yes, it’s got more depth and warmth and vibe than plain old TPDF (or truncation), but I think the interesting part is examining the faults of VinylDither (crackling noises, not smooth noise) and understanding how they can be turned into advantages (crackling noises OK, do not correct! As it says on my vinyl copy of Live At Leeds cooge )"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class VinylDither final : public Effect<T>
{
    double NSOddL;
    double prevL;
    double nsL[16];
    double NSOddR;
    double prevR;
    double nsR[16];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    VinylDither()
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
        NSOddL = 0.0;
        prevL = 0.0;
        nsL[0] = 0;
        nsL[1] = 0;
        nsL[2] = 0;
        nsL[3] = 0;
        nsL[4] = 0;
        nsL[5] = 0;
        nsL[6] = 0;
        nsL[7] = 0;
        nsL[8] = 0;
        nsL[9] = 0;
        nsL[10] = 0;
        nsL[11] = 0;
        nsL[12] = 0;
        nsL[13] = 0;
        nsL[14] = 0;
        nsL[15] = 0;
        NSOddR = 0.0;
        prevR = 0.0;
        nsR[0] = 0;
        nsR[1] = 0;
        nsR[2] = 0;
        nsR[3] = 0;
        nsR[4] = 0;
        nsR[5] = 0;
        nsR[6] = 0;
        nsR[7] = 0;
        nsR[8] = 0;
        nsR[9] = 0;
        nsR[10] = 0;
        nsR[11] = 0;
        nsR[12] = 0;
        nsR[13] = 0;
        nsR[14] = 0;
        nsR[15] = 0;
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

        double absSample;
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
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            // 0-1 is now one bit, now we dither
            absSample = ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[0] += absSample;
            nsL[0] /= 2;
            absSample -= nsL[0];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[1] += absSample;
            nsL[1] /= 2;
            absSample -= nsL[1];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[2] += absSample;
            nsL[2] /= 2;
            absSample -= nsL[2];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[3] += absSample;
            nsL[3] /= 2;
            absSample -= nsL[3];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[4] += absSample;
            nsL[4] /= 2;
            absSample -= nsL[4];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[5] += absSample;
            nsL[5] /= 2;
            absSample -= nsL[5];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[6] += absSample;
            nsL[6] /= 2;
            absSample -= nsL[6];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[7] += absSample;
            nsL[7] /= 2;
            absSample -= nsL[7];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[8] += absSample;
            nsL[8] /= 2;
            absSample -= nsL[8];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[9] += absSample;
            nsL[9] /= 2;
            absSample -= nsL[9];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[10] += absSample;
            nsL[10] /= 2;
            absSample -= nsL[10];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[11] += absSample;
            nsL[11] /= 2;
            absSample -= nsL[11];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[12] += absSample;
            nsL[12] /= 2;
            absSample -= nsL[12];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[13] += absSample;
            nsL[13] /= 2;
            absSample -= nsL[13];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[14] += absSample;
            nsL[14] /= 2;
            absSample -= nsL[14];
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            absSample += ((double(fpdL) / UINT32_MAX) - 0.5);
            nsL[15] += absSample;
            nsL[15] /= 2;
            absSample -= nsL[15];
            // install noise and then shape it
            absSample += inputSampleL;
            if (NSOddL > 0) {
                NSOddL -= 0.97;
            }
            if (NSOddL < 0) {
                NSOddL += 0.97;
            }
            NSOddL -= (NSOddL * NSOddL * NSOddL * 0.475);
            NSOddL += prevL;
            absSample += (NSOddL * 0.475);
            prevL = floor(absSample) - inputSampleL;
            inputSampleL = floor(absSample);
            // TenNines dither L
            absSample = ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[0] += absSample;
            nsR[0] /= 2;
            absSample -= nsR[0];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[1] += absSample;
            nsR[1] /= 2;
            absSample -= nsR[1];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[2] += absSample;
            nsR[2] /= 2;
            absSample -= nsR[2];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[3] += absSample;
            nsR[3] /= 2;
            absSample -= nsR[3];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[4] += absSample;
            nsR[4] /= 2;
            absSample -= nsR[4];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[5] += absSample;
            nsR[5] /= 2;
            absSample -= nsR[5];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[6] += absSample;
            nsR[6] /= 2;
            absSample -= nsR[6];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[7] += absSample;
            nsR[7] /= 2;
            absSample -= nsR[7];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[8] += absSample;
            nsR[8] /= 2;
            absSample -= nsR[8];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[9] += absSample;
            nsR[9] /= 2;
            absSample -= nsR[9];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[10] += absSample;
            nsR[10] /= 2;
            absSample -= nsR[10];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[11] += absSample;
            nsR[11] /= 2;
            absSample -= nsR[11];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[12] += absSample;
            nsR[12] /= 2;
            absSample -= nsR[12];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[13] += absSample;
            nsR[13] /= 2;
            absSample -= nsR[13];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[14] += absSample;
            nsR[14] /= 2;
            absSample -= nsR[14];
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            absSample += ((double(fpdR) / UINT32_MAX) - 0.5);
            nsR[15] += absSample;
            nsR[15] /= 2;
            absSample -= nsR[15];
            // install noise and then shape it
            absSample += inputSampleR;
            if (NSOddR > 0) {
                NSOddR -= 0.97;
            }
            if (NSOddR < 0) {
                NSOddR += 0.97;
            }
            NSOddR -= (NSOddR * NSOddR * NSOddR * 0.475);
            NSOddR += prevR;
            absSample += (NSOddR * 0.475);
            prevR = floor(absSample) - inputSampleR;
            inputSampleR = floor(absSample);
            // TenNines dither R
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
} // namespace airwindohhs::vinyldither
