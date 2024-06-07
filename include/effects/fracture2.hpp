#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::fracture2 {

constexpr std::string_view k_name{ "Fracture2" };
constexpr std::string_view k_short_description{
    "Fracture2 is a wavefolder that allows more extreme disruption."
};
constexpr std::string_view k_long_description{
    "Fracture2 is straight up a West Coast wavefolder, in a plugin. You've got Drive, Fracture, Threshold, Output and Dry/Wet controls. Drive is basically overdrive like in Mackity, and you can get unity gain at a setting of 0.1, and Output and Dry/Wet are what you'd expect.Fracture makes the signal go exponential before folding. What this means is, if it's at zero you'll get a simple wavefold. With a sine, that'll create a distinct overtone (stronger than the original Fracture) which will slow down as it nears the top, then reverse. As you increase Fracture, the pitch of the wavefold increases because the wave you feed in will start folding faster and faster the more it folds: it will also open up the unfolded part of the wave, making it more dynamic. On non-tonal sounds, this makes the effect dryer and punchier and more aggressive.Threshold is by request from a fan who asked in Youtube comments, and whom I've got some things in common with. The idea is basically, can you make the folding part only take up a certain amount of space, like back to the zero point but not past? Turns out you can! And what this does is, it lets you have a volume control on the fold part. So, at zero threshold, you have a soft clip (or, with Fracture, a weird expando-clip like High Impact). As you increase it, the wavefold comes out of the clipping, expanding down (or up) from the flat-top of the wave, with 0.5 going to the center of the waveform and 1.0 doing a full-amplitude wavefold.Because this effect lends itself to the Rack port of my plugins (where you can feed audio rates into any controls), all the controls except Fracture are smoothed for better automation. I don't do this all the time, but it just seems to be the kind of effect where you might be sending sine waves into it (revealing zipper noises) and then modulating it like crazy, so it's using the same smoothing that's in the Z2-series filters.Hope you like it!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Fracture2 final : public Effect<T>
{
    double densityA;
    double densityB;
    double thresholdA;
    double thresholdB;
    double outputA;
    double outputB;
    double wetA;
    double wetB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Fracture2()
    {
        A = 0.1;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        densityA = 1.0;
        densityB = 1.0;
        thresholdA = 0.5;
        thresholdB = 0.5;
        outputA = 1.0;
        outputB = 1.0;
        wetA = 1.0;
        wetB = 1.0;
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
            case kParamA: return 0.1; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
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
            case kParamB: return "fractre"; break;
            case kParamC: return "thresh"; break;
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
            case kParamB: return "Fractre"; break;
            case kParamC: return "Thresh"; break;
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        densityA = densityB;
        densityB = A * 10.0; // 0.0 to 10.0
        int stages = B * 8.0;
        thresholdA = thresholdB;
        thresholdB = C;
        outputA = outputB;
        outputB = D;
        wetA = wetB;
        wetB = E;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double density = (densityA * temp) + (densityB * (1.0 - temp));
            double threshold = (thresholdA * temp) + (thresholdB * (1.0 - temp));
            double output = (outputA * temp) + (outputB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            inputSampleL *= density;
            inputSampleR *= density;
            for (int x = 0; x < stages; x++) {
                inputSampleL *= (fabs(inputSampleL) + 1.0);
                inputSampleR *= (fabs(inputSampleR) + 1.0);
            }
            if (inputSampleL > M_PI_2) {
                inputSampleL = (sin(inputSampleL) * threshold) + (1.0 * (1.0 - threshold));
            }
            else if (inputSampleL < -M_PI_2) {
                inputSampleL = (sin(inputSampleL) * threshold) + (-1.0 * (1.0 - threshold));
            }
            else {
                inputSampleL = sin(inputSampleL);
            }
            if (inputSampleR > M_PI_2) {
                inputSampleR = (sin(inputSampleR) * threshold) + (1.0 * (1.0 - threshold));
            }
            else if (inputSampleR < -M_PI_2) {
                inputSampleR = (sin(inputSampleR) * threshold) + (-1.0 * (1.0 - threshold));
            }
            else {
                inputSampleR = sin(inputSampleR);
            }
            inputSampleL *= output;
            inputSampleR *= output;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::fracture2
