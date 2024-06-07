#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::reseq2 {

constexpr std::string_view k_name{ "ResEQ2" };
constexpr std::string_view k_short_description{
    "ResEQ2 is a single, sharp, sonorous mid peak."
};
constexpr std::string_view k_long_description{
    "This is another 'piece of an upcoming great plugin'. In order to do an MCI console properly I had to do a good mid peak.And we're talking 'way better than just a sharp biquad filter' mid peak. I needed clarity and character beyond what regular digital EQ cookbooks could cook up.ResEQ2 continues on the work I started in the original ResEQ, where I observed that Manley EQ impulses for sharp resonances seemed to be like a sine-like ring, except the onset did NOT seem to be at the same frequency: seemed to start faster, even double the frequency. I made a whole plugin, ResEQ, giving it my best shot for generating multiple resonant 'rings' and combining them, to produce a convolution impulse that was the sum of multiple analog-like resoances. It still exists: it's way before I routinely worked at 96k, and it's got a lot of quirks, but it does get a distinct sound.I returned to those deep, murky waters when trying to come up with a sweepable mid peak like certain classic analog consoles.ResEQ2 is the result. It's the opposite of what you'll normally find in great classic analog consoles. A lot of the classics really had quite limited analog EQ: detailed parametric sculpting came in with SSL, and to some extent API before that. In the olden days, things were a lot simpler (and you gained something sonically from this simplicity).But there were a few special cases, and so you had MCI's sweepable mid, that could only boost. Not cut. It just gave you a sort of ring, wherever you wanted it. Not the most flexible circuit… but a hitmaker.This is because, contrary to modern practice, there's huge power in being able to single out a midrange, upper-mid, or treble frequency, and sort of just open up the top of it so it can get effortlessly loud. Instead of just blasting everything, you find one presence peak on your track that really lets it speak, and you just give that a boost. More peak energy, more clarity exactly where it's most useful, and it's almost never in the same place for different instruments or vocalists, so the combined sound of the mix cuts through on dozens of sonorities at once, and everything is powerful and clear.It's the mids equivalent of Airwindows Weight for bass, and it works incredibly well (even if you do it with biquads or EQ-design cookbooks). And I don't have the analog-Console projects finished yet… but you can have this part of it now.Use the 'ow argh way too extreme' settings like 1.0, where everything kind of turns into an audio laser, to dial in exactly what spot opens up an instrument or voice for maximum passion and sonority. Then, dial it back to around 0.5 and begin increasing it, seeing at what point you've got too much of a good thing. ResEQ2 is great at being a subtle light-bringer and giving clarity to a track. It's also a full-on energy weapon that can be set to 'way too much', so use it however you please. The resonance increases as you turn it up, so feel free to dial it back if it gets ringy. Probably not a good plugin for mixing live sound unless you like dial-a-feedback :) hope you enjoy ResEQ2!"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class ResEQ2 final : public Effect<T>
{
    double mpkL[2005];
    double mpkR[2005];
    double f[66];
    double prevfreqMPeak;
    double prevamountMPeak;
    int mpc;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    ResEQ2()
    {
        A = 0.5;
        B = 0.0;
        for (int count = 0; count < 2004; count++) {
            mpkL[count] = 0.0;
            mpkR[count] = 0.0;
        }
        for (int count = 0; count < 65; count++) {
            f[count] = 0.0;
        }
        prevfreqMPeak = -1;
        prevamountMPeak = -1;
        mpc = 1;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "msweep"; break;
            case kParamB: return "mboost"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "MSweep"; break;
            case kParamB: return "MBoost"; break;

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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        // begin ResEQ2 Mid Boost
        double freqMPeak = pow(A + 0.15, 3);
        double amountMPeak = pow(B, 2);
        int maxMPeak = (amountMPeak * 63.0) + 1;
        if ((freqMPeak != prevfreqMPeak) || (amountMPeak != prevamountMPeak)) {
            for (int x = 0; x < maxMPeak; x++) {
                if (((double)x * freqMPeak) < M_PI_4) {
                    f[x] = sin(((double)x * freqMPeak) * 4.0) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
                else {
                    f[x] = cos((double)x * freqMPeak) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
            }
            prevfreqMPeak = freqMPeak;
            prevamountMPeak = amountMPeak;
        } // end ResEQ2 Mid Boost
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
            // begin ResEQ2 Mid Boost
            mpc++;
            if (mpc < 1 || mpc > 2001) {
                mpc = 1;
            }
            mpkL[mpc] = inputSampleL;
            mpkR[mpc] = inputSampleR;
            double midMPeakL = 0.0;
            double midMPeakR = 0.0;
            for (int x = 0; x < maxMPeak; x++) {
                int y = x * cycleEnd;
                switch (cycleEnd)
                {
                    case 1:
                        midMPeakL += (mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        midMPeakR += (mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        break;
                    case 2:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        break;
                    case 3:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        break;
                    case 4:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25); // break
                }
            }
            inputSampleL = (midMPeakL * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleL : inputSampleL * (1.5 - amountMPeak));
            inputSampleR = (midMPeakR * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? inputSampleR : inputSampleR * (1.5 - amountMPeak));
            // end ResEQ2 Mid Boost
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
} // namespace airwindohhs::reseq2
