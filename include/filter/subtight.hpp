#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SubTight final : public Effect<T>
{
    const std::string m_name{ "SubTight" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double subL[22]; // probably worth just using a number here
    double subR[22]; // probably worth just using a number here
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    SubTight()
    {
        A = 0.3;
        B = 0.5;
        for (int x = 0; x < 21; x++) {
            subL[x] = 0.0;
            subR[x] = 0.0;
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
            case kParamA: return "Trim";
            case kParamB: return "Steep";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int subStages = pow(B, 2) * 16.0;
        if (subStages < 1) {
            subStages = 1;
        }
        double subTrim = pow((A * 0.3) + (pow(B, 2) * 0.2), subStages) / overallscale;
        // to use this as an analog modeler for restricting digital lows, find set values that still show bass
        // Note that this is best used sparingly, on the 'not enough subtraction' side of the node.
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
            // you want subStages and subTrim to be hardcoded values when embedding this into something else
            // then it only needs the sub[] array, and to have it initialized to 0.0
            // begin SubTight section
            double subSampleL = inputSampleL * subTrim;
            double subSampleR = inputSampleR * subTrim;
            for (int x = 0; x < subStages; x++) {
                double scale = 0.5 + fabs(subSampleL * 0.5);
                subSampleL = (subL[x] + (sin(subL[x] - subSampleL) * scale));
                subL[x] = subSampleL * scale;
                scale = 0.5 + fabs(subSampleR * 0.5);
                subSampleR = (subR[x] + (sin(subR[x] - subSampleR) * scale));
                subR[x] = subSampleR * scale;
            }
            if (subStages % 2 > 0) {
                subSampleL = -subSampleL;
                subSampleR = -subSampleR;
            }
            if (subSampleL > 0.25) {
                subSampleL = 0.25;
            }
            if (subSampleL < -0.25) {
                subSampleL = -0.25;
            }
            if (subSampleR > 0.25) {
                subSampleR = 0.25;
            }
            if (subSampleR < -0.25) {
                subSampleR = -0.25;
            }
            inputSampleL -= (subSampleL * 16.0);
            inputSampleR -= (subSampleR * 16.0);
            // end SubTight section
            // cut the level WAY down, then the modified Creature code blows up subs.
            // the adjustment of scale destabilizes the routine to blow up more DC.
            // this is boosted by 24dB and subtracted from the dry signal
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
} // namespace airwindohhs
