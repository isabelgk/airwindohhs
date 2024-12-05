#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestdrive {

constexpr std::string_view k_name{ "PurestDrive" };
constexpr std::string_view k_short_description{
    "PurestDrive is the magic saturation plugin of subtlety and French House tone."
};
constexpr std::string_view k_long_description{
    "The funny thing about PurestDrive is that I didn’t expect much from it at all.This plugin (now coming to VST and open source) was simply an experiment. I’d been making complicated plugins with things like anti-aliasing, just a lot of calculation going on, and wasn’t sure I was getting what I wanted. It didn’t seem to be furthering the ‘Airwindows sound’. And I’d hit on an interesting tactic, noise shaping for the floating point buss. It seemed to me that you might be able to get a good sound by the opposite to complication: simplifying things, running the math at the highest possible resolution (in this case, long double precision, which is 80 bits on Intel and possibly 106 bits on PPC, which Airwindows still supports)The saturation algorithm was going to be sin(), which is the smoothest saturation curve you can have. Then I thought it’d be good to get a little more sophisticated than that because I had quite a few plugins based on sin(), so I did this:inputSample = sin(inputSample);apply = (fabs(previousSample + inputSample) / 2.0) * intensity;inputSample = (drySample * (1.0 – apply)) + (inputSample * apply);That’s it. That’s what makes PurestDrive (apart from some code supporting it, which is up and documented on GitHub as part of making it open source).What it does is this: it keeps the previous dry sample around, and ‘apply’ will be the combination of that and the saturated one. This makes apply be large if we’re on a low frequency wave, and if we’re doing a really bright transient apply might be zero. Intensity is how we include the slider control (0 to 1) and can also make apply be lower. Then, we’re adding the sin() effect, but ONLY relating to apply: we saturate the body of the sound, but as things get brighter or lower volume, we’re dynamically doing a dry/wet control at audio frequencies, so PurestDrive only throws saturation on dense solid material: airy highs, subtleties, get through more easily. As a side-effect, if our input gets super hot we can end up subtracting a little dry from the saturated inputSample.What PurestDrive did… was take off and become hugely popular. I didn’t expect that, it was simply an experiment in tone purity and I didn’t think anyone would notice. I knew the little tweak with ‘apply’ sounded good, but I figured the whole thing was so subtle that it wasn’t going to set the world on fire. Yet, PurestDrive developed incredibly dedicated fans, using it all over their mixes, swearing by its tone.And now, it’s a free and open source plugin, AU and Mac and PC VST. Remember it’s designed to not be a big obvious coat of paint over everything: because it’s using the dry sample so intensely, it cleans up and stays out of the way. But because it’s modulating between saturation and dry at audio frequencies based on both the current and previous sample, there’s a lot of textural effect on the sound. I never got so much mileage out of so few lines of code. Hope you like it. :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class PurestDrive final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double previousSampleL;
    double previousSampleR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    PurestDrive()
    {
        A = 0.0;
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
            case kParamA: return "drive"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;

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

        double intensity = A;
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
        double apply;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL = sin(inputSampleL);
            // basic distortion factor
            apply = (fabs(previousSampleL + inputSampleL) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleL = (drySampleL * (1.0 - apply)) + (inputSampleL * apply);
            // dry-wet control for intensity also has FM modulation to clean up highs
            previousSampleL = sin(drySampleL);
            // apply the sine while storing previous sample
            inputSampleR = sin(inputSampleR);
            // basic distortion factor
            apply = (fabs(previousSampleR + inputSampleR) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleR = (drySampleR * (1.0 - apply)) + (inputSampleR * apply);
            // dry-wet control for intensity also has FM modulation to clean up highs
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
} // namespace airwindohhs::purestdrive
