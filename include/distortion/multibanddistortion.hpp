#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::multibanddistortion {

constexpr std::string_view k_name{ "MultiBandDistortion" };
constexpr std::string_view k_short_description{
    "MultiBandDistortion is an old weird gnarly sound wrecker :)"
};
constexpr std::string_view k_long_description{
    "Some plugins just want to watch the world burn :)I don’t have all that much to say about this one. I’ve been needing to post it: I’ve got folks who ask me for soundgoodizer/OTT type things and try to get a ‘slammed’, deeply unnatural, multiband kind of effect going… and all my life I’ve been learning how to get AWAY from that kind of sonic disassociation and audio gibberish towards stuff with a very different texture. So I’m not a natural match for that kind of thing, and yet I get asked for my take on plugins like that.So, long ago, I made MultiBandDistortion. I figured if you were going to wreck your sound, let’s REALLY wreck it, and I did some very gnarly things in there that I now don’t entirely understand. I know that if you turn ‘stable’ down, you get a choppy effect not unlike the ZVex Fuzz Factory pedal, only not: the interaction between the bands can get sketchy. Again, I don’t remember how this worked and don’t expect to be revising it or making it more controllable or cleaner: that kind of misses the point. If you’re trying to get a slick version of this kind of thing you start by not using this kind of thing :)"
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class MultiBandDistortion final : public Effect<T>
{
    double ataLast3SampleL;
    double ataLast2SampleL;
    double ataLast1SampleL;
    double ataHalfwaySampleL;
    double ataHalfDrySampleL;
    double ataHalfDiffSampleL;
    double ataAL;
    double ataBL;
    double ataCL;
    double ataDecayL;
    double ataUpsampleHighTweakL;
    double ataDrySampleL;
    double ataDiffSampleL;
    double ataPrevDiffSampleL;
    double iirSampleAL;
    double iirSampleBL;
    bool ataFlip; // end defining of antialiasing variables
    double ataLast3SampleR;
    double ataLast2SampleR;
    double ataLast1SampleR;
    double ataHalfwaySampleR;
    double ataHalfDrySampleR;
    double ataHalfDiffSampleR;
    double ataAR;
    double ataBR;
    double ataCR;
    double ataDecayR;
    double ataUpsampleHighTweakR;
    double ataDrySampleR;
    double ataDiffSampleR;
    double ataPrevDiffSampleR;
    double iirSampleAR;
    double iirSampleBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;

  public:
    MultiBandDistortion()
    {
        A = 0.5;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        E = 0.0;
        F = 1.0;
        G = 1.0;
        H = 1.0;
        I = 0.75;
        ataLast3SampleL = ataLast2SampleL = ataLast1SampleL = 0.0;
        ataHalfwaySampleL = ataHalfDrySampleL = ataHalfDiffSampleL = 0.0;
        ataAL = ataBL = ataCL = ataDrySampleL = ataDiffSampleL = ataPrevDiffSampleL = 0.0;
        ataUpsampleHighTweakL = 0.0414213562373095048801688; // more adds treble to upsampling
        ataDecayL = 0.915965594177219015; // Catalan's constant, more adds focus and clarity
        iirSampleAL = 0;
        iirSampleBL = 0;
        ataFlip = false; // end reset of antialias parameters
        ataLast3SampleR = ataLast2SampleR = ataLast1SampleR = 0.0;
        ataHalfwaySampleR = ataHalfDrySampleR = ataHalfDiffSampleR = 0.0;
        ataAR = ataBR = ataCR = ataDrySampleR = ataDiffSampleR = ataPrevDiffSampleR = 0.0;
        ataUpsampleHighTweakR = 0.0414213562373095048801688; // more adds treble to upsampling
        ataDecayR = 0.915965594177219015; // Catalan's constant, more adds focus and clarity
        iirSampleAR = 0;
        iirSampleBR = 0;
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
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kParamF = 5,
        kParamG = 6,
        kParamH = 7,
        kParamI = 8,
        kNumParameters = 9
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
            break;
        kParamF:
            F = value;
            break;
        kParamG:
            G = value;
            break;
        kParamH:
            H = value;
            break;
        kParamI:
            I = value;
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
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;
        kParamE:
            return E;
            break;
        kParamF:
            return F;
            break;
        kParamG:
            return G;
            break;
        kParamH:
            return H;
            break;
        kParamI:
            return I;
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
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.0;
            break;
        kParamD:
            return 0.0;
            break;
        kParamE:
            return 0.0;
            break;
        kParamF:
            return 1.0;
            break;
        kParamG:
            return 1.0;
            break;
        kParamH:
            return 1.0;
            break;
        kParamI:
            return 0.75;
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
            return "freqcy";
            break;
        kParamB:
            return "h gain";
            break;
        kParamC:
            return "l gain";
            break;
        kParamD:
            return "h hard";
            break;
        kParamE:
            return "l hard";
            break;
        kParamF:
            return "h mtrx";
            break;
        kParamG:
            return "l mtrx";
            break;
        kParamH:
            return "stable";
            break;
        kParamI:
            return "output";
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
            return "Freqcy";
            break;
        kParamB:
            return "H Gain";
            break;
        kParamC:
            return "L Gain";
            break;
        kParamD:
            return "H Hard";
            break;
        kParamE:
            return "L Hard";
            break;
        kParamF:
            return "H Mtrx";
            break;
        kParamG:
            return "L Mtrx";
            break;
        kParamH:
            return "Stable";
            break;
        kParamI:
            return "Output";
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
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
            break;
        kParamF:
            return std::to_string(F);
            break;
        kParamG:
            return std::to_string(G);
            break;
        kParamH:
            return std::to_string(H);
            break;
        kParamI:
            return std::to_string(I);
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
        kParamB:
            return "dB";
            break;
        kParamC:
            return "dB";
            break;
        kParamD:
            return "";
            break;
        kParamE:
            return "";
            break;
        kParamF:
            return "";
            break;
        kParamG:
            return "";
            break;
        kParamH:
            return "";
            break;
        kParamI:
            return "dB";
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double iirAmount = pow(A, 3) / overallscale;
        double gainH = pow(10.0, (B * 48.0) / 20);
        double thresholdH = D;
        double hardnessH;
        if (thresholdH < 1.0) {
            hardnessH = 1.0 / (1.0 - thresholdH);
        }
        else {
            hardnessH = 999999999999999999999.0;
        }
        double gainL = pow(10.0, (C * 48.0) / 20);
        double thresholdL = E;
        double hardnessL;
        if (thresholdL < 1.0) {
            hardnessL = 1.0 / (1.0 - thresholdL);
        }
        else {
            hardnessL = 999999999999999999999.0;
        }
        double breakup = 1.5707963267949;
        double bridgerectifier;
        double outputH = F;
        double outputL = G;
        double outputD = H * 0.597;
        double outtrim = outputH + outputL + outputD;
        outputH *= outtrim;
        outputL *= outtrim;
        outputD *= outtrim;
        double outputGlobal = pow(10.0, ((I - 1.0) * 48.0) / 20);
        double tempHL;
        double tempLL;
        double tempHR;
        double tempLR;
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
            ataDrySampleL = inputSampleL;
            ataHalfDrySampleL = ataHalfwaySampleL = (inputSampleL + ataLast1SampleL + ((-ataLast2SampleL + ataLast3SampleL) * ataUpsampleHighTweakL)) / 2.0;
            ataLast3SampleL = ataLast2SampleL;
            ataLast2SampleL = ataLast1SampleL;
            ataLast1SampleL = inputSampleL;
            // setting up oversampled special antialiasing
            ataDrySampleR = inputSampleR;
            ataHalfDrySampleR = ataHalfwaySampleR = (inputSampleR + ataLast1SampleR + ((-ataLast2SampleR + ataLast3SampleR) * ataUpsampleHighTweakR)) / 2.0;
            ataLast3SampleR = ataLast2SampleR;
            ataLast2SampleR = ataLast1SampleR;
            ataLast1SampleR = inputSampleR;
            // setting up oversampled special antialiasing
            // pre-center code on inputSample and halfwaySample in parallel
            // begin interpolated sample- change inputSample -> ataHalfwaySample, ataDrySample -> ataHalfDrySample
            tempLL = iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (ataHalfwaySampleL * iirAmount);
            tempHL = ataHalfwaySampleL - iirSampleAL;
            // highpass section
            tempLR = iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (ataHalfwaySampleR * iirAmount);
            tempHR = ataHalfwaySampleR - iirSampleAR;
            // highpass section
            tempHL *= gainH;
            if (fabs(tempHL) > thresholdH)
            {
                bridgerectifier = (fabs(tempHL) - thresholdH) * hardnessH;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessH;
                // do the sine factor, scale back to proper amount
                if (tempHL > 0) {
                    tempHL = bridgerectifier + thresholdH;
                }
                else {
                    tempHL = -(bridgerectifier + thresholdH);
                }
            }
            tempHR *= gainH;
            if (fabs(tempHR) > thresholdH)
            {
                bridgerectifier = (fabs(tempHR) - thresholdH) * hardnessH;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessH;
                // do the sine factor, scale back to proper amount
                if (tempHR > 0) {
                    tempHR = bridgerectifier + thresholdH;
                }
                else {
                    tempHR = -(bridgerectifier + thresholdH);
                }
            }
            tempLL *= gainL;
            if (fabs(tempLL) > thresholdL)
            {
                bridgerectifier = (fabs(tempLL) - thresholdL) * hardnessL;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessL;
                // do the sine factor, scale back to proper amount
                if (tempLL > 0) {
                    tempLL = bridgerectifier + thresholdL;
                }
                else {
                    tempLL = -(bridgerectifier + thresholdL);
                }
            }
            tempLR *= gainL;
            if (fabs(tempLR) > thresholdL)
            {
                bridgerectifier = (fabs(tempLR) - thresholdL) * hardnessL;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessL;
                // do the sine factor, scale back to proper amount
                if (tempLR > 0) {
                    tempLR = bridgerectifier + thresholdL;
                }
                else {
                    tempLR = -(bridgerectifier + thresholdL);
                }
            }
            ataHalfwaySampleL = (tempLL * outputL) + (tempHL * outputH);
            ataHalfwaySampleR = (tempLR * outputL) + (tempHR * outputH);
            // end interpolated sample
            // begin raw sample- inputSample and ataDrySample handled separately here
            tempLL = iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
            tempHL = inputSampleL - iirSampleBL;
            // highpass section
            tempLR = iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
            tempHR = inputSampleR - iirSampleBR;
            // highpass section
            tempHL *= gainH;
            if (fabs(tempHL) > thresholdH)
            {
                bridgerectifier = (fabs(tempHL) - thresholdH) * hardnessH;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessH;
                // do the sine factor, scale back to proper amount
                if (tempHL > 0) {
                    tempHL = bridgerectifier + thresholdH;
                }
                else {
                    tempHL = -(bridgerectifier + thresholdH);
                }
            }
            tempHR *= gainH;
            if (fabs(tempHR) > thresholdH)
            {
                bridgerectifier = (fabs(tempHR) - thresholdH) * hardnessH;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessH;
                // do the sine factor, scale back to proper amount
                if (tempHR > 0) {
                    tempHR = bridgerectifier + thresholdH;
                }
                else {
                    tempHR = -(bridgerectifier + thresholdH);
                }
            }
            tempLL *= gainL;
            if (fabs(tempLL) > thresholdL)
            {
                bridgerectifier = (fabs(tempLL) - thresholdL) * hardnessL;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessL;
                // do the sine factor, scale back to proper amount
                if (tempLL > 0) {
                    tempLL = bridgerectifier + thresholdL;
                }
                else {
                    tempLL = -(bridgerectifier + thresholdL);
                }
            }
            tempLR *= gainL;
            if (fabs(tempLR) > thresholdL)
            {
                bridgerectifier = (fabs(tempLR) - thresholdL) * hardnessL;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardnessL;
                // do the sine factor, scale back to proper amount
                if (tempLR > 0) {
                    tempLR = bridgerectifier + thresholdL;
                }
                else {
                    tempLR = -(bridgerectifier + thresholdL);
                }
            }
            inputSampleL = (tempLL * outputL) + (tempHL * outputH);
            inputSampleR = (tempLR * outputL) + (tempHR * outputH);
            // end raw sample
            // begin antialiasing section for halfway sample
            ataCL = ataHalfwaySampleL - ataHalfDrySampleL;
            if (ataFlip) {
                ataAL *= ataDecayL;
                ataBL *= ataDecayL;
                ataAL += ataCL;
                ataBL -= ataCL;
                ataCL = ataAL;
            }
            else {
                ataBL *= ataDecayL;
                ataAL *= ataDecayL;
                ataBL += ataCL;
                ataAL -= ataCL;
                ataCL = ataBL;
            }
            ataHalfDiffSampleL = (ataCL * ataDecayL);
            // end antialiasing section for halfway sample L
            ataCR = ataHalfwaySampleR - ataHalfDrySampleR;
            if (ataFlip) {
                ataAR *= ataDecayR;
                ataBR *= ataDecayR;
                ataAR += ataCR;
                ataBR -= ataCR;
                ataCR = ataAR;
            }
            else {
                ataBR *= ataDecayR;
                ataAR *= ataDecayR;
                ataBR += ataCR;
                ataAR -= ataCR;
                ataCR = ataBR;
            }
            ataHalfDiffSampleR = (ataCR * ataDecayR);
            // end antialiasing section for halfway sample R
            // begin antialiasing section for raw sample
            ataCL = inputSampleL - ataDrySampleL;
            if (ataFlip) {
                ataAL *= ataDecayL;
                ataBL *= ataDecayL;
                ataAL += ataCL;
                ataBL -= ataCL;
                ataCL = ataAL;
            }
            else {
                ataBL *= ataDecayL;
                ataAL *= ataDecayL;
                ataBL += ataCL;
                ataAL -= ataCL;
                ataCL = ataBL;
            }
            ataDiffSampleL = (ataCL * ataDecayL);
            // end antialiasing section for input sample L
            ataCR = inputSampleR - ataDrySampleR;
            if (ataFlip) {
                ataAR *= ataDecayR;
                ataBR *= ataDecayR;
                ataAR += ataCR;
                ataBR -= ataCR;
                ataCR = ataAR;
            }
            else {
                ataBR *= ataDecayR;
                ataAR *= ataDecayR;
                ataBR += ataCR;
                ataAR -= ataCR;
                ataCR = ataBR;
            }
            ataDiffSampleR = (ataCR * ataDecayR);
            // end antialiasing section for input sample R
            ataFlip = !ataFlip;
            inputSampleL = ataDrySampleL * outputD;
            inputSampleL += (ataDiffSampleL + ataHalfDiffSampleL);
            inputSampleR = ataDrySampleR * outputD;
            inputSampleR += (ataDiffSampleR + ataHalfDiffSampleR);
            // apply processing as difference to non-oversampled raw input
            inputSampleL *= outputGlobal;
            inputSampleR *= outputGlobal;
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
} // namespace airwindohhs::multibanddistortion
