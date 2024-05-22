#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Biquad final : public Effect<T>
{
    const std::string m_name{ "Biquad" };

    double biquad[11]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8 and 9-10, and freq/res/coefficients serve both.
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
    Biquad()
    {
        for (int x = 0; x < 11; x++) {
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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
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

    float get_parameter_value(int index) override
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

    std::string get_parameter_name(int index) override
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

    std::string get_parameter_display(int index) override
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

    std::string get_parameter_label(int index) override
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

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int type = ceil((A * 3.999) + 0.00001);
        biquad[0] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquad[0] < 0.0001) {
            biquad[0] = 0.0001;
        }
        biquad[1] = (C * C * C * 29.99) + 0.01;
        if (biquad[1] < 0.0001) {
            biquad[1] = 0.0001;
        }
        double wet = (D * 2.0) - 1.0;
        // biquad contains these values:
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        //[2] is a0 but you need distinct ones for additional biquad instances so it's here
        //[3] is a1 but you need distinct ones for additional biquad instances so it's here
        //[4] is a2 but you need distinct ones for additional biquad instances so it's here
        //[5] is b1 but you need distinct ones for additional biquad instances so it's here
        //[6] is b2 but you need distinct ones for additional biquad instances so it's here
        //[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
        //[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
        // to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
        // put this code either within the sample buffer (for smoothly modulating freq or res)
        // or in this 'read the controls' area (for letting you change freq and res with controls)
        // or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
        if (type == 1) { // lowpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K * K * norm;
            biquad[3] = 2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = norm;
            biquad[3] = -2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K / biquad[1] * norm;
            biquad[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquad[4] = -biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = (1.0 + K * K) * norm;
            biquad[3] = 2.0 * (K * K - 1) * norm;
            biquad[4] = biquad[2];
            biquad[5] = biquad[3];
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            /*
             double mid = inputSampleL + inputSampleR;
             double side = inputSampleL - inputSampleR;
             //assign mid and side.Between these sections, you can do mid/side processing
             double tempSampleM = (mid * biquad[2]) + biquad[7];
             biquad[7] = (mid * biquad[3]) - (tempSampleM * biquad[5]) + biquad[8];
             biquad[8] = (mid * biquad[4]) - (tempSampleM * biquad[6]);
             mid = tempSampleM; //like mono AU, 7 and 8 store mid channel
             double tempSampleS = (side * biquad[2]) + biquad[9];
             biquad[9] = (side * biquad[3]) - (tempSampleS * biquad[5]) + biquad[10];
             biquad[10] = (side * biquad[4]) - (tempSampleS * biquad[6]);
             inputSampleR = tempSampleS; //note: 9 and 10 store the side channel
             inputSampleL = (mid+side)/2.0;
             inputSampleR = (mid-side)/2.0;
             //unassign mid and side
             */
            double tempSampleL = (inputSampleL * biquad[2]) + biquad[7];
            biquad[7] = (inputSampleL * biquad[3]) - (tempSampleL * biquad[5]) + biquad[8];
            biquad[8] = (inputSampleL * biquad[4]) - (tempSampleL * biquad[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquad[2]) + biquad[9];
            biquad[9] = (inputSampleR * biquad[3]) - (tempSampleR * biquad[5]) + biquad[10];
            biquad[10] = (inputSampleR * biquad[4]) - (tempSampleR * biquad[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
