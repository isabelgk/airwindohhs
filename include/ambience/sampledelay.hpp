#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class SampleDelay final : public Effect<T>
{
    const std::string m_name{ "SampleDelay" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double pL[16386];
    double pR[16386];
    int gcount;
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    SampleDelay()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        for (int count = 0; count < 16385; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        gcount = 0;
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
            case kParamA: return "ms";
            case kParamB: return "Samples";
            case kParamC: return "Subsamp";
            case kParamD: return "Inv/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(floor);
            case kParamB: return std::to_string(floor);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string((D * 2.0) - 1.0);

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
            case kParamC: return "";
            case kParamD: return "";
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
        double coarse = A * (4410.0 * overallscale);
        double fine = B * 10.0;
        double subsample = C;
        int offset = floor(coarse + fine);
        if (offset > 16380) {
            offset = 16380; // insanity check
        }
        int maxtime = 16384;
        double phase = (D * 2.0) - 1.0;
        double dryLevel = 1.0 - fabs(phase);
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
            if (gcount < 0 || gcount > maxtime) {
                gcount = maxtime;
            }
            int count = gcount;
            pL[count] = inputSampleL;
            pR[count] = inputSampleR;
            count += offset;
            inputSampleL = pL[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
            inputSampleR = pR[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
            inputSampleL += pL[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
            inputSampleR += pR[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
            gcount--;
            if (phase < 1.0) {
                inputSampleL *= phase;
                inputSampleR *= phase;
            }
            if (dryLevel > 0.0) {
                inputSampleL += (drySampleL * dryLevel);
                inputSampleR += (drySampleR * dryLevel);
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
