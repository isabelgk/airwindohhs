#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Gringer final : public Effect<T>
{
    const std::string m_name{ "Gringer" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double inbandL[9];
    double outbandL[9];
    double inbandR[9];
    double outbandR[9];

    enum params
    {
        kNumParameters = 0

    };

  public:
    Gringer()
    {
        for (int x = 0; x < 9; x++) {
            inbandL[x] = 0.0;
            outbandL[x] = 0.0;
            inbandR[x] = 0.0;
            outbandR[x] = 0.0;
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
        inbandL[0] = 0.025 / overallscale;
        outbandL[0] = 0.025 / overallscale;
        inbandL[1] = 0.001;
        outbandL[1] = 0.001;
        inbandR[0] = 0.025 / overallscale;
        outbandR[0] = 0.025 / overallscale;
        inbandR[1] = 0.001;
        outbandR[1] = 0.001;
        // hardwired for wide bandpass around the rectification
        double K = tan(M_PI * inbandL[0]);
        double norm = 1.0 / (1.0 + K / inbandL[1] + K * K);
        inbandL[2] = K / inbandL[1] * norm;
        inbandL[4] = -inbandL[2];
        inbandL[5] = 2.0 * (K * K - 1.0) * norm;
        inbandL[6] = (1.0 - K / inbandL[1] + K * K) * norm;
        K = tan(M_PI * outbandL[0]);
        norm = 1.0 / (1.0 + K / outbandL[1] + K * K);
        outbandL[2] = K / outbandL[1] * norm;
        outbandL[4] = -outbandL[2];
        outbandL[5] = 2.0 * (K * K - 1.0) * norm;
        outbandL[6] = (1.0 - K / outbandL[1] + K * K) * norm;
        K = tan(M_PI * inbandR[0]);
        norm = 1.0 / (1.0 + K / inbandR[1] + K * K);
        inbandR[2] = K / inbandR[1] * norm;
        inbandR[4] = -inbandR[2];
        inbandR[5] = 2.0 * (K * K - 1.0) * norm;
        inbandR[6] = (1.0 - K / inbandR[1] + K * K) * norm;
        K = tan(M_PI * outbandR[0]);
        norm = 1.0 / (1.0 + K / outbandR[1] + K * K);
        outbandR[2] = K / outbandR[1] * norm;
        outbandR[4] = -outbandR[2];
        outbandR[5] = 2.0 * (K * K - 1.0) * norm;
        outbandR[6] = (1.0 - K / outbandR[1] + K * K) * norm;
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double tempSample = (inputSampleL * inbandL[2]) + inbandL[7];
            inbandL[7] = -(tempSample * inbandL[5]) + inbandL[8];
            inbandL[8] = (inputSampleL * inbandL[4]) - (tempSample * inbandL[6]);
            inputSampleL = fabs(tempSample);
            // this is all you gotta do to make the Green Ringer fullwave rectification effect
            // the rest is about making it work within a DAW context w. filtering and such
            tempSample = (inputSampleR * inbandR[2]) + inbandR[7];
            inbandR[7] = -(tempSample * inbandR[5]) + inbandR[8];
            inbandR[8] = (inputSampleR * inbandR[4]) - (tempSample * inbandR[6]);
            inputSampleR = fabs(tempSample);
            // this is all you gotta do to make the Green Ringer fullwave rectification effect
            // the rest is about making it work within a DAW context w. filtering and such
            tempSample = (inputSampleL * outbandL[2]) + outbandL[7];
            outbandL[7] = -(tempSample * outbandL[5]) + outbandL[8];
            outbandL[8] = (inputSampleL * outbandL[4]) - (tempSample * outbandL[6]);
            inputSampleL = tempSample;
            tempSample = (inputSampleR * outbandR[2]) + outbandR[7];
            outbandR[7] = -(tempSample * outbandR[5]) + outbandR[8];
            outbandR[8] = (inputSampleR * outbandR[4]) - (tempSample * outbandR[6]);
            inputSampleR = tempSample;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
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
