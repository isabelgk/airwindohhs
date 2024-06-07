#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sinew {

constexpr std::string_view k_name{ "Sinew" };
constexpr std::string_view k_short_description{
    "Sinew combines sines and slew clipping for a tape bias effect!"
};
constexpr std::string_view k_long_description{
    "Looks like I'm working on three major fronts at the moment, and here's a key advance on at least one of them :)Sinew is the answer to the question, 'what if slew clipping, but it was more restrictive the closer you got to what would be regular clipping?'I realize the answer is typically going to be 'slew what now?' but Airwindows fans are long aware of the strange pleasures of slew clipping. What you don't know is, the real answer to that question is 'then you get something that acts like analog tape's inability to capture super loud high frequencies'… but without the actual tape saturation!This might have all kinds of uses: I know it's going to find its way into a ToTape update. For now, you can have the raw version, the one where (like other Slew-oriented plugins) you can set it to extreme values and screw up the sound in interesting ways. Sinew might be just the thing for making heavy guitars louder, or adding guts to drums, but you can try it on whatever you like.It'll hang on to brightness for quite a long time, until suddenly it's really stepping on the sound. What's happening there is, you can't hear it doing more subtle work, so you only hear it when it's turned up too far. Listen to the character of things and you might hear it kicking in without apparently cutting back brightness at all! This is the farthest thing from a simple filter. Good luck experimenting with Sinew!"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Sinew final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSinewL;
    double lastSinewR;
    float A;

  public:
    Sinew()
    {
        A = 0.5;
        lastSinewL = 0.0;
        lastSinewR = 0.0;
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
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "sinew";
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
            return "Sinew";
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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double threshSinew = pow((1.0 - A), 4) / overallscale;
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
            double temp = inputSampleL;
            double clamp = inputSampleL - lastSinewL;
            double sinew = threshSinew * cos(lastSinewL * lastSinewL);
            if (clamp > sinew) {
                temp = lastSinewL + sinew;
            }
            if (-clamp > sinew) {
                temp = lastSinewL - sinew;
            }
            inputSampleL = lastSinewL = temp;
            temp = inputSampleR;
            clamp = inputSampleR - lastSinewR;
            sinew = threshSinew * cos(lastSinewR * lastSinewR);
            if (clamp > sinew) {
                temp = lastSinewR + sinew;
            }
            if (-clamp > sinew) {
                temp = lastSinewR - sinew;
            }
            inputSampleR = lastSinewR = temp;
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
} // namespace airwindohhs::sinew
