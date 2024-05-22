#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Pear2 final : public Effect<T>
{
    const std::string m_name{ "Pear2" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        prevSampL7,
        prevSlewL7,
        prevSampR7,
        prevSlewR7,
        prevSampL8,
        prevSlewL8,
        prevSampR8,
        prevSlewR8,
        prevSampL9,
        prevSlewL9,
        prevSampR9,
        prevSlewR9,
        prevSampL10,
        prevSlewL10,
        prevSampR10,
        prevSlewR10,
        prevSampL11,
        prevSlewL11,
        prevSampR11,
        prevSlewR11,
        prevSampL12,
        prevSlewL12,
        prevSampR12,
        prevSlewR12,
        prevSampL13,
        prevSlewL13,
        prevSampR13,
        prevSlewR13,
        prevSampL14,
        prevSlewL14,
        prevSampR14,
        prevSlewR14,
        prevSampL15,
        prevSlewL15,
        prevSampR15,
        prevSlewR15,
        prevSampL16,
        prevSlewL16,
        prevSampR16,
        prevSlewR16,
        pear_total
    }; // fixed frequency pear filter for ultrasonics, stereo
    double pear[pear_total]; // probably worth just using a number here
    double freqA;
    double freqB;
    double nonLinA;
    double nonLinB;
    double wetA;
    double wetB;
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    Pear2()
    {
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        for (int x = 0; x < pear_total; x++) {
            pear[x] = 0.0;
        }
        freqA = freqB = 0.5;
        nonLinA = nonLinB = 0.0;
        wetA = wetB = 0.5;
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
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Freq";
            case kParamB: return "NonLin";
            case kParamC: return "Poles";
            case kParamD: return "Inv/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        freqA = freqB;
        freqB = pow(A, 2);
        if (freqB < 0.0001) {
            freqB = 0.0001;
        }
        nonLinA = nonLinB;
        nonLinB = pow(B, 2);
        double maxPoles = pow(C, 3) * pear_total;
        wetA = wetB;
        wetB = D; // inv-dry-wet for highpass
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double freq = (freqA * temp) + (freqB * (1.0 - temp));
            double nonLin = (nonLinA * temp) + (nonLinB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            double dry = 2.0 - (wet * 2.0);
            if (dry > 1.0) {
                dry = 1.0; // full dry for use with inv, to 0.0 at full wet
            }
            wet = (wet * 2.0) - 1.0; // inv-dry-wet for highpass
            for (int x = 0; x < maxPoles; x += 4) {
                double di = fabs(freq * (1.0 + (inputSampleL * nonLin)));
                if (di > 1.0) {
                    di = 1.0;
                }
                double slew = ((inputSampleL - pear[x]) + pear[x + 1]) * di * 0.5;
                pear[x] = inputSampleL = (di * inputSampleL) + ((1.0 - di) * (pear[x] + pear[x + 1]));
                pear[x + 1] = slew;
                di = fabs(freq * (1.0 + (inputSampleR * nonLin)));
                if (di > 1.0) {
                    di = 1.0;
                }
                slew = ((inputSampleR - pear[x + 2]) + pear[x + 3]) * di * 0.5;
                pear[x + 2] = inputSampleR = (di * inputSampleR) + ((1.0 - di) * (pear[x + 2] + pear[x + 3]));
                pear[x + 3] = slew;
            }
            inputSampleL *= wet;
            inputSampleR *= wet;
            drySampleL *= dry;
            drySampleR *= dry;
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