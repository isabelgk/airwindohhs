#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::flutter {

constexpr std::string_view k_name{ "Flutter" };
constexpr std::string_view k_short_description{
    "Flutter is the most recent Airwindows flutter, standalone."
};
constexpr std::string_view k_long_description{
    "This is by request. Sometimes you want stuff to go a little unsteady and wobbly, but you don't want a full-on tape emulation with, like, dubly and everything. (you don't do heavy metal in dubly, you know.) And so, here is Flutter, standalone!This is a tricky little algorithm, so let me give some details on what's happening here. It's not a vibrato, or even particularly random. Flutter gets its rate of waver, from the input signal coming in. This probably means that if you put a test tone in, you get a regular warble… or maybe even some awkward noise… out. The reason it does this is to react to the input waveform more directly. I realize the input waveform doesn't directly modsulate tape flutter, but this does, so there you go.In practice, you get a flutter/warble that stays pretty subtle right up to when it doesn't. Push it far enough and you get aggressive flutter. Dial it back and it quickly becomes more well-behaved, but it's still functioning and can do a nice job of destabilizing pitch in a tapelike way. A side effect of this quirky approach is that it runs with low enough (but NOT zero) latency that you can use it on a live instrument. You can sneak it onto a delay send, or reverb send, to give yourself just a little spread and layering to what otherwise sounds digitally flat and flawless. Or you can sit it on regular tracks and just go for that very subtle mulch-tone of old school retro, and use other means to dirty the sound up.Sometimes it's handy to be able to take something ultra-pure, like a bell or electric piano, and destabilize it without any tonal adjustment at all. Retain the chime but add that bit of drift. I hope you like Flutter."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Flutter final : public Effect<T>
{
    double dL[1002];
    double dR[1002];
    int gcount;
    double rateof;
    double sweep;
    double nextmax;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Flutter()
    {
        A = 0.0;
        for (int temp = 0; temp < 1001; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        gcount = 0;
        sweep = M_PI;
        rateof = 0.5;
        nextmax = 0.5;
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
            case kParamA: return "flutter"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Flutter"; break;

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
        double depth = pow(A, 2) * overallscale * 70;
        double fluttertrim = (0.0024 * pow(A, 2)) / overallscale;
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
            if (gcount < 0 || gcount > 999) {
                gcount = 999;
            }
            dL[gcount] = inputSampleL;
            dR[gcount] = inputSampleR;
            int count = gcount;
            double offset = depth + (depth * pow(rateof, 2) * sin(sweep));
            count += (int)floor(offset);
            inputSampleL = (dL[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
            inputSampleL += (dL[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));
            inputSampleR = (dR[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
            inputSampleR += (dR[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));

            rateof = (rateof * (1.0 - fluttertrim)) + (nextmax * fluttertrim);
            sweep += rateof * fluttertrim;
            if (sweep >= (M_PI * 2.0)) {
                sweep -= M_PI;
                nextmax = 0.24 + (fpdL / (double)UINT32_MAX * 0.74);
            }
            // apply to input signal only when flutter is present, interpolate samples
            gcount--;
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
} // namespace airwindohhs::flutter
