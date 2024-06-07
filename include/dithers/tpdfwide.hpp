#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tpdfwide {

constexpr std::string_view k_name{ "TPDFWide" };
constexpr std::string_view k_short_description{
    "TPDFWide is still TPDF dither but better and wider. With 16/24 bit output and a DeRez control."
};
constexpr std::string_view k_long_description{
    "TPDF is Triangular Probability Density Function. It’s about using two random sources of EXACTLY the right amplitude (one Least Significant Bit each) to render anything else, including the noise floor, free of quantization distortion so you can hear deeply INTO the noise floor and have it sound musical. Dither is important, and TPDF is the industry standard.Me being Chris from Airwindows, I always manage to find a way to tweak things just a bit, and in my TPDF dither the randomness contrives to be added to the audio in such a way that it ‘leans’ a bit to one side of the waveform, while still being purely random noise sources of exactly the right amplitude. This causes the ultimate noise floor to be distributed over slightly more values than you’d get if it was perfectly lined up, at the expense of a teeny-tiny DC offset down about 12 db under the noise floor.Turns out there’s more that can be done. If you have purely random sources, then your stereo channels will sometimes end up at the same random values. This is similar to the dither being in mono, which is known to narrow soundstage and hurt the sensation of wideness. But what if you could avoid that?TPDFWide is purely random-source Triangular Probability Density Function dither, but once it’s rolled up its random numbers, it checks to see if the two channels came out the same (or close to the same). If they are… it rerolls a channel. If they still are… it rerolls the other channel. If they still are, it rerolls the first channel again, and then gives up so as not to hurt itself. In every case these are still random numbers: we’re just rejecting ones where they’re too mono.And that gives us a ‘regulation’ TPDF dither that sounds better AND wider than generic, ordinary TPDF dithers. Firstly it covers just a slightly larger range at the noise floor, without actually being any louder. And secondly, it resists ‘mono dither’ which causes it to sound wider and more spatial than ordinary TPDF dithers. And yet it’s still a TPDF dither, at heart. If you would like something a little special, but don’t like my more unusual wordlength reduction techniques like NJAD or Spatialize or Dark or Beam etc. then you might enjoy TPDFWide. It has a ‘DeRez’ control that lets you go to low-bit realms and hear what the noise floor really sounds like, and it’s got 24 and 16 bit modes when you have DeRez set to 0 (which you should, for normal operation). You can use it as a 24 bit dither for monitoring and file-making, and as a 16 bit dither when you need to export to 16 bit. I hope you like it :)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class TPDFWide final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    TPDFWide()
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
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            // 0-1 is now one bit, now we dither
            double ditherL = -1.0;
            ditherL += (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            ditherL += (double(fpdL) / UINT32_MAX);
            // TPDF: two 0-1 random noises
            double ditherR = -1.0;
            ditherR += (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            ditherR += (double(fpdR) / UINT32_MAX);
            // TPDF: two 0-1 random noises
            if (fabs(ditherL - ditherR) < 0.5) {
                ditherL = -1.0;
                ditherL += (double(fpdL) / UINT32_MAX);
                fpdL ^= fpdL << 13;
                fpdL ^= fpdL >> 17;
                fpdL ^= fpdL << 5;
                ditherL += (double(fpdL) / UINT32_MAX);
            }
            if (fabs(ditherL - ditherR) < 0.5) {
                ditherR = -1.0;
                ditherR += (double(fpdR) / UINT32_MAX);
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
                ditherR += (double(fpdR) / UINT32_MAX);
            }
            if (fabs(ditherL - ditherR) < 0.5) {
                ditherL = -1.0;
                ditherL += (double(fpdL) / UINT32_MAX);
                fpdL ^= fpdL << 13;
                fpdL ^= fpdL >> 17;
                fpdL ^= fpdL << 5;
                ditherL += (double(fpdL) / UINT32_MAX);
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
} // namespace airwindohhs::tpdfwide
