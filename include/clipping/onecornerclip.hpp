#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class OneCornerClip final : public Effect<T>
{
    const std::string m_name{ "OneCornerClip" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double limitPosL;
    double limitNegL;
    double lastSampleR;
    double limitPosR;
    double limitNegR;
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
    OneCornerClip()
    {
        A = 0.33333333333333333;
        B = 0.966;
        C = 0.966;
        D = 0.618;
        E = 1.0;
        lastSampleL = 0.0;
        limitPosL = 0.0;
        limitNegL = 0.0;
        lastSampleR = 0.0;
        limitPosR = 0.0;
        limitNegR = 0.0;
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
            case kParamA: return "Input";
            case kParamB: return "Pos Thr";
            case kParamC: return "Neg Thr";
            case kParamD: return "Voicing";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 36.0) - 12.0);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double inputGain = pow(10.0, (((A * 36.0) - 12.0) / 20.0));
        double posThreshold = B;
        double posTargetL = posThreshold;
        double posTargetR = posThreshold;
        double negThreshold = -C;
        double negTargetL = negThreshold;
        double negTargetR = negThreshold;
        double voicing = D;
        if (voicing == 0.618) {
            voicing = 0.618033988749894848204586;
        }
        // special case: we will do a perfect golden ratio as the default 0.618
        // just 'cos magic universality sauce (seriously, it seems a sweetspot)
        if (overallscale > 0.0) {
            voicing /= overallscale;
        }
        // translate to desired sample rate, 44.1K is the base
        if (voicing < 0.0) {
            voicing = 0.0;
        }
        if (voicing > 1.0) {
            voicing = 1.0;
        }
        // some insanity checking
        double inverseHardness = 1.0 - voicing;
        bool clipEngage = false;
        double wet = E;
        // removed extra dry variable
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
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
            if (inputGain != 1.0)
            {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
                clipEngage = true;
                // if we are altering gain we will always process
            }
            else
            {
                clipEngage = false;
                // if we are not touching gain, we will bypass unless
                // a clip is actively being softened.
            }
            if (inputSampleL > posTargetL)
            {
                inputSampleL = (lastSampleL * voicing) + (posThreshold * inverseHardness);
                posTargetL = inputSampleL;
                clipEngage = true;
            }
            else
            {
                posTargetL = posThreshold;
            }
            if (inputSampleR > posTargetR)
            {
                inputSampleR = (lastSampleR * voicing) + (posThreshold * inverseHardness);
                posTargetR = inputSampleR;
                clipEngage = true;
            }
            else
            {
                posTargetR = posThreshold;
            }
            if (inputSampleL < negTargetL)
            {
                inputSampleL = (lastSampleL * voicing) + (negThreshold * inverseHardness);
                negTargetL = inputSampleL;
                clipEngage = true;
            }
            else {
                negTargetL = negThreshold;
            }
            if (inputSampleR < negTargetR)
            {
                inputSampleR = (lastSampleR * voicing) + (negThreshold * inverseHardness);
                negTargetR = inputSampleR;
                clipEngage = true;
            }
            else {
                negTargetR = negThreshold;
            }
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
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
            if (clipEngage == false)
            {
                inputSampleL = *in1;
                inputSampleR = *in2;
            }
            // fall back to raw passthrough if at all possible
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
