#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SlewOnly final : public Effect<T>
{
    std::string m_name{ "SlewOnly" };

    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;

    enum params
    {
        kNumParameters = 0

    };

  public:
    SlewOnly()
    {
        lastSampleL = 0.0;
        lastSampleR = 0.0;
    }

    constexpr std::string_view name()
    {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
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
        double outputSampleL;
        double outputSampleR;
        double trim = 2.302585092994045684017991; // natural logarithm of 10
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
            outputSampleL = (inputSampleL - lastSampleL) * trim;
            outputSampleR = (inputSampleR - lastSampleR) * trim;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            if (outputSampleL > 1.0) {
                outputSampleL = 1.0;
            }
            if (outputSampleR > 1.0) {
                outputSampleR = 1.0;
            }
            if (outputSampleL < -1.0) {
                outputSampleL = -1.0;
            }
            if (outputSampleR < -1.0) {
                outputSampleR = -1.0;
            }
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
