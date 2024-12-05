#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::bite {

constexpr std::string_view k_name{ "Bite" };
constexpr std::string_view k_short_description{
    "Bite is an unusual edge-maker."
};
constexpr std::string_view k_long_description{
    "Bite puts on a sort of midrange edge. At high sample rates it’ll be more of a trebly edge. It’s no specific frequency, so much as it’s just a harshening factor: you can also use it inversely, to take out midrange edge. It runs a couple samples of latency: on VST that works as a couple samples of delay. Again: an experiment, a science project."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Bite final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double sampleAL;
    double sampleBL;
    double sampleCL;
    double sampleDL;
    double sampleEL;
    double sampleFL;
    double sampleGL;
    double sampleHL;
    double sampleIL;
    double sampleAR;
    double sampleBR;
    double sampleCR;
    double sampleDR;
    double sampleER;
    double sampleFR;
    double sampleGR;
    double sampleHR;
    double sampleIR;
    float A;
    float B;

  public:
    Bite()
    {
        A = 0.5;
        B = 1.0;
        sampleAL = 0.0;
        sampleBL = 0.0;
        sampleCL = 0.0;
        sampleDL = 0.0;
        sampleEL = 0.0;
        sampleFL = 0.0;
        sampleGL = 0.0;
        sampleHL = 0.0;
        sampleIL = 0.0;
        sampleAR = 0.0;
        sampleBR = 0.0;
        sampleCR = 0.0;
        sampleDR = 0.0;
        sampleER = 0.0;
        sampleFR = 0.0;
        sampleGR = 0.0;
        sampleHR = 0.0;
        sampleIR = 0.0;
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

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bite"; break;
            case kParamB: return "output level"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Bite"; break;
            case kParamB: return "Output Level"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return " "; break;

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.3;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double gain = ((A * 2.0) - 1.0) * overallscale;
        double outputgain = B;
        double midA;
        double midB;
        double midC;
        double midD;
        double trigger;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            sampleIL = sampleHL;
            sampleHL = sampleGL;
            sampleGL = sampleFL;
            sampleFL = sampleEL;
            sampleEL = sampleDL;
            sampleDL = sampleCL;
            sampleCL = sampleBL;
            sampleBL = sampleAL;
            sampleAL = *in1;
            sampleIR = sampleHR;
            sampleHR = sampleGR;
            sampleGR = sampleFR;
            sampleFR = sampleER;
            sampleER = sampleDR;
            sampleDR = sampleCR;
            sampleCR = sampleBR;
            sampleBR = sampleAR;
            sampleAR = *in2;
            // rotate the buffer in primitive fashion
            if (fabs(sampleAL) < 1.18e-23) {
                sampleAL = fpdL * 1.18e-17;
            }
            if (fabs(sampleAR) < 1.18e-23) {
                sampleAR = fpdR * 1.18e-17;
            }
            midA = sampleAL - sampleEL;
            midB = sampleIL - sampleEL;
            midC = sampleCL - sampleEL;
            midD = sampleGL - sampleEL;
            midA *= ((((sampleBL + sampleCL + sampleDL) / 3) - ((sampleAL + sampleEL) / 2.0)) * gain);
            midB *= ((((sampleFL + sampleGL + sampleHL) / 3) - ((sampleEL + sampleIL) / 2.0)) * gain);
            midC *= ((sampleDL - ((sampleCL + sampleEL) / 2.0)) * gain);
            midD *= ((sampleFL - ((sampleEL + sampleGL) / 2.0)) * gain);
            trigger = sin(midA + midB + midC + midD);
            inputSampleL = sampleEL + (trigger * 8.0);
            midA = sampleAR - sampleER;
            midB = sampleIR - sampleER;
            midC = sampleCR - sampleER;
            midD = sampleGR - sampleER;
            midA *= ((((sampleBR + sampleCR + sampleDR) / 3) - ((sampleAR + sampleER) / 2.0)) * gain);
            midB *= ((((sampleFR + sampleGR + sampleHR) / 3) - ((sampleER + sampleIR) / 2.0)) * gain);
            midC *= ((sampleDR - ((sampleCR + sampleER) / 2.0)) * gain);
            midD *= ((sampleFR - ((sampleER + sampleGR) / 2.0)) * gain);
            trigger = sin(midA + midB + midC + midD);
            inputSampleR = sampleER + (trigger * 8.0);
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
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
} // namespace airwindohhs::bite
