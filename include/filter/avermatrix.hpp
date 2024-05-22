#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class AverMatrix final : public Effect<T>
{
    const std::string m_name{ "AverMatrix" };

    double bL[11][11];
    double bR[11][11];
    double f[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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
    AverMatrix()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        for (int x = 0; x < 11; x++) {
            f[x] = 0.0;
            for (int y = 0; y < 11; y++) {
                bL[x][y] = 0.0;
                bR[x][y] = 0.0;
            }
        }
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
            case kParamA: return "Average";
            case kParamB: return "Depth";
            case kParamC: return "Inv/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 9.0) + 1.0);
            case kParamB: return std::to_string((B * 9.0) + 1.0);
            case kParamC: return std::to_string((C * 2.0) - 1.0);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "taps";
            case kParamB: return "poles";
            case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overalltaps = (A * 9.0) + 1.0;
        double taps = overalltaps;
        // this is our averaging, which is not integer but continuous
        double overallpoles = (B * 9.0) + 1.0;
        // this is the poles of the filter, also not integer but continuous
        int yLimit = floor(overallpoles) + 1;
        double yPartial = overallpoles - floor(overallpoles);
        // now we can do a for loop, and also apply the final pole continuously
        double wet = (C * 2.0) - 1.0;
        double dry = (1.0 - wet);
        if (dry > 1.0) {
            dry = 1.0;
        }
        int xLimit = 1;
        for (int x = 0; x < 11; x++) {
            if (taps > 1.0) {
                f[x] = 1.0;
                taps -= 1.0;
                xLimit++;
            }
            else {
                f[x] = taps;
                taps = 0.0;
            }
        } // there, now we have a neat little moving average with remainders
        if (xLimit > 9) {
            xLimit = 9;
        }
        if (overalltaps < 1.0) {
            overalltaps = 1.0;
        }
        for (int x = 0; x < xLimit; x++) {
            f[x] /= overalltaps;
        } // and now it's neatly scaled, too
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
            double previousPoleL = 0;
            double previousPoleR = 0;
            for (int y = 0; y < yLimit; y++) {
                for (int x = xLimit; x >= 0; x--) {
                    bL[x + 1][y] = bL[x][y];
                    bR[x + 1][y] = bR[x][y];
                }
                bL[0][y] = previousPoleL = inputSampleL;
                bR[0][y] = previousPoleR = inputSampleR;
                inputSampleL = 0.0;
                inputSampleR = 0.0;
                for (int x = 0; x < xLimit; x++) {
                    inputSampleL += (bL[x][y] * f[x]);
                    inputSampleR += (bR[x][y] * f[x]);
                }
            }
            inputSampleL = (previousPoleL * (1.0 - yPartial)) + (inputSampleL * yPartial);
            inputSampleR = (previousPoleR * (1.0 - yPartial)) + (inputSampleR * yPartial);
            // in this way we can blend in the final pole
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            // wet can be negative, in which case dry is always full volume and they cancel
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
