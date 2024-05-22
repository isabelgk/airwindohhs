#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PurestAir final : public Effect<T>
{
    const std::string m_name{ "PurestAir" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double last1SampleL;
    double halfwaySampleL;
    double halfDrySampleL;
    double halfDiffSampleL;
    double diffSampleL;
    double lastSampleL;
    double s1L;
    double s2L;
    double s3L;
    double applyL;
    double last1SampleR;
    double halfwaySampleR;
    double halfDrySampleR;
    double halfDiffSampleR;
    double diffSampleR;
    double lastSampleR;
    double s1R;
    double s2R;
    double s3R;
    double applyR;
    float A;
    float B;
    float C;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    PurestAir()
    {
        A = 0.5;
        B = 0.0;
        C = 1.0;
        last1SampleL = 0.0;
        halfwaySampleL = halfDrySampleL = halfDiffSampleL = diffSampleL = 0.0;
        lastSampleL = 0.0;
        s1L = s2L = s3L = 0.0;
        applyL = 0.0;
        last1SampleR = 0.0;
        halfwaySampleR = halfDrySampleR = halfDiffSampleR = diffSampleR = 0.0;
        lastSampleR = 0.0;
        s1R = s2R = s3R = 0.0;
        applyR = 0.0;
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

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Boost";
            case kParamB: return "Limit";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 2.0) - 1.0);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double applyTarget = (A * 2.0) - 1.0;
        double threshold = pow((1 - fabs(applyTarget)), 3);
        if (applyTarget > 0) {
            applyTarget *= 3;
        }
        double intensity = pow(B, 2) * 5.0;
        double wet = C;
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
            halfDrySampleL = halfwaySampleL = (inputSampleL + last1SampleL) / 2.0;
            last1SampleL = inputSampleL;
            s3L = s2L;
            s2L = s1L;
            s1L = inputSampleL;
            double m1 = (s1L - s2L) * ((s1L - s2L) / 1.3);
            double m2 = (s2L - s3L) * ((s1L - s2L) / 1.3);
            double sense = fabs((m1 - m2) * ((m1 - m2) / 1.3)) * intensity;
            // this will be 0 for smooth, high for SSS
            applyL += applyTarget - sense;
            applyL *= 0.5;
            if (applyL < -1.0) {
                applyL = -1.0;
            }
            double clamp = halfwaySampleL - halfDrySampleL;
            if (clamp > threshold) {
                halfwaySampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                halfwaySampleL = lastSampleL - threshold;
            }
            lastSampleL = halfwaySampleL;
            clamp = inputSampleL - lastSampleL;
            if (clamp > threshold) {
                inputSampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleL - threshold;
            }
            lastSampleL = inputSampleL;
            diffSampleL = *in1 - inputSampleL;
            halfDiffSampleL = halfDrySampleL - halfwaySampleL;
            inputSampleL = *in1 + ((diffSampleL + halfDiffSampleL) * applyL);
            // done with left channel
            halfDrySampleR = halfwaySampleR = (inputSampleR + last1SampleR) / 2.0;
            last1SampleR = inputSampleR;
            s3R = s2R;
            s2R = s1R;
            s1R = inputSampleR;
            m1 = (s1R - s2R) * ((s1R - s2R) / 1.3);
            m2 = (s2R - s3R) * ((s1R - s2R) / 1.3);
            sense = fabs((m1 - m2) * ((m1 - m2) / 1.3)) * intensity;
            // this will be 0 for smooth, high for SSS
            applyR += applyTarget - sense;
            applyR *= 0.5;
            if (applyR < -1.0) {
                applyR = -1.0;
            }
            clamp = halfwaySampleR - halfDrySampleR;
            if (clamp > threshold) {
                halfwaySampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                halfwaySampleR = lastSampleR - threshold;
            }
            lastSampleR = halfwaySampleR;
            clamp = inputSampleR - lastSampleR;
            if (clamp > threshold) {
                inputSampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleR - threshold;
            }
            lastSampleR = inputSampleR;
            diffSampleR = *in2 - inputSampleR;
            halfDiffSampleR = halfDrySampleR - halfwaySampleR;
            inputSampleR = *in2 + ((diffSampleR + halfDiffSampleR) * applyR);
            // done with right channel
            if (wet != 1.0) {
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
