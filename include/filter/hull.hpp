#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hull {

constexpr std::string_view k_name{ "Hull" };
constexpr std::string_view k_short_description{
    "Hull is an alternate form of highpass/lowpass filter."
};
constexpr std::string_view k_long_description{
    "This is the second time I’ve done a filter based on stuff the stock market folks have got up to, and both of ’em start with H. First there was Holt, and now this is Hull. It’s set up to work as either a lowpass or highpass filter: the Bright control is really a dry/wet. Bright hard left gives you darkening and the normal output of the filter, Bright hard right subtracts the output giving you a highpass.This is another audio chainsaw/proof of concept. I feel it’ll be useful as part of other plugins, in a controlled setting, but you can play with it however you like. Be careful, as setting the Freq control very high (increasing the averaging the plugin runs on, and lowering the cutoff frequency) can produce LOTS of CPU munching. I’ve left it that way in case people find a need for it and can handle the CPU demands, but especially at high sample rates it’s a beast at super-high averaging windows.Hull is a form of playing averaging filters against each other to produce an ‘accurate’ picture of underlying movement beneath noise. This is of course not true: it only appears to be giving optimal information, but it’s effectively synthesizing fake info to make the chart look more specific in its trajectories. It does a really good job of looking like it’s magically clearing away the randomness, but I don’t believe it really is, and you can hear it in the audio performance: it’s dirty, produces obvious artifacts and accentuates weird stuff.But for a sound effects filter, this is great! So, you can use Hull for various purposes, knowing it has ‘its own sound’ and will really bring a tone to your filtering. It sounds like a grungy old school analog filter that’s maybe distorting and being overloaded by the power of the audio going through it. The lowpass and highpass forms have very distinct tones: lowpassing sounds resonant and sonorous, and reminds me of the oldest Emu samplers (I’m working on getting a Eurorack filter that uses the same chip, to further explore this since I don’t yet have an SP1200). Highpassing does the opposite: it sounds like high frequency boosts done using Hull have a particular airiness and lightness to them.Taking it way down to the bass and demolishing your CPU in the process, a couple interesting things happen. Lowpassing gives you kicks with a LOT of punch, which let through a bunch of midrange in a way that accentuates impact. (There may be a way to implement this with much lower CPU if it’s a fixed frequency filter: the buffer size isn’t a problem, but allowing the adjustment means implementing it naively and doing things the hard way). Highpassing way down in the bass gives an equally distinctive sound which would translate over smaller speakers very effectively.This was a good day at work! I feel like modified versions of this principle will lead to some very cool-sounding EQs, even to stuff on the ’emulation’ side of things: this is because I like the sonority and intensity of these filters. They CAN also be CPU-efficient, though this implementation is not (except at high frequencies, where it is fine)."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Hull final : public Effect<T>
{
    double bL[4005];
    double bR[4005];
    int bPointer;
    double cL[105];
    double cR[105];
    int cPointer;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Hull()
    {
        A = 0.0;
        B = 0.5;
        for (int count = 0; count < 4002; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
        }
        for (int count = 0; count < 102; count++) {
            cL[count] = 0.0;
            cR[count] = 0.0;
        }
        bPointer = 1;
        cPointer = 1;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "freq"; break;
            case kParamB: return "bright"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Freq"; break;
            case kParamB: return "Bright"; break;

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

        double overallscale = 1.0;
        overallscale /= 96000.0;
        overallscale *= Effect<T>::getSampleRate();
        if (overallscale > 1.0) {
            overallscale = 1.0; // and if you go for 192k, rather than crash
        }
        // it just cuts out the maximum (2000) depth of averaging you can get
        double hullSetting = pow(A, 3) * overallscale;
        int limitA = (hullSetting * 2000.0) + 1.0;
        double divisorA = 1.0 / limitA;
        int limitB = (hullSetting * 1000.0) + 1.0;
        double divisorB = 1.0 / limitB;
        int limitC = sqrt(hullSetting * 2000.0) + 1.0;
        double divisorC = 1.0 / limitC;
        double wet = -((B * 2.0) - 1.0); // functions as dark/bright
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
            bPointer--;
            if (bPointer < 0) {
                bPointer += 2000;
            }
            bL[bPointer] = bL[bPointer + 2000] = inputSampleL;
            bR[bPointer] = bR[bPointer + 2000] = inputSampleR;
            int x = bPointer;
            double longAverageL = 0.0;
            double longAverageR = 0.0;
            double shortAverageL = 0.0;
            double shortAverageR = 0.0;
            while (x < bPointer + limitB) {
                shortAverageL += bL[x];
                shortAverageR += bR[x];
                longAverageL += bL[x];
                longAverageR += bR[x];
                x++;
            } // once shorter average is reached, continue to longer
            while (x < bPointer + limitA) {
                longAverageL += bL[x];
                longAverageR += bR[x];
                x++;
            }
            longAverageL *= divisorA;
            longAverageR *= divisorA;
            shortAverageL *= divisorB;
            shortAverageR *= divisorB;
            cPointer--;
            if (cPointer < 0) {
                cPointer += 50;
            }
            cL[cPointer] = cL[cPointer + 50] = shortAverageL + (shortAverageL - longAverageL);
            cR[cPointer] = cR[cPointer + 50] = shortAverageR + (shortAverageR - longAverageR);
            x = cPointer;
            double shortestAverageL = 0.0;
            double shortestAverageR = 0.0;
            while (x < cPointer + limitC) {
                shortestAverageL += cL[x];
                shortestAverageR += cR[x];
                x++;
            }
            shortestAverageL *= divisorC;
            shortestAverageR *= divisorC;
            if (wet > 0.0) {
                inputSampleL = (shortestAverageL * wet) + (drySampleL * (1.0 - wet)); // dark
                inputSampleR = (shortestAverageR * wet) + (drySampleR * (1.0 - wet)); // dark
            }
            else {
                inputSampleL = ((inputSampleL - shortestAverageL) * fabs(wet)) + (drySampleL * (1.0 + wet)); // bright
                inputSampleR = ((inputSampleR - shortestAverageR) * fabs(wet)) + (drySampleR * (1.0 + wet)); // bright
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::hull
