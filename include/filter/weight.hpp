#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Weight final : public Effect<T>
{
    const std::string m_name{ "Weight" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    double previousSampleL[9];
    double previousTrendL[9];
    double previousSampleR[9];
    double previousTrendR[9];

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    Weight()
    {
        A = 0.5;
        B = 0.0;
        for (int i = 0; i < 8; i++) {
            previousSampleL[i] = 0.0;
            previousTrendL[i] = 0.0;
            previousSampleR[i] = 0.0;
            previousTrendR[i] = 0.0;
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
            case kParamA: return "Freq";
            case kParamB: return "Weight";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 100.0) + 20.0);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "hz";
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
        double targetFreq = A;
        // gives us a 0-1 value like the VST will be. For the VST, start with 0-1 and
        // have the plugin display the number as 20-120.
        targetFreq = ((targetFreq + 0.53) * 0.2) / sqrt(overallscale);
        // must use square root of what the real scale would be, to get correct output
        double alpha = pow(targetFreq, 4);
        double out = B;
        double resControl = (out * 0.05) + 0.2;
        double beta = (alpha * pow(resControl, 2));
        alpha += ((1.0 - beta) * pow(targetFreq, 3)); // correct for droop in frequency
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
            // begin Weight
            double trend;
            double forecast;
            for (int i = 0; i < 8; i++) {
                trend = (beta * (inputSampleL - previousSampleL[i]) + ((0.999 - beta) * previousTrendL[i]));
                forecast = previousSampleL[i] + previousTrendL[i];
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleL[i] = inputSampleL;
                previousTrendL[i] = trend;
                trend = (beta * (inputSampleR - previousSampleR[i]) + ((0.999 - beta) * previousTrendR[i]));
                forecast = previousSampleR[i] + previousTrendR[i];
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleR[i] = inputSampleR;
                previousTrendR[i] = trend;
            }
            // inputSample is now the bass boost to be added
            inputSampleL *= out;
            inputSampleR *= out;
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
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
