#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::midside {

constexpr std::string_view k_name{ "MidSide" };
constexpr std::string_view k_short_description{
    "MidSide is mid/side conversion utility plugins."
};
constexpr std::string_view k_long_description{
    "It’s always nice to expand one’s horizons! You don’t have to run the latest Logic to be able to use any plugin in mid/side mode… though you might need to do a little fiddling with settings.EdIsDim comes in two plugs: first use MidSide to convert your stereo track into mid/side (on left and right channels). Then apply your processing (there’s a control with which you can balance the mid/side balance, which also means you can gain stage the M/S content into your plugin), then, go into EdIsDim (read it backwards) to convert back to stereo. The same control is present, and can reverse the gain staging you applied.Some good techniques include using it with Density to saturate the center and leave the stereo width lively, or getting a fresh take on a stereo reverb that’s much wider than usual. But the real purpose is opening up funky routing techniques in your DAW: send the L and R to separate busses, do all manner of things with plugins that don’t even have to be the same, or to line up (sample delay on just mid or side, anyone?) and merge them back to a stereo buss with EdIsDim on it. Or design weird synth patches that are heavy on the left with a lot of odd stereo behaviors, just to run into EdIsDim as an intentional part of the synth sound. Sky’s the limit :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class MidSide final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    MidSide()
    {
        A = 0.5;
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
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "mid/side"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Mid/Side"; break;

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
            case kParamA: return " "; break;

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

        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
        double sidegain = A * 2.0;
        double midgain = 2.0 - sidegain;
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
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            mid *= midgain;
            side *= sidegain;
            // stereo 64 bit dither, made small and tidy.
            // int expon; frexpf((float)mid, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // mid += ((double(fpdL)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
            // end 32 bit floating point dither
            // frexpf((float)side, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // side += ((double(fpdR)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
            // end 64 bit dither
            *out1 = mid;
            *out2 = side;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::midside
