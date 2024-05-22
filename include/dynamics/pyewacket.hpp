#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Pyewacket final : public Effect<T>
{
    std::string m_name{ "Pyewacket" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double chase;
    double lastrectifierL;
    double lastrectifierR;
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
    Pyewacket()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        chase = 1.0;
        lastrectifierL = 0.0;
        lastrectifierR = 0.0;
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
            case kParamA: return "Input Gain";
            case kParamB: return "Release";
            case kParamC: return "Output Gain";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24.0) - 12.0);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string((C * 24.0) - 12.0);

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
            case kParamC: return "dB";
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
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double bridgerectifier;
        double temprectifier;
        double inputSense;
        double inputGain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
        double attack = ((B + 0.5) * 0.006) / overallscale;
        double decay = ((B + 0.01) * 0.0004) / overallscale;
        double outputGain = pow(10.0, ((C * 24.0) - 12.0) / 20.0);
        double wet;
        double maxblur;
        double blurdry;
        double out;
        double dry;
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
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            // we will take the greater of either channel and just use that, then apply the result
            // to both stereo channels.
            if (chase < inputSense) {
                chase += attack;
            }
            if (chase > 1.0) {
                chase = 1.0;
            }
            if (chase > inputSense) {
                chase -= decay;
            }
            if (chase < 0.0) {
                chase = 0.0;
            }
            // chase will be between 0 and ? (if input is super hot)
            out = wet = chase;
            if (wet > 1.0) {
                wet = 1.0;
            }
            maxblur = wet * fpNew;
            blurdry = 1.0 - maxblur;
            // scaled back so that blur remains balance of both
            if (out > fpOld) {
                out = fpOld - (out - fpOld);
            }
            if (out < 0.0) {
                out = 0.0;
            }
            dry = 1.0 - wet;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            temprectifier = 1 - cos(bridgerectifier);
            bridgerectifier = ((lastrectifierL * maxblur) + (temprectifier * blurdry));
            lastrectifierL = temprectifier;
            // starved version is also blurred by one sample
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * dry) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * dry) - (bridgerectifier * out);
            }
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            temprectifier = 1 - cos(bridgerectifier);
            bridgerectifier = ((lastrectifierR * maxblur) + (temprectifier * blurdry));
            lastrectifierR = temprectifier;
            // starved version is also blurred by one sample
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * dry) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * dry) - (bridgerectifier * out);
            }
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
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
