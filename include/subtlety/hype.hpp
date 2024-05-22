#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Hype final : public Effect<T>
{
    const std::string m_name{ "Hype" };

    double softL[11];
    double softR[11];
    int cycleEnd;
    // from undersampling code, used as a way to space out HF taps
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

    enum params
    {
        kNumParameters = 0

    };

  public:
    Hype()
    {
        for (int count = 0; count < 10; count++) {
            softL[count] = 0.0;
            softR[count] = 0.0;
        }
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd == 3) {
            cycleEnd = 4;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
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

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

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
            if (cycleEnd == 4) {
                softL[8] = softL[7];
                softL[7] = softL[6];
                softL[6] = softL[5];
                softL[5] = softL[4];
                softL[4] = softL[3];
                softL[3] = softL[2];
                softL[2] = softL[1];
                softL[1] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[7];
                softR[7] = softR[6];
                softR[6] = softR[5];
                softR[5] = softR[4];
                softR[4] = softR[3];
                softR[3] = softR[2];
                softR[2] = softR[1];
                softR[1] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 2) {
                softL[8] = softL[6];
                softL[6] = softL[4];
                softL[4] = softL[2];
                softL[2] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[6];
                softR[6] = softR[4];
                softR[4] = softR[2];
                softR[2] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 1) {
                softL[8] = softL[4];
                softL[4] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[4];
                softR[4] = softR[0];
                softR[0] = inputSampleR;
            }
            softL[9] = ((softL[0] - softL[4]) - (softL[4] - softL[8]));
            if (softL[9] < -1.57079633) {
                softL[9] = -1.57079633;
            }
            if (softL[9] > 1.57079633) {
                softL[9] = 1.57079633;
            }
            inputSampleL = softL[8] + (sin(softL[9]) * 0.61803398);
            softR[9] = ((softR[0] - softR[4]) - (softR[4] - softR[8]));
            if (softR[9] < -1.57079633) {
                softR[9] = -1.57079633;
            }
            if (softR[9] > 1.57079633) {
                softR[9] = 1.57079633;
            }
            inputSampleR = softR[8] + (sin(softR[9]) * 0.61803398);
            // treble softening effect ended up being an aural exciter
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
