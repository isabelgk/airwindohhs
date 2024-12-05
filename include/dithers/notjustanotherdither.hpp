#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::notjustanotherdither {

constexpr std::string_view k_name{ "NotJustAnotherDither" };
constexpr std::string_view k_short_description{
    "NotJustAnotherDither is a next-gen wordlength reducer at 24 bit, with DeRez."
};
constexpr std::string_view k_long_description{
    "The votes are in, and the Internet has spoken. We’ve got a name for the new noise shaping variant on Naturalize, after a January of wild suggestions. Seems most people simply wanted to emphasize that this was not your ordinary dither, not your daddy’s TPDF.This is the plugin (two, in fact) that beats all the other ones, including the original Naturalize. In normal use, this ‘dither’ (in fact it’s not a dither at all: it’s the Benford realness stuff from Naturalize, run with a noise shaper, and all the added random noise taken OUT) will make any fixed-point output sound like infinite resolution. It’s AU, Mac and PC VST. It has a switch for 16/24 bit operation (defaults to 24) and a ‘DeRez’ control where you can get low-bit NJAD audio if you like, or if you want to audition its noise floor behavior.The output levels are low, without a bunch of extra energy coming from the noise shaper: it’s pretty well behaved, though like any hyper-performance ‘dither’ this is really meant for output formats. I don’t think any harm can come from repeatedly using this in processing, but you can always use a normal TPDF if you want no change in tonal balance (plain TPDF dither only adds broadband noise: more opaque, but if it builds up it won’t accentuate anything: dithers with a lot of high frequency energy can end up exaggerating that.)(followup: I added a 16/24 bit switch, and DeRez. Also, the most current version of this is in Monitoring.)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class NotJustAnotherDither final : public Effect<T>
{
    double bynL[13];
    double bynR[13];
    double noiseShapingL;
    double noiseShapingR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    NotJustAnotherDither()
    {
        A = 1.0;
        B = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        bynL[0] = 1000;
        bynL[1] = 301;
        bynL[2] = 176;
        bynL[3] = 125;
        bynL[4] = 97;
        bynL[5] = 79;
        bynL[6] = 67;
        bynL[7] = 58;
        bynL[8] = 51;
        bynL[9] = 46;
        bynL[10] = 1000;
        noiseShapingL = 0.0;
        bynR[0] = 1000;
        bynR[1] = 301;
        bynR[2] = 176;
        bynR[3] = 125;
        bynR[4] = 97;
        bynR[5] = 79;
        bynR[6] = 67;
        bynR[7] = 58;
        bynR[8] = 51;
        bynR[9] = 46;
        bynR[10] = 1000;
        noiseShapingR = 0.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "quant"; break;
            case kParamB: return "derez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Quant"; break;
            case kParamB: return "DeRez"; break;

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

        int processing = (VstInt32)(A * 1.999);
        bool highres = false;
        if (processing == 1) {
            highres = true;
        }
        float scaleFactor;
        if (highres) {
            scaleFactor = 8388608.0;
        }
        else {
            scaleFactor = 32768.0;
        }
        float derez = B;
        if (derez > 0.0) {
            scaleFactor *= pow(1.0 - derez, 6);
        }
        if (scaleFactor < 0.0001) {
            scaleFactor = 0.0001;
        }
        float outScale = scaleFactor;
        if (outScale < 8.0) {
            outScale = 8.0;
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            // 0-1 is now one bit, now we dither
            // begin Not Just Another Dither
            bool cutbinsL;
            cutbinsL = false;
            bool cutbinsR;
            cutbinsR = false;
            double drySampleL;
            drySampleL = inputSampleL;
            double drySampleR;
            drySampleR = inputSampleR;
            inputSampleL -= noiseShapingL;
            inputSampleR -= noiseShapingR;
            // NJAD L
            double benfordize;
            benfordize = floor(inputSampleL);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            while (benfordize < 1.0 && benfordize > 0.0000001) {
                benfordize *= 10;
            }
            int hotbinA;
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            double totalA;
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynL[hotbinA] += 1;
                if (bynL[hotbinA] > 982) {
                    cutbinsL = true;
                }
                totalA += (301 - bynL[1]);
                totalA += (176 - bynL[2]);
                totalA += (125 - bynL[3]);
                totalA += (97 - bynL[4]);
                totalA += (79 - bynL[5]);
                totalA += (67 - bynL[6]);
                totalA += (58 - bynL[7]);
                totalA += (51 - bynL[8]);
                totalA += (46 - bynL[9]);
                bynL[hotbinA] -= 1;
            }
            else {
                hotbinA = 10;
            }
            // produce total number- smaller is closer to Benford real
            benfordize = ceil(inputSampleL);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            while (benfordize < 1.0 && benfordize > 0.0000001) {
                benfordize *= 10;
            }
            int hotbinB;
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            double totalB;
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynL[hotbinB] += 1;
                if (bynL[hotbinB] > 982) {
                    cutbinsL = true;
                }
                totalB += (301 - bynL[1]);
                totalB += (176 - bynL[2]);
                totalB += (125 - bynL[3]);
                totalB += (97 - bynL[4]);
                totalB += (79 - bynL[5]);
                totalB += (67 - bynL[6]);
                totalB += (58 - bynL[7]);
                totalB += (51 - bynL[8]);
                totalB += (46 - bynL[9]);
                bynL[hotbinB] -= 1;
            }
            else {
                hotbinB = 10;
            }
            // produce total number- smaller is closer to Benford real
            double outputSampleL;
            if (totalA < totalB) {
                bynL[hotbinA] += 1;
                outputSampleL = floor(inputSampleL);
            }
            else {
                bynL[hotbinB] += 1;
                outputSampleL = floor(inputSampleL + 1);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            if (cutbinsL) {
                bynL[1] *= 0.99;
                bynL[2] *= 0.99;
                bynL[3] *= 0.99;
                bynL[4] *= 0.99;
                bynL[5] *= 0.99;
                bynL[6] *= 0.99;
                bynL[7] *= 0.99;
                bynL[8] *= 0.99;
                bynL[9] *= 0.99;
                bynL[10] *= 0.99;
            }
            noiseShapingL += outputSampleL - drySampleL;
            if (noiseShapingL > fabs(inputSampleL)) {
                noiseShapingL = fabs(inputSampleL);
            }
            if (noiseShapingL < -fabs(inputSampleL)) {
                noiseShapingL = -fabs(inputSampleL);
            }
            // finished NJAD L
            // NJAD R
            benfordize = floor(inputSampleR);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            while (benfordize < 1.0 && benfordize > 0.0000001) {
                benfordize *= 10;
            }
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynR[hotbinA] += 1;
                if (bynR[hotbinA] > 982) {
                    cutbinsR = true;
                }
                totalA += (301 - bynR[1]);
                totalA += (176 - bynR[2]);
                totalA += (125 - bynR[3]);
                totalA += (97 - bynR[4]);
                totalA += (79 - bynR[5]);
                totalA += (67 - bynR[6]);
                totalA += (58 - bynR[7]);
                totalA += (51 - bynR[8]);
                totalA += (46 - bynR[9]);
                bynR[hotbinA] -= 1;
            }
            else {
                hotbinA = 10;
            }
            // produce total number- smaller is closer to Benford real
            benfordize = ceil(inputSampleR);
            while (benfordize >= 1.0) {
                benfordize /= 10;
            }
            while (benfordize < 1.0 && benfordize > 0.0000001) {
                benfordize *= 10;
            }
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynR[hotbinB] += 1;
                if (bynR[hotbinB] > 982) {
                    cutbinsR = true;
                }
                totalB += (301 - bynR[1]);
                totalB += (176 - bynR[2]);
                totalB += (125 - bynR[3]);
                totalB += (97 - bynR[4]);
                totalB += (79 - bynR[5]);
                totalB += (67 - bynR[6]);
                totalB += (58 - bynR[7]);
                totalB += (51 - bynR[8]);
                totalB += (46 - bynR[9]);
                bynR[hotbinB] -= 1;
            }
            else {
                hotbinB = 10;
            }
            // produce total number- smaller is closer to Benford real
            double outputSampleR;
            if (totalA < totalB) {
                bynR[hotbinA] += 1;
                outputSampleR = floor(inputSampleR);
            }
            else {
                bynR[hotbinB] += 1;
                outputSampleR = floor(inputSampleR + 1);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            if (cutbinsR) {
                bynR[1] *= 0.99;
                bynR[2] *= 0.99;
                bynR[3] *= 0.99;
                bynR[4] *= 0.99;
                bynR[5] *= 0.99;
                bynR[6] *= 0.99;
                bynR[7] *= 0.99;
                bynR[8] *= 0.99;
                bynR[9] *= 0.99;
                bynR[10] *= 0.99;
            }
            noiseShapingR += outputSampleR - drySampleR;
            if (noiseShapingR > fabs(inputSampleR)) {
                noiseShapingR = fabs(inputSampleR);
            }
            if (noiseShapingR < -fabs(inputSampleR)) {
                noiseShapingR = -fabs(inputSampleR);
            }
            // finished NJAD R
            inputSampleL = outputSampleL / outScale;
            inputSampleR = outputSampleR / outScale;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::notjustanotherdither
