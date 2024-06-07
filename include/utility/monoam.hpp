#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::monoam {

constexpr std::string_view k_name{ "MoNoam" };
constexpr std::string_view k_short_description{
    "MoNoam gives you variations on mono, mid, and side in different channels."
};
constexpr std::string_view k_long_description{
    "Here's MoNoam. It’s named sort of after the person who wanted something like it, and sort of after the concept ‘Mono’. Because all it does, is switch around some mid/side type things for your convenience.You see, the new Monitoring is getting ‘Aurat’ settings that give you that but as mono coming from literally one speaker (you pick). Seemed like a good way to replicate what is literally a mono mix check speaker. But I got requests for that on straight mono audio… and on side, for that matter… and a lot of stuff I can’t put in Monitoring without it getting way complicated. The thing is, if I do a ‘massmarket’ generally useful plugin like Monitoring, part of that is me choosing to leave stuff out, and what’s left in takes on greater importance as stuff I think you should be doing. But there are always exceptions…So, MoNoam (almost symmetrical, like a palindrome) gives you mono/mid and side (directly) in such a way that you could sum them to get the stereo back again. But it also gives you mono in either one or the other channel, to come out of only one speaker. And it gives you ‘mono-ized’ side: this will be fuller-sounding than the true side, because the true side is out of phase between channels. And you can put the mono-ized side in one or the other channel if you want. If you’re recombining things it’ll get weird, but if you just want to hear it then this is your chance. And maybe you might have tricky situations having to do with mid/side, or you’re doing odd things in Blue Cat Patchwork, or just making complicated signal routings, and need one of these options. They’re odd enough to require a lot of fiddling if you were doing them just in the DAW. Now they’re one simple plugin :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class MoNoam final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    static const int kBYPASS = 0;
    static const int kMONO = 1;
    static const int kMONOR = 2;
    static const int kMONOL = 3;
    static const int kSIDE = 4;
    static const int kSIDEM = 5;
    static const int kSIDER = 6;
    static const int kSIDEL = 7;
    float A;

  public:
    MoNoam()
    {
        A = 0.0;
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
            case kParamA: return "monotor"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Monotor"; break;

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

        int processing = (VstInt32)(A * 7.999);
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
            double mid;
            mid = inputSampleL + inputSampleR;
            double side;
            side = inputSampleL - inputSampleR;
            if (processing == kMONO || processing == kMONOR || processing == kMONOL) {
                side = 0.0;
            }
            if (processing == kSIDE || processing == kSIDEM || processing == kSIDER || processing == kSIDEL) {
                mid = 0.0;
            }
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            if (processing == kSIDEM || processing == kSIDER || processing == kSIDEL) {
                inputSampleL = -inputSampleL;
            }
            if (processing == kMONOR || processing == kSIDER) {
                inputSampleL = 0.0;
            }
            if (processing == kMONOL || processing == kSIDEL) {
                inputSampleR = 0.0;
            }
            if (processing == kBYPASS) {
                inputSampleL = *in1;
                inputSampleR = *in2;
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
} // namespace airwindohhs::monoam
