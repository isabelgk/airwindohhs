#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::surgetide {

constexpr std::string_view k_name{ "SurgeTide" };
constexpr std::string_view k_short_description{
    "SurgeTide is a surge and flow dynamics plugin."
};
constexpr std::string_view k_long_description{
    "This is a sort of dynamics plugin.It comes from an experiment, where I had to find a way to make a behavior useful: SurgeTide runs on three different compression time constants stacked onto each other like the waves in an FM synthesizer. You don’t usually see a compressor work with the rate of the rate of the rate of change, because for normal sounds and time constants, the result sounds bizarre and unmusical.BUT, it turns out if you set it up to run a very deep and slow change, like tidal forces on the mix, it can do really interesting things. You end up with a mix that seems totally uncompressed, because small variations just don’t alter the sound at all… but as the pressures of the music affect the compressor, it can ease off or boost volume.And because the behavior’s so odd, it can react to an easing of pressure by swinging up very quickly. This behavior can be timed, sort of. You can end up with an effect that’s a little like EDM compressor pumping for effect, except it swings up to accentuate the downbeat. And not just the downbeat: a huge surge of bass underneath the downbeat. You can practically pull any degree of thump out of a track, but it’s tricky to dial in because mostly you can’t hear it working. It’s like an invisible size boost for subs.The way to get SurgeTide working is to adjust the Surge Node until it squishes away the sound on the beat, then find the right speed for Surge Rate to work, and then back off Surge Node until it’s no longer inverting the dynamics. (unless you really want to: I’m not the boss of you.) It works really well as a subtle accentuation of mix low-end movement, giving some of the effect of a buss compressor but in an unusual and much cleaner way. Also works to subtly act as a level control and restrain dense mix moments so they can hit something like loudenation with more consistency.It doesn’t work in any useful way on isolated tracks, particularly not staccato drum tracks: just maybe it would do helpful things with say, a lead vocal or a synth pad. Just remember that SurgeTide is for powerful, whole-mix movements rather than the usual compressor things, and that it can have effects on the extreme low bass, and build up the swing and flow of a mix. It’ll work on some things and be useless on others. I hope you like it."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class SurgeTide final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    double chaseA;
    double chaseB;
    double chaseC;
    float A;
    float B;
    float C;

  public:
    SurgeTide()
    {
        A = 0.5;
        B = 0.3;
        C = 1.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = true;
        chaseA = 0.0;
        chaseB = 0.0;
        chaseC = 0.0;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.3; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "surgenode"; break;
            case kParamB: return "surgerate"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "SurgeNode"; break;
            case kParamB: return "SurgeRate"; break;
            case kParamC: return "Dry/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return " "; break;
            case kParamC: return " "; break;

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
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double intensity = A;
        double attack = ((B + 0.1) * 0.0005) / overallscale;
        double decay = ((B + 0.001) * 0.00005) / overallscale;
        double wet = C;
        // removed extra dry variable
        double inputSense;
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
            inputSampleL *= 8.0;
            inputSampleR *= 8.0;
            inputSampleL *= intensity;
            inputSampleR *= intensity;
            inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            if (chaseC < inputSense) {
                chaseA += attack;
            }
            if (chaseC > inputSense) {
                chaseA -= decay;
            }
            if (chaseA > decay) {
                chaseA = decay;
            }
            if (chaseA < -attack) {
                chaseA = -attack;
            }
            chaseB += (chaseA / overallscale);
            if (chaseB > decay) {
                chaseB = decay;
            }
            if (chaseB < -attack) {
                chaseB = -attack;
            }
            chaseC += (chaseB / overallscale);
            if (chaseC > 1.0) {
                chaseC = 1.0;
            }
            if (chaseC < 0.0) {
                chaseC = 0.0;
            }
            inputSampleL *= chaseC;
            inputSampleL = drySampleL - (inputSampleL * intensity);
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
            inputSampleR *= chaseC;
            inputSampleR = drySampleR - (inputSampleR * intensity);
            inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
} // namespace airwindohhs::surgetide
