#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::pearliteeq {

constexpr std::string_view k_name{ "PearLiteEQ" };
constexpr std::string_view k_short_description{
    "PearLiteEQ is a four-band Pear-based graphic EQ."
};
constexpr std::string_view k_long_description{
    "It's been a minute since we've seen PearEQ… at least directly. It's been a secret weapon in my console emulations for some time. You know how there are EQ designs that are normal and act normally? There's also weirder options, and I've tried most of them and invented a few.AngleFilter was one of them: very strange and messy, and it became the lowpass and highpass in Cabs and several variations on Console. The newest Console lets you have Anglefilters, but also control the steepness so it doesn't have to get as fierce. AngleFilter is what makes PointyGuitar and PointyDeluxe especially, be so pungent and unmanageable.SmoothEQ was the opposite: a regulation biquad filter, but used in a new topology for a purer sound. That one? Not weird at all, not a lot of character either. You get to make stuff be all frequencies rather than character. It's got its uses.And then there's Pear.So this is basically a kinder, gentler version of PearEQ. It's way nicer than AngleFilter or PointyDeluxe, but it's still strong on character. It's got an ability to strongly shape frequency ranges, but tends to sound more cohesive and less clinical. It's softer and nicer than PearEQ (a six-band) and is easier to come to grips with, as it's just four nice-sounding bands.PearLiteEQ is for being friends with your sound. Though you can do pretty extreme EQ moves with it, it resists turning into total sound deconstruction, it hangs onto the basic tone of the sounds it's working on. It's not quite as invisible as a time-aligned BexEQ, but it also doesn't have to run latency. It's the sort of thing you might build into a console emulation or have on every track of your mix, and Pear will continue to find use as a kind of crossover in Airwindows EQ designs: neither so surgical that it's clinical, nor so 'character' that it's awkward and messy.I hope PearLiteEQ brightens your day :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class PearLiteEQ final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'peap';
    float A;
    float B;
    float C;
    float D;
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        prevSampL7,
        prevSlewL7,
        prevSampR7,
        prevSlewR7,
        prevSampL8,
        prevSlewL8,
        prevSampR8,
        prevSlewR8,
        pear_max,
        figL,
        figR,
        gndL,
        gndR,
        slew,
        freq,
        levl,
        pear_total
    }; // new pear filter
    double pearA[pear_total];
    double pearB[pear_total];
    double pearC[pear_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PearLiteEQ()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        for (int x = 0; x < pear_total; x++) {
            pearA[x] = 0.0;
            pearB[x] = 0.0;
            pearC[x] = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "high"; break;
            case kParamB: return "hmid"; break;
            case kParamC: return "lmid"; break;
            case kParamD: return "bass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "High"; break;
            case kParamB: return "HMid"; break;
            case kParamC: return "LMid"; break;
            case kParamD: return "Bass"; break;

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
        double topLevl = sqrt(A + 0.5);
        pearA[levl] = sqrt(B + 0.5);
        pearB[levl] = sqrt(C + 0.5);
        pearC[levl] = sqrt(D + 0.5);
        double freqFactor = sqrt(overallscale) + (overallscale * 0.5);
        pearA[freq] = pow(0.564, freqFactor + 0.85);
        pearB[freq] = pow(0.564, freqFactor + 4.1);
        pearC[freq] = pow(0.564, freqFactor + 7.1);
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
            for (int x = 0; x < pear_max; x += 4) {
                // begin Pear filter stages
                pearA[figL] = inputSampleL;
                pearA[figR] = inputSampleR;
                pearA[slew] = ((pearA[figL] - pearA[x]) + pearA[x + 1]) * pearA[freq] * 0.5;
                pearA[x] = pearA[figL] = (pearA[freq] * pearA[figL]) + ((1.0 - pearA[freq]) * (pearA[x] + pearA[x + 1]));
                pearA[x + 1] = pearA[slew];
                pearA[slew] = ((pearA[figR] - pearA[x + 2]) + pearA[x + 3]) * pearA[freq] * 0.5;
                pearA[x + 2] = pearA[figR] = (pearA[freq] * pearA[figR]) + ((1.0 - pearA[freq]) * (pearA[x + 2] + pearA[x + 3]));
                pearA[x + 3] = pearA[slew];
                inputSampleL -= pearA[figL];
                inputSampleR -= pearA[figR];
                pearB[figL] = pearA[figL];
                pearB[figR] = pearA[figR];
                pearB[slew] = ((pearB[figL] - pearB[x]) + pearB[x + 1]) * pearB[freq] * 0.5;
                pearB[x] = pearB[figL] = (pearB[freq] * pearA[figL]) + ((1.0 - pearB[freq]) * (pearB[x] + pearB[x + 1]));
                pearB[x + 1] = pearB[slew];
                pearB[slew] = ((pearB[figR] - pearB[x + 2]) + pearB[x + 3]) * pearB[freq] * 0.5;
                pearB[x + 2] = pearB[figR] = (pearB[freq] * pearA[figR]) + ((1.0 - pearB[freq]) * (pearB[x + 2] + pearB[x + 3]));
                pearB[x + 3] = pearB[slew];
                pearA[figL] -= pearB[figL];
                pearA[figR] -= pearB[figR];
                pearC[figL] = pearB[figL];
                pearC[figR] = pearB[figR];
                pearC[slew] = ((pearC[figL] - pearC[x]) + pearC[x + 1]) * pearC[freq] * 0.5;
                pearC[x] = pearC[figL] = (pearC[freq] * pearB[figL]) + ((1.0 - pearC[freq]) * (pearC[x] + pearC[x + 1]));
                pearC[x + 1] = pearC[slew];
                pearC[slew] = ((pearC[figR] - pearC[x + 2]) + pearC[x + 3]) * pearC[freq] * 0.5;
                pearC[x + 2] = pearC[figR] = (pearC[freq] * pearB[figR]) + ((1.0 - pearC[freq]) * (pearC[x + 2] + pearC[x + 3]));
                pearC[x + 3] = pearC[slew];
                pearB[figL] -= pearC[figL];
                pearB[figR] -= pearC[figR];
                inputSampleL *= topLevl;
                inputSampleR *= topLevl;
                inputSampleL += (pearA[figL] * pearA[levl]);
                inputSampleR += (pearA[figR] * pearA[levl]);
                inputSampleL += (pearB[figL] * pearB[levl]);
                inputSampleR += (pearB[figR] * pearB[levl]);
                inputSampleL += (pearC[figL] * pearC[levl]);
                inputSampleR += (pearC[figR] * pearC[levl]);
            }
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
} // namespace airwindohhs::pearliteeq
