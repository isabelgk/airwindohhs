#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::air4 {

constexpr std::string_view k_name{ "Air4" };
constexpr std::string_view k_short_description{
    "Air4 extends Air3 with controllable high frequency limiting."
};
constexpr std::string_view k_long_description{
    "I'm pleased and honored to bring you Air4, which extends Air3 in a way that can help people trying my Meter plugin, and unable to get good high frequencies without bombing the slew section with red 'overslew' spikes.Back in the day, because of natural limitations of analog media and disc mastering, we didn't have extended treble, and yet there were still bright hit records. We more readily hear brightness when it's really upper-mids, but the really high stuff has special characteristics: first, it's harder to hear, and second, it conveys a spatial position in air. That position is 'CLOSE'. Within arm's reach, within a few inches, within a few millimeters… of your eardrum.This is because air is lossy, but digital isn't. Digital loses nothing, it just adds various distortions while it goes (this is why oversampling fans get excited about that: the extra distortions are rarely nice). For that reason, it's easy to make digital sounds unrealistically 'close', and this characterizes nearly all digital mixing.We see this in Airwindows Meter as massive red spikes. Because there's absolutely no flexibility or 'glue' to the highs, what happens is you get all the sonic cues of 'far too close to my ear' before you get the brightness lift you're expecting. Air3 only aggravates this: it's based on Kalman filtering, and it can boost ONLY that airy glitter, and then you've got that problem. Fine if you want that effect, but then you're also distorting.Air is the control for doing that, and Gnd means 'ground' and is literally everything else, so you can also adjust levels with it. Then there's the new controls. DarkF means 'dark filter', and it's the same as the control in Sinew (remember that? you already have it, but I saw it as a tube-fuzz sort of thing). And Ratio is basically a dry/wet for just DarkF, just Sinew (which, when it was invented, didn't get one: really novel stuff often appears as a super minimal plugin first).As seen in the video, if you adjust DarkF it produces a hard limit on the slews. It defaults to just over 0.5, which automatically stops 'red spikes' in the Slew section of Meter, no matter what treble boost you do. This'll be an aggressive sort of 'glue' for the highs, or a safety highs-restrictor if you want to not mess with the red spikes of brightness.To use it the way I use it, maybe lower the DarkF a bit, and then sneak Ratio back from full crank. What'll happen is, the spikes will gradually creep up towards their original volume, forming a spread-out cloud rather than a hard clipped line, except they'll be quieter than they were. The idea is to find the loudness you want for these bright peaks, and then work out how wide a range they're to cover. Doing this gives you brightness, but without the brittle harshness of digital treble boosting.I hope you like Air4! I'm working on ConsoleX every day now, and it'll get there. It's a LOT of work."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Air4 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    enum
    {
        pvAL1,
        pvSL1,
        accSL1,
        acc2SL1,
        pvAL2,
        pvSL2,
        accSL2,
        acc2SL2,
        pvAL3,
        pvSL3,
        accSL3,
        pvAL4,
        pvSL4,
        gndavgL,
        outAL,
        gainAL,
        lastSL,
        pvAR1,
        pvSR1,
        accSR1,
        acc2SR1,
        pvAR2,
        pvSR2,
        accSR2,
        acc2SR2,
        pvAR3,
        pvSR3,
        accSR3,
        pvAR4,
        pvSR4,
        gndavgR,
        outAR,
        gainAR,
        lastSR,
        air_total
    };
    double air[air_total];

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Air4()
    {
        A = 0.5;
        B = 0.5;
        C = 0.52;
        D = 0.0;
        for (int x = 0; x < air_total; x++) {
            air[x] = 0.0;
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
            case kParamC: return 0.52; break;
            case kParamD: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "air"; break;
            case kParamB: return "gnd"; break;
            case kParamC: return "darkf"; break;
            case kParamD: return "ratio"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Air"; break;
            case kParamB: return "Gnd"; break;
            case kParamC: return "DarkF"; break;
            case kParamD: return "Ratio"; break;

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
        double airGain = A * 2.0;
        if (airGain > 1.0) {
            airGain = pow(airGain, 3.0 + sqrt(overallscale));
        }
        double gndGain = B * 2.0;
        double threshSinew = pow(C, 2) / overallscale;
        double depthSinew = D;
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
            air[pvSL4] = air[pvAL4] - air[pvAL3];
            air[pvSL3] = air[pvAL3] - air[pvAL2];
            air[pvSL2] = air[pvAL2] - air[pvAL1];
            air[pvSL1] = air[pvAL1] - inputSampleL;
            air[accSL3] = air[pvSL4] - air[pvSL3];
            air[accSL2] = air[pvSL3] - air[pvSL2];
            air[accSL1] = air[pvSL2] - air[pvSL1];
            air[acc2SL2] = air[accSL3] - air[accSL2];
            air[acc2SL1] = air[accSL2] - air[accSL1];
            air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1]) * 0.5));
            air[gainAL] *= 0.5;
            air[gainAL] += fabs(drySampleL - air[outAL]) * 0.5;
            if (air[gainAL] > 0.3 * sqrt(overallscale)) {
                air[gainAL] = 0.3 * sqrt(overallscale);
            }
            air[pvAL4] = air[pvAL3];
            air[pvAL3] = air[pvAL2];
            air[pvAL2] = air[pvAL1];
            air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
            double gnd = drySampleL - ((air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
            double temp = (gnd + air[gndavgL]) * 0.5;
            air[gndavgL] = gnd;
            gnd = temp;
            inputSampleL = ((drySampleL - gnd) * airGain) + (gnd * gndGain);
            air[pvSR4] = air[pvAR4] - air[pvAR3];
            air[pvSR3] = air[pvAR3] - air[pvAR2];
            air[pvSR2] = air[pvAR2] - air[pvAR1];
            air[pvSR1] = air[pvAR1] - inputSampleR;
            air[accSR3] = air[pvSR4] - air[pvSR3];
            air[accSR2] = air[pvSR3] - air[pvSR2];
            air[accSR1] = air[pvSR2] - air[pvSR1];
            air[acc2SR2] = air[accSR3] - air[accSR2];
            air[acc2SR1] = air[accSR2] - air[accSR1];
            air[outAR] = -(air[pvAR1] + air[pvSR3] + air[acc2SR2] - ((air[acc2SR2] + air[acc2SR1]) * 0.5));
            air[gainAR] *= 0.5;
            air[gainAR] += fabs(drySampleR - air[outAR]) * 0.5;
            if (air[gainAR] > 0.3 * sqrt(overallscale)) {
                air[gainAR] = 0.3 * sqrt(overallscale);
            }
            air[pvAR4] = air[pvAR3];
            air[pvAR3] = air[pvAR2];
            air[pvAR2] = air[pvAR1];
            air[pvAR1] = (air[gainAR] * air[outAR]) + drySampleR;
            gnd = drySampleR - ((air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
            temp = (gnd + air[gndavgR]) * 0.5;
            air[gndavgR] = gnd;
            gnd = temp;
            inputSampleR = ((drySampleR - gnd) * airGain) + (gnd * gndGain);
            temp = inputSampleL;
            if (temp > 1.0) {
                temp = 1.0;
            }
            if (temp < -1.0) {
                temp = -1.0;
            }
            double sinew = threshSinew * cos(air[lastSL] * air[lastSL]);
            if (temp - air[lastSL] > sinew) {
                temp = air[lastSL] + sinew;
            }
            if (-(temp - air[lastSL]) > sinew) {
                temp = air[lastSL] - sinew;
            }
            air[lastSL] = temp;
            inputSampleL = (inputSampleL * (1.0 - depthSinew)) + (air[lastSL] * depthSinew);
            temp = inputSampleR;
            if (temp > 1.0) {
                temp = 1.0;
            }
            if (temp < -1.0) {
                temp = -1.0;
            }
            sinew = threshSinew * cos(air[lastSR] * air[lastSR]);
            if (temp - air[lastSR] > sinew) {
                temp = air[lastSR] + sinew;
            }
            if (-(temp - air[lastSR]) > sinew) {
                temp = air[lastSR] - sinew;
            }
            air[lastSR] = temp;
            inputSampleR = (inputSampleR * (1.0 - depthSinew)) + (air[lastSR] * depthSinew);
            // run Sinew to stop excess slews, but run a dry/wet to allow a range of brights
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
} // namespace airwindohhs::air4
