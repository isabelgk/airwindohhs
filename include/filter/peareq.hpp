#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::peareq {

constexpr std::string_view k_name{ "PearEQ" };
constexpr std::string_view k_short_description{
    "PearEQ is a six-band Pear-based graphic EQ."
};
constexpr std::string_view k_long_description{
    "Turns out Pear wasn't done, even though it's been around for years. Pear, Pear2 for more nonlinearity, ConsoleMC and MD, ConsoleLA… all based on my filter derived from the Holt filter. I've done it by itself, I've stacked it up in the traditional way for making a steep multipole filter, I've taken those and made multi-band units nineteen Pear filters deep.And all of that was before I started fooling with AngleEQ, which is incapable of doing what Pear or a biquad filter can do. Angle screws up the phase so thoroughly that if you generate a rolled-off filtered crossover, and subtract the filtered part from dry, the result still has just as much bass as before.So, the trick was reconstructing the original sound out of however many bands you have, EACH pole of the filter. Seemingly a pointless endeavor, but when you do that… suddenly the weird filter is able to filter both ways. So what happens if you do that to a biquad filter that was already able to do both things?You get SmoothEQ2. That's what I did to make the hyper-flexible filter with tilty shelves. And that's great, and Pear was just sitting there, waiting for me to try it.PearEQ combines an intensely natural, analog-sounding character around the sharpness of the filter edges, with a steepness otherwise unavailable to that kind of sound. It's a completely different sound from any other way you'd get that Q factor. You can take any biquad filter (for instance, any DAW standard filter) and crank up the steepness, and you'll get that sharp of a crossover… with obvious resonance, and it'll sound totally different. You can construct an isolator filter out of biquads and it'll still act different: Pear produces an increasingly steep drop-off into the stopband, and biquads won't. It's just different, and PearEQ lets you use that differentness either for great subtlety and natural tone… or to rip and boost frequencies WAY more than you should.Go right ahead, and I'll keep working on more out of all this, as it comes together and shows its usefulness :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class PearEQ final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
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
        prevSampL9,
        prevSlewL9,
        prevSampR9,
        prevSlewR9,
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
    double pearD[pear_total];
    double pearE[pear_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PearEQ()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        for (int x = 0; x < pear_total; x++) {
            pearA[x] = 0.0;
            pearB[x] = 0.0;
            pearC[x] = 0.0;
            pearD[x] = 0.0;
            pearE[x] = 0.0;
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "hmid"; break;
            case kParamC: return "mid"; break;
            case kParamD: return "lmid"; break;
            case kParamE: return "bass"; break;
            case kParamF: return "sub"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "HMid"; break;
            case kParamC: return "Mid"; break;
            case kParamD: return "LMid"; break;
            case kParamE: return "Bass"; break;
            case kParamF: return "Sub"; break;

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
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double topLevl = sqrt(A + 0.5);
        pearA[levl] = sqrt(B + 0.5);
        pearB[levl] = sqrt(C + 0.5);
        pearC[levl] = sqrt(D + 0.5);
        pearD[levl] = sqrt(E + 0.5);
        pearE[levl] = sqrt(F + 0.5);
        double freqFactor = sqrt(overallscale) + (overallscale * 0.5);
        pearA[freq] = pow(0.564, freqFactor + 0.85);
        pearB[freq] = pow(0.564, freqFactor + 1.9);
        pearC[freq] = pow(0.564, freqFactor + 4.1);
        pearD[freq] = pow(0.564, freqFactor + 6.2);
        pearE[freq] = pow(0.564, freqFactor + 7.7);
        while (--sampleFrames >= 0) {
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
                pearD[figL] = pearC[figL];
                pearD[figR] = pearC[figR];
                pearD[slew] = ((pearD[figL] - pearD[x]) + pearD[x + 1]) * pearD[freq] * 0.5;
                pearD[x] = pearD[figL] = (pearD[freq] * pearC[figL]) + ((1.0 - pearD[freq]) * (pearD[x] + pearD[x + 1]));
                pearD[x + 1] = pearD[slew];
                pearD[slew] = ((pearD[figR] - pearD[x + 2]) + pearD[x + 3]) * pearD[freq] * 0.5;
                pearD[x + 2] = pearD[figR] = (pearD[freq] * pearC[figR]) + ((1.0 - pearD[freq]) * (pearD[x + 2] + pearD[x + 3]));
                pearD[x + 3] = pearD[slew];
                pearC[figL] -= pearD[figL];
                pearC[figR] -= pearD[figR];
                pearE[figL] = pearD[figL];
                pearE[figR] = pearD[figR];
                pearE[slew] = ((pearE[figL] - pearE[x]) + pearE[x + 1]) * pearE[freq] * 0.5;
                pearE[x] = pearE[figL] = (pearE[freq] * pearD[figL]) + ((1.0 - pearE[freq]) * (pearE[x] + pearE[x + 1]));
                pearE[x + 1] = pearE[slew];
                pearE[slew] = ((pearE[figR] - pearE[x + 2]) + pearE[x + 3]) * pearE[freq] * 0.5;
                pearE[x + 2] = pearE[figR] = (pearE[freq] * pearD[figR]) + ((1.0 - pearE[freq]) * (pearE[x + 2] + pearE[x + 3]));
                pearE[x + 3] = pearE[slew];
                pearD[figL] -= pearE[figL];
                pearD[figR] -= pearE[figR];
                inputSampleL *= topLevl;
                inputSampleR *= topLevl;
                inputSampleL += (pearA[figL] * pearA[levl]);
                inputSampleR += (pearA[figR] * pearA[levl]);
                inputSampleL += (pearB[figL] * pearB[levl]);
                inputSampleR += (pearB[figR] * pearB[levl]);
                inputSampleL += (pearC[figL] * pearC[levl]);
                inputSampleR += (pearC[figR] * pearC[levl]);
                inputSampleL += (pearD[figL] * pearD[levl]);
                inputSampleR += (pearD[figR] * pearD[levl]);
                inputSampleL += (pearE[figL] * pearE[levl]);
                inputSampleR += (pearE[figR] * pearE[levl]);
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
} // namespace airwindohhs::peareq
