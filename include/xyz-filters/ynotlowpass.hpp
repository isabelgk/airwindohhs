#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ynotlowpass {

constexpr std::string_view k_name{ "YNotLowpass" };
constexpr std::string_view k_short_description{
    "YNotLowpass is soft and smooth to nasty, edgy texture-varying filtering, no control smoothing."
};
constexpr std::string_view k_long_description{
    "YNotLowpass introduces a new way to internally distort filters, and a new control… Resonant Edge! The ‘normal’ position for this is around 0.1 on the control. If you make it less, you get a slightly asymmetrical distortion that lets you get really warm analog filter sounds, even when they’re resonant. If you crank it up, the Resonant Edge lets you go to very aggressive, glitchy sounds that are a lot more like circuit bending than bit-banging.This is the alternate version of YLowpass, except without control smoothing. That means it'll be slightly less CPU-hungry, and might be preferable for situations like use in VCV Rack at very small buffer sizes. These are also buying me some time to work on the upcoming ConsoleMC, which is beginning to take shape… and on a new update for ToTape, for which I've got an idea for a bias control. So, use YNotLowpass if you'd like slightly more CPU efficiency, if you run tiny buffers, if you are using it as a fixed filter sound, or if you want that 'neuro' glitchy zipper-noise sound, at which it'll be really good since it already has an aggressively unnatural filter tone :)"
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class YNotLowpass final : public Effect<T>
{
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total

        public :
            YNotLowpass(){
                A = 0.1;
                B = 0.5;
                C = 0.1;
                D = 0.1;
                E = 1.0;
                F = 1.0;
                for (int x = 0; x < biq_total; x++){ biquad[x] = 0.0; }
    for (int x = 0; x < fix_total; x++) {
        fixA[x] = 0.0;
        fixB[x] = 0.0;
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

enum params {
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
    kParamA:
        A = value;
        break;
    kParamB:
        B = value;
        break;
    kParamC:
        C = value;
        break;
    kParamD:
        D = value;
        break;
    kParamE:
        E = value;
        break;
    kParamF:
        F = value;
        break;

        default: break;
    }
}

float get_parameter_value(int index)
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return A;
        break;
    kParamB:
        return B;
        break;
    kParamC:
        return C;
        break;
    kParamD:
        return D;
        break;
    kParamE:
        return E;
        break;
    kParamF:
        return F;
        break;

        default: break;
    }
    return 0.0;
}

T get_parameter_default(int index)
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return 0.1;
        break;
    kParamB:
        return 0.5;
        break;
    kParamC:
        return 0.1;
        break;
    kParamD:
        return 0.1;
        break;
    kParamE:
        return 1.0;
        break;
    kParamF:
        return 1.0;
        break;

        default: break;
    }
    return 0.0;
}

constexpr std::string_view get_parameter_name(int index)
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return "gain";
        break;
    kParamB:
        return "freq";
        break;
    kParamC:
        return "reson8";
        break;
    kParamD:
        return "resedge";
        break;
    kParamE:
        return "output";
        break;
    kParamF:
        return "dry/wet";
        break;

        default: break;
    }
    return {};
}

constexpr std::string_view get_parameter_title(int index)
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return "Gain";
        break;
    kParamB:
        return "Freq";
        break;
    kParamC:
        return "Reson8";
        break;
    kParamD:
        return "ResEdge";
        break;
    kParamE:
        return "Output";
        break;
    kParamF:
        return "Dry/Wet";
        break;

        default: break;
    }
    return {};
}

std::string get_parameter_display(int index) const
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return std::to_string(A);
        break;
    kParamB:
        return std::to_string(B);
        break;
    kParamC:
        return std::to_string(C);
        break;
    kParamD:
        return std::to_string(D);
        break;
    kParamE:
        return std::to_string(E);
        break;
    kParamF:
        return std::to_string(F);
        break;

        default: break;
    }
    return {};
}

