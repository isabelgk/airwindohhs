#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::curve {

constexpr std::string_view k_name{ "curve" };
constexpr std::string_view k_short_description{
    "curve is the simplest, purest form of Recurve with no extra boosts."
};
constexpr std::string_view k_long_description{
    "sometimes you just want the effect to be lowercase.the previous plugin recurve slammed home with 6 db of gain in its compression so you could hear it working, and had a clipper on the end in case you wanted to use it as a loudenator.but maybe you don’t. maybe the best thing for the purest, subtlest compression (the one with no transition point between getting louder and getting quieter, and no edges in the sound anywhere) is to have no gain either. it can still catch overs… most of the time, occasionally not. it can still be heard, probably, but in this form it can sit on nearly any track, unnoticed, quietly balancing levels in lowercase.you could put it on everything, even though compression multiplies (you get the ratio of all the different compressions, times each other). curve is so calm and gentle that even putting it on all tracks and stems and then the 2-buss still shouldn’t give you a heavily compressed sound.people have asked whether you can still do airwindows patreon for one dollar a month rather than the ‘fifty dollars a year’ concept. the answer is yes, of course, the per-plugin concept is just to give people something they can relate to. also if you can only spare one dollar a month i would rather help you. but hey, if you gotta share the love i cannot argue as it would be most hypocritical given my own fierce affections for the music producing community.i was helping clean up my late dad’s house and got a book called archy and mehitabel. whether it influenced curve will have to remain a mystery to the non-literary. suffice to say there’s a dance in the old plugin-monger yet. whatthehell, whatthehell<3do 2s and 3s count as uppercase"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class curve final : public Effect<T>
{
    double gain;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    curve()
    {
        gain = 1.0;
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
            if (gain > 1.0) {
                gain = 1.0;
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
} // namespace airwindohhs::curve
