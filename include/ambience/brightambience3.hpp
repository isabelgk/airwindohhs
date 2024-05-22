#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BrightAmbience3 final : public Effect<T>
{
    std::string m_name{ "BrightAmbience3" };

    int gcount;
    double pL[32768];
    double pR[32768];
    double feedbackA;
    double feedbackB;
    double figureL[9];
    double figureR[9];
    double lastRefL[10];
    double lastRefR[10];
    int cycle;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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
    BrightAmbience3()
    {
        A = 1.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        for (int count = 0; count < 32767; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        feedbackA = feedbackB = 0.0;
        for (int x = 0; x < 9; x++) {
            figureL[x] = 0.0;
            figureR[x] = 0.0;
        }
        for (int c = 0; c < 9; c++) {
            lastRefL[c] = 0.0;
            lastRefR[c] = 0.0;
        }
        cycle = 0;
        gcount = 0;
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
            case kParamD: D = value; break;

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
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Start";
            case kParamB: return "Length";
            case kParamC: return "Feedback";
            case kParamD: return "Dry/Wet";

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
            case kParamD: return std::to_string(D);

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
            case kParamD: return "";
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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        if (cycle > cycleEnd - 1) {
            cycle = cycleEnd - 1; // sanity check
        }
        int start = (int)(A * 400) + 88;
        int length = (int)(pow(B, 2) * 487) + 1;
        if (start + length > 488) {
            start = 488 - length;
        }
        double feedbackAmount = C * 0.25;
        double wet = D;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        figureL[0] = figureR[0] = 1000.0 / getSampleRate(); // fixed frequency, 3.515775k
        figureL[1] = figureR[1] = pow(length * 0.037 * feedbackAmount, 2) + 0.01; // resonance
        double K = tan(M_PI * figureR[0]);
        double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
        figureL[2] = figureR[2] = K / figureR[1] * norm;
        figureL[4] = figureR[4] = -figureR[2];
        figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
        figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
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
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do an Air sample
                double tempL = 0.0;
                double tempR = 0.0;
                if (gcount < 0 || gcount > 32767) {
                    gcount = 32767;
                }
                int count = gcount;
                pL[count] = inputSampleL + feedbackB;
                pR[count] = inputSampleR + feedbackA;
                for (int offset = start; offset < start + length; offset++) {
                    tempL += pL[count + primeL[offset] - ((count + primeL[offset] > 32767) ? 32768 : 0)];
                    tempR += pR[count + primeR[offset] - ((count + primeR[offset] > 32767) ? 32768 : 0)];
                }
                inputSampleL = tempL / cbrt(length);
                inputSampleR = tempR / cbrt(length);
                double tempSample = (inputSampleL * figureL[2]) + figureL[7];
                figureL[7] = -(tempSample * figureL[5]) + figureL[8];
                figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
                feedbackA = sin(tempSample) * feedbackAmount;
                tempSample = (inputSampleR * figureR[2]) + figureR[7];
                figureR[7] = -(tempSample * figureR[5]) + figureR[8];
                figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
                feedbackB = sin(tempSample) * feedbackAmount;
                gcount--;
                if (cycleEnd == 4) {
                    lastRefL[0] = lastRefL[4]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[1] = (lastRefL[0] + lastRefL[2]) / 2; // one quarter
                    lastRefL[3] = (lastRefL[2] + inputSampleL) / 2; // three quarters
                    lastRefL[4] = inputSampleL; // full
                    lastRefR[0] = lastRefR[4]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[1] = (lastRefR[0] + lastRefR[2]) / 2; // one quarter
                    lastRefR[3] = (lastRefR[2] + inputSampleR) / 2; // three quarters
                    lastRefR[4] = inputSampleR; // full
                }
                if (cycleEnd == 3) {
                    lastRefL[0] = lastRefL[3]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + lastRefL[0] + inputSampleL) / 3; // third
                    lastRefL[1] = (lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
                    lastRefL[3] = inputSampleL; // full
                    lastRefR[0] = lastRefR[3]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + lastRefR[0] + inputSampleR) / 3; // third
                    lastRefR[1] = (lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
                    lastRefR[3] = inputSampleR; // full
                }
                if (cycleEnd == 2) {
                    lastRefL[0] = lastRefL[2]; // start from previous last
                    lastRefL[1] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[2] = inputSampleL; // full
                    lastRefR[0] = lastRefR[2]; // start from previous last
                    lastRefR[1] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[2] = inputSampleR; // full
                }
                if (cycleEnd == 1) {
                    lastRefL[0] = inputSampleL;
                    lastRefR[0] = inputSampleR;
                }
                cycle = 0; // reset
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
            }
            else {
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
                // we are going through our references now
            }
            switch (cycleEnd) // multi-pole average using lastRef[] variables
            {
                case 4:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[7]) * 0.5;
                    lastRefL[7] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[7]) * 0.5;
                    lastRefR[7] = lastRefR[8]; // continue, do not break
                case 3:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[6]) * 0.5;
                    lastRefL[6] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[6]) * 0.5;
                    lastRefR[6] = lastRefR[8]; // continue, do not break
                case 2:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[5]) * 0.5;
                    lastRefL[5] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[5]) * 0.5;
                    lastRefR[5] = lastRefR[8]; // continue, do not break
                case 1:
                    break; // no further averaging
            }
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs
