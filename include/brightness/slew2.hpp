#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::slew2 {

constexpr std::string_view k_name{ "Slew2" };
constexpr std::string_view k_short_description{
    "Slew2 works like a de-esser or acceleration limiter: controls extreme highs."
};
constexpr std::string_view k_long_description{
    "Sometimes I just get lucky :)This plugin is just the anti-aliasing technology I was experimenting with in 2010, applied to Slew, my simple slew clipper. I thought it was going to make the slightly grungy Slew smoother (Slew is a clipper, and can be expected to have clipper-like qualities). That anti-aliasing code was a bit odd. It’s possible I made a mistake somewhere.If so, I’m keeping it, and now what I invented is out for VST also, and free to all.Slew2 doesn’t act anything like Slew. What it does, is it puts a particular behavior on the extreme highs. Set to an intermediate value, it’s an acceleration limiter. Cranked all the way up… well, check out the video. It behaves like one of the nodes in Average (also coming to VST) with a 100% cancellation, but the point of total cancellation is also the Nyquist frequency. Slew2 produces a very natural-sounding brickwall filter exactly at that frequency, with the response falling off faster and faster until it’s totally gone when you hit the Nyquist limit (digital sampling theory: at 44.1K (CD quality) it is 22.05K where the treble cuts off).People are using this for de-essers, and it’s pretty much ideal for any sound that must not be overwhelming in the super highs. The peculiar character of the rolloff means it’s suitable for almost anything because it won’t affect lower treble, and the fact that it’s really a clipper means you can use the slider to set a threshold where, the harder you push Slew2, the more it’ll refuse to let more brightness through. I’ve not tried using it as an acceleration limiter on a mastering lathe, but it could probably do that: which also means if you’re going for classic analog tone, you can just toss this on the 2-buss and use just enough to take excess brightness away. Slew2 is one of the ‘secret weapon’ plugins I was specifically asked to port to VST, and I’m happy to bring it to you all."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Slew2 final : public Effect<T>
{
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
            case kParamA: return "clamping"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Clamping"; break;

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

        double overallscale = 2.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
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
} // namespace airwindohhs::slew2
