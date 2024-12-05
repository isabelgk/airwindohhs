#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::toneslant {

constexpr std::string_view k_name{ "ToneSlant" };
constexpr std::string_view k_short_description{
    "ToneSlant is a super-transparent ‘tilt EQ’ with very low Q."
};
constexpr std::string_view k_long_description{
    "This is a new plugin, not a port from an existing Audio Unit. It’s based on a variation on the Average concept. Turns out, it’s the tail end of the ‘averaged samples block’ that causes the cancellation node. ToneSlant implements a much bigger sample block (100 taps) but linearly fades the samples off between the first and last sample in the block.What that does, is produce an extremely transparent ’tilt EQ’ with a controllable corner point. And, it’s implemented in such a way that you can set it to null out (at extreme high boosts) and then bring in only the brightest highs (shown in the video). Quirky, maybe, but it broadens the ToneSlant toolkit. So, the main uses are:-Extremely low Q treble rolloff above a set point (with a fixed dB/oct)-That, plus you bring in the dry signal by not putting the Highs to an extreme (it’s like a positive/negative wet/dry/wet, but that doesn’t fit on the label, especially for VST)-Total cancellation with Taps at 1 and Highs at +1, and then you put Taps to just barely greater than 1 and you can have a very natural high-shelf controllable with the Highs controlThese come out of the algorithm: the reason it performs so well sonically is both the extremely low Q and the simplicity of the algorithm. Not everything I make belongs in a mastering studio, but ToneSlant is peculiarly suited to that use: it’s like those specialty EQs that have very few parts and impart no color to the sound. You should be careful applying ToneSlant, because it ‘hides’ and tries not to be apparent as EQ. Use it as such, when you need perhaps a strong ’tilt’ but don’t want the result to sound equalized."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class ToneSlant final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    double bL[102];
    double bR[102];
    double f[102];

  public:
    ToneSlant()
    {
        A = 0.0;
        B = 0.0;
        for (int count = 0; count < 102; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
        }
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
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "voicing"; break;
            case kParamB: return "highs"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Voicing"; break;
            case kParamB: return "Highs"; break;

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
            case kParamA: return "taps"; break;
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

        double inputSampleL;
        double inputSampleR;
        double correctionSampleL;
        double correctionSampleR;
        double accumulatorSampleL;
        double accumulatorSampleR;
        double drySampleL;
        double drySampleR;
        double overallscale = (A * 99.0) + 1.0;
        double applySlant = (B * 2.0) - 1.0;
        f[0] = 1.0 / overallscale;
        // count to f(gain) which will be 0. f(0) is x1
        for (int count = 1; count < 102; count++) {
            if (count <= overallscale) {
                f[count] = (1.0 - (count / overallscale)) / overallscale;
                // recalc the filter and don't change the buffer it'll apply to
            }
            else {
                bL[count] = 0.0; // blank the unused buffer so when we return to it, no pops
                bR[count] = 0.0; // blank the unused buffer so when we return to it, no pops
            }
        }
        while (--sampleFrames >= 0)
        {
            for (int count = overallscale; count >= 0; count--) {
                bL[count + 1] = bL[count];
                bR[count + 1] = bR[count];
            }
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            bL[0] = accumulatorSampleL = drySampleL = inputSampleL;
            bR[0] = accumulatorSampleR = drySampleR = inputSampleR;
            accumulatorSampleL *= f[0];
            accumulatorSampleR *= f[0];
            for (int count = 1; count < overallscale; count++) {
                accumulatorSampleL += (bL[count] * f[count]);
                accumulatorSampleR += (bR[count] * f[count]);
            }
            correctionSampleL = inputSampleL - (accumulatorSampleL * 2.0);
            correctionSampleR = inputSampleR - (accumulatorSampleR * 2.0);
            // we're gonna apply the total effect of all these calculations as a single subtract
            inputSampleL += (correctionSampleL * applySlant);
            inputSampleR += (correctionSampleR * applySlant);
            // our one math operation on the input data coming in
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
} // namespace airwindohhs::toneslant
