#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hype {

constexpr std::string_view k_name{ "Hype" };
constexpr std::string_view k_short_description{
    "Hype is a treble softening experiment gone strangely awry :)"
};
constexpr std::string_view k_long_description{
    "I may have mentioned that Console8 attempted a treble softening thing that went terribly wrong and acted like an aural exciter?This is it :)No controls, and in fact it fought my attempts to add any. If you use external controls, for instance the dry/wet you can get in Reaper, it will not behave nicely and will do weird things at intermediate settings. I have no idea what will happen if you try and oversample it: it, and you, may die, so you have been warned. This is the Airwindows experiment that went TOO weird to use as the treble softening in Console8, which is already pushing the limits of reasonableness in certain ways.Can’t even describe this, just drop it into your mix anywhere and see what it does. It’s like it tries to do a treble soften, but doesn’t get the concept entirely. In the video I made, you can see that stacking about six to eight of these up gets you into obvious distorted crackling territory. The original use in Console8 had three of these stacked, and I knew there was trouble when instead of softening the tone subtly and nicely, it was like a light turned on inside things. Don’t stack up too many of them if you want things to be more reasonable.I also can’t blend between this and the final Console8 treble softening. Nor am I interested in adding a switch so all Console8 (‘in’ plugs) have this, it’s just too eccentric, more like what Console7Cascade was, except Console8 now leans a little bit in the direction of what Console7Cascade was. That said, simply adding this to Console8 doesn’t replicate what the original experiment did, this is just that part of it stripped out and isolated in a sort of featureless plugin test-tube so you can play with the effect all by itself."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Hype final : public Effect<T>
{
    double softL[11];
    double softR[11];
    int cycleEnd;
    // from undersampling code, used as a way to space out HF taps
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Hype()
    {
        for (int count = 0; count < 10; count++) {
            softL[count] = 0.0;
            softR[count] = 0.0;
        }
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd == 3) {
            cycleEnd = 4;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
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
            if (cycleEnd == 4) {
                softL[8] = softL[7];
                softL[7] = softL[6];
                softL[6] = softL[5];
                softL[5] = softL[4];
                softL[4] = softL[3];
                softL[3] = softL[2];
                softL[2] = softL[1];
                softL[1] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[7];
                softR[7] = softR[6];
                softR[6] = softR[5];
                softR[5] = softR[4];
                softR[4] = softR[3];
                softR[3] = softR[2];
                softR[2] = softR[1];
                softR[1] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 2) {
                softL[8] = softL[6];
                softL[6] = softL[4];
                softL[4] = softL[2];
                softL[2] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[6];
                softR[6] = softR[4];
                softR[4] = softR[2];
                softR[2] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 1) {
                softL[8] = softL[4];
                softL[4] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[4];
                softR[4] = softR[0];
                softR[0] = inputSampleR;
            }
            softL[9] = ((softL[0] - softL[4]) - (softL[4] - softL[8]));
            if (softL[9] < -1.57079633) {
                softL[9] = -1.57079633;
            }
            if (softL[9] > 1.57079633) {
                softL[9] = 1.57079633;
            }
            inputSampleL = softL[8] + (sin(softL[9]) * 0.61803398);
            softR[9] = ((softR[0] - softR[4]) - (softR[4] - softR[8]));
            if (softR[9] < -1.57079633) {
                softR[9] = -1.57079633;
            }
            if (softR[9] > 1.57079633) {
                softR[9] = 1.57079633;
            }
            inputSampleR = softR[8] + (sin(softR[9]) * 0.61803398);
            // treble softening effect ended up being an aural exciter
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
} // namespace airwindohhs::hype
