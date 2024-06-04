#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::crystal {

constexpr std::string_view k_name{ "Crystal" };
constexpr std::string_view k_short_description{
    "Crystal is a tone shaper and buss soft-clipper."
};
constexpr std::string_view k_long_description{
    "Crystal’s the first of the Character reissues, by request: I know there’s the possibility for this to become people’s favorite plugin, because it already is one user’s favorite buss plugin and he begged me to rerelease it with updated code and VST compatibility. This is the result. Tonally it’s exactly the same as the classic ‘magic’ Audio Unit, but it’s got the denormalization and noise shaping to the floating point buss of 2018 and beyond.The controls you’ll be interested in are Hardness and Personality. Hardness applies the same algorithm that was in ‘New Channel’: though it’s not a replacement for what made Channel special, it’s got its own uses. It lets you define the onset of clipping, whether soft-clip saturation or digital hard clipping. Though this dirties up the sound a little, it lets you dial the ‘fatness’ of the saturation effect and gives you a tonal parameter that no other Airwindows plugin gives you. Think of it as a slider for how much the roaring, overdriven midrange sticks out.Personality is a precursor to what became BussColors (and there are other flavors to come) but in Crystal it’s a little different. The BussColors algorithms are taken from hardware convolution impulses, and there’s a time-constant making the interpolation between ‘loud’ and ‘soft’ impulses happen over several samples. In the Character plugins, this didn’t happen. It was sample-by-sample, so on the one hand there was no dynamic behavior, just each sample got a fixed convolution behavior.On the other hand (and it took me a while to properly understand this) every convolution sample got its own, separate dynamic behavior. The curve was different for each one, so it became a more tightly controlled little kernel rather than a set of possible kernels. There are still people who swear these were the great ones, and I’ve learned to pay closer attention to such things.And the thing is, Crystal’s not using a hardware sample. Unlike anything in BussColors, Crystal’s using a data set that comes from doing a brickwall filter: if I remember correctly, two different ones at different Q/steepness, and then generating the dynamic behavior out of that. So it’s doing a treble-restricting EQ behavior (a FIR filter), but then it manipulates that. The question is, do you like what it does? Some people really, really liked this one. Not everything about it is in line with how I usually do things. That’s why it’s different. Maybe it’s right up your alley? Let your ears guide you, and have fun checking it out."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Crystal final : public Effect<T>
{
    double bL[35];
    double lastSampleL;
    double bR[35];
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    Crystal()
    {
        A = 0.7;
        B = 1.0;
        C = 0.333; // 0.0 to 3.0 range
        D = 1.0;
        for (int count = 0; count < 34; count++) {
            bL[count] = 0;
            bR[count] = 0;
        }
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            case kParamA: return 0.7;
            case kParamB: return 1.0;
            case kParamC: return 0.333;
            case kParamD: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "hardnss";
            case kParamB: return "personl";
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
            case kParamA: return "Hardnss";
            case kParamB: return "Personl";
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
            case kParamB: return std::to_string(B);
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
        double sqdrive = B;
        if (sqdrive > 1.0) {
            sqdrive *= sqdrive;
        }
        sqdrive = sqrt(sqdrive);
        double indrive = C * 3.0;
        if (indrive > 1.0) {
            indrive *= indrive;
        }
        indrive *= (1.0 - (0.1695 * sqdrive));
        // no gain loss of convolution for APIcolypse
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
                inputSampleL += (bL[1] * (0.38856694371895023 + (0.14001177830115491 * fabs(bL[1]))));
                inputSampleL -= (bL[2] * (0.17469488984546111 + (0.05204541941091459 * fabs(bL[2]))));
                inputSampleL += (bL[3] * (0.11643521461774288 - (0.01193121216518472 * fabs(bL[3]))));
                inputSampleL -= (bL[4] * (0.08874416268268183 - (0.05867502375036486 * fabs(bL[4]))));
                inputSampleL += (bL[5] * (0.07222999223073785 - (0.08519974113692971 * fabs(bL[5]))));
                inputSampleL -= (bL[6] * (0.06103207678880003 - (0.09230674983449150 * fabs(bL[6]))));
                inputSampleL += (bL[7] * (0.05277389277465404 - (0.08487342372497046 * fabs(bL[7]))));
                inputSampleL -= (bL[8] * (0.04631144388636078 - (0.06976851898821038 * fabs(bL[8]))));
                inputSampleL += (bL[9] * (0.04102721072495113 - (0.05337974329110802 * fabs(bL[9]))));
                inputSampleL -= (bL[10] * (0.03656047655964371 - (0.03990914278458497 * fabs(bL[10]))));
                inputSampleL += (bL[11] * (0.03268677450573373 - (0.03090433934018759 * fabs(bL[11]))));
                inputSampleL -= (bL[12] * (0.02926012259262895 - (0.02585223214266682 * fabs(bL[12]))));
                inputSampleL += (bL[13] * (0.02618257163789973 - (0.02326667039588473 * fabs(bL[13]))));
                inputSampleL -= (bL[14] * (0.02338568277879992 - (0.02167067760829789 * fabs(bL[14]))));
                inputSampleL += (bL[15] * (0.02082142324645262 - (0.02013392273267951 * fabs(bL[15]))));
                inputSampleL -= (bL[16] * (0.01845525966656259 - (0.01833038930966512 * fabs(bL[16]))));
                inputSampleL += (bL[17] * (0.01626113504980445 - (0.01631893218593511 * fabs(bL[17]))));
                inputSampleL -= (bL[18] * (0.01422084088669267 - (0.01427828125219885 * fabs(bL[18]))));
                inputSampleL += (bL[19] * (0.01231993595709338 - (0.01233991521342998 * fabs(bL[19]))));
                inputSampleL -= (bL[20] * (0.01054774630451013 - (0.01054774630542346 * fabs(bL[20]))));
                inputSampleL += (bL[21] * (0.00889548162355088 - (0.00889548162263755 * fabs(bL[21]))));
                inputSampleL -= (bL[22] * (0.00735749099304526 - (0.00735749099395860 * fabs(bL[22]))));
                inputSampleL += (bL[23] * (0.00592812350468000 - (0.00592812350376666 * fabs(bL[23]))));
            } // the Character plugins as individual processors did this. BussColors applies an averaging factor to produce
            // more of a consistent variation between soft and loud convolutions. For years I thought this code was a
            // mistake and did nothing, but in fact what it's doing is producing slightly different curves for every single
            // convolution kernel location: this will be true of the Character individual plugins as well.
            // calibrated to match gain through convolution and -0.3 correction
            if (sqdrive > 0.0) {
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
                inputSampleR += (bR[1] * (0.38856694371895023 + (0.14001177830115491 * fabs(bR[1]))));
                inputSampleR -= (bR[2] * (0.17469488984546111 + (0.05204541941091459 * fabs(bR[2]))));
                inputSampleR += (bR[3] * (0.11643521461774288 - (0.01193121216518472 * fabs(bR[3]))));
                inputSampleR -= (bR[4] * (0.08874416268268183 - (0.05867502375036486 * fabs(bR[4]))));
                inputSampleR += (bR[5] * (0.07222999223073785 - (0.08519974113692971 * fabs(bR[5]))));
                inputSampleR -= (bR[6] * (0.06103207678880003 - (0.09230674983449150 * fabs(bR[6]))));
                inputSampleR += (bR[7] * (0.05277389277465404 - (0.08487342372497046 * fabs(bR[7]))));
                inputSampleR -= (bR[8] * (0.04631144388636078 - (0.06976851898821038 * fabs(bR[8]))));
                inputSampleR += (bR[9] * (0.04102721072495113 - (0.05337974329110802 * fabs(bR[9]))));
                inputSampleR -= (bR[10] * (0.03656047655964371 - (0.03990914278458497 * fabs(bR[10]))));
                inputSampleR += (bR[11] * (0.03268677450573373 - (0.03090433934018759 * fabs(bR[11]))));
                inputSampleR -= (bR[12] * (0.02926012259262895 - (0.02585223214266682 * fabs(bR[12]))));
                inputSampleR += (bR[13] * (0.02618257163789973 - (0.02326667039588473 * fabs(bR[13]))));
                inputSampleR -= (bR[14] * (0.02338568277879992 - (0.02167067760829789 * fabs(bR[14]))));
                inputSampleR += (bR[15] * (0.02082142324645262 - (0.02013392273267951 * fabs(bR[15]))));
                inputSampleR -= (bR[16] * (0.01845525966656259 - (0.01833038930966512 * fabs(bR[16]))));
                inputSampleR += (bR[17] * (0.01626113504980445 - (0.01631893218593511 * fabs(bR[17]))));
                inputSampleR -= (bR[18] * (0.01422084088669267 - (0.01427828125219885 * fabs(bR[18]))));
                inputSampleR += (bR[19] * (0.01231993595709338 - (0.01233991521342998 * fabs(bR[19]))));
                inputSampleR -= (bR[20] * (0.01054774630451013 - (0.01054774630542346 * fabs(bR[20]))));
                inputSampleR += (bR[21] * (0.00889548162355088 - (0.00889548162263755 * fabs(bR[21]))));
                inputSampleR -= (bR[22] * (0.00735749099304526 - (0.00735749099395860 * fabs(bR[22]))));
                inputSampleR += (bR[23] * (0.00592812350468000 - (0.00592812350376666 * fabs(bR[23]))));
            } // the Character plugins as individual processors did this. BussColors applies an averaging factor to produce
            // more of a consistent variation between soft and loud convolutions. For years I thought this code was a
            // mistake and did nothing, but in fact what it's doing is producing slightly different curves for every single
            // convolution kernel location: this will be true of the Character individual plugins as well.
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
            } // otherwise we leave it untouched by the overdrive stuff
            // this is the notorious New Channel Density algorithm. It's much less popular than the original Density,
            // because it introduces a point where the saturation 'curve' changes from straight to curved.
            // People don't like these discontinuities, but you can use them for effect or to grit up the sound.
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
            } // otherwise we leave it untouched by the overdrive stuff
            // this is the notorious New Channel Density algorithm. It's much less popular than the original Density,
            // because it introduces a point where the saturation 'curve' changes from straight to curved.
            // People don't like these discontinuities, but you can use them for effect or to grit up the sound.
            randy = ((double(fpdL) / UINT32_MAX) * 0.022);
            bridgerectifier = ((inputSampleL * (1 - randy)) + (lastSampleL * randy)) * outlevel;
            lastSampleL = inputSampleL;
            inputSampleL = bridgerectifier; // applies a tiny 'fuzz' to highs: from original Crystal.
            randy = ((double(fpdR) / UINT32_MAX) * 0.022);
            bridgerectifier = ((inputSampleR * (1 - randy)) + (lastSampleR * randy)) * outlevel;
            lastSampleR = inputSampleR;
            inputSampleR = bridgerectifier; // applies a tiny 'fuzz' to highs: from original Crystal.
            // This is akin to the old Chrome Oxide plugin, applying a fuzz to only the slews. The noise only appears
            // when current and old samples are different from each other, otherwise you can't tell it's there.
            // This is not only during silence but the tops of low frequency waves: it scales down to affect lows more gently.
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
} // namespace airwindohhs::crystal
