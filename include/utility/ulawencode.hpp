#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class uLawEncode final : public Effect<T>
{
    const std::string m_name{ "uLawEncode" };

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
    uLawEncode()
    {
        A = 1.0;
        B = 1.0;
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
        double gain = A;
        double wet = B;
        // removed extra dry variable
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            noisesourceL = noisesourceL % 1700021;
            noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
                inputSampleL -= applyresidue;
            }
            noisesourceR = noisesourceR % 1700021;
            noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
                inputSampleR -= applyresidue;
            }
            // for live air, we always apply the dither noise. Then, if our result is
            // effectively digital black, we'll subtract it auLawEncode. We want a 'air' hiss
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleL < 0) {
                inputSampleL = -log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleR > 0) {
                inputSampleR = log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
            if (inputSampleR < 0) {
                inputSampleR = -log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
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
