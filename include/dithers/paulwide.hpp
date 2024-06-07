#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::paulwide {

constexpr std::string_view k_name{ "PaulWide" };
constexpr std::string_view k_short_description{
    "PaulWide is a highpassed TPDF wide dither. (quieter, airier AND wider)"
};
constexpr std::string_view k_long_description{
    "Return Of The Son Of Monster Dither :DSo if you tuned in last week for TPDFWide you probably thought that was all I had, in the dither tank, for now.But guess what? Happy accidents occurred. I thought it might be fun to try this same principle not on TPDFDither, but on PaulDither.PaulDither is of course a simple highpassed dither. It does the Airwindows-style very tiny offset to make the dither noisefloor sit across one extra possible value, and now PaulWide is the same thing except it rerolls the randomness if it’s going to seem too much like mono. It’s still TPDF, it’s still just random one bit noise sources, it’s still technically correct as far as dithering accurately (some might freak out at the offset, but it’s on purpose and WAY too quiet to hurt you, as it’s less than one significant bit), but now it’s also widened while remaining purely TPDF in nature.So this is your ‘hot-rod’ industry-standard dither. If you need TPDF and fancy at the same time, here you go. It is silky in texture just like Paul Frindle’s preferred dithers because it’s the highpass dither, it’s slightly richer in texture because it’s offset a teeny amount like Airwindows TPDF, and then it’s widened through discarding random values that lead to ‘mono-like’ results: except it turns out, when you apply that to a highpassed dither, you get MUCH MORE wideness than you do with the regular highpass.Enjoy the most boutique, decked-out, hot-rod ‘regular ordinary dither’ you’re ever likely to see. I’m not sure what else can be even done here without ceasing to be TPDF anymore."
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class PaulWide final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double previousDitherL;
    double previousDitherR;
    float A;
    float B;

  public:
    PaulWide()
    {
        A = 1.0;
        B = 0.0;
        previousDitherL = 0.0;
        previousDitherR = 0.0;
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
            // Paul Frindle: It's true that the dither itself can sound different
            // if it's given a different freq response and you get to hear it.
            // The one we use most is triangular single pole high pass dither.
            // It not freq bent enough sound odd, but is slightly less audible than
            // flat dither. It can also be easily made by taking one sample of dither
            // away from the previous one - this gives you the triangular PDF and the
            // filtering in one go :-)
            double currentDither = (double(fpdL) / UINT32_MAX);
            double ditherL = currentDither;
            ditherL -= previousDitherL;
            previousDitherL = currentDither;
            // TPDF: two 0-1 random noises
            currentDither = (double(fpdR) / UINT32_MAX);
            double ditherR = currentDither;
            ditherR -= previousDitherR;
            previousDitherR = currentDither;
            // TPDF: two 0-1 random noises
            if (fabs(ditherL - ditherR) < 0.5) {
                fpdL ^= fpdL << 13;
                fpdL ^= fpdL >> 17;
                fpdL ^= fpdL << 5;
                currentDither = (double(fpdL) / UINT32_MAX);
                ditherL = currentDither;
                ditherL -= previousDitherL;
                previousDitherL = currentDither;
            }
            if (fabs(ditherL - ditherR) < 0.5) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
                currentDither = (double(fpdR) / UINT32_MAX);
                ditherR = currentDither;
                ditherR -= previousDitherR;
                previousDitherR = currentDither;
            }
            if (fabs(ditherL - ditherR) < 0.5) {
                fpdL ^= fpdL << 13;
                fpdL ^= fpdL >> 17;
                fpdL ^= fpdL << 5;
                currentDither = (double(fpdL) / UINT32_MAX);
                ditherL = currentDither;
                ditherL -= previousDitherL;
                previousDitherL = currentDither;
            }
            inputSampleL = floor(inputSampleL + ditherL);
            inputSampleR = floor(inputSampleR + ditherR);
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::paulwide
