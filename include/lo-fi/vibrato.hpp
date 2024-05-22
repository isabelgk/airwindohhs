#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Vibrato final : public Effect<T>
{
    const std::string m_name{ "Vibrato" };

    double pL[16386]; // this is processed, not raw incoming samples
    double pR[16386]; // this is processed, not raw incoming samples
    double sweep;
    double sweepB;
    int gcount;
    double airPrevL;
    double airEvenL;
    double airOddL;
    double airFactorL;
    double airPrevR;
    double airEvenR;
    double airOddR;
    double airFactorR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
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
    Vibrato()
    {
        A = 0.3;
        B = 0.0;
        C = 0.4;
        D = 0.0;
        E = 1.0;
        for (int count = 0; count < 16385; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        sweep = 3.141592653589793238 / 2.0;
        sweepB = 3.141592653589793238 / 2.0;
        gcount = 0;
        airPrevL = 0.0;
        airEvenL = 0.0;
        airOddL = 0.0;
        airFactorL = 0.0;
        airPrevR = 0.0;
        airEvenR = 0.0;
        airOddR = 0.0;
        airFactorR = 0.0;
        flip = false;
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
            case kParamE: E = value; break;

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
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Speed";
            case kParamB: return "Depth";
            case kParamC: return "FMSpeed";
            case kParamD: return "FMDepth";
            case kParamE: return "Inv/Wet";

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
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string((E * 2.0) - 1.0);

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
            case kParamE: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double speed = pow(0.1 + A, 6);
        double depth = (pow(B, 3) / sqrt(speed)) * 4.0;
        double speedB = pow(0.1 + C, 6);
        double depthB = pow(D, 3) / sqrt(speedB);
        double tupi = 3.141592653589793238 * 2.0;
        double wet = (E * 2.0) - 1.0; // note: inv/dry/wet
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
            airFactorL = airPrevL - inputSampleL;
            airFactorR = airPrevR - inputSampleR;
            if (flip) {
                airEvenL += airFactorL;
                airOddL -= airFactorL;
                airFactorL = airEvenL;
                airEvenR += airFactorR;
                airOddR -= airFactorR;
                airFactorR = airEvenR;
            }
            else {
                airOddL += airFactorL;
                airEvenL -= airFactorL;
                airFactorL = airOddL;
                airOddR += airFactorR;
                airEvenR -= airFactorR;
                airFactorR = airOddR;
            }
            airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
            airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
            airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
            airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
            airPrevL = inputSampleL;
            airPrevR = inputSampleR;
            inputSampleL += airFactorL;
            inputSampleR += airFactorR;
            flip = !flip;
            // air, compensates for loss of highs in the interpolation
            if (gcount < 1 || gcount > 8192) {
                gcount = 8192;
            }
            int count = gcount;
            pL[count + 8192] = pL[count] = inputSampleL;
            pR[count + 8192] = pR[count] = inputSampleR;
            double offset = depth + (depth * sin(sweep));
            count += (int)floor(offset);
            inputSampleL = pL[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += pL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += pL[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
            inputSampleL -= ((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50.0; // interpolation hacks 'r us
            inputSampleL *= 0.5; // gain trim
            inputSampleR = pR[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += pR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += pR[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
            inputSampleR -= ((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50.0; // interpolation hacks 'r us
            inputSampleR *= 0.5; // gain trim
            sweep += (speed + (speedB * sin(sweepB) * depthB));
            sweepB += speedB;
            if (sweep > tupi) {
                sweep -= tupi;
            }
            if (sweep < 0.0) {
                sweep += tupi;
            } // through zero FM
            if (sweepB > tupi) {
                sweepB -= tupi;
            }
            gcount--;
            // still scrolling through the samples, remember
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
            }
            // Inv/Dry/Wet control
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
