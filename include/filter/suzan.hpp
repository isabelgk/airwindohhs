#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::suzan {

constexpr std::string_view k_name{ "Suzan" };
constexpr std::string_view k_short_description{
    "Suzan is a new type of ladder filter."
};
constexpr std::string_view k_long_description{
    "So a funny thing happened to me on the way to Dattorro…I was talking about the very simple state variable filter I made, Dattorro, and how it was very handy, could even substitute for a biquad filter according to Andy from Cytomic who's an eager advocate of these (as opposed to, say, a biquad filter of direct form 1), and how I'd come up with a twist on it using only a sin() function to do the unexpected thing of subtracting the distorted bandpass element FROM the lowpass, and how that had a neat tone I liked. The twist was new, and I was excited about the possibilities.Then, on a famously crotchety forum, a guy popped up to object… saying I shouldn't call it not a biquad. It's just an SVF and not direct form 1, but it's still a biquadratic filter. You could even have literal analog filters that are biquads. I had the language wrong and he apologized for the fact that this drove him up the wall.He's Antoine Portes, and he's writing an experimental book about filters called Working Class Filter Design. Biquadratic means the transfer function's denominator is a quadratic formula, another name for second order polynomial. He's been trying to do a really good Moog ladder, which apparently is more like fourth order, and he notes that we use Q for resonance, but there's another part that goes 1/(2*Q) and this doesn't have a consistent name. He's calling it Damping, but suggests if you don't like that you can call it Suzan for all the difference it'll make to him.Naturally, I was charmed though his math is over my foolish head :) and so, I got busy again, except it was with experimenting in code rather than figuring out the math of it. And I told him I'd name the filter, meant to be experiments on this fourth-order, Moog-flavored filter, 'Suzan' in his honor.And this is the result. It's a small stack of SVFs. The difference is, Antoine understandably is interested in representing these things in correct math, and doing sensible things at least some of the time. Me on the other hand… my whole deal is being able to try things that don't exist yet. By all rights the AIs should be paying me to come up with novel things for them (maybe there'll come a day when that's a reality).And so, Suzan is not two but three SVFs in series. And the SVFs are still normal, except I'm up to my Dattorro tricks but even worse. Stage A subtracts the bandpass of stage C. Stage B subtracts the bandpass of stage A… and stage C adds the bandpass of stage B. Oh, except they're all the sines of those bandpasses, and the first two are using half the amplitude of the bandpass, the final one which adds is using the full amplitude going into the sine function.I don't claim justification for this or an analysis of why it works. Swept way up high it can have high frequency instability when hit with full blast white noise, which I hope doesn't cause issues (I got it to where it was simply a boost but didn't blow up). I find it's a nice chunky resonant peak and even gives appropriate amounts of bass restriction when resonance is turned up… and the inappropriate use of feedback across the stages does a nice job of giving a colorful texture. One I am intimately familiar with both from records, and from owning everything from a MG-1 Concertmate to a Werkstatt to a Sub Phatty. But we're not calling it that, because it isn't one of those. It's Suzan, and it's trying to bring the right kind of mojo to my filter collection. I hope you like it :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Suzan final : public Effect<T>
{
    float A;
    float B;
    float C;
    double lowAL, lowBL, lowCL;
    double bandAL, bandBL, bandCL; // these are series poles
    double lowAR, lowBR, lowCR;
    double bandAR, bandBR, bandCR; // these are series poles
    double freqA, freqB;
    double resoA, resoB;
    double outA, outB; // these are control smoothing
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Suzan()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        lowAL = lowBL = lowCL = bandAL = bandBL = bandCL = 0.0;
        lowAR = lowBR = lowCR = bandAR = bandBR = bandCR = 0.0;
        freqA = freqB = 0.5;
        resoA = resoB = 0.5;
        outA = outB = 1.0;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "freq"; break;
            case kParamB: return "reso"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Freq"; break;
            case kParamB: return "Reso"; break;
            case kParamC: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;

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
        freqA = freqB;
        resoA = resoB;
        outA = outB;
        freqB = pow(A, overallscale + 1.0) * 1.22;
        resoB = pow(1.0 - B, 2.0);
        if (resoB < 0.001) {
            resoB = 0.001; // q of 0.0 is just a tone
        }
        outB = C / sqrt(resoB);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            const double temp = (double)sampleFrames / inFramesToProcess;
            const double freq = (freqA * temp) + (freqB * (1.0 - temp));
            const double reso = (resoA * temp) + (resoB * (1.0 - temp));
            const double out = (outA * temp) + (outB * (1.0 - temp)); // dezippering
            lowAL += freq * bandAL;
            bandAL += freq * ((reso * inputSampleL) - lowAL - (reso * bandAL));
            inputSampleL = (lowAL - sin(bandCL * 0.5)); // alternate airwindowsizationA
            lowBL += freq * bandBL;
            bandBL += freq * ((reso * inputSampleL) - lowBL - (reso * bandBL));
            inputSampleL = (lowBL - sin(bandAL * 0.5)); // alternate airwindowsizationB
            lowCL += freq * bandBL;
            bandCL += freq * ((reso * inputSampleL) - lowCL - (reso * bandCL));
            inputSampleL = (lowCL + sin(bandBL)) * out; // alternate airwindowsizationC
            lowAR += freq * bandAR;
            bandAR += freq * ((reso * inputSampleR) - lowAR - (reso * bandAR));
            inputSampleR = (lowAR - sin(bandCR * 0.5)); // alternate airwindowsizationA
            lowBR += freq * bandBR;
            bandBR += freq * ((reso * inputSampleR) - lowBR - (reso * bandBR));
            inputSampleR = (lowBR - sin(bandAR * 0.5)); // alternate airwindowsizationB
            lowCR += freq * bandBR;
            bandCR += freq * ((reso * inputSampleR) - lowCR - (reso * bandCR));
            inputSampleR = (lowCR + sin(bandBR)) * out; // alternate airwindowsizationC
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
} // namespace airwindohhs::suzan
