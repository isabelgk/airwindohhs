#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Drive final : public Effect<T>
{
    const std::string m_name{ "Drive" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    bool fpFlip;
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    Drive()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        D = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        fpFlip = true;
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
            case kParamD: D = value; break;

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
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive";
            case kParamB: return "Highpass";
            case kParamC: return "Out Level";
            case kParamD: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A * 100.0);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "%";
            case kParamB: return " ";
            case kParamC: return " ";
            case kParamD: return " ";
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
        double driveone = pow(A * 2.0, 2);
        double iirAmount = pow(B, 3) / overallscale;
        double output = C;
        double wet = D;
        double dry = 1.0 - wet;
        double glitch = 0.60;
        double out;
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
            if (fpFlip)
            {
                iirSampleAL = (iirSampleAL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirSampleAL;
                iirSampleAR = (iirSampleAR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirSampleBL;
                iirSampleBR = (iirSampleBR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirSampleBR;
            }
            // highpass section
            fpFlip = !fpFlip;
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
            out = driveone;
            while (out > glitch)
            {
                out -= glitch;
                inputSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch));
                inputSampleL *= (1.0 + glitch);
                inputSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch));
                inputSampleR *= (1.0 + glitch);
            }
            // that's taken care of the really high gain stuff
            inputSampleL -= (inputSampleL * (fabs(inputSampleL) * out) * (fabs(inputSampleL) * out));
            inputSampleL *= (1.0 + out);
            inputSampleR -= (inputSampleR * (fabs(inputSampleR) * out) * (fabs(inputSampleR) * out));
            inputSampleR *= (1.0 + out);
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
