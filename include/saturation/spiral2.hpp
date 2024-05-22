#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Spiral2 final : public Effect<T>
{
    std::string m_name{ "Spiral2" };

    double iirSampleAL;
    double iirSampleBL;
    double prevSampleL;
    uint32_t fpdL;
    uint32_t fpdR;
    double iirSampleAR;
    double iirSampleBR;
    double prevSampleR;
    bool flip;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5

    };

  public:
    Spiral2()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        prevSampleL = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        prevSampleR = 0.0;
        flip = true;
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
            case kParamD: D = value; break;
            case kParamE: E = value; break;

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
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";
            case kParamB: return "Highpass";
            case kParamC: return "Presence";
            case kParamD: return "Output";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
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
        double gain = pow(A * 2.0, 2.0);
        double iirAmount = pow(B, 3.0) / overallscale;
        double presence = C;
        double output = D;
        double wet = E;
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
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
                prevSampleL *= gain;
                prevSampleR *= gain;
            }
            if (flip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // highpass section
            double presenceSampleL = sin(inputSampleL * fabs(prevSampleL)) / ((prevSampleL == 0.0) ? 1 : fabs(prevSampleL));
            double presenceSampleR = sin(inputSampleR * fabs(prevSampleR)) / ((prevSampleR == 0.0) ? 1 : fabs(prevSampleR));
            // change from first Spiral: delay of one sample on the scaling factor.
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
                presenceSampleL *= output;
                presenceSampleR *= output;
            }
            if (presence > 0.0) {
                inputSampleL = (inputSampleL * (1.0 - presence)) + (presenceSampleL * presence);
                inputSampleR = (inputSampleR * (1.0 - presence)) + (presenceSampleR * presence);
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
            prevSampleL = drySampleL;
            prevSampleR = drySampleR;
            flip = !flip;
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
