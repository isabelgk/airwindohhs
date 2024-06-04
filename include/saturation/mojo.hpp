#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::mojo {

constexpr std::string_view k_name{ "Mojo" };
constexpr std::string_view k_short_description{
    "Mojo is a biggenator that also works as a loudenator."
};
constexpr std::string_view k_long_description{
    "Happy accidents! What I was trying to do was add a blend control to Spiral, so I could make a Channel that let you go between the original, ‘fatter’ sound and the cleaner, more transparent but less fat Spiral sound.Instead, I got this (and another, complementary plugin to be revealed later). I coded up a refinement to the algorithm, where the ‘curve factor’ of Spiral got modified by powers of itself, or powers of powers of itself… up to the fourth power, which turned out to sort of have MAGICAL powers, or at least that’s how it seemed when I worked out what was happening to my test sine waves.Mojo’s the result. It’s a neat little algorithm that doesn’t sound anything like Density, or Spiral. Instead, it sounds like concentrated HUGE. Even at no added boost, it makes the sound a lot fatter (much like what was asked: a more refined algorithm that still gets the fatness of Density). But then there’s more… when you start slamming it.Turns out this simple little algorithm, one single transfer function without extra parts or switches or added tricks, soaks up input gain like nothing I’ve seen.Understand, it’s not ‘clean’. It thickens and fattens the sound without any real EQ change, by where it puts the energy and how it rounds off peaks. It’s got a weirdly effective way of being able to round stuff off and then turn it into a mostly flat-topped output, like full-on digital clipping style loudness, but with neatly sculpted little curves going in and out of the flat stuff. It’s also such a nonintuitive algorithm that I wasn’t able to find an ideal spot to just straight clip it… so, like original Spiral, if you push beyond its limits it’ll start wavefolding on you (which can be an indication of too much slam). But the sweet spot is unusually wide and forgiving, and it sounds really loud while you explore that maximum limit.Mojo is an accident, but it’s also an obvious ‘popular’ plugin. Check to see if you’re okay with the extreme fattening effect it has, since it does really have a sound and isn’t what you’d call clean, even in the absence of extra boost. But if you were already looking for some ‘mojo’ to be added, this Mojo might be just the type… and, like the original Density algorithm, this one is likely to show up in other plugins as an added saturation element, because it’s got a distinct flavor that will help certain plugins do their thing."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Mojo final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Mojo()
    {
        A = 0.5;
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
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24.0) - 12.0);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double gain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
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
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            double mojo = pow(fabs(inputSampleL), 0.25);
            if (mojo > 0.0) {
                inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo) * 0.987654321;
            }
            // mojo is the one that flattens WAAAAY out very softly before wavefolding
            mojo = pow(fabs(inputSampleR), 0.25);
            if (mojo > 0.0) {
                inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo) * 0.987654321;
            }
            // mojo is the one that flattens WAAAAY out very softly before wavefolding
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
} // namespace airwindohhs::mojo
