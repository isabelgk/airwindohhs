#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::aquickvoiceclip {

constexpr std::string_view k_name{ "AQuickVoiceClip" };
constexpr std::string_view k_short_description{
    "AQuickVoiceClip softens headset mic recordings that have been super hard clipped on capture."
};
constexpr std::string_view k_long_description{
    "I hope this is handy for somebody: I made it for myself. The idea is, if you’re a youtuber or something and have the ability to post-process your mic feed, you might have it set up so normal levels give a normal sound, but then if some monster jumps out and KILLS you in your video game, you might scream very loudly into your mic and blow the recording into ridiculously loud clipping.This does several things. Firstly, it clips and softens those moments further and tries to suppress some of the highs you got from the super-hard clipping. Secondly, it’s got a highpass which can subdue thumps and pops, and can also be used in conjunction with something like a pitch shifter to give cartoony voices (I’ve tried this with DiracFxAU, now known as zynaptiq: only used their demo AU, didn’t use their library in anything code-wise and won’t be doing so as they are unaffordable). I found highpassing before the pitch processing was very helpful for toon voices of that type.This isn’t the seriousest of plugins, but I’ve seen people ask me for it, so now it’s free and Mac/PC VST too. I think it’s best confined to its intended use, processing single voice tracks that have wildly distorted moments. If you try to use it on the 2-buss I will be very cross with you ;)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class AQuickVoiceClip final : public Effect<T>
{
    double LataLast6Sample;
    double LataLast5Sample;
    double LataLast4Sample;
    double LataLast3Sample;
    double LataLast2Sample;
    double LataLast1Sample;
    double LataHalfwaySample;
    double LataHalfDrySample;
    double LataHalfDiffSample;
    double LataLastDiffSample;
    double LataDrySample;
    double LataDiffSample;
    double LataPrevDiffSample;
    double RataLast6Sample;
    double RataLast5Sample;
    double RataLast4Sample;
    double RataLast3Sample;
    double RataLast2Sample;
    double RataLast1Sample;
    double RataHalfwaySample;
    double RataHalfDrySample;
    double RataHalfDiffSample;
    double RataLastDiffSample;
    double RataDrySample;
    double RataDiffSample;
    double RataPrevDiffSample;
    double ataK1;
    double ataK2;
    double ataK3;
    double ataK4;
    double ataK5;
    double ataK6;
    double ataK7;
    double ataK8; // end antialiasing variables
    double LlastSample;
    double LlastOutSample;
    double LlastOut2Sample;
    double LlastOut3Sample;
    double LlpDepth;
    double Lovershoot;
    double Loverall;
    double LiirSampleA;
    double LiirSampleB;
    double LiirSampleC;
    double LiirSampleD;
    double RlastSample;
    double RlastOutSample;
    double RlastOut2Sample;
    double RlastOut3Sample;
    double RlpDepth;
    double Rovershoot;
    double Roverall;
    double RiirSampleA;
    double RiirSampleB;
    double RiirSampleC;
    double RiirSampleD;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    AQuickVoiceClip()
    {
        A = 0.42;
        LataLast6Sample = LataLast5Sample = LataLast4Sample = 0.0;
        LataLast3Sample = LataLast2Sample = LataLast1Sample = 0.0;
        LataHalfwaySample = LataHalfDrySample = LataHalfDiffSample = 0.0;
        LataDrySample = LataDiffSample = LataPrevDiffSample = 0.0;
        RataLast6Sample = RataLast5Sample = RataLast4Sample = 0.0;
        RataLast3Sample = RataLast2Sample = RataLast1Sample = 0.0;
        RataHalfwaySample = RataHalfDrySample = RataHalfDiffSample = 0.0;
        RataDrySample = RataDiffSample = RataPrevDiffSample = 0.0;
        LlastSample = 0.0;
        LlastOutSample = 0.0;
        LlastOut2Sample = 0.0;
        LlastOut3Sample = 0.0;
        LlpDepth = 0.0;
        Lovershoot = 0.0;
        Loverall = 0;
        LiirSampleA = 0.0;
        LiirSampleB = 0.0;
        LiirSampleC = 0.0;
        LiirSampleD = 0.0;
        RlastSample = 0.0;
        RlastOutSample = 0.0;
        RlastOut2Sample = 0.0;
        RlastOut3Sample = 0.0;
        RlpDepth = 0.0;
        Rovershoot = 0.0;
        Roverall = 0;
        RiirSampleA = 0.0;
        RiirSampleB = 0.0;
        RiirSampleC = 0.0;
        RiirSampleD = 0.0;
        flip = true;
        ataK1 = -0.646; // first FIR shaping of interpolated sample, brightens
        ataK2 = 0.311; // second FIR shaping of interpolated sample, thickens
        ataK6 = -0.093; // third FIR shaping of interpolated sample, brings air
        ataK7 = 0.057; // fourth FIR shaping of interpolated sample, thickens
        ataK8 = -0.023; // fifth FIR shaping of interpolated sample, brings air
        ataK3 = 0.114; // add raw to interpolated dry, toughens
        ataK4 = 0.886; // remainder of interpolated dry, adds up to 1.0
        ataK5 = 0.431; // subtract this much prev. diff sample, brightens.  0.431 becomes flat
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
            case kParamA: return 0.42; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "highpass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Highpass"; break;

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
            case kParamA: return "hz"; break;

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
        double softness = 0.484416;
        double hardness = 1.0 - softness;
        double iirAmount = ((pow(A, 3) * 2070) + 30) / 8000.0;
        iirAmount /= overallscale;
        double altAmount = (1.0 - iirAmount);
        double cancelnew = 0.0682276;
        double cancelold = 1.0 - cancelnew;
        double lpSpeed = 0.0009;
        double cliplevel = 0.98;
        double refclip = 0.5; // preset to cut out gain quite a lot. 91%? no touchy unless clip
        double LmaxRecent;
        bool LclipOnset;
        double LpassThrough;
        double LoutputSample;
        double LdrySample;
        double RmaxRecent;
        bool RclipOnset;
        double RpassThrough;
        double RoutputSample;
        double RdrySample;
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
            LpassThrough = LataDrySample = inputSampleL;
            RpassThrough = RataDrySample = inputSampleR;
            LataHalfDrySample = LataHalfwaySample = (inputSampleL + LataLast1Sample + (LataLast2Sample * ataK1) + (LataLast3Sample * ataK2) + (LataLast4Sample * ataK6) + (LataLast5Sample * ataK7) + (LataLast6Sample * ataK8)) / 2.0;
            LataLast6Sample = LataLast5Sample;
            LataLast5Sample = LataLast4Sample;
            LataLast4Sample = LataLast3Sample;
            LataLast3Sample = LataLast2Sample;
            LataLast2Sample = LataLast1Sample;
            LataLast1Sample = inputSampleL;
            // setting up oversampled special antialiasing
            RataHalfDrySample = RataHalfwaySample = (inputSampleR + RataLast1Sample + (RataLast2Sample * ataK1) + (RataLast3Sample * ataK2) + (RataLast4Sample * ataK6) + (RataLast5Sample * ataK7) + (RataLast6Sample * ataK8)) / 2.0;
            RataLast6Sample = RataLast5Sample;
            RataLast5Sample = RataLast4Sample;
            RataLast4Sample = RataLast3Sample;
            RataLast3Sample = RataLast2Sample;
            RataLast2Sample = RataLast1Sample;
            RataLast1Sample = inputSampleR;
            // setting up oversampled special antialiasing
            LclipOnset = false;
            RclipOnset = false;
            LmaxRecent = fabs(LataLast6Sample);
            if (fabs(LataLast5Sample) > LmaxRecent) {
                LmaxRecent = fabs(LataLast5Sample);
            }
            if (fabs(LataLast4Sample) > LmaxRecent) {
                LmaxRecent = fabs(LataLast4Sample);
            }
            if (fabs(LataLast3Sample) > LmaxRecent) {
                LmaxRecent = fabs(LataLast3Sample);
            }
            if (fabs(LataLast2Sample) > LmaxRecent) {
                LmaxRecent = fabs(LataLast2Sample);
            }
            if (fabs(LataLast1Sample) > LmaxRecent) {
                LmaxRecent = fabs(LataLast1Sample);
            }
            if (fabs(inputSampleL) > LmaxRecent) {
                LmaxRecent = fabs(inputSampleL);
            }
            // this gives us something that won't cut out in zero crossings, to interpolate with
            RmaxRecent = fabs(RataLast6Sample);
            if (fabs(RataLast5Sample) > RmaxRecent) {
                RmaxRecent = fabs(RataLast5Sample);
            }
            if (fabs(RataLast4Sample) > RmaxRecent) {
                RmaxRecent = fabs(RataLast4Sample);
            }
            if (fabs(RataLast3Sample) > RmaxRecent) {
                RmaxRecent = fabs(RataLast3Sample);
            }
            if (fabs(RataLast2Sample) > RmaxRecent) {
                RmaxRecent = fabs(RataLast2Sample);
            }
            if (fabs(RataLast1Sample) > RmaxRecent) {
                RmaxRecent = fabs(RataLast1Sample);
            }
            if (fabs(inputSampleR) > RmaxRecent) {
                RmaxRecent = fabs(inputSampleR);
            }
            // this gives us something that won't cut out in zero crossings, to interpolate with
            LmaxRecent *= 2.0;
            RmaxRecent *= 2.0;
            // by refclip this is 1.0 and fully into the antialiasing
            if (LmaxRecent > 1.0) {
                LmaxRecent = 1.0;
            }
            if (RmaxRecent > 1.0) {
                RmaxRecent = 1.0;
            }
            // and it tops out at 1. Higher means more antialiasing, lower blends into passThrough without antialiasing
            LataHalfwaySample -= Loverall;
            RataHalfwaySample -= Roverall;
            // subtract dist-cancel from input after getting raw input, before doing anything
            LdrySample = LataHalfwaySample;
            RdrySample = RataHalfwaySample;
            // begin L channel for the clipper
            if (LlastSample >= refclip)
            {
                LlpDepth += 0.1;
                if (LataHalfwaySample < refclip)
                {
                    LlastSample = ((refclip * hardness) + (LataHalfwaySample * softness));
                }
                else {
                    LlastSample = refclip;
                }
            }
            if (LlastSample <= -refclip)
            {
                LlpDepth += 0.1;
                if (LataHalfwaySample > -refclip)
                {
                    LlastSample = ((-refclip * hardness) + (LataHalfwaySample * softness));
                }
                else {
                    LlastSample = -refclip;
                }
            }
            if (LataHalfwaySample > refclip)
            {
                LlpDepth += 0.1;
                if (LlastSample < refclip)
                {
                    LataHalfwaySample = ((refclip * hardness) + (LlastSample * softness));
                }
                else {
                    LataHalfwaySample = refclip;
                }
            }
            if (LataHalfwaySample < -refclip)
            {
                LlpDepth += 0.1;
                if (LlastSample > -refclip)
                {
                    LataHalfwaySample = ((-refclip * hardness) + (LlastSample * softness));
                }
                else {
                    LataHalfwaySample = -refclip;
                }
            }
            /// end L channel for the clipper
            // begin R channel for the clipper
            if (RlastSample >= refclip)
            {
                RlpDepth += 0.1;
                if (RataHalfwaySample < refclip)
                {
                    RlastSample = ((refclip * hardness) + (RataHalfwaySample * softness));
                }
                else {
                    RlastSample = refclip;
                }
            }
            if (RlastSample <= -refclip)
            {
                RlpDepth += 0.1;
                if (RataHalfwaySample > -refclip)
                {
                    RlastSample = ((-refclip * hardness) + (RataHalfwaySample * softness));
                }
                else {
                    RlastSample = -refclip;
                }
            }
            if (RataHalfwaySample > refclip)
            {
                RlpDepth += 0.1;
                if (RlastSample < refclip)
                {
                    RataHalfwaySample = ((refclip * hardness) + (RlastSample * softness));
                }
                else {
                    RataHalfwaySample = refclip;
                }
            }
            if (RataHalfwaySample < -refclip)
            {
                RlpDepth += 0.1;
                if (RlastSample > -refclip)
                {
                    RataHalfwaySample = ((-refclip * hardness) + (RlastSample * softness));
                }
                else {
                    RataHalfwaySample = -refclip;
                }
            }
            /// end R channel for the clipper
            LoutputSample = LlastSample;
            RoutputSample = RlastSample;
            LlastSample = LataHalfwaySample;
            RlastSample = RataHalfwaySample;
            LataHalfwaySample = LoutputSample;
            RataHalfwaySample = RoutputSample;
            // swap around in a circle for one final ADClip,
            // this time not tracking overshoot anymore
            // end interpolated sample
            // begin raw sample- inputSample and ataDrySample handled separately here
            inputSampleL -= Loverall;
            inputSampleR -= Roverall;
            // subtract dist-cancel from input after getting raw input, before doing anything
            LdrySample = inputSampleL;
            RdrySample = inputSampleR;
            // begin second L clip
            if (LlastSample >= refclip)
            {
                LlpDepth += 0.1;
                if (inputSampleL < refclip)
                {
                    LlastSample = ((refclip * hardness) + (inputSampleL * softness));
                }
                else {
                    LlastSample = refclip;
                }
            }
            if (LlastSample <= -refclip)
            {
                LlpDepth += 0.1;
                if (inputSampleL > -refclip)
                {
                    LlastSample = ((-refclip * hardness) + (inputSampleL * softness));
                }
                else {
                    LlastSample = -refclip;
                }
            }
            if (inputSampleL > refclip)
            {
                LlpDepth += 0.1;
                if (LlastSample < refclip)
                {
                    inputSampleL = ((refclip * hardness) + (LlastSample * softness));
                }
                else {
                    inputSampleL = refclip;
                }
            }
            if (inputSampleL < -refclip)
            {
                LlpDepth += 0.1;
                if (LlastSample > -refclip)
                {
                    inputSampleL = ((-refclip * hardness) + (LlastSample * softness));
                }
                else {
                    inputSampleL = -refclip;
                }
            }
            // end second L clip
            // begin second R clip
            if (RlastSample >= refclip)
            {
                RlpDepth += 0.1;
                if (inputSampleR < refclip)
                {
                    RlastSample = ((refclip * hardness) + (inputSampleR * softness));
                }
                else {
                    RlastSample = refclip;
                }
            }
            if (RlastSample <= -refclip)
            {
                RlpDepth += 0.1;
                if (inputSampleR > -refclip)
                {
                    RlastSample = ((-refclip * hardness) + (inputSampleR * softness));
                }
                else {
                    RlastSample = -refclip;
                }
            }
            if (inputSampleR > refclip)
            {
                RlpDepth += 0.1;
                if (RlastSample < refclip)
                {
                    inputSampleR = ((refclip * hardness) + (RlastSample * softness));
                }
                else {
                    inputSampleR = refclip;
                }
            }
            if (inputSampleR < -refclip)
            {
                RlpDepth += 0.1;
                if (RlastSample > -refclip)
                {
                    inputSampleR = ((-refclip * hardness) + (RlastSample * softness));
                }
                else {
                    inputSampleR = -refclip;
                }
            }
            // end second R clip
            LoutputSample = LlastSample;
            RoutputSample = RlastSample;
            LlastSample = inputSampleL;
            RlastSample = inputSampleR;
            inputSampleL = LoutputSample;
            inputSampleR = RoutputSample;
            LataHalfDrySample = (LataDrySample * ataK3) + (LataHalfDrySample * ataK4);
            LataHalfDiffSample = (LataHalfwaySample - LataHalfDrySample) / 2.0;
            LataLastDiffSample = LataDiffSample * ataK5;
            LataDiffSample = (inputSampleL - LataDrySample) / 2.0;
            LataDiffSample += LataHalfDiffSample;
            LataDiffSample -= LataLastDiffSample;
            inputSampleL = LataDrySample;
            inputSampleL += LataDiffSample;
            RataHalfDrySample = (RataDrySample * ataK3) + (RataHalfDrySample * ataK4);
            RataHalfDiffSample = (RataHalfwaySample - RataHalfDrySample) / 2.0;
            RataLastDiffSample = RataDiffSample * ataK5;
            RataDiffSample = (inputSampleR - RataDrySample) / 2.0;
            RataDiffSample += RataHalfDiffSample;
            RataDiffSample -= RataLastDiffSample;
            inputSampleR = RataDrySample;
            inputSampleR += RataDiffSample;
            Loverall = (Loverall * cancelold) + (LataDiffSample * cancelnew);
            Roverall = (Roverall * cancelold) + (RataDiffSample * cancelnew);
            // apply all the diffs to a lowpassed IIR
            if (flip)
            {
                LiirSampleA = (LiirSampleA * altAmount) + (inputSampleL * iirAmount);
                inputSampleL -= LiirSampleA;
                LiirSampleC = (LiirSampleC * altAmount) + (LpassThrough * iirAmount);
                LpassThrough -= LiirSampleC;
                RiirSampleA = (RiirSampleA * altAmount) + (inputSampleR * iirAmount);
                inputSampleR -= RiirSampleA;
                RiirSampleC = (RiirSampleC * altAmount) + (RpassThrough * iirAmount);
                RpassThrough -= RiirSampleC;
            }
            else
            {
                LiirSampleB = (LiirSampleB * altAmount) + (inputSampleL * iirAmount);
                inputSampleL -= LiirSampleB;
                LiirSampleD = (LiirSampleD * altAmount) + (LpassThrough * iirAmount);
                LpassThrough -= LiirSampleD;
                RiirSampleB = (RiirSampleB * altAmount) + (inputSampleR * iirAmount);
                inputSampleR -= RiirSampleB;
                RiirSampleD = (RiirSampleD * altAmount) + (RpassThrough * iirAmount);
                RpassThrough -= RiirSampleD;
            }
            flip = !flip;
            // highpass section
            LlastOut3Sample = LlastOut2Sample;
            LlastOut2Sample = LlastOutSample;
            LlastOutSample = inputSampleL;
            RlastOut3Sample = RlastOut2Sample;
            RlastOut2Sample = RlastOutSample;
            RlastOutSample = inputSampleR;
            LlpDepth -= lpSpeed;
            RlpDepth -= lpSpeed;
            if (LlpDepth > 0.0)
            {
                if (LlpDepth > 1.0) {
                    LlpDepth = 1.0;
                }
                inputSampleL *= (1.0 - LlpDepth);
                inputSampleL += (((LlastOutSample + LlastOut2Sample + LlastOut3Sample) / 3.6) * LlpDepth);
            }
            if (RlpDepth > 0.0)
            {
                if (RlpDepth > 1.0) {
                    RlpDepth = 1.0;
                }
                inputSampleR *= (1.0 - RlpDepth);
                inputSampleR += (((RlastOutSample + RlastOut2Sample + RlastOut3Sample) / 3.6) * RlpDepth);
            }
            if (LlpDepth < 0.0) {
                LlpDepth = 0.0;
            }
            if (RlpDepth < 0.0) {
                RlpDepth = 0.0;
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
            inputSampleL *= (1.0 - LmaxRecent);
            inputSampleR *= (1.0 - RmaxRecent);
            inputSampleL += (LpassThrough * LmaxRecent);
            inputSampleR += (RpassThrough * RmaxRecent);
            // there's our raw signal, without antialiasing. Brings up low level stuff and softens more when hot
            if (inputSampleL > cliplevel) {
                inputSampleL = cliplevel;
            }
            if (inputSampleL < -cliplevel) {
                inputSampleL = -cliplevel;
            }
            if (inputSampleR > cliplevel) {
                inputSampleR = cliplevel;
            }
            if (inputSampleR < -cliplevel) {
                inputSampleR = -cliplevel;
            }
            // final iron bar
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::aquickvoiceclip
