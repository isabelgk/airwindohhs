#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PlatinumSlew final : public Effect<T>
{
    const std::string m_name{ "PlatinumSlew" };

    enum
    {
        prevSampL1,
        prevSampR1,
        invSampL1,
        invSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        invSampL2,
        invSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        invSampL3,
        invSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        invSampL4,
        invSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        invSampL5,
        invSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        invSampL6,
        invSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        invSampL7,
        invSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        invSampL8,
        invSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        invSampL9,
        invSampR9,
        threshold9,
        prevSampL10,
        prevSampR10,
        invSampL10,
        invSampR10,
        threshold10,
        gslew_total
    }; // fixed frequency pear filter for ultrasonics, stereo
    double gslew[gslew_total]; // probably worth just using a number here
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
    PlatinumSlew()
    {
        A = 0.0;
        for (int x = 0; x < gslew_total; x++) {
            gslew[x] = 0.0;
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
            case kParamA: return "Slew";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double source = pow(1 - A, 4) / overallscale;
        gslew[threshold10] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold9] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold8] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold7] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold6] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold5] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold4] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold3] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold2] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold1] = source;
        source *= 1.618033988749894848204586;
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
            for (int x = 0; x < gslew_total; x += 5) {
                if (((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * 0.156)) + (gslew[x + 4] * 0.844);
                }
                if (-((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * 0.2)) - (gslew[x + 4] * 0.8);
                }
                gslew[x + 2] = gslew[x] * 0.844;
                gslew[x] = inputSampleL;
                if (((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * 0.156)) + (gslew[x + 4] * 0.844);
                }
                if (-((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * 0.2)) - (gslew[x + 4] * 0.8);
                }
                gslew[x + 3] = gslew[x + 1] * 0.844;
                gslew[x + 1] = inputSampleR;
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
