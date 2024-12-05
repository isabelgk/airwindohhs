#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::powersag {

constexpr std::string_view k_name{ "PowerSag" };
constexpr std::string_view k_short_description{
    "PowerSag is for emulating power supply limitations in analog modeling."
};
constexpr std::string_view k_long_description{
    "We’re going to explore the early Desk plugins (as free VSTs), so here we’ll start off with one of the underlying principles! PowerSag models the problem of analog power supplies that can’t source enough current to drive the output of the circuit. The circuit doesn’t directly distort, but the more output it’s been making, the less is in reserve. This is part of the Desk line of plugins, but now it’s a distinct component to play with.You get a Depth and Speed control. Dial in the effect by exaggerating Depth and then exploring with Speed: it’ll create a variety of compressey or distortey effects, but since they’re sucking energy out of the body of the sound, it’s a completely different type of distortion from saturation or clipping. Then, return Depth to zero and sneak small amounts of it back in, until the desired effect is reached. You’ll get a more tubey effect with extremely slow Speed, a big-console transistory effect with very quick Speed.The neat thing about PowerSag is that, if you like grunge and distortion, it’s capable of adding some grind to the sound while pulling the channel back in the mix, where traditional distortion and saturation pushes sounds forward. When you balance that with normal overdrive, you can get a lot of energy and character happening without everything becoming too fatiguing and up-front. Balance is good, being able to trim the body of a sound is good: if you like sculpting mixes with distortion and saturation, this might be right up your alley :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class PowerSag final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double dL[9000];
    double dR[9000];
    double controlL;
    double controlR;
    int gcount;
    float A;
    float B;

  public:
    PowerSag()
    {
        for (int count = 0; count < 8999; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        controlL = 0;
        controlR = 0;
        gcount = 0;
        A = 0.0;
        B = 0.3;
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
            case kParamB: return 0.3; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "depth"; break;
            case kParamB: return "speed"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Depth"; break;
            case kParamB: return "Speed"; break;

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

        double intensity = pow(A, 5) * 80.0;
        double depthA = pow(B, 2);
        int offsetA = (int)(depthA * 3900) + 1;
        double clamp;
        double thickness;
        double out;
        double bridgerectifier;
        double inputSampleL;
        double inputSampleR;
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
            if (gcount < 0 || gcount > 4000) {
                gcount = 4000;
            }
            // doing L
            dL[gcount + 4000] = dL[gcount] = fabs(inputSampleL) * intensity;
            controlL += (dL[gcount] / offsetA);
            controlL -= (dL[gcount + offsetA] / offsetA);
            controlL -= 0.000001;
            clamp = 1;
            if (controlL < 0) {
                controlL = 0;
            }
            if (controlL > 1) {
                clamp -= (controlL - 1);
                controlL = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            thickness = ((1.0 - controlL) * 2.0) - 1.0;
            out = fabs(thickness);
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
            inputSampleL *= clamp;
            // end L
            // doing R
            dR[gcount + 4000] = dR[gcount] = fabs(inputSampleR) * intensity;
            controlR += (dR[gcount] / offsetA);
            controlR -= (dR[gcount + offsetA] / offsetA);
            controlR -= 0.000001;
            clamp = 1;
            if (controlR < 0) {
                controlR = 0;
            }
            if (controlR > 1) {
                clamp -= (controlR - 1);
                controlR = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            thickness = ((1.0 - controlR) * 2.0) - 1.0;
            out = fabs(thickness);
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
            inputSampleR *= clamp;
            // end R
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::powersag
