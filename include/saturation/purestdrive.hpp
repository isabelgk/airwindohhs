#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PurestDrive final : public Effect<T>
{
    std::string m_name{ "PurestDrive" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double previousSampleL;
    double previousSampleR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    PurestDrive()
    {
        A = 0.0;
        previousSampleL = 0.0;
        previousSampleR = 0.0;
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
            case kParamA: return "Drive";

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

        double intensity = A;
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
        double apply;
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
            inputSampleL = sin(inputSampleL);
            // basic distortion factor
            apply = (fabs(previousSampleL + inputSampleL) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleL = (drySampleL * (1.0 - apply)) + (inputSampleL * apply);
            // dry-wet control for intensity also has FM modulation to clean up highs
            previousSampleL = sin(drySampleL);
            // apply the sine while storing previous sample
            inputSampleR = sin(inputSampleR);
            // basic distortion factor
            apply = (fabs(previousSampleR + inputSampleR) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleR = (drySampleR * (1.0 - apply)) + (inputSampleR * apply);
            // dry-wet control for intensity also has FM modulation to clean up highs
            previousSampleR = sin(drySampleR);
            // apply the sine while storing previous sample
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
