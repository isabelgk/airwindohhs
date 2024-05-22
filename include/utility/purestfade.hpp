#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PurestFade final : public Effect<T>
{
    const std::string m_name{ "PurestFade" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double gainchase;
    double settingchase;
    double gainBchase;
    double chasespeed;
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    PurestFade()
    {
        A = 0.5;
        B = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        gainBchase = -90.0;
        chasespeed = 350.0;
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
            case kParamB: return "Fade";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 80.0) - 40.0);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double inputgain = (A * 80.0) - 40.0;
        if (settingchase != inputgain) {
            chasespeed *= 2.0;
            settingchase = inputgain;
            // increment the slowness for each fader movement
            // continuous alteration makes it react smoother
            // sudden jump to setting, not so much
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        // bail out if it's too extreme
        if (gainchase < -60.0) {
            gainchase = pow(10.0, inputgain / 20.0);
            // shouldn't even be a negative number
            // this is about starting at whatever's set, when
            // plugin is instantiated.
            // Otherwise it's the target, in dB.
        }
        double targetgain;
        // done with top controller
        double targetBgain = B;
        if (gainBchase < 0.0) {
            gainBchase = targetBgain;
        }
        // this one is not a dB value, but straight multiplication
        // done with slow fade controller
        double fadeSpeed = targetBgain;
        if (fadeSpeed < 0.0027) {
            fadeSpeed = 0.0027; // minimum fade speed
        }
        fadeSpeed = ((getSampleRate() * 0.004) / fadeSpeed);
        // this will tend to be much slower than PurestGain, and also adapt to sample rates
        double outputgain;
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
            targetgain = pow(10.0, settingchase / 20.0);
            // now we have the target in our temp variable
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + targetgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            gainBchase = (((gainBchase * fadeSpeed) + targetBgain) / (fadeSpeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            outputgain = gainchase * gainBchase;
            // directly multiply the dB gain by the straight multiply gain
            if (1.0 == outputgain)
            {
                *out1 = *in1;
                *out2 = *in2;
            }
            else {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
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
            }
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
