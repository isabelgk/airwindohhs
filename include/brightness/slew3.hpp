#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Slew3 final : public Effect<T>
{
    std::string m_name{ "Slew3" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    Slew3()
    {
        A = 0.0;
        lastSampleAL = lastSampleBL = lastSampleCL = 0.0;
        lastSampleAR = lastSampleBR = lastSampleCR = 0.0;
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
            case kParamA: return "Clampng";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double threshold = pow((1 - A), 4) / overallscale;
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
            // begin L
            double clamp = (lastSampleBL - lastSampleCL) * 0.381966011250105;
            clamp -= (lastSampleAL - lastSampleBL) * 0.6180339887498948482045;
            clamp += inputSampleL - lastSampleAL; // regular slew clamping added
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = inputSampleL; // now our output relates off lastSampleB
            if (clamp > threshold) {
                inputSampleL = lastSampleBL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleBL - threshold;
            }
            lastSampleAL = (lastSampleAL * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end L
            // begin R
            clamp = (lastSampleBR - lastSampleCR) * 0.381966011250105;
            clamp -= (lastSampleAR - lastSampleBR) * 0.6180339887498948482045;
            clamp += inputSampleR - lastSampleAR; // regular slew clamping added
            lastSampleCR = lastSampleBR;
            lastSampleBR = lastSampleAR;
            lastSampleAR = inputSampleR; // now our output relates off lastSampleB
            if (clamp > threshold) {
                inputSampleR = lastSampleBR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleBR - threshold;
            }
            lastSampleAR = (lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end R
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
