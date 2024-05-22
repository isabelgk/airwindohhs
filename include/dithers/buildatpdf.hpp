#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BuildATPDF final : public Effect<T>
{
    std::string m_name{ "BuildATPDF" };

    double bL[11];
    double bR[11];
    double f[11];
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;
    float J;
    // parameters. Always 0-1, and we scale/alter them elsewhere.
    uint32_t fpdL;
    uint32_t fpdR;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kParamF = 5,
        kParamG = 6,
        kParamH = 7,
        kParamI = 8,
        kParamJ = 9,
        kNumParameters = 10

    };

  public:
    BuildATPDF()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 0.5;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
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
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            case kParamI: I = value; break;
            case kParamJ: J = value; break;

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
            case kParamE: return E;
            case kParamF: return F;
            case kParamG: return G;
            case kParamH: return H;
            case kParamI: return I;
            case kParamJ: return J;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "First";
            case kParamB: return "Second";
            case kParamC: return "Third";
            case kParamD: return "Fourth";
            case kParamE: return "Fifth";
            case kParamF: return "Sixth";
            case kParamG: return "Seventh";
            case kParamH: return "Eighth";
            case kParamI: return "Ninth";
            case kParamJ: return "Tenth";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 2) - 1);
            case kParamB: return std::to_string((B * 2) - 1);
            case kParamC: return std::to_string((C * 2) - 1);
            case kParamD: return std::to_string((D * 2) - 1);
            case kParamE: return std::to_string((E * 2) - 1);
            case kParamF: return std::to_string((F * 2) - 1);
            case kParamG: return std::to_string((G * 2) - 1);
            case kParamH: return std::to_string((H * 2) - 1);
            case kParamI: return std::to_string((I * 2) - 1);
            case kParamJ: return std::to_string((J * 2) - 1);

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
            case kParamE: return " ";
            case kParamF: return " ";
            case kParamG: return " ";
            case kParamH: return " ";
            case kParamI: return " ";
            case kParamJ: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        f[0] = (A * 2) - 1;
        f[1] = (B * 2) - 1;
        f[2] = (C * 2) - 1;
        f[3] = (D * 2) - 1;
        f[4] = (E * 2) - 1;
        f[5] = (F * 2) - 1;
        f[6] = (G * 2) - 1;
        f[7] = (H * 2) - 1;
        f[8] = (I * 2) - 1;
        f[9] = (J * 2) - 1;
        double currentDither;
        double inputSampleL;
        double inputSampleR;
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
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = (double(fpdL) / UINT32_MAX);
            currentDither = (bL[0] * f[0]);
            currentDither += (bL[1] * f[1]);
            currentDither += (bL[2] * f[2]);
            currentDither += (bL[3] * f[3]);
            currentDither += (bL[4] * f[4]);
            currentDither += (bL[5] * f[5]);
            currentDither += (bL[6] * f[6]);
            currentDither += (bL[7] * f[7]);
            currentDither += (bL[8] * f[8]);
            currentDither += (bL[9] * f[9]);
            inputSampleL += currentDither;
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = (double(fpdR) / UINT32_MAX);
            currentDither = (bR[0] * f[0]);
            currentDither += (bR[1] * f[1]);
            currentDither += (bR[2] * f[2]);
            currentDither += (bR[3] * f[3]);
            currentDither += (bR[4] * f[4]);
            currentDither += (bR[5] * f[5]);
            currentDither += (bR[6] * f[6]);
            currentDither += (bR[7] * f[7]);
            currentDither += (bR[8] * f[8]);
            currentDither += (bR[9] * f[9]);
            inputSampleR += currentDither;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
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
