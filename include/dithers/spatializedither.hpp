#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::spatializedither {

constexpr std::string_view k_name{ "SpatializeDither" };
constexpr std::string_view k_short_description{
    "SpatializeDither is a high-performance clarity and accuracy dither."
};
constexpr std::string_view k_long_description{
    "Spatialize is one of my high-performance boutique dithers. Though I think my Naturalize beats it, that’s a preference: I like digital sound to sound organic and real, and I like ambience and foggy distance and other things Naturalize is great at emphasizing.Spatialize is also good at reverb depths and softness, but what it excels at is focus.This modified dither algorithm has opinions about what ought to be randomized. Any normal dither (especially a technically correct TPDF-based one, such as PaulDither, TapeDither or NodeDither that can encompass either) has no preferences about what samples it gets. It will apply noise regardless, with perfect impartiality.Spatialize (which I’ve also termed Contingent Dither, early in its development) isn’t like that. It says, ‘hey, this sample is exactly on a quantization value. No way am I going to mess that up, it’s staying right where it is!’. Or, it says ‘this sample is exactly between two quantization values. If I rapidly flip between adjacent values I can try to get the DAC to produce output between them. What could go wrong?’. Or, it says ‘this sample is none of the above, let’s bring in some randomness and apply dither like some normal plugin that isn’t crazy, would do’.Or all of the above, blended…That’s how Spatialize works. These are pretty bold things to try to do, especially the attempt to balance between two quantization values: that’s not really a reasonable thing to try, even when blended with random noise. And it pays something of a price: while Spatialize is quiet in its noise generation even without resorting to noise shaping, its behavior down around the noise floor isn’t perfectly well-behaved.But that’s a trade-off, because by sacrificing this good behavior, Spatialize gets to be very sure that when samples hit perfectly on quantization boundaries, they’ll be accurately represented. And the bit-flippiness of the exactly-between behavior gives rise to a really strong highpassy effect that heightens treble energy. The result is a dither with a holographic, intense sonic reality to it: and it IS reality, because it comes out of this determination to honor the true values of the samples wherever possible. Spatialize is always prepared to abandon ‘appropriate’ noise floor behavior if it can nail down the sonic envelope with more ruthless accuracy.So, if you’re into the hyper-real, high-definition sound of extreme clarity and accuracy, Spatialize might be your preferred Airwindows boutique dither. And, since it does it all with no noise shaping, there’s still an ease and naturalness to the resulting sound. It’s probably my own favorite apart from Naturalize, and for some things I prefer it. And, it’s free, AU and VST. :)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class SpatializeDither final : public Effect<T>
{
    double contingentErrL;
    double contingentErrR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    SpatializeDither()
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
        contingentErrL = 0.0;
        contingentErrR = 0.0;
        flip = false;
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

        double contingentRnd;
        double absSample;
        double contingent;
        double randyConstant = 1.61803398874989484820458683436563811772030917980576;
        double omegaConstant = 0.56714329040978387299996866221035554975381578718651;
        double expConstant = 0.06598803584531253707679018759684642493857704825279;
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
            if (inputSampleL > 0) {
                inputSampleL += 0.383;
            }
            if (inputSampleL < 0) {
                inputSampleL -= 0.383;
            }
            if (inputSampleR > 0) {
                inputSampleR += 0.383;
            }
            if (inputSampleR < 0) {
                inputSampleR -= 0.383;
            }
            // adjusting to permit more information drug outta the noisefloor
            contingentRnd = (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            contingentRnd += ((double(fpdL) / UINT32_MAX) - 1.0);
            contingentRnd *= randyConstant; // produce TPDF dist, scale
            contingentRnd -= contingentErrL * omegaConstant; // include err
            absSample = fabs(inputSampleL);
            contingentErrL = absSample - floor(absSample); // get next err
            contingent = contingentErrL * 2.0; // scale of quantization levels
            if (contingent > 1.0) {
                contingent = ((-contingent + 2.0) * omegaConstant) + expConstant;
            }
            else {
                contingent = (contingent * omegaConstant) + expConstant;
            }
            // zero is next to a quantization level, one is exactly between them
            if (flip) {
                contingentRnd = (contingentRnd * (1.0 - contingent)) + contingent + 0.5;
            }
            else {
                contingentRnd = (contingentRnd * (1.0 - contingent)) - contingent + 0.5;
            }
            inputSampleL += (contingentRnd * contingent);
            // Contingent Dither
            inputSampleL = floor(inputSampleL);
            contingentRnd = (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            contingentRnd += ((double(fpdR) / UINT32_MAX) - 1.0);
            contingentRnd *= randyConstant; // produce TPDF dist, scale
            contingentRnd -= contingentErrR * omegaConstant; // include err
            absSample = fabs(inputSampleR);
            contingentErrR = absSample - floor(absSample); // get next err
            contingent = contingentErrR * 2.0; // scale of quantization levels
            if (contingent > 1.0) {
                contingent = ((-contingent + 2.0) * omegaConstant) + expConstant;
            }
            else {
                contingent = (contingent * omegaConstant) + expConstant;
            }
            // zero is next to a quantization level, one is exactly between them
            if (flip) {
                contingentRnd = (contingentRnd * (1.0 - contingent)) + contingent + 0.5;
            }
            else {
                contingentRnd = (contingentRnd * (1.0 - contingent)) - contingent + 0.5;
            }
            inputSampleR += (contingentRnd * contingent);
            // Contingent Dither
            inputSampleR = floor(inputSampleR);
            // note: this does not dither for values exactly the same as 16 bit values-
            // which forces the dither to gate at 0.0. It goes to digital black,
            // and does a teeny parallel-compression thing when almost at digital black.
            flip = !flip;
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
} // namespace airwindohhs::spatializedither
