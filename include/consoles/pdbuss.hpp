#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PDBuss final : public Effect<T>
{
    const std::string m_name{ "PDBuss" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double gainchase;
    double settingchase;
    double chasespeed;
    double previousSampleL;
    double previousSampleR;
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    PDBuss()
    {
        A = 1.0;
        B = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        chasespeed = 350.0;
        previousSampleL = 0.0;
        previousSampleR = 0.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain";
            case kParamB: return "Drive";

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double inputgain = A;
        double intensity = B;
        double applyL;
        double applyR;
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
        if (settingchase != inputgain) {
            chasespeed *= 2.0;
            settingchase = inputgain;
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        if (gainchase < 0.0) {
            gainchase = inputgain;
        }
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
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + inputgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= gainchase;
                inputSampleR *= gainchase;
            }
            // done with trim control
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // basic distortion factor
            applyL = (fabs(previousSampleL + inputSampleL) / 2.0) * intensity;
            applyR = (fabs(previousSampleR + inputSampleR) / 2.0) * intensity;
            // saturate less if previous sample was undistorted and low level, or if it was
            // inverse polarity. Lets through highs and brightness more.
            inputSampleL = (drySampleL * (1.0 - applyL)) + (inputSampleL * applyL);
            inputSampleR = (drySampleR * (1.0 - applyR)) + (inputSampleR * applyR);
            // dry-wet control for intensity also has FM modulation to clean up highs
            previousSampleL = sin(drySampleL);
            previousSampleR = sin(drySampleR);
            // apply the sine while storing previous sample
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
