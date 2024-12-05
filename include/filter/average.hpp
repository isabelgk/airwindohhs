#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::average {

constexpr std::string_view k_name{ "Average" };
constexpr std::string_view k_short_description{
    "Average is a distinctive sort of lowpass filter."
};
constexpr std::string_view k_long_description{
    "This plugin is an exploration of a fascinating filter type that, I think, is really underrated. It’s a straight-up extremely simple averaging filter. If you set it to integer values, it exactly averages that number of taps on the input audio (adjacent samples). If you pick in-between values it interpolates, causing the ‘frequency’ of the filter to smoothly blend between the values.There’s a reason you don’t see this filter used for lowpasses: it’s not technically correct. In fact it whacks a great big cancellation node into the high frequencies, and the tone (while pleasing) is very obviously affected by this. It’s a bit of a flangey quality.BUT, we know better than to be limited by technical concepts, right? After all, the classic Scream Tracker resonant lowpass is known to be broken, and that has a real character to it. And even the Roland Supersaw is known to have some quirks that make it what it is…Average has a fantastic tonality once you accept the response quirks. You can dial in the notches to suppress unwanted content, and (in a post-video revision) it’s got a dry-wet control so you can moderate the effect—and, typically for Airwindows, if this dry/wet is set to full wet, the calculations for doing that drop out of the plugin entirely so you’re not doing an unnecessary multiply."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Average final : public Effect<T>
{
    double bL[11];
    double f[11];
    double bR[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Average()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "average"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Average"; break;
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
            case kParamA: return "taps"; break;
            case kParamB: return " "; break;

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

        double correctionSample;
        double accumulatorSampleL;
        double accumulatorSampleR;
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
        double overallscale = (A * 9.0) + 1.0;
        double wet = B;
        // removed extra dry variable
        double gain = overallscale;
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
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = accumulatorSampleL = inputSampleL;
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = accumulatorSampleR = inputSampleR;
            // primitive way of doing this: for larger batches of samples, you might
            // try using a circular buffer like in a reverb. If you add the new sample
            // and subtract the one on the end you can keep a running tally of the samples
            // between. Beware of tiny floating-point math errors eventually screwing up
            // your system, though!
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
            // we are doing our repetitive calculations on a separate value
            correctionSample = inputSampleL - accumulatorSampleL;
            // we're gonna apply the total effect of all these calculations as a single subtract
            inputSampleL -= correctionSample;
            correctionSample = inputSampleR - accumulatorSampleR;
            inputSampleR -= correctionSample;
            // our one math operation on the input data coming in
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // dry/wet control only applies if you're using it. We don't do a multiply by 1.0
            // if it 'won't change anything' but our sample might be at a very different scaling
            // in the floating point system.
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
} // namespace airwindohhs::average
