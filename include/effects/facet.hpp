#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::facet {

constexpr std::string_view k_name{ "Facet" };
constexpr std::string_view k_short_description{
    "Facet is a new kind of hard clip that adds two sharp corners into the wave."
};
constexpr std::string_view k_long_description{
    "Facet came out of me trying to come up with a new and distinct kind of distortion for the combined-distortions plugin I’m going to do. Rather than just bring Drive, Density, Spiral, Mojo etc. together, I wanted to add something different.So, Facet adds a ‘clip point’ but rather than a hard clip it just changes the ‘knee’ of the transfer function so your peaks can still go super high, way beyond 0 dB if you like.But, they change ‘angle’ at a sharp point which becomes sort of a corner in the sound. Except for high settings it’s both high up in the loudness, and a very gentle ‘corner’: still sharp, but very little change.And down near zero, it’s damn near total hard clipping. At silence, it literally is a hard clip to silence.Aw hell. Play with it, see what you think. i can’t explain this one well, at least not right now. The mad scientist labs have released another weird one. It will act like it’s a hard clip or a semi-dirty saturation, except the range of adjustment acts different, and it’ll throw an odd artifact on pure low-frequency tones… just play with it, see what you get. It will find its way into the combined plugin because it’s got a flavor all its own, but I can’t describe it today."
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Facet final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Facet()
    {
        A = 1.0;
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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "facet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Facet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double pos = A;
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
            if (inputSampleL > pos) {
                inputSampleL = ((inputSampleL - pos) * pos) + pos;
            } // facet algorithm: put a corner distortion in producing an artifact
            if (inputSampleL < -pos) {
                inputSampleL = ((inputSampleL + pos) * pos) - pos;
            } // we increasingly produce a sharp 'angle' in the transfer function
            if (inputSampleR > pos) {
                inputSampleR = ((inputSampleR - pos) * pos) + pos;
            } // facet algorithm: put a corner distortion in producing an artifact
            if (inputSampleR < -pos) {
                inputSampleR = ((inputSampleR + pos) * pos) - pos;
            } // we increasingly produce a sharp 'angle' in the transfer function
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
} // namespace airwindohhs::facet
