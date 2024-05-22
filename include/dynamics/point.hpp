#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Point final : public Effect<T>
{
    std::string m_name{ "Point" };

    uint32_t fpdL;
    uint32_t fpdR;
    bool fpFlip;
    // default stuff
    double nibAL;
    double nobAL;
    double nibBL;
    double nobBL;
    double nibAR;
    double nobAR;
    double nibBR;
    double nobBR;
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
    Point()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        nibAL = 0.0;
        nobAL = 0.0;
        nibBL = 0.0;
        nobBL = 0.0;
        nibAR = 0.0;
        nobAR = 0.0;
        nibBR = 0.0;
        nobBR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Trim";
            case kParamB: return "Point";
            case kParamC: return "Reaction Speed";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24.0) - 12.0);
            case kParamB: return std::to_string((B * 2.0) - 1.0);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return " ";
            case kParamC: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double gaintrim = pow(10.0, ((A * 24.0) - 12.0) / 20);
        double nibDiv = 1 / pow(C + 0.2, 7);
        nibDiv /= overallscale;
        double nobDiv;
        if (((B * 2.0) - 1.0) > 0) {
            nobDiv = nibDiv / (1.001 - ((B * 2.0) - 1.0));
        }
        else {
            nobDiv = nibDiv * (1.001 - pow(((B * 2.0) - 1.0) * 0.75, 2));
        }
        double nibnobFactor = 0.0; // start with the fallthrough value, why not
        double absolute;
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
            inputSampleL *= gaintrim;
            absolute = fabs(inputSampleL);
            if (fpFlip)
            {
                nibAL = nibAL + (absolute / nibDiv);
                nibAL = nibAL / (1 + (1 / nibDiv));
                nobAL = nobAL + (absolute / nobDiv);
                nobAL = nobAL / (1 + (1 / nobDiv));
                if (nobAL > 0)
                {
                    nibnobFactor = nibAL / nobAL;
                }
            }
            else
            {
                nibBL = nibBL + (absolute / nibDiv);
                nibBL = nibBL / (1 + (1 / nibDiv));
                nobBL = nobBL + (absolute / nobDiv);
                nobBL = nobBL / (1 + (1 / nobDiv));
                if (nobBL > 0)
                {
                    nibnobFactor = nibBL / nobBL;
                }
            }
            inputSampleL *= nibnobFactor;
            inputSampleR *= gaintrim;
            absolute = fabs(inputSampleR);
            if (fpFlip)
            {
                nibAR = nibAR + (absolute / nibDiv);
                nibAR = nibAR / (1 + (1 / nibDiv));
                nobAR = nobAR + (absolute / nobDiv);
                nobAR = nobAR / (1 + (1 / nobDiv));
                if (nobAR > 0)
                {
                    nibnobFactor = nibAR / nobAR;
                }
            }
            else
            {
                nibBR = nibBR + (absolute / nibDiv);
                nibBR = nibBR / (1 + (1 / nibDiv));
                nobBR = nobBR + (absolute / nobDiv);
                nobBR = nobBR / (1 + (1 / nobDiv));
                if (nobBR > 0)
                {
                    nibnobFactor = nibBR / nobBR;
                }
            }
            inputSampleR *= nibnobFactor;
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
