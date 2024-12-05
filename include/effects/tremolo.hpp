#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tremolo {

constexpr std::string_view k_name{ "Tremolo" };
constexpr std::string_view k_short_description{
    "Tremolo is fluctuating saturation curves for a tubey tremolo."
};
constexpr std::string_view k_long_description{
    "So, you’ve probably got a tremolo built in to your DAW.But, if you’ve heard tremolo effects off classic records, it’s a whole different animal. DAW tremolos are neat little volume animations, capable of many cool effects (try making ’em squarewaves synced to the tempo for a nifty sequencey effect). But they don’t have that organic pulsating thing that takes a sound and gives it a whole new character.So I made this!This Tremolo uses saturation and antisaturation curves like you’d find in Density, and does the tremolo with that. It’s the same trick I use on the compressor ‘Pyewacket’. The result is, the loud parts develop a density and thickness mere volume won’t give you, and the lean parts hang on to a skeletal form of the transient attacks so your music comes through. This is not just ‘analog color’ like a coat of paint, Tremolo works quite differently from your DAW tremolo. It doesn’t sync to tempo, but that’s partly because I don’t know what to read (in AU and VST) that’d give me that information: could be added in future if the secrets are forthcoming, but there’s no sense withholding Tremolo just because of that!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Tremolo final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double sweep;
    double speedChase;
    double depthChase;
    double speedAmount;
    double depthAmount;
    double lastSpeed;
    double lastDepth;
    float A;
    float B;

  public:
    Tremolo()
    {
        A = 0.5;
        B = 1.0;
        sweep = 3.141592653589793238 / 2.0;
        speedChase = 0.0;
        depthChase = 0.0;
        speedAmount = 1.0;
        depthAmount = 0.0;
        lastSpeed = 1000.0;
        lastDepth = 1000.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "speed"; break;
            case kParamB: return "depth"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Speed"; break;
            case kParamB: return "Depth"; break;

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
            case kParamA: return " "; break;
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
        speedChase = pow(A, 4);
        depthChase = B;
        double speedSpeed = 300 / (fabs(lastSpeed - speedChase) + 1.0);
        double depthSpeed = 300 / (fabs(lastDepth - depthChase) + 1.0);
        lastSpeed = speedChase;
        lastDepth = depthChase;
        double speed;
        double depth;
        double skew;
        double density;
        double tupi = 3.141592653589793238;
        double control;
        double tempcontrol;
        double thickness;
        double out;
        double bridgerectifier;
        double offset;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            speedAmount = (((speedAmount * speedSpeed) + speedChase) / (speedSpeed + 1.0));
            depthAmount = (((depthAmount * depthSpeed) + depthChase) / (depthSpeed + 1.0));
            speed = 0.0001 + (speedAmount / 1000.0);
            speed /= overallscale;
            depth = 1.0 - pow(1.0 - depthAmount, 5);
            skew = 1.0 + pow(depthAmount, 9);
            density = ((1.0 - depthAmount) * 2.0) - 1.0;
            offset = sin(sweep);
            sweep += speed;
            if (sweep > tupi) {
                sweep -= tupi;
            }
            control = fabs(offset);
            if (density > 0)
            {
                tempcontrol = sin(control);
                control = (control * (1.0 - density)) + (tempcontrol * density);
            }
            else
            {
                tempcontrol = 1 - cos(control);
                control = (control * (1.0 + density)) + (tempcontrol * -density);
            }
            // produce either boosted or starved version of control signal
            // will go from 0 to 1
            thickness = ((control * 2.0) - 1.0) * skew;
            out = fabs(thickness);
            // do L
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            inputSampleL *= (1.0 - control);
            inputSampleL *= 2.0;
            // apply tremolo, apply gain boost to compensate for volume loss
            inputSampleL = (drySampleL * (1 - depth)) + (inputSampleL * depth);
            // end L
            // do R
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            inputSampleR *= (1.0 - control);
            inputSampleR *= 2.0;
            // apply tremolo, apply gain boost to compensate for volume loss
            inputSampleR = (drySampleR * (1 - depth)) + (inputSampleR * depth);
            // end R
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
} // namespace airwindohhs::tremolo
