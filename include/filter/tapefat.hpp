#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapefat {

constexpr std::string_view k_name{ "TapeFat" };
constexpr std::string_view k_short_description{
    "TapeFat is the tone control from TapeDelay."
};
constexpr std::string_view k_long_description{
    "TapeFat is just the tone control from TapeDelay. It works like an averaging filter that you can use to either roll off highs (or eventually mids), or subtract the effect to create a highpass and take out the lows.The reason this is interesting is, that tone control is completely bizarre. It’s an averaging filter, but on a pile of delay taps arranged according to prime numbers. Works more like an ambience control, but more densely packed. If you put an impulse through it you don’t get a smoothed-out lowpass so much as a bizarre micro-reverb. Since it’s using primes, it doesn’t reinforce any particular frequency. Since it’s an ambience, it doesn’t have any pre-echoes like linear phase EQs, and the artifacts it produces become a tone of their own (either in-phase, or inverted).You can hear it on the video, which has a number of things updated, not least this: the new audio is directly captured analog sorta-house music out of my livestreams. This way you ought to be able to really hear the way my plugins retain analog qualities, because now the demo music is essentially AAD: not products of other DAW mixes or digital synthesis, but source material."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class TapeFat final : public Effect<T>
{
    int pL[258];
    int pR[258];
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    TapeFat()
    {
        A = 1.0;
        B = 0.0;
        for (int count = 0; count < 257; count++) {
            pL[count] = 0;
            pR[count] = 0;
        }
        gcount = 0;
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
        kNumParameters = 2
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 0.0;
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
            return "lean/ft";
            break;
        kParamB:
            return "depth";
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
            return "Lean/Ft";
            break;
        kParamB:
            return "Depth";
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
            return "taps";
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

        double leanfat = ((A * 2.0) - 1.0);
        double wet = fabs(leanfat);
        int fatness = (int)floor((B * 29.0) + 3.0);
        double floattotalL = 0.0;
        double floattotalR = 0.0;
        int sumtotalL = 0;
        int sumtotalR = 0;
        int count;
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
            if (gcount < 0 || gcount > 128) {
                gcount = 128;
            }
            count = gcount;
            pL[count + 128] = pL[count] = sumtotalL = (int)(inputSampleL * 8388608.0);
            pR[count + 128] = pR[count] = sumtotalR = (int)(inputSampleR * 8388608.0);
            switch (fatness)
            {
                case 32: sumtotalL += pL[count + 127]; sumtotalR += pR[count + 127]; // note NO break statement.
                case 31: sumtotalL += pL[count + 113]; sumtotalR += pR[count + 113]; // This jumps to the relevant tap
                case 30: sumtotalL += pL[count + 109]; sumtotalR += pR[count + 109]; // and then includes all smaller taps.
                case 29: sumtotalL += pL[count + 107]; sumtotalR += pR[count + 107];
                case 28: sumtotalL += pL[count + 103]; sumtotalR += pR[count + 103];
                case 27: sumtotalL += pL[count + 101]; sumtotalR += pR[count + 101];
                case 26: sumtotalL += pL[count + 97]; sumtotalR += pR[count + 97];
                case 25: sumtotalL += pL[count + 89]; sumtotalR += pR[count + 89];
                case 24: sumtotalL += pL[count + 83]; sumtotalR += pR[count + 83];
                case 23: sumtotalL += pL[count + 79]; sumtotalR += pR[count + 79];
                case 22: sumtotalL += pL[count + 73]; sumtotalR += pR[count + 73];
                case 21: sumtotalL += pL[count + 71]; sumtotalR += pR[count + 71];
                case 20: sumtotalL += pL[count + 67]; sumtotalR += pR[count + 67];
                case 19: sumtotalL += pL[count + 61]; sumtotalR += pR[count + 61];
                case 18: sumtotalL += pL[count + 59]; sumtotalR += pR[count + 59];
                case 17: sumtotalL += pL[count + 53]; sumtotalR += pR[count + 53];
                case 16: sumtotalL += pL[count + 47]; sumtotalR += pR[count + 47];
                case 15: sumtotalL += pL[count + 43]; sumtotalR += pR[count + 43];
                case 14: sumtotalL += pL[count + 41]; sumtotalR += pR[count + 41];
                case 13: sumtotalL += pL[count + 37]; sumtotalR += pR[count + 37];
                case 12: sumtotalL += pL[count + 31]; sumtotalR += pR[count + 31];
                case 11: sumtotalL += pL[count + 29]; sumtotalR += pR[count + 29];
                case 10: sumtotalL += pL[count + 23]; sumtotalR += pR[count + 23];
                case 9: sumtotalL += pL[count + 19]; sumtotalR += pR[count + 19];
                case 8: sumtotalL += pL[count + 17]; sumtotalR += pR[count + 17];
                case 7: sumtotalL += pL[count + 13]; sumtotalR += pR[count + 13];
                case 6: sumtotalL += pL[count + 11]; sumtotalR += pR[count + 11];
                case 5: sumtotalL += pL[count + 7]; sumtotalR += pR[count + 7];
                case 4: sumtotalL += pL[count + 5]; sumtotalR += pR[count + 5];
                case 3: sumtotalL += pL[count + 3]; sumtotalR += pR[count + 3];
                case 2: sumtotalL += pL[count + 2]; sumtotalR += pR[count + 2];
                case 1: sumtotalL += pL[count + 1]; sumtotalR += pR[count + 1];
            }
            floattotalL = (double)(sumtotalL / fatness + 1);
            floattotalR = (double)(sumtotalR / fatness + 1);
            floattotalL /= 8388608.0;
            floattotalR /= 8388608.0;
            floattotalL *= wet;
            floattotalR *= wet;
            if (leanfat < 0) {
                inputSampleL = inputSampleL - floattotalL;
                inputSampleR = inputSampleR - floattotalR;
            }
            else {
                inputSampleL = (inputSampleL * (1 - wet)) + floattotalL;
                inputSampleR = (inputSampleR * (1 - wet)) + floattotalR;
            }
            gcount--;
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
} // namespace airwindohhs::tapefat
