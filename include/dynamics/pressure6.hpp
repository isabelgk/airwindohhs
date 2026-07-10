#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pressure6 {

constexpr std::string_view k_name{ "Pressure6" };
constexpr std::string_view k_short_description{
    "Pressure6 refines the Pressure compressor and how it moves."
};
constexpr std::string_view k_long_description{
    "Until now, the most recent compressor I've made was BeziComp, which takes compression and applies Bezier curves to clean up the dynamics until there are no artifacts at all… which is not what you usually get with compressors and limiters! I put it out as a plugin, and I used it in ChimeyGuitar, where it modulates things with perfect smoothness… and yet. Turns out it just doesn't seem to move right. Yes, perfect smoothness and freeness from artifact, but the way it moves seems odd.Back to the drawing board! (though as always, the previous try remains available to all, because who knows when it'll come in handy.)Pressure6 is the result of refactoring the Pressure compression algorithm as it's never been refactored before. It probably runs more efficiently on modern CPUs (which are happier doing math than they are keeping around extra variables). It also has an entirely new sound, from having isolated some of the strangest and trickiest qualities of the original Pressure. No longer is it as much of a mystery: in fact, I can probably do more variations on the theme. But this time around, my target was '2-buss Compressor'.This isn't the same as 'limiter' or 'loudenator'. It's more like 'glue' compressor. You can get it to squish things radically… if you drive it fully into dynamic inversion, as it's still a variation on Vari-Mu. But the ability to control its internal attack and release speeds (which has always been a distinguishing factor for Pressure) meant I could control how it imposed movement into a mix, way better than I'd ever been able to before.When used as a glue compressor, Pressure6 imparts dramatic movement to, and inside, a mix. Across all kinds of transient attacks, Pressure6 has a springy force that lets the music pounce forward eagerly, clarifying and heightening attacks. It highlights peak energy in a way compressors don't often manage. Used on drums, it lets sustaining elements of the kit hang steady, hypes dense parts of the beat like the snare, and heightens the impact of the kick.When you move towards lower ratios (this is a modified dry/wet) it blends in the dry signal but also starts to pad the output of the compressor, before it hits a soft saturation (like the original Pressure) to manage attack spikes. Doing this subtly raises the energy of those spikes even as you merge them with the dry signal using Ratio. The result is a compressor fit for 2-buss duties (for many, possibly not all, genres) which has a slightly touchy Compres control (as its behavior is dependent on you setting that just right) but an extremely forgiving and useful Ratio control, in which a broad range of settings sound good.I think this'll be useful and I hope you enjoy it :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pressure6 final : public Effect<T>
{
    float A;
    float B;
    double muComp;
    double muSpd;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Pressure6()
    {
        A = 0.0;
        B = 1.0;
        muComp = 1.0;
        muSpd = 100.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "compres"; break;
            case kParamB: return "ratio"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Compres"; break;
            case kParamB: return "Ratio"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
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
        double threshold = 1.0 - pow(A * 0.9, 3.0);
        double adjSpd = ((A * 92.0) + 92.0) * overallscale;
        double wet = B * 0.9;
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
            double inputSample = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSample) {
                inputSample = fabs(inputSampleR);
            }
            inputSample *= (muComp / threshold);
            inputSampleL *= (muComp / threshold);
            inputSampleR *= (muComp / threshold);
            if (fabs(inputSample) > threshold) {
                muComp *= muSpd;
                if (threshold / fabs(inputSample) < threshold) {
                    muComp += threshold * fabs(inputSample);
                }
                else {
                    muComp -= threshold / fabs(inputSample);
                }
                muComp /= muSpd;
            }
            else {
                muComp *= (muSpd * muSpd);
                muComp += ((1.1 + threshold) - fabs(inputSample));
                muComp /= (muSpd * muSpd);
            }
            muComp = fmax(fmin(muComp, 1.0), threshold);
            inputSample *= (muComp * muComp);
            inputSampleL *= (muComp * muComp);
            inputSampleR *= (muComp * muComp);
            muSpd = fmax(fmin(((muSpd * (muSpd - 1.0)) + (fabs(inputSample * adjSpd))) / muSpd, adjSpd * 2.0), adjSpd);
            inputSampleL = (drySampleL * (1.0 - (wet * 1.1))) + (sin(fmax(fmin(inputSampleL * wet, M_PI_2), -M_PI_2)) * wet);
            inputSampleR = (drySampleR * (1.0 - (wet * 1.1))) + (sin(fmax(fmin(inputSampleR * wet, M_PI_2), -M_PI_2)) * wet);
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
} // namespace airwindohhs::pressure6
