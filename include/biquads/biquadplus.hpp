#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BiquadPlus final : public Effect<T>
{
    std::string m_name{ "BiquadPlus" };

    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_aA0,
        biq_aA1,
        biq_aA2,
        biq_bA1,
        biq_bA2,
        biq_aB0,
        biq_aB1,
        biq_aB2,
        biq_bB1,
        biq_bB2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    };
    double biquad[biq_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamfor (int x = 0,
kParamA = 1,
kParamB = 2,
kParamC = 3,
kNumParameters = 4

    };

  public:
    BiquadPlus()
    {
        for (int x = 0; x < biq_total; x++) {
            biquad[x] = 0.0;
        }
        A = 1.0;
        B = 0.5;
        C = 0.5;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: for (int x = value; break;
case kParamA: A = value; break;
case kParamB: B = value; break;
case kParamC: C = value; break;

        default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return for (int x;
case kParamA: return A;
case kParamB: return B;
case kParamC: return C;

        default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "Type";
        case kParamA: return "Freq";
        case kParamB: return "Q";
        case kParamC: return "Inv/Wet";

        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return std::to_string((A*3.999)+0.00001));
        case kParamA: return std::to_string((B * B * B * 0.9999) + 0.0001);
        case kParamB: return std::to_string((C * C * C * 29.99) + 0.01);
        case kParamC: return std::to_string((D * 2.0) - 1.0);

        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "";
        case kParamA: return "";
        case kParamB: return "";
        case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int type = ceil((A * 3.999) + 0.00001);
        biquad[biq_freq] = ((B * B * B * 20000.0) / getSampleRate());
        if (biquad[biq_freq] < 0.0001) {
            biquad[biq_freq] = 0.0001;
        }
        biquad[biq_reso] = (C * C * C * 29.99) + 0.01;
        if (biquad[biq_reso] < 0.0001) {
            biquad[biq_reso] = 0.0001;
        }
        double wet = (D * 2.0) - 1.0;
        // to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
        // put this code either within the sample buffer (for smoothly modulating freq or res)
        // or in this 'read the controls' area (for letting you change freq and res with controls)
        // or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
        biquad[biq_aA0] = biquad[biq_aB0];
        biquad[biq_aA1] = biquad[biq_aB1];
        biquad[biq_aA2] = biquad[biq_aB2];
        biquad[biq_bA1] = biquad[biq_bB1];
        biquad[biq_bA2] = biquad[biq_bB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        if (type == 1) { // lowpass
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = K * K * norm;
            biquad[biq_aB1] = 2.0 * biquad[biq_aB0];
            biquad[biq_aB2] = biquad[biq_aB0];
            biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = norm;
            biquad[biq_aB1] = -2.0 * biquad[biq_aB0];
            biquad[biq_aB2] = biquad[biq_aB0];
            biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = K / biquad[biq_reso] * norm;
            biquad[biq_aB1] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquad[biq_aB2] = -biquad[biq_aB0];
            biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = (1.0 + K * K) * norm;
            biquad[biq_aB1] = 2.0 * (K * K - 1) * norm;
            biquad[biq_aB2] = biquad[biq_aB0];
            biquad[biq_bB1] = biquad[biq_aB1];
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (biquad[biq_aA0] == 0.0) { // if we have just started, start directly with raw info
            biquad[biq_aA0] = biquad[biq_aB0];
            biquad[biq_aA1] = biquad[biq_aB1];
            biquad[biq_aA2] = biquad[biq_aB2];
            biquad[biq_bA1] = biquad[biq_bB1];
            biquad[biq_bA2] = biquad[biq_bB2];
        }
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
            double buf = (double)sampleFrames / inFramesToProcess;
            biquad[biq_a0] = (biquad[biq_aA0] * buf) + (biquad[biq_aB0] * (1.0 - buf));
            biquad[biq_a1] = (biquad[biq_aA1] * buf) + (biquad[biq_aB1] * (1.0 - buf));
            biquad[biq_a2] = (biquad[biq_aA2] * buf) + (biquad[biq_aB2] * (1.0 - buf));
            biquad[biq_b1] = (biquad[biq_bA1] * buf) + (biquad[biq_bB1] * (1.0 - buf));
            biquad[biq_b2] = (biquad[biq_bA2] * buf) + (biquad[biq_bB2] * (1.0 - buf));
            double tempSample = (inputSampleL * biquad[biq_a0]) + biquad[biq_sL1];
            biquad[biq_sL1] = (inputSampleL * biquad[biq_a1]) - (tempSample * biquad[biq_b1]) + biquad[biq_sL2];
            biquad[biq_sL2] = (inputSampleL * biquad[biq_a2]) - (tempSample * biquad[biq_b2]);
            inputSampleL = tempSample;
            tempSample = (inputSampleR * biquad[biq_a0]) + biquad[biq_sR1];
            biquad[biq_sR1] = (inputSampleR * biquad[biq_a1]) - (tempSample * biquad[biq_b1]) + biquad[biq_sR2];
            biquad[biq_sR2] = (inputSampleR * biquad[biq_a2]) - (tempSample * biquad[biq_b2]);
            inputSampleR = tempSample;
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
                // inv/dry/wet lets us turn LP into HP and band into notch
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
