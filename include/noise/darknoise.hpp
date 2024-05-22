#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class DarkNoise final : public Effect<T>
{
    const std::string m_name{ "DarkNoise" };

    double rL[8193];
    double bL[11][11];
    double lastRandyL;
    double outputNoiseL;
    double rR[8193];
    double bR[11][11];
    double lastRandyR;
    double outputNoiseR;
    int freq;
    double f[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamfor(int count = 0,
kParamfor(int x = 1,
kParamf[x] = 2,
kParamfor (int y = 3,
kNumParameters = 4

    };

  public:
    DarkNoise()
    {
        for (int count = 0; count < 8192; count++) {
            rL[count] = 0.5;
            rR[count] = 0.5;
        }
        for (int x = 0; x < 11; x++) {
            f[x] = 0.0;
            for (int y = 0; y < 11; y++) {
                bL[x][y] = 0.0;
                bR[x][y] = 0.0;
            }
        }
        lastRandyL = 0.0;
        outputNoiseL = 0.5;
        lastRandyR = 0.0;
        outputNoiseR = 0.5;
        freq = 1;
        A = 0.5;
        B = 1.0;
        C = 1.0;
        D = 1.0;
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
        case kParamfor(int count: for(int count = value; break;
case kParamfor(int x: for(int x = value; break;
case kParamf[x]: f[x] = value; break;
case kParamfor (int y: for (int y = value; break;

        default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return for(int count;
case kParamfor(int x: return for(int x;
case kParamf[x]: return f[x];
case kParamfor (int y: return for (int y;

        default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return "Freq";
case kParamfor(int x: return "Dark";
case kParamf[x]: return "Output";
case kParamfor (int y: return "Dry/Wet";

default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return std::to_string(A);
case kParamfor(int x: return std::to_string(B);
case kParamf[x]: return std::to_string(C);
case kParamfor (int y: return std::to_string(D);

default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
        case kParamfor(int count: return "";
case kParamfor(int x: return "";
case kParamf[x]: return "";
case kParamfor (int y: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        int freqTarget = (int)(pow(A, 3) * 8192.0);
        if (freqTarget < 2) {
            freqTarget = 2;
        }
        double volumeScale = (1.0 / freqTarget) * sqrt(freqTarget);
        double overalltaps = (pow(A, 2) * 8.0) + 2.0;
        double taps = overalltaps;
        // this is our averaging, which is not integer but continuous
        double overallpoles = (A * 9.0) + 1.0;
        // this is the poles of the filter, also not integer but continuous
        int yLimit = floor(overallpoles) + 1;
        double yPartial = overallpoles - floor(overallpoles);
        // now we can do a for loop, and also apply the final pole continuously
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
        double dark = B;
        double out = C * 0.5;
        double wet = D;
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
            if (freqTarget < freq) {
                outputNoiseL = ((outputNoiseL - 0.5) * (1.0 - (1.0 / freq))) + 0.5;
                outputNoiseR = ((outputNoiseR - 0.5) * (1.0 - (1.0 / freq))) + 0.5;
                rL[freq] = 0.5;
                rR[freq] = 0.5;
                freq--;
            }
            if (freqTarget > freq) {
                freq++;
            }
            // our tone control chases the input one bin at a time
            double randyL = (double)fpdL / UINT32_MAX;
            double randyR = (double)fpdR / UINT32_MAX;
            // 0 to 1 random value
            int replacementBinL = randyL * freq;
            outputNoiseL -= rL[replacementBinL];
            rL[replacementBinL] = lastRandyL;
            outputNoiseL += lastRandyL;
            lastRandyL = randyL;
            // we update only one of the slots we're using
            int replacementBinR = randyR * freq;
            outputNoiseR -= rR[replacementBinR];
            rR[replacementBinR] = lastRandyR;
            outputNoiseR += lastRandyR;
            lastRandyR = randyR;
            // we update only one of the slots we're using
            inputSampleL = (-0.5 + outputNoiseL) * volumeScale;
            inputSampleR = (-0.5 + outputNoiseR) * volumeScale;
            double nondarkSampleL = inputSampleL;
            double nondarkSampleR = inputSampleR;
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
            if (dark != 1.0) {
                inputSampleL = (inputSampleL * dark) + (nondarkSampleL * (1.0 - dark));
                inputSampleR = (inputSampleR * dark) + (nondarkSampleR * (1.0 - dark));
            }
            if (out != 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
