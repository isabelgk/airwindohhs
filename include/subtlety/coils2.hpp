#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::coils2 {

constexpr std::string_view k_name{ "Coils2" };
constexpr std::string_view k_short_description{
    "Coils2 is a transformer overdrive emulator."
};
constexpr std::string_view k_long_description{
    "This is to Coils what Srsly2 was to Srsly. In other words, Coils was always ultra subtle. Coils2 lets you dial up the cheapness until the tone is wrecked (if you so choose).There’s two controls, Saturation and Cheapness. Saturation determines what happens outside the ‘resonant band’, and Cheapness narrows the band in which the transformer’s putting out clean, pure sound. This is not a ‘stompbox distortion’ type of distortion. It’s shaping the way the transformer produces sonority. If you narrow the ‘sonority’ band with more Cheapness, you increasingly distort and lose the lows and highs, and also get a hysteresis effect of magnetizing the transformer core. More Cheapness lets you hear the transformer crap out better when you overdrive it.Saturation gives you the maximum overload you can get to, NOT ‘more total gain’. It’s clamping down, not boosting into. That means if you turn it up all the way you get a sort of bandpass: it’s no longer really a model, you’re hearing only what’s left over after the transformer dies. To hear the grind, you have to set it to less than full crank, and halfway should already be quite a lot of overload (except if Cheapness is really low, it might be hard to overload the transformer, so it’s a matter of taste.That’s a lot of talk to say: play with the knobs. They should do what they’re labeled to do, and as long as you’re not thinking ‘turn everything up all the way’ you’ll be fine. It’s meant to pass through a great deal of sonority even for tiny cheap transformers, just in such a way that you can really hear it this time. Coils2 is still in the spirit of Coils, in that you’ll get the most accurate ‘modeling’ by not treating it as its own stompbox. It’s the output stage, for shaping and sculpting things that already exist. You might combine it with tube distortion effects to get an ‘amp-like’ character, or use it subtly on a 2-buss to tighten lows and give you more impact and vibe. Remember to not use too much saturation and cheapness for full mixes :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Coils2 final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    double hysteresisL;
    double hysteresisR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Coils2()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
        }
        hysteresisL = hysteresisR = 0.0;
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
        kNumParameters = 3
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
            return "saturate";
            break;
        kParamB:
            return "cheapness";
            break;
        kParamC:
            return "dry/wet";
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
            return "Saturate";
            break;
        kParamB:
            return "Cheapness";
            break;
        kParamC:
            return "Dry/Wet";
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
        double distScaling = pow(1.0 - A, 2);
        if (distScaling < 0.0001) {
            distScaling = 0.0001;
        }
        biquadA[0] = 600.0 / Effect<T>::getSampleRate();
        biquadA[1] = 0.01 + (pow(B, 2) * 0.5);
        double iirAmount = biquadA[1] / overallscale;
        double K = tan(M_PI * biquadA[0]);
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K / biquadA[1] * norm;
        biquadA[4] = -biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        biquadB[0] = (21890.0 - (biquadA[1] * 890.0)) / Effect<T>::getSampleRate();
        biquadB[1] = 0.89;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        double wet = C;
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
            if (biquadA[0] < 0.49999) {
                double tempSample = (inputSampleL * biquadA[2]) + biquadA[7];
                biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
                biquadA[8] = (inputSampleL * biquadA[4]) - (tempSample * biquadA[6]);
                inputSampleL = tempSample;
                tempSample = (inputSampleR * biquadA[2]) + biquadA[9];
                biquadA[9] = -(tempSample * biquadA[5]) + biquadA[10];
                biquadA[10] = (inputSampleR * biquadA[4]) - (tempSample * biquadA[6]);
                inputSampleR = tempSample; // create bandpass of clean tone
            }
            double diffSampleL = (drySampleL - inputSampleL) / distScaling;
            double diffSampleR = (drySampleR - inputSampleR) / distScaling; // mids notched out
            if (biquadB[0] < 0.49999) {
                double tempSample = (diffSampleL * biquadB[2]) + biquadB[7];
                biquadB[7] = (diffSampleL * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
                biquadB[8] = (diffSampleL * biquadB[4]) - (tempSample * biquadB[6]);
                diffSampleL = tempSample;
                tempSample = (diffSampleR * biquadB[2]) + biquadB[9];
                biquadB[9] = (diffSampleR * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[10];
                biquadB[10] = (diffSampleR * biquadB[4]) - (tempSample * biquadB[6]);
                diffSampleR = tempSample;
            }
            hysteresisL = (hysteresisL * (1.0 - iirAmount)) + (diffSampleL * iirAmount);
            if (fabs(hysteresisL) < 1.18e-37) {
                hysteresisL = 0.0;
            }
            else {
                diffSampleL -= hysteresisL;
            }
            if (diffSampleL > 1.571) {
                diffSampleL = 1.571;
            }
            else if (diffSampleL < -1.571) {
                diffSampleL = -1.571;
            }
            if (hysteresisL > 1.571) {
                hysteresisL = 1.571;
            }
            else if (hysteresisL < -1.571) {
                hysteresisL = -1.571;
            }
            hysteresisR = (hysteresisR * (1.0 - iirAmount)) + (diffSampleR * iirAmount);
            if (fabs(hysteresisR) < 1.18e-37) {
                hysteresisR = 0.0;
            }
            else {
                diffSampleR -= hysteresisR;
            }
            if (diffSampleR > 1.571) {
                diffSampleR = 1.571;
            }
            else if (diffSampleR < -1.571) {
                diffSampleR = -1.571;
            }
            if (hysteresisR > 1.571) {
                hysteresisR = 1.571;
            }
            else if (hysteresisR < -1.571) {
                hysteresisR = -1.571;
            }
            inputSampleL += (sin(diffSampleL) - sin(hysteresisL)) * distScaling;
            inputSampleR += (sin(diffSampleR) - sin(hysteresisR)) * distScaling; // apply transformer distortions
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
} // namespace airwindohhs::coils2
