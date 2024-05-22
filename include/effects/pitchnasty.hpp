#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PitchNasty final : public Effect<T>
{
    std::string m_name{ "PitchNasty" };

    double dL[10002];
    double dR[10002];
    int inCount;
    double outCount;
    bool switchTransition;
    double switchAmountL;
    double switchAmountR;
    double feedbackSampleL;
    double feedbackSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.

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
    PitchNasty()
    {
        A = 0.0;
        B = 0.0;
        C = 0.25;
        D = 0.0;
        E = 1.0;
        F = 1.0;
        for (int temp = 0; temp < 10001; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        inCount = 1;
        outCount = 1.0;
        switchTransition = false;
        switchAmountL = 0.0;
        switchAmountR = 0.0;
        feedbackSampleL = 0.0;
        feedbackSampleR = 0.0;
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
            case kParamA: return "Note";
            case kParamB: return "Bend";
            case kParamC: return "Grind";
            case kParamD: return "Feedbck";
            case kParamE: return "Output";
            case kParamF: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 24) - 12);
            case kParamB: return std::to_string((B * 24.0) - 12.0);
            case kParamC: return std::to_string((C * 72) - 36);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);
            case kParamF: return std::to_string(F);

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

        double note = (int)(A * 24) - 12; // -12 to +12
        double bend = (B * 24.0) - 12.0; // -12 to +12
        double speed = pow(2, note / 12.0) * pow(2, bend / 12.0);
        double grindRef = (1.0 / 261.6) * getSampleRate(); // samples per Middle C cycle
        double grindNote = pow(2, ((int)(C * 72) - 36) / 12.0);
        double width = grindRef / grindNote;
        if (width > 9990) {
            width = 9990; // safety check
        }
        double feedback = D;
        double trim = E;
        double wet = F;
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
            inputSampleL += (feedbackSampleL * feedback);
            inputSampleR += (feedbackSampleR * feedback);
            if (fabs(feedbackSampleL) > 1.0) {
                inputSampleL /= fabs(feedbackSampleL);
            }
            if (fabs(feedbackSampleR) > 1.0) {
                inputSampleR /= fabs(feedbackSampleR);
            }
            dL[inCount] = inputSampleL;
            dR[inCount] = inputSampleR;
            inCount++;
            outCount += speed;
            if (outCount > inCount && switchTransition) {
                switchAmountL = 1.0;
                switchAmountR = 1.0;
                switchTransition = false;
            }
            if (inCount > width) {
                inCount -= width;
            }
            if (outCount > width) {
                outCount -= width;
                switchTransition = true;
                // reset the transition point
            }
            int count = floor(outCount);
            int arrayWidth = floor(width);
            inputSampleL = (dL[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (outCount - floor(outCount))));
            inputSampleL += (dL[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (outCount - floor(outCount)));
            inputSampleR = (dR[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (outCount - floor(outCount))));
            inputSampleR += (dR[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (outCount - floor(outCount)));
            if (switchAmountL > fabs(inputSampleL - dL[inCount]) * 2.0) {
                switchAmountL = (switchAmountL * 0.5) + (fabs(inputSampleL - dL[inCount]));
            }
            if (switchAmountR > fabs(inputSampleR - dR[inCount]) * 2.0) {
                switchAmountR = (switchAmountR * 0.5) + (fabs(inputSampleR - dR[inCount]));
            }
            inputSampleL = (dL[inCount] * switchAmountL) + (inputSampleL * (1.0 - switchAmountL));
            inputSampleR = (dR[inCount] * switchAmountR) + (inputSampleR * (1.0 - switchAmountR));
            feedbackSampleL = inputSampleL;
            feedbackSampleR = inputSampleR;
            if (trim != 1.0) {
                inputSampleL *= trim;
                inputSampleR *= trim;
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs
