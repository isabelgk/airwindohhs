#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::acceleration2 {

constexpr std::string_view k_name{ "Acceleration2" };
constexpr std::string_view k_short_description{
    "Acceleration2 is Acceleration, better! Updated for high sample rate and more extreme effect."
};
constexpr std::string_view k_long_description{
    "Acceleration2 is an updated version of my high-frequency limiter, Acceleration. It’s like a brightness control that doesn’t really take away apparent brightness, just glues it really hard. Acceleration limiting is what you’d use mastering to vinyl, to avoid burning out the cutting head: it will get you a nice retro tone without obviously coloring things. It is not an ’emulation’ of any specific gear: real mastering engineers are not looking for ’emulation of Neumann sound’ or anything like that, they’re looking to get the functionality of this in the most colorless way.It’s been updated to work exactly the same at all sample rates (the previous one had issues adapting to them) and now has more intense depth of effect: if you crank it up you can make things real soft and dull. Don’t do that :) the purpose is not to do what you could do with an EQ, the purpose is to glue things and take the edge off the super-highs without harming the sparkle and air of the recording. Acceleration2 can do that real well: you might even find it useful if you’re doing very digital mixes and avoiding my other stuff like Console7. If you put this on the mix buss of an otherwise super-digital mix, you can make the highs prettier without making them any darker or duller. Do that by applying only small amounts of the Limit control."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Acceleration2 final : public Effect<T>
{
    double lastSampleL;
    double sL[34];
    double m1L;
    double m2L;
    double lastSampleR;
    double sR[34];
    double m1R;
    double m2R;
    double biquadA[11];
    double biquadB[11]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8 and 9-10, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Acceleration2()
    {
        A = 0.32;
        B = 1.0;
        lastSampleL = lastSampleR = 0.0;
        for (int count = 0; count < 33; count++) {
            sL[count] = sR[count] = 0.0;
        }
        m1L = m2L = m1R = m2R = 0.0;
        for (int x = 0; x < 11; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
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
        kParamB = 1,
        kNumParameters = 2
    };

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
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.32; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "limit"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Limit"; break;
            case kParamB: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double intensity = pow(A, 3) * 32;
        double wet = B;
        int spacing = (int)(1.73 * overallscale) + 1;
        if (spacing > 16) {
            spacing = 16;
        }
        biquadA[0] = (20000.0 * (1.0 - (A * 0.618033988749894848204586))) / Effect<T>::getSampleRate();
        biquadB[0] = 20000.0 / Effect<T>::getSampleRate();
        biquadA[1] = 0.7071;
        biquadB[1] = 0.7071;
        double K = tan(M_PI * biquadA[0]);
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
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
            double tempSample = (inputSampleL * biquadA[2]) + biquadA[7];
            biquadA[7] = (inputSampleL * biquadA[3]) - (tempSample * biquadA[5]) + biquadA[8];
            biquadA[8] = (inputSampleL * biquadA[4]) - (tempSample * biquadA[6]);
            double smoothL = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (inputSampleR * biquadA[2]) + biquadA[9];
            biquadA[9] = (inputSampleR * biquadA[3]) - (tempSample * biquadA[5]) + biquadA[10];
            biquadA[10] = (inputSampleR * biquadA[4]) - (tempSample * biquadA[6]);
            double smoothR = tempSample; // note: 9 and 10 store the R channel
            for (int count = spacing * 2; count >= 0; count--) {
                sL[count + 1] = sL[count];
                sR[count + 1] = sR[count];
            }
            sL[0] = inputSampleL;
            sR[0] = inputSampleR;
            m1L = (sL[0] - sL[spacing]) * (fabs(sL[0] - sL[spacing]));
            m2L = (sL[spacing] - sL[spacing * 2]) * (fabs(sL[spacing] - sL[spacing * 2]));
            double senseL = (intensity * intensity * fabs(m1L - m2L));
            if (senseL > 1.0) {
                senseL = 1.0;
            }
            inputSampleL = (inputSampleL * (1.0 - senseL)) + (smoothL * senseL);
            m1R = (sR[0] - sR[spacing]) * (fabs(sR[0] - sR[spacing]));
            m2R = (sR[spacing] - sR[spacing * 2]) * (fabs(sR[spacing] - sR[spacing * 2]));
            double senseR = (intensity * intensity * fabs(m1R - m2R));
            if (senseR > 1.0) {
                senseR = 1.0;
            }
            inputSampleR = (inputSampleR * (1.0 - senseR)) + (smoothR * senseR);
            tempSample = (inputSampleL * biquadB[2]) + biquadB[7];
            biquadB[7] = (inputSampleL * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
            biquadB[8] = (inputSampleL * biquadB[4]) - (tempSample * biquadB[6]);
            inputSampleL = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (inputSampleR * biquadB[2]) + biquadB[9];
            biquadB[9] = (inputSampleR * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[10];
            biquadB[10] = (inputSampleR * biquadB[4]) - (tempSample * biquadB[6]);
            inputSampleR = tempSample; // note: 9 and 10 store the R channel
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
} // namespace airwindohhs::acceleration2
