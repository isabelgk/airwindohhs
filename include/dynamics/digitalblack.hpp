#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::digitalblack {

constexpr std::string_view k_name{ "DigitalBlack" };
constexpr std::string_view k_short_description{
    "DigitalBlack is a quick, staccato gate."
};
constexpr std::string_view k_long_description{
    "So here’s a gate. And here is why you should care :DDigitalBlack is real simple on the surface: a threshold, and a dry/wet control. But what it does is more complicated. This isn’t a ‘gently fade to silence’ gate like SoftGate, or a ‘special effect’ like Gatelope. DigitalBlack was designed for one purpose: tightening up staccato direct-recorded tracks. I’m demonstrating it with a worst-case scenario because that’s all I had: I don’t get to do music, mostly, except for my live jams. But I had a drum room track where you can get some idea: play with it yourself if you’re curious, it’s free.DigitalBlack does three things that are interesting, two of which are pretty unique. Firstly, it uses hysteresis to prevent ‘sputtering’. That’s pretty normal. Second, it fades not with a simple volume, instead it fades into negative Density (bulk of the sound attenuated, only the transients stick out) which has the effect of sounding like it’s fading backwards away from you, very quick. This gives it a physical motion not common to gates. And third, it tracks zero crossings in a special way so that the ‘silence’ time it has to traverse, before hitting the negative Density area and then true silence, is related to the bassiness of the content.What this means is, if you’re hitting it with bassy content it’ll handle that gracefully. If you’ve got loads of midrange, it’ll gate that tighter. And if you’re making bright trebly sounds without a lot of bass, it’ll gate those FAST. And you can hear this on my lame demo, because in situations where only the initial spike of the drum hit got through on one side? (this is not linked: it’s designed so you could throw it on a submix with different stuff happening on L and R so it’s dual-mono) Even in my demo you can hear that some of those attacks are chopped off insanely fast. You’ll probably recognize pretty quickly if this is the gate for you. Try it on something like a DI guitar going into heavy ampsims, and see if you can’t get good results out of it. Put it on something like a kick or on individual drum mics (that you’re not already using Gatelope on), or on anything that needs to be insanely tight and quick to gate itself. It might be just what you needed. (for linked gentler slower gating to silence, try SoftGate: for a gate that also acts like envelope filters, use Gatelope)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class DigitalBlack final : public Effect<T>
{
    bool WasNegativeL;
    int ZeroCrossL;
    double gaterollerL;
    bool WasNegativeR;
    int ZeroCrossR;
    double gaterollerR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    DigitalBlack()
    {
        A = 0.0;
        B = 1.0;
        WasNegativeL = false;
        ZeroCrossL = 0;
        gaterollerL = 0.0;
        WasNegativeR = false;
        ZeroCrossR = 0;
        gaterollerR = 0.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
            break;
        kParamB:
            return 1.0;
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
            return "dry/wet";
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
            return "Dry/Wet";
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
        double threshold = (pow(A, 4) / 3) + 0.00018;
        double release = 0.0064 / overallscale;
        double wet = B;
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
            if (inputSampleL > 0) {
                if (WasNegativeL == true) {
                    ZeroCrossL = 0;
                }
                WasNegativeL = false;
            }
            else {
                ZeroCrossL += 1;
                WasNegativeL = true;
            }
            if (inputSampleR > 0) {
                if (WasNegativeR == true) {
                    ZeroCrossR = 0;
                }
                WasNegativeR = false;
            }
            else {
                ZeroCrossR += 1;
                WasNegativeR = true;
            }
            if (ZeroCrossL > 6000) {
                ZeroCrossL = 6000;
            }
            if (ZeroCrossR > 6000) {
                ZeroCrossR = 6000;
            }
            if (fabs(inputSampleL) > threshold) {
                if (gaterollerL < ZeroCrossL) {
                    gaterollerL = ZeroCrossL;
                }
            }
            else {
                gaterollerL -= release;
            }
            if (fabs(inputSampleR) > threshold) {
                if (gaterollerR < ZeroCrossR) {
                    gaterollerR = ZeroCrossR;
                }
            }
            else {
                gaterollerR -= release;
            }
            if (gaterollerL < 0.0) {
                gaterollerL = 0.0;
            }
            if (gaterollerR < 0.0) {
                gaterollerR = 0.0;
            }
            double gate = 1.0;
            if (gaterollerL < 1.0) {
                gate = gaterollerL;
            }
            double bridgerectifier = 1 - cos(fabs(inputSampleL));
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * gate) + (bridgerectifier * (1 - gate));
            }
            else {
                inputSampleL = (inputSampleL * gate) - (bridgerectifier * (1 - gate));
            }
            if ((gate == 0.0) && (fabs(inputSampleL * 3) < threshold)) {
                inputSampleL = 0.0;
            }
            gate = 1.0;
            if (gaterollerR < 1.0) {
                gate = gaterollerR;
            }
            bridgerectifier = 1 - cos(fabs(inputSampleR));
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * gate) + (bridgerectifier * (1 - gate));
            }
            else {
                inputSampleR = (inputSampleR * gate) - (bridgerectifier * (1 - gate));
            }
            if ((gate == 0.0) && (fabs(inputSampleR * 3) < threshold)) {
                inputSampleR = 0.0;
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
} // namespace airwindohhs::digitalblack
