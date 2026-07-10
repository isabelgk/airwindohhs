#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::channel8 {

constexpr std::string_view k_name{ "Channel8" };
constexpr std::string_view k_short_description{
    "Channel8 is Channel7 with updates from Capacitor2 and Slew3. More and better of what Channel is."
};
constexpr std::string_view k_long_description{
    "Channel8 is a subtle ‘analogifier’ that applies saturation boosts, subtle highpass filtering, and slew clipping. What that means is, there’s a ‘drive’ knob you can turn up to boost things, and you can pick three ‘flavors’ you might recognize. They’re not really the consoles associated with those things, but they are calibrated to take effect about the same way, and when you use Channel8 (on tracks, on the mix buss, wherever) you add a little ‘analog flavor’ by restricting what the audio can do, in ways that are characteristic of real analog gear’s limitations. This restricting can help make things sound bigger and more solid.This is basically the newest evolution of Channel, which has always been one of my most popular plugins. It uses the improvements in Capacitor2 and Slew3, so basically it’s still relatively subtle, still very much Channel, but it’s taken on a new level in quality and natural tone. I’m real happy with it, and I’ll be returning to trying to fix up my video setup etc (and finishing up the dither re-releases, etc etc) knowing that I’ve dropped a nice audio bomb on the world. If you ever liked Channel, this should make you real happy."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel8 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleLA;
    double iirSampleRA;
    double iirSampleLB;
    double iirSampleRB;
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;
    double iirAmount;
    double threshold;
    bool flip;
    float consoletype;
    float drive; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float output;

  public:
    Channel8()
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
        lastSampleAL = lastSampleBL = lastSampleCL = 0.0;
        lastSampleAR = lastSampleBR = lastSampleCR = 0.0;
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
        double nonLin = 5.0 - density; // number is smaller for more intense, larger for more subtle
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double dielectricScaleL = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
            double dielectricScaleR = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));
            if (flip) {
                iirSampleLA = (iirSampleLA * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
                inputSampleL = inputSampleL - iirSampleLA;
                iirSampleRA = (iirSampleRA * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
                inputSampleR = inputSampleR - iirSampleRA;
            }
            else {
                iirSampleLB = (iirSampleLB * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
                inputSampleL = inputSampleL - iirSampleLB;
                iirSampleRB = (iirSampleRB * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
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
            // begin L
            double clamp = (lastSampleBL - lastSampleCL) * 0.381966011250105;
            clamp -= (lastSampleAL - lastSampleBL) * 0.6180339887498948482045;
            clamp += inputSampleL - lastSampleAL; // regular slew clamping added
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = inputSampleL; // now our output relates off lastSampleB
            if (clamp > localthreshold) {
                inputSampleL = lastSampleBL + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleL = lastSampleBL - localthreshold;
            }
            lastSampleAL = (lastSampleAL * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end L
            // begin R
            clamp = (lastSampleBR - lastSampleCR) * 0.381966011250105;
            clamp -= (lastSampleAR - lastSampleBR) * 0.6180339887498948482045;
            clamp += inputSampleR - lastSampleAR; // regular slew clamping added
            lastSampleCR = lastSampleBR;
            lastSampleBR = lastSampleAR;
            lastSampleAR = inputSampleR; // now our output relates off lastSampleB
            if (clamp > localthreshold) {
                inputSampleR = lastSampleBR + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleR = lastSampleBR - localthreshold;
            }
            lastSampleAR = (lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end R
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
} // namespace airwindohhs::channel8
