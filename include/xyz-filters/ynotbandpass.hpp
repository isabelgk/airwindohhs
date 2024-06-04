#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ynotbandpass {

constexpr std::string_view k_name{ "YNotBandpass" };
constexpr std::string_view k_short_description{
    "YNotBandpass is soft and smooth to nasty, edgy texture-varying filtering, no control smoothing."
};
constexpr std::string_view k_long_description{
    "YNotBandpass has various uses. One I was trying out in the original video, was setting it up as a ‘walkie-talkie’ sort of tone on my voice, and then turning up ResEdge to really trash the hell out of the audio in a characteristic way that’s not easily found anywhere else.In the video for YNotBandpass, the version without control smoothing, I demoed it on a bunch of huge reverb, alternating between a Bricasti Cathedral and my version of that same sound… and the more rapid switching between Freq settings turns out to sound pretty nice! Except when the ResEdge control is used to give a mean, electrical, circuit-bendy quality that still retains a lot of the depth of the underlying sound."
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class YNotBandpass final : public Effect<T>
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
    }; // coefficient interpolating biquad filter, stereo
    double biquad[biq_total];
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
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    YNotBandpass()
    {
        A = 0.1;
        B = 0.5;
        C = 0.1;
        D = 0.1;
        E = 1.0;
        F = 1.0;
        for (int x = 0; x < biq_total; x++) {
            biquad[x] = 0.0;
        }
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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;
            case kParamF: return F;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.1;
            case kParamB: return 0.5;
            case kParamC: return 0.1;
            case kParamD: return 0.1;
            case kParamE: return 1.0;
            case kParamF: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "gain";
            case kParamB: return "freq";
            case kParamC: return "reson8";
            case kParamD: return "resedge";
            case kParamE: return "output";
            case kParamF: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain";
            case kParamB: return "Freq";
            case kParamC: return "Reson8";
            case kParamD: return "ResEdge";
            case kParamE: return "Output";
            case kParamF: return "Dry/Wet";

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
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);
            case kParamF: return std::to_string(F);

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
            case kParamE: return "";
            case kParamF: return "";
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
        overallscale *= getSampleRate();
        double inTrim = A * 10.0;
        biquad[biq_freq] = pow(B, 3) * 20000.0;
        if (biquad[biq_freq] < 15.0) {
            biquad[biq_freq] = 15.0;
        }
        biquad[biq_freq] /= getSampleRate();
        biquad[biq_reso] = (pow(C, 2) * 15.0) + 0.5571;
        double K = tan(M_PI * biquad[biq_freq]);
        double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
        biquad[biq_a0] = K / biquad[biq_reso] * norm;
        // biquad[biq_a1] = 0.0; //bandpass can simplify the biquad kernel: leave out this multiply
        biquad[biq_a2] = -biquad[biq_a0];
        biquad[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        biquad[biq_b2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        // for the coefficient-interpolated biquad filter
        double powFactor = pow(D + 0.9, 4);
        // 1.0 == target neutral
        double outTrim = E;
        double wet = F;
        fixA[fix_freq] = fixB[fix_freq] = 20000.0 / getSampleRate();
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
            biquad[biq_sL1] = -(temp * biquad[biq_b1]) + biquad[biq_sL2];
            biquad[biq_sL2] = (inputSampleL * biquad[biq_a2]) - (temp * biquad[biq_b2]);
            inputSampleL = temp; // coefficient interpolating biquad filter
            temp = (inputSampleR * biquad[biq_a0]) + biquad[biq_sR1];
            biquad[biq_sR1] = -(temp * biquad[biq_b1]) + biquad[biq_sR2];
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
} // namespace airwindohhs::ynotbandpass
