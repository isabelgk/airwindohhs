#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::podcast {

constexpr std::string_view k_name{ "Podcast" };
constexpr std::string_view k_short_description{
    "PodcastDeluxe is a pile of compressors (curve style) and phase rotators."
};
constexpr std::string_view k_long_description{
    "Podcast is the same technique as PodcastDeluxe, multiple compressors that are the precursor to curve and Recurve, but simplified: without the phase rotators, without the full-on attempt to do ‘radio broadcast’ style tricks.This also means Podcast can have a dry/wet control, because there aren’t any phase rotations delaying things. In fact, Podcast runs no latency and can work very well as a ‘glue’ style buss compressor… so long as you don’t want ‘pumping and swelling’ effects, or sidechainy whooshes of level. That’s because Podcast is still in the curve school of compressors and quits changing levels if the input goes silent.It also hard-clips the output, making it a kind of ‘safety compressor’: though it’s not clean like a limiter, it’ll strike a balance between dynamics processor and distortion device. You can use it on drums and things meant to be aggressively smashed, or turn it way down and use it as a clean buss comp.Treat this as another flavor of compressor from Airwindows, and see if it finds uses for you. The greater simplicity of Podcast (much like PodcastDeluxe, heavily updated from the original versions) makes it more adaptable to different purposes, while it remains simple and un-fiddly, delivering its effects with whatever intensity and blend you like. :)PodcastDeluxe is the precursor to curve, along with its counterpart Podcast. Both of them have five curve-like compressors in series (slightly less refined), but PodcastDeluxe has five phase rotators and an ADClip-style output stage. The idea is that it would be radio station style processing. The reason it didn’t show up sooner is that it didn’t really work to my satisfaction. It’s still not perfect, but it’s different! I demonstrate it on house-type music, and on raw drums. It’s not really clean, not really dirty, not really squish-capable thanks to the curve-style compression (even five stages doesn’t give you ‘compression pumping’) but it’ll give a perhaps interesting, definitely processed-sounding effect.Maybe you’ll like it on a mix because you’re not fussy about distorting, maybe you’ll find some useful place for it elsewhere. I think it’s got a knack for high-impact drum busses without distorting them too obviously. At any rate, there’s nothing quite like it. Enjoy!"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Podcast final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double c1L;
    double c2L;
    double c3L;
    double c4L;
    double c5L;
    // the compressor
    double c1R;
    double c2R;
    double c3R;
    double c4R;
    double c5R;
    // the compressor
    float A;
    float B;

  public:
    Podcast()
    {
        A = 0.5;
        B = 1.0;
        c1L = 2.0;
        c2L = 2.0;
        c3L = 2.0;
        c4L = 2.0;
        c5L = 2.0; // startup comp gains
        c1R = 2.0;
        c2R = 2.0;
        c3R = 2.0;
        c4R = 2.0;
        c5R = 2.0; // startup comp gains
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
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "boost";
            case kParamB: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Boost";
            case kParamB: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
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
        double compress = 1.0 + pow(A, 2);
        double wet = B;
        double speed1 = 64.0 / pow(compress, 2);
        speed1 *= overallscale;
        double speed2 = speed1 * 1.4;
        double speed3 = speed2 * 1.5;
        double speed4 = speed3 * 1.6;
        double speed5 = speed4 * 1.7;
        double trigger;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            inputSampleL *= c1L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1L += trigger / speed5;
            if (c1L > compress) {
                c1L = compress;
            }
            // compress stage
            inputSampleR *= c1R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1R += trigger / speed5;
            if (c1R > compress) {
                c1R = compress;
            }
            // compress stage
            inputSampleL *= c2L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2L += trigger / speed4;
            if (c2L > compress) {
                c2L = compress;
            }
            // compress stage
            inputSampleR *= c2R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2R += trigger / speed4;
            if (c2R > compress) {
                c2R = compress;
            }
            // compress stage
            inputSampleL *= c3L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3L += trigger / speed3;
            if (c3L > compress) {
                c3L = compress;
            }
            // compress stage
            inputSampleR *= c3R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3R += trigger / speed3;
            if (c3R > compress) {
                c3R = compress;
            }
            // compress stage
            inputSampleL *= c4L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4L += trigger / speed2;
            if (c4L > compress) {
                c4L = compress;
            }
            // compress stage
            inputSampleR *= c4R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4R += trigger / speed2;
            if (c4R > compress) {
                c4R = compress;
            }
            // compress stage
            inputSampleL *= c5L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5L += trigger / speed1;
            if (c5L > compress) {
                c5L = compress;
            }
            // compress stage
            inputSampleR *= c5R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5R += trigger / speed1;
            if (c5R > compress) {
                c5R = compress;
            }
            // compress stage
            if (compress > 1.0) {
                inputSampleL /= compress;
                inputSampleR /= compress;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            if (inputSampleL > 0.999) {
                inputSampleL = 0.999;
            }
            if (inputSampleL < -0.999) {
                inputSampleL = -0.999;
            }
            if (inputSampleR > 0.999) {
                inputSampleR = 0.999;
            }
            if (inputSampleR < -0.999) {
                inputSampleR = -0.999;
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
} // namespace airwindohhs::podcast
