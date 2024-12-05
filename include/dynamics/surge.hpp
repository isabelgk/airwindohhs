#pragma once
#pragma warning(disable : 4458)

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow"
#endif

#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::surge {

constexpr std::string_view k_name{ "Surge" };
constexpr std::string_view k_short_description{
    "Surge is a compressor for accentuating beats and pulses."
};
constexpr std::string_view k_long_description{
    "What is ‘ballistics’ when it comes to audio gear? It’s the way the gear handles dynamics coming in. It’s the swing of a VU meter that can tell you what your music feels like.Or, since this is Airwindows and we don’t do no steenkin’ meters, ballistics is the way dynamics hit this compressor to produce outrageous groove.This is also a good reason to keep me around working on stuff… this plugin wouldn’t exist if not for an earlier free VST plug from me, called SurgeTide. That one controls dynamics by altering the rate of change of the rate of change OF the rate of change of the audio. What, you may ask, would happen if you did that? I’ll tell you: firstly, there is no smoother compressor. It’s utterly, totally fluid and artifact-less, organic to the Nth degree. But, it doesn’t sound like it’s doing anything until suddenly it kicks in and starts making the dynamics swing up wildly to boost every beat. Incredibly hard to control. You really can get a sense of ‘huge surges of the musical tide’ emphasizing the deepest rhythms, but it’s a bear to set up!And since I’m still out there working on stuff thanks to my Patreon, now there’s a whole new version, so different it counts as a separate compressor: Surge.Here’s the thing. Compressors are as unique as, say, guitar overdrives. You can search for the magic one, but there can be all different sorts of ‘right one’, with hugely different characters to ’em. I made Pyewacket as a free VST. It delivers a super-articulate attack transient and leans out the sound rather than simply turning it down, and gives a ‘British Classic Rock’ vibe without any silly adding of faux-analog mud: that one clamps down on the body of sounds while leaving tons of intensity and headroom to the tone. Its ‘ballistics’ are very simple and it’s more about delivering a certain kind of attack, and making stuff more even and consistent.Surge isn’t anything like that. It’s SurgeTide, but easier to control: just bring it up until it starts compressing enough, and don’t go too far to where it inverts the dynamics. You can hear it easily, unlike SurgeTide. But it still retains most of the uncanny fluid organic quality SurgeTide has, and most importantly, it has the ballistics. That means that if you put a pumping, rhythmic mix through it… it will accentuate the beats in a very aggressive way that’s not like anything you’ve heard before. The attacks pop way out, including the deepest bass pulses: this is yet another way to handle compressed bassy mixes and transform them into something punchier. And unlike SurgeTide, it’s very controllable.You might want to push it hard enough that the dynamics jump way out. You might want to use it as ‘glue’ and hit it only very gently, and take advantage of that extreme fluid openness (a result of the way its control is so abstracted: again, it alters the rate of change of the rate of change OF the rate of change). The one thing it won’t do at all is limiter-like behavior, it’s way too funky for that! So anywhere you need to use a compressor that is just ultimately funky, bouncy, squishy… try Surge. It might be right up your alley.Summary: Surge is more obvious and responds easier, SurgeTide is cleaner and harder to control."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Surge final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    double chaseA;
    double chaseB;
    double chaseC;
    double chaseD;
    double chaseMax;
    float A;
    float B;

  public:
    Surge()
    {
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
        chaseD = 0.0;
        chaseMax = 0.0;
        A = 0.0;
        B = 1.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "surge"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Surge"; break;
            case kParamB: return "Dry/Wet"; break;

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
            case kParamA: return " "; break;
            case kParamB: return " "; break;

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
        double chaseMax = 0.0;
        double intensity = (1.0 - (pow((1.0 - A), 2))) * 0.7;
        double attack = ((intensity + 0.1) * 0.0005) / overallscale;
        double decay = ((intensity + 0.001) * 0.00005) / overallscale;
        double wet = B;
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
            if (chaseMax < inputSense) {
                chaseA += attack;
            }
            if (chaseMax > inputSense) {
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
            if (chaseC > decay) {
                chaseC = decay;
            }
            if (chaseC < -attack) {
                chaseC = -attack;
            }
            chaseD += (chaseC / overallscale);
            if (chaseD > 1.0) {
                chaseD = 1.0;
            }
            if (chaseD < 0.0) {
                chaseD = 0.0;
            }
            chaseMax = chaseA;
            if (chaseMax < chaseB) {
                chaseMax = chaseB;
            }
            if (chaseMax < chaseC) {
                chaseMax = chaseC;
            }
            if (chaseMax < chaseD) {
                chaseMax = chaseD;
            }
            inputSampleL *= chaseMax;
            inputSampleL = drySampleL - (inputSampleL * intensity);
            inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
            inputSampleR *= chaseMax;
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
} // namespace airwindohhs::surge
#pragma warning(default : 4458)

#if defined(__clang__)
#pragma clang diagnostic pop
#endif