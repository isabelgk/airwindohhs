#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Interstage final : public Effect<T>
{
    std::string m_name{ "Interstage" };

    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double lastSampleL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;

    enum params
    {
        kNumParameters = 0

    };

  public:
    Interstage()
    {
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        lastSampleL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        lastSampleR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = true;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double firstStage = 0.381966011250105 / overallscale;
        double iirAmount = 0.00295 / overallscale;
        double threshold = 0.381966011250105;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            inputSampleL = (inputSampleL + lastSampleL) * 0.5;
            inputSampleR = (inputSampleR + lastSampleR) * 0.5; // start the lowpassing with an average
            if (flip) {
                iirSampleAL = (iirSampleAL * (1 - firstStage)) + (inputSampleL * firstStage);
                inputSampleL = iirSampleAL;
                iirSampleCL = (iirSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleCL;
                iirSampleEL = (iirSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleEL;
                inputSampleL = drySampleL - inputSampleL;
                // make highpass
                if (inputSampleL - iirSampleAL > threshold) {
                    inputSampleL = iirSampleAL + threshold;
                }
                if (inputSampleL - iirSampleAL < -threshold) {
                    inputSampleL = iirSampleAL - threshold;
                }
                // slew limit against lowpassed reference point
                iirSampleAR = (iirSampleAR * (1 - firstStage)) + (inputSampleR * firstStage);
                inputSampleR = iirSampleAR;
                iirSampleCR = (iirSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleCR;
                iirSampleER = (iirSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleER;
                inputSampleR = drySampleR - inputSampleR;
                // make highpass
                if (inputSampleR - iirSampleAR > threshold) {
                    inputSampleR = iirSampleAR + threshold;
                }
                if (inputSampleR - iirSampleAR < -threshold) {
                    inputSampleR = iirSampleAR - threshold;
                }
                // slew limit against lowpassed reference point
            }
            else {
                iirSampleBL = (iirSampleBL * (1 - firstStage)) + (inputSampleL * firstStage);
                inputSampleL = iirSampleBL;
                iirSampleDL = (iirSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleDL;
                iirSampleFL = (iirSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleFL;
                inputSampleL = drySampleL - inputSampleL;
                // make highpass
                if (inputSampleL - iirSampleBL > threshold) {
                    inputSampleL = iirSampleBL + threshold;
                }
                if (inputSampleL - iirSampleBL < -threshold) {
                    inputSampleL = iirSampleBL - threshold;
                }
                // slew limit against lowpassed reference point
                iirSampleBR = (iirSampleBR * (1 - firstStage)) + (inputSampleR * firstStage);
                inputSampleR = iirSampleBR;
                iirSampleDR = (iirSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleDR;
                iirSampleFR = (iirSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleFR;
                inputSampleR = drySampleR - inputSampleR;
                // make highpass
                if (inputSampleR - iirSampleBR > threshold) {
                    inputSampleR = iirSampleBR + threshold;
                }
                if (inputSampleR - iirSampleBR < -threshold) {
                    inputSampleR = iirSampleBR - threshold;
                }
                // slew limit against lowpassed reference point
            }
            flip = !flip;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
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
