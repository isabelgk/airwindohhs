#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Distance final : public Effect<T>
{
    const std::string m_name{ "Distance" };

    double lastclampL;
    double clampL;
    double changeL;
    double thirdresultL;
    double prevresultL;
    double lastL;
    double lastclampR;
    double clampR;
    double changeR;
    double thirdresultR;
    double prevresultR;
    double lastR;
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
    Distance()
    {
        A = 0.0;
        B = 1.0;
        thirdresultL = prevresultL = lastclampL = clampL = changeL = lastL = 0.0;
        thirdresultR = prevresultR = lastclampR = clampR = changeR = lastR = 0.0;
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
            case kParamA: return "Distance";
            case kParamB: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A * 2.0);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "miles";
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double softslew = (pow(A * 2.0, 3.0) * 12.0) + 0.6;
        softslew *= overallscale;
        double filtercorrect = softslew / 2.0;
        double thirdfilter = softslew / 3.0;
        double levelcorrect = 1.0 + (softslew / 6.0);
        double postfilter;
        double wet = B;
        double dry = 1.0 - wet;
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
            inputSampleL *= softslew;
            lastclampL = clampL;
            clampL = inputSampleL - lastL;
            postfilter = changeL = fabs(clampL - lastclampL);
            postfilter += filtercorrect;
            if (changeL > 1.5707963267949) {
                changeL = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeL));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleL = lastL + (clampL * bridgerectifier);
            lastL = inputSampleL;
            inputSampleL /= softslew;
            inputSampleL += (thirdresultL * thirdfilter);
            inputSampleL /= (thirdfilter + 1.0);
            inputSampleL += (prevresultL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultL = prevresultL;
            prevresultL = inputSampleL;
            inputSampleL *= levelcorrect;
            inputSampleR *= softslew;
            lastclampR = clampR;
            clampR = inputSampleR - lastR;
            postfilter = changeR = fabs(clampR - lastclampR);
            postfilter += filtercorrect;
            if (changeR > 1.5707963267949) {
                changeR = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeR));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleR = lastR + (clampR * bridgerectifier);
            lastR = inputSampleR;
            inputSampleR /= softslew;
            inputSampleR += (thirdresultR * thirdfilter);
            inputSampleR /= (thirdfilter + 1.0);
            inputSampleR += (prevresultR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultR = prevresultR;
            prevresultR = inputSampleR;
            inputSampleR *= levelcorrect;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
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
