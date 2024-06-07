#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::recurve {

constexpr std::string_view k_name{ "Recurve" };
constexpr std::string_view k_short_description{
    "Recurve is a special buss compressor with no threshold point."
};
constexpr std::string_view k_long_description{
    "Sometimes the most amazing things are the simplest.Recurve is ONE line of code (per channel) plus a bit of implementation. It’s a compressor, and this ‘preview’ look at it is a lot like when Spiral came out: this is Recurve as a ‘black box’, set up for maximum explosiveness and impressiveness. (I’ve got more elegant plugins in the works and this will become a core technique due to its effectiveness and simplicity of coding, so this is the introduction to what the algorithm does)Recurve works on principles similar to Spiral. It’s using a very high resolution sine calculation to affect the sound, and scales part of itself relative to the loudness of the sample it’s on. It also enjoys similar principles of ‘no sharp transitions, ever, for internal calculations’. In a clipper that means the rate of curvature doesn’t abruptly change, which is the principle behind Spiral. For Recurve, this is applied to the threshold of a compression and whether gain is being turned up or down.If the signal is super-quiet, the gain doesn’t change. Recurve takes moments of space and ambience between loud sounds, and preserves their character. It doesn’t swoop up in volume: if you need sidechainy pumping, you have to use something else.If the signal is medium, the gain gets turned up until it hits its max (with Recurve, it’ll exactly double the gain, which is BitShiftGain at its smallest increment. Exactly 6dB of very clean gain boosting things. This isn’t at a threshold: it’s at the most intense spot of a sine curve, so as a waveform passes through this zone it’ll increment the gain smoothly and without any transition points.If the signal is loud enough, it pushes this curve back through zero to its most negative point, and the sine function delivers a -1, for the maximum gain-cut, which is also scaled by how loud the sample is. So Recurve can cut back an over-loud transient FAST. In fact everything it does is really fast because the lack of transition points and the gradual nature of this sine-triggering lets it react very efficiently without edginess. It also lets through sonority and projection but cuts dull and muted stuff, kind of like Pyewacket, but without Pyewacket’s inherent pointyness.Sounds complicated? Just listen to stuff and switch it on. This is a preview in extra-dramatic form of an algorithm that’s going to find many uses in Airwindows plugins. It’s the compression equivalent of Spiral for saturation, and it could be adapted in many ways.The way you get today is a buss-comp or limiter form, running in true (linked) stereo. Mix into it and Recurve will gracefully eat up whatever you send it, even if you push it real hard. There’s a built-in 6dB of very clean boost to show off what it can do, and there’s a clipper on the output in case you get carried away and want to slam it so hard that transient attacks might poke out. And because of Recurve’s curve-and-recurve gain adjustment style, you’ll get none of the usual compression pumping and breathing: it’s just plain different. I hope you like it. There will be more :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Recurve final : public Effect<T>
{
    double gain;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Recurve()
    {
        gain = 2.0;
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
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            if (gain < 0.0078125) {
                gain = 0.0078125;
            }
            if (gain > 2.0) {
                gain = 2.0;
            }
            // gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
            // smaller number is maximum clamping, if too small it'll take a while to bounce back
            inputSampleL *= gain;
            inputSampleR *= gain;
            gain += sin((fabs(inputSampleL * 4) > 1) ? 4 : fabs(inputSampleL * 4)) * pow(inputSampleL, 4);
            gain += sin((fabs(inputSampleR * 4) > 1) ? 4 : fabs(inputSampleR * 4)) * pow(inputSampleR, 4);
            // 4.71239 radians sined will turn to -1 which is the maximum gain reduction speed
            inputSampleL *= 2.0;
            inputSampleR *= 2.0;
            if (inputSampleL > 0.9999) {
                inputSampleL = 0.9999;
            }
            if (inputSampleR > 0.9999) {
                inputSampleR = 0.9999;
            }
            if (inputSampleL < -0.9999) {
                inputSampleL = -0.9999;
            }
            if (inputSampleR < -0.9999) {
                inputSampleR = -0.9999;
            }
            // iron bar so people can play with this as a loudenator
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
} // namespace airwindohhs::recurve
