#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::clipsoftly {

constexpr std::string_view k_name{ "ClipSoftly" };
constexpr std::string_view k_short_description{
    "ClipSoftly is ClipOnly2 but as a softclipper: a very handy building block."
};
constexpr std::string_view k_long_description{
    "What more needs be said? ClipSoftly is ultimate softclipping, using the techniques used on ClipOnly2 for softening the edge of the hard clipping, but on the softest possible clipping. Where ClipOnly2 manipulates the onsets of the brightest transients to give you pure hard-clipping without digital grit, ClipSoftly is doing that without even hardclipping.So what we have here, basically, is a control-less (like ClipOnly and ClipOnly2) safety clipper, but it's a saturation effect. It's also a building block I (or others) can use inside larger plugins or new versions of Console, much like the recent Pop2 uses ClipOnly2 on the output, to give you the ability for really aggressive attacks.You'd use something like ClipOnly2 in mastering specifically because it won't touch the values of any unclipped samples. ClipSoftly is different: it will touch the values of ALL samples, reshaping the whole sound to make it bigger, fatter, tubier.I hope you like it, and the way it'll affect my plugins going forward :)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ClipSoftly final : public Effect<T>
{
    double lastSampleL;
    double intermediateL[16];
    double lastSampleR;
    double intermediateR[16];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ClipSoftly()
    {
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
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
            double softSpeed = fabs(inputSampleL);
            if (softSpeed < 1.0) {
                softSpeed = 1.0;
            }
            else {
                softSpeed = 1.0 / softSpeed;
            }
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL) * 0.9549925859; // scale to what cliponly uses
            inputSampleL = (inputSampleL * softSpeed) + (lastSampleL * (1.0 - softSpeed));
            softSpeed = fabs(inputSampleR);
            if (softSpeed < 1.0) {
                softSpeed = 1.0;
            }
            else {
                softSpeed = 1.0 / softSpeed;
            }
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR) * 0.9549925859; // scale to what cliponly uses
            inputSampleR = (inputSampleR * softSpeed) + (lastSampleR * (1.0 - softSpeed));
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
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
} // namespace airwindohhs::clipsoftly
