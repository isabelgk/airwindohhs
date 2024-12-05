#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dustbunny {

constexpr std::string_view k_name{ "DustBunny" };
constexpr std::string_view k_short_description{
    "DustBunny is unpredictable distorty noise, like dust wedged under your turntable needle."
};
constexpr std::string_view k_long_description{
    "And then sometimes there’s a plugin that just makes you go ‘wut’…DustBunny was an accident. I was doing something and put out a plugin (possibly a freebie) and there was a bug and I was in a hurry and put it out without checking… and quickly learned something horrible was wrong. Initial reports were along the lines of ‘oops’ and ‘yikes’, and when I checked, sure enough, the plugin erupted in terrible scrunch, and I in turn erupted in apologies and scrambled to fix the problem (which wasn’t that hard).But even before I’d got the fix out (and simple oversight problems, I sometimes fix within hours), more reports were coming in: hold on, don’t fix it, it’s cool!That wasn’t what I had in mind, but the solution was obvious: DustBunny was born. This twisted little plugin just applies a weird accidental scrunch based on where the bunny control is set. Please don’t use it on the 2-buss, or in mastering :) but more seriously, this is born to sit inside some kind of weird plugin matrix device as part of a nefarious sound design idea. It would’ve been perfect in one of the parallel effect chains used on the latest DOOM soundtrack, mangling a sinewave. If you hit it with high levels it gets kind of jumpy, so you might want to give it more restrained levels. If you don’t like running a gain trim in front of it, run something more amusing like a delay or a flanger to pad your signal a touch.DustBunny is kind of like a joke, except for it’s real and does produce an unusual, distinctive effect (or 1000, as all the settings are kind of unique)."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DustBunny final : public Effect<T>
{
    float LataLast3Sample;
    float LataLast2Sample;
    float LataLast1Sample;
    float LataHalfwaySample;
    float LataHalfDrySample;
    float LataHalfDiffSample;
    float LataA;
    float LataB;
    float LataC;
    float LataDecay;
    float LataUpsampleHighTweak;
    float LataDrySample;
    float LataDiffSample;
    float LataPrevDiffSample;
    float RataLast3Sample;
    float RataLast2Sample;
    float RataLast1Sample;
    float RataHalfwaySample;
    float RataHalfDrySample;
    float RataHalfDiffSample;
    float RataA;
    float RataB;
    float RataC;
    float RataDecay;
    float RataUpsampleHighTweak;
    float RataDrySample;
    float RataDiffSample;
    float RataPrevDiffSample;
    bool LataFlip; // end defining of antialiasing variables
    bool RataFlip; // end defining of antialiasing variables
    uint32_t fpdL;
    uint32_t fpdR;
    float A;

  public:
    DustBunny()
    {
        A = 0.0;
        LataLast3Sample = LataLast2Sample = LataLast1Sample = 0.0;
        LataHalfwaySample = LataHalfDrySample = LataHalfDiffSample = 0.0;
        LataA = LataB = LataC = LataDrySample = LataDiffSample = LataPrevDiffSample = 0.0;
        LataUpsampleHighTweak = 0.0414213562373095048801688; // more adds treble to upsampling
        LataDecay = 0.915965594177219015; // Catalan's constant, more adds focus and clarity
        RataLast3Sample = RataLast2Sample = RataLast1Sample = 0.0;
        RataHalfwaySample = RataHalfDrySample = RataHalfDiffSample = 0.0;
        RataA = RataB = RataC = RataDrySample = RataDiffSample = RataPrevDiffSample = 0.0;
        RataUpsampleHighTweak = 0.0414213562373095048801688; // more adds treble to upsampling
        RataDecay = 0.915965594177219015; // CRatalan's constant, more adds focus and clarity
        LataFlip = false; // end reset of antialias parameters
        RataFlip = false; // end reset of antialias parameters
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
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bunny"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Bunny"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;

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

        unsigned int bunny = (unsigned int)(pow((1.255 - A), 5) * 1000);
        bunny = (bunny * bunny);
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            // note: this algorithm does goofy stuff with bit masks, so the 64-bit buss will use floats for processing to produce the same output.
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            LataDrySample = inputSampleL;
            RataDrySample = inputSampleR;
            LataHalfDrySample = LataHalfwaySample = (inputSampleL + LataLast1Sample + ((-LataLast2Sample + LataLast3Sample) * LataUpsampleHighTweak)) / 2.0;
            LataLast3Sample = LataLast2Sample;
            LataLast2Sample = LataLast1Sample;
            LataLast1Sample = inputSampleL;
            // setting up oversampled special antialiasing
            // begin first half- change inputSample -> LataHalfwaySample, LataDrySample -> LataHalfDrySample
            if (((*(unsigned int*)&LataHalfwaySample) & bunny) == 0) {
                LataHalfwaySample = 0.0;
            }
            // end first half
            // begin antialiasing section for halfway sample
            LataC = LataHalfwaySample - LataHalfDrySample;
            if (LataFlip) {
                LataA *= LataDecay;
                LataB *= LataDecay;
                LataA += LataC;
                LataB -= LataC;
                LataC = LataA;
            }
            else {
                LataB *= LataDecay;
                LataA *= LataDecay;
                LataB += LataC;
                LataA -= LataC;
                LataC = LataB;
            }
            LataHalfDiffSample = (LataC * LataDecay);
            LataFlip = !LataFlip;
            // end antialiasing section for halfway sample
            // begin second half- inputSample and LataDrySample handled separately here
            if (((*(unsigned int*)&inputSampleL) & bunny) == 0) {
                inputSampleL = 0.0;
            }
            // end second half
            // begin antialiasing section for input sample
            LataC = inputSampleL - LataDrySample;
            if (LataFlip) {
                LataA *= LataDecay;
                LataB *= LataDecay;
                LataA += LataC;
                LataB -= LataC;
                LataC = LataA;
            }
            else {
                LataB *= LataDecay;
                LataA *= LataDecay;
                LataB += LataC;
                LataA -= LataC;
                LataC = LataB;
            }
            LataDiffSample = (LataC * LataDecay);
            LataFlip = !LataFlip;
            // end antialiasing section for input sample
            inputSampleL = LataDrySample;
            inputSampleL += ((LataDiffSample + LataHalfDiffSample + LataPrevDiffSample) / 0.734);
            LataPrevDiffSample = LataDiffSample / 2.0;
            // apply processing as difference to non-oversampled raw input
            RataHalfDrySample = RataHalfwaySample = (inputSampleR + RataLast1Sample + ((-RataLast2Sample + RataLast3Sample) * RataUpsampleHighTweak)) / 2.0;
            RataLast3Sample = RataLast2Sample;
            RataLast2Sample = RataLast1Sample;
            RataLast1Sample = inputSampleR;
            // setting up oversampled special antialiasing
            // begin first half- change inputSample -> RataHalfwaySample, RataDrySample -> RataHalfDrySample
            if (((*(unsigned int*)&RataHalfwaySample) & bunny) == 0) {
                RataHalfwaySample = 0.0;
            }
            // end first half
            // begin antialiasing section for halfway sample
            RataC = RataHalfwaySample - RataHalfDrySample;
            if (RataFlip) {
                RataA *= RataDecay;
                RataB *= RataDecay;
                RataA += RataC;
                RataB -= RataC;
                RataC = RataA;
            }
            else {
                RataB *= RataDecay;
                RataA *= RataDecay;
                RataB += RataC;
                RataA -= RataC;
                RataC = RataB;
            }
            RataHalfDiffSample = (RataC * RataDecay);
            RataFlip = !RataFlip;
            // end antialiasing section for halfway sample
            // begin second half- inputSample and RataDrySample handled separately here
            if (((*(unsigned int*)&inputSampleR) & bunny) == 0) {
                inputSampleR = 0.0;
            }
            // end second half
            // begin antialiasing section for input sample
            RataC = inputSampleR - RataDrySample;
            if (RataFlip) {
                RataA *= RataDecay;
                RataB *= RataDecay;
                RataA += RataC;
                RataB -= RataC;
                RataC = RataA;
            }
            else {
                RataB *= RataDecay;
                RataA *= RataDecay;
                RataB += RataC;
                RataA -= RataC;
                RataC = RataB;
            }
            RataDiffSample = (RataC * RataDecay);
            RataFlip = !RataFlip;
            // end antialiasing section for input sample
            inputSampleR = RataDrySample;
            inputSampleR += ((RataDiffSample + RataHalfDiffSample + RataPrevDiffSample) / 0.734);
            RataPrevDiffSample = RataDiffSample / 2.0;
            // apply processing as difference to non-oversampled raw input
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::dustbunny
