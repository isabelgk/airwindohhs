#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::luxor {

constexpr std::string_view k_name{ "Luxor" };
constexpr std::string_view k_short_description{
    "Luxor is a re-release of another old Character plugin."
};
constexpr std::string_view k_long_description{
    "Luxor is the Character plugin that’s based off Manley impulses. It does NOT sound like a Manley anything. Instead, it does a weird and maybe cool thing: you might treat it like an amp sim or something, see what you can get out of it. Cranking syn drums through it ought to be good. It is what it is, namely ‘a weird plugin not like anything else’ and I hope you like it."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Luxor final : public Effect<T>
{
    double bR[35];
    double lastSampleR;
    double bL[35];
    double lastSampleL;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    Luxor()
    {
        A = 0.70;
        B = 0.3333333;
        C = 0.3333333;
        D = 1.0;
        for (int count = 0; count < 34; count++) {
            bR[count] = 0;
            bL[count] = 0;
        }
        lastSampleR = 0.0;
        lastSampleL = 0.0;
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
        kNumParameters = 4

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.70;
            case kParamB: return 0.3333333;
            case kParamC: return 0.3333333;
            case kParamD: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "hardns";
            case kParamB: return "persnlty";
            case kParamC: return "drive";
            case kParamD: return "output";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Hardns";
            case kParamB: return "Persnlty";
            case kParamC: return "Drive";
            case kParamD: return "Output";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B * 3.0);
            case kParamC: return std::to_string(C * 3.0);
            case kParamD: return std::to_string(D);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double threshold = A;
        double hardness;
        double breakup = (1.0 - (threshold / 2.0)) * 3.14159265358979;
        double bridgerectifier;
        double sqdrive = (B * 3.0);
        if (sqdrive > 1.0) {
            sqdrive *= sqdrive;
        }
        sqdrive = sqrt(sqdrive);
        double indrive = C * 3.0;
        if (indrive > 1.0) {
            indrive *= indrive;
        }
        indrive *= (1.0 + (0.1935 * sqdrive));
        // correct for gain loss of convolution
        // calibrate this to match noise level with character at 1.0
        // you get for instance 0.819 and 1.0-0.819 is 0.181
        double randy;
        double outlevel = D;
        if (threshold < 1) {
            hardness = 1.0 / (1.0 - threshold);
        }
        else {
            hardness = 999999999999999999999.0;
        }
        // set up hardness to exactly fill gap between threshold and 0db
        // if threshold is literally 1 then hardness is infinite, so we make it very big
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
            inputSampleL *= indrive;
            inputSampleR *= indrive;
            // calibrated to match gain through convolution and -0.3 correction
            if (sqdrive > 0.0) {
                bL[33] = bL[32];
                bL[32] = bL[31];
                bL[31] = bL[30];
                bL[30] = bL[29];
                bL[29] = bL[28];
                bL[28] = bL[27];
                bL[27] = bL[26];
                bL[26] = bL[25];
                bL[25] = bL[24];
                bL[24] = bL[23];
                bL[23] = bL[22];
                bL[22] = bL[21];
                bL[21] = bL[20];
                bL[20] = bL[19];
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
                bL[0] = inputSampleL * sqdrive;
                inputSampleL -= (bL[1] * (0.20641602693167951 - (0.00078952185394898 * fabs(bL[1]))));
                inputSampleL += (bL[2] * (0.07601816702459827 + (0.00022786334179951 * fabs(bL[2]))));
                inputSampleL -= (bL[3] * (0.03929765560019285 - (0.00054517993246352 * fabs(bL[3]))));
                inputSampleL -= (bL[4] * (0.00298333157711103 - (0.00033083756545638 * fabs(bL[4]))));
                inputSampleL += (bL[5] * (0.00724006282304610 + (0.00045483683460812 * fabs(bL[5]))));
                inputSampleL -= (bL[6] * (0.03073108963506036 - (0.00038190060537423 * fabs(bL[6]))));
                inputSampleL += (bL[7] * (0.02332434692533051 + (0.00040347288688932 * fabs(bL[7]))));
                inputSampleL -= (bL[8] * (0.03792606869061214 - (0.00039673687335892 * fabs(bL[8]))));
                inputSampleL += (bL[9] * (0.02437059376675688 + (0.00037221210539535 * fabs(bL[9]))));
                inputSampleL -= (bL[10] * (0.03416764311979521 - (0.00040314850796953 * fabs(bL[10]))));
                inputSampleL += (bL[11] * (0.01761669868102127 + (0.00035989484330131 * fabs(bL[11]))));
                inputSampleL -= (bL[12] * (0.02538237753523052 - (0.00040149119125394 * fabs(bL[12]))));
                inputSampleL += (bL[13] * (0.00770737340728377 + (0.00035462118723555 * fabs(bL[13]))));
                inputSampleL -= (bL[14] * (0.01580706228482803 - (0.00037563141307594 * fabs(bL[14]))));
                inputSampleL -= (bL[15] * (0.00055119240005586 - (0.00035409299268971 * fabs(bL[15]))));
                inputSampleL -= (bL[16] * (0.00818552143438768 - (0.00036507661042180 * fabs(bL[16]))));
                inputSampleL -= (bL[17] * (0.00661842703548304 - (0.00034550528559056 * fabs(bL[17]))));
                inputSampleL -= (bL[18] * (0.00362447476272098 - (0.00035553012761240 * fabs(bL[18]))));
                inputSampleL -= (bL[19] * (0.00957098027225745 - (0.00034091691045338 * fabs(bL[19]))));
                inputSampleL -= (bL[20] * (0.00193621774016660 - (0.00034554529131668 * fabs(bL[20]))));
                inputSampleL -= (bL[21] * (0.01005433027357935 - (0.00033878223153845 * fabs(bL[21]))));
                inputSampleL -= (bL[22] * (0.00221712428802004 - (0.00033481410137711 * fabs(bL[22]))));
                inputSampleL -= (bL[23] * (0.00911255639207995 - (0.00033263425232666 * fabs(bL[23]))));
                inputSampleL -= (bL[24] * (0.00339667169034909 - (0.00032634428038430 * fabs(bL[24]))));
                inputSampleL -= (bL[25] * (0.00774096948249924 - (0.00032599868802996 * fabs(bL[25]))));
                inputSampleL -= (bL[26] * (0.00463907626773794 - (0.00032131993173361 * fabs(bL[26]))));
                inputSampleL -= (bL[27] * (0.00658222997260378 - (0.00032014977430211 * fabs(bL[27]))));
                inputSampleL -= (bL[28] * (0.00550347079924993 - (0.00031557153256653 * fabs(bL[28]))));
                inputSampleL -= (bL[29] * (0.00588754981375325 - (0.00032041307242303 * fabs(bL[29]))));
                inputSampleL -= (bL[30] * (0.00590293898419892 - (0.00030457857428714 * fabs(bL[30]))));
                inputSampleL -= (bL[31] * (0.00558952010441800 - (0.00030448053548086 * fabs(bL[31]))));
                inputSampleL -= (bL[32] * (0.00598183557634295 - (0.00030715064323181 * fabs(bL[32]))));
                inputSampleL -= (bL[33] * (0.00555223929714115 - (0.00030319367948553 * fabs(bL[33]))));
                // voxbox 40 db
                bR[33] = bR[32];
                bR[32] = bR[31];
                bR[31] = bR[30];
                bR[30] = bR[29];
                bR[29] = bR[28];
                bR[28] = bR[27];
                bR[27] = bR[26];
                bR[26] = bR[25];
                bR[25] = bR[24];
                bR[24] = bR[23];
                bR[23] = bR[22];
                bR[22] = bR[21];
                bR[21] = bR[20];
                bR[20] = bR[19];
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
                bR[0] = inputSampleR * sqdrive;
                inputSampleR -= (bR[1] * (0.20641602693167951 - (0.00078952185394898 * fabs(bR[1]))));
                inputSampleR += (bR[2] * (0.07601816702459827 + (0.00022786334179951 * fabs(bR[2]))));
                inputSampleR -= (bR[3] * (0.03929765560019285 - (0.00054517993246352 * fabs(bR[3]))));
                inputSampleR -= (bR[4] * (0.00298333157711103 - (0.00033083756545638 * fabs(bR[4]))));
                inputSampleR += (bR[5] * (0.00724006282304610 + (0.00045483683460812 * fabs(bR[5]))));
                inputSampleR -= (bR[6] * (0.03073108963506036 - (0.00038190060537423 * fabs(bR[6]))));
                inputSampleR += (bR[7] * (0.02332434692533051 + (0.00040347288688932 * fabs(bR[7]))));
                inputSampleR -= (bR[8] * (0.03792606869061214 - (0.00039673687335892 * fabs(bR[8]))));
                inputSampleR += (bR[9] * (0.02437059376675688 + (0.00037221210539535 * fabs(bR[9]))));
                inputSampleR -= (bR[10] * (0.03416764311979521 - (0.00040314850796953 * fabs(bR[10]))));
                inputSampleR += (bR[11] * (0.01761669868102127 + (0.00035989484330131 * fabs(bR[11]))));
                inputSampleR -= (bR[12] * (0.02538237753523052 - (0.00040149119125394 * fabs(bR[12]))));
                inputSampleR += (bR[13] * (0.00770737340728377 + (0.00035462118723555 * fabs(bR[13]))));
                inputSampleR -= (bR[14] * (0.01580706228482803 - (0.00037563141307594 * fabs(bR[14]))));
                inputSampleR -= (bR[15] * (0.00055119240005586 - (0.00035409299268971 * fabs(bR[15]))));
                inputSampleR -= (bR[16] * (0.00818552143438768 - (0.00036507661042180 * fabs(bR[16]))));
                inputSampleR -= (bR[17] * (0.00661842703548304 - (0.00034550528559056 * fabs(bR[17]))));
                inputSampleR -= (bR[18] * (0.00362447476272098 - (0.00035553012761240 * fabs(bR[18]))));
                inputSampleR -= (bR[19] * (0.00957098027225745 - (0.00034091691045338 * fabs(bR[19]))));
                inputSampleR -= (bR[20] * (0.00193621774016660 - (0.00034554529131668 * fabs(bR[20]))));
                inputSampleR -= (bR[21] * (0.01005433027357935 - (0.00033878223153845 * fabs(bR[21]))));
                inputSampleR -= (bR[22] * (0.00221712428802004 - (0.00033481410137711 * fabs(bR[22]))));
                inputSampleR -= (bR[23] * (0.00911255639207995 - (0.00033263425232666 * fabs(bR[23]))));
                inputSampleR -= (bR[24] * (0.00339667169034909 - (0.00032634428038430 * fabs(bR[24]))));
                inputSampleR -= (bR[25] * (0.00774096948249924 - (0.00032599868802996 * fabs(bR[25]))));
                inputSampleR -= (bR[26] * (0.00463907626773794 - (0.00032131993173361 * fabs(bR[26]))));
                inputSampleR -= (bR[27] * (0.00658222997260378 - (0.00032014977430211 * fabs(bR[27]))));
                inputSampleR -= (bR[28] * (0.00550347079924993 - (0.00031557153256653 * fabs(bR[28]))));
                inputSampleR -= (bR[29] * (0.00588754981375325 - (0.00032041307242303 * fabs(bR[29]))));
                inputSampleR -= (bR[30] * (0.00590293898419892 - (0.00030457857428714 * fabs(bR[30]))));
                inputSampleR -= (bR[31] * (0.00558952010441800 - (0.00030448053548086 * fabs(bR[31]))));
                inputSampleR -= (bR[32] * (0.00598183557634295 - (0.00030715064323181 * fabs(bR[32]))));
                inputSampleR -= (bR[33] * (0.00555223929714115 - (0.00030319367948553 * fabs(bR[33]))));
                // voxbox 40 db
            }
            if (fabs(inputSampleL) > threshold)
            {
                bridgerectifier = (fabs(inputSampleL) - threshold) * hardness;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardness;
                // do the sine factor, scale back to proper amount
                if (inputSampleL > 0) {
                    inputSampleL = bridgerectifier + threshold;
                }
                else {
                    inputSampleL = -(bridgerectifier + threshold);
                }
            }
            // otherwise we leave it untouched by the overdrive stuff
            if (fabs(inputSampleR) > threshold)
            {
                bridgerectifier = (fabs(inputSampleR) - threshold) * hardness;
                // skip flat area if any, scale to distortion limit
                if (bridgerectifier > breakup) {
                    bridgerectifier = breakup;
                }
                // max value for sine function, 'breakup' modeling for trashed console tone
                // more hardness = more solidness behind breakup modeling. more softness, more 'grunge' and sag
                bridgerectifier = sin(bridgerectifier) / hardness;
                // do the sine factor, scale back to proper amount
                if (inputSampleR > 0) {
                    inputSampleR = bridgerectifier + threshold;
                }
                else {
                    inputSampleR = -(bridgerectifier + threshold);
                }
            }
            // otherwise we leave it untouched by the overdrive stuff
            randy = ((double(fpdL) / UINT32_MAX) * 0.031);
            inputSampleL = ((inputSampleL * (1 - randy)) + (lastSampleL * randy)) * outlevel;
            lastSampleL = inputSampleL;
            randy = ((double(fpdR) / UINT32_MAX) * 0.031);
            inputSampleR = ((inputSampleR * (1 - randy)) + (lastSampleR * randy)) * outlevel;
            lastSampleR = inputSampleR;
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
} // namespace airwindohhs::luxor
