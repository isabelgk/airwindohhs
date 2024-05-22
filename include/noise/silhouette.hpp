#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Silhouette final : public Effect<T>
{
    std::string m_name{ "Silhouette" };

    double lastSampleL;
    double outSampleL;
    double lastSampleR;
    double outSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    Silhouette()
    {
        A = 0.0;
        lastSampleL = 0.0;
        outSampleL = 0.0;
        lastSampleR = 0.0;
        outSampleR = 0.0;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

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

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Dry/Wet";

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

        double wet = pow(A, 5);
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
            // begin L channel
            double bridgerectifier = fabs(inputSampleL) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            double silhouette = (double(fpdL) / UINT32_MAX);
            silhouette -= 0.5;
            silhouette *= 2.0;
            silhouette *= fabs(inputSampleL);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            double smoother = (double(fpdL) / UINT32_MAX);
            smoother -= 0.5;
            smoother *= 2.0;
            smoother *= fabs(lastSampleL);
            lastSampleL = inputSampleL;
            silhouette += smoother;
            bridgerectifier = fabs(silhouette) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            if (silhouette > 0.0) {
                silhouette = bridgerectifier;
            }
            else {
                silhouette = -bridgerectifier;
            }
            inputSampleL = (silhouette + outSampleL) / 2.0;
            outSampleL = silhouette;
            // end L channel
            // begin R channel
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            silhouette = (double(fpdR) / UINT32_MAX);
            silhouette -= 0.5;
            silhouette *= 2.0;
            silhouette *= fabs(inputSampleR);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            smoother = (double(fpdR) / UINT32_MAX);
            smoother -= 0.5;
            smoother *= 2.0;
            smoother *= fabs(lastSampleR);
            lastSampleR = inputSampleR;
            silhouette += smoother;
            bridgerectifier = fabs(silhouette) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            if (silhouette > 0.0) {
                silhouette = bridgerectifier;
            }
            else {
                silhouette = -bridgerectifier;
            }
            inputSampleR = (silhouette + outSampleR) / 2.0;
            outSampleR = silhouette;
            // end R channel
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
} // namespace airwindohhs
