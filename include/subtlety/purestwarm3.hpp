#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::purestwarm3 {

constexpr std::string_view k_name{ "PurestWarm3" };
constexpr std::string_view k_short_description{
    "PurestWarm3 is a subtle tone shaper and warmth adder."
};
constexpr std::string_view k_long_description{
    "It's been some years since PurestWarm saw any attention. It's always been a bit tricky: proper second harmonic distortion is 'warmth' but it's very hard to hear. But that's just the start of it…Every version of PurestWarm ever (OK, so that's just the original and v2) has had a flaw that I didn't fully understand in 2022, and have come to respect. Freya Holmer's video on spline continuity tipped me off, but I learned about it years earlier with an ill-fated Channel 'upgrade' that I had to roll back as if it was 'New Coke'. People respond to sudden changes in derivatives even when the waveform seems to have perfectly good continuity.The crossover through zero in an audio waveform counts.So, the previous versions of PurestWarm do an abrupt switch between 'dry' and 'effect' at the zero cross. That's nearly as bad as what a sine does (abruptly changing from positive to negative curvature at zero) and even though it's fairly hard to hear the largely even harmonics this produces, it's still a limited approach. And so, PurestWarm3 is here… but what's it doing that's different?An odd little use of 'FMA' (fused multiply-add) to produce a super high resolution version of a continuous asymmetrical distortion. One that can scale up and down to produce more or less of an effect. One that's already been heard in RetroBass, in ChannelX… now you've got it standalone, in a stripped-down and minimal plugin that only does that one thing.And it's so much purer and smoother than even the previous two, that you damn near can't hear it at all.That's the trouble with these. PurestWarm3 actually throws two harmonics, a second and a fourth. It doesn't matter much, you still can barely hear it. I've set it up so it can be exaggerated wildly, just to be heard. The challenge there (not present with the previous two) is, this is a continuous algorithm. It just reshapes everything. So what it takes from one side of the wave, it adds as peakiness to the other side, and so it will distort enthusiastically if you push it too hard, and then if you don't push it too hard you kinda can't hear what it's doing, much.Add to that the same derivative BIP tech I've been developing, and it really really stays out of the way sonically. A puzzle.So, this is not really a 'slather on thicc warmth!' effect. If you have sounds that are already asymmetrical, such as certain basses, it can reshape those to ironically make them LESS warm but cleaner. Or, you can exaggerate the effect and make it more striking. Push it too far and you're messing with a kind of expansion that won't play nicely with mixing.I hope to make use of this in a future console plugin where it'll convey the exaggerated transient spikes of certain late-70s noise reduction, so it's not like there's no use for it. Just don't expect an 'EQ-like' activity out of PurestWarm. It is rearranging energy, almost losslessly, to be a subtly different shape, and leaving next to no trace it was ever there. To some of you, this might be of great interest if that got your attention…"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class PurestWarm3 final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'puwo';
    float A;
    enum
    {
        bip_dvLA,
        bip_dvLB,
        bip_dvLC,
        bip_dvLD,
        bip_pvLA,
        bip_pvLB,
        bip_pvLC,
        bip_pvLD,
        bip_dvRA,
        bip_dvRB,
        bip_dvRC,
        bip_dvRD,
        bip_pvRA,
        bip_pvRB,
        bip_pvRC,
        bip_pvRD,
        bip_total // each distortion section can have one of these, it stacks well
    }; // not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestWarm3()
    {
        A = 0.5;
        for (int x = 0; x < bip_total; x++) {
            bip[x] = 0.0;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "asym"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Asym"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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

        double asym = (A - 0.5) * 8.0;
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
            if (asym != 0.0) {
                double bip_delta = inputSampleL; // delta can be just local and re-used
                inputSampleL *= asym;
                inputSampleL = fma(((inputSampleL * inputSampleL) * -0.166666666666666), (inputSampleL * inputSampleL), inputSampleL);
                inputSampleL /= asym;
                bip[bip_dvLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
                bip[bip_dvLB] = bip[bip_pvLA] - bip[bip_dvLA];
                bip[bip_pvLA] = bip[bip_dvLA]; // velocity
                bip[bip_dvLC] = bip[bip_pvLB] - bip[bip_dvLB];
                bip[bip_pvLB] = bip[bip_dvLB]; // acceleration
                bip[bip_dvLD] = bip[bip_pvLC] - bip[bip_dvLC];
                bip[bip_pvLC] = bip[bip_dvLC]; // jerk
                double bip_dvE = bip[bip_pvLD] - bip[bip_dvLD];
                bip[bip_pvLD] = bip[bip_dvLD]; // snap
                inputSampleL *= (1.0 + (fabs(bip[bip_dvLC]) * 0.0618) + (fabs(bip[bip_dvLD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
                bip_delta = inputSampleR; // delta can be just local and re-used
                inputSampleR *= asym;
                inputSampleR = fma(((inputSampleR * inputSampleR) * -0.166666666666666), (inputSampleR * inputSampleR), inputSampleR);
                inputSampleR /= asym;
                bip[bip_dvRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
                bip[bip_dvRB] = bip[bip_pvRA] - bip[bip_dvRA];
                bip[bip_pvRA] = bip[bip_dvRA]; // velocity
                bip[bip_dvRC] = bip[bip_pvRB] - bip[bip_dvRB];
                bip[bip_pvRB] = bip[bip_dvRB]; // acceleration
                bip[bip_dvRD] = bip[bip_pvRC] - bip[bip_dvRC];
                bip[bip_pvRC] = bip[bip_dvRC]; // jerk
                bip_dvE = bip[bip_pvRD] - bip[bip_dvRD];
                bip[bip_pvRD] = bip[bip_dvRD]; // snap
                inputSampleR *= (1.0 + (fabs(bip[bip_dvRC]) * 0.0618) + (fabs(bip[bip_dvRD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
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
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::purestwarm3
