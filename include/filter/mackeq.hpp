#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::mackeq {

constexpr std::string_view k_name{ "MackEQ" };
constexpr std::string_view k_short_description{
    "MackEQ is the Mackie distortion but with treble and bass controls added."
};
constexpr std::string_view k_long_description{
    "Folks who wanted Mackity, were looking to do some crazy things with it. We’re taking an old Mackie 1202, pre-VLZ (I have one and know how to abuse it for effect) and we’re mangling sounds with it. People wanted to take a kick drum, run it through one channel turned all the way up, patch that to another channel with crazy EQ and also turned up until ready to explode, and so on. Mackity was my best shot at just the input section of the 1202, with all the gain on tap but set up to work as close to ‘clean’ as these desks ever got: more so, in fact, as it doesn’t add hiss and chip noise. Mackity was really good at sounding exactly like that, for those who like patching out of the insert points and getting a reasonably hi-fi sound out of their old 90s mixer.There are plenty of people who know without the shadow of a doubt, that the very idea is ridiculous: that, compared to your Neves and APIs etc, these tiny budget mixers are garbage.They will HATE this plugin. This is the same thing only with the garbage EQs in the 1202, the original two-band version, in which you can also overdrive the op-amps inside the EQs for good measure. Nothing about this sounds nice. You might want to pad down the output if you try: it’s pretty horrifying.Some folks will go and immediately do that… and some of those, are very used to their original Mackie analog mixers, and know the exact tone they should get. And I can’t tell you whether those folks will be happy with MackEQ, because I have the real one (not an 8-buss, but a 1202) to compare with, and I did not get a perfect exact clone. I got something else. I think it might be useful: certainly it can get the correct TYPE of tone, but I don’t believe I have the true 100% 90s drum and bass madness exactly down. There’s a texture in there, especially when you start aggressively distorting highs, that just defies being captured in a plugin, much like you don’t really get a Marshall Plexi in the box.But I captured SOMETHING in a plugin, and it’s in the ballpark. If you can accept a slight re-voicing of the thing, or if your use of it doesn’t involve torturing hi-hats and such in the first place, you might find MackEQ is useful to you. That’s my hope. I daresay I’ll find uses for it myself… including, use after certain secret projects I’m still working on. Seems I’ve devoted myself to the DnB flame. If only I could play the music, I’d really be on to something :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class MackEQ final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double biquadD[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    MackEQ()
    {
        A = 0.1;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
        }
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
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.1; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "trim"; break;
            case kParamB: return "hi"; break;
            case kParamC: return "lo"; break;
            case kParamD: return "gain"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Trim"; break;
            case kParamB: return "Hi"; break;
            case kParamC: return "Lo"; break;
            case kParamD: return "Gain"; break;
            case kParamE: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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
        double inTrim = A * 10.0;
        inTrim *= inTrim;
        double gainHigh = pow(B, 2) * 4.0;
        double outHigh = sqrt(B);
        double gainBass = pow(C, 2) * 4.0;
        double outBass = sqrt(C);
        double outPad = D;
        double wet = E;
        double iirAmountA = 0.001860867 / overallscale;
        double iirAmountB = 0.000287496 / overallscale;
        double iirBassMid = 0.159 / overallscale;
        double iirMidHigh = 0.236 / overallscale;
        biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = 19160.0 / Effect<T>::getSampleRate();
        biquadA[1] = 0.431684981684982;
        biquadB[1] = 1.1582298;
        biquadC[1] = 0.657027382751269;
        biquadD[1] = 1.076210852946577;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        K = tan(M_PI * biquadD[0]);
        norm = 1.0 / (1.0 + K / biquadD[1] + K * K);
        biquadD[2] = K * K * norm;
        biquadD[3] = 2.0 * biquadD[2];
        biquadD[4] = biquadD[2];
        biquadD[5] = 2.0 * (K * K - 1.0) * norm;
        biquadD[6] = (1.0 - K / biquadD[1] + K * K) * norm;
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
            if (fabs(iirSampleAL) < 1.18e-37) {
                iirSampleAL = 0.0;
            }
            iirSampleAL = (iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
            inputSampleL -= iirSampleAL;
            if (fabs(iirSampleAR) < 1.18e-37) {
                iirSampleAR = 0.0;
            }
            iirSampleAR = (iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
            inputSampleR -= iirSampleAR;
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
            // begin Mackity input stage
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL -= pow(inputSampleL, 5) * 0.1768;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR -= pow(inputSampleR, 5) * 0.1768;
            outSampleL = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
            biquadB[8] = biquadB[7];
            biquadB[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadB[10] = biquadB[9];
            biquadB[9] = inputSampleL; // DF1 left
            outSampleR = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
            biquadB[12] = biquadB[11];
            biquadB[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadB[14] = biquadB[13];
            biquadB[13] = inputSampleR; // DF1 right
            if (fabs(iirSampleBL) < 1.18e-37) {
                iirSampleBL = 0.0;
            }
            iirSampleBL = (iirSampleBL * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
            inputSampleL -= iirSampleBL;
            if (fabs(iirSampleBR) < 1.18e-37) {
                iirSampleBR = 0.0;
            }
            iirSampleBR = (iirSampleBR * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
            inputSampleR -= iirSampleBR;
            // end Mackity input stage
            // begin EQ section
            if (fabs(iirSampleCL) < 1.18e-37) {
                iirSampleCL = 0.0;
            }
            iirSampleCL = (iirSampleCL * (1.0 - iirBassMid)) + (inputSampleL * iirBassMid);
            if (fabs(iirSampleCR) < 1.18e-37) {
                iirSampleCR = 0.0;
            }
            iirSampleCR = (iirSampleCR * (1.0 - iirBassMid)) + (inputSampleR * iirBassMid);
            double bassSampleL = iirSampleCL;
            double midSampleL = inputSampleL - bassSampleL;
            double bassSampleR = iirSampleCR;
            double midSampleR = inputSampleR - bassSampleR;
            if (gainBass != 1.0) {
                bassSampleL *= gainBass;
                bassSampleR *= gainBass;
            }
            if (bassSampleL > 1.0) {
                bassSampleL = 1.0;
            }
            if (bassSampleL < -1.0) {
                bassSampleL = -1.0;
            }
            bassSampleL -= pow(bassSampleL, 5) * 0.1768;
            if (bassSampleR > 1.0) {
                bassSampleR = 1.0;
            }
            if (bassSampleR < -1.0) {
                bassSampleR = -1.0;
            }
            bassSampleR -= pow(bassSampleR, 5) * 0.1768;
            if (fabs(iirSampleDL) < 1.18e-37) {
                iirSampleDL = 0.0;
            }
            iirSampleDL = (iirSampleDL * (1.0 - iirBassMid)) + (bassSampleL * iirBassMid);
            bassSampleL = iirSampleDL;
            if (fabs(iirSampleDR) < 1.18e-37) {
                iirSampleDR = 0.0;
            }
            iirSampleDR = (iirSampleDR * (1.0 - iirBassMid)) + (bassSampleR * iirBassMid);
            bassSampleR = iirSampleDR;
            // we've taken the bass sample, made the mids from it, distorted it
            // and hit it with another pole of darkening.
            // mid sample is still normal from undistorted bass
            if (fabs(iirSampleEL) < 1.18e-37) {
                iirSampleEL = 0.0;
            }
            iirSampleEL = (iirSampleEL * (1.0 - iirMidHigh)) + (midSampleL * iirMidHigh);
            double highSampleL = midSampleL - iirSampleEL;
            midSampleL = iirSampleEL;
            if (fabs(iirSampleER) < 1.18e-37) {
                iirSampleER = 0.0;
            }
            iirSampleER = (iirSampleER * (1.0 - iirMidHigh)) + (midSampleR * iirMidHigh);
            double highSampleR = midSampleR - iirSampleER;
            midSampleR = iirSampleER;
            // here is where we make the high sample out of the mid, and take highs
            // away from the mid.
            if (fabs(iirSampleFL) < 1.18e-37) {
                iirSampleFL = 0.0;
            }
            iirSampleFL = (iirSampleFL * (1.0 - iirMidHigh)) + (highSampleL * iirMidHigh);
            highSampleL -= iirSampleFL;
            if (fabs(iirSampleFR) < 1.18e-37) {
                iirSampleFR = 0.0;
            }
            iirSampleFR = (iirSampleFR * (1.0 - iirMidHigh)) + (highSampleR * iirMidHigh);
            highSampleR -= iirSampleFR;
            if (gainHigh != 1.0) {
                highSampleL *= gainHigh;
                highSampleR *= gainHigh;
            }
            if (highSampleL > 1.0) {
                highSampleL = 1.0;
            }
            if (highSampleL < -1.0) {
                highSampleL = -1.0;
            }
            highSampleL -= pow(highSampleL, 5) * 0.1768;
            if (highSampleR > 1.0) {
                highSampleR = 1.0;
            }
            if (highSampleR < -1.0) {
                highSampleR = -1.0;
            }
            highSampleR -= pow(highSampleR, 5) * 0.1768;
            // highpassing HighSample another stage, before distorting it
            inputSampleL = ((bassSampleL * outBass) + midSampleL + (highSampleL * outHigh)) * 4.0;
            inputSampleR = ((bassSampleR * outBass) + midSampleR + (highSampleR * outHigh)) * 4.0;
            // end EQ section
            outSampleL = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
            biquadC[8] = biquadC[7];
            biquadC[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadC[10] = biquadC[9];
            biquadC[9] = inputSampleL; // DF1 left
            outSampleR = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
            biquadC[12] = biquadC[11];
            biquadC[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadC[14] = biquadC[13];
            biquadC[13] = inputSampleR; // DF1 right
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL -= pow(inputSampleL, 5) * 0.1768;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR -= pow(inputSampleR, 5) * 0.1768;
            outSampleL = biquadD[2] * inputSampleL + biquadD[3] * biquadD[7] + biquadD[4] * biquadD[8] - biquadD[5] * biquadD[9] - biquadD[6] * biquadD[10];
            biquadD[8] = biquadD[7];
            biquadD[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadD[10] = biquadD[9];
            biquadD[9] = inputSampleL; // DF1 left
            outSampleR = biquadD[2] * inputSampleR + biquadD[3] * biquadD[11] + biquadD[4] * biquadD[12] - biquadD[5] * biquadD[13] - biquadD[6] * biquadD[14];
            biquadD[12] = biquadD[11];
            biquadD[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadD[14] = biquadD[13];
            biquadD[13] = inputSampleR; // DF1 right
            if (outPad != 1.0) {
                inputSampleL *= outPad;
                inputSampleR *= outPad;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::mackeq
