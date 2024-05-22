#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class DitherMeDiskers final : public Effect<T>
{
    std::string m_name{ "DitherMeDiskers" };

    double noiseShapingL;
    double noiseShapingR;
    double lastSampleL;
    double lastSample2L;
    double lastSampleR;
    double lastSample2R;

    enum params
    {
        kNumParameters = 0

    };

  public:
    DitherMeDiskers()
    {
        noiseShapingL = 0.0;
        noiseShapingR = 0.0;
        lastSampleL = 0.0;
        lastSample2L = 0.0;
        lastSampleR = 0.0;
        lastSample2R = 0.0;
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

        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1 * 32768.0;
            double inputSampleR = *in2 * 32768.0;
            double outputSampleL;
            double outputSampleR;
            lastSampleL -= (noiseShapingL * 0.125);
            lastSampleR -= (noiseShapingR * 0.125);
            if ((lastSampleL + lastSampleL) >= (inputSampleL + lastSample2L)) {
                outputSampleL = floor(lastSampleL);
            }
            else {
                outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
            }
            if ((lastSampleR + lastSampleR) >= (inputSampleR + lastSample2R)) {
                outputSampleR = floor(lastSampleR);
            }
            else {
                outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
            }
            lastSample2L = lastSampleL;
            lastSampleL = inputSampleL; // we retain three samples in a row
            lastSample2R = lastSampleR;
            lastSampleR = inputSampleR; // we retain three samples in a row
            noiseShapingL += outputSampleL;
            noiseShapingL -= lastSampleL;
            noiseShapingR += outputSampleR;
            noiseShapingR -= lastSampleR;
            if (outputSampleL > 32760.0) {
                outputSampleL = 32760.0;
                noiseShapingL *= 0.5;
            }
            if (outputSampleR > 32760.0) {
                outputSampleR = 32760.0;
                noiseShapingR *= 0.5;
            }
            if (outputSampleL < -32760.0) {
                outputSampleL = -32760.0;
                noiseShapingL *= 0.5;
            }
            if (outputSampleR < -32760.0) {
                outputSampleR = -32760.0;
                noiseShapingR *= 0.5;
            }
            *out1 = outputSampleL / 32768.0;
            *out2 = outputSampleR / 32768.0;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
