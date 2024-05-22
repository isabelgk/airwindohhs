#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Isolator final : public Effect<T>
{
    const std::string m_name{ "Isolator" };

    double biquadA[15];
    double biquadB[15];
    double biquadC[15]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8-9-10 and 11-12-13-14, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParamfor (int x = 0,
kParamA = 1,
kParamB = 2,
kNumParameters = 3

    };

  public:
    Isolator()
    {
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
        }
        A = 1.0;
        B = 0.0;
        C = 1.0;
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
        case kParamfor (int x: for (int x = value; break;
case kParamA: A = value; break;
case kParamB: B = value; break;

        default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return for (int x;
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
        case kParamfor (int x: return "Freq";
        case kParamA: return "High";
        case kParamB: return "Low";

        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return std::to_string(A);
        case kParamA: return std::to_string(B);
        case kParamB: return std::to_string(C);

        default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "";
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
        bool bypass = (A == 1.0);
        double high = B;
        double low = C; // this gives us shelving, and
        // the ability to use the isolator as a highpass
        if (high > 0.0) {
            bypass = false;
        }
        if (low < 1.0) {
            bypass = false;
        }
        biquadA[0] = pow(A, (2.0 * sqrt(overallscale))) * 0.4999;
        if (biquadA[0] < 0.001) {
            biquadA[0] = 0.001;
        }
        biquadC[0] = biquadB[0] = biquadA[0];
        biquadA[1] = 0.5;
        biquadB[1] = 0.618033988749894848204586;
        biquadC[1] = 1.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        // there is a form for highpass
        // but I would suggest subtracting the lowpass from dry
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
            double drySampleL = *in1;
            double drySampleR = *in2;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            outSampleL = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
            biquadB[8] = biquadB[7];
            biquadB[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadB[10] = biquadB[9];
            biquadB[9] = inputSampleL; // DF1 left
            outSampleL = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
            biquadC[8] = biquadC[7];
            biquadC[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadC[10] = biquadC[9];
            biquadC[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            outSampleR = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
            biquadB[12] = biquadB[11];
            biquadB[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadB[14] = biquadB[13];
            biquadB[13] = inputSampleR; // DF1 right
            outSampleR = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
            biquadC[12] = biquadC[11];
            biquadC[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadC[14] = biquadC[13];
            biquadC[13] = inputSampleR; // DF1 right
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            if (bypass) {
                inputSampleL = drySampleL;
                inputSampleR = drySampleR;
            }
            else {
                inputSampleL = (inputSampleL * low) + ((drySampleL - inputSampleL) * high);
                inputSampleR = (inputSampleR * low) + ((drySampleR - inputSampleR) * high);
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
