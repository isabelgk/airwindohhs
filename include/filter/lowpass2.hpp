#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Lowpass2 final : public Effect<T>
{
    std::string m_name{ "Lowpass2" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleGL;
    double iirSampleHL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double iirSampleGR;
    double iirSampleHR;
    bool fpFlip;
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
    Lowpass2()
    {
        A = 0.0;
        B = 0.5; //-1.0 to 1.0
        C = 0.25; // 0.0 to 4.0
        D = 1.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleGL = 0.0;
        iirSampleHL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        iirSampleGR = 0.0;
        iirSampleHR = 0.0;
        fpFlip = true;
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
            case kParamB: return "Sft/Hrd";
            case kParamC: return "Poles";
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
            case kParamB: return std::to_string((B * 2.0) - 1.0);
            case kParamC: return std::to_string(C * 4.0);
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

        double iirAmount = A;
        double tight = (B * 2.0) - 1.0;
        iirAmount += fabs(tight);
        if (iirAmount < 0.0000001) {
            iirAmount = 0.0000001;
        }
        if (iirAmount > 1) {
            iirAmount = 1;
        }
        if (tight < 0.0) {
            tight *= 0.5;
        }
        double offsetL;
        double offsetR;
        double aWet = 1.0;
        double bWet = 1.0;
        double cWet = 1.0;
        double dWet = C * 4.0;
        // four-stage wet/dry control using progressive stages that bypass when not engaged
        if (dWet < 1.0) {
            aWet = dWet;
            bWet = 0.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 2.0) {
            bWet = dWet - 1.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 3.0) {
            cWet = dWet - 2.0;
            dWet = 0.0;
        }
        else {
            dWet -= 3.0;
        }
        // this is one way to make a little set of dry/wet stages that are successively added to the
        // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        // beyond that point: this is a way to progressively add a 'black box' sound processing
        // which lets you fall through to simpler processing at lower settings.
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
            if (tight > 0) {
                offsetL = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offsetL = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offsetL < 0.0000001) {
                offsetL = 0.0000001;
            }
            if (offsetL > 1) {
                offsetL = 1;
            }
            if (tight > 0) {
                offsetR = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offsetR = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offsetR < 0.0000001) {
                offsetR = 0.0000001;
            }
            if (offsetR > 1) {
                offsetR = 1;
            }
            if (fpFlip)
            {
                if (aWet > 0.0) {
                    iirSampleAL = (iirSampleAL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleAL * aWet) + (inputSampleL * (1.0 - aWet));
                    iirSampleAR = (iirSampleAR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleAR * aWet) + (inputSampleR * (1.0 - aWet));
                }
                if (bWet > 0.0) {
                    iirSampleCL = (iirSampleCL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleCL * bWet) + (inputSampleL * (1.0 - bWet));
                    iirSampleCR = (iirSampleCR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleCR * bWet) + (inputSampleR * (1.0 - bWet));
                }
                if (cWet > 0.0) {
                    iirSampleEL = (iirSampleEL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleEL * cWet) + (inputSampleL * (1.0 - cWet));
                    iirSampleER = (iirSampleER * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleER * cWet) + (inputSampleR * (1.0 - cWet));
                }
                if (dWet > 0.0) {
                    iirSampleGL = (iirSampleGL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleGL * dWet) + (inputSampleL * (1.0 - dWet));
                    iirSampleGR = (iirSampleGR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleGR * dWet) + (inputSampleR * (1.0 - dWet));
                }
            }
            else
            {
                if (aWet > 0.0) {
                    iirSampleBL = (iirSampleBL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleBL * aWet) + (inputSampleL * (1.0 - aWet));
                    iirSampleBR = (iirSampleBR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleBR * aWet) + (inputSampleR * (1.0 - aWet));
                }
                if (bWet > 0.0) {
                    iirSampleDL = (iirSampleDL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleDL * bWet) + (inputSampleL * (1.0 - bWet));
                    iirSampleDR = (iirSampleDR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleDR * bWet) + (inputSampleR * (1.0 - bWet));
                }
                if (cWet > 0.0) {
                    iirSampleFL = (iirSampleFL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleFL * cWet) + (inputSampleL * (1.0 - cWet));
                    iirSampleFR = (iirSampleFR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleFR * cWet) + (inputSampleR * (1.0 - cWet));
                }
                if (dWet > 0.0) {
                    iirSampleHL = (iirSampleHL * (1 - (offsetL * iirAmount))) + (inputSampleL * (offsetL * iirAmount));
                    inputSampleL = (iirSampleHL * dWet) + (inputSampleL * (1.0 - dWet));
                    iirSampleHR = (iirSampleHR * (1 - (offsetR * iirAmount))) + (inputSampleR * (offsetR * iirAmount));
                    inputSampleR = (iirSampleHR * dWet) + (inputSampleR * (1.0 - dWet));
                }
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
            fpFlip = !fpFlip;
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
