#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::distance {

constexpr std::string_view k_name{ "Distance" };
constexpr std::string_view k_short_description{
    "Distance is a sound design or reverb far-away-izer."
};
constexpr std::string_view k_long_description{
    "Here’s another utility plugin: Distance is specifically set up to mimic through-air high frequency attenuation. It’s from my initial wave of Airwindows plugins, come to VST and with a new twist: though in the video it’s a one-knobber, when you download it you’ll find that it’s got a Dry/Wet control, just to expand the things you can do with it. That’s new! I try to listen to people, even when it’s tempting to make it a super-dedicated one-trick pony.As you can see from how it behaves, Distance is a lot more complicated than just running a shelf. For that reason, I suggest this plugin for sound design and creative mixing purposes. Don’t try to use it for mastering or 2-buss, I feel it’s too intensely colored. However, for creative use it’s exactly what it says on the tin! Stick it on anything that’s supposed to ‘read’ sonically like it’s super far away, and you’ll be able to hear for miles and miles. Works on anything from pads to thunder to basses to reverb returns (I suggest using it on reverb returns rather than sends: it will be able to add thunder and size to the output of the reverb algorithm)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Distance final : public Effect<T>
{
    double lastclampL;
    double clampL;
    double changeL;
    double thirdresultL;
    double prevresultL;
    double lastL;
    double lastclampR;
    double clampR;
    double changeR;
    double thirdresultR;
    double prevresultR;
    double lastR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Distance()
    {
        A = 0.0;
        B = 1.0;
        thirdresultL = prevresultL = lastclampL = clampL = changeL = lastL = 0.0;
        thirdresultR = prevresultR = lastclampR = clampR = changeR = lastR = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "distance"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Distance"; break;
            case kParamB: return "Dry/Wet"; break;

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
            case kParamA: return "miles"; break;
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
        double softslew = (pow(A * 2.0, 3.0) * 12.0) + 0.6;
        softslew *= overallscale;
        double filtercorrect = softslew / 2.0;
        double thirdfilter = softslew / 3.0;
        double levelcorrect = 1.0 + (softslew / 6.0);
        double postfilter;
        double wet = B;
        double dry = 1.0 - wet;
        double bridgerectifier;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            inputSampleL *= softslew;
            lastclampL = clampL;
            clampL = inputSampleL - lastL;
            postfilter = changeL = fabs(clampL - lastclampL);
            postfilter += filtercorrect;
            if (changeL > 1.5707963267949) {
                changeL = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeL));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleL = lastL + (clampL * bridgerectifier);
            lastL = inputSampleL;
            inputSampleL /= softslew;
            inputSampleL += (thirdresultL * thirdfilter);
            inputSampleL /= (thirdfilter + 1.0);
            inputSampleL += (prevresultL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultL = prevresultL;
            prevresultL = inputSampleL;
            inputSampleL *= levelcorrect;
            inputSampleR *= softslew;
            lastclampR = clampR;
            clampR = inputSampleR - lastR;
            postfilter = changeR = fabs(clampR - lastclampR);
            postfilter += filtercorrect;
            if (changeR > 1.5707963267949) {
                changeR = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeR));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleR = lastR + (clampR * bridgerectifier);
            lastR = inputSampleR;
            inputSampleR /= softslew;
            inputSampleR += (thirdresultR * thirdfilter);
            inputSampleR /= (thirdfilter + 1.0);
            inputSampleR += (prevresultR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultR = prevresultR;
            prevresultR = inputSampleR;
            inputSampleR *= levelcorrect;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
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
} // namespace airwindohhs::distance
