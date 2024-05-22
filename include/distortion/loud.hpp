#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Loud final : public Effect<T>
{
    const std::string m_name{ "Loud" };

    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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
    Loud()
    {
        A = 0.0;
        B = 1.0;
        C = 1.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            case kParamB: return "Output Level";
            case kParamC: return "Dry/Wet";

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
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " ";
            case kParamB: return " ";
            case kParamC: return " ";
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
        double boost = pow(A + 1.0, 5);
        double output = B;
        double wet = C;
        double dry = 1.0 - wet;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double clamp;
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
            // begin L
            inputSampleL *= boost;
            clamp = inputSampleL - lastSampleL;
            if (clamp > 0)
            {
                inputSampleL = -(inputSampleL - 1.0);
                inputSampleL *= 1.2566108;
                if (inputSampleL < 0.0) {
                    inputSampleL = 0.0;
                }
                if (inputSampleL > 3.141527) {
                    inputSampleL = 3.141527;
                }
                inputSampleL = sin(inputSampleL) * overallscale;
                if (clamp > inputSampleL) {
                    clamp = inputSampleL;
                }
            }
            if (clamp < 0)
            {
                inputSampleL += 1.0;
                inputSampleL *= 1.2566108;
                if (inputSampleL < 0.0) {
                    inputSampleL = 0.0;
                }
                if (inputSampleL > 3.141527) {
                    inputSampleL = 3.141527;
                }
                inputSampleL = -sin(inputSampleL) * overallscale;
                if (clamp < inputSampleL) {
                    clamp = inputSampleL;
                }
            }
            inputSampleL = lastSampleL + clamp;
            lastSampleL = inputSampleL;
            // finished L
            // begin R
            inputSampleR *= boost;
            clamp = inputSampleR - lastSampleR;
            if (clamp > 0)
            {
                inputSampleR = -(inputSampleR - 1.0);
                inputSampleR *= 1.2566108;
                if (inputSampleR < 0.0) {
                    inputSampleR = 0.0;
                }
                if (inputSampleR > 3.141527) {
                    inputSampleR = 3.141527;
                }
                inputSampleR = sin(inputSampleR) * overallscale;
                if (clamp > inputSampleR) {
                    clamp = inputSampleR;
                }
            }
            if (clamp < 0)
            {
                inputSampleR += 1.0;
                inputSampleR *= 1.2566108;
                if (inputSampleR < 0.0) {
                    inputSampleR = 0.0;
                }
                if (inputSampleR > 3.141527) {
                    inputSampleR = 3.141527;
                }
                inputSampleR = -sin(inputSampleR) * overallscale;
                if (clamp < inputSampleR) {
                    clamp = inputSampleR;
                }
            }
            inputSampleR = lastSampleR + clamp;
            lastSampleR = inputSampleR;
            // finished R
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
