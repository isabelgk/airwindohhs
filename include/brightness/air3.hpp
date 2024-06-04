#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::air3 {

constexpr std::string_view k_name{ "Air3" };
constexpr std::string_view k_short_description{
    "Air3 creates a new form of air-band EQ based on Kalman filtering."
};
constexpr std::string_view k_long_description{
    "This should be fun :)Welcome to the experiment zone, once again, with Air3: an air band EQ based on an extension of Kalman filtering, which isn't normally used for audio purposes. As usual, Airwindows is bringing in algorithms that act weirdly because they come out of finance or science. In this case, it's science.What's Kalman filtering? You'll be hearing more about it, but it's a method of trying to bring accuracy out of noisy data by projecting what the real data might be, and then incorporating sensor readings based on how closely they correspond to the projection. The key idea there is 'based on', because Kalman is all about willingness to throw out bad data and try and zero in on what's really happening.Of course, in audio, it's ALL real. There's nothing to throw out, it's all legit. Give Air3 some audio, and it begins to do its analysis and projections based on something that doesn't exist: a hypothetical underlying sound, upon which your real-world cymbal sparkle or voice breathiness are just 'noise' to be removed. It's not even based on frequencies, it's trying to chart out a 'real' sound based on what's already a real sound.What happens? For one, you get a rolled-off sound that is very, very convincing. It's easy to believe the sound was just that way to start with. You don't get a sense of brightness reduction, it just sounds natural. Air3 is shunting off the whole air band to a separate control, and what it leaves behind is very plausible and doesn't sound filtered. Great for reduction of tizz, glare and detail.And then you take that redirected air band, bring it back in, and crank it up five times as loud, and Air3 comes alive.I've not heard a better EQ for this super-high stuff. It seems really good at lighting up any audio you send through it. Bear in mind that much like both Air and Air2 before it, this is not an EQ in the normal sense at all. Air and Air2 used the algorithms I also put in Energy, and what Air3 does has more in common with DeBess or Acceleration than it does an EQ. It's going way into concepts of measuring the rate of change of the rate of change and trying to project that onto expected future samples… it's got a large amount of boost on tap, but can be a bit unpredictable. If you simply cut the Gnd control (for Ground) you get just whatever air there was, if you cut the Air control you remove it, but if you boost the Air control lots and cut Gnd, you get another sound from the sheer intensity of boosting being done.I think this is going to be hard to beat for this purpose, especially used subtly, and expect to be including it into upcoming stuff that doesn't have to act like 'an EQ'. Still a lot of 'lettered' Console versions that need to be made, to emulate analog gear. This one's for imagining where we can go after that, for a new sort of mixing in the box that doesn't follow the constrictions of analog gear, letting you dial in tone characteristics as much as frequencies.For now, enjoy Air3, and if you've got opinions on what needs to follow it next, drop by a livestream: you have Air3 today because a livestream viewer asked for it to come out ASAP, and here we are. There's lots more. I'll keep working on all this stuff, it's great to see it all come together :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Air3 final : public Effect<T>
{
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
        air_total
    };
    double air[air_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Air3()
    {
        A = 0.5;
        B = 0.5;
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
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "air";
            case kParamB: return "gnd";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Air";
            case kParamB: return "Gnd";

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
        double airGain = A * 2.0;
        if (airGain > 1.0) {
            airGain = pow(airGain, 3.0 + sqrt(overallscale));
        }
        double gndGain = B * 2.0;
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
} // namespace airwindohhs::air3
