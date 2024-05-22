#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class HighGlossDither final : public Effect<T>
{
    std::string m_name{ "HighGlossDither" };

    int Position;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;

    enum params
    {
        kNumParameters = 0

    };

  public:
    HighGlossDither()
    {
        Position = 99999999;
        flip = false;
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

        int hotbinA;
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
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            Position += 1;
            // Note- uses integer overflow as a 'mod' operator
            hotbinA = Position * Position;
            hotbinA = hotbinA % 170003; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 17011; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 1709; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 173; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 17;
            hotbinA *= 0.0635;
            if (flip) {
                hotbinA = -hotbinA;
            }
            inputSampleL += hotbinA;
            inputSampleR += hotbinA;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            // Quadratic dither
            flip = !flip;
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
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
