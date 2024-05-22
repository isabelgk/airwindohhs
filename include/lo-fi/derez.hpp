#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class DeRez final : public Effect<T>
{
    const std::string m_name{ "DeRez" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double heldSampleL;
    double lastSampleR;
    double heldSampleR;
    double position;
    double incrementA;
    double incrementB;
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    DeRez()
    {
        A = 1.0;
        B = 1.0;
        lastSampleL = 0.0;
        heldSampleL = 0.0;
        lastSampleR = 0.0;
        heldSampleR = 0.0;
        position = 0.0;
        incrementA = 0.0;
        incrementB = 0.0;
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
            case kParamA: return "Rate";
            case kParamB: return "Rez";

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
        double targetA = pow(A, 3) + 0.0005;
        if (targetA > 1.0) {
            targetA = 1.0;
        }
        double soften = (1.0 + targetA) / 2;
        double targetB = pow(1.0 - B, 3) / 3;
        targetA /= overallscale;
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23 && (targetB == 0)) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23 && (targetB == 0)) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            incrementA = ((incrementA * 999.0) + targetA) / 1000.0;
            incrementB = ((incrementB * 999.0) + targetB) / 1000.0;
            // incrementA is the frequency derez
            // incrementB is the bit depth derez
            position += incrementA;
            double outputSampleL = heldSampleL;
            double outputSampleR = heldSampleR;
            if (position > 1.0)
            {
                position -= 1.0;
                heldSampleL = (lastSampleL * position) + (inputSampleL * (1.0 - position));
                outputSampleL = (outputSampleL * (1.0 - soften)) + (heldSampleL * soften);
                // softens the edge of the derez
                heldSampleR = (lastSampleR * position) + (inputSampleR * (1.0 - position));
                outputSampleR = (outputSampleR * (1.0 - soften)) + (heldSampleR * soften);
                // softens the edge of the derez
            }
            inputSampleL = outputSampleL;
            inputSampleR = outputSampleR;
            double offset;
            if (incrementB > 0.0005)
            {
                if (inputSampleL > 0)
                {
                    offset = inputSampleL;
                    while (offset > 0) {
                        offset -= incrementB;
                    }
                    inputSampleL -= offset;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleR > 0)
                {
                    offset = inputSampleR;
                    while (offset > 0) {
                        offset -= incrementB;
                    }
                    inputSampleR -= offset;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleL < 0)
                {
                    offset = inputSampleL;
                    while (offset < 0) {
                        offset += incrementB;
                    }
                    inputSampleL -= offset;
                    // it's above 0 so subtracting subtracts the remainder
                }
                if (inputSampleR < 0)
                {
                    offset = inputSampleR;
                    while (offset < 0) {
                        offset += incrementB;
                    }
                    inputSampleR -= offset;
                    // it's above 0 so subtracting subtracts the remainder
                }
                inputSampleL *= (1.0 - incrementB);
                inputSampleR *= (1.0 - incrementB);
            }
            lastSampleL = drySampleL;
            lastSampleR = drySampleR;
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
