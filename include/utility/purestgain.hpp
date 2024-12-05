#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestgain {

constexpr std::string_view k_name{ "PurestGain" };
constexpr std::string_view k_short_description{
    "PurestGain is a high-res noise shaped gain, with smoothed fader."
};
constexpr std::string_view k_long_description{
    "Marking the 200th plugin in Airwindows’ ‘AU’ category (not perfectly accurate, but yay anyhow) is PurestGain, in VST-enabled form!What’s to explain? It’s a gain utility. :DNo, seriously, that’s what it is. Here’s why some folks are a fan of this plugin anyway, even though every DAW has this as a utility plugin, plus the DAW faders built in.Firstly, gain is processing. When you apply even a simple gain change, it expands the word length of your digital audio out to arbitrary size. PurestGain comes from a set of plugins I did to experiment with the extremes of digital audio accuracy. You might think digital audio is automatically accurate, but that’s far from the truth. We hear degradation in the resolution domain as flatness, cardboardy-ness, and it’s cumulative. I don’t think anybody can hear the difference between PurestGain and a DAW utility gain plugin, when just a single plugin is in the signal path… but it’s cumulative.Also, you can’t be sure that a gain plugin is truly minimalist. If a plugin takes in floating-point audio of great quietness, and multiplies it by 1.0… that’s a math operation that can force the result into the same floating-point ‘level of resolution’ as the 1.0. Floating-point is treacherous, and the damage done is still very subtle but again is cumulative.PurestGain takes the input audio and does the gain processing at ‘long double’ resolution. It then noise shapes the result back into the DAW audio buss, whether that’s a 32 bit buss for normal VST and AU, or a double-precision 64 bit VST buss, if available. The result is an ultra-high-precision gain plugin that refuses to lose any audio quality. It’s the plugin equivalent of using switched attenuators with precision resistors in a mastering console, rather than potentiometers.There’s one more trick PurestGain has up its sleeve: a second control especially for fades. The trouble with DAW faders is that they must serve two masters: they’ve got to adjust smoothly and avoid zipper noise (crackling while you move the control, most clearly audible if you get a low-frequency sine wave going and then manipulate the control) but they’ve also got to snap instantly to a position if asked. The second slider in PurestGain runs in series with the dB gain control, but it functions very differently. One way to resist zipper noise is to have the gain smoothly ramp between volume settings, and that second control is designed for human-performed gain rides. Map the fader on a control surface to it, do your active mixing, and PurestGain will smooth every fader motion until it’s as fluid as any real-world analog console: try it with sinewaves and see how flawless the result can be.That’s a surprising amount to say about a gain plugin, but that’s Airwindows for you :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class PurestGain final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double gainchase;
    double settingchase;
    double gainBchase;
    double chasespeed;
    float A;
    float B;

  public:
    PurestGain()
    {
        A = 0.5;
        B = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        gainBchase = -90.0;
        chasespeed = 350.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "gain"; break;
            case kParamB: return "slow fade"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain"; break;
            case kParamB: return "Slow Fade"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB"; break;
            case kParamB: return " "; break;

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
        double inputgain = (A * 80.0) - 40.0;
        if (settingchase != inputgain) {
            chasespeed *= 2.0;
            settingchase = inputgain;
            // increment the slowness for each fader movement
            // continuous alteration makes it react smoother
            // sudden jump to setting, not so much
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        // bail out if it's too extreme
        if (gainchase < -60.0) {
            gainchase = pow(10.0, inputgain / 20.0);
            // shouldn't even be a negative number
            // this is about starting at whatever's set, when
            // plugin is instantiated.
            // Otherwise it's the target, in dB.
        }
        double targetgain;
        // done with top controller
        double targetBgain = B;
        if (gainBchase < 0.0) {
            gainBchase = targetBgain;
        }
        // this one is not a dB value, but straight multiplication
        // done with slow fade controller
        double outputgain;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            targetgain = pow(10.0, settingchase / 20.0);
            // now we have the target in our temp variable
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + targetgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            gainBchase = (((gainBchase * 4000) + targetBgain) / 4001);
            // gainchase is chasing the target, as a simple multiply gain factor
            outputgain = gainchase * gainBchase;
            // directly multiply the dB gain by the straight multiply gain
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (1.0 == outputgain)
            {
                *out1 = *in1;
                *out2 = *in2;
            }
            else {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
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
            }
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::purestgain
