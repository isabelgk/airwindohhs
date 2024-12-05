#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::nodedither {

constexpr std::string_view k_name{ "NodeDither" };
constexpr std::string_view k_short_description{
    "NodeDither is adjusta-TPDF-dither, like a dither flanger. Can do Paul and Tape settings."
};
constexpr std::string_view k_long_description{
    "And just one more dither plugin before the eagerly anticipated Naturalize… (because who will care about these then?) …NodeDither, the testbed upon which PaulDither and TapeDither were developed!I still think it’s good to have those as separate plugins (for simplicity and purity of concept) but since the concept is ‘TPDF made up of a sample of noise, and then a delayed version either normal or inverted’, how can I not give out the tool I used to find things like TapeDither? This was coded to do something that wasn’t possible: put a big delay in so there’s a ‘cancellation node’ at around 1.5K, making the noise quieter there.Trouble was, all you get is comb filtering and no nice clear ‘quieter’ zone. That’s why I experimented and found that four samples delay (inverted) gives you TapeDither, a nice softer noise. One sample inverted gives you PaulDither (in other words, a very bright one-pole highpass). Two samples gives you a neat silky texture which is still very bright. Eight samples gives you a thing kinda like TapeDither but even darker and beginning to sound flangey… you might call it CassetteDither, or maybe ‘TapeDither for 96K’.Call it what you like, because NodeDither lets you use all those and anything else, up to really extreme delays that give heavy, heavy comb filtering (flangeyness). You can reinforce lows rather than cancelling them through out-of-phase, and dial in dark moody dither noises that fit better with your project. You can tune the flangey note of the dither to your track. All manner of silly things can be yours with NodeDither :)And yes, you can make your noisefloor a flanger. I’ve demonstrated in the video how to do this with automation. If people really REALLY need a flange dither, I can make one later, but Naturalize will not be delayed any longer and is due to come out this Wednesday. But hopefully the possibilities of NodeDither are worth having! Or, put it this way: if NodeDither is for you, you know who you are and you’ve probably already downloaded it and are playing with the controls and thinking of how it would affect your creations.Because, bottom line: this is still technically correct TPDF dither. It’s just also an instrument, now. Have fun playing it :)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class NodeDither final : public Effect<T>
{
    double dL[5000];
    double dR[5000];
    int gcount;
    // default stuff
    uint32_t fpdL;
    uint32_t fpdR;
    float A;
    float B;

  public:
    NodeDither()
    {
        A = 0.04;
        B = 0.0;
        for (int count = 0; count < 4999; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        gcount = 0;
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
            case kParamA: return 0.04; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "node"; break;
            case kParamB: return "phase"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Node"; break;
            case kParamB: return "Phase"; break;

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
            case kParamA: return "samples"; break;
            case kParamB: return " "; break;

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
        int offsetA = (int)((A * 100) * overallscale);
        if (offsetA < 1) {
            offsetA = 1;
        }
        if (offsetA > 2440) {
            offsetA = 2440;
        }
        int phase = floor(B * 1.999);
        // 0 default is out of phase, 1 is in phase
        double inputSampleL;
        double inputSampleR;
        double currentDitherL;
        double currentDitherR;
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
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            if (gcount < 0 || gcount > 2450) {
                gcount = 2450;
            }
            currentDitherL = (double(fpdL) / UINT32_MAX);
            inputSampleL += currentDitherL;
            currentDitherR = (double(fpdR) / UINT32_MAX);
            inputSampleR += currentDitherR;
            if (phase == 1) {
                inputSampleL -= 1.0;
                inputSampleL += dL[gcount + offsetA];
                inputSampleR -= 1.0;
                inputSampleR += dR[gcount + offsetA];
            }
            else {
                inputSampleL -= dL[gcount + offsetA];
                inputSampleR -= dR[gcount + offsetA];
            }
            // in phase means adding, otherwise we subtract
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            dL[gcount + 2450] = dL[gcount] = currentDitherL;
            dR[gcount + 2450] = dR[gcount] = currentDitherR;
            gcount--;
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
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
} // namespace airwindohhs::nodedither
