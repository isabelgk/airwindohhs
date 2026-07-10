#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::zacidlowpass {

constexpr std::string_view k_name{ "ZAcidLowpass" };
constexpr std::string_view k_short_description{
    "ZAcidLowpass is like if an e6400 went insane and grew shark teeth."
};
constexpr std::string_view k_long_description{
    "Okay, next week we can get normal :)This algorithm is less than a week old. It's what happened when I tried to extend my Quadratic Bezier Curve audio processing to Cubic Bezier Curve audio processing. And I'm not sure it's time to give up on that, but in the meantime, the experiment failed spectacularly.So, generating curves can follow various rules. The quadratic bezier curves are already breaking audio rules, as what they do has nothing to do with frequencies, or harmonics, or any audio concepts. They're drawing a continuous curve directed by the sample values and it turned out this is possible, but it doesn't have audio significance. It's quite good at handling control voltages, like in a compressor! But it's still kind of arbitrary, constructing a curve from the starting position of a previous sample.The cubic bezier curves proved much less manageable.In theory, you could get interesting results out of them since you get two independent control points, but it turned out weirder: there's this tendency to draw the curves, indeed make them be interesting curves with striking contours, but then jump around in the reconstruction. It'd not-randomly space out the curves with tiny straight lines like a sawtooth wave gone mad. Very pretty but never joining up the waves into something smooth and connected. The idea was to see where the extra control point took the reconstruction, but at least for now it took the reconstruction to crazy-town.And produced a horrible hybrid of a steep resonant ringy filter, and a ring modulator, and I don't know what else. I never heard anything like that, and so there was only one thing to do.Put the Z filters output stage on it so you could distort it wildly, and let people have it. What else?The cutoff is roughly what you'd expect, a frequency control. Over gives you highpassing, under gives you lowpassing, both gives you the original sound (this is a sort of crossover! somehow!). Meltdown does a specific thing: rather than feeding the filter with an averaged, darker input, you can feed it with the raw audio input and get aliasing and chaos fed into the points and control voltages, and this sounds wilder and dirtier. If you're using Over to produce highpassing, Meltdown is reversed, and setting it to 0 gives you the crazy overtones and harsh noises. Drive is like the other Z filters, and so is Output, and the distortion stage is the same as in the Z filters, so if you're using those and would like a contrast that's still tonally similar, this is the plugin for you.They can't all be ToTape9 :) if this suits you, have fun!"
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class ZAcidLowpass final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_DL,
        bez_DR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezA[bez_total];
    double biquadE[15];
    double biquadF[15];
    double iirSampleAL;
    double iirSampleAR;
    double cutoffA;
    double cutoffB;
    double overA;
    double overB;
    double underA;
    double underB;
    double meltdownA;
    double meltdownB;
    double inTrimA;
    double inTrimB;
    double outTrimA;
    double outTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ZAcidLowpass()
    {
        A = 0.5;
        B = 0.0;
        C = 1.0;
        D = 0.0;
        E = 0.1;
        F = 1.0;
        for (int x = 0; x < bez_total; x++) {
            bezA[x] = 0.0;
        }
        bezA[bez_cycle] = 1.0;
        for (int x = 0; x < 15; x++) {
            biquadE[x] = 0.0;
            biquadF[x] = 0.0;
        }
        iirSampleAL = 0.0;
        iirSampleAR = 0.0;
        cutoffA = 0.5;
        cutoffB = 0.5;
        overA = 0.0;
        overB = 0.0;
        underA = 1.0;
        underB = 1.0;
        meltdownA = 0.0;
        meltdownB = 0.0;
        inTrimA = 0.1;
        inTrimB = 0.1;
        outTrimA = 1.0;
        outTrimB = 1.0;
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
        kNumParameters = 6
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
            case kParamF: F = value; break;

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
            case kParamF: return F; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.1; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "cutoff"; break;
            case kParamB: return "over"; break;
            case kParamC: return "under"; break;
            case kParamD: return "meltdwn"; break;
            case kParamE: return "drive"; break;
            case kParamF: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Cutoff"; break;
            case kParamB: return "Over"; break;
            case kParamC: return "Under"; break;
            case kParamD: return "Meltdwn"; break;
            case kParamE: return "Drive"; break;
            case kParamF: return "Output"; break;

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
            case kParamF: return std::to_string(F); break;

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
            case kParamF: return ""; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        cutoffA = cutoffB;
        cutoffB = pow(A, 3.0) / overallscale;
        cutoffB /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
        if (cutoffB < 0.0001) {
            cutoffB = 0.0001;
        }
        if (cutoffB > 1.0) {
            cutoffB = 1.0;
        }
        overA = overB;
        overB = B;
        underA = underB;
        underB = C;
        meltdownA = meltdownB;
        meltdownB = D;
        // opamp stuff
        inTrimA = inTrimB;
        inTrimB = E * 10.0;
        inTrimB *= inTrimB;
        inTrimB *= inTrimB;
        outTrimA = outTrimB;
        outTrimB = F;
        double iirAmountA = 0.00069 / overallscale;
        biquadF[0] = biquadE[0] = 15500.0 / Effect<T>::getSampleRate();
        biquadF[1] = biquadE[1] = 0.935;
        double K = tan(M_PI * biquadE[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadE[1] + K * K);
        biquadE[2] = K * K * norm;
        biquadE[3] = 2.0 * biquadE[2];
        biquadE[4] = biquadE[2];
        biquadE[5] = 2.0 * (K * K - 1.0) * norm;
        biquadE[6] = (1.0 - K / biquadE[1] + K * K) * norm;
        for (int x = 0; x < 7; x++) {
            biquadF[x] = biquadE[x];
        }
        // end opamp stuff
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double cutoff = (cutoffA * temp) + (cutoffB * (1.0 - temp));
            double over = (overA * temp) + (overB * (1.0 - temp));
            double under = (underA * temp) + (underB * (1.0 - temp));
            double meltdown = (meltdownA * temp) + (meltdownB * (1.0 - temp));
            double inTrim = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            double outTrim = (outTrimA * temp) + (outTrimB * (1.0 - temp));
            double acidTrim = 1.0 - pow(cutoff * 0.5, 1.0 / (cutoff * 0.5));
            bezA[bez_cycle] += cutoff;
            bezA[bez_SampL] += (inputSampleL * cutoff);
            bezA[bez_SampR] += (inputSampleR * cutoff);
            if (bezA[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bezA[bez_cycle] -= 1.0;
                bezA[bez_DL] = bezA[bez_CL];
                bezA[bez_CL] = bezA[bez_BL];
                bezA[bez_BL] = bezA[bez_AL];
                bezA[bez_AL] = (bezA[bez_SampL] * (1.0 - meltdown)) + (inputSampleL * meltdown);
                bezA[bez_SampL] = 0.0;
                bezA[bez_DR] = bezA[bez_CR];
                bezA[bez_CR] = bezA[bez_BR];
                bezA[bez_BR] = bezA[bez_AR];
                bezA[bez_AR] = (bezA[bez_SampR] * (1.0 - meltdown)) + (inputSampleR * meltdown);
                bezA[bez_SampR] = 0.0;
            }
            double X = bezA[bez_cycle] * acidTrim;
            double midL = bezA[bez_DL] * pow(1.0 - X, 3.0);
            midL += bezA[bez_CL] * 3.0 * pow(1.0 - X, 2.0) * X;
            midL += bezA[bez_BL] * 3.0 * (1.0 - X) * X * X;
            midL += bezA[bez_AL] * pow(X, 3.0);
            inputSampleL = (midL * under) + ((inputSampleL - midL) * over);
            double midR = bezA[bez_DR] * pow(1.0 - X, 3.0);
            midR += bezA[bez_CR] * 3.0 * pow(1.0 - X, 2.0) * X;
            midR += bezA[bez_BR] * 3.0 * (1.0 - X) * X * X;
            midR += bezA[bez_AR] * pow(X, 3.0);
            inputSampleR = (midR * under) + ((inputSampleR - midR) * over);
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
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
            double outSample = biquadE[2] * inputSampleL + biquadE[3] * biquadE[7] + biquadE[4] * biquadE[8] - biquadE[5] * biquadE[9] - biquadE[6] * biquadE[10];
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
            if (outTrim != 1.0) {
                inputSampleL *= outTrim;
                inputSampleR *= outTrim;
            }
            // end opamp stage
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
} // namespace airwindohhs::zacidlowpass
