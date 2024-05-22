#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Hombre final : public Effect<T>
{
    std::string m_name{ "Hombre" };

    double pL[4001];
    double pR[4001];
    double slide;
    int gcount;
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
    Hombre()
    {
        A = 0.421;
        B = 0.5;
        for (int count = 0; count < 4000; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        gcount = 0;
        slide = 0.421;
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Voicing";
            case kParamB: return "Intensity";

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
        double target = A;
        double offsetA;
        double offsetB;
        int widthA = (int)(1.0 * overallscale);
        int widthB = (int)(7.0 * overallscale); // max 364 at 44.1, 792 at 96K
        double wet = B;
        // removed extra dry variable
        double totalL;
        double totalR;
        int count;
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
            slide = (slide * 0.9997) + (target * 0.0003);
            offsetA = ((pow(slide, 2)) * 77) + 3.2;
            offsetB = (3.85 * offsetA) + 41;
            offsetA *= overallscale;
            offsetB *= overallscale;
            // adjust for sample rate
            if (gcount < 1 || gcount > 2000) {
                gcount = 2000;
            }
            count = gcount;
            pL[count + 2000] = pL[count] = inputSampleL;
            pR[count + 2000] = pR[count] = inputSampleR;
            // double buffer
            count = (int)(gcount + floor(offsetA));
            totalL = pL[count] * 0.391; // less as value moves away from .0
            totalL += pL[count + widthA]; // we can assume always using this in one way or another?
            totalL += pL[count + widthA + widthA] * 0.391; // greater as value moves away from .0
            totalR = pR[count] * 0.391; // less as value moves away from .0
            totalR += pR[count + widthA]; // we can assume always using this in one way or another?
            totalR += pR[count + widthA + widthA] * 0.391; // greater as value moves away from .0
            inputSampleL += ((totalL * 0.274));
            inputSampleR += ((totalR * 0.274));
            count = (int)(gcount + floor(offsetB));
            totalL = pL[count] * 0.918; // less as value moves away from .0
            totalL += pL[count + widthB]; // we can assume always using this in one way or another?
            totalL += pL[count + widthB + widthB] * 0.918; // greater as value moves away from .0
            totalR = pR[count] * 0.918; // less as value moves away from .0
            totalR += pR[count + widthB]; // we can assume always using this in one way or another?
            totalR += pR[count + widthB + widthB] * 0.918; // greater as value moves away from .0
            inputSampleL -= ((totalL * 0.629));
            inputSampleR -= ((totalR * 0.629));
            inputSampleL /= 4;
            inputSampleR /= 4;
            gcount--;
            // still scrolling through the samples, remember
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
