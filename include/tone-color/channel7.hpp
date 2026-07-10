#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::channel7 {

constexpr std::string_view k_name{ "Channel7" };
constexpr std::string_view k_short_description{
    "Channel7 improves slew clipping and lets you blend Channel 6 and 5."
};
constexpr std::string_view k_long_description{
    "Short and sweet. You know how Interstage got an update because the slew clipping was too intense on high sample rates? This applied to Channel, too. So, Channel7 now handles high sample rates better than any previous Channel.There’s more: Channel5 was the last one with the ‘Density’ algorithm, that’s extra fat-sounding. Channel6 got the Spiral algorithm, much purer and clearer… but without that fatness effect Density gets. My attempts to make a special algorithm that does both gave me Mojo and Dyno… neither of which are Channel or a replacement for either Spiral or Density.So, Channel7 simply goes to 200%, not 100%. Up to 100% it’s the same as Channel6, but with the slew clipping fix. From 100% to 200% it does a crossfade into Channel5’s Density algorithm—so it ends up being best of all possible worlds. You can seamlessly go from perfectly clean, to the purest saturation there is, and beyond that to add any degree of fatness you like to the sound (and it fades between Spiral and Density, so there’s extra subtlety to the way it transitions).Channel7: yeah, you’re pretty much going to want this upgrade no matter which Channel you liked. It is all of the best, more effective than ever."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel7 final : public Effect<T>
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
    Channel7()
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
        double localiirAmount = iirAmount / overallscale;
        double localthreshold = threshold; // we've learned not to try and adjust threshold for sample rate
        double density = drive * 2.0; // 0-2
        double phattity = density - 1.0;
        if (density > 1.0) {
            density = 1.0; // max out at full wet for Spiral aspect
        }
        if (phattity < 0.0) {
            phattity = 0.0; //
        }
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
            double phatSampleL = sin(inputSampleL * 1.57079633);
            inputSampleL *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            double distSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleL = distSampleL; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleL = (drySampleL * (1 - density)) + (distSampleL * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleL = (inputSampleL * (1 - phattity)) + (phatSampleL * phattity); // apply original Density on top
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            double phatSampleR = sin(inputSampleR * 1.57079633);
            inputSampleR *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            double distSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            inputSampleR = distSampleR; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleR = (drySampleR * (1 - density)) + (distSampleR * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleR = (inputSampleR * (1 - phattity)) + (phatSampleR * phattity); // apply original Density on top
            }
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
} // namespace airwindohhs::channel7
