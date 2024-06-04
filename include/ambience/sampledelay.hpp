#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sampledelay {

constexpr std::string_view k_name{ "SampleDelay" };
constexpr std::string_view k_short_description{
    "SampleDelay is three delays combined: millisecond, sample and subsample."
};
constexpr std::string_view k_long_description{
    "SampleDelay is a very simple thing, implemented in a particular way. It's not exactly an echo (though it can be), not exactly a timing adjustment tool, not exactly a sound shaper… more like, a little of all of those things.You've got up to 100 milliseconds of delay on the first control. On the second, you have 0 to 10 samples of delay, exactly. And on the third control, you have zero to one sample: you have a subsample trim. They all work separately and combine into a single delay, with an inverse/dry/wet control on the fourth slider.Why? Why like this?Sure, you can use part-dry and part-wet for a single slapback echo. Sure, you could set it to half-inverse and have total cancellation and make a comb-filter effect, or set it full wet… or full inverse… for a small timing adjustment on a multimiked setup.But let's consider that multimike setup. Suppose we're trying to get a snare mike in phase with nearby overheads. Well, one thing we could do is isolate the snare mic with an overhead, and set the snare to fully inverse (assuming the mic's phase isn't also flipped…) and dial in the timing to cancel as much as possible. Then, go to the opposite (inverse or wet) and you have your fully in phase signal.Same with multimiked guitar cabs. Find the beef by canceling it and then flipping from inverse to wet (or vice versa)Or, go for effect and have the spot mike stay inverted and don't bring it up as much, and it'll make things more bright and complex… or put it slightly off for a tonal shift. Three separately adjustable delay ranges down to the finest you could have, and the one inv/wet control to let you quickly do whatever you want.There's a reason I've been asked to bring this one back. Hope you like it. For some this will be a very boring effect. For others… not :)"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class SampleDelay final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double pL[16386];
    double pR[16386];
    int gcount;
    float A;
    float B;
    float C;
    float D;

  public:
    SampleDelay()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        for (int count = 0; count < 16385; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        gcount = 0;
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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;
            case kParamB: return 0.0;
            case kParamC: return 0.0;
            case kParamD: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "ms";
            case kParamB: return "samples";
            case kParamC: return "subsamp";
            case kParamD: return "invwet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "ms";
            case kParamB: return "Samples";
            case kParamC: return "Subsamp";
            case kParamD: return "Inv/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(floor);
            case kParamB: return std::to_string(floor);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string((D * 2.0) - 1.0);

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
            case kParamC: return "";
            case kParamD: return "";
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
        double coarse = A * (4410.0 * overallscale);
        double fine = B * 10.0;
        double subsample = C;
        int offset = floor(coarse + fine);
        if (offset > 16380) {
            offset = 16380; // insanity check
        }
        int maxtime = 16384;
        double phase = (D * 2.0) - 1.0;
        double dryLevel = 1.0 - fabs(phase);
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
            if (gcount < 0 || gcount > maxtime) {
                gcount = maxtime;
            }
            int count = gcount;
            pL[count] = inputSampleL;
            pR[count] = inputSampleR;
            count += offset;
            inputSampleL = pL[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
            inputSampleR = pR[count - ((count > maxtime) ? maxtime + 1 : 0)] * (1.0 - subsample);
            inputSampleL += pL[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
            inputSampleR += pR[count + 1 - ((count + 1 > maxtime) ? maxtime + 1 : 0)] * subsample;
            gcount--;
            if (phase < 1.0) {
                inputSampleL *= phase;
                inputSampleR *= phase;
            }
            if (dryLevel > 0.0) {
                inputSampleL += (drySampleL * dryLevel);
                inputSampleR += (drySampleR * dryLevel);
            }
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
} // namespace airwindohhs::sampledelay
