#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pdchannel {

constexpr std::string_view k_name{ "PDChannel" };
constexpr std::string_view k_short_description{
    "PDChannel is Console5 and PurestDrive, sittin’ in a tree… (i.e. both at once, on channels and buss)"
};
constexpr std::string_view k_long_description{
    "What can I even say? This is pretty much just what you’d think it is. Console5 (PurestConsole version) with a PurestDrive stage in every channel and on the buss. There’s a dedicated control for each PurestDrive, and since it’s implemented as a single plugin it saves you having to go out to the 32-bit floating point buss (not applicable to the 64-bit buss VST, in that case it saves you having to go out to that buss)Behavior-wise, it’s a Console5 variant. You can swap in the channel or buss and exchange it for any other Console5 plugin of that type and it’ll work as intended. I should mention that I don’t really mean for these to be all mix-and-match, even though I know people will want to do that: in my opinion it’s best to find an optimal combination where the channels match and the buss is your preferred buss (maybe from the same Console variation, maybe not) and then tweaking for channel EQ etc is done using more normal tools. I’m allowing people to do the digital version of recording stuff with all different preamps in the belief that’s best, because I know it pleases some people a lot, but don’t think that you MUST make a big mix-n-match Console rig from all the different variations. It’ll be more cohesive if you use the same ones: the threads of the tapestry will match.And if you’re using PDConsole as your preferred version, you’re going to have a really soft, lush tapestry. It’s not a big blur-and-ooze offender, but all the same it will pull everything into a smoother, more seamless zone. If you’re craving analog warmth of the recording-console type, this is probably the version of Console5 that most closely resembles an analog console built of hardware electronic parts. Other Consoles seek to deliver holographic imaging or great depth (there’s one coming later which ignores sounding like hardware, and tries to sound like acoustic sounds mixing LOUD in free air, not even a mixing desk at all). But PDConsole blatantly goes for that lush, big-console sound, smoothing and rounding stuff off in a controlled, euphonic way (without getting muddy about it).I put out these experiments because I know that for some people out there, one will be by far the best. It won’t be the same one for everyone, but I like knowing that there will be some listeners who go ‘Yeah! This one is absolutely the one for me. I’m now all set, and my mixes sound even more like my intentions’."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class PDChannel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double gainchase;
    double settingchase;
    double chasespeed;
    double previousSampleL;
    double previousSampleR;
    float A;
    float B;

  public:
    PDChannel()
    {
        A = 1.0;
        B = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        chasespeed = 350.0;
        previousSampleL = 0.0;
        previousSampleR = 0.0;
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
            case kParamA: return 1.0; break;
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
            case kParamB: return "drive"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain"; break;
            case kParamB: return "Drive"; break;

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
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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

        double inputgain = A;
        double intensity = B;
        double applyL;
        double applyR;
        double drySampleL;
        double drySampleR;
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // basic distortion factor
            applyL = (fabs(previousSampleL + inputSampleL) / 2.0) * intensity;
            applyR = (fabs(previousSampleR + inputSampleR) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleL = (drySampleL * (1.0 - applyL)) + (inputSampleL * applyL);
            inputSampleR = (drySampleR * (1.0 - applyR)) + (inputSampleR * applyR);
            // dry-wet control for intensity also has FM modulation to clean up highs
            previousSampleL = sin(drySampleL);
            previousSampleR = sin(drySampleR);
            // apply the sine while storing previous sample
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
} // namespace airwindohhs::pdchannel
