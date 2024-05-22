#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Hull final : public Effect<T>
{
    std::string m_name{ "Hull" };

    double bL[4005];
    double bR[4005];
    int bPointer;
    double cL[105];
    double cR[105];
    int cPointer;
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
    Hull()
    {
        A = 0.0;
        B = 0.5;
        for (int count = 0; count < 4002; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
        }
        for (int count = 0; count < 102; count++) {
            cL[count] = 0.0;
            cR[count] = 0.0;
        }
        bPointer = 1;
        cPointer = 1;
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
            case kParamA: return "Freq";
            case kParamB: return "Bright";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string((B * 2.0) - 1.0);

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
        overallscale /= 96000.0;
        overallscale *= getSampleRate();
        if (overallscale > 1.0) {
            overallscale = 1.0; // and if you go for 192k, rather than crash
        }
        // it just cuts out the maximum (2000) depth of averaging you can get
        double hullSetting = pow(A, 3) * overallscale;
        int limitA = (hullSetting * 2000.0) + 1.0;
        double divisorA = 1.0 / limitA;
        int limitB = (hullSetting * 1000.0) + 1.0;
        double divisorB = 1.0 / limitB;
        int limitC = sqrt(hullSetting * 2000.0) + 1.0;
        double divisorC = 1.0 / limitC;
        double wet = -((B * 2.0) - 1.0); // functions as dark/bright
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
            bPointer--;
            if (bPointer < 0) {
                bPointer += 2000;
            }
            bL[bPointer] = bL[bPointer + 2000] = inputSampleL;
            bR[bPointer] = bR[bPointer + 2000] = inputSampleR;
            int x = bPointer;
            double longAverageL = 0.0;
            double longAverageR = 0.0;
            double shortAverageL = 0.0;
            double shortAverageR = 0.0;
            while (x < bPointer + limitB) {
                shortAverageL += bL[x];
                shortAverageR += bR[x];
                longAverageL += bL[x];
                longAverageR += bR[x];
                x++;
            } // once shorter average is reached, continue to longer
            while (x < bPointer + limitA) {
                longAverageL += bL[x];
                longAverageR += bR[x];
                x++;
            }
            longAverageL *= divisorA;
            longAverageR *= divisorA;
            shortAverageL *= divisorB;
            shortAverageR *= divisorB;
            cPointer--;
            if (cPointer < 0) {
                cPointer += 50;
            }
            cL[cPointer] = cL[cPointer + 50] = shortAverageL + (shortAverageL - longAverageL);
            cR[cPointer] = cR[cPointer + 50] = shortAverageR + (shortAverageR - longAverageR);
            x = cPointer;
            double shortestAverageL = 0.0;
            double shortestAverageR = 0.0;
            while (x < cPointer + limitC) {
                shortestAverageL += cL[x];
                shortestAverageR += cR[x];
                x++;
            }
            shortestAverageL *= divisorC;
            shortestAverageR *= divisorC;
            if (wet > 0.0) {
                inputSampleL = (shortestAverageL * wet) + (drySampleL * (1.0 - wet)); // dark
                inputSampleR = (shortestAverageR * wet) + (drySampleR * (1.0 - wet)); // dark
            }
            else {
                inputSampleL = ((inputSampleL - shortestAverageL) * fabs(wet)) + (drySampleL * (1.0 + wet)); // bright
                inputSampleR = ((inputSampleR - shortestAverageR) * fabs(wet)) + (drySampleR * (1.0 + wet)); // bright
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs
