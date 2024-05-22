#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Podcast final : public Effect<T>
{
    const std::string m_name{ "Podcast" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double c1L;
    double c2L;
    double c3L;
    double c4L;
    double c5L;
    // the compressor
    double c1R;
    double c2R;
    double c3R;
    double c4R;
    double c5R;
    // the compressor
    float A;
    float B;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    Podcast()
    {
        A = 0.5;
        B = 1.0;
        c1L = 2.0;
        c2L = 2.0;
        c3L = 2.0;
        c4L = 2.0;
        c5L = 2.0; // startup comp gains
        c1R = 2.0;
        c2R = 2.0;
        c3R = 2.0;
        c4R = 2.0;
        c5R = 2.0; // startup comp gains
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
            case kParamA: return "Boost";
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
        double compress = 1.0 + pow(A, 2);
        double wet = B;
        double speed1 = 64.0 / pow(compress, 2);
        speed1 *= overallscale;
        double speed2 = speed1 * 1.4;
        double speed3 = speed2 * 1.5;
        double speed4 = speed3 * 1.6;
        double speed5 = speed4 * 1.7;
        double trigger;
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
            inputSampleL *= c1L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1L += trigger / speed5;
            if (c1L > compress) {
                c1L = compress;
            }
            // compress stage
            inputSampleR *= c1R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1R += trigger / speed5;
            if (c1R > compress) {
                c1R = compress;
            }
            // compress stage
            inputSampleL *= c2L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2L += trigger / speed4;
            if (c2L > compress) {
                c2L = compress;
            }
            // compress stage
            inputSampleR *= c2R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2R += trigger / speed4;
            if (c2R > compress) {
                c2R = compress;
            }
            // compress stage
            inputSampleL *= c3L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3L += trigger / speed3;
            if (c3L > compress) {
                c3L = compress;
            }
            // compress stage
            inputSampleR *= c3R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3R += trigger / speed3;
            if (c3R > compress) {
                c3R = compress;
            }
            // compress stage
            inputSampleL *= c4L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4L += trigger / speed2;
            if (c4L > compress) {
                c4L = compress;
            }
            // compress stage
            inputSampleR *= c4R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4R += trigger / speed2;
            if (c4R > compress) {
                c4R = compress;
            }
            // compress stage
            inputSampleL *= c5L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5L += trigger / speed1;
            if (c5L > compress) {
                c5L = compress;
            }
            // compress stage
            inputSampleR *= c5R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5R += trigger / speed1;
            if (c5R > compress) {
                c5R = compress;
            }
            // compress stage
            if (compress > 1.0) {
                inputSampleL /= compress;
                inputSampleR /= compress;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            if (inputSampleL > 0.999) {
                inputSampleL = 0.999;
            }
            if (inputSampleL < -0.999) {
                inputSampleL = -0.999;
            }
            if (inputSampleR > 0.999) {
                inputSampleR = 0.999;
            }
            if (inputSampleR < -0.999) {
                inputSampleR = -0.999;
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