constexpr std::string_view get_parameter_label(int index) const
{
    switch (static_cast<params>(index))
    {
    kParamA:
        return "";
        break;
    kParamB:
        return "";
        break;
    kParamC:
        return "";
        break;
    kParamD:
        return "";
        break;
    kParamE:
        return "";
        break;
    kParamF:
        return "";
        break;

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
    biquad[biq_freq] = pow(B, 3) * 20000.0;
    if (biquad[biq_freq] < 15.0) {
        biquad[biq_freq] = 15.0;
    }
    biquad[biq_freq] /= Effect<T>::getSampleRate();
    biquad[biq_reso] = (pow(C, 2) * 15.0) + 0.5571;
    double K = tan(M_PI * biquad[biq_freq]);
    double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
    biquad[biq_a0] = K * K * norm;
    biquad[biq_a1] = 2.0 * biquad[biq_a0];
    biquad[biq_a2] = biquad[biq_a0];
    biquad[biq_b1] = 2.0 * (K * K - 1.0) * norm;
    biquad[biq_b2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
    // for the coefficient-interpolated biquad filter
    double powFactor = pow(D + 0.9, 4);
    // 1.0 == target neutral
    double outTrim = E;
    double wet = F;
    fixA[fix_freq] = fixB[fix_freq] = 20000.0 / Effect<T>::getSampleRate();
    fixA[fix_reso] = fixB[fix_reso] = 0.7071; // butterworth Q
    K = tan(M_PI * fixA[fix_freq]);
    norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
    fixA[fix_a0] = fixB[fix_a0] = K * K * norm;
    fixA[fix_a1] = fixB[fix_a1] = 2.0 * fixA[fix_a0];
    fixA[fix_a2] = fixB[fix_a2] = fixA[fix_a0];
    fixA[fix_b1] = fixB[fix_b1] = 2.0 * (K * K - 1.0) * norm;
    fixA[fix_b2] = fixB[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
    // for the fixed-position biquad filter
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
        inputSampleL *= inTrim;
        inputSampleR *= inTrim;
        double temp = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
        fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sL2];
        fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (temp * fixA[fix_b2]);
        inputSampleL = temp; // fixed biquad filtering ultrasonics
        temp = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
        fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sR2];
        fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (temp * fixA[fix_b2]);
        inputSampleR = temp; // fixed biquad filtering ultrasonics
        // encode/decode courtesy of torridgristle under the MIT license
        if (inputSampleL > 1.0) {
            inputSampleL = 1.0;
        }
        else if (inputSampleL > 0.0) {
            inputSampleL = 1.0 - pow(1.0 - inputSampleL, powFactor);
        }
        if (inputSampleL < -1.0) {
            inputSampleL = -1.0;
        }
        else if (inputSampleL < 0.0) {
            inputSampleL = -1.0 + pow(1.0 + inputSampleL, powFactor);
        }
        if (inputSampleR > 1.0) {
            inputSampleR = 1.0;
        }
        else if (inputSampleR > 0.0) {
            inputSampleR = 1.0 - pow(1.0 - inputSampleR, powFactor);
        }
        if (inputSampleR < -1.0) {
            inputSampleR = -1.0;
        }
        else if (inputSampleR < 0.0) {
            inputSampleR = -1.0 + pow(1.0 + inputSampleR, powFactor);
        }
        temp = (inputSampleL * biquad[biq_a0]) + biquad[biq_sL1];
        biquad[biq_sL1] = (inputSampleL * biquad[biq_a1]) - (temp * biquad[biq_b1]) + biquad[biq_sL2];
        biquad[biq_sL2] = (inputSampleL * biquad[biq_a2]) - (temp * biquad[biq_b2]);
        inputSampleL = temp; // coefficient interpolating biquad filter
        temp = (inputSampleR * biquad[biq_a0]) + biquad[biq_sR1];
        biquad[biq_sR1] = (inputSampleR * biquad[biq_a1]) - (temp * biquad[biq_b1]) + biquad[biq_sR2];
        biquad[biq_sR2] = (inputSampleR * biquad[biq_a2]) - (temp * biquad[biq_b2]);
        inputSampleR = temp; // coefficient interpolating biquad filter
        // encode/decode courtesy of torridgristle under the MIT license
        if (inputSampleL > 1.0) {
            inputSampleL = 1.0;
        }
        else if (inputSampleL > 0.0) {
            inputSampleL = 1.0 - pow(1.0 - inputSampleL, (1.0 / powFactor));
        }
        if (inputSampleL < -1.0) {
            inputSampleL = -1.0;
        }
        else if (inputSampleL < 0.0) {
            inputSampleL = -1.0 + pow(1.0 + inputSampleL, (1.0 / powFactor));
        }
        if (inputSampleR > 1.0) {
            inputSampleR = 1.0;
        }
        else if (inputSampleR > 0.0) {
            inputSampleR = 1.0 - pow(1.0 - inputSampleR, (1.0 / powFactor));
        }
        if (inputSampleR < -1.0) {
            inputSampleR = -1.0;
        }
        else if (inputSampleR < 0.0) {
            inputSampleR = -1.0 + pow(1.0 + inputSampleR, (1.0 / powFactor));
        }
        inputSampleL *= outTrim;
        inputSampleR *= outTrim;
        temp = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
        fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sL2];
        fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (temp * fixB[fix_b2]);
        inputSampleL = temp; // fixed biquad filtering ultrasonics
        temp = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
        fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sR2];
        fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (temp * fixB[fix_b2]);
        inputSampleR = temp; // fixed biquad filtering ultrasonics
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
        in1++;
        in2++;
        out1++;
        out2++;
    }
}

};
} // namespace airwindohhs::ynotlowpass
