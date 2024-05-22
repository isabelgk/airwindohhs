#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Acceleration final : public Effect<T>
{
    const std::string m_name{ "Acceleration" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double ataLastOutL;
    double s1L;
    double s2L;
    double s3L;
    double o1L;
    double o2L;
    double o3L;
    double m1L;
    double m2L;
    double desL;
    double ataLastOutR;
    double s1R;
    double s2R;
    double s3R;
    double o1R;
    double o2R;
    double o3R;
    double m1R;
    double m2R;
    double desR;
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    Acceleration()
    {
        A = 0.32;
        B = 1.0;
        ataLastOutL = 0.0;
        s1L = s2L = s3L = 0.0;
        o1L = o2L = o3L = 0.0;
        m1L = m2L = desL = 0.0;
        ataLastOutR = 0.0;
        s1R = s2R = s3R = 0.0;
        o1R = o2R = o3R = 0.0;
        m1R = m2R = desR = 0.0;
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
            case kParamA: return "Limit";
            case kParamB: return "Dry/Wet";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double intensity = pow(A, 3) * (32 / overallscale);
        double wet = B;
        // removed extra dry variable
        double senseL;
        double smoothL;
        double senseR;
        double smoothR;
        double accumulatorSample;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            s3L = s2L;
            s2L = s1L;
            s1L = inputSampleL;
            smoothL = (s3L + s2L + s1L) / 3.0;
            m1L = (s1L - s2L) * ((s1L - s2L) / 1.3);
            m2L = (s2L - s3L) * ((s1L - s2L) / 1.3);
            senseL = fabs(m1L - m2L);
            senseL = (intensity * intensity * senseL);
            o3L = o2L;
            o2L = o1L;
            o1L = senseL;
            if (o2L > senseL) {
                senseL = o2L;
            }
            if (o3L > senseL) {
                senseL = o3L;
            }
            // sense on the most intense
            s3R = s2R;
            s2R = s1R;
            s1R = inputSampleR;
            smoothR = (s3R + s2R + s1R) / 3.0;
            m1R = (s1R - s2R) * ((s1R - s2R) / 1.3);
            m2R = (s2R - s3R) * ((s1R - s2R) / 1.3);
            senseR = fabs(m1R - m2R);
            senseR = (intensity * intensity * senseR);
            o3R = o2R;
            o2R = o1R;
            o1R = senseR;
            if (o2R > senseR) {
                senseR = o2R;
            }
            if (o3R > senseR) {
                senseR = o3R;
            }
            // sense on the most intense
            if (senseL > 1.0) {
                senseL = 1.0;
            }
            if (senseR > 1.0) {
                senseR = 1.0;
            }
            inputSampleL *= (1.0 - senseL);
            inputSampleR *= (1.0 - senseR);
            inputSampleL += (smoothL * senseL);
            inputSampleR += (smoothR * senseR);
            senseL /= 2.0;
            senseR /= 2.0;
            accumulatorSample = (ataLastOutL * senseL) + (inputSampleL * (1.0 - senseL));
            ataLastOutL = inputSampleL;
            inputSampleL = accumulatorSample;
            accumulatorSample = (ataLastOutR * senseR) + (inputSampleR * (1.0 - senseR));
            ataLastOutR = inputSampleR;
            inputSampleR = accumulatorSample;
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
