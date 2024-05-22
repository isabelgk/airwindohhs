#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PhaseNudge final : public Effect<T>
{
    std::string m_name{ "PhaseNudge" };

    double dL[1503];
    double dR[1503];
    int one, maxdelay;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    PhaseNudge()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 1502; count++) {
            dL[count] = 0.0;
            dR[count] = 0.0;
        }
        one = 1;
        maxdelay = 9001;
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Phase Nudge";
            case kParamB: return "Dry/Wet";

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        int allpasstemp;
        double outallpass = 0.618033988749894848204586; // golden ratio!
        // if you see 0.6180 it's not a wild stretch to wonder whether you are working with a constant
        int maxdelayTarget = (int)(pow(A, 3) * 1501.0);
        double wet = B;
        // removed extra dry variable
        double bridgerectifier;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            inputSampleL /= 4.0;
            inputSampleR /= 4.0;
            bridgerectifier = fabs(inputSampleL);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            bridgerectifier = fabs(inputSampleR);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            if (fabs(maxdelay - maxdelayTarget) > 1500) {
                maxdelay = maxdelayTarget;
            }
            if (maxdelay < maxdelayTarget) {
                maxdelay++;
                dL[maxdelay] = (dL[0] + dL[maxdelay - 1]) / 2.0;
                dR[maxdelay] = (dR[0] + dR[maxdelay - 1]) / 2.0;
            }
            if (maxdelay > maxdelayTarget) {
                maxdelay--;
                dL[maxdelay] = (dL[0] + dL[maxdelay]) / 2.0;
                dR[maxdelay] = (dR[0] + dR[maxdelay]) / 2.0;
            }
            allpasstemp = one - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay) {
                allpasstemp = maxdelay;
            }
            inputSampleL -= dL[allpasstemp] * outallpass;
            inputSampleR -= dR[allpasstemp] * outallpass;
            dL[one] = inputSampleL;
            dR[one] = inputSampleR;
            inputSampleL *= outallpass;
            inputSampleR *= outallpass;
            one--;
            if (one < 0 || one > maxdelay) {
                one = maxdelay;
            }
            inputSampleL += (dL[one]);
            inputSampleR += (dR[one]);
            bridgerectifier = fabs(inputSampleL);
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL -= bridgerectifier;
            }
            else {
                inputSampleL += bridgerectifier;
            }
            bridgerectifier = fabs(inputSampleR);
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR -= bridgerectifier;
            }
            else {
                inputSampleR += bridgerectifier;
            }
            inputSampleL *= 4.0;
            inputSampleR *= 4.0;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
