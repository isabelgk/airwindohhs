#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class C5RawBuss final : public Effect<T>
{
    const std::string m_name{ "C5RawBuss" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastFXBussL;
    double lastSampleBussL;
    double lastFXBussR;
    double lastSampleBussR;
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    C5RawBuss()
    {
        A = 0.0;
        lastFXBussL = 0.0;
        lastSampleBussL = 0.0;
        lastFXBussR = 0.0;
        lastSampleBussR = 0.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Center";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double centering = A * 0.5;
        centering = 1.0 - pow(centering, 5);
        // we can set our centering force from zero to rather high, but
        // there's a really intense taper on it forcing it to mostly be almost 1.0.
        // If it's literally 1.0, we don't even apply it, and you get the original
        // Xmas Morning bugged-out Console5, which is the default setting for Raw Console5.
        double differenceL;
        double differenceR;
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
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL = asin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            differenceL = lastSampleBussL - inputSampleL;
            lastSampleBussL = inputSampleL;
            // derive slew part off direct sample measurement + from last time
            differenceR = lastSampleBussR - inputSampleR;
            lastSampleBussR = inputSampleR;
            // derive slew part off direct sample measurement + from last time
            if (differenceL > 1.57079633) {
                differenceL = 1.57079633;
            }
            if (differenceL < -1.57079633) {
                differenceL = -1.57079633;
            }
            if (differenceR > 1.57079633) {
                differenceR = 1.57079633;
            }
            if (differenceR < -1.57079633) {
                differenceR = -1.57079633;
            }
            inputSampleL = lastFXBussL + sin(differenceL);
            lastFXBussL = inputSampleL;
            if (centering < 1.0) {
                lastFXBussL *= centering;
            }
            // if we're using the crude centering force, it's applied here
            inputSampleR = lastFXBussR + sin(differenceR);
            lastFXBussR = inputSampleR;
            if (centering < 1.0) {
                lastFXBussR *= centering;
            }
            // if we're using the crude centering force, it's applied here
            if (lastFXBussL > 1.0) {
                lastFXBussL = 1.0;
            }
            if (lastFXBussL < -1.0) {
                lastFXBussL = -1.0;
            }
            // build new signal off what was present in output last time
            if (lastFXBussR > 1.0) {
                lastFXBussR = 1.0;
            }
            if (lastFXBussR < -1.0) {
                lastFXBussR = -1.0;
            }
            // build new signal off what was present in output last time
            // slew aspect
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
