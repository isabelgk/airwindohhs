#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Coils2 final : public Effect<T>
{
    std::string m_name{ "Coils2" };

    double biquadA[15];
    double biquadB[15];
    double hysteresisL;
    double hysteresisR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    Coils2()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
        }
        hysteresisL = hysteresisR = 0.0;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Saturate";
            case kParamB: return "Cheapness";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
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
        double distScaling = pow(1.0 - A, 2);
        if (distScaling < 0.0001) {
            distScaling = 0.0001;
        }
        biquadA[0] = 600.0 / getSampleRate();
        biquadA[1] = 0.01 + (pow(B, 2) * 0.5);
        double iirAmount = biquadA[1] / overallscale;
        double K = tan(M_PI * biquadA[0]);
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K / biquadA[1] * norm;
        biquadA[4] = -biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        biquadB[0] = (21890.0 - (biquadA[1] * 890.0)) / getSampleRate();
        biquadB[1] = 0.89;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        double wet = C;
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
            if (biquadA[0] < 0.49999) {
                double tempSample = (inputSampleL * biquadA[2]) + biquadA[7];
                biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
                biquadA[8] = (inputSampleL * biquadA[4]) - (tempSample * biquadA[6]);
                inputSampleL = tempSample;
                tempSample = (inputSampleR * biquadA[2]) + biquadA[9];
                biquadA[9] = -(tempSample * biquadA[5]) + biquadA[10];
                biquadA[10] = (inputSampleR * biquadA[4]) - (tempSample * biquadA[6]);
                inputSampleR = tempSample; // create bandpass of clean tone
            }
            double diffSampleL = (drySampleL - inputSampleL) / distScaling;
            double diffSampleR = (drySampleR - inputSampleR) / distScaling; // mids notched out
            if (biquadB[0] < 0.49999) {
                double tempSample = (diffSampleL * biquadB[2]) + biquadB[7];
                biquadB[7] = (diffSampleL * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
                biquadB[8] = (diffSampleL * biquadB[4]) - (tempSample * biquadB[6]);
                diffSampleL = tempSample;
                tempSample = (diffSampleR * biquadB[2]) + biquadB[9];
                biquadB[9] = (diffSampleR * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[10];
                biquadB[10] = (diffSampleR * biquadB[4]) - (tempSample * biquadB[6]);
                diffSampleR = tempSample;
            }
            hysteresisL = (hysteresisL * (1.0 - iirAmount)) + (diffSampleL * iirAmount);
            if (fabs(hysteresisL) < 1.18e-37) {
                hysteresisL = 0.0;
            }
            else {
                diffSampleL -= hysteresisL;
            }
            if (diffSampleL > 1.571) {
                diffSampleL = 1.571;
            }
            else if (diffSampleL < -1.571) {
                diffSampleL = -1.571;
            }
            if (hysteresisL > 1.571) {
                hysteresisL = 1.571;
            }
            else if (hysteresisL < -1.571) {
                hysteresisL = -1.571;
            }
            hysteresisR = (hysteresisR * (1.0 - iirAmount)) + (diffSampleR * iirAmount);
            if (fabs(hysteresisR) < 1.18e-37) {
                hysteresisR = 0.0;
            }
            else {
                diffSampleR -= hysteresisR;
            }
            if (diffSampleR > 1.571) {
                diffSampleR = 1.571;
            }
            else if (diffSampleR < -1.571) {
                diffSampleR = -1.571;
            }
            if (hysteresisR > 1.571) {
                hysteresisR = 1.571;
            }
            else if (hysteresisR < -1.571) {
                hysteresisR = -1.571;
            }
            inputSampleL += (sin(diffSampleL) - sin(hysteresisL)) * distScaling;
            inputSampleR += (sin(diffSampleR) - sin(hysteresisR)) * distScaling; // apply transformer distortions
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
