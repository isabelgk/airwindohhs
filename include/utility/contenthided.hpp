#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ContentHideD final : public Effect<T>
{
    const std::string m_name{ "ContentHideD" };

    double demotimer;
    uint32_t fpd;
    // default stuff

    enum params
    {
        kNumParameters = 0

    };

  public:
    ContentHideD()
    {
        demotimer = 0.0;
        fpd = 1.0;
        while (fpd < 16386) {
            fpd = rand() * UINT32_MAX;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            double duck = 0.0;
            demotimer -= (1.0 / overallscale);
            if (demotimer < 441.0) {
                duck = 1.0 - (demotimer / 441.0);
            }
            if (demotimer > 44100.0) {
                duck = ((demotimer - 44100.0) / 441.0);
            }
            if (duck > 1.0) {
                duck = 1.0;
            }
            duck = sin((1.0 - duck) * 1.57);
            if ((demotimer < 1) || (demotimer > 441000.0)) {
                double randy = (double(fpd) / (double)UINT32_MAX); // 0 to 1 the noise, may not be needed
                demotimer = 100000.0 * (randy + 2.0);
            }
            inputSampleL *= duck;
            inputSampleR *= duck;
            fpd ^= fpd << 13;
            fpd ^= fpd >> 17;
            fpd ^= fpd << 5;
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
