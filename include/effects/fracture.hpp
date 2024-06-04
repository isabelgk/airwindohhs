#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::fracture {

constexpr std::string_view k_name{ "Fracture" };
constexpr std::string_view k_short_description{
    "Fracture2 is a wavefolder that allows more extreme disruption."
};
constexpr std::string_view k_long_description{
    "Fracture is straight up a West Coast wavefolder, in a plugin. My original description doesn't help explain that too much: it is a sine-based distortion that wraps around and lets you set a maximum 'fold' which becomes a hard clip. Boost into it with Drive to do more intense wavefolding, and increase Fracture to allow you to fold more times before it just clips. This also allows you to clip either at full crank, or at silence (for a different effect). Original post was simply this:Hi! Merry Xmas to Xmas-ers. Here’s a truly freaky little wave-shaper. It’s of interest to distortion fanciers, those who make wubs and things, and anybody who might like the digital equivalent of an insane boutique stompbox that doesn’t sound like anything else.Fracture2 is straight up a West Coast wavefolder, in a plugin. You've got Drive, Fracture, Threshold, Output and Dry/Wet controls. Drive is basically overdrive like in Mackity, and you can get unity gain at a setting of 0.1, and Output and Dry/Wet are what you'd expect.Fracture makes the signal go exponential before folding. What this means is, if it's at zero you'll get a simple wavefold. With a sine, that'll create a distinct overtone (stronger than the original Fracture) which will slow down as it nears the top, then reverse. As you increase Fracture, the pitch of the wavefold increases because the wave you feed in will start folding faster and faster the more it folds: it will also open up the unfolded part of the wave, making it more dynamic. On non-tonal sounds, this makes the effect dryer and punchier and more aggressive.Threshold is by request from a fan who asked in Youtube comments, and whom I've got some things in common with. The idea is basically, can you make the folding part only take up a certain amount of space, like back to the zero point but not past? Turns out you can! And what this does is, it lets you have a volume control on the fold part. So, at zero threshold, you have a soft clip (or, with Fracture, a weird expando-clip like High Impact). As you increase it, the wavefold comes out of the clipping, expanding down (or up) from the flat-top of the wave, with 0.5 going to the center of the waveform and 1.0 doing a full-amplitude wavefold.Because this effect lends itself to the Rack port of my plugins (where you can feed audio rates into any controls), all the controls except Fracture are smoothed for better automation. I don't do this all the time, but it just seems to be the kind of effect where you might be sending sine waves into it (revealing zipper noises) and then modulating it like crazy, so it's using the same smoothing that's in the Z2-series filters.Hope you like it!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Fracture final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Fracture()
    {
        A = 0.25; // 1 from 0 to 4: A*4
        B = 0.5; // 2 from 1 to 3: (B*2.999)+1
        C = 1.0;
        D = 1.0;
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
        kNumParameters = 4

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.25;
            case kParamB: return 0.5;
            case kParamC: return 1.0;
            case kParamD: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive";
            case kParamB: return "fracture";
            case kParamC: return "out lvl";
            case kParamD: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive";
            case kParamB: return "Fracture";
            case kParamC: return "Out Lvl";
            case kParamD: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 4));
            case kParamB: return std::to_string((B * 2.999) + 1);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " ";
            case kParamB: return " ";
            case kParamC: return "dB";
            case kParamD: return " ";
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
        overallscale *= getSampleRate();
        double density = A * 4;
        double fracture = (((B * 2.999) + 1) * 3.14159265358979);
        double output = C;
        double wet = D;
        double dry = 1.0 - wet;
        double bridgerectifier;
        density = density * fabs(density);
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
            inputSampleL *= density;
            inputSampleR *= density;
            bridgerectifier = fabs(inputSampleL) * fracture;
            if (bridgerectifier > fracture) {
                bridgerectifier = fracture;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // blend according to density control
            bridgerectifier = fabs(inputSampleR) * fracture;
            if (bridgerectifier > fracture) {
                bridgerectifier = fracture;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // blend according to density control
            inputSampleL *= output;
            inputSampleR *= output;
            inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
            inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
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
} // namespace airwindohhs::fracture
