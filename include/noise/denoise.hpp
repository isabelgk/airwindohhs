#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::denoise {

constexpr std::string_view k_name{ "DeNoise" };
constexpr std::string_view k_short_description{
    "DeNoise can work as a multiband gate and as a wild effect."
};
constexpr std::string_view k_long_description{
    "So it starts as a multiband gate. That's pretty direct. You've got frequency ranges, and when they're set to zero there's no gating and full wet is the same as dry. Turn up a band, and you're increasing the threshold: when the sound at that band drops below the threshold it gates and there's silence. You can use the dry/wet control to moderate the effect a little, and you can dial in settings that neatly denoise a sound without interfering with the main audio too much.The speed control governs how rapidly it transitions, and increasing that slows the reaction time, so you can keep the gate from chattering, set it as quick as possible without an obvious click and you're good. On top of that there's a trick from DeCrackle that's part of the dry/wet: the same as DeCrackle, if you set it full dry rather than just bypassing it, what you get is the delta function with dry subtracted from the result. This isn't the same as an inv/wet control so I didn't label it that: it's a special case for 'monitor only what's being taken away' which is sometimes handy for stuff like this.And now, we get freaky with it :)When you raise the gating a lot farther than mere noise gating, you get into some aggressive sound sculpting very quickly. Since it's a gate, you're shortening everything, but since it's multiband it can wrench the tone around. Instead of cutting low mids on the sound, you can gate them out super aggressively and it'll shorten and tighten up a kick, but keep the low mids as part of the attack: you're basically handling the decay of every part differently. You can have the bands blend across different settings, or have adjacent bands very different from each other, and all these things twist and mutate the tone.And then once you're doing that, the speed control comes into play. It governs both how fast the gate can slam shut, and how quick it can open, and that's also relative to the frequency of the band, so exaggerate this and you get filter-sweepy effects. Adjusting this can lock in exact gatey tones really well, it should be obvious where Speed wants to be set. And of course if that's not enough for you, you can make a tone entirely out of the Delta, where it loses all the attacks and keeps only the decays being gated off…And, it's one of the more CPU-efficient Airwindows plugins so it does all that while barely touching your CPU.It should be fun, as well as useful in the normal way. Hope you like it :)"
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class DeNoise final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampL2,
        prevSlewL2,
        prevSampL3,
        prevSlewL3,
        prevSampL4,
        prevSlewL4,
        prevSampL5,
        prevSlewL5,
        prevSampL6,
        prevSlewL6,
        prevSampL7,
        prevSlewL7,
        prevSampL8,
        prevSlewL8,
        prevSampL9,
        prevSlewL9,
        prevSampR1,
        prevSlewR1,
        prevSampR2,
        prevSlewR2,
        prevSampR3,
        prevSlewR3,
        prevSampR4,
        prevSlewR4,
        prevSampR5,
        prevSlewR5,
        prevSampR6,
        prevSlewR6,
        prevSampR7,
        prevSlewR7,
        prevSampR8,
        prevSlewR8,
        prevSampR9,
        prevSlewR9,
        pear_max,
        figL,
        gndL,
        figR,
        gndR,
        slew,
        freq,
        levlL,
        levlR,
        pear_total
    }; // new pear filter
    double topLevlL;
    double topLevlR;
    double pearA[pear_total];
    double pearB[pear_total];
    double pearC[pear_total];
    double pearD[pear_total];
    double pearE[pear_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeNoise()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        E = 0.0;
        F = 0.0;
        G = 0.25;
        H = 1.0;
        for (int x = 0; x < pear_total; x++) {
            pearA[x] = 0.0;
            pearB[x] = 0.0;
            pearC[x] = 0.0;
            pearD[x] = 0.0;
            pearE[x] = 0.0;
        }
        topLevlL = 1.0;
        pearA[levlL] = 1.0;
        pearB[levlL] = 1.0;
        pearC[levlL] = 1.0;
        pearD[levlL] = 1.0;
        pearE[levlL] = 1.0;
        topLevlR = 1.0;
        pearA[levlR] = 1.0;
        pearB[levlR] = 1.0;
        pearC[levlR] = 1.0;
        pearD[levlR] = 1.0;
        pearE[levlR] = 1.0;
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
        kParamG = 6,
        kParamH = 7,
        kNumParameters = 8
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
            case kParamG: G = value; break;
            case kParamH: H = value; break;

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
            case kParamG: return G; break;
            case kParamH: return H; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.0; break;
            case kParamF: return 0.0; break;
            case kParamG: return 0.25; break;
            case kParamH: return 1.0; break;

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
            case kParamG: return "speed"; break;
            case kParamH: return "dry/wet"; break;

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
            case kParamG: return "Speed"; break;
            case kParamH: return "Dry/Wet"; break;

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
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;

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
            case kParamG: return ""; break;
            case kParamH: return ""; break;

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
        double topThresh = pow(A * 0.25, 4.0);
        double pearAThresh = pow(B * 0.25, 4.0);
        double pearBThresh = pow(C * 0.25, 4.0);
        double pearCThresh = pow(D * 0.25, 4.0);
        double pearDThresh = pow(E * 0.25, 4.0);
        double pearEThresh = pow(F * 0.25, 4.0);
        double spd = pow(G, 3.0) / overallscale;
        double wet = H;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
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
                topLevlL *= (1.0 - spd);
                topLevlL += ((fabs(inputSampleL) < topThresh) ? 0.0 : spd * 2.0);
                inputSampleL = (inputSampleL * fmin(topLevlL, 1.0));
                pearA[levlL] *= (1.0 - (spd * pearA[freq]));
                pearA[levlL] += ((fabs(pearA[figL]) < pearAThresh) ? 0.0 : (spd * 2.0 * pearA[freq]));
                inputSampleL += (pearA[figL] * fmin(pearA[levlL], 1.0));
                pearB[levlL] *= (1.0 - (spd * pearB[freq]));
                pearB[levlL] += ((fabs(pearB[figL]) < pearBThresh) ? 0.0 : (spd * 2.0 * pearB[freq]));
                inputSampleL += (pearB[figL] * fmin(pearB[levlL], 1.0));
                pearC[levlL] *= (1.0 - (spd * pearC[freq]));
                pearC[levlL] += ((fabs(pearC[figL]) < pearCThresh) ? 0.0 : (spd * 2.0 * pearC[freq]));
                inputSampleL += (pearC[figL] * fmin(pearC[levlL], 1.0));
                pearD[levlL] *= (1.0 - (spd * pearD[freq]));
                pearD[levlL] += ((fabs(pearD[figL]) < pearDThresh) ? 0.0 : (spd * 2.0 * pearD[freq]));
                inputSampleL += (pearD[figL] * fmin(pearD[levlL], 1.0));
                pearE[levlL] *= (1.0 - (spd * pearE[freq]));
                pearE[levlL] += ((fabs(pearE[figL]) < pearEThresh) ? 0.0 : (spd * 2.0 * pearE[freq]));
                inputSampleL += (pearE[figL] * fmin(pearE[levlL], 1.0));
                topLevlR *= (1.0 - spd);
                topLevlR += ((fabs(inputSampleR) < topThresh) ? 0.0 : spd * 2.0);
                inputSampleR = (inputSampleR * fmin(topLevlR, 1.0));
                pearA[levlR] *= (1.0 - (spd * pearA[freq]));
                pearA[levlR] += ((fabs(pearA[figR]) < pearAThresh) ? 0.0 : (spd * 2.0 * pearA[freq]));
                inputSampleR += (pearA[figR] * fmin(pearA[levlR], 1.0));
                pearB[levlR] *= (1.0 - (spd * pearB[freq]));
                pearB[levlR] += ((fabs(pearB[figR]) < pearBThresh) ? 0.0 : (spd * 2.0 * pearB[freq]));
                inputSampleR += (pearB[figR] * fmin(pearB[levlR], 1.0));
                pearC[levlR] *= (1.0 - (spd * pearC[freq]));
                pearC[levlR] += ((fabs(pearC[figR]) < pearCThresh) ? 0.0 : (spd * 2.0 * pearC[freq]));
                inputSampleR += (pearC[figR] * fmin(pearC[levlR], 1.0));
                pearD[levlR] *= (1.0 - (spd * pearD[freq]));
                pearD[levlR] += ((fabs(pearD[figR]) < pearDThresh) ? 0.0 : (spd * 2.0 * pearD[freq]));
                inputSampleR += (pearD[figR] * fmin(pearD[levlR], 1.0));
                pearE[levlR] *= (1.0 - (spd * pearE[freq]));
                pearE[levlR] += ((fabs(pearE[figR]) < pearEThresh) ? 0.0 : (spd * 2.0 * pearE[freq]));
                inputSampleR += (pearE[figR] * fmin(pearE[levlR], 1.0));
            }
            if (wet == 0.0) {
                drySampleL = inputSampleL - drySampleL;
                drySampleR = inputSampleR - drySampleR;
            } // clicks only at full dry
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::denoise
