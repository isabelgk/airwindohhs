#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pear2 {

constexpr std::string_view k_name{ "Pear2" };
constexpr std::string_view k_short_description{
    "Pear2 is my Pear filter plus nonlinearity."
};
constexpr std::string_view k_long_description{
    "Turns out I got more use out of Pear than anybody else… until now.Pear (as a plugin) is real experimental. It's got fixed cutoff points based on bit shifting, to see what that was like and whether it would maximize tone purity. Maybe it did (you've got it already so you're free to check it out). It's got a Poles control, but it's not meant to be a sweepy synth filter, it was purely an experiment on things I could do with the Holt algorithm.Turns out you can have a lot more fun with it when you turn it loose.Pear2 doesn't restrict the frequencies. In fact it smooths the control, specifically so you can sweep it. You can sweep everything except Poles: there's a switch on that, but you get to add WAY more poles than before.And then there's the Nonlin control… and now it's time to get gnarly.This doesn't have a distortion circuit! As filthy as it can get, none of that is from distortion or saturation. It's purely from the same nonlinearity calculation present in Capacitor2, in BiquadNonlin, and so on. That is applied here to a completely different algorithm based on Holt, and the more poles you add to the gnarly brew, the weirder it gets. You can use this for a really vibey analog-style EQ (high or low shelf: I'll be using it as a crossover) or you can push it until it's making a sound that has not been heard before.So if you liked weirdness like the Y series filters, this is your new toy. Back next week with……more.Yeah, let's just call it 'more' ;)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Pear2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        prevSampL7,
        prevSlewL7,
        prevSampR7,
        prevSlewR7,
        prevSampL8,
        prevSlewL8,
        prevSampR8,
        prevSlewR8,
        prevSampL9,
        prevSlewL9,
        prevSampR9,
        prevSlewR9,
        prevSampL10,
        prevSlewL10,
        prevSampR10,
        prevSlewR10,
        prevSampL11,
        prevSlewL11,
        prevSampR11,
        prevSlewR11,
        prevSampL12,
        prevSlewL12,
        prevSampR12,
        prevSlewR12,
        prevSampL13,
        prevSlewL13,
        prevSampR13,
        prevSlewR13,
        prevSampL14,
        prevSlewL14,
        prevSampR14,
        prevSlewR14,
        prevSampL15,
        prevSlewL15,
        prevSampR15,
        prevSlewR15,
        prevSampL16,
        prevSlewL16,
        prevSampR16,
        prevSlewR16,
        pear_total

        public :
            Pear2(){
                A = 1.0;
                B = 0.5;
                C = 0.5;
                D = 1.0;
                for (int x = 0; x < pear_total; x++) pear[x] = 0.0;
                freqA = freqB = 0.5;
                nonLinA = nonLinB = 0.0;
                wetA = wetB = 0.5;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
            return 1.0;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 1.0;
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
            return "freq";
            break;
        kParamB:
            return "nonlin";
            break;
        kParamC:
            return "poles";
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
            return "Freq";
            break;
        kParamB:
            return "NonLin";
            break;
        kParamC:
            return "Poles";
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        freqA = freqB;
        freqB = pow(A, 2);
        if (freqB < 0.0001) {
            freqB = 0.0001;
        }
        nonLinA = nonLinB;
        nonLinB = pow(B, 2);
        double maxPoles = pow(C, 3) * pear_total;
        wetA = wetB;
        wetB = D; // inv-dry-wet for highpass
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double freq = (freqA * temp) + (freqB * (1.0 - temp));
            double nonLin = (nonLinA * temp) + (nonLinB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            double dry = 2.0 - (wet * 2.0);
            if (dry > 1.0) {
                dry = 1.0; // full dry for use with inv, to 0.0 at full wet
            }
            wet = (wet * 2.0) - 1.0; // inv-dry-wet for highpass
            for (int x = 0; x < maxPoles; x += 4) {
                double di = fabs(freq * (1.0 + (inputSampleL * nonLin)));
                if (di > 1.0) {
                    di = 1.0;
                }
                double slew = ((inputSampleL - pear[x]) + pear[x + 1]) * di * 0.5;
                pear[x] = inputSampleL = (di * inputSampleL) + ((1.0 - di) * (pear[x] + pear[x + 1]));
                pear[x + 1] = slew;
                di = fabs(freq * (1.0 + (inputSampleR * nonLin)));
                if (di > 1.0) {
                    di = 1.0;
                }
                slew = ((inputSampleR - pear[x + 2]) + pear[x + 3]) * di * 0.5;
                pear[x + 2] = inputSampleR = (di * inputSampleR) + ((1.0 - di) * (pear[x + 2] + pear[x + 3]));
                pear[x + 3] = slew;
            }
            inputSampleL *= wet;
            inputSampleR *= wet;
            drySampleL *= dry;
            drySampleR *= dry;
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
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
} // namespace airwindohhs::pear2
