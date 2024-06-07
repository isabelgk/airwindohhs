#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::softgate {

constexpr std::string_view k_name{ "SoftGate" };
constexpr std::string_view k_short_description{
    "SoftGate is a gate that can mute hiss and smooth sample tails."
};
constexpr std::string_view k_long_description{
    "Hi! Today’s Airwindows tool is for sample makers (though you could use in in a mix if you really wanted to, or abuse it for special effects). It’s called SoftGate.The purpose of SoftGate is this: you can set it so it’ll take the noise floor of a recording that you want to turn to samples (or multisamples), and fade it into darkness. You can set it to do this quickly, or quite slowly. This will clean up the noise floor of your multisample instruments and let you sustain things for longer, whatever the source recording quality is: if it’s real noisy and all your samples fade into a bunch of ugly noise, tell SoftGate to fade more slowly (lower on the slider) and allow that to kick in as the sound fades away into what’s supposed to be silence.Or, if it’s a good effect but you don’t need to produce that much gating, use the bottom fader to bring in the raw signal again and balance that with the processed one, to get the right kind of fade.The Audio Unit comes in SoftGate and SoftGateMono, because it’s inherently a linked stereo plugin (so stereo image doesn’t wander as it fades). You can use those on mono tracks in VST, but in AU you’re not supposed to run a stereo plugin on a mono source… so SoftGateMono exists to cover non-stereo tracks. If you use it on a stereo track it’ll gate Left and Right separately, so be warned. Also, SoftGateMono is technically ‘N to N’, like many of my AUs that aren’t stereo or stereo linked, so like many of my AUs you can also use it on quad, 5.1 or 7.1 tracks (did you know you could do that? Have fun, surround mixers)."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class SoftGate final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedL[2];
    double diffL;
    double storedR[2];
    double diffR;
    double gate;
    float A;
    float B;
    float C;

  public:
    SoftGate()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        storedL[0] = storedL[1] = 0.0;
        diffL = 0.0;
        storedR[0] = storedR[1] = 0.0;
        diffR = 0.0;
        gate = 1.1;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "thresh";
            break;
        kParamB:
            return "darken";
            break;
        kParamC:
            return "silence";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Thresh";
            break;
        kParamB:
            return "Darken";
            break;
        kParamC:
            return "Silence";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;

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
        overallscale *= getSampleRate();
        double threshold = pow(A, 6);
        double recovery = pow((B * 0.5), 6);
        recovery /= overallscale;
        double baseline = pow(C, 6);
        double invrec = 1.0 - recovery;
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
            storedL[1] = storedL[0];
            storedL[0] = inputSampleL;
            diffL = storedL[0] - storedL[1];
            storedR[1] = storedR[0];
            storedR[0] = inputSampleR;
            diffR = storedR[0] - storedR[1];
            if (gate > 0) {
                gate = ((gate - baseline) * invrec) + baseline;
            }
            if ((fabs(diffR) > threshold) || (fabs(diffL) > threshold)) {
                gate = 1.1;
            }
            else {
                gate = (gate * invrec);
                if (threshold > 0) {
                    gate += ((fabs(inputSampleL) / threshold) * recovery);
                    gate += ((fabs(inputSampleR) / threshold) * recovery);
                }
            }
            if (gate < 0) {
                gate = 0;
            }
            if (gate < 1.0)
            {
                storedL[0] = storedL[1] + (diffL * gate);
                storedR[0] = storedR[1] + (diffR * gate);
            }
            if (gate < 1) {
                inputSampleL = (inputSampleL * gate) + (storedL[0] * (1.0 - gate));
                inputSampleR = (inputSampleR * gate) + (storedR[0] * (1.0 - gate));
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
} // namespace airwindohhs::softgate
