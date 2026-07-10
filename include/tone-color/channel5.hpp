#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdlib>

namespace airwindohhs::channel5 {

constexpr std::string_view k_name{ "Channel5" };
constexpr std::string_view k_short_description{
    "Channel5 is Channel for 2018, with new subtleties."
};
constexpr std::string_view k_long_description{
    "Channel has long been a sort of test-bed of mine, and so when I started working with new methods for some of my stuff it was an obvious choice. I’ve been asked for an extra Output level control, which it has (for those of you who are really into level matching, can’t fault you for that though sometimes I think it’s not relevant) but if you set Output to 1.0 it bypasses itself and becomes exactly, exactly the same as Channel4.Except for two things, which first appeared in Atmosphere: this lets you A/B them with the original versions, using Channel 4 and 5.One, I introduced the concept of live air denormalizing: before this (and on plugins I think are suited to it), Airwindows plugins watched for total digital black silence, and if it was present, produced an impossibly faint hiss sound, at around 300 dB down. If there was any audio at all that wasn’t digital black, this got switched off. With Atmosphere, it’s different: the denormalizing signal is always there, and if the result WITH the signal is digital black, then the signal’s subtracted again. Same result, but there’s a permanent noise hundreds of times quieter than typical ‘noise adding plugins’. The idea is that there can be no ‘dead’ silence, only ‘alive’ silence: you can’t save the file in normal formats and retain it (24 bit audio saving will simply turn this noise to digital black again), you most likely can’t hear it ever no matter what you do, but I wanted the deepest darkest silence to be alive rather than dead black nothingness.Two, it noise shapes to the floating point buss differently, and on the one hand I bet you can’t consistently hear it but on the other, I think it matters. Before, I used an interleaving technique much like I often use in IIR filters and plugs such as Capacitor. It related to the sample rate by forcing everything to be considered as the interaction between two or more samples, and the energy being brought back into the output audio provided corrections at high frequencies. What began with Atmosphere, and what you’ll (maybe) hear with Channel5, is a completely different and more radical approach. These plugins noise shape ALL the error energy back into the audio. It accumulates, sticks around, and so it can contribute to bass energy in tiny amounts because it persists over thousands and thousands of samples. There’s a ‘pull-back’ in the form of multiplying the amount by something like 0.999999, which will bleed built-up energy away, but this multiply doesn’t happen on a per-sample basis. It happens per BUFFER. So, in the world of noise shaping floating point error, this is the one that stores up every tiniest bit of error and applies it, not interleaved but very smoothly, to the audio.What should happen as a result of this is, there’s a sense of ease from switching from interleaved to the smooth way. It might seem a little less lively, and I’ll have some plugins that retain the other way. But due to the greatly increased time during which the error is held, plugins that use the new noise shaping will be crazy good at presenting analog-like, seamless bass and mids. So, more soothing, possibly less exciting, but a huge jump in depth and naturalness.…in RELATIVE terms to what was there before. That’s the thing. I feel it’s possible that when you get used to it the change can be dimly sensed as a good thing. I don’t think you, or I, or anyone would be able to pick this out double blind. I can think of a mastering guy or two that MIGHT, but they’d struggle: these are people who take pains to dither to 24 bit and say that they sense something wrong if they don’t. It’s on the order of that, or even more subtle.But I do feel that one eventually senses the full range of a sound one works with, and more importantly if I can make something better I will do so, without hesitating.Have fun playing with Channel5. And if anyone needs to sound the alarm and say HEY! You need to stop using the new stuff! That noise is driving me crazy, and your noise shaping sounds awful now!…well, that’d be pretty funny :)"
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel5 final : public Effect<T>
{
    double fpNShapeL;
    double fpNShapeR;
    bool fpFlip;
    // default stuff
    double iirSampleLA;
    double iirSampleRA;
    double iirSampleLB;
    double iirSampleRB;
    double lastSampleL;
    double lastSampleR;
    double iirAmount;
    double threshold;
    float consoletype;
    float drive; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float output;

  public:
    Channel5()
    {
        fpNShapeL = 0.0;
        fpNShapeR = 0.0;
        consoletype = 0.0;
        drive = 0.0;
        output = 1.0;
        fpFlip = true;
        iirSampleLA = 0.0;
        iirSampleRA = 0.0;
        iirSampleLB = 0.0;
        iirSampleRB = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            if (fpFlip) {
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
            double bridgerectifier = fabs(inputSampleL) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleL = (inputSampleL * (1 - density)) - (bridgerectifier * density);
            }
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleR = (inputSampleR * (1 - density)) - (bridgerectifier * density);
            }
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
            fpFlip = !fpFlip;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            // noise shaping to 64-bit floating point
            double fpTemp = inputSampleL;
            fpNShapeL += (inputSampleL - fpTemp);
            inputSampleL += fpNShapeL;
            // if this confuses you look at the wordlength for fpTemp :)
            fpTemp = inputSampleR;
            fpNShapeR += (inputSampleR - fpTemp);
            inputSampleR += fpNShapeR;
            // for deeper space and warmth, we try a non-oscillating noise shaping
            // that is kind of ruthless: it will forever retain the rounding errors
            // except we'll dial it back a hair at the end of every buffer processed
            // end noise shaping on 64 bit output
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
        fpNShapeL *= 0.999999;
        fpNShapeR *= 0.999999;
        // we will just delicately dial back the FP noise shaping, not even every sample
        // this is a good place to put subtle 'no runaway' calculations, though bear in mind
        // that it will be called more often when you use shorter sample buffers in the DAW.
        // So, very low latency operation will call these calculations more often.
    }
};
} // namespace airwindohhs::channel5
