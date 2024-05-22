#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ToneSlant final : public Effect<T>
{
    const std::string m_name{ "ToneSlant" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    double bL[102];
    double bR[102];
    double f[102];

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Voicing";
            case kParamB: return "Highs";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 99.0) + 1.0);
            case kParamB: return std::to_string((B * 2.0) - 1.0);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "taps";
            case kParamB: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
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
} // namespace airwindohhs
