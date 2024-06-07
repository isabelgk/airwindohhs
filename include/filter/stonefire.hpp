#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::stonefire {

constexpr std::string_view k_name{ "Stonefire" };
constexpr std::string_view k_short_description{
    "Stonefire is the non-EQ EQ designed for ConsoleX."
};
constexpr std::string_view k_long_description{
    "There's a reason people have never turned to using Kalman filters for audio purposes.They're tricky little buggers, unpredictable, with a 'filter slope' that makes no sense at all. They'll take the crossover point and bounce it around wildly, they'll throw in weird gatey behaviors, they'll turn what's supposed to be a 'smooth' 'filtered' sound into an edgy growl and sputter. They're meant to pull real data out of a pile of noise, not to take real audio and give you anything sensible. Nobody would want a Kalman filter for audio purposes.Stonefire uses two different kinds of Kalman filters for audio, as crossovers. The top crossover is the same as what's in Air3. The bottom crossover comes with a range control (even though that won't give you a 'frequency') and is the same as what's in Kalman.And when used properly, Stonefire gives you unprecedented levels of tonal control over the texture and presentation of your sound, in a way that almost doesn't even have to do with frequency.There's three bands, plus the Range control. Air, Fire, and Stone. Each can be cut back to zero, or boosted (Fire and Stone match, while Air has a lot more gain on tap but will match from 0.5 down to 0.0.) If you cut them back to zero you'll get the weird Kalman behavior, but it'll help you set the Range control appropriately. You can set it so Stone covers the lowest lows (never JUST those, it will always do other stuff too) or up to the high mids and lower treble.Then, if you keep everything balanced and make smaller adjustments, the secret of the Kalman filter emerges. You have to use it as a crossover, and let it apply its incredibly strong character to texture, not frequency. It utterly fails to be a 'filter', but it's an extraordinary texture-shaper, and it's what I'm going to be building the upcoming ConsoleX system around.Use the Air control like you would in Air3. You can cut back super-highs while seeming to not affect the brightness at all, with a strikingly natural effect. Or, boost it to bring that sparkly aura and light up the sound. It's a custom algorithm that deals with high sample rates by just ramping up the boost: treat it with respect, but it's there to serve your needs for glitter or lack of same.Use the Fire control like it was an attitude knob, as much as a midrange. You can get a lot of wildness out of this one with careful settings of Range. Between Fire and Stone, only one can be louder: it's a crossover. If you're boosting Fire, that means you want your sound to command attention.Use the Stone control like it's the bedrock of your sound. If you lean entirely on it and kill all the Fire, you'll get a monumentally heavy, sputtering, gatey foundation that zeroes in on the lowest lows, but also tries to put backbone behind anything it thinks is heavy and powerful. This includes lower midrange. Cutting it can control unwieldy bass, but adding it isn't the same as adding a 'bass boost': it'll zero in on things like kick weight and try to present them with unnatural isolation. The secret to adding weight with the Stone control is to let it not seem to be that much of a boost: you can transform the feel of a sound well before you hear 'added bass'. You can also dial the Range up and use the same effect up into the midrange, for an intensely solid punchy character anywhere a sound seems flimsy.God help you if you try to measure this thing with PluginDoctor or SPAN. I don't even have any idea what will happen. It's all made out of Kalman filters, which are not for use with audio.Except… when they are ;)Enjoy the new tone shaping. If it's too ugly, use way less, see how it treats you :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Stonefire final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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

        public :
            Stonefire(){
                A = 0.5;
                B = 0.5;
                C = 0.5;
                D = 0.5;
                for (int x = 0; x < air_total; x++) air[x] = 0.0;
                for (int x = 0; x < kal_total; x++) kal[x] = 0.0;
                trebleGainA = 1.0; trebleGainB = 1.0;
                midGainA = 1.0; midGainB = 1.0;
                bassGainA = 1.0; bassGainB = 1.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 0.5;
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
            return "air";
            break;
        kParamB:
            return "fire";
            break;
        kParamC:
            return "stone";
            break;
        kParamD:
            return "range";
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
            return "Air";
            break;
        kParamB:
            return "Fire";
            break;
        kParamC:
            return "Stone";
            break;
        kParamD:
            return "Range";
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        trebleGainA = trebleGainB;
        trebleGainB = A * 2.0;
        midGainA = midGainB;
        midGainB = B * 2.0;
        bassGainA = bassGainB;
        bassGainB = C * 2.0;
        // simple three band to adjust
        double kalman = 1.0 - pow(D, 2);
        // crossover frequency between mid/bass
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double trebleGain = (trebleGainA * temp) + (trebleGainB * (1.0 - temp));
            if (trebleGain > 1.0) {
                trebleGain = pow(trebleGain, 3.0 + sqrt(overallscale));
            }
            if (trebleGain < 1.0) {
                trebleGain = 1.0 - pow(1.0 - trebleGain, 2);
            }
            double midGain = (midGainA * temp) + (midGainB * (1.0 - temp));
            if (midGain > 1.0) {
                midGain *= midGain;
            }
            if (midGain < 1.0) {
                midGain = 1.0 - pow(1.0 - midGain, 2);
            }
            double bassGain = (bassGainA * temp) + (bassGainB * (1.0 - temp));
            if (bassGain > 1.0) {
                bassGain *= bassGain;
            }
            if (bassGain < 1.0) {
                bassGain = 1.0 - pow(1.0 - bassGain, 2);
            }
            // begin Air3L
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
            double midL = drySampleL - ((air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
            temp = (midL + air[gndavgL]) * 0.5;
            air[gndavgL] = midL;
            midL = temp;
            double trebleL = drySampleL - midL;
            inputSampleL = midL;
            // end Air3L
            // begin Air3R
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
            double midR = drySampleR - ((air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
            temp = (midR + air[gndavgR]) * 0.5;
            air[gndavgR] = midR;
            midR = temp;
            double trebleR = drySampleR - midR;
            inputSampleR = midR;
            // end Air3R
            // begin KalmanL
            temp = inputSampleL = inputSampleL * (1.0 - kalman) * 0.777;
            inputSampleL *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2];
            kal[prevSlewL3] *= 0.5;
            kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1];
            kal[prevSlewL2] *= 0.5;
            kal[prevSlewL1] += kal[prevSampL1] - inputSampleL;
            kal[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2];
            kal[accSlewL2] *= 0.5;
            kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1];
            kal[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]);
            kal[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3];
            kal[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainL] += fabs(temp - kal[kalOutL]) * kalman * 8.0;
            kal[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainL] > kalman * 0.5) {
                kal[kalGainL] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutL] += (temp * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampL3] = kal[prevSampL2];
            kal[prevSampL2] = kal[prevSampL1];
            kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0 - kal[kalGainL]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampL1] > 1.0) {
                kal[prevSampL1] = 1.0;
            }
            if (kal[prevSampL1] < -1.0) {
                kal[prevSampL1] = -1.0;
            }
            double bassL = kal[kalOutL] * 0.777;
            midL -= bassL;
            // end KalmanL
            // begin KalmanR
            temp = inputSampleR = inputSampleR * (1.0 - kalman) * 0.777;
            inputSampleR *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2];
            kal[prevSlewR3] *= 0.5;
            kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1];
            kal[prevSlewR2] *= 0.5;
            kal[prevSlewR1] += kal[prevSampR1] - inputSampleR;
            kal[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2];
            kal[accSlewR2] *= 0.5;
            kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1];
            kal[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]);
            kal[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3];
            kal[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainR] += fabs(temp - kal[kalOutR]) * kalman * 8.0;
            kal[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainR] > kalman * 0.5) {
                kal[kalGainR] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutR] += (temp * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampR3] = kal[prevSampR2];
            kal[prevSampR2] = kal[prevSampR1];
            kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0 - kal[kalGainR]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampR1] > 1.0) {
                kal[prevSampR1] = 1.0;
            }
            if (kal[prevSampR1] < -1.0) {
                kal[prevSampR1] = -1.0;
            }
            double bassR = kal[kalOutR] * 0.777;
            midR -= bassR;
            // end KalmanR
            inputSampleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            inputSampleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
            // applies pan section, and smoothed fader gain
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
} // namespace airwindohhs::stonefire
