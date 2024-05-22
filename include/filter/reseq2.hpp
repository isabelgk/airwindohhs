#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ResEQ2 final : public Effect<T>
{
    const std::string m_name{ "ResEQ2" };

    double mpkL[2005];
    double mpkR[2005];
    double f[66];
    double prevfreqMPeak;
    double prevamountMPeak;
    int mpc;
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
    ResEQ2()
    {
        A = 0.5;
        B = 0.0;
        for (int count = 0; count < 2004; count++) {
            mpkL[count] = 0.0;
            mpkR[count] = 0.0;
        }
        for (int count = 0; count < 65; count++) {
            f[count] = 0.0;
        }
        prevfreqMPeak = -1;
        prevamountMPeak = -1;
        mpc = 1;
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
            case kParamA: return "MSweep";
            case kParamB: return "MBoost";

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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        // begin ResEQ2 Mid Boost
        double freqMPeak = pow(A + 0.15, 3);
        double amountMPeak = pow(B, 2);
        int maxMPeak = (amountMPeak * 63.0) + 1;
        if ((freqMPeak != prevfreqMPeak) || (amountMPeak != prevamountMPeak)) {
            for (int x = 0; x < maxMPeak; x++) {
                if (((double)x * freqMPeak) < M_PI_4) {
                    f[x] = sin(((double)x * freqMPeak) * 4.0) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
                else {
                    f[x] = cos((double)x * freqMPeak) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
            }
            prevfreqMPeak = freqMPeak;
            prevamountMPeak = amountMPeak;
        } // end ResEQ2 Mid Boost
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
            // begin ResEQ2 Mid Boost
            mpc++;
            if (mpc < 1 || mpc > 2001) {
                mpc = 1;
            }
            mpkL[mpc] = inputSampleL;
            mpkR[mpc] = inputSampleR;
            double midMPeakL = 0.0;
            double midMPeakR = 0.0;
            for (int x = 0; x < maxMPeak; x++) {
                int y = x * cycleEnd;
                switch (cycleEnd)
                {
                    case 1:
                        midMPeakL += (mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        midMPeakR += (mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        break;
                    case 2:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        break;
                    case 3:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        break;
                    case 4:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25); // break
                }
            }
            inputSampleL = (midMPeakL * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleL : inputSampleL * (1.5 - amountMPeak));
            inputSampleR = (midMPeakR * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleR : inputSampleR * (1.5 - amountMPeak));
            // end ResEQ2 Mid Boost
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
