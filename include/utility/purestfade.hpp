#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestfade {

constexpr std::string_view k_name{ "PurestFade" };
constexpr std::string_view k_short_description{
    "PurestFade is just like PurestGain, but for LONG fades."
};
constexpr std::string_view k_long_description{
    "This is a plugin for one person.Okay, it’s a plugin that EXISTS because one person asked the right thing at the right time. A mastering engineer emailed me and said, PurestGain is great, it’s just perfect, everything I need except… when I do fades with it, there’s a discontinuity when I go from 0.001 volume on the Slow Fade, to 0. It cuts out abruptly, and that’s a problem. Can you help?I can!Thing is, PurestGain is designed so you can whack that control around aggressively. It doesn’t mute instantly, but you can kill a track real fast in a mix, gracefully, without zipper noise. PurestGain is correct for some users. If you needed to have a control on some kind of USB controller, like a NanoKontrol or something (say you’re doing an automation pass) you might well need to trim the overall gain with the top slider, and then assign the hardware controller to the bottom slider and automate. It’ll smooth out the incoming MIDI data and the result will be dynamic and aggressive, will feel like you’re yanking faders around on a real board. That’s the design intent for the ballistics of PurestGain.PurestFade is totally different. If you are at full crank and snap it instantly to zero, you’ll get an automated fade of about eight seconds or so. It gets subtler and subtler the quieter you go, until at the point of silence you literally can’t mess up the fade: it will always smooth out the last tiny bit.If you drag it slower than ten seconds you can do any slowness of fade you want, smooth as you like. And, an interesting thing is that if you need a faster fade than the automatic one (of just going fully to zero in an instant and letting the plugin do it) what you can do is manually drag it down faster than that. It’ll chase your fade speed, which means if you’re doing an unnaturally fast fade it’ll be trying to smooth you out a bit but it will let you do that. I think it’s the ultimate smoothed fade.And, in mixing (since it’s still PurestGain at heart) I think you can decide which you prefer, and if you’ve got a style then you know which one is right for you. Probably won’t be both. If you need to do abrupt, dynamic things it’s PurestGain you’ll want… but if your fader moves are generally more subtle, invisible, then you’re probably going to always want PurestFade for everything. (The top control will still give you more rapid moves if you need them)Which one is the right one for you? Most likely you already know which one you’ll be using, just from the descriptions."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class PurestFade final : public Effect<T>
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
    PurestFade()
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
            return 0.5;
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
            return "gain";
            break;
        kParamB:
            return "fade";
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
            return "Gain";
            break;
        kParamB:
            return "Fade";
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
            return "dB";
            break;
        kParamB:
            return " ";
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
        double fadeSpeed = targetBgain;
        if (fadeSpeed < 0.0027) {
            fadeSpeed = 0.0027; // minimum fade speed
        }
        fadeSpeed = ((Effect<T>::getSampleRate() * 0.004) / fadeSpeed);
        // this will tend to be much slower than PurestGain, and also adapt to sample rates
        double outputgain;
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
            gainBchase = (((gainBchase * fadeSpeed) + targetBgain) / (fadeSpeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            outputgain = gainchase * gainBchase;
            // directly multiply the dB gain by the straight multiply gain
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
} // namespace airwindohhs::purestfade
