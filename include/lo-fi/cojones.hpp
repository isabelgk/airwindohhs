#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Cojones final : public Effect<T>
{
    const std::string m_name{ "Cojones" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedL[2];
    double diffL[6];
    double storedR[2];
    double diffR[6];
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5

    };

  public:
    Cojones()
    {
        A = 0.5; // 0-2
        B = 0.5; // 0-2
        C = 0.5; // 0-2
        D = 1.0;
        E = 1.0;
        storedL[0] = storedL[1] = 0.0;
        diffL[0] = diffL[1] = diffL[2] = diffL[3] = diffL[4] = diffL[5] = 0.0;
        storedR[0] = storedR[1] = 0.0;
        diffR[0] = diffR[1] = diffR[2] = diffR[3] = diffR[4] = diffR[5] = 0.0;
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
            case kParamE: E = value; break;

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
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Breathy";
            case kParamB: return "Cojones";
            case kParamC: return "Body";
            case kParamD: return "Output";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A * 2.0);
            case kParamB: return std::to_string(B * 2.0);
            case kParamC: return std::to_string(C * 2.0);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

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
            case kParamE: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double breathy = A * 2.0;
        double cojones = B * 2.0;
        double body = C * 2.0;
        double output = D;
        double wet = E;
        double averageL[5];
        double averageR[5];
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
            // begin L
            storedL[1] = storedL[0];
            storedL[0] = inputSampleL;
            diffL[5] = diffL[4];
            diffL[4] = diffL[3];
            diffL[3] = diffL[2];
            diffL[2] = diffL[1];
            diffL[1] = diffL[0];
            diffL[0] = storedL[0] - storedL[1];
            averageL[0] = diffL[0] + diffL[1];
            averageL[1] = averageL[0] + diffL[2];
            averageL[2] = averageL[1] + diffL[3];
            averageL[3] = averageL[2] + diffL[4];
            averageL[4] = averageL[3] + diffL[5];
            averageL[0] /= 2.0;
            averageL[1] /= 3.0;
            averageL[2] /= 4.0;
            averageL[3] /= 5.0;
            averageL[4] /= 6.0;
            double meanA = diffL[0];
            double meanB = diffL[0];
            if (fabs(averageL[4]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[4];
            }
            if (fabs(averageL[3]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[3];
            }
            if (fabs(averageL[2]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[2];
            }
            if (fabs(averageL[1]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[1];
            }
            if (fabs(averageL[0]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[0];
            }
            double meanOut = ((meanA + meanB) / 2.0);
            storedL[0] = (storedL[1] + meanOut);
            double outputSample = storedL[0] * body;
            // presubtract cojones
            outputSample += (((inputSampleL - storedL[0]) - averageL[1]) * cojones);
            outputSample += (averageL[1] * breathy);
            inputSampleL = outputSample;
            // end L
            // begin R
            storedR[1] = storedR[0];
            storedR[0] = inputSampleR;
            diffR[5] = diffR[4];
            diffR[4] = diffR[3];
            diffR[3] = diffR[2];
            diffR[2] = diffR[1];
            diffR[1] = diffR[0];
            diffR[0] = storedR[0] - storedR[1];
            averageR[0] = diffR[0] + diffR[1];
            averageR[1] = averageR[0] + diffR[2];
            averageR[2] = averageR[1] + diffR[3];
            averageR[3] = averageR[2] + diffR[4];
            averageR[4] = averageR[3] + diffR[5];
            averageR[0] /= 2.0;
            averageR[1] /= 3.0;
            averageR[2] /= 4.0;
            averageR[3] /= 5.0;
            averageR[4] /= 6.0;
            meanA = diffR[0];
            meanB = diffR[0];
            if (fabs(averageR[4]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[4];
            }
            if (fabs(averageR[3]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[3];
            }
            if (fabs(averageR[2]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[2];
            }
            if (fabs(averageR[1]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[1];
            }
            if (fabs(averageR[0]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[0];
            }
            meanOut = ((meanA + meanB) / 2.0);
            storedR[0] = (storedR[1] + meanOut);
            outputSample = storedR[0] * body;
            // presubtract cojones
            outputSample += (((inputSampleR - storedR[0]) - averageR[1]) * cojones);
            outputSample += (averageR[1] * breathy);
            inputSampleR = outputSample;
            // end R
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
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
