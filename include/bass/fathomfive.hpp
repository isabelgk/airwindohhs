#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class FathomFive final : public Effect<T>
{
    std::string m_name{ "FathomFive" };

    bool WasNegativeL;
    bool SubOctaveL;
    double iirSampleLA;
    double iirSampleLB;
    double iirSampleLC;
    double iirSampleLD;
    bool WasNegativeR;
    bool SubOctaveR;
    double iirSampleRA;
    double iirSampleRB;
    double iirSampleRC;
    double iirSampleRD;
    float A; // Root Note
    float B; // SubOctave
    float C; // Frequency
    float D; // Dry/Wet
    // parameters. Always 0-1, and we scale/alter them elsewhere.
    uint32_t fpdL;
    uint32_t fpdR;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    FathomFive()
    {
        A = 1.0;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        WasNegativeL = false;
        SubOctaveL = false;
        iirSampleLA = 0.0;
        iirSampleLB = 0.0;
        iirSampleLC = 0.0;
        iirSampleLD = 0.0;
        WasNegativeR = false;
        SubOctaveR = false;
        iirSampleRA = 0.0;
        iirSampleRB = 0.0;
        iirSampleRC = 0.0;
        iirSampleRD = 0.0;
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
            case kParamA: return "Root Note";
            case kParamB: return "SubOctave";
            case kParamC: return "Frequency";
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

        double EQ = 0.01 + ((pow(C, 4) / getSampleRate()) * 32000.0);
        double dcblock = EQ / 320.0;
        double wet = D * 2.0;
        double dry = 2.0 - wet;
        double bridgerectifier;
        double tempL;
        double tempR;
        double basstrim = (0.01 / EQ) + 1.0;
        if (wet > 1.0) {
            wet = 1.0;
        }
        if (dry > 1.0) {
            dry = 1.0;
        }
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
            if (inputSampleL > 0)
            {
                if (WasNegativeL) {
                    SubOctaveL = !SubOctaveL;
                }
                WasNegativeL = false;
            }
            else {
                WasNegativeL = true;
            }
            if (inputSampleR > 0)
            {
                if (WasNegativeR) {
                    SubOctaveR = !SubOctaveR;
                }
                WasNegativeR = false;
            }
            else {
                WasNegativeR = true;
            }
            iirSampleLD = (iirSampleLD * (1 - EQ)) + (inputSampleL * EQ);
            bridgerectifier = fabs(iirSampleLD);
            if (SubOctaveL) {
                tempL = bridgerectifier * B;
            }
            else {
                tempL = -bridgerectifier * B;
            }
            iirSampleRD = (iirSampleRD * (1 - EQ)) + (inputSampleR * EQ);
            bridgerectifier = fabs(iirSampleRD);
            if (SubOctaveR) {
                tempR = bridgerectifier * B;
            }
            else {
                tempR = -bridgerectifier * B;
            }
            tempL += (inputSampleL * A);
            tempR += (inputSampleR * A);
            iirSampleLA += (tempL * EQ);
            iirSampleLA -= (iirSampleLA * iirSampleLA * iirSampleLA * EQ);
            if (iirSampleLA > 0) {
                iirSampleLA -= dcblock;
            }
            else {
                iirSampleLA += dcblock;
            }
            tempL = iirSampleLA * basstrim;
            iirSampleRA += (tempR * EQ);
            iirSampleRA -= (iirSampleRA * iirSampleRA * iirSampleRA * EQ);
            if (iirSampleRA > 0) {
                iirSampleRA -= dcblock;
            }
            else {
                iirSampleRA += dcblock;
            }
            tempR = iirSampleRA * basstrim;
            iirSampleLB = (iirSampleLB * (1 - EQ)) + (tempL * EQ);
            tempL = iirSampleLB;
            iirSampleRB = (iirSampleRB * (1 - EQ)) + (tempR * EQ);
            tempR = iirSampleRB;
            iirSampleLC = (iirSampleLC * (1 - EQ)) + (tempL * EQ);
            tempL = iirSampleLC;
            iirSampleRC = (iirSampleRC * (1 - EQ)) + (tempR * EQ);
            tempR = iirSampleRC;
            inputSampleL = (inputSampleL * dry) + (tempL * wet);
            inputSampleR = (inputSampleR * dry) + (tempR * wet);
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
