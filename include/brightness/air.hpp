#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::air {

constexpr std::string_view k_name{ "Air" };
constexpr std::string_view k_short_description{
    "Air3 creates a new form of air-band EQ based on Kalman filtering."
};
constexpr std::string_view k_long_description{
    "See Air2 for more on this. Air is the same, but simpler, and restricted to one sample rate so that if you're at a higher sample rate, the effects will shift up in frequency, possibly making them inaudible.Here’s what you get: Air2 has three treble-boost bands, Hiss, Glitter and Air. The first two are very like what’s in Energy2, but Air2 is NOT the same: it’s a different algorithm, not done the samw way Energy2’s ‘Rat’ band is done, and it has a broader, softer air band that’s less ‘raw harsh energy injection’ and more ‘bright and pretty’.And then, they all run through a ‘Silk’ control… inspired by but NOT the same as the Neve Portico Silk circuit. That is a real hardware transformer biased into second harmonic creation by a DC current. Air2’s Silk control is the same thing as Single Ended Triode (which you can download and use already) but only applied to the highs out of Air2, so it’s the same general concept but is not a clone of the Portico preamp. Plus if they freak out I will rename it to ‘Sow’s Ear’: it’s part of a treble brightener and the same basic functionality and as far as I’m concerned, nothing is stopping me from asymmetrically distorting highs, and Silk is the best general term for what that does. Except with mine you can push it too far for effect, because of course you can :)And finally, unlike Energy2, the Dry/Wet control for Air2 strikes a new balance between the Energy style of increasing the effect, and the traditional Dry/Wet control. New with Air2, you can now turn it to full-wet and get ONLY the output of the brightener bands. They’re not exactly filters but they act like it, and by adjusting them against each other you can produce insanely treble-boosted sounds and wipe out everything else using Dry/Wet. The way it works is, up to 75% (0.75) you still have full volume Dry. It won’t re-balance your track, just add whatever Air2 highs you’re looking for. Then, between 0.75 and 1 (full Wet) the dry goes away, so if you crank it up all the way you get the fullest extreme, but for most normal use it’s like Energy2, taking an unvarying dry signal (which is NOT undersampled) and adding however much of the effect you like.This should be fun :)Welcome to the experiment zone, once again, with Air3: an air band EQ based on an extension of Kalman filtering, which isn't normally used for audio purposes. As usual, Airwindows is bringing in algorithms that act weirdly because they come out of finance or science. In this case, it's science.What's Kalman filtering? You'll be hearing more about it, but it's a method of trying to bring accuracy out of noisy data by projecting what the real data might be, and then incorporating sensor readings based on how closely they correspond to the projection. The key idea there is 'based on', because Kalman is all about willingness to throw out bad data and try and zero in on what's really happening.Of course, in audio, it's ALL real. There's nothing to throw out, it's all legit. Give Air3 some audio, and it begins to do its analysis and projections based on something that doesn't exist: a hypothetical underlying sound, upon which your real-world cymbal sparkle or voice breathiness are just 'noise' to be removed. It's not even based on frequencies, it's trying to chart out a 'real' sound based on what's already a real sound.What happens? For one, you get a rolled-off sound that is very, very convincing. It's easy to believe the sound was just that way to start with. You don't get a sense of brightness reduction, it just sounds natural. Air3 is shunting off the whole air band to a separate control, and what it leaves behind is very plausible and doesn't sound filtered. Great for reduction of tizz, glare and detail.And then you take that redirected air band, bring it back in, and crank it up five times as loud, and Air3 comes alive.I've not heard a better EQ for this super-high stuff. It seems really good at lighting up any audio you send through it. Bear in mind that much like both Air and Air2 before it, this is not an EQ in the normal sense at all. Air and Air2 used the algorithms I also put in Energy, and what Air3 does has more in common with DeBess or Acceleration than it does an EQ. It's going way into concepts of measuring the rate of change of the rate of change and trying to project that onto expected future samples… it's got a large amount of boost on tap, but can be a bit unpredictable. If you simply cut the Gnd control (for Ground) you get just whatever air there was, if you cut the Air control you remove it, but if you boost the Air control lots and cut Gnd, you get another sound from the sheer intensity of boosting being done.I think this is going to be hard to beat for this purpose, especially used subtly, and expect to be including it into upcoming stuff that doesn't have to act like 'an EQ'. Still a lot of 'lettered' Console versions that need to be made, to emulate analog gear. This one's for imagining where we can go after that, for a new sort of mixing in the box that doesn't follow the constrictions of analog gear, letting you dial in tone characteristics as much as frequencies.For now, enjoy Air3, and if you've got opinions on what needs to follow it next, drop by a livestream: you have Air3 today because a livestream viewer asked for it to come out ASAP, and here we are. There's lots more. I'll keep working on all this stuff, it's great to see it all come together :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Air final : public Effect<T>
{
    double airPrevAL;
    double airEvenAL;
    double airOddAL;
    double airFactorAL;
    double airPrevBL;
    double airEvenBL;
    double airOddBL;
    double airFactorBL;
    double airPrevCL;
    double airEvenCL;
    double airOddCL;
    double airFactorCL;
    double tripletPrevL;
    double tripletMidL;
    double tripletAL;
    double tripletBL;
    double tripletCL;
    double tripletFactorL;
    double airPrevAR;
    double airEvenAR;
    double airOddAR;
    double airFactorAR;
    double airPrevBR;
    double airEvenBR;
    double airOddBR;
    double airFactorBR;
    double airPrevCR;
    double airEvenCR;
    double airOddCR;
    double airFactorCR;
    double tripletPrevR;
    double tripletMidR;
    double tripletAR;
    double tripletBR;
    double tripletCR;
    double tripletFactorR;
    bool flipA;
    bool flipB;
    bool flop;
    int count;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float E;
    float F;

  public:
    Air()
    {
        airPrevAL = 0.0;
        airEvenAL = 0.0;
        airOddAL = 0.0;
        airFactorAL = 0.0;
        airPrevBL = 0.0;
        airEvenBL = 0.0;
        airOddBL = 0.0;
        airFactorBL = 0.0;
        airPrevCL = 0.0;
        airEvenCL = 0.0;
        airOddCL = 0.0;
        airFactorCL = 0.0;
        tripletPrevL = 0.0;
        tripletMidL = 0.0;
        tripletAL = 0.0;
        tripletBL = 0.0;
        tripletCL = 0.0;
        tripletFactorL = 0.0;
        airPrevAR = 0.0;
        airEvenAR = 0.0;
        airOddAR = 0.0;
        airFactorAR = 0.0;
        airPrevBR = 0.0;
        airEvenBR = 0.0;
        airOddBR = 0.0;
        airFactorBR = 0.0;
        airPrevCR = 0.0;
        airEvenCR = 0.0;
        airOddCR = 0.0;
        airFactorCR = 0.0;
        tripletPrevR = 0.0;
        tripletMidR = 0.0;
        tripletAR = 0.0;
        tripletBR = 0.0;
        tripletCR = 0.0;
        tripletFactorR = 0.0;
        flipA = false;
        flipB = false;
        flop = false;
        count = 1;
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        E = 1.0;
        F = 1.0;
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
        kParamairPrevAL = 0,
        kParamairEvenAL = 1,
        kParamairOddAL = 2,
        kParamairFactorAL = 3,
        kParamairPrevBL = 4,
        kParamairEvenBL = 5,
        kNumParameters = 6

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: airPrevAL = value; break;
            case kParamairEvenAL: airEvenAL = value; break;
            case kParamairOddAL: airOddAL = value; break;
            case kParamairFactorAL: airFactorAL = value; break;
            case kParamairPrevBL: airPrevBL = value; break;
            case kParamairEvenBL: airEvenBL = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return airPrevAL;
            case kParamairEvenAL: return airEvenAL;
            case kParamairOddAL: return airOddAL;
            case kParamairFactorAL: return airFactorAL;
            case kParamairPrevBL: return airPrevBL;
            case kParamairEvenBL: return airEvenBL;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return 0.0;
            case kParamairEvenAL: return 0.0;
            case kParamairOddAL: return 0.0;
            case kParamairFactorAL: return 0.0;
            case kParamairPrevBL: return 0.0;
            case kParamairEvenBL: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return "22k tap";
            case kParamairEvenAL: return "15k tap";
            case kParamairOddAL: return "11k tap";
            case kParamairFactorAL: return "filters q";
            case kParamairPrevBL: return "output level";
            case kParamairEvenBL: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return "22K tap";
            case kParamairEvenAL: return "15K tap";
            case kParamairOddAL: return "11K tap";
            case kParamairFactorAL: return "filters Q";
            case kParamairPrevBL: return "Output Level";
            case kParamairEvenBL: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return std::to_string((A * 2.0) - 1.0);
            case kParamairEvenAL: return std::to_string((B * 2.0) - 1.0);
            case kParamairOddAL: return std::to_string((C * 2.0) - 1.0);
            case kParamairFactorAL: return std::to_string(D);
            case kParamairPrevBL: return std::to_string(E);
            case kParamairEvenBL: return std::to_string(F);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamairPrevAL: return " ";
            case kParamairEvenAL: return " ";
            case kParamairOddAL: return " ";
            case kParamairFactorAL: return " ";
            case kParamairPrevBL: return " ";
            case kParamairEvenBL: return " ";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double hiIntensity = -pow(((A * 2.0) - 1.0), 3) * 2;
        double tripletintensity = -pow(((B * 2.0) - 1.0), 3);
        double airIntensity = -pow(((C * 2.0) - 1.0), 3) / 2;
        double filterQ = 2.1 - D;
        double output = E;
        double wet = F;
        double dry = 1.0 - wet;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double correctionL;
        double correctionR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            correctionL = 0.0;
            correctionR = 0.0; // from here on down, please add L and R to the code
            if (count < 1 || count > 3) {
                count = 1;
            }
            tripletFactorL = tripletPrevL - inputSampleL;
            tripletFactorR = tripletPrevR - inputSampleR;
            switch (count)
            {
                case 1:
                    tripletAL += tripletFactorL;
                    tripletCL -= tripletFactorL;
                    tripletFactorL = tripletAL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletAR += tripletFactorR;
                    tripletCR -= tripletFactorR;
                    tripletFactorR = tripletAR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
                case 2:
                    tripletBL += tripletFactorL;
                    tripletAL -= tripletFactorL;
                    tripletFactorL = tripletBL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletBR += tripletFactorR;
                    tripletAR -= tripletFactorR;
                    tripletFactorR = tripletBR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
                case 3:
                    tripletCL += tripletFactorL;
                    tripletBL -= tripletFactorL;
                    tripletFactorL = tripletCL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletCR += tripletFactorR;
                    tripletBR -= tripletFactorR;
                    tripletFactorR = tripletCR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
            }
            tripletAL /= filterQ;
            tripletBL /= filterQ;
            tripletCL /= filterQ;
            correctionL = correctionL + tripletFactorL;
            tripletAR /= filterQ;
            tripletBR /= filterQ;
            tripletCR /= filterQ;
            correctionR = correctionR + tripletFactorR;
            count++;
            // finished Triplet section- 15K
            if (flop)
            {
                airFactorAL = airPrevAL - inputSampleL;
                airFactorAR = airPrevAR - inputSampleR;
                if (flipA)
                {
                    airEvenAL += airFactorAL;
                    airOddAL -= airFactorAL;
                    airFactorAL = airEvenAL * airIntensity;
                    airEvenAR += airFactorAR;
                    airOddAR -= airFactorAR;
                    airFactorAR = airEvenAR * airIntensity;
                }
                else
                {
                    airOddAL += airFactorAL;
                    airEvenAL -= airFactorAL;
                    airFactorAL = airOddAL * airIntensity;
                    airOddAR += airFactorAR;
                    airEvenAR -= airFactorAR;
                    airFactorAR = airOddAR * airIntensity;
                }
                airOddAL = (airOddAL - ((airOddAL - airEvenAL) / 256.0)) / filterQ;
                airEvenAL = (airEvenAL - ((airEvenAL - airOddAL) / 256.0)) / filterQ;
                airPrevAL = inputSampleL;
                correctionL = correctionL + airFactorAL;
                airOddAR = (airOddAR - ((airOddAR - airEvenAR) / 256.0)) / filterQ;
                airEvenAR = (airEvenAR - ((airEvenAR - airOddAR) / 256.0)) / filterQ;
                airPrevAR = inputSampleR;
                correctionR = correctionR + airFactorAR;
                flipA = !flipA;
            }
            else
            {
                airFactorBL = airPrevBL - inputSampleL;
                airFactorBR = airPrevBR - inputSampleR;
                if (flipB)
                {
                    airEvenBL += airFactorBL;
                    airOddBL -= airFactorBL;
                    airFactorBL = airEvenBL * airIntensity;
                    airEvenBR += airFactorBR;
                    airOddBR -= airFactorBR;
                    airFactorBR = airEvenBR * airIntensity;
                }
                else
                {
                    airOddBL += airFactorBL;
                    airEvenBL -= airFactorBL;
                    airFactorBL = airOddBL * airIntensity;
                    airOddBR += airFactorBR;
                    airEvenBR -= airFactorBR;
                    airFactorBR = airOddBR * airIntensity;
                }
                airOddBL = (airOddBL - ((airOddBL - airEvenBL) / 256.0)) / filterQ;
                airEvenBL = (airEvenBL - ((airEvenBL - airOddBL) / 256.0)) / filterQ;
                airPrevBL = inputSampleL;
                correctionL = correctionL + airFactorBL;
                airOddBR = (airOddBR - ((airOddBR - airEvenBR) / 256.0)) / filterQ;
                airEvenBR = (airEvenBR - ((airEvenBR - airOddBR) / 256.0)) / filterQ;
                airPrevBR = inputSampleR;
                correctionR = correctionR + airFactorBR;
                flipB = !flipB;
            }
            // 11K one
            airFactorCL = airPrevCL - inputSampleL;
            airFactorCR = airPrevCR - inputSampleR;
            if (flop)
            {
                airEvenCL += airFactorCL;
                airOddCL -= airFactorCL;
                airFactorCL = airEvenCL * hiIntensity;
                airEvenCR += airFactorCR;
                airOddCR -= airFactorCR;
                airFactorCR = airEvenCR * hiIntensity;
            }
            else
            {
                airOddCL += airFactorCL;
                airEvenCL -= airFactorCL;
                airFactorCL = airOddCL * hiIntensity;
                airOddCR += airFactorCR;
                airEvenCR -= airFactorCR;
                airFactorCR = airOddCR * hiIntensity;
            }
            airOddCL = (airOddCL - ((airOddCL - airEvenCL) / 256.0)) / filterQ;
            airEvenCL = (airEvenCL - ((airEvenCL - airOddCL) / 256.0)) / filterQ;
            airPrevCL = inputSampleL;
            correctionL = correctionL + airFactorCL;
            airOddCR = (airOddCR - ((airOddCR - airEvenCR) / 256.0)) / filterQ;
            airEvenCR = (airEvenCR - ((airEvenCR - airOddCR) / 256.0)) / filterQ;
            airPrevCR = inputSampleR;
            correctionR = correctionR + airFactorCR;
            flop = !flop;
            inputSampleL += correctionL;
            inputSampleR += correctionR;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
} // namespace airwindohhs::air
