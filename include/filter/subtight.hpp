#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::subtight {

constexpr std::string_view k_name{ "SubTight" };
constexpr std::string_view k_short_description{
    "SubTight uses a variation on the Creature algorithm to tighten sub-lows."
};
constexpr std::string_view k_long_description{
    "So here's another interesting little tool that's not been seen before.Creature has a special mode where you can set the dry/wet to 'inverse', cancelling out what the algorithm produces. It's a soft slew clipper, not a normal algorithm or a simple filter, so what it cancels isn't easily controllable. However, for the most part it darkens and distorts the sound, and then when you stack up multiple poles of it, it becomes uncontrollable, particularly as frequency drops.So, in theory, you could use it as an increasingly steep lowpass… and subtract it from dry, to make something that acts like a highpass.The thing is, it's not really increasingly steep, not at all. And it's not even a lowpass, because its behavior is so dynamics-dependent. But it's not a saturation (or anti-saturation) either, because it's slew that's being softclipped, not amplitude. This is why I initially released it believing it shouldn't be scaled by sample rate: it's a very odd sort of processing.Having put out a Redux version that applies corrections to make it consistent across sample rates, what do we get? SubTight does NOT get steeper as you increase the 'steep' control, unless you think of it as 'low settings are pretty weak tea, and cranking it up gets you a much stronger and more intense brew'.What you'll get is a behavior: as you increase the trim on SubTight, the weakness and flabbiness will get sucked out of the bass. With very low Steep settings it's pretty across the board, really. It's not hyping the highs so much as it is pulling softness and 'glue' out of all the frequencies, more as it goes lower. Super nonlinear, super tricky to interpret, but dramatic and energetic.As you increase Steepness, it gets more aggressive about this. It's like you're defining a little 'nega-zone' inside the bass and the solidness of the sound, and then making it vanish, so the transients hit with full power from a more silent, empty backdrop. Probably handy on spot drum mics in general, where you'd use gating to make them punch more! The farther you push Steep, the more it tries to refine that 'empty space' down into a tiny intense core at the center of the sub-bass… but it will continue to affect everything, because it's not a filter and Steep isn't a crossover. It's sort of a strength control. I've generally got my use out of it between 0.2 and 0.4, but I provide more extreme settings because of course I do that, you should know me by now ;)I admit this is still a strange plugin, but hopefully it is both more adaptable and better explained now. If it's no use to you directly, you may still appreciate the way it brings a distinct lifelike sound to ConsoleMC, and other things like that :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class SubTight final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double subL[22]; // probably worth just using a number here
    double subR[22]; // probably worth just using a number here
    float A;
    float B;

  public:
    SubTight()
    {
        A = 0.3;
        B = 0.5;
        for (int x = 0; x < 21; x++) {
            subL[x] = 0.0;
            subR[x] = 0.0;
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
            case kParamA: return 0.3; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "trim"; break;
            case kParamB: return "steep"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Trim"; break;
            case kParamB: return "Steep"; break;

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
        int subStages = pow(B, 2) * 16.0;
        if (subStages < 1) {
            subStages = 1;
        }
        double subTrim = pow((A * 0.3) + (pow(B, 2) * 0.2), subStages) / overallscale;
        // to use this as an analog modeler for restricting digital lows, find set values that still show bass
        // Note that this is best used sparingly, on the 'not enough subtraction' side of the node.
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
            // you want subStages and subTrim to be hardcoded values when embedding this into something else
            // then it only needs the sub[] array, and to have it initialized to 0.0
            // begin SubTight section
            double subSampleL = inputSampleL * subTrim;
            double subSampleR = inputSampleR * subTrim;
            for (int x = 0; x < subStages; x++) {
                double scale = 0.5 + fabs(subSampleL * 0.5);
                subSampleL = (subL[x] + (sin(subL[x] - subSampleL) * scale));
                subL[x] = subSampleL * scale;
                scale = 0.5 + fabs(subSampleR * 0.5);
                subSampleR = (subR[x] + (sin(subR[x] - subSampleR) * scale));
                subR[x] = subSampleR * scale;
            }
            if (subStages % 2 > 0) {
                subSampleL = -subSampleL;
                subSampleR = -subSampleR;
            }
            if (subSampleL > 0.25) {
                subSampleL = 0.25;
            }
            if (subSampleL < -0.25) {
                subSampleL = -0.25;
            }
            if (subSampleR > 0.25) {
                subSampleR = 0.25;
            }
            if (subSampleR < -0.25) {
                subSampleR = -0.25;
            }
            inputSampleL -= (subSampleL * 16.0);
            inputSampleR -= (subSampleR * 16.0);
            // end SubTight section
            // cut the level WAY down, then the modified Creature code blows up subs.
            // the adjustment of scale destabilizes the routine to blow up more DC.
            // this is boosted by 24dB and subtracted from the dry signal
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
} // namespace airwindohhs::subtight
