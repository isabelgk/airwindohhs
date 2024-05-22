#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Slew2 final : public Effect<T>
{
    std::string m_name{ "Slew2" };

    double LataLast3Sample;
    double LataLast2Sample;
    double LataLast1Sample;
    double LataHalfwaySample;
    double LataHalfDrySample;
    double LataHalfDiffSample;
    double LataA;
    double LataB;
    double LataC;
    double LataDecay;
    double LataUpsampleHighTweak;
    double LataDrySample;
    double LataDiffSample;
    double LataPrevDiffSample;
    double RataLast3Sample;
    double RataLast2Sample;
    double RataLast1Sample;
    double RataHalfwaySample;
    double RataHalfDrySample;
    double RataHalfDiffSample;
    double RataA;
    double RataB;
    double RataC;
    double RataDecay;
    double RataUpsampleHighTweak;
    double RataDrySample;
    double RataDiffSample;
    double RataPrevDiffSample;
    bool LataFlip; // end defining of antialiasing variables
    bool RataFlip; // end defining of antialiasing variables
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    Slew2()
    {
        A = 0.0;
        LataLast3Sample = LataLast2Sample = LataLast1Sample = 0.0;
        LataHalfwaySample = LataHalfDrySample = LataHalfDiffSample = 0.0;
        LataA = LataB = LataC = LataDrySample = LataDiffSample = LataPrevDiffSample = 0.0;
        LataUpsampleHighTweak = 0.0414213562373095048801688; // more adds treble to upsampling
        LataDecay = 0.915965594177219015; // Catalan's constant, more adds focus and clarity
        lastSampleL = 0.0;
        RataLast3Sample = RataLast2Sample = RataLast1Sample = 0.0;
        RataHalfwaySample = RataHalfDrySample = RataHalfDiffSample = 0.0;
        RataA = RataB = RataC = RataDrySample = RataDiffSample = RataPrevDiffSample = 0.0;
        RataUpsampleHighTweak = 0.0414213562373095048801688; // more adds treble to upsampling
        RataDecay = 0.915965594177219015; // CRatalan's constant, more adds focus and clarity
        LataFlip = false; // end reset of antialias parameters
        RataFlip = false; // end reset of antialias parameters
        lastSampleR = 0.0;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    constexpr std::string_view name()
    {
        return m_name;
    }

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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Clamping";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 2.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double clamp;
        double threshold = pow((1 - A), 4) / overallscale;
        double inputSampleL;
        double inputSampleR;
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
            LataDrySample = inputSampleL;
            RataDrySample = inputSampleR;
            LataHalfDrySample = LataHalfwaySample = (inputSampleL + LataLast1Sample + ((-LataLast2Sample + LataLast3Sample) * LataUpsampleHighTweak)) / 2.0;
            LataLast3Sample = LataLast2Sample;
            LataLast2Sample = LataLast1Sample;
            LataLast1Sample = inputSampleL;
            // setting up oversampled special antialiasing
            // begin first half- change inputSample -> LataHalfwaySample, LataDrySample -> LataHalfDrySample
            clamp = LataHalfwaySample - LataHalfDrySample;
            if (clamp > threshold) {
                LataHalfwaySample = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                LataHalfwaySample = lastSampleL - threshold;
            }
            lastSampleL = LataHalfwaySample;
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
            clamp = inputSampleL - lastSampleL;
            if (clamp > threshold) {
                inputSampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleL - threshold;
            }
            lastSampleL = inputSampleL;
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
            clamp = RataHalfwaySample - RataHalfDrySample;
            if (clamp > threshold) {
                RataHalfwaySample = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                RataHalfwaySample = lastSampleR - threshold;
            }
            lastSampleR = RataHalfwaySample;
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
            clamp = inputSampleR - lastSampleR;
            if (clamp > threshold) {
                inputSampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleR - threshold;
            }
            lastSampleR = inputSampleR;
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
