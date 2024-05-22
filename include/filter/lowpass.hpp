#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Lowpass final : public Effect<T>
{
    const std::string m_name{ "Lowpass" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    bool fpFlip;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    Lowpass()
    {
        A = 1.0;
        B = 0.5;
        C = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
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
            case kParamA: return "Lowpass";
            case kParamB: return "Soft/Hard";
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
            case kParamB: return std::to_string((B * 2.0) - 1.0);
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
        double iirAmount = (pow(A, 2) + A) / 2.0;
        iirAmount /= overallscale;
        double tight = (B * 2.0) - 1.0;
        double wet = C;
        // removed extra dry variable
        double offset;
        double inputSampleL;
        double inputSampleR;
        double outputSampleL;
        double outputSampleR;
        iirAmount += (iirAmount * tight * tight);
        if (tight > 0) {
            tight /= 1.5;
        }
        else {
            tight /= 3.0;
        }
        // we are setting it up so that to either extreme we can get an audible sound,
        // but sort of scaled so small adjustments don't shift the cutoff frequency yet.
        if (iirAmount <= 0.0) {
            iirAmount = 0.0;
        }
        if (iirAmount > 1.0) {
            iirAmount = 1.0;
        }
        // handle the change in cutoff frequency
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
            outputSampleL = inputSampleL;
            outputSampleR = inputSampleR;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip)
            {
                iirSampleAL = (iirSampleAL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                outputSampleL = iirSampleAL;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                outputSampleL = iirSampleBL;
            }
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip)
            {
                iirSampleAR = (iirSampleAR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                outputSampleR = iirSampleAR;
            }
            else
            {
                iirSampleBR = (iirSampleBR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                outputSampleR = iirSampleBR;
            }
            fpFlip = !fpFlip;
            if (wet < 1.0) {
                outputSampleL = (outputSampleL * wet) + (inputSampleL * (1.0 - wet));
            }
            if (wet < 1.0) {
                outputSampleR = (outputSampleR * wet) + (inputSampleR * (1.0 - wet));
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
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs