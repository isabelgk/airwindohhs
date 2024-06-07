#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console7cascade {

constexpr std::string_view k_name{ "Console7Cascade" };
constexpr std::string_view k_short_description{
    "Console7Cascade is a drop-in replacement for Console7Channel that allows for MUCH higher gain."
};
constexpr std::string_view k_long_description{
    "I heard you liked slamming consoles. So I put five individually ultrasonic-filtered stages of slamming into your console so you can slam console while you Console7 :DThis is pretty straightforward. It’s a drop-in replacement for Console7Channel, right down to the gain staging that works with the trim control to fit the result into the mix.The difference is, this uses FIVE gain stages of the same processing in Console7Channel. And of course it’s always better (I’m learning) to filter more gently between individual stages, rather than try to super-filter all at once and then do all the distorting. And so, Console7Cascade is born: turns out to be a very very ‘consoley’ type of crunch.By that I mean it seems to barely crunch at all. It just intensifies and gets REALLY LOUD. Might work as a guitar amp sim too? It worked so well for me on my drums that I might end up just using it by default for that: just all of the channels, all get Console7Cascade. You could also put it in place on a submix… or all the submixes, if you’re kind of insane. This produces a really intense tubey loud effect with very little scratchyness or grind. I’m pretty sure it’ll be kind of brutal on the CPU as it’s not only an Ultrasonic, but also five Console7Channels, each of which run two sine functions.You might just find it was all worth it, though. Why compress when you can cascade Console7Channels?"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console7Cascade final : public Effect<T>
{
    double gainchase;
    double chasespeed;
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double biquadD[15];
    double biquadE[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console7Cascade()
    {
        A = 0.39;
        gainchase = -1.0;
        chasespeed = 64.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
            biquadE[x] = 0.0;
        }
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.39;
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
            return "cascade";
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
            return "Cascade";
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

        double inputgain = 1.0 - (pow(1.0 - A, 2));
        if (gainchase != inputgain) {
            chasespeed *= 2.0;
        }
        if (chasespeed > sampleFrames) {
            chasespeed = sampleFrames;
        }
        if (gainchase < 0.0) {
            gainchase = inputgain;
        }
        biquadE[0] = biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = 20000.0 / getSampleRate();
        biquadA[1] = 3.19622661; // tenth order Butterworth out of five biquads
        biquadB[1] = 1.10134463;
        biquadC[1] = 0.70710678;
        biquadD[1] = 0.56116312;
        biquadE[1] = 0.50623256;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        K = tan(M_PI * biquadD[0]);
        norm = 1.0 / (1.0 + K / biquadD[1] + K * K);
        biquadD[2] = K * K * norm;
        biquadD[3] = 2.0 * biquadD[2];
        biquadD[4] = biquadD[2];
        biquadD[5] = 2.0 * (K * K - 1.0) * norm;
        biquadD[6] = (1.0 - K / biquadD[1] + K * K) * norm;
        K = tan(M_PI * biquadE[0]);
        norm = 1.0 / (1.0 + K / biquadE[1] + K * K);
        biquadE[2] = K * K * norm;
        biquadE[3] = 2.0 * biquadE[2];
        biquadE[4] = biquadE[2];
        biquadE[5] = 2.0 * (K * K - 1.0) * norm;
        biquadE[6] = (1.0 - K / biquadE[1] + K * K) * norm;
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
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 64.0) {
                chasespeed = 64.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + inputgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            double cascade = gainchase + (gainchase * 0.62);
            if (1.0 != cascade) {
                inputSampleL *= cascade;
                inputSampleR *= cascade;
            }
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x
            outSampleL = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
            biquadB[8] = biquadB[7];
            biquadB[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadB[10] = biquadB[9];
            biquadB[9] = inputSampleL; // DF1 left
            outSampleR = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
            biquadB[12] = biquadB[11];
            biquadB[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadB[14] = biquadB[13];
            biquadB[13] = inputSampleR; // DF1 right
            if (1.0 != cascade) {
                inputSampleL *= cascade;
                inputSampleR *= cascade;
            }
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x
            outSampleL = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
            biquadC[8] = biquadC[7];
            biquadC[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadC[10] = biquadC[9];
            biquadC[9] = inputSampleL; // DF1 left
            outSampleR = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
            biquadC[12] = biquadC[11];
            biquadC[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadC[14] = biquadC[13];
            biquadC[13] = inputSampleR; // DF1 right
            if (1.0 != cascade) {
                inputSampleL *= cascade;
                inputSampleR *= cascade;
            }
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x
            outSampleL = biquadD[2] * inputSampleL + biquadD[3] * biquadD[7] + biquadD[4] * biquadD[8] - biquadD[5] * biquadD[9] - biquadD[6] * biquadD[10];
            biquadD[8] = biquadD[7];
            biquadD[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadD[10] = biquadD[9];
            biquadD[9] = inputSampleL; // DF1 left
            outSampleR = biquadD[2] * inputSampleR + biquadD[3] * biquadD[11] + biquadD[4] * biquadD[12] - biquadD[5] * biquadD[13] - biquadD[6] * biquadD[14];
            biquadD[12] = biquadD[11];
            biquadD[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadD[14] = biquadD[13];
            biquadD[13] = inputSampleR; // DF1 right
            if (1.0 != cascade) {
                inputSampleL *= cascade;
                inputSampleR *= cascade;
            }
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x
            outSampleL = biquadE[2] * inputSampleL + biquadE[3] * biquadE[7] + biquadE[4] * biquadE[8] - biquadE[5] * biquadE[9] - biquadE[6] * biquadE[10];
            biquadE[8] = biquadE[7];
            biquadE[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadE[10] = biquadE[9];
            biquadE[9] = inputSampleL; // DF1 left
            outSampleR = biquadE[2] * inputSampleR + biquadE[3] * biquadE[11] + biquadE[4] * biquadE[12] - biquadE[5] * biquadE[13] - biquadE[6] * biquadE[14];
            biquadE[12] = biquadE[11];
            biquadE[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadE[14] = biquadE[13];
            biquadE[13] = inputSampleR; // DF1 right
            if (1.0 != cascade) {
                inputSampleL *= cascade;
                inputSampleR *= cascade;
            }
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // Console7Channel distortion stage, with a simpler form of the gain boost: no extra accentuation, because it's repeated 5x
            if (cascade > 1.0) {
                inputSampleL /= cascade;
                inputSampleR /= cascade;
            }
            // we re-amplify after the distortion relative to how much we cut back previously.
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
} // namespace airwindohhs::console7cascade
