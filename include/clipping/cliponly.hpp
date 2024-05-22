#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ClipOnly final : public Effect<T>
{
    const std::string m_name{ "ClipOnly" };

    double lastSampleL;
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    bool wasPosClipR;
    bool wasNegClipR;

    enum params
    {
        kNumParameters = 0

    };

  public:
    ClipOnly()
    {
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
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

        double hardness = 0.7390851332151606; // x == cos(x)
        double softness = 1.0 - hardness;
        double refclip = 0.9549925859; // -0.2dB
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) { // next one will NOT be over
                    lastSampleL = ((refclip * hardness) + (inputSampleL * softness));
                }
                else { // still clipping, still chasing the target
                    lastSampleL = ((lastSampleL * hardness) + (refclip * softness));
                }
            }
            wasPosClipL = false;
            if (inputSampleL > refclip) { // next will be over the true clip level. otherwise we directly use it
                wasPosClipL = true; // set the clip flag
                inputSampleL = ((refclip * hardness) + (lastSampleL * softness));
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) { // next one will NOT be -over
                    lastSampleL = ((-refclip * hardness) + (inputSampleL * softness));
                }
                else { // still clipping, still chasing the target
                    lastSampleL = ((lastSampleL * hardness) + (-refclip * softness));
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -refclip) { // next will be -refclip.  otherwise we directly use it
                wasNegClipL = true; // set the clip flag
                inputSampleL = ((-refclip * hardness) + (lastSampleL * softness));
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) { // next one will NOT be over
                    lastSampleR = ((refclip * hardness) + (inputSampleR * softness));
                }
                else { // still clipping, still chasing the target
                    lastSampleR = ((lastSampleR * hardness) + (refclip * softness));
                }
            }
            wasPosClipR = false;
            if (inputSampleR > refclip) { // next will be over the true clip level. otherwise we directly use it
                wasPosClipR = true; // set the clip flag
                inputSampleR = ((refclip * hardness) + (lastSampleR * softness));
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) { // next one will NOT be -over
                    lastSampleR = ((-refclip * hardness) + (inputSampleR * softness));
                }
                else { // still clipping, still chasing the target
                    lastSampleR = ((lastSampleR * hardness) + (-refclip * softness));
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -refclip) { // next will be -refclip.  otherwise we directly use it
                wasNegClipR = true; // set the clip flag
                inputSampleR = ((-refclip * hardness) + (lastSampleR * softness));
            }
            *out1 = lastSampleL;
            *out2 = lastSampleR;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
