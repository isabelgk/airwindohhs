#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::doublepaul {

constexpr std::string_view k_name{ "DoublePaul" };
constexpr std::string_view k_short_description{
    "DoublePaul is like PaulDither but more so."
};
constexpr std::string_view k_long_description{
    "This is for sort of a special purpose.I’ve got an outrageous custom dither (well, wordlength reducer: there’s no noise and no randomness in it) called Not Just Another Dither. For final output, it’s that I recommend.However, I’ve got a mastering engineer friend (Hi Bob!) who favors another dither of mine, PaulDither. The reason is, it’s traditional TPDF dither. It handles dithering duties perfectly (with a slight Airwindows-izing: the weight of the final noise floor is balanced to make one ‘bit’ function as zero, and to make the dither activate adjacent bits on either side for a better sound. This also gives a teeny least-significant-bit DC offset, if you assume ‘zero’ is between the two smallest bits, positive and negative)I wanted to give Bob (and anyone else selecting traditional TPDF) something fancier that still counted as purely normal dither. So, here’s DoublePaul.It’s TPDF highpassed dither, just like PaulDither. But, I’m also using some adjacent samples to skew the noise profile even further into the highs. It’s just a redistribution of energy, and it leaves the key pair of full-intensity noise samples to stay as TPDF dither. I just doctored the way you hear the noise, to push it a little farther into the highs. I’ve also backed off the teeny DC offset, and shaped every filter tap by ear. If you liked PaulDither, try DoublePaul and see if you like taking the PaulDither concept just a teeny bit farther, with no penalty to how the TPDF works. It’s just like a little sinc interpolation, a thing that’s trying to hide the TPDF energy by pushing it farther into the extreme highs. If you don’t trust NotJustAnotherDither, but you’d like something even quieter than PaulDither, try this."
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class DoublePaul final : public Effect<T>
{
    double bL[11];
    double bR[11];
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    DoublePaul()
    {
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
        }
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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

        double currentDitherL;
        double currentDitherR;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = (double(fpdL) / UINT32_MAX);
            currentDitherL = (bL[0] * 0.061);
            currentDitherL -= (bL[1] * 0.11);
            currentDitherL += (bL[8] * 0.126);
            currentDitherL -= (bL[7] * 0.23);
            currentDitherL += (bL[2] * 0.25);
            currentDitherL -= (bL[3] * 0.43);
            currentDitherL += (bL[6] * 0.5);
            currentDitherL -= bL[5];
            currentDitherL += bL[4];
            // this sounds different from doing it in order of sample position
            // cumulative tiny errors seem to build up even at this buss depth
            // considerably more pronounced at 32 bit float.
            // Therefore we add the most significant components LAST.
            // trying to keep values on like exponents of the floating point value.
            inputSampleL += currentDitherL;
            inputSampleL = floor(inputSampleL);
            // done with L
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = (double(fpdR) / UINT32_MAX);
            currentDitherR = (bR[0] * 0.061);
            currentDitherR -= (bR[1] * 0.11);
            currentDitherR += (bR[8] * 0.126);
            currentDitherR -= (bR[7] * 0.23);
            currentDitherR += (bR[2] * 0.25);
            currentDitherR -= (bR[3] * 0.43);
            currentDitherR += (bR[6] * 0.5);
            currentDitherR -= bR[5];
            currentDitherR += bR[4];
            // this sounds different from doing it in order of sample position
            // cumulative tiny errors seem to build up even at this buss depth
            // considerably more pronounced at 32 bit float.
            // Therefore we add the most significant components LAST.
            // trying to keep values on like exponents of the floating point value.
            inputSampleR += currentDitherR;
            inputSampleR = floor(inputSampleR);
            // done with R
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::doublepaul
