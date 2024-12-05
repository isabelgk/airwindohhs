#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sidedull {

constexpr std::string_view k_name{ "SideDull" };
constexpr std::string_view k_short_description{
    "SideDull is like Sidepass, but a lowpass on the side channel. Treble-centerer."
};
constexpr std::string_view k_long_description{
    "There are some nice things about Patreon-era Airwindows. Not the money: that sucks, though I think I just about have basic survival taken care of as long as I don’t eat much and nothing happens to my car :)No, the cool thing is this: I can put out tools that have NOTHING to do with a plugin marketplace. If even one person finds the plugin useful, I can put it out. SideDull is kind of like that. I was asked, ‘since Sidepass is great, can we have a lowpass version?’. And I went ‘huhh? why would anyone want to narrow just top end, or center mids and up while not touching lows?’The answer is of course ‘why not?’ and ‘if you are sure you won’t be doing that, don’t install this plugin’ :)I don’t need to explain what SideDull does: it’s the same as Sidepass but in reverse, narrowing/mono-ing highs and down. I WOULD like to explain something about its role in a DAW user’s toolkit, though. I’m seeing this increasing churn in the plugin industry, a frenzy of new stuff and new requirements and DRM systems and dongles and such. It’s like a fulltime job to keep up with your recording system. My own pet fear is updating something only to find that I’ve hosed myself and everything’s now broken.That’s why SideDull, and every other plugin, is built on a time-capsule 10.6.8 system and the VST’s built on a time-capsule isolated-from-the-internet virtual Windows 7. I’m using the simplest possible interfaces (some vendors, like Blue Cat, have taken pains to implement the generic interface super well, and can even skin it attractively) and not touching stuff that causes forced obsolescence, which I think is really bad news for us all.So, the free AU/VST plugins (and the Kagi AUs before them) can be like your favorite screwdriver: you get to have the best and simplest audio tools that do NOT break. That also means that if you can only afford legacy computers (or just like being frugal and spending your money on Rickenbackers, as I long to do) you aren’t out in the cold… ever. I’m dedicating the rest of my life to putting tools in the hands of kids and dinosaurs alike, and following it up with my open source initiative. And the longer I do live, the more of it I get to do: but if I keel over tomorrow, all the plugs will still work and I will have had a good day and slept with a clear conscience :)"
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class SideDull final : public Effect<T>
{
    double iirSampleA;
    double iirSampleB;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    float A;

  public:
    SideDull()
    {
        A = 0.0;
        iirSampleA = 0.0;
        iirSampleB = 0.0;
        flip = true;
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
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "cutoff"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Cutoff"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;

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
        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
        double iirAmount = pow(A, 3) / overallscale;
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
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            if (flip)
            {
                iirSampleA = (iirSampleA * (1 - iirAmount)) + (side * iirAmount);
                side = iirSampleA;
            }
            else
            {
                iirSampleB = (iirSampleB * (1 - iirAmount)) + (side * iirAmount);
                side = iirSampleB;
            }
            // highpass section
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
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
} // namespace airwindohhs::sidedull
