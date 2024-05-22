#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Capacitor2 final : public Effect<T>
{
    std::string m_name{ "Capacitor2" };

    double iirHighpassAL;
    double iirHighpassBL;
    double iirHighpassCL;
    double iirHighpassDL;
    double iirHighpassEL;
    double iirHighpassFL;
    double iirLowpassAL;
    double iirLowpassBL;
    double iirLowpassCL;
    double iirLowpassDL;
    double iirLowpassEL;
    double iirLowpassFL;
    double iirHighpassAR;
    double iirHighpassBR;
    double iirHighpassCR;
    double iirHighpassDR;
    double iirHighpassER;
    double iirHighpassFR;
    double iirLowpassAR;
    double iirLowpassBR;
    double iirLowpassCR;
    double iirLowpassDR;
    double iirLowpassER;
    double iirLowpassFR;
    int count;
    double lowpassChase;
    double highpassChase;
    double wetChase;
    double lowpassBaseAmount;
    double highpassBaseAmount;
    double wet;
    double lastLowpass;
    double lastHighpass;
    double lastWet;
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
    Capacitor2()
    {
        A = 1.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        iirHighpassAL = 0.0;
        iirHighpassBL = 0.0;
        iirHighpassCL = 0.0;
        iirHighpassDL = 0.0;
        iirHighpassEL = 0.0;
        iirHighpassFL = 0.0;
        iirLowpassAL = 0.0;
        iirLowpassBL = 0.0;
        iirLowpassCL = 0.0;
        iirLowpassDL = 0.0;
        iirLowpassEL = 0.0;
        iirLowpassFL = 0.0;
        iirHighpassAR = 0.0;
        iirHighpassBR = 0.0;
        iirHighpassCR = 0.0;
        iirHighpassDR = 0.0;
        iirHighpassER = 0.0;
        iirHighpassFR = 0.0;
        iirLowpassAR = 0.0;
        iirLowpassBR = 0.0;
        iirLowpassCR = 0.0;
        iirLowpassDR = 0.0;
        iirLowpassER = 0.0;
        iirLowpassFR = 0.0;
        count = 0;
        lowpassChase = 0.0;
        highpassChase = 0.0;
        wetChase = 0.0;
        lowpassBaseAmount = 1.0;
        highpassBaseAmount = 0.0;
        wet = 1.0;
        lastLowpass = 1000.0;
        lastHighpass = 1000.0;
        lastWet = 1000.0;
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
            case kParamA: return "Lowpass";
            case kParamB: return "Highpass";
            case kParamC: return "NonLin";
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

        lowpassChase = pow(A, 2);
        highpassChase = pow(B, 2);
        double nonLin = 1.0 + ((1.0 - C) * 6.0);
        double nonLinTrim = 1.5 / cbrt(nonLin);
        wetChase = D;
        // should not scale with sample rate, because values reaching 1 are important
        // to its ability to bypass when set to max
        double lowpassSpeed = 300 / (fabs(lastLowpass - lowpassChase) + 1.0);
        double highpassSpeed = 300 / (fabs(lastHighpass - highpassChase) + 1.0);
        double wetSpeed = 300 / (fabs(lastWet - wetChase) + 1.0);
        lastLowpass = lowpassChase;
        lastHighpass = highpassChase;
        lastWet = wetChase;
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
            double dielectricScaleL = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
            double dielectricScaleR = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));
            lowpassBaseAmount = (((lowpassBaseAmount * lowpassSpeed) + lowpassChase) / (lowpassSpeed + 1.0));
            // positive voltage will mean lower capacitance when capacitor is barium titanate
            // on the lowpass, higher pressure means positive swings/smaller cap/larger value for lowpassAmount
            double lowpassAmountL = lowpassBaseAmount * dielectricScaleL;
            double invLowpassL = 1.0 - lowpassAmountL;
            double lowpassAmountR = lowpassBaseAmount * dielectricScaleR;
            double invLowpassR = 1.0 - lowpassAmountR;
            highpassBaseAmount = (((highpassBaseAmount * highpassSpeed) + highpassChase) / (highpassSpeed + 1.0));
            // positive voltage will mean lower capacitance when capacitor is barium titanate
            // on the highpass, higher pressure means positive swings/smaller cap/larger value for highpassAmount
            double highpassAmountL = highpassBaseAmount * dielectricScaleL;
            double invHighpassL = 1.0 - highpassAmountL;
            double highpassAmountR = highpassBaseAmount * dielectricScaleR;
            double invHighpassR = 1.0 - highpassAmountR;
            wet = (((wet * wetSpeed) + wetChase) / (wetSpeed + 1.0));
            count++;
            if (count > 5) {
                count = 0;
            }
            switch (count)
            {
                case 0:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassBL;
                    iirHighpassDL = (iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassDL;
                    iirLowpassDL = (iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassDL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassBR;
                    iirHighpassDR = (iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassDR;
                    iirLowpassDR = (iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassDR;
                    break;
                case 1:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassCL;
                    iirHighpassEL = (iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassEL;
                    iirLowpassEL = (iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassEL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassCR;
                    iirHighpassER = (iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassER;
                    iirLowpassER = (iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassER;
                    break;
                case 2:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassBL;
                    iirHighpassFL = (iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassFL;
                    iirLowpassFL = (iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassFL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassBR;
                    iirHighpassFR = (iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassFR;
                    iirLowpassFR = (iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassFR;
                    break;
                case 3:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassCL;
                    iirHighpassDL = (iirHighpassDL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassDL;
                    iirLowpassDL = (iirLowpassDL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassDL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassCR;
                    iirHighpassDR = (iirHighpassDR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassDR;
                    iirLowpassDR = (iirLowpassDR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassDR;
                    break;
                case 4:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassBL;
                    iirHighpassEL = (iirHighpassEL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassEL;
                    iirLowpassEL = (iirLowpassEL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassEL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassBR;
                    iirHighpassER = (iirHighpassER * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassER;
                    iirLowpassER = (iirLowpassER * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassER;
                    break;
                case 5:
                    iirHighpassAL = (iirHighpassAL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassCL;
                    iirHighpassFL = (iirHighpassFL * invHighpassL) + (inputSampleL * highpassAmountL);
                    inputSampleL -= iirHighpassFL;
                    iirLowpassFL = (iirLowpassFL * invLowpassL) + (inputSampleL * lowpassAmountL);
                    inputSampleL = iirLowpassFL;
                    iirHighpassAR = (iirHighpassAR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassCR;
                    iirHighpassFR = (iirHighpassFR * invHighpassR) + (inputSampleR * highpassAmountR);
                    inputSampleR -= iirHighpassFR;
                    iirLowpassFR = (iirLowpassFR * invLowpassR) + (inputSampleR * lowpassAmountR);
                    inputSampleR = iirLowpassFR;
                    break;
            }
            // Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
            // steepens the filter after minimizing artifacts.
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * nonLinTrim * wet);
            inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * nonLinTrim * wet);
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
