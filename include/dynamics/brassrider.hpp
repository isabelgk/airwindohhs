#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BrassRider final : public Effect<T>
{
    std::string m_name{ "BrassRider" };

    double d[80002];
    double e[80002];
    double highIIRL;
    double slewIIRL;
    double highIIR2L;
    double slewIIR2L;
    double highIIRR;
    double slewIIRR;
    double highIIR2R;
    double slewIIR2R;
    double control;
    double clamp;
    double lastSampleL;
    double lastSlewL;
    double lastSampleR;
    double lastSlewR;
    int gcount;
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
    BrassRider()
    {
        A = 0.0;
        B = 0.0;
        for (int count = 0; count < 80001; count++) {
            d[count] = 0.0;
            e[count] = 0.0;
        }
        control = 0.0;
        clamp = 0.0;
        highIIRL = 0.0;
        slewIIRL = 0.0;
        highIIR2L = 0.0;
        slewIIR2L = 0.0;
        lastSampleL = 0.0;
        lastSlewL = 0.0;
        highIIRR = 0.0;
        slewIIRR = 0.0;
        highIIR2R = 0.0;
        slewIIR2R = 0.0;
        lastSampleR = 0.0;
        lastSlewR = 0.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thresh";
            case kParamB: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double limitOut = A * 16;
        int offsetA = 13500;
        int offsetB = 16700;
        double wet = B;
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
            inputSampleL *= limitOut;
            highIIRL = (highIIRL * 0.5);
            highIIRL += (inputSampleL * 0.5);
            inputSampleL -= highIIRL;
            highIIR2L = (highIIR2L * 0.5);
            highIIR2L += (inputSampleL * 0.5);
            inputSampleL -= highIIR2L;
            double slewSampleL = fabs(inputSampleL - lastSampleL);
            lastSampleL = inputSampleL;
            slewSampleL /= fabs(inputSampleL * lastSampleL) + 0.2;
            slewIIRL = (slewIIRL * 0.5);
            slewIIRL += (slewSampleL * 0.5);
            slewSampleL = fabs(slewSampleL - slewIIRL);
            slewIIR2L = (slewIIR2L * 0.5);
            slewIIR2L += (slewSampleL * 0.5);
            slewSampleL = fabs(slewSampleL - slewIIR2L);
            double bridgerectifier = slewSampleL;
            // there's the left channel, now to feed it to overall clamp
            if (bridgerectifier > 3.1415) {
                bridgerectifier = 0.0;
            }
            bridgerectifier = sin(bridgerectifier);
            if (gcount < 0 || gcount > 40000) {
                gcount = 40000;
            }
            d[gcount + 40000] = d[gcount] = bridgerectifier;
            control += (d[gcount] / (offsetA + 1));
            control -= (d[gcount + offsetA] / offsetA);
            double ramp = (control * control) * 16.0;
            e[gcount + 40000] = e[gcount] = ramp;
            clamp += (e[gcount] / (offsetB + 1));
            clamp -= (e[gcount + offsetB] / offsetB);
            if (clamp > wet * 8) {
                clamp = wet * 8;
            }
            gcount--;
            inputSampleR *= limitOut;
            highIIRR = (highIIRR * 0.5);
            highIIRR += (inputSampleR * 0.5);
            inputSampleR -= highIIRR;
            highIIR2R = (highIIR2R * 0.5);
            highIIR2R += (inputSampleR * 0.5);
            inputSampleR -= highIIR2R;
            double slewSampleR = fabs(inputSampleR - lastSampleR);
            lastSampleR = inputSampleR;
            slewSampleR /= fabs(inputSampleR * lastSampleR) + 0.2;
            slewIIRR = (slewIIRR * 0.5);
            slewIIRR += (slewSampleR * 0.5);
            slewSampleR = fabs(slewSampleR - slewIIRR);
            slewIIR2R = (slewIIR2R * 0.5);
            slewIIR2R += (slewSampleR * 0.5);
            slewSampleR = fabs(slewSampleR - slewIIR2R);
            bridgerectifier = slewSampleR;
            // there's the right channel, now to feed it to overall clamp
            if (bridgerectifier > 3.1415) {
                bridgerectifier = 0.0;
            }
            bridgerectifier = sin(bridgerectifier);
            if (gcount < 0 || gcount > 40000) {
                gcount = 40000;
            }
            d[gcount + 40000] = d[gcount] = bridgerectifier;
            control += (d[gcount] / (offsetA + 1));
            control -= (d[gcount + offsetA] / offsetA);
            ramp = (control * control) * 16.0;
            e[gcount + 40000] = e[gcount] = ramp;
            clamp += (e[gcount] / (offsetB + 1));
            clamp -= (e[gcount + offsetB] / offsetB);
            if (clamp > wet * 8) {
                clamp = wet * 8;
            }
            gcount--;
            inputSampleL = (drySampleL * (1.0 - wet)) + (drySampleL * clamp * wet * 16.0);
            inputSampleR = (drySampleR * (1.0 - wet)) + (drySampleR * clamp * wet * 16.0);
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
