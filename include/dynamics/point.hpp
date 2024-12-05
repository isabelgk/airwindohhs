#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::point {

constexpr std::string_view k_name{ "Point" };
constexpr std::string_view k_short_description{
    "Point is an explosive transient designer."
};
constexpr std::string_view k_long_description{
    "Point was introduced in 2007, just ahead of an amazing series of spatializers, analog modelers, and stompbox-style FX that consumed months of work. The curious thing is, Point didn’t. It’s one of those odd plugins that only required an idea: ‘what would happen if I did this?’, and an afternoon of coding. And ever after, it’s lived on as a mysterious and untameable plugin monster, secret weapon and mixer’s friend, always just as an obscure Audio Unit……until now.You get three controls: an input trim, the Point control, and a reaction speed. Point goes from -1.0 to 1.0 and ‘dry’ is 0.0. Reaction speed goes from 0.0 to 1.0 and there’s nothing to particularly suggest where anything should be set, so I’ll tell you now, and I’ll also tell you where NOT to set it if you know what’s good for you.For squishing off the fronts of snaredrums to make them huge, use Point -1.0 and a reaction speed around 0.166.To spotlight cymbal attacks while rounding the drums, use Point -1.0 and a reaction speed around 0.14.To hype up kick drum attacks and suppress the sustain in a gatey sort of way, use a reaction speed of around 0.3 and carefully add positive Point until you have the effect you want.To blow up the DAW and kill your ears, do that and crank Point to 1.0, then stop the transport, and then start it up again with Point still at 1.0…That’s your warning. Point is kind of like a ZVex Fuzz Factory or some such mad hardware device: the range of settings DOES include ‘out of control’, and it’s such a simple ‘circuit’ that it does little to restrain things when you Go Too Far and operate it in a state that will explode. It won’t just do it out of nowhere, but don’t make it transition between ‘off’ and Point 1.0: even if you have the fader buried, it can still clobber you.The reason I leave behaviors like that in there, in a plugin like Point, is that some people will want the full range of Point’s output, and will be following it with something to manage Point’s outbursts. If you’ve got it surrounded with plugins to tame it, I want you to be able to use Point settings near or at 1.0, and if you set it near that, you’ll immediately hear how intense it’s being so it won’t come as too much of a shock to discover it’s become an unstable isotope of transient destruction.:)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Point final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    bool fpFlip;
    // default stuff
    double nibAL;
    double nobAL;
    double nibBL;
    double nobBL;
    double nibAR;
    double nobAR;
    double nibBR;
    double nobBR;
    float A;
    float B;
    float C;

  public:
    Point()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        nibAL = 0.0;
        nobAL = 0.0;
        nibBL = 0.0;
        nobBL = 0.0;
        nibAR = 0.0;
        nobAR = 0.0;
        nibBR = 0.0;
        nobBR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        fpFlip = true;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input trim"; break;
            case kParamB: return "point"; break;
            case kParamC: return "reaction speed"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Trim"; break;
            case kParamB: return "Point"; break;
            case kParamC: return "Reaction Speed"; break;

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
            case kParamC: return std::to_string(C); break;

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
            case kParamC: return " "; break;

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
        double gaintrim = pow(10.0, ((A * 24.0) - 12.0) / 20);
        double nibDiv = 1 / pow(C + 0.2, 7);
        nibDiv /= overallscale;
        double nobDiv;
        if (((B * 2.0) - 1.0) > 0) {
            nobDiv = nibDiv / (1.001 - ((B * 2.0) - 1.0));
        }
        else {
            nobDiv = nibDiv * (1.001 - pow(((B * 2.0) - 1.0) * 0.75, 2));
        }
        double nibnobFactor = 0.0; // start with the fallthrough value, why not
        double absolute;
        double inputSampleL;
        double inputSampleR;
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
            inputSampleL *= gaintrim;
            absolute = fabs(inputSampleL);
            if (fpFlip)
            {
                nibAL = nibAL + (absolute / nibDiv);
                nibAL = nibAL / (1 + (1 / nibDiv));
                nobAL = nobAL + (absolute / nobDiv);
                nobAL = nobAL / (1 + (1 / nobDiv));
                if (nobAL > 0)
                {
                    nibnobFactor = nibAL / nobAL;
                }
            }
            else
            {
                nibBL = nibBL + (absolute / nibDiv);
                nibBL = nibBL / (1 + (1 / nibDiv));
                nobBL = nobBL + (absolute / nobDiv);
                nobBL = nobBL / (1 + (1 / nobDiv));
                if (nobBL > 0)
                {
                    nibnobFactor = nibBL / nobBL;
                }
            }
            inputSampleL *= nibnobFactor;
            inputSampleR *= gaintrim;
            absolute = fabs(inputSampleR);
            if (fpFlip)
            {
                nibAR = nibAR + (absolute / nibDiv);
                nibAR = nibAR / (1 + (1 / nibDiv));
                nobAR = nobAR + (absolute / nobDiv);
                nobAR = nobAR / (1 + (1 / nobDiv));
                if (nobAR > 0)
                {
                    nibnobFactor = nibAR / nobAR;
                }
            }
            else
            {
                nibBR = nibBR + (absolute / nibDiv);
                nibBR = nibBR / (1 + (1 / nibDiv));
                nobBR = nobBR + (absolute / nobDiv);
                nobBR = nobBR / (1 + (1 / nobDiv));
                if (nobBR > 0)
                {
                    nibnobFactor = nibBR / nobBR;
                }
            }
            inputSampleR *= nibnobFactor;
            fpFlip = !fpFlip;
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
} // namespace airwindohhs::point
