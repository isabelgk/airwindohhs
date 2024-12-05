#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::loud {

constexpr std::string_view k_name{ "Loud" };
constexpr std::string_view k_short_description{
    "Loud is distortion and demolition of air molecules, modeled."
};
constexpr std::string_view k_long_description{
    "Here’s something rather special. What if you could distort like air molecules distort?I studied recordings of competitive tractor pulls, of Space Shuttle launches, various recordings that represented the way air can be mangled and break apart. The result is Loud… a step into a much louder world. It’s a distortion that can be slammed to unthinkable ‘heart of a supernova’ dB levels, but can also be subtly introduced to give the sonic coloration of a big LOUD noise in open air. Makes for a very interesting ‘glue’ at zero boost!Here’s how it’s done: rather than apply a consistent transfer function like a normal distortion, Loud knows whether you’re compressing the air, or letting it rebound. And if it’s snapping back, it can do it with the speed of lightning, but if it’s compressing, the air can be squished to practically solid, increasing heat. This extreme nonlinearity is why Loud sounds the way it does. It can sit on a whole mix to give it scope and authority, or it can be pushed harder on individual tracks like guitars and drums to amp up the ferocity.Remember, if you’ve got it totally fuzzing out, you are probably already beyond any sound level achievable by human means. The completely fried sound of cranked-up Loud is not meant to seem like acoustic phenomena as we know it. It turns up that loud because I grew up reading Douglas Adams’ “Hitchhiker’s Guide to the Galaxy”, and because in no other way can you accurately emulate a Disaster Area concert. :)"
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class Loud final : public Effect<T>
{
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Loud()
    {
        A = 0.0;
        B = 1.0;
        C = 1.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "boost"; break;
            case kParamB: return "output level"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Boost"; break;
            case kParamB: return "Output Level"; break;
            case kParamC: return "Dry/Wet"; break;

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
            case kParamA: return " "; break;
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
        double boost = pow(A + 1.0, 5);
        double output = B;
        double wet = C;
        double dry = 1.0 - wet;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double clamp;
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
            // begin L
            inputSampleL *= boost;
            clamp = inputSampleL - lastSampleL;
            if (clamp > 0)
            {
                inputSampleL = -(inputSampleL - 1.0);
                inputSampleL *= 1.2566108;
                if (inputSampleL < 0.0) {
                    inputSampleL = 0.0;
                }
                if (inputSampleL > 3.141527) {
                    inputSampleL = 3.141527;
                }
                inputSampleL = sin(inputSampleL) * overallscale;
                if (clamp > inputSampleL) {
                    clamp = inputSampleL;
                }
            }
            if (clamp < 0)
            {
                inputSampleL += 1.0;
                inputSampleL *= 1.2566108;
                if (inputSampleL < 0.0) {
                    inputSampleL = 0.0;
                }
                if (inputSampleL > 3.141527) {
                    inputSampleL = 3.141527;
                }
                inputSampleL = -sin(inputSampleL) * overallscale;
                if (clamp < inputSampleL) {
                    clamp = inputSampleL;
                }
            }
            inputSampleL = lastSampleL + clamp;
            lastSampleL = inputSampleL;
            // finished L
            // begin R
            inputSampleR *= boost;
            clamp = inputSampleR - lastSampleR;
            if (clamp > 0)
            {
                inputSampleR = -(inputSampleR - 1.0);
                inputSampleR *= 1.2566108;
                if (inputSampleR < 0.0) {
                    inputSampleR = 0.0;
                }
                if (inputSampleR > 3.141527) {
                    inputSampleR = 3.141527;
                }
                inputSampleR = sin(inputSampleR) * overallscale;
                if (clamp > inputSampleR) {
                    clamp = inputSampleR;
                }
            }
            if (clamp < 0)
            {
                inputSampleR += 1.0;
                inputSampleR *= 1.2566108;
                if (inputSampleR < 0.0) {
                    inputSampleR = 0.0;
                }
                if (inputSampleR > 3.141527) {
                    inputSampleR = 3.141527;
                }
                inputSampleR = -sin(inputSampleR) * overallscale;
                if (clamp < inputSampleR) {
                    clamp = inputSampleR;
                }
            }
            inputSampleR = lastSampleR + clamp;
            lastSampleR = inputSampleR;
            // finished R
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
} // namespace airwindohhs::loud
