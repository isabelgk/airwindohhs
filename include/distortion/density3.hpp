#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::density3 {

constexpr std::string_view k_name{ "Density3" };
constexpr std::string_view k_short_description{
    "Density3 refines Density to keep up with recent developments."
};
constexpr std::string_view k_long_description{
    "This secret weapon's seen a little fine-tuning.Density's one of my first plugins. Its power is in using sin() based overdrive combined with a very gentle highpass, one that can dial back the amount of saturation on heavy basses… while allowing it through anyhow using the dry/wet control. It's an incredible dirt-shaper for refined work.But what if it used the technique from PurestSaturation? That's Density3: finally an upgrade.But there's more! Turns out the negative Density range enjoys a change too: it blended better with a phase-flip. I could have known it needed one, except nobody constructs inverse saturations like this, as it doesn't seem like a useful sound. Until you take advantage of its gatey qualities to dial in tone, between -1 and 0, that keeps the nature of the tone the same but dries up its quieter regions, as subtly as you'd like.This is out because another dev I know was looking into Density2, and wanting to shoot it out against his own algorithms. I said, just so you know, there's a Density3 and it's on the github repository, it's just not out yet.Now it is :)"
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class Density3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double iirSampleL;
    double iirSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Density3()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        D = 1.0;
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
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "density"; break;
            case kParamB: return "highpas"; break;
            case kParamC: return "output"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Density"; break;
            case kParamB: return "Highpas"; break;
            case kParamC: return "Output"; break;
            case kParamD: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
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
        double density = A * 5.0;
        double iirAmount = pow(B, 3) / overallscale;
        if (iirAmount == 0.0) {
            iirSampleL = 0.0;
            iirSampleR = 0.0;
        }
        double output = C;
        double wet = D;
        while (--sampleFrames >= 0) {
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
            iirSampleL = (iirSampleL * (1.0 - iirAmount)) + (inputSampleL * iirAmount);
            inputSampleL -= iirSampleL;
            iirSampleR = (iirSampleR * (1.0 - iirAmount)) + (inputSampleR * iirAmount);
            inputSampleR -= iirSampleR;
            double altered = inputSampleL;
            if (density > 1.0) {
                altered = fmax(fmin(inputSampleL * density * M_PI_2, M_PI_2), -M_PI_2);
                double X = altered * altered;
                double temp = altered * X;
                altered -= (temp / 6.0);
                temp *= X;
                altered += (temp / 120.0);
                temp *= X;
                altered -= (temp / 5040.0);
                temp *= X;
                altered += (temp / 362880.0);
                temp *= X;
                altered -= (temp / 39916800.0);
            }
            if (density < 1.0) {
                altered = fmax(fmin(inputSampleL, 1.0), -1.0);
                double polarity = altered;
                double X = inputSampleL * altered;
                double temp = X;
                altered = (temp / 2.0);
                temp *= X;
                altered -= (temp / 24.0);
                temp *= X;
                altered += (temp / 720.0);
                temp *= X;
                altered -= (temp / 40320.0);
                temp *= X;
                altered += (temp / 3628800.0);
                altered *= ((polarity < 0.0) ? -1.0 : 1.0);
            }
            if (density > 2.0) {
                inputSampleL = altered;
            }
            else {
                inputSampleL = (inputSampleL * (1.0 - fabs(density - 1.0))) + (altered * fabs(density - 1.0));
            }
            altered = inputSampleR;
            if (density > 1.0) {
                altered = fmax(fmin(inputSampleR * density * M_PI_2, M_PI_2), -M_PI_2);
                double X = altered * altered;
                double temp = altered * X;
                altered -= (temp / 6.0);
                temp *= X;
                altered += (temp / 120.0);
                temp *= X;
                altered -= (temp / 5040.0);
                temp *= X;
                altered += (temp / 362880.0);
                temp *= X;
                altered -= (temp / 39916800.0);
            }
            if (density < 1.0) {
                altered = fmax(fmin(inputSampleR, 1.0), -1.0);
                double polarity = altered;
                double X = inputSampleR * altered;
                double temp = X;
                altered = (temp / 2.0);
                temp *= X;
                altered -= (temp / 24.0);
                temp *= X;
                altered += (temp / 720.0);
                temp *= X;
                altered -= (temp / 40320.0);
                temp *= X;
                altered += (temp / 3628800.0);
                altered *= ((polarity < 0.0) ? -1.0 : 1.0);
            }
            if (density > 2.0) {
                inputSampleR = altered;
            }
            else {
                inputSampleR = (inputSampleR * (1.0 - fabs(density - 1.0))) + (altered * fabs(density - 1.0));
            }
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * output * wet);
            inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * output * wet);
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::density3
