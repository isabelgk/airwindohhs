#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Ensemble final : public Effect<T>
{
    const std::string m_name{ "Ensemble" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    const static int totalsamples = 65540;
    float dL[totalsamples];
    float dR[totalsamples];
    double sweep[49];
    int gcount;
    double airPrevL;
    double airEvenL;
    double airOddL;
    double airFactorL;
    double airPrevR;
    double airEvenR;
    double airOddR;
    double airFactorR;
    bool fpFlip;
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
    Ensemble()
    {
        A = 0.5;
        B = 0.0;
        C = 1.0;
        D = 1.0;
        for (int count = 0; count < totalsamples - 1; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        for (int count = 0; count < 49; count++) {
            sweep[count] = 3.141592653589793238 / 2.0;
        }
        gcount = 0;
        airPrevL = 0.0;
        airEvenL = 0.0;
        airOddL = 0.0;
        airFactorL = 0.0;
        airPrevR = 0.0;
        airEvenR = 0.0;
        airOddR = 0.0;
        airFactorR = 0.0;
        fpFlip = true;
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
            case kParamA: return "Ensemble";
            case kParamB: return "Fullness";
            case kParamC: return "Brighten";
            case kParamD: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(floor);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "vox";
            case kParamB: return " ";
            case kParamC: return " ";
            case kParamD: return " ";
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
        double spd = pow(0.4 + (B / 12), 10);
        spd *= overallscale;
        double depth = 0.002 / spd;
        double tupi = 3.141592653589793238 * 2.0;
        double taps = floor((A * 46.0) + 2.9);
        double brighten = C;
        double wet = D;
        // removed extra dry variable
        double hapi = 3.141592653589793238 / taps;
        double offset;
        double floffset;
        double start[49];
        double sinoffset[49];
        double speed[49];
        int count;
        int ensemble;
        double tempL;
        double tempR;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        // now we'll precalculate some stuff that needn't be in every sample
        for (count = 1; count <= taps; count++)
        {
            start[count] = depth * count;
            sinoffset[count] = hapi * (count - 1);
            speed[count] = spd / (1 + (count / taps));
        }
        // that's for speeding up things in the sample-processing area
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
            airFactorL = airPrevL - inputSampleL;
            if (fpFlip) {
                airEvenL += airFactorL;
                airOddL -= airFactorL;
                airFactorL = airEvenL;
            }
            else {
                airOddL += airFactorL;
                airEvenL -= airFactorL;
                airFactorL = airOddL;
            }
            airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
            airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
            airPrevL = inputSampleL;
            inputSampleL += (airFactorL * brighten);
            // air, compensates for loss of highs in flanger's interpolation
            airFactorR = airPrevR - inputSampleR;
            if (fpFlip) {
                airEvenR += airFactorR;
                airOddR -= airFactorR;
                airFactorR = airEvenR;
            }
            else {
                airOddR += airFactorR;
                airEvenR -= airFactorR;
                airFactorR = airOddR;
            }
            airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
            airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
            airPrevR = inputSampleR;
            inputSampleR += (airFactorR * brighten);
            // air, compensates for loss of highs in flanger's interpolation
            fpFlip = !fpFlip;
            if (gcount < 1 || gcount > 32767) {
                gcount = 32767;
            }
            count = gcount;
            dL[count + 32767] = dL[count] = tempL = inputSampleL;
            dR[count + 32767] = dR[count] = tempR = inputSampleR;
            // double buffer
            for (ensemble = 1; ensemble <= taps; ensemble++)
            {
                offset = start[ensemble] + (depth * sin(sweep[ensemble] + sinoffset[ensemble]));
                floffset = offset - floor(offset);
                count = gcount + (int)floor(offset);
                tempL += dL[count] * (1 - floffset); // less as value moves away from .0
                tempL += dL[count + 1]; // we can assume always using this in one way or another?
                tempL += dL[count + 2] * floffset; // greater as value moves away from .0
                tempL -= ((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50; // interpolation hacks 'r us
                tempR += dR[count] * (1 - floffset); // less as value moves away from .0
                tempR += dR[count + 1]; // we can assume always using this in one way or another?
                tempR += dR[count + 2] * floffset; // greater as value moves away from .0
                tempR -= ((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50; // interpolation hacks 'r us
                sweep[ensemble] += speed[ensemble];
                if (sweep[ensemble] > tupi) {
                    sweep[ensemble] -= tupi;
                }
            }
            gcount--;
            // still scrolling through the samples, remember
            inputSampleL = tempL / (4.0 * sqrt(taps));
            inputSampleR = tempR / (4.0 * sqrt(taps));
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