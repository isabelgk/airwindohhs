#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BrightAmbience2 final : public Effect<T>
{
    const std::string m_name{ "BrightAmbience2" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    int gcount;
    float pL[32768];
    float pR[32768];
    double feedbackA;
    double feedbackB;
    double feedbackC;
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamfor(int count = 0,
kParamfeedbackA = 1,
kParamgcount = 2,
kParamA = 3,
kNumParameters = 4

    };

  public:
    BrightAmbience2()
    {
        for (int count = 0; count < 32767; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        feedbackA = feedbackB = feedbackC = 0.0;
        gcount = 0;
        A = 0.2;
        B = 0.2;
        C = 0.0;
        D = 0.5;
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
        case kParamfor(int count: for(int count = value; break;
case kParamfeedbackA: feedbackA = value; break;
case kParamgcount: gcount = value; break;
case kParamA: A = value; break;

        default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return for(int count;
case kParamfeedbackA: return feedbackA;
case kParamgcount: return gcount;
case kParamA: return A;

        default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return "Start";
        case kParamfeedbackA: return "Length";
        case kParamgcount: return "Feedbck";
        case kParamA: return "Dry/Wet";

        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return std::to_string(A);
        case kParamfeedbackA: return std::to_string(B);
        case kParamgcount: return std::to_string(C);
        case kParamA: return std::to_string(D);

        default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return "";
        case kParamfeedbackA: return "";
        case kParamgcount: return "";
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

        int start = (int)(pow(A, 2) * 480) + 1;
        int length = (int)(pow(B, 2) * 480) + 1;
        if (start + length > 488) {
            start = 488 - length;
        }
        double feedbackAmount = 1.0 - (pow(1.0 - C, 2));
        double wet = D;
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
            double tempL = 0.0;
            double tempR = 0.0;
            if (gcount < 0 || gcount > 32767) {
                gcount = 32767;
            }
            int count = gcount;
            pL[count] = inputSampleL + ((sin(feedbackA) / sqrt(length + 1)) * feedbackAmount);
            pR[count] = inputSampleR + ((sin(feedbackB) / sqrt(length + 1)) * feedbackAmount);
            for (int offset = start; offset < start + length; offset++) {
                tempL += pL[count + primeL[offset] - ((count + primeL[offset] > 32767) ? 32768 : 0)];
                tempR += pR[count + primeR[offset] - ((count + primeR[offset] > 32767) ? 32768 : 0)];
            }
            inputSampleL = tempL / sqrt(length);
            inputSampleR = tempR / sqrt(length);
            feedbackA = (feedbackA * (1.0 - feedbackAmount)) + (inputSampleR * feedbackAmount);
            feedbackB = (feedbackB * (1.0 - feedbackAmount)) + (inputSampleL * feedbackAmount);
            gcount--;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
