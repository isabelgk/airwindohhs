#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Hull2 final : public Effect<T>
{
    const std::string m_name{ "Hull2" };

    double hullL[225];
    double hullR[225];
    int hullp;
    double hullbL[5];
    double hullbR[5];
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
    Hull2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int count = 0; count < 222; count++) {
            hullL[count] = 0.0;
            hullR[count] = 0.0;
        }
        for (int count = 0; count < 4; count++) {
            hullbL[count] = 0.0;
            hullbR[count] = 0.0;
        }
        hullp = 1;
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
            case kParamA: return "Treble";
            case kParamB: return "Mid";
            case kParamC: return "Bass";

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
            case kParamC: return std::to_string(C);

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
        // max out at 4x, 192k
        double treble = A * 2.0;
        double mid = B * 2.0;
        double bass = C * 2.0;
        double iirAmount = 0.125 / cycleEnd;
        int limit = 4 * cycleEnd;
        double divisor = 2.0 / limit;
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
            // begin Hull2 treble crossover
            hullp--;
            if (hullp < 0) {
                hullp += 60;
            }
            hullL[hullp] = hullL[hullp + 60] = inputSampleL;
            hullR[hullp] = hullR[hullp + 60] = inputSampleR;
            int x = hullp;
            double midSampleL = 0.0;
            double midSampleR = 0.0;
            while (x < hullp + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 20] = hullL[hullp + 80] = midSampleL;
            hullR[hullp + 20] = hullR[hullp + 80] = midSampleR;
            x = hullp + 20;
            midSampleL = midSampleR = 0.0;
            while (x < hullp + 20 + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + 20 + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 40] = hullL[hullp + 100] = midSampleL;
            hullR[hullp + 40] = hullR[hullp + 100] = midSampleR;
            x = hullp + 40;
            midSampleL = midSampleR = 0.0;
            while (x < hullp + 40 + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + 40 + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            double trebleSampleL = drySampleL - midSampleL;
            double trebleSampleR = drySampleR - midSampleR;
            // end Hull2 treble crossover
            // begin Hull2 midbass crossover
            double bassSampleL = midSampleL;
            double bassSampleR = midSampleR;
            x = 0;
            while (x < 3) {
                hullbL[x] = (hullbL[x] * (1.0 - iirAmount)) + (bassSampleL * iirAmount);
                hullbR[x] = (hullbR[x] * (1.0 - iirAmount)) + (bassSampleR * iirAmount);
                if (fabs(hullbL[x]) < 1.18e-37) {
                    hullbL[x] = 0.0;
                }
                if (fabs(hullbR[x]) < 1.18e-37) {
                    hullbR[x] = 0.0;
                }
                bassSampleL = hullbL[x];
                bassSampleR = hullbR[x];
                x++;
            }
            midSampleL -= bassSampleL;
            midSampleR -= bassSampleR;
            // end Hull2 midbass crossover
            inputSampleL = (bassSampleL * bass) + (midSampleL * mid) + (trebleSampleL * treble);
            inputSampleR = (bassSampleR * bass) + (midSampleR * mid) + (trebleSampleR * treble);
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
