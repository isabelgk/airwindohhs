#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Melt final : public Effect<T>
{
    std::string m_name{ "Melt" };

    float dL[32002];
    float dR[32002];
    float combineL;
    float combineR;
    float scalefactorL;
    float scalefactorR;
    int minTap[32];
    int maxTap[32];
    int position[32];
    int stepTap[32];
    int stepCount;
    int slowCount;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    Melt()
    {
        A = 0.25;
        B = 0.75;
        C = 1.0;
        D = 1.0;
        for (int count = 0; count < 32001; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        combineL = 0;
        combineR = 0;
        scalefactorL = 0.999;
        scalefactorR = 0.999;
        for (int count = 0; count < 31; count++) {
            minTap[count] = 0;
            maxTap[count] = 0;
            position[count] = 1;
            stepTap[count] = 1;
        }
        stepCount = 0;
        slowCount = 0;
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
            case kParamA: return "Depth";
            case kParamB: return "Range";
            case kParamC: return "Output";
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
            case kParamA: return " ";
            case kParamB: return " ";
            case kParamC: return " ";
            case kParamD: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double rate = 1 / (pow(A, 2) + 0.001);
        double depthB = (B * 139.5) + 2;
        double depthA = depthB * (1.0 - A);
        double output = C * 0.05;
        double wet = D;
        double dry = 1.0 - wet;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        minTap[0] = floor(2 * depthA);
        maxTap[0] = floor(2 * depthB);
        minTap[1] = floor(3 * depthA);
        maxTap[1] = floor(3 * depthB);
        minTap[2] = floor(5 * depthA);
        maxTap[2] = floor(5 * depthB);
        minTap[3] = floor(7 * depthA);
        maxTap[3] = floor(7 * depthB);
        minTap[4] = floor(11 * depthA);
        maxTap[4] = floor(11 * depthB);
        minTap[5] = floor(13 * depthA);
        maxTap[5] = floor(13 * depthB);
        minTap[6] = floor(17 * depthA);
        maxTap[6] = floor(17 * depthB);
        minTap[7] = floor(19 * depthA);
        maxTap[7] = floor(19 * depthB);
        minTap[8] = floor(23 * depthA);
        maxTap[8] = floor(23 * depthB);
        minTap[9] = floor(29 * depthA);
        maxTap[9] = floor(29 * depthB);
        minTap[10] = floor(31 * depthA);
        maxTap[10] = floor(31 * depthB);
        minTap[11] = floor(37 * depthA);
        maxTap[11] = floor(37 * depthB);
        minTap[12] = floor(41 * depthA);
        maxTap[12] = floor(41 * depthB);
        minTap[13] = floor(43 * depthA);
        maxTap[13] = floor(43 * depthB);
        minTap[14] = floor(47 * depthA);
        maxTap[14] = floor(47 * depthB);
        minTap[15] = floor(53 * depthA);
        maxTap[15] = floor(53 * depthB);
        minTap[16] = floor(59 * depthA);
        maxTap[16] = floor(59 * depthB);
        minTap[17] = floor(61 * depthA);
        maxTap[17] = floor(61 * depthB);
        minTap[18] = floor(67 * depthA);
        maxTap[18] = floor(67 * depthB);
        minTap[19] = floor(71 * depthA);
        maxTap[19] = floor(71 * depthB);
        minTap[20] = floor(73 * depthA);
        maxTap[20] = floor(73 * depthB);
        minTap[21] = floor(79 * depthA);
        maxTap[21] = floor(79 * depthB);
        minTap[22] = floor(83 * depthA);
        maxTap[22] = floor(83 * depthB);
        minTap[23] = floor(89 * depthA);
        maxTap[23] = floor(89 * depthB);
        minTap[24] = floor(97 * depthA);
        maxTap[24] = floor(97 * depthB);
        minTap[25] = floor(101 * depthA);
        maxTap[25] = floor(101 * depthB);
        minTap[26] = floor(103 * depthA);
        maxTap[26] = floor(103 * depthB);
        minTap[27] = floor(107 * depthA);
        maxTap[27] = floor(107 * depthB);
        minTap[28] = floor(109 * depthA);
        maxTap[28] = floor(109 * depthB);
        minTap[29] = floor(113 * depthA);
        maxTap[29] = floor(113 * depthB);
        minTap[30] = floor(117 * depthA);
        maxTap[30] = floor(117 * depthB);
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            if (gcount < 0 || gcount > 16000) {
                gcount = 16000;
            }
            dL[gcount + 16000] = dL[gcount] = inputSampleL;
            dR[gcount + 16000] = dR[gcount] = inputSampleR;
            if (slowCount > rate || slowCount < 0) {
                slowCount = 0;
                stepCount++;
                if (stepCount > 29 || stepCount < 0) {
                    stepCount = 0;
                }
                position[stepCount] += stepTap[stepCount];
                if (position[stepCount] < minTap[stepCount]) {
                    position[stepCount] = minTap[stepCount];
                    stepTap[stepCount] = 1;
                }
                if (position[stepCount] > maxTap[stepCount]) {
                    position[stepCount] = maxTap[stepCount];
                    stepTap[stepCount] = -1;
                }
            }
            // begin L
            scalefactorL *= 0.9999;
            scalefactorL += (100.0 - fabs(combineL)) * 0.000001;
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[29]]);
            combineL += (dL[gcount + position[28]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[27]]);
            combineL += (dL[gcount + position[26]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[25]]);
            combineL += (dL[gcount + position[24]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[23]]);
            combineL += (dL[gcount + position[22]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[21]]);
            combineL += (dL[gcount + position[20]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[19]]);
            combineL += (dL[gcount + position[18]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[17]]);
            combineL += (dL[gcount + position[16]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[15]]);
            combineL += (dL[gcount + position[14]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[13]]);
            combineL += (dL[gcount + position[12]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[11]]);
            combineL += (dL[gcount + position[10]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[9]]);
            combineL += (dL[gcount + position[8]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[7]]);
            combineL += (dL[gcount + position[6]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[5]]);
            combineL += (dL[gcount + position[4]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[3]]);
            combineL += (dL[gcount + position[2]]);
            combineL *= scalefactorL;
            combineL -= (dL[gcount + position[1]]);
            combineL += (dL[gcount + position[0]]);
            inputSampleL = combineL;
            // done with L
            // begin R
            scalefactorR *= 0.9999;
            scalefactorR += (100.0 - fabs(combineR)) * 0.000001;
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[29]]);
            combineR += (dR[gcount + position[28]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[27]]);
            combineR += (dR[gcount + position[26]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[25]]);
            combineR += (dR[gcount + position[24]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[23]]);
            combineR += (dR[gcount + position[22]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[21]]);
            combineR += (dR[gcount + position[20]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[19]]);
            combineR += (dR[gcount + position[18]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[17]]);
            combineR += (dR[gcount + position[16]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[15]]);
            combineR += (dR[gcount + position[14]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[13]]);
            combineR += (dR[gcount + position[12]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[11]]);
            combineR += (dR[gcount + position[10]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[9]]);
            combineR += (dR[gcount + position[8]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[7]]);
            combineR += (dR[gcount + position[6]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[5]]);
            combineR += (dR[gcount + position[4]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[3]]);
            combineR += (dR[gcount + position[2]]);
            combineR *= scalefactorR;
            combineR -= (dR[gcount + position[1]]);
            combineR += (dR[gcount + position[0]]);
            inputSampleR = combineR;
            // done with R
            gcount--;
            slowCount++;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
