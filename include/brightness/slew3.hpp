#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::slew3 {

constexpr std::string_view k_name{ "Slew3" };
constexpr std::string_view k_short_description{
    "Slew3 is a new approach to slew clipping meant for a more analog-like darkening effect."
};
constexpr std::string_view k_long_description{
    "A Chris’s work is never done! Or at least it’s not, when it comes to refining basic tonal building blocks that apply to many plugins. It’s just recently that I updated Capacitor with an analog behavior found (quite strikingly) in certain real-world capacitors. I’m at it again.This makes a third Slew plugin, and every one is strikingly different. Slew (original) darkens radically and makes a grungy, clipped tone (it’s in Channel, too, very subtly). Slew2, though there are some similarities in code, acts wildly different: it produces an intense rolloff that is only right up at the Nyquist frequency, and is an elegant anti-glare solution, but barely has a tone at all.Slew3 uses ideas from Acceleration and DeBess to produce a slew clipping that’s actually reading information beyond what the samples provide: it’s like it reconstructs the wave a bit and is most effective where you’d get intersample peaks. It’s NOT an EQ: it has very striking dynamic qualities. It’s not a pure ‘glare cutter’ like Slew2, either: there’s a limit to how much it will darken. But what it’s all about is producing an analog top-end on your digital content.This is an experimental plugin. In development, some of my audio caused it to freak out, and it took extra time to get it to behave (I suppose I could also put out the freak-out version but for now let’s stay safe, OK?). It’s not quite linear or predictable (neither is real analog) and though I feel like it might have some very serious mojo to bring, I’m also interested in whether it dies given certain kinds of audio. I’m pretty sure I have it tamed to where it won’t do anything crazy, but is it really the silver bullet? I guess we’ll find out together."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Slew3 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;

  public:
    Slew3()
    {
        A = 0.0;
        lastSampleAL = lastSampleBL = lastSampleCL = 0.0;
        lastSampleAR = lastSampleBR = lastSampleCR = 0.0;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "clampng";
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
            return "Clampng";
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
        double threshold = pow((1 - A), 4) / overallscale;
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
            // begin L
            double clamp = (lastSampleBL - lastSampleCL) * 0.381966011250105;
            clamp -= (lastSampleAL - lastSampleBL) * 0.6180339887498948482045;
            clamp += inputSampleL - lastSampleAL; // regular slew clamping added
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = inputSampleL; // now our output relates off lastSampleB
            if (clamp > threshold) {
                inputSampleL = lastSampleBL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleBL - threshold;
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
            if (clamp > threshold) {
                inputSampleR = lastSampleBR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleBR - threshold;
            }
            lastSampleAR = (lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
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
} // namespace airwindohhs::slew3
