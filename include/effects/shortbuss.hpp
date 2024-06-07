#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::shortbuss {

constexpr std::string_view k_name{ "ShortBuss" };
constexpr std::string_view k_short_description{
    "ShortBuss chases second harmonic, to add or remove it."
};
constexpr std::string_view k_long_description{
    "This plugin is a little hard to explain, as it doesn't do normal things.ShortBuss is a re-imagining of the original ShortBuss, that came before Righteous and perhaps even NC-17. It's a seed that created a lot of unusual, powerful Airwindows plugins, but it's taken on new powers in its current incarnation.It's all about asymmetry. ShortBuss will try to find imbalance in your waveform: even harmonics, more weight on one side than on the other. When it does, you can turn it up, to further augment that effect. I've learned that people are using Righteous to do this, liking the warmth of the bass and not minding the fact that the rest of Righteous doesn't care for that treatment. So, ShortBuss is there to specialize in that effect if you want it.Second harmonic can be hard to hear, and this is not an EQ, or even a saturation in any normal sense. So it might be a bit tricky to hear, but that's what it's doing (you will probably notice higher peaks if you do that, as well).But there's more: this time, you can apply reverse ShortBuss. Not only ShortBuss, but backwards! And then it'll fight even harmonics. It'll try to take them away. This might clean up the low end very slightly (it's not really set up to act like an EQ, what it takes from one polarity it'll put on the other).More interestingly, it might make for an interesting bass-conditioner. Like on a guitar going to heavy distortion. Or a bass-heavy mix heading to loudenation. It will do a conditioning effect that most likely doesn't exist elsewhere, because this is such a strange and hard to explain effect that it's unmarketable and can only exist in the world of open source and/or Airwindows.But then there's even more… because that same conditioning has eerie similarities to what might happen on the steel of a plate reverb when you bring the damper in real close, physically suppressing its lower frequencies in a way that's beyond simple EQ…ShortBuss is yours to play with, but it's also a neat addition to my toolbox, and I'm looking forward to using it in some unexpected places to bring sounds that aren't like usual sounds. I have no idea if you'll like it, but there's probably about three people who'll absolutely love it, and that's good enough for me. Enjoy :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class ShortBuss final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double sbSampleL;
    double sbSampleR;
    float A;
    float B;

  public:
    ShortBuss()
    {
        A = 0.5;
        B = 1.0;
        sbSampleL = 0.0;
        sbSampleR = 0.0;
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
            return "shortbs";
            break;
        kParamB:
            return "dry/wet";
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
            return "ShortBs";
            break;
        kParamB:
            return "Dry/Wet";
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
            return "";
            break;
        kParamB:
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
        overallscale *= getSampleRate();
        double sbScale = (pow((A * 2.0) - 1.0, 3) * 0.001) / sqrt(overallscale);
        double wet = B;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            inputSampleL += (2.0 * inputSampleL * inputSampleL) * sbSampleL;
            sbSampleL += ((inputSampleL - sin(inputSampleL)) * sbScale);
            sbSampleL = sin(sbSampleL * 0.015625) * 64.0;
            inputSampleR += (2.0 * inputSampleR * inputSampleR) * sbSampleR;
            sbSampleR += ((inputSampleR - sin(inputSampleR)) * sbScale);
            sbSampleR = sin(sbSampleR * 0.015625) * 64.0;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::shortbuss
