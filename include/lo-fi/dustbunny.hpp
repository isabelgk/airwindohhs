#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class DustBunny final : public Effect<T>
{
    const std::string m_name{ "DustBunny" };

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

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Bunny";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
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
} // namespace airwindohhs
