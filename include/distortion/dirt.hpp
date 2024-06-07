#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dirt {

constexpr std::string_view k_name{ "Dirt" };
constexpr std::string_view k_short_description{
    "Dirt is a soft-clip distortion in the spirit of Edge."
};
constexpr std::string_view k_long_description{
    "In the event that you’d like a softer, tubier Edge… I’ve got you covered.If you liked Airwindows Edge, this is a variation. It doesn’t go nearly as high gain but it’s got a much softer clip to it, and the same controls set up the same way (so highpass/lowpass settings ought to match if you want them to). My intent with it was to have a companion plugin to Edge for use with ITB guitars: I’d be using it with Cabs, but folks who need full-on IRs might try that too, or sandwiching your IR between Dirt and Cabs as a final tone/presentation tweak.I hope you like it… and not just on guitars. In line with my current ultrasonic filtering approach, you’ll get more mileage out of this and Edge at high sample rates, but the lowpass will let you get some space between you and aliasing no matter what sample rate you’re at."
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class Dirt final : public Effect<T>
{
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total
    }; // fixed frequency biquad filter for ultrasonics, stereo
    double fixA[fix_total];
    double fixB[fix_total];
    double fixC[fix_total];
    double fixD[fix_total];
    double fixE[fix_total];
    double fixF[fix_total];
    double fixG[fix_total];
    double iirSampleL;
    double iirSampleR;

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Dirt()
    {
        A = 0.1;
        B = 1.0;
        C = 0.0;
        D = 1.0;
        E = 1.0;
        for (int x = 0; x < fix_total; x++) {
            fixA[x] = 0.0;
            fixB[x] = 0.0;
            fixC[x] = 0.0;
            fixD[x] = 0.0;
            fixE[x] = 0.0;
            fixF[x] = 0.0;
            fixG[x] = 0.0;
        }
        iirSampleL = 0.0;
        iirSampleR = 0.0;
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
            case kParamB: return 1.0; break;
            case kParamC: return 0.0; break;
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
            case kParamA: return "gain"; break;
            case kParamB: return "lowpass"; break;
            case kParamC: return "highpass"; break;
            case kParamD: return "output"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain"; break;
            case kParamB: return "Lowpass"; break;
            case kParamC: return "Highpass"; break;
            case kParamD: return "Output"; break;
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
        double inTrim = ((1.0 - pow(1.0 - A, 2)) * 1.0) + 1.0;
        double cutoff = (B * 25000.0) / Effect<T>::getSampleRate();
        if (cutoff > 0.49) {
            cutoff = 0.49; // don't crash if run at 44.1k
        }
        if (cutoff < 0.001) {
            cutoff = 0.001; // or if cutoff's too low
        }
        double iirAmount = pow(C, 3) * 0.5;
        if (iirAmount < 0.00000001) {
            iirAmount = 0.00000001; // or if cutoff's too low
        }
        iirAmount /= overallscale; // highpass is very gentle
        double outPad = D;
        double wet = E;
        fixG[fix_freq] = fixF[fix_freq] = fixE[fix_freq] = fixD[fix_freq] = fixC[fix_freq] = fixB[fix_freq] = fixA[fix_freq] = cutoff;
        fixA[fix_reso] = 4.46570214;
        fixB[fix_reso] = 1.51387132;
        fixC[fix_reso] = 0.93979296;
        fixD[fix_reso] = 0.70710678;
        fixE[fix_reso] = 0.59051105;
        fixF[fix_reso] = 0.52972649;
        fixG[fix_reso] = 0.50316379;
        double K = tan(M_PI * fixA[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixB[fix_freq]);
        norm = 1.0 / (1.0 + K / fixB[fix_reso] + K * K);
        fixB[fix_a0] = K * K * norm;
        fixB[fix_a1] = 2.0 * fixB[fix_a0];
        fixB[fix_a2] = fixB[fix_a0];
        fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixB[fix_b2] = (1.0 - K / fixB[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixC[fix_freq]);
        norm = 1.0 / (1.0 + K / fixC[fix_reso] + K * K);
        fixC[fix_a0] = K * K * norm;
        fixC[fix_a1] = 2.0 * fixC[fix_a0];
        fixC[fix_a2] = fixC[fix_a0];
        fixC[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixC[fix_b2] = (1.0 - K / fixC[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixD[fix_freq]);
        norm = 1.0 / (1.0 + K / fixD[fix_reso] + K * K);
        fixD[fix_a0] = K * K * norm;
        fixD[fix_a1] = 2.0 * fixD[fix_a0];
        fixD[fix_a2] = fixD[fix_a0];
        fixD[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixD[fix_b2] = (1.0 - K / fixD[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixE[fix_freq]);
        norm = 1.0 / (1.0 + K / fixE[fix_reso] + K * K);
        fixE[fix_a0] = K * K * norm;
        fixE[fix_a1] = 2.0 * fixE[fix_a0];
        fixE[fix_a2] = fixE[fix_a0];
        fixE[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixE[fix_b2] = (1.0 - K / fixE[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixF[fix_freq]);
        norm = 1.0 / (1.0 + K / fixF[fix_reso] + K * K);
        fixF[fix_a0] = K * K * norm;
        fixF[fix_a1] = 2.0 * fixF[fix_a0];
        fixF[fix_a2] = fixF[fix_a0];
        fixF[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixF[fix_b2] = (1.0 - K / fixF[fix_reso] + K * K) * norm;
        K = tan(M_PI * fixG[fix_freq]);
        norm = 1.0 / (1.0 + K / fixG[fix_reso] + K * K);
        fixG[fix_a0] = K * K * norm;
        fixG[fix_a1] = 2.0 * fixG[fix_a0];
        fixG[fix_a2] = fixG[fix_a0];
        fixG[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixG[fix_b2] = (1.0 - K / fixG[fix_reso] + K * K) * norm;
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (fabs(iirSampleL) < 1.18e-33) {
                iirSampleL = 0.0;
            }
            iirSampleL = (iirSampleL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            inputSampleL = inputSampleL - iirSampleL;
            if (fabs(iirSampleR) < 1.18e-33) {
                iirSampleR = 0.0;
            }
            iirSampleR = (iirSampleR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            inputSampleR = inputSampleR - iirSampleR;
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
            double outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
            fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
            fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
            fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
            fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixC[fix_a0]) + fixC[fix_sL1];
            fixC[fix_sL1] = (inputSampleL * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sL2];
            fixC[fix_sL2] = (inputSampleL * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixC[fix_a0]) + fixC[fix_sR1];
            fixC[fix_sR1] = (inputSampleR * fixC[fix_a1]) - (outSample * fixC[fix_b1]) + fixC[fix_sR2];
            fixC[fix_sR2] = (inputSampleR * fixC[fix_a2]) - (outSample * fixC[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixD[fix_a0]) + fixD[fix_sL1];
            fixD[fix_sL1] = (inputSampleL * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sL2];
            fixD[fix_sL2] = (inputSampleL * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixD[fix_a0]) + fixD[fix_sR1];
            fixD[fix_sR1] = (inputSampleR * fixD[fix_a1]) - (outSample * fixD[fix_b1]) + fixD[fix_sR2];
            fixD[fix_sR2] = (inputSampleR * fixD[fix_a2]) - (outSample * fixD[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixE[fix_a0]) + fixE[fix_sL1];
            fixE[fix_sL1] = (inputSampleL * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sL2];
            fixE[fix_sL2] = (inputSampleL * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixE[fix_a0]) + fixE[fix_sR1];
            fixE[fix_sR1] = (inputSampleR * fixE[fix_a1]) - (outSample * fixE[fix_b1]) + fixE[fix_sR2];
            fixE[fix_sR2] = (inputSampleR * fixE[fix_a2]) - (outSample * fixE[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixF[fix_a0]) + fixF[fix_sL1];
            fixF[fix_sL1] = (inputSampleL * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sL2];
            fixF[fix_sL2] = (inputSampleL * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixF[fix_a0]) + fixF[fix_sR1];
            fixF[fix_sR1] = (inputSampleR * fixF[fix_a1]) - (outSample * fixF[fix_b1]) + fixF[fix_sR2];
            fixF[fix_sR2] = (inputSampleR * fixF[fix_a2]) - (outSample * fixF[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL - (inputSampleL * fabs(inputSampleL) * 0.5)) * inTrim;
            inputSampleR = (inputSampleR - (inputSampleR * fabs(inputSampleR) * 0.5)) * inTrim;
            outSample = (inputSampleL * fixG[fix_a0]) + fixG[fix_sL1];
            fixG[fix_sL1] = (inputSampleL * fixG[fix_a1]) - (outSample * fixG[fix_b1]) + fixG[fix_sL2];
            fixG[fix_sL2] = (inputSampleL * fixG[fix_a2]) - (outSample * fixG[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics L
            outSample = (inputSampleR * fixG[fix_a0]) + fixG[fix_sR1];
            fixG[fix_sR1] = (inputSampleR * fixG[fix_a1]) - (outSample * fixG[fix_b1]) + fixG[fix_sR2];
            fixG[fix_sR2] = (inputSampleR * fixG[fix_a2]) - (outSample * fixG[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics R
            inputSampleL = (inputSampleL * wet * outPad) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet * outPad) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::dirt
