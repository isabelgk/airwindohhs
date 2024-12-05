#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console5darkch {

constexpr std::string_view k_name{ "Console5DarkCh" };
constexpr std::string_view k_short_description{
    "Console5DarkCh is a richer, warmer Console system."
};
constexpr std::string_view k_long_description{
    "Console5 uses some more ‘expensive’ math operations, where previous Consoles tried to do their thing while keeping the Channel component as low-CPU as they possibly could. This might mean a heavier CPU cost, or it might be not that much of a difference. It’s a change (the math here more closely resembles Density or PurestDrive).What do you gain? Using this more advanced math means there are functions which can exactly ‘undistort’ what comes in (more on that property later: there’s a variation on Console that perfectly nulls when only one track is active). This brings an added level of bigness and signal purity. Then, Console5 applies a similar behavior to the slew factor, but backwards to what the amplitude factor is getting. Doing that takes Console5 away from perfect transparency (and subtlety) and gives it a big, beefy, large-console sound that still doesn’t radically alter individual tones… but throws in TONS of ‘glue’ and solidness compared to the raw digital mix.This is not a thing you’d struggle to hear (listen for depth and space, not frequency changes). This is not a thing that’d get washed out in mp3 encoding (in fact, because of the way it restricts slews in Console5Buss, it’ll actually help encoding a teeny bit, because superhigh frequencies waste bandwidth better used on the mids). This is the new Console, and it should be a real revelation to mix through, no matter what style or genre you’re working in.As seen in the video, if you’ve got a DAW that can enable/disable plugins on selected channels, you can audition it with one mouse click to switch. Console5 works like this: you want Console5Channel on every channel feeding the 2-buss (with all submixing and all post-plugin faders at unity gain), and Console5Buss first on the 2-buss. That’s all, just replace digital summing with this system. If you can do post-fader plugins, you can use the faders (otherwise, best use the trims on the Console5Channels, or any earlier gain trim). The point is to replace your digital summing network with the Console5 system.If you have that mastered, you can start playing with stuff like putting things ‘inside’ Console: delays, reverbs, EQs. Plain digital EQ in particular benefits from being post-Console5Channel on the track. Gain stage everything so you’re not slamming Console5Buss more than about +3 dB: it should survive hot peaks but there’s no special benefit to clipping it, and Console5Buss will clip there. Ideally, you’ll frame a mix with Console5 in place, and you may find you don’t need to do nearly as much ‘twitchy DAW stuff’ to get things sounding acceptable. Console5 addresses the root of the problem in a way no other ‘console emulation’ does. (if they do, you’ll find they have exactly the same constraints: needing to keep unity gain between Channel and Buss plugins is a dead giveaway they are using the Airwindows design)This is the Console version that can generate DC offset from sawtooth waveforms due to its interaction with slew rate handling, and the final Console5 you can get attempts to accomodate that. For the launch version (with a different DC-block strategy in place), use C5RawConsole."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console5DarkCh final : public Effect<T>
{
    double lastSampleChannelL;
    double lastSampleChannelR;
    double lastFXChannelL;
    double lastFXChannelR;
    double iirCorrectL;
    double iirCorrectR;
    double gainchase;
    double settingchase;
    double chasespeed;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console5DarkCh()
    {
        A = 1.0;
        lastSampleChannelL = 0.0;
        lastSampleChannelR = 0.0;
        lastFXChannelL = 0.0;
        lastFXChannelR = 0.0;
        iirCorrectL = 0.0;
        iirCorrectR = 0.0;
        gainchase = -90.0;
        settingchase = -90.0;
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
            case kParamA: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;

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
            case kParamA: return ""; break;

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
        double inputgain = A;
        double differenceL;
        double differenceR;
        double nearZeroL;
        double nearZeroR;
        double servoTrim = 0.0000001 / overallscale;
        double bassTrim = 0.005 / overallscale;
        double inputSampleL;
        double inputSampleR;
        if (settingchase != inputgain) {
            chasespeed *= 2.0;
            settingchase = inputgain;
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        if (gainchase < 0.0) {
            gainchase = inputgain;
        }
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
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + inputgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= gainchase;
                inputSampleR *= gainchase;
            }
            // done with trim control
            differenceL = lastSampleChannelL - inputSampleL;
            lastSampleChannelL = inputSampleL;
            differenceR = lastSampleChannelR - inputSampleR;
            lastSampleChannelR = inputSampleR;
            // derive slew part off direct sample measurement + from last time
            if (differenceL > 1.0) {
                differenceL = 1.0;
            }
            if (differenceL < -1.0) {
                differenceL = -1.0;
            }
            if (differenceR > 1.0) {
                differenceR = 1.0;
            }
            if (differenceR < -1.0) {
                differenceR = -1.0;
            }
            // clamp the slew correction to prevent invalid math results
            differenceL = lastFXChannelL + sin(differenceL);
            differenceR = lastFXChannelR + sin(differenceR);
            // we're about to use this twice and then not use difference again, so we'll reuse it
            // enhance slew is arcsin(): cutting it back is sin()
            iirCorrectL += inputSampleL - differenceL;
            inputSampleL = differenceL;
            iirCorrectR += inputSampleR - differenceR;
            inputSampleR = differenceR;
            // apply the slew to stored value: can develop DC offsets.
            // store the change we made so we can dial it back
            lastFXChannelL = inputSampleL;
            lastFXChannelR = inputSampleR;
            if (lastFXChannelL > 1.0) {
                lastFXChannelL = 1.0;
            }
            if (lastFXChannelL < -1.0) {
                lastFXChannelL = -1.0;
            }
            if (lastFXChannelR > 1.0) {
                lastFXChannelR = 1.0;
            }
            if (lastFXChannelR < -1.0) {
                lastFXChannelR = -1.0;
            }
            // store current sample as new base for next offset
            nearZeroL = pow(fabs(fabs(lastFXChannelL) - 1.0), 2);
            nearZeroR = pow(fabs(fabs(lastFXChannelR) - 1.0), 2);
            // if the sample is very near zero this number is higher.
            if (iirCorrectL > 0) {
                iirCorrectL -= servoTrim;
            }
            if (iirCorrectL < 0) {
                iirCorrectL += servoTrim;
            }
            if (iirCorrectR > 0) {
                iirCorrectR -= servoTrim;
            }
            if (iirCorrectR < 0) {
                iirCorrectR += servoTrim;
            }
            // cut back the servo by which we're pulling back the DC
            lastFXChannelL += (iirCorrectL * 0.0000005);
            lastFXChannelR += (iirCorrectR * 0.0000005);
            // apply the servo to the stored value, pulling back the DC
            lastFXChannelL *= (1.0 - (nearZeroL * bassTrim));
            lastFXChannelR *= (1.0 - (nearZeroR * bassTrim));
            // this cuts back the DC offset directly, relative to how near zero we are
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
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
} // namespace airwindohhs::console5darkch
