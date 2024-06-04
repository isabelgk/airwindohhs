#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::srsly2 {

constexpr std::string_view k_name{ "Srsly2" };
constexpr std::string_view k_short_description{
    "Srsly2 is a revisit of Srsly, to make the stereo widening more extreme."
};
constexpr std::string_view k_long_description{
    "“You call that a wide? Now THIS is a wide!”A little while ago, I put out a plugin that reverse engineered the famous Hughes SRS stereo widener, from pictures in an old Popular Mechanics article. By applying a set of narrow little EQ boosts and cuts to stereo, mid and side channels, you could make a sort of holographic effect. Srsly still exists, just as it was, for use tweaking out more natural stereo imagery.But the rabbit hole goes a bit deeper than just that…Srsly was by request of my friend Chad whose Hughes SRS wasn’t working properly, and who wanted a plugin version that didn’t hum. I didn’t have one of my own, so it was largely guesswork. Thing is, somewhere in there I got my hands on one (thanks Patreon! Between that and getting a real Mackie 1202 to play with, it turns out it’s useful for me to get actual gear relevant to my plugin interests, especially when I’m not getting the plugin right at first)And before I used it myself, I didn’t really ‘get it’, but then I started putting it on reverb returns, and quickly got very fond of a certain ultra-wide reverb field.And then I got more heavily into mixing in the box (and not with my hardware stuff) after Console7 came out… and discovered that my ITB reverbs did NOT do that kind of wide, and tried out my original Srsly… and had the same problem Chad ran into. It just didn’t do what the hardware box did. But I wasn’t done… so I started running stuff into the real hardware box, and just fooling around with the specific audio I’d begun to use, and rapidly worked out what was happening. My original Srsly left out a lot. It was more ‘audiophile’, more subtle, would fit in with more accurate recordings, but the real deal hardware device could be pushed WAY farther.…in a way that I could interpret. And coding ensued…Meet Srsly2. I’ve intentionally not tried super hard to exactly duplicate what is, after all, an unobtainable original hardware box by Howard Hughes’ company. Variations of this are still being licensed for use in car stereos and things, and I intentionally make no claim that I’m duplicating someone else’s property.But. But. BUT. What I was asked for, was to accomplish a particular effect, where the stereo wideness could be made crazy exaggerated. And I was able to interpret what a real hardware box (not original, though) was doing. And I continued to modify Srsly until, with Srsly2, you can now dial it in to do very similar crazy and unreasonable things… and that’s probably close enough for a free and open source plugin modeling an ancient hardware device that can’t really be found anymore. You’ll find the controls ought to work as you’d expect them to, and you may find as I did that leaving the Center control alone and cranking up the Space control just right, can get you into a wild and somewhat boosted and hyped zone that makes the most of your spectacular stereo content, in much the same way the original, obscure, Hughes box did.That’s my hope, anyhow. Hope you like it! I know I’ll be using it on stuff."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class Srsly2 final : public Effect<T>
{
    double biquadM2[11];
    double biquadM7[11];
    double biquadM10[11];
    double biquadL3[11];
    double biquadL7[11];
    double biquadR3[11];
    double biquadR7[11];
    double biquadS3[11];
    double biquadS5[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Srsly2()
    {
        for (int x = 0; x < 11; x++) {
            biquadM2[x] = 0.0;
            biquadM7[x] = 0.0;
            biquadM10[x] = 0.0;
            biquadL3[x] = 0.0;
            biquadL7[x] = 0.0;
            biquadR3[x] = 0.0;
            biquadR7[x] = 0.0;
            biquadS3[x] = 0.0;
            biquadS5[x] = 0.0;
        }
        A = 0.5;
        B = 0.5;
        C = 1.0;
        D = 0.5;
        E = 1.0;
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
        kParamfor (int x = 0,
kParambiquadM2[x] = 1,
kParambiquadM7[x] = 2,
kParambiquadM10[x] = 3,
kParambiquadL3[x] = 4,
kNumParameters = 5

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: for (int x = value; break;
case kParambiquadM2[x]: biquadM2[x] = value; break;
case kParambiquadM7[x]: biquadM7[x] = value; break;
case kParambiquadM10[x]: biquadM10[x] = value; break;
case kParambiquadL3[x]: biquadL3[x] = value; break;

        default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return for (int x;
case kParambiquadM2[x]: return biquadM2[x];
case kParambiquadM7[x]: return biquadM7[x];
case kParambiquadM10[x]: return biquadM10[x];
case kParambiquadL3[x]: return biquadL3[x];

        default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return 0;
        case kParambiquadM2[x]: return 0.0;
        case kParambiquadM7[x]: return 0.0;
        case kParambiquadM10[x]: return 0.0;
        case kParambiquadL3[x]: return 0.0;

        default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "center";
        case kParambiquadM2[x]: return "space";
        case kParambiquadM7[x]: return "level";
        case kParambiquadM10[x]: return "q";
        case kParambiquadL3[x]: return "drywet";

        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "Center";
        case kParambiquadM2[x]: return "Space";
        case kParambiquadM7[x]: return "Level";
        case kParambiquadM10[x]: return "Q";
        case kParambiquadL3[x]: return "Dry/Wet";

        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return std::to_string(A);
        case kParambiquadM2[x]: return std::to_string(B);
        case kParambiquadM7[x]: return std::to_string(C);
        case kParambiquadM10[x]: return std::to_string(D);
        case kParambiquadL3[x]: return std::to_string(E);

        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "";
        case kParambiquadM2[x]: return "";
        case kParambiquadM7[x]: return "";
        case kParambiquadM10[x]: return "";
        case kParambiquadL3[x]: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double sampleRate = getSampleRate();
        if (sampleRate < 22000) {
            sampleRate = 22000; // keep biquads in range
        }
        double tempSample;
        biquadM2[0] = 2000 / sampleRate; // up
        biquadM7[0] = 7000 / sampleRate; // down
        biquadM10[0] = 10000 / sampleRate; // down
        biquadL3[0] = 3000 / sampleRate; // up
        biquadL7[0] = 7000 / sampleRate; // way up
        biquadR3[0] = 3000 / sampleRate; // up
        biquadR7[0] = 7000 / sampleRate; // way up
        biquadS3[0] = 3000 / sampleRate; // up
        biquadS5[0] = 5000 / sampleRate; // way down
        double focusM = 15.0 - (A * 10.0);
        double focusS = 21.0 - (B * 15.0);
        double Q = D + 0.25; // add Q control: from half to double intensity
        double gainM = A * 2.0;
        double gainS = B * 2.0;
        if (gainS > 1.0) {
            gainM /= gainS;
        }
        if (gainM > 1.0) {
            gainM = 1.0;
        }
        biquadM2[1] = focusM * 0.25 * Q; // Q, mid 2K boost is much broader
        biquadM7[1] = focusM * Q; // Q
        biquadM10[1] = focusM * Q; // Q
        biquadS3[1] = focusM * Q; // Q
        biquadS5[1] = focusM * Q; // Q
        biquadL3[1] = focusS * Q; // Q
        biquadL7[1] = focusS * Q; // Q
        biquadR3[1] = focusS * Q; // Q
        biquadR7[1] = focusS * Q; // Q
        double K = tan(M_PI * biquadM2[0]);
        double norm = 1.0 / (1.0 + K / biquadM2[1] + K * K);
        biquadM2[2] = K / biquadM2[1] * norm;
        biquadM2[4] = -biquadM2[2];
        biquadM2[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM2[6] = (1.0 - K / biquadM2[1] + K * K) * norm;
        K = tan(M_PI * biquadM7[0]);
        norm = 1.0 / (1.0 + K / biquadM7[1] + K * K);
        biquadM7[2] = K / biquadM7[1] * norm;
        biquadM7[4] = -biquadM7[2];
        biquadM7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM7[6] = (1.0 - K / biquadM7[1] + K * K) * norm;
        K = tan(M_PI * biquadM10[0]);
        norm = 1.0 / (1.0 + K / biquadM10[1] + K * K);
        biquadM10[2] = K / biquadM10[1] * norm;
        biquadM10[4] = -biquadM10[2];
        biquadM10[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM10[6] = (1.0 - K / biquadM10[1] + K * K) * norm;
        K = tan(M_PI * biquadL3[0]);
        norm = 1.0 / (1.0 + K / biquadL3[1] + K * K);
        biquadL3[2] = K / biquadL3[1] * norm;
        biquadL3[4] = -biquadL3[2];
        biquadL3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadL3[6] = (1.0 - K / biquadL3[1] + K * K) * norm;
        K = tan(M_PI * biquadL7[0]);
        norm = 1.0 / (1.0 + K / biquadL7[1] + K * K);
        biquadL7[2] = K / biquadL7[1] * norm;
        biquadL7[4] = -biquadL7[2];
        biquadL7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadL7[6] = (1.0 - K / biquadL7[1] + K * K) * norm;
        K = tan(M_PI * biquadR3[0]);
        norm = 1.0 / (1.0 + K / biquadR3[1] + K * K);
        biquadR3[2] = K / biquadR3[1] * norm;
        biquadR3[4] = -biquadR3[2];
        biquadR3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadR3[6] = (1.0 - K / biquadR3[1] + K * K) * norm;
        K = tan(M_PI * biquadR7[0]);
        norm = 1.0 / (1.0 + K / biquadR7[1] + K * K);
        biquadR7[2] = K / biquadR7[1] * norm;
        biquadR7[4] = -biquadR7[2];
        biquadR7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadR7[6] = (1.0 - K / biquadR7[1] + K * K) * norm;
        K = tan(M_PI * biquadS3[0]);
        norm = 1.0 / (1.0 + K / biquadS3[1] + K * K);
        biquadS3[2] = K / biquadS3[1] * norm;
        biquadS3[4] = -biquadS3[2];
        biquadS3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadS3[6] = (1.0 - K / biquadS3[1] + K * K) * norm;
        K = tan(M_PI * biquadS5[0]);
        norm = 1.0 / (1.0 + K / biquadS5[1] + K * K);
        biquadS5[2] = K / biquadS5[1] * norm;
        biquadS5[4] = -biquadS5[2];
        biquadS5[5] = 2.0 * (K * K - 1.0) * norm;
        biquadS5[6] = (1.0 - K / biquadS5[1] + K * K) * norm;
        double depthM = pow(A, 2) * 2.0;
        ; // proportion to mix in the filtered stuff
        double depthS = pow(B, 2) * 2.0;
        ; // proportion to mix in the filtered stuff
        double level = C; // output pad
        double wet = E; // dry/wet
        // biquad contains these values:
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        //[2] is a0 but you need distinct ones for additional biquad instances so it's here
        //[3] is a1 but you need distinct ones for additional biquad instances so it's here
        //[4] is a2 but you need distinct ones for additional biquad instances so it's here
        //[5] is b1 but you need distinct ones for additional biquad instances so it's here
        //[6] is b2 but you need distinct ones for additional biquad instances so it's here
        //[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
        //[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double mid = inputSampleL + inputSampleR;
            double rawmid = mid * 0.5; // we'll use this to isolate L&R a little
            double side = inputSampleL - inputSampleR;
            double boostside = side * depthS;
            // assign mid and side.Between these sections, you can do mid/side processing
            tempSample = (mid * biquadM2[2]) + biquadM2[7];
            biquadM2[7] = (-tempSample * biquadM2[5]) + biquadM2[8];
            biquadM2[8] = (mid * biquadM2[4]) - (tempSample * biquadM2[6]);
            double M2Sample = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (mid * biquadM7[2]) + biquadM7[7];
            biquadM7[7] = (-tempSample * biquadM7[5]) + biquadM7[8];
            biquadM7[8] = (mid * biquadM7[4]) - (tempSample * biquadM7[6]);
            double M7Sample = -tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            tempSample = (mid * biquadM10[2]) + biquadM10[7];
            biquadM10[7] = (-tempSample * biquadM10[5]) + biquadM10[8];
            biquadM10[8] = (mid * biquadM10[4]) - (tempSample * biquadM10[6]);
            double M10Sample = -tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            // mid
            tempSample = (side * biquadS3[2]) + biquadS3[7];
            biquadS3[7] = (-tempSample * biquadS3[5]) + biquadS3[8];
            biquadS3[8] = (side * biquadS3[4]) - (tempSample * biquadS3[6]);
            double S3Sample = tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            tempSample = (side * biquadS5[2]) + biquadS5[7];
            biquadS5[7] = (-tempSample * biquadS5[5]) + biquadS5[8];
            biquadS5[8] = (side * biquadS5[4]) - (tempSample * biquadS5[6]);
            double S5Sample = -tempSample * 5.0; // like mono AU, 7 and 8 store L channel
            mid = (M2Sample + M7Sample + M10Sample) * depthM;
            side = (S3Sample + S5Sample + boostside) * depthS;
            double msOutSampleL = (mid + side) / 2.0;
            double msOutSampleR = (mid - side) / 2.0;
            // unassign mid and side
            double isoSampleL = inputSampleL - rawmid;
            double isoSampleR = inputSampleR - rawmid; // trying to isolate L and R a little
            tempSample = (isoSampleL * biquadL3[2]) + biquadL3[7];
            biquadL3[7] = (-tempSample * biquadL3[5]) + biquadL3[8];
            biquadL3[8] = (isoSampleL * biquadL3[4]) - (tempSample * biquadL3[6]);
            double L3Sample = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (isoSampleR * biquadR3[2]) + biquadR3[9];
            biquadR3[9] = (-tempSample * biquadR3[5]) + biquadR3[10];
            biquadR3[10] = (isoSampleR * biquadR3[4]) - (tempSample * biquadR3[6]);
            double R3Sample = tempSample; // note: 9 and 10 store the R channel
            tempSample = (isoSampleL * biquadL7[2]) + biquadL7[7];
            biquadL7[7] = (-tempSample * biquadL7[5]) + biquadL7[8];
            biquadL7[8] = (isoSampleL * biquadL7[4]) - (tempSample * biquadL7[6]);
            double L7Sample = tempSample * 3.0; // like mono AU, 7 and 8 store L channel
            tempSample = (isoSampleR * biquadR7[2]) + biquadR7[9];
            biquadR7[9] = (-tempSample * biquadR7[5]) + biquadR7[10];
            biquadR7[10] = (isoSampleR * biquadR7[4]) - (tempSample * biquadR7[6]);
            double R7Sample = tempSample * 3.0; // note: 9 and 10 store the R channel
            double processingL = msOutSampleL + ((L3Sample + L7Sample) * depthS);
            double processingR = msOutSampleR + ((R3Sample + R7Sample) * depthS);
            // done with making filters, now we apply them
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            // re-assign mid and side.Between these sections, you can do mid/side processing
            mid *= gainM;
            side *= gainS;
            // we crank things up more than a bit, or cut them in line with how the hardware box works
            if (side > 1.57079633) {
                side = 1.57079633;
            }
            if (side < -1.57079633) {
                side = -1.57079633;
            }
            side = sin(side);
            side *= gainS;
            inputSampleL = ((mid + side) / 2.0) + processingL;
            inputSampleR = ((mid - side) / 2.0) + processingR;
            // unassign mid and side
            if (level < 1.0) {
                inputSampleL *= level;
                inputSampleR *= level;
            }
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::srsly2
