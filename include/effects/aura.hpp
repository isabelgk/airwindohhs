#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::aura {

constexpr std::string_view k_name{ "Aura" };
constexpr std::string_view k_short_description{
    "Aura is a new kind of resonant lowpass EQ."
};
constexpr std::string_view k_long_description{
    "So this turned out to be a bear to bring up to date, because the original code was insane. I’m not quite sure how I got there, and I was still bugfixing long after the demo video was made. You’ll find the actual version of Aura has a slightly wider range, better adjustability in the low range, and the Dry/Wet control gets a touch of added functionality: as you go full wet, the resonant quality gets enhanced, so be sure and explore the half-wet or barely-wet settings. If it’s too scary-resonant, just pull it back a teeny bit and it should cooperate.And this one is a bit scary as it seems to be channeling analog filters. I agree that it would be great to have this principle work as a full-range, synth-style filter that goes all the way into the bass. I can’t do it, though: it freaks out when I try, and it took endless hacking just to expand it a bit from what you see in the video. This is the algorithm derived from GrooveWear, which averages the rate of change OF the rate of change of the waveform. It’s not even slightly normal. You get what you get.But what you get, is a lowpass with a striking resonant edge that’s implemented in a totally new way, and which has no pre-ring at all… and the way it gets its sound gives it an extraordinary sonority. Pretty much anything in audio that you’d want to project loudly as if from an acoustic space, can be given a sheen and glisten and sonority with Aura. I’ve got it extending down fairly low into the midrange if you’re at 44.1K or so: that should help if you need to use it at high sample rates, because the technique for doing it is not exactly cooperative and I found no way to simply tune it down: everything’s so geared to slew rate between samples that it’s best used for treble effects. I think it’s got a useful tonality for its treble manipulations, and I’ve spent a lot of time coming up with interesting ways to cut or enhance treble. This one’s good at what it does. You can really do stuff with the texture of your mix by aggressively using Aura on suitable elements."
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Aura final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double previousVelocityL;
    double lastSampleR;
    double previousVelocityR;
    double bL[21];
    double bR[21];
    double f[21];
    float A;
    float B;

  public:
    Aura()
    {
        A = 0.5;
        B = 1.0;
        for (int count = 0; count < 21; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
        }
        lastSampleL = 0.0;
        previousVelocityL = 0.0;
        lastSampleR = 0.0;
        previousVelocityR = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "voicing"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Voicing"; break;
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

        double correctionL;
        double correctionR;
        double accumulatorSampleL;
        double accumulatorSampleR;
        double velocityL;
        double velocityR;
        double trim = A;
        double wet = B;
        // removed extra dry variable
        double overallscale = trim * 10.0;
        double gain = overallscale + (pow(wet, 3) * 0.187859642462067);
        trim *= (1.0 - (pow(wet, 3) * 0.187859642462067));
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        if (gain < 1.0) {
            gain = 1.0;
        }
        if (gain > 1.0) {
            f[0] = 1.0;
            gain -= 1.0;
        }
        else {
            f[0] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[1] = 1.0;
            gain -= 1.0;
        }
        else {
            f[1] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[2] = 1.0;
            gain -= 1.0;
        }
        else {
            f[2] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[3] = 1.0;
            gain -= 1.0;
        }
        else {
            f[3] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[4] = 1.0;
            gain -= 1.0;
        }
        else {
            f[4] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[5] = 1.0;
            gain -= 1.0;
        }
        else {
            f[5] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[6] = 1.0;
            gain -= 1.0;
        }
        else {
            f[6] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[7] = 1.0;
            gain -= 1.0;
        }
        else {
            f[7] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[8] = 1.0;
            gain -= 1.0;
        }
        else {
            f[8] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[9] = 1.0;
            gain -= 1.0;
        }
        else {
            f[9] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[10] = 1.0;
            gain -= 1.0;
        }
        else {
            f[10] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[11] = 1.0;
            gain -= 1.0;
        }
        else {
            f[11] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[12] = 1.0;
            gain -= 1.0;
        }
        else {
            f[12] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[13] = 1.0;
            gain -= 1.0;
        }
        else {
            f[13] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[14] = 1.0;
            gain -= 1.0;
        }
        else {
            f[14] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[15] = 1.0;
            gain -= 1.0;
        }
        else {
            f[15] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[16] = 1.0;
            gain -= 1.0;
        }
        else {
            f[16] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[17] = 1.0;
            gain -= 1.0;
        }
        else {
            f[17] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[18] = 1.0;
            gain -= 1.0;
        }
        else {
            f[18] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[19] = 1.0;
            gain -= 1.0;
        }
        else {
            f[19] = gain;
            gain = 0.0;
        }
        // there, now we have a neat little moving average with remainders
        if (overallscale < 1.0) {
            overallscale = 1.0;
        }
        f[0] /= overallscale;
        f[1] /= overallscale;
        f[2] /= overallscale;
        f[3] /= overallscale;
        f[4] /= overallscale;
        f[5] /= overallscale;
        f[6] /= overallscale;
        f[7] /= overallscale;
        f[8] /= overallscale;
        f[9] /= overallscale;
        f[10] /= overallscale;
        f[11] /= overallscale;
        f[12] /= overallscale;
        f[13] /= overallscale;
        f[14] /= overallscale;
        f[15] /= overallscale;
        f[16] /= overallscale;
        f[17] /= overallscale;
        f[18] /= overallscale;
        f[19] /= overallscale;
        // and now it's neatly scaled, too
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
            velocityL = lastSampleL - inputSampleL;
            correctionL = previousVelocityL - velocityL;
            bL[19] = bL[18];
            bL[18] = bL[17];
            bL[17] = bL[16];
            bL[16] = bL[15];
            bL[15] = bL[14];
            bL[14] = bL[13];
            bL[13] = bL[12];
            bL[12] = bL[11];
            bL[11] = bL[10];
            bL[10] = bL[9];
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = accumulatorSampleL = correctionL;
            // we are accumulating rates of change of the rate of change
            accumulatorSampleL *= f[0];
            accumulatorSampleL += (bL[1] * f[1]);
            accumulatorSampleL += (bL[2] * f[2]);
            accumulatorSampleL += (bL[3] * f[3]);
            accumulatorSampleL += (bL[4] * f[4]);
            accumulatorSampleL += (bL[5] * f[5]);
            accumulatorSampleL += (bL[6] * f[6]);
            accumulatorSampleL += (bL[7] * f[7]);
            accumulatorSampleL += (bL[8] * f[8]);
            accumulatorSampleL += (bL[9] * f[9]);
            accumulatorSampleL += (bL[10] * f[10]);
            accumulatorSampleL += (bL[11] * f[11]);
            accumulatorSampleL += (bL[12] * f[12]);
            accumulatorSampleL += (bL[13] * f[13]);
            accumulatorSampleL += (bL[14] * f[14]);
            accumulatorSampleL += (bL[15] * f[15]);
            accumulatorSampleL += (bL[16] * f[16]);
            accumulatorSampleL += (bL[17] * f[17]);
            accumulatorSampleL += (bL[18] * f[18]);
            accumulatorSampleL += (bL[19] * f[19]);
            velocityL = previousVelocityL + accumulatorSampleL;
            inputSampleL = lastSampleL + velocityL;
            lastSampleL = inputSampleL;
            previousVelocityL = -velocityL * pow(trim, 2);
            // left channel done
            velocityR = lastSampleR - inputSampleR;
            correctionR = previousVelocityR - velocityR;
            bR[19] = bR[18];
            bR[18] = bR[17];
            bR[17] = bR[16];
            bR[16] = bR[15];
            bR[15] = bR[14];
            bR[14] = bR[13];
            bR[13] = bR[12];
            bR[12] = bR[11];
            bR[11] = bR[10];
            bR[10] = bR[9];
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = accumulatorSampleR = correctionR;
            // we are accumulating rates of change of the rate of change
            accumulatorSampleR *= f[0];
            accumulatorSampleR += (bR[1] * f[1]);
            accumulatorSampleR += (bR[2] * f[2]);
            accumulatorSampleR += (bR[3] * f[3]);
            accumulatorSampleR += (bR[4] * f[4]);
            accumulatorSampleR += (bR[5] * f[5]);
            accumulatorSampleR += (bR[6] * f[6]);
            accumulatorSampleR += (bR[7] * f[7]);
            accumulatorSampleR += (bR[8] * f[8]);
            accumulatorSampleR += (bR[9] * f[9]);
            accumulatorSampleR += (bR[10] * f[10]);
            accumulatorSampleR += (bR[11] * f[11]);
            accumulatorSampleR += (bR[12] * f[12]);
            accumulatorSampleR += (bR[13] * f[13]);
            accumulatorSampleR += (bR[14] * f[14]);
            accumulatorSampleR += (bR[15] * f[15]);
            accumulatorSampleR += (bR[16] * f[16]);
            accumulatorSampleR += (bR[17] * f[17]);
            accumulatorSampleR += (bR[18] * f[18]);
            accumulatorSampleR += (bR[19] * f[19]);
            // we are doing our repetitive calculations on a separate value
            velocityR = previousVelocityR + accumulatorSampleR;
            inputSampleR = lastSampleR + velocityR;
            lastSampleR = inputSampleR;
            previousVelocityR = -velocityR * pow(trim, 2);
            // right channel done
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
} // namespace airwindohhs::aura
