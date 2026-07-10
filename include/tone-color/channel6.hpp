#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::channel6 {

constexpr std::string_view k_name{ "Channel6" };
constexpr std::string_view k_short_description{
    "Channel6 uses the Spiral algorithm."
};
constexpr std::string_view k_long_description{
    "I use the plugin Channel to test out my latest code regarding things like how to prevent denormalized numbers, what’s the best noise shaping to return to the floating point buss, and so on. That’s because after a fairly brief debug period (at first, the model-choosing popup wasn’t actually changing anything and you always got SSL behavior) the plugin was established as a totally complete plugin: it always used the same ‘Density’ algorithm for the saturation, it always blended that with dry signal using a drive control that was really a dry/wet control (which gave it its openness at partial settings), and it always used the same slew clipping and simple IIR highpass code each time, which was so simple that it couldn’t be improved.And it’s always been ‘one’ of my most popular and successful plugins, through all those variations. Once there was a big blind shootout on the most popular plugin forum (back when people did that) and someone put up my stuff against several contenders including the most hyped plugins out there. At the time I was selling plugins for $50 a pop, and I’d hoped someone would pick BussColors and do a comparison against my flagship tone-emulator, and have it clobber its rivals in blind testing.Instead they picked the latest Channel, and used its settings of ‘API’ and ‘Neve’ and ‘SSL’ (in other words ‘slew clipping and a simple highpass and Density’, no real ’emulation’ just shaping set to calibrated amounts), and that free plugin was the one that clobbered the pricey rivals. And soon blind testing went out of fashion, because such shenanigans get embarrassing I guess :) And I didn’t get any sales out of it as Channel was free, but it was fun to watch.So will this be…Channel is back in version 6, and there’s one key change. The distortion algorithm which I thought was unbeatable has now been beaten by Spiral… so now Channel uses Spiral, not Density, for its distortion model! For once this should be a plainly audible change: Spiral’s less fat-sounding than the Density algorithm. (I fixed the bug with the dry/wet control in the video: now dry doesn’t clip)What won’t change is this: you’d be surprised how little ‘processing’ can give you a great, analog-style sound. Literally all Channel does is apply a very simple IIR highpass, add a distortion that’s very clear and pure-sounding, and do a slew clipping set so high that it will almost never kick in. The highpass isn’t very steep, because it’s just one stage of interleaved IIR filter (a trick I use in a more complicated way in Capacitor). These are subtle, gentle differences… but they’re done with absolutely minimal processing, not tons of processing, and the data integrity is kept pristine through long double math and noise shaping to the floating point buss. In simpler terms: this doesn’t suck your tone like heavy digital processing does, this applies ‘hardware-like’ changes calibrated to measurements of real hardware, and this does it at impressively low CPU compared to other plugins of this type."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel6 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleLA;
    double iirSampleRA;
    double iirSampleLB;
    double iirSampleRB;
    double lastSampleL;
    double lastSampleR;
    double iirAmount;
    double threshold;
    bool flip;
    float consoletype;
    float drive; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float output;

  public:
    Channel6()
    {
        consoletype = 0.0;
        drive = 0.0;
        output = 1.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        iirSampleLA = 0.0;
        iirSampleRA = 0.0;
        iirSampleLB = 0.0;
        iirSampleRB = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        flip = false;
        iirAmount = 0.005832;
        threshold = 0.33362176; // instantiating with Neve values
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: consoletype = value; break;
            case kParamB: drive = value; break;
            case kParamC: output = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return consoletype; break;
            case kParamB: return drive; break;
            case kParamC: return output; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "console type"; break;
            case kParamB: return "drive"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Console Type"; break;
            case kParamB: return "Drive"; break;
            case kParamC: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(consoletype); break;
            case kParamB: return std::to_string(drive); break;
            case kParamC: return std::to_string(output); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return "%"; break;
            case kParamC: return ""; break;

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
        const double localiirAmount = iirAmount / overallscale;
        const double localthreshold = threshold / overallscale;
        const double density = pow(drive, 2); // this doesn't relate to the plugins Density and Drive much
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (flip) {
                iirSampleLA = (iirSampleLA * (1 - localiirAmount)) + (inputSampleL * localiirAmount);
                inputSampleL = inputSampleL - iirSampleLA;
                iirSampleRA = (iirSampleRA * (1 - localiirAmount)) + (inputSampleR * localiirAmount);
                inputSampleR = inputSampleR - iirSampleRA;
            }
            else {
                iirSampleLB = (iirSampleLB * (1 - localiirAmount)) + (inputSampleL * localiirAmount);
                inputSampleL = inputSampleL - iirSampleLB;
                iirSampleRB = (iirSampleRB * (1 - localiirAmount)) + (inputSampleR * localiirAmount);
                inputSampleR = inputSampleR - iirSampleRB;
            }
            // highpass section
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output
            double distSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleL = (drySampleL * (1 - density)) + (distSampleL * density);
            // drive section
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output
            double distSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            inputSampleR = (drySampleR * (1 - density)) + (distSampleR * density);
            // drive section
            double clamp = inputSampleL - lastSampleL;
            if (clamp > localthreshold) {
                inputSampleL = lastSampleL + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleL = lastSampleL - localthreshold;
            }
            lastSampleL = inputSampleL;
            clamp = inputSampleR - lastSampleR;
            if (clamp > localthreshold) {
                inputSampleR = lastSampleR + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleR = lastSampleR - localthreshold;
            }
            lastSampleR = inputSampleR;
            // slew section
            flip = !flip;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
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
} // namespace airwindohhs::channel6
