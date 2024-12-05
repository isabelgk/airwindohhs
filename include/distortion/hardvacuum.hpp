#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hardvacuum {

constexpr std::string_view k_name{ "HardVacuum" };
constexpr std::string_view k_short_description{
    "HardVacuum is tube style saturation effects."
};
constexpr std::string_view k_long_description{
    "Time to revisit an Airwindows classic!This is one of the plugins Airwindows ‘made its bones’ on, brought up to date and converted to VST for Mac, Windows and Linux (and made open source!)Hard Vacuum refers to vacuum tubes, and as you’d expect it’s a saturation device but with some interesting twists. It’s got a warmth control that brings in second harmonic and nonlinearity like you might see in a class A tube design, but set up to be abused if you’d like to produce exaggerated effects (that aren’t really ‘warm’ anymore, but if you want to play nice, don’t push this control too far). It’s got a nice Airwindows sine-based saturation curve… and it’s got a control called Aura that will bring out sparkly highs and hot searing overtones you might not have heard from a plugin before.The way that one works is thus: I saw scope traces of tube circuits that were showing slanty tops on squarewaves, like the power supplies weren’t keeping up. It became an obsession to make plugin saturation do that, and I came up with something that turned into Aura. Note that this is NOT ‘analog modeling’, not as people normally mean it: I’m rarely interested in running hapless audio through lots of math pretending to be electronic parts. I generally want a simple, unexpected algorithm that’ll do what I intend with minimal unnecessary math, because I find that overprocessing digital audio hurts the tone.As such, Aura doesn’t model any specific tube. You could probably combine it with Desk4 and who knows what else, to make a really good emulation given decent reference material, but Aura is simply a way to doctor the tone in a way that’s not EQ. By that I mean, the effect might lift up highs but it’s not working in terms of frequency zones, it sees only the amplitude of any given moment and the angle by which it reached that moment. Think of it as an extra thing you can do to the sound. This one’s sat around being Mac AU only for ten years before getting brought up to date (with denormalization fixes, noise shaping to the floating point buss, etc) and ported to VST, so now most of you can join in the fun."
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class HardVacuum final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double lastSampleR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    HardVacuum()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        E = 1.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive"; break;
            case kParamB: return "warmth"; break;
            case kParamC: return "aura"; break;
            case kParamD: return "output"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;
            case kParamB: return "Warmth"; break;
            case kParamC: return "Aura"; break;
            case kParamD: return "Output"; break;
            case kParamE: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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

        double multistage = A * 2.0;
        if (multistage > 1) {
            multistage *= multistage;
        }
        // WE MAKE LOUD NOISE! RAWWWK!
        double countdown;
        double warmth = B;
        double invwarmth = 1.0 - warmth;
        warmth /= 1.57079633;
        double aura = C * 3.1415926;
        double out = D;
        double wet = E;
        double drive;
        double positive;
        double negative;
        double bridgerectifierL;
        double bridgerectifierR;
        double skewL;
        double skewR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            skewL = (inputSampleL - lastSampleL);
            skewR = (inputSampleR - lastSampleR);
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            // skew will be direction/angle
            bridgerectifierL = fabs(skewL);
            bridgerectifierR = fabs(skewR);
            if (bridgerectifierL > 3.1415926) {
                bridgerectifierL = 3.1415926;
            }
            if (bridgerectifierR > 3.1415926) {
                bridgerectifierR = 3.1415926;
            }
            // for skew we want it to go to zero effect again, so we use full range of the sine
            bridgerectifierL = sin(bridgerectifierL);
            bridgerectifierR = sin(bridgerectifierR);
            if (skewL > 0) {
                skewL = bridgerectifierL * aura;
            }
            else {
                skewL = -bridgerectifierL * aura;
            }
            if (skewR > 0) {
                skewR = bridgerectifierR * aura;
            }
            else {
                skewR = -bridgerectifierR * aura;
            }
            // skew is now sined and clamped and then re-amplified again
            skewL *= inputSampleL;
            skewR *= inputSampleR;
            // cools off sparkliness and crossover distortion
            skewL *= 1.557079633;
            skewR *= 1.557079633;
            // crank up the gain on this so we can make it sing
            // We're doing all this here so skew isn't incremented by each stage
            countdown = multistage;
            // begin the torture
            while (countdown > 0)
            {
                if (countdown > 1.0) {
                    drive = 1.557079633;
                }
                else {
                    drive = countdown * (1.0 + (0.557079633 * invwarmth));
                }
                // full crank stages followed by the proportional one
                // whee. 1 at full warmth to 1.5570etc at no warmth
                positive = drive - warmth;
                negative = drive + warmth;
                // set up things so we can do repeated iterations, assuming that
                // wet is always going to be 0-1 as in the previous plug.
                bridgerectifierL = fabs(inputSampleL);
                bridgerectifierR = fabs(inputSampleR);
                bridgerectifierL += skewL;
                bridgerectifierR += skewR;
                // apply it here so we don't overload
                if (bridgerectifierL > 1.57079633) {
                    bridgerectifierL = 1.57079633;
                }
                if (bridgerectifierR > 1.57079633) {
                    bridgerectifierR = 1.57079633;
                }
                bridgerectifierL = sin(bridgerectifierL);
                bridgerectifierR = sin(bridgerectifierR);
                // the distortion section.
                bridgerectifierL *= drive;
                bridgerectifierR *= drive;
                bridgerectifierL += skewL;
                bridgerectifierR += skewR;
                // again
                if (bridgerectifierL > 1.57079633) {
                    bridgerectifierL = 1.57079633;
                }
                if (bridgerectifierR > 1.57079633) {
                    bridgerectifierR = 1.57079633;
                }
                bridgerectifierL = sin(bridgerectifierL);
                bridgerectifierR = sin(bridgerectifierR);
                if (inputSampleL > 0)
                {
                    inputSampleL = (inputSampleL * (1 - positive + skewL)) + (bridgerectifierL * (positive + skewL));
                }
                else
                {
                    inputSampleL = (inputSampleL * (1 - negative + skewL)) - (bridgerectifierL * (negative + skewL));
                }
                if (inputSampleR > 0)
                {
                    inputSampleR = (inputSampleR * (1 - positive + skewR)) + (bridgerectifierR * (positive + skewR));
                }
                else
                {
                    inputSampleR = (inputSampleR * (1 - negative + skewR)) - (bridgerectifierR * (negative + skewR));
                }
                // blend according to positive and negative controls
                countdown -= 1.0;
                // step down a notch and repeat.
            }
            if (out != 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::hardvacuum
