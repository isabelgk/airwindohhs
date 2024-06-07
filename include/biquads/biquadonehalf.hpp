#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquadonehalf {

constexpr std::string_view k_name{ "BiquadOneHalf" };
constexpr std::string_view k_short_description{
    "BiquadOneHalf is an interleaved biquad filter like Biquad."
};
constexpr std::string_view k_long_description{
    "BiquadOneHalf is tech that’s going into my future plugins. You have to set the frequency twice as high as you would using Biquad, and it can only go up to 1/4 Nyquist, but the way it interacts with the sampling frequency is special and works very well for some types of filtering. You’ll be getting more developed plugins of that nature soon, and all through January, but for now I got this: working version of the new Biquad filter variation, that you can use on stuff.It’s the first version, not the ‘sweepable’ version, because this is going to find its way into fixed-frequency EQs, ‘head bump’ etc… compare it to Biquad remembering to use twice the frequency you otherwise would. It’s happiest on bass or midrange: much like regular Biquad, trying to force it to get real close to its high frequency limit can get messy."
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class BiquadOneHalf final : public Effect<T>
{
    double biquadAL[9];
    double biquadAR[9];
    double biquadBL[9];
    double biquadBR[9];
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    BiquadOneHalf()
    {
        for (int x = 0; x < 9; x++) {
            biquadAL[x] = 0.0;
            biquadAR[x] = 0.0;
            biquadBL[x] = 0.0;
            biquadBR[x] = 0.0;
        }
        flip = false;
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 0.0;
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

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.0;
            break;
        kParamD:
            return 0.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "type";
            break;
        kParamB:
            return "freq";
            break;
        kParamC:
            return "q";
            break;
        kParamD:
            return "inv/wet";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Type";
            break;
        kParamB:
            return "Freq";
            break;
        kParamC:
            return "Q";
            break;
        kParamD:
            return "Inv/Wet";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        int type = ceil((A * 3.999) + 0.00001);
        biquadAL[0] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquadAL[0] < 0.0001) {
            biquadAL[0] = 0.0001;
        }
        biquadAL[1] = (C * C * C * 29.99) + 0.01;
        if (biquadAL[1] < 0.0001) {
            biquadAL[1] = 0.0001;
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
        //[7] is stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is stored delayed sample (you have to include the coefficient making code if you do that)
        // to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
        // put this code either within the sample buffer (for smoothly modulating freq or res)
        // or in this 'read the controls' area (for letting you change freq and res with controls)
        // or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
        if (type == 1) { // lowpass
            double K = tan(M_PI * biquadAL[0]);
            double norm = 1.0 / (1.0 + K / biquadAL[1] + K * K);
            biquadAL[2] = K * K * norm;
            biquadAL[3] = 2.0 * biquadAL[2];
            biquadAL[4] = biquadAL[2];
            biquadAL[5] = 2.0 * (K * K - 1.0) * norm;
            biquadAL[6] = (1.0 - K / biquadAL[1] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquadAL[0]);
            double norm = 1.0 / (1.0 + K / biquadAL[1] + K * K);
            biquadAL[2] = norm;
            biquadAL[3] = -2.0 * biquadAL[2];
            biquadAL[4] = biquadAL[2];
            biquadAL[5] = 2.0 * (K * K - 1.0) * norm;
            biquadAL[6] = (1.0 - K / biquadAL[1] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquadAL[0]);
            double norm = 1.0 / (1.0 + K / biquadAL[1] + K * K);
            biquadAL[2] = K / biquadAL[1] * norm;
            biquadAL[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquadAL[4] = -biquadAL[2];
            biquadAL[5] = 2.0 * (K * K - 1.0) * norm;
            biquadAL[6] = (1.0 - K / biquadAL[1] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquadAL[0]);
            double norm = 1.0 / (1.0 + K / biquadAL[1] + K * K);
            biquadAL[2] = (1.0 + K * K) * norm;
            biquadAL[3] = 2.0 * (K * K - 1) * norm;
            biquadAL[4] = biquadAL[2];
            biquadAL[5] = biquadAL[3];
            biquadAL[6] = (1.0 - K / biquadAL[1] + K * K) * norm;
        }
        for (int x = 0; x < 7; x++) {
            biquadAR[x] = biquadBL[x] = biquadBR[x] = biquadAL[x];
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
            double tempSampleL;
            double tempSampleR;
            if (flip)
            {
                tempSampleL = (inputSampleL * biquadAL[2]) + biquadAL[7];
                biquadAL[7] = (inputSampleL * biquadAL[3]) - (tempSampleL * biquadAL[5]) + biquadAL[8];
                biquadAL[8] = (inputSampleL * biquadAL[4]) - (tempSampleL * biquadAL[6]);
                inputSampleL = tempSampleL;
                tempSampleR = (inputSampleR * biquadAR[2]) + biquadAR[7];
                biquadAR[7] = (inputSampleR * biquadAR[3]) - (tempSampleR * biquadAR[5]) + biquadAR[8];
                biquadAR[8] = (inputSampleR * biquadAR[4]) - (tempSampleR * biquadAR[6]);
                inputSampleR = tempSampleR;
            }
            else
            {
                tempSampleL = (inputSampleL * biquadBL[2]) + biquadBL[7];
                biquadBL[7] = (inputSampleL * biquadBL[3]) - (tempSampleL * biquadBL[5]) + biquadBL[8];
                biquadBL[8] = (inputSampleL * biquadBL[4]) - (tempSampleL * biquadBL[6]);
                inputSampleL = tempSampleL;
                tempSampleR = (inputSampleR * biquadBR[2]) + biquadBR[7];
                biquadBR[7] = (inputSampleR * biquadBR[3]) - (tempSampleR * biquadBR[5]) + biquadBR[8];
                biquadBR[8] = (inputSampleR * biquadBR[4]) - (tempSampleR * biquadBR[6]);
                inputSampleR = tempSampleR;
            }
            flip = !flip;
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
} // namespace airwindohhs::biquadonehalf
