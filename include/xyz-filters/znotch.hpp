#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::znotch {

constexpr std::string_view k_name{ "ZNotch" };
constexpr std::string_view k_short_description{
    "ZNotch is a notch filter made to sound and act like the Emu e6400 Phaser."
};
constexpr std::string_view k_long_description{
    "And finally, the fourth filter type (I’m doing basic filters, not trying to compete with real Z-Plane releases), ZNotch!This is made to sound like the Emu Phaser, which has notchlike qualities. At heart it’s an extension of the Airwindows Z series, not anything specifically Emu Z-Plane: I didn’t find a notch specifically in the Emu e6400 Ultra, but I thought the Phaser options were very notchlike, so I went for a take on those. You’ll get constant highs, the ability to notch out quite deep into the bass, and that overdrive that’s on tap in all the Airwindows Z plugins, plus the ability to do all that and then add that color, subtly, to the dry signal (do this by getting your distorted and notched tone just right, setting Poles to zero, turning the output up all the way and then bringing in just enough of the color that you can hear what it’s doing. Adjust to taste)This concludes the DnB-inspired sampler emulation EQs. Hopefully this bank of four Z-plugins will be handy to reach for, in the box, to get those vivid tones and grinds… I suspect I’ll find ’em useful in the place of more ‘normal’ EQs simply because the character of the distortion will be so handy. You don’t have to distort them, but since they take a little overdrive so nicely they’ll serve a purpose in all kinds of slight overdrive, or character adding, situations. And since they’re the Z series, you’ll know where to find them even if you’ve installed ALL the Airwindows plugins (and you can do that… if you dare!)"
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class ZNotch final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double biquadD[15];
    double biquadE[15];
    double biquadF[15];
    double iirSampleAL;
    double iirSampleAR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    ZNotch()
    {
        A = 0.1;
        B = 0.5;
        C = 1.0;
        D = 0.5;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
            biquadE[x] = 0.0;
            biquadF[x] = 0.0;
        }
        iirSampleAL = 0.0;
        iirSampleAR = 0.0;
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
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;

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
            case kParamC: return 1.0; break;
            case kParamD: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;
            case kParamB: return "freq"; break;
            case kParamC: return "output"; break;
            case kParamD: return "poles"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;
            case kParamB: return "Freq"; break;
            case kParamC: return "Output"; break;
            case kParamD: return "Poles"; break;

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
        biquadA[0] = ((pow(B, 3) * 4700.0) / Effect<T>::getSampleRate()) + 0.0009963;
        double clipFactor = 0.91 - ((1.0 - B) * 0.15);
        biquadA[1] = 0.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]);
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = (1.0 + K * K) * norm;
        biquadA[3] = 2.0 * (K * K - 1) * norm;
        biquadA[4] = biquadA[2];
        biquadA[5] = biquadA[3];
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        for (int x = 0; x < 7; x++) {
            biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x];
        }
        // opamp stuff
        double inTrim = A * 10.0;
        inTrim *= inTrim;
        inTrim *= inTrim;
        double outPad = C * 10.0;
        double iirAmountA = 0.00069 / overallscale;
        biquadF[0] = biquadE[0] = 15500.0 / Effect<T>::getSampleRate();
        biquadF[1] = biquadE[1] = 0.935;
        K = tan(M_PI * biquadE[0]); // lowpass
        norm = 1.0 / (1.0 + K / biquadE[1] + K * K);
        biquadE[2] = K * K * norm;
        biquadE[3] = 2.0 * biquadE[2];
        biquadE[4] = biquadE[2];
        biquadE[5] = 2.0 * (K * K - 1.0) * norm;
        biquadE[6] = (1.0 - K / biquadE[1] + K * K) * norm;
        for (int x = 0; x < 7; x++) {
            biquadF[x] = biquadE[x];
        }
        // end opamp stuff
        double trim = 0.1 + (3.712 * biquadA[0]);
        double wet = pow(D, 2);
        double aWet = 1.0;
        double bWet = 1.0;
        double cWet = 1.0;
        double dWet = wet * 4.0;
        // four-stage wet/dry control using progressive stages that bypass when not engaged
        if (dWet < 1.0) {
            aWet = dWet;
            bWet = 0.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 2.0) {
            bWet = dWet - 1.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 3.0) {
            cWet = dWet - 2.0;
            dWet = 0.0;
        }
        else {
            dWet -= 3.0;
        }
        // this is one way to make a little set of dry/wet stages that are successively added to the
        // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        // beyond that point: this is a way to progressively add a 'black box' sound processing
        // which lets you fall through to simpler processing at lower settings.
        double outSample = 0.0;
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
            double overallDrySampleL = drySampleL;
            double overallDrySampleR = drySampleR;
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
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
            inputSampleL *= trim;
            inputSampleR *= trim;
            inputSampleL /= clipFactor;
            outSample = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            biquadA[10] = biquadA[9];
            if (outSample > 1.0) {
                outSample = 1.0;
            }
            if (outSample < -1.0) {
                outSample = -1.0;
            }
            drySampleL = inputSampleL = biquadA[9] = outSample; // DF1
            inputSampleR /= clipFactor;
            outSample = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            biquadA[14] = biquadA[13];
            if (outSample > 1.0) {
                outSample = 1.0;
            }
            if (outSample < -1.0) {
                outSample = -1.0;
            }
            drySampleR = inputSampleR = biquadA[13] = outSample; // DF1
            if (bWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
                biquadB[8] = biquadB[7];
                biquadB[7] = inputSampleL;
                biquadB[10] = biquadB[9];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadB[9] = outSample; // DF1
                drySampleL = inputSampleL = (outSample * bWet) + (drySampleL * (1.0 - bWet));
                inputSampleR /= clipFactor;
                outSample = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
                biquadB[12] = biquadB[11];
                biquadB[11] = inputSampleR;
                biquadB[14] = biquadB[13];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadB[13] = outSample; // DF1
                drySampleR = inputSampleR = (outSample * bWet) + (drySampleR * (1.0 - bWet));
            }
            if (cWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
                biquadC[8] = biquadC[7];
                biquadC[7] = inputSampleL;
                biquadC[10] = biquadC[9];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadC[9] = outSample; // DF1
                drySampleL = inputSampleL = (outSample * cWet) + (drySampleL * (1.0 - cWet));
                inputSampleR /= clipFactor;
                outSample = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
                biquadC[12] = biquadC[11];
                biquadC[11] = inputSampleR;
                biquadC[14] = biquadC[13];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadC[13] = outSample; // DF1
                drySampleR = inputSampleR = (outSample * cWet) + (drySampleR * (1.0 - cWet));
            }
            if (dWet > 0.0) {
                inputSampleL /= clipFactor;
                outSample = biquadD[2] * inputSampleL + biquadD[3] * biquadD[7] + biquadD[4] * biquadD[8] - biquadD[5] * biquadD[9] - biquadD[6] * biquadD[10];
                biquadD[8] = biquadD[7];
                biquadD[7] = inputSampleL;
                biquadD[10] = biquadD[9];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadD[9] = outSample; // DF1
                drySampleL = inputSampleL = (outSample * dWet) + (drySampleL * (1.0 - dWet));
                inputSampleR /= clipFactor;
                outSample = biquadD[2] * inputSampleR + biquadD[3] * biquadD[11] + biquadD[4] * biquadD[12] - biquadD[5] * biquadD[13] - biquadD[6] * biquadD[14];
                biquadD[12] = biquadD[11];
                biquadD[11] = inputSampleR;
                biquadD[14] = biquadD[13];
                if (outSample > 1.0) {
                    outSample = 1.0;
                }
                if (outSample < -1.0) {
                    outSample = -1.0;
                }
                biquadD[13] = outSample; // DF1
                drySampleR = inputSampleR = (outSample * dWet) + (drySampleR * (1.0 - dWet));
            }
            inputSampleL /= clipFactor;
            inputSampleR /= clipFactor;
            // opamp stage
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
            outSample = biquadE[2] * inputSampleL + biquadE[3] * biquadE[7] + biquadE[4] * biquadE[8] - biquadE[5] * biquadE[9] - biquadE[6] * biquadE[10];
            biquadE[8] = biquadE[7];
            biquadE[7] = inputSampleL;
            inputSampleL = outSample;
            biquadE[10] = biquadE[9];
            biquadE[9] = inputSampleL; // DF1 left
            outSample = biquadE[2] * inputSampleR + biquadE[3] * biquadE[11] + biquadE[4] * biquadE[12] - biquadE[5] * biquadE[13] - biquadE[6] * biquadE[14];
            biquadE[12] = biquadE[11];
            biquadE[11] = inputSampleR;
            inputSampleR = outSample;
            biquadE[14] = biquadE[13];
            biquadE[13] = inputSampleR; // DF1 right
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL -= (inputSampleL * inputSampleL * inputSampleL * inputSampleL * inputSampleL * 0.1768);
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR -= (inputSampleR * inputSampleR * inputSampleR * inputSampleR * inputSampleR * 0.1768);
            outSample = biquadF[2] * inputSampleL + biquadF[3] * biquadF[7] + biquadF[4] * biquadF[8] - biquadF[5] * biquadF[9] - biquadF[6] * biquadF[10];
            biquadF[8] = biquadF[7];
            biquadF[7] = inputSampleL;
            inputSampleL = outSample;
            biquadF[10] = biquadF[9];
            biquadF[9] = inputSampleL; // DF1 left
            outSample = biquadF[2] * inputSampleR + biquadF[3] * biquadF[11] + biquadF[4] * biquadF[12] - biquadF[5] * biquadF[13] - biquadF[6] * biquadF[14];
            biquadF[12] = biquadF[11];
            biquadF[11] = inputSampleR;
            inputSampleR = outSample;
            biquadF[14] = biquadF[13];
            biquadF[13] = inputSampleR; // DF1 right
            if (outPad != 1.0) {
                inputSampleL *= outPad;
                inputSampleR *= outPad;
            }
            // end opamp stage
            if (aWet != 1.0) {
                inputSampleL = (inputSampleL * aWet) + (overallDrySampleL * (1.0 - aWet));
                inputSampleR = (inputSampleR * aWet) + (overallDrySampleR * (1.0 - aWet));
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::znotch
