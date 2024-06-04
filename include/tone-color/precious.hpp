#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::precious {

constexpr std::string_view k_name{ "Precious" };
constexpr std::string_view k_short_description{
    "Precious is a re-release of another old Character plugin."
};
constexpr std::string_view k_long_description{
    "So this is pretty straightforward: another Character re-issue, now free and open source and available in VST form. Woot!Might serve as a good color-adder, or fake guitar amp? Based on Precision 8 impulses, but it’s turned into very much its own thing: I think this became Holo in the Busscolors plugins. In the original form, here, you can crank up the color intensity a lot, or distort the hell out of it."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Precious final : public Effect<T>
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
    Precious()
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
        indrive *= (1.0 - (0.2095 * sqdrive));
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
                inputSampleL += (bL[1] * (0.59188440274551890 - (0.00008361469668405 * fabs(bL[1]))));
                inputSampleL -= (bL[2] * (0.24439750948076133 + (0.00002651678396848 * fabs(bL[2]))));
                inputSampleL += (bL[3] * (0.14109876103205621 - (0.00000840487181372 * fabs(bL[3]))));
                inputSampleL -= (bL[4] * (0.10053507128157971 + (0.00001768100964598 * fabs(bL[4]))));
                inputSampleL += (bL[5] * (0.05859287880626238 - (0.00000361398065989 * fabs(bL[5]))));
                inputSampleL -= (bL[6] * (0.04337406889823660 + (0.00000735941182117 * fabs(bL[6]))));
                inputSampleL += (bL[7] * (0.01589900680531097 + (0.00000207347387987 * fabs(bL[7]))));
                inputSampleL -= (bL[8] * (0.01087234854973281 + (0.00000732123412029 * fabs(bL[8]))));
                inputSampleL -= (bL[9] * (0.00845782429679176 - (0.00000133058605071 * fabs(bL[9]))));
                inputSampleL += (bL[10] * (0.00662278586618295 - (0.00000424594730611 * fabs(bL[10]))));
                inputSampleL -= (bL[11] * (0.02000592193760155 + (0.00000632896879068 * fabs(bL[11]))));
                inputSampleL += (bL[12] * (0.01321157777167565 - (0.00001421171592570 * fabs(bL[12]))));
                inputSampleL -= (bL[13] * (0.02249955362988238 + (0.00000163937127317 * fabs(bL[13]))));
                inputSampleL += (bL[14] * (0.01196492077581504 - (0.00000535385220676 * fabs(bL[14]))));
                inputSampleL -= (bL[15] * (0.01905917427000097 + (0.00000121672882030 * fabs(bL[15]))));
                inputSampleL += (bL[16] * (0.00761909482108073 - (0.00000326242895115 * fabs(bL[16]))));
                inputSampleL -= (bL[17] * (0.01362744780256239 + (0.00000359274216003 * fabs(bL[17]))));
                inputSampleL += (bL[18] * (0.00200183122683721 - (0.00000089207452791 * fabs(bL[18]))));
                inputSampleL -= (bL[19] * (0.00833042637239315 + (0.00000946767677294 * fabs(bL[19]))));
                inputSampleL -= (bL[20] * (0.00258481175207224 - (0.00000087429351464 * fabs(bL[20]))));
                inputSampleL -= (bL[21] * (0.00459744479712244 - (0.00000049519758701 * fabs(bL[21]))));
                inputSampleL -= (bL[22] * (0.00534277030993820 + (0.00000397547847155 * fabs(bL[22]))));
                inputSampleL -= (bL[23] * (0.00272332919605675 + (0.00000040077229097 * fabs(bL[23]))));
                inputSampleL -= (bL[24] * (0.00637243782359372 - (0.00000139419072176 * fabs(bL[24]))));
                inputSampleL -= (bL[25] * (0.00233001590327504 + (0.00000420129915747 * fabs(bL[25]))));
                inputSampleL -= (bL[26] * (0.00623296727793041 + (0.00000019010664856 * fabs(bL[26]))));
                inputSampleL -= (bL[27] * (0.00276177096376805 + (0.00000580301901385 * fabs(bL[27]))));
                inputSampleL -= (bL[28] * (0.00559184754866264 + (0.00000080597287792 * fabs(bL[28]))));
                inputSampleL -= (bL[29] * (0.00343180144395919 - (0.00000243701142085 * fabs(bL[29]))));
                inputSampleL -= (bL[30] * (0.00493325428861701 + (0.00000300985740900 * fabs(bL[30]))));
                inputSampleL -= (bL[31] * (0.00396140827680823 - (0.00000051459681789 * fabs(bL[31]))));
                inputSampleL -= (bL[32] * (0.00448497879902493 + (0.00000744412841743 * fabs(bL[32]))));
                inputSampleL -= (bL[33] * (0.00425146888772076 - (0.00000082346016542 * fabs(bL[33]))));
                // Precision 8
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
                inputSampleR += (bR[1] * (0.59188440274551890 - (0.00008361469668405 * fabs(bR[1]))));
                inputSampleR -= (bR[2] * (0.24439750948076133 + (0.00002651678396848 * fabs(bR[2]))));
                inputSampleR += (bR[3] * (0.14109876103205621 - (0.00000840487181372 * fabs(bR[3]))));
                inputSampleR -= (bR[4] * (0.10053507128157971 + (0.00001768100964598 * fabs(bR[4]))));
                inputSampleR += (bR[5] * (0.05859287880626238 - (0.00000361398065989 * fabs(bR[5]))));
                inputSampleR -= (bR[6] * (0.04337406889823660 + (0.00000735941182117 * fabs(bR[6]))));
                inputSampleR += (bR[7] * (0.01589900680531097 + (0.00000207347387987 * fabs(bR[7]))));
                inputSampleR -= (bR[8] * (0.01087234854973281 + (0.00000732123412029 * fabs(bR[8]))));
                inputSampleR -= (bR[9] * (0.00845782429679176 - (0.00000133058605071 * fabs(bR[9]))));
                inputSampleR += (bR[10] * (0.00662278586618295 - (0.00000424594730611 * fabs(bR[10]))));
                inputSampleR -= (bR[11] * (0.02000592193760155 + (0.00000632896879068 * fabs(bR[11]))));
                inputSampleR += (bR[12] * (0.01321157777167565 - (0.00001421171592570 * fabs(bR[12]))));
                inputSampleR -= (bR[13] * (0.02249955362988238 + (0.00000163937127317 * fabs(bR[13]))));
                inputSampleR += (bR[14] * (0.01196492077581504 - (0.00000535385220676 * fabs(bR[14]))));
                inputSampleR -= (bR[15] * (0.01905917427000097 + (0.00000121672882030 * fabs(bR[15]))));
                inputSampleR += (bR[16] * (0.00761909482108073 - (0.00000326242895115 * fabs(bR[16]))));
                inputSampleR -= (bR[17] * (0.01362744780256239 + (0.00000359274216003 * fabs(bR[17]))));
                inputSampleR += (bR[18] * (0.00200183122683721 - (0.00000089207452791 * fabs(bR[18]))));
                inputSampleR -= (bR[19] * (0.00833042637239315 + (0.00000946767677294 * fabs(bR[19]))));
                inputSampleR -= (bR[20] * (0.00258481175207224 - (0.00000087429351464 * fabs(bR[20]))));
                inputSampleR -= (bR[21] * (0.00459744479712244 - (0.00000049519758701 * fabs(bR[21]))));
                inputSampleR -= (bR[22] * (0.00534277030993820 + (0.00000397547847155 * fabs(bR[22]))));
                inputSampleR -= (bR[23] * (0.00272332919605675 + (0.00000040077229097 * fabs(bR[23]))));
                inputSampleR -= (bR[24] * (0.00637243782359372 - (0.00000139419072176 * fabs(bR[24]))));
                inputSampleR -= (bR[25] * (0.00233001590327504 + (0.00000420129915747 * fabs(bR[25]))));
                inputSampleR -= (bR[26] * (0.00623296727793041 + (0.00000019010664856 * fabs(bR[26]))));
                inputSampleR -= (bR[27] * (0.00276177096376805 + (0.00000580301901385 * fabs(bR[27]))));
                inputSampleR -= (bR[28] * (0.00559184754866264 + (0.00000080597287792 * fabs(bR[28]))));
                inputSampleR -= (bR[29] * (0.00343180144395919 - (0.00000243701142085 * fabs(bR[29]))));
                inputSampleR -= (bR[30] * (0.00493325428861701 + (0.00000300985740900 * fabs(bR[30]))));
                inputSampleR -= (bR[31] * (0.00396140827680823 - (0.00000051459681789 * fabs(bR[31]))));
                inputSampleR -= (bR[32] * (0.00448497879902493 + (0.00000744412841743 * fabs(bR[32]))));
                inputSampleR -= (bR[33] * (0.00425146888772076 - (0.00000082346016542 * fabs(bR[33]))));
                // Precision 8
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
            randy = ((double(fpdL) / UINT32_MAX) * 0.017);
            inputSampleL = ((inputSampleL * (1 - randy)) + (lastSampleL * randy)) * outlevel;
            lastSampleL = inputSampleL;
            randy = ((double(fpdR) / UINT32_MAX) * 0.017);
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
} // namespace airwindohhs::precious
