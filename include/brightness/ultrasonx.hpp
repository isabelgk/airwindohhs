#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ultrasonx {

constexpr std::string_view k_name{ "UltrasonX" };
constexpr std::string_view k_short_description{
    "UltrasonX is a method for rolling your own Console-type systems with total control over your ultrasonic filtering."
};
constexpr std::string_view k_long_description{
    "A little while back, I made a simpler ‘Ultrasonic’ filter, with the intention of making a lighter-weight utility that could be used where Ultrasonic might go. The idea was, Ultrasonic (which is already available) was too heavy: it uses five stages of biquad filtering and I thought doing the same thing with just one or two stages would be better.Turns out you can do that, but if you stack them up you start losing the super-highs. The one or two-stage versions weren’t steep enough: didn’t bring out the super highs close enough to the ultrasonic zone. But, the five stage version is still just too much processing to put all over the mix. What do?UltrasonX solves this problem (that maybe nobody but me had :) ).This is a plugin that does any one of the five stages in Ultrasonic, one at a time. It’s got settings for A, B, C, D and E stages. Each of these are a carefully calibrated resonance value, that add up to nice and flat and clear all the way up to the supersonic region.Console7Cascade does something like this, internally, and gets a particular tone because its ‘more resonant A stage filter) is before distortion, and the softer unresonant filters that compensate for this are after the distortions.But here’s the thing: if you want to make a Console topology where the channels aren’t overly brightening, but you’re feeding a submix buss that is more crunchy and shouldn’t be hit too hard, you can do that too. This is likely going to end up my problem but now you can experiment!To do that, construct a Console system, using an earlier Console or PurestConsole (the original) and see to it that there are exactly five instances of UltrasonX in each signal path. Maybe two on each channel, two on the submix, one on the 2-buss! Or, one on each channel before hitting the PurestConsoleChannel, one on the 2-buss before the PurestConsoleBuss, and then three more sprinkled between your 2-buss processing. And any of these can be the ‘pre-brightening’ or ‘complementary darkening’ ones, meaning you could have the brightness boost after summing for a bit of air in between your instruments, or early on for softening saturation and adding glue.Or, you could use it anywhere, or on one single channel (that didn’t have ultrasonic filtering) as long as you’ve got room to fit five individual filter plugins. Bottom line is, if your audio goes through ABCDE and comes out the other end, it should be correct. The rest is up to you :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class UltrasonX final : public Effect<T>
{
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total

        public :
            UltrasonX(){
                A = 0.5;
                for (int x = 0; x < fix_total; x++){ fixA[x] = 0.0; }
    fixA[fix_reso] = 0.7071; // butterworth Q
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

enum params {
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
        return 0.5;
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
        return "q";
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
        return "Q";
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

    fixA[fix_freq] = 21000.0 / getSampleRate();
    switch ((VstInt32)(A * 4.999))
    {
        case kA:
            fixA[fix_reso] = 3.19622661;
            break;
        case kB:
            fixA[fix_reso] = 1.10134463;
            break;
        case kC:
            fixA[fix_reso] = 0.70710678; // butterworth Q
            break;
        case kD:
            fixA[fix_reso] = 0.56116312;
            break;
        case kE:
            fixA[fix_reso] = 0.50623256;
            break;
    }
    double K = tan(M_PI * fixA[fix_freq]);
    double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
    fixA[fix_a0] = K * K * norm;
    fixA[fix_a1] = 2.0 * fixA[fix_a0];
    fixA[fix_a2] = fixA[fix_a0];
    fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
    fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
    // for the fixed-position biquad filter
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
        double temp = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
        fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sL2];
        fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (temp * fixA[fix_b2]);
        inputSampleL = temp; // fixed biquad filtering ultrasonics
        temp = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
        fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sR2];
        fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (temp * fixA[fix_b2]);
        inputSampleR = temp; // fixed biquad filtering ultrasonics
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
} // namespace airwindohhs::ultrasonx
