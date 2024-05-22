#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ADT final : public Effect<T>
{
    std::string m_name{ "ADT" };

    double pL[10000];
    double pR[10000];
    int gcount;
    double offsetA;
    double offsetB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kParamF = 5,
        kNumParameters = 6

    };

  public:
    ADT()
    {
        A = 0.5; // 0.0 to 2.0 headroom
        B = 0.5; // delay time
        C = 0.5; //-1.0  to 1.0 inv/out
        D = 0.5; // delay time
        E = 0.5; //-1.0 to 1.0 inv/out
        F = 0.5; // 0.0 to 2.0 output level
        for (int count = 0; count < 9999; count++) {
            pL[count] = 0;
            pR[count] = 0;
        }
        offsetA = 9001;
        offsetB = 9001; // :D
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
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;
            case kParamF: return F;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Headrm";
            case kParamB: return "A Delay";
            case kParamC: return "A Level";
            case kParamD: return "B Delay";
            case kParamE: return "B Level";
            case kParamF: return "Output";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A * 2.0);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string((C * 2.0) - 1.0);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string((E * 2.0) - 1.0);
            case kParamF: return std::to_string(F * 2.0);

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
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
            case kParamF: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double gain = A * 1.272;
        double targetA = pow(B, 4) * 4790.0;
        double fractionA;
        double minusA;
        double intensityA = C - 0.5;
        // first delay
        double targetB = (pow(D, 4) * 4790.0);
        double fractionB;
        double minusB;
        double intensityB = E - 0.5;
        // second delay
        double output = F * 2.0;
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
            if (fabs(offsetA - targetA) > 1000) {
                offsetA = targetA;
            }
            offsetA = ((offsetA * 999.0) + targetA) / 1000.0;
            fractionA = offsetA - floor(offsetA);
            minusA = 1.0 - fractionA;
            if (fabs(offsetB - targetB) > 1000) {
                offsetB = targetB;
            }
            offsetB = ((offsetB * 999.0) + targetB) / 1000.0;
            fractionB = offsetB - floor(offsetB);
            minusB = 1.0 - fractionB;
            // chase delay taps for smooth action
            if (gain > 0) {
                inputSampleL /= gain;
                inputSampleR /= gain;
            }
            if (inputSampleL > 1.2533141373155) {
                inputSampleL = 1.2533141373155;
            }
            if (inputSampleL < -1.2533141373155) {
                inputSampleL = -1.2533141373155;
            }
            if (inputSampleR > 1.2533141373155) {
                inputSampleR = 1.2533141373155;
            }
            if (inputSampleR < -1.2533141373155) {
                inputSampleR = -1.2533141373155;
            }
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            // Spiral: lean out the sound a little when decoded by ConsoleBuss
            if (gcount < 1 || gcount > 4800) {
                gcount = 4800;
            }
            int count = gcount;
            double totalL = 0.0;
            double totalR = 0.0;
            double tempL;
            double tempR;
            pL[count + 4800] = pL[count] = inputSampleL;
            pR[count + 4800] = pR[count] = inputSampleR;
            // double buffer
            if (intensityA != 0.0)
            {
                count = (int)(gcount + floor(offsetA));
                tempL = (pL[count] * minusA); // less as value moves away from .0
                tempL += pL[count + 1]; // we can assume always using this in one way or another?
                tempL += (pL[count + 2] * fractionA); // greater as value moves away from .0
                tempL -= (((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
                totalL += (tempL * intensityA);
                tempR = (pR[count] * minusA); // less as value moves away from .0
                tempR += pR[count + 1]; // we can assume always using this in one way or another?
                tempR += (pR[count + 2] * fractionA); // greater as value moves away from .0
                tempR -= (((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
                totalR += (tempR * intensityA);
            }
            if (intensityB != 0.0)
            {
                count = (int)(gcount + floor(offsetB));
                tempL = (pL[count] * minusB); // less as value moves away from .0
                tempL += pL[count + 1]; // we can assume always using this in one way or another?
                tempL += (pL[count + 2] * fractionB); // greater as value moves away from .0
                tempL -= (((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
                totalL += (tempL * intensityB);
                tempR = (pR[count] * minusB); // less as value moves away from .0
                tempR += pR[count + 1]; // we can assume always using this in one way or another?
                tempR += (pR[count + 2] * fractionB); // greater as value moves away from .0
                tempR -= (((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
                totalR += (tempR * intensityB);
            }
            gcount--;
            // still scrolling through the samples, remember
            inputSampleL += totalL;
            inputSampleR += totalR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            inputSampleL *= gain;
            inputSampleR *= gain;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
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
