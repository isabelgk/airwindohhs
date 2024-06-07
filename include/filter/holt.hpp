#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::holt {

constexpr std::string_view k_name{ "Holt" };
constexpr std::string_view k_short_description{
    "Holt is a synth-like resonant lowpass filter focussed on bass frequencies."
};
constexpr std::string_view k_long_description{
    "Holt is a plugin suggested by one of my Q&A livestream regulars, that treads the waters of professional accountancy. The plugin, that is, not the regular. As far as I know.How is this? Simple: Holt is a method you can use in Excel for predicting sales figures based on trends. It basically uses two variables each of which chase each other to try and cut through the noise of realworld data and produce useful predictive results. My friend from the livestream thought it might make a lowpass that was more fuzzy in tonality. It did not do that thing.Instead, I got something like a low-frequency version of Aura: a resonant lowpass like a synth filter with huge control over the extreme lows. I had to do weird things to get it to track fairly consistently over different resonance settings, because the Holt method doesn’t really have anything like that at all: turning it into a synth filter is strictly my deal. So is the multipole arrangement: this thing morphs seamlessly from no poles (dry) to four poles (24 dB per octave) with intense resonance or no resonance at all, based on how you set it.It’s got an instance of Spiral built in to save you when you make it squawk, because otherwise it’ll blow up your bassbins and just laugh at you. This sucker is MEAN from the midrange on down. The interesting thing is, if you crank the frequency up it tames itself and reverts immediately to dry again. It ONLY does its madness on the lows, and high frequencies are completely tame and nice. You can use it as a sophisticated and well-behaved lowpass on the upper mids and highs, and it’s totally polite. It’s just when you drop the cutoff frequency down that it explodes in juicy bass.There are even several ways to go between that and clean, untouched audio. You can raise the cutoff, or you can use the dry/wet control, OR you can use the poles control (at any resonance setting) to morph it from bassy madness to perfect clarity… because the poles control is four different dry/wet controls bundled into one. This also means that if you’re using less than one pole of filter, you’re not even running through the other stages: less processing, unless you want it.It can act like a DJ ‘isolator’, it can act like a synth bass lowpass, it can damn near self-resonate, and all from just a couple variables (per pole) that interact strangely. It’s a neat example of extreme simplicity (like the Purest series) producing a striking result. Have fun and I hope you like it."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Holt final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double previousSampleAL;
    double previousTrendAL;
    double previousSampleBL;
    double previousTrendBL;
    double previousSampleCL;
    double previousTrendCL;
    double previousSampleDL;
    double previousTrendDL;
    double previousSampleAR;
    double previousTrendAR;
    double previousSampleBR;
    double previousTrendBR;
    double previousSampleCR;
    double previousTrendCR;
    double previousSampleDR;
    double previousTrendDR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Holt()
    {
        A = 1.0;
        B = 0.5;
        C = 1.0;
        D = 1.0;
        E = 1.0;
        previousSampleAL = 0.0;
        previousTrendAL = 0.0;
        previousSampleBL = 0.0;
        previousTrendBL = 0.0;
        previousSampleCL = 0.0;
        previousTrendCL = 0.0;
        previousSampleDL = 0.0;
        previousTrendDL = 0.0;
        previousSampleAR = 0.0;
        previousTrendAR = 0.0;
        previousSampleBR = 0.0;
        previousTrendBR = 0.0;
        previousSampleCR = 0.0;
        previousTrendCR = 0.0;
        previousSampleDR = 0.0;
        previousTrendDR = 0.0;
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
        kParamE = 4,
        kNumParameters = 5
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
        kParamE:
            E = value;
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
        kParamE:
            return E;
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
            return 1.0;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 1.0;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
            return 1.0;
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
            return "freq";
            break;
        kParamB:
            return "reso";
            break;
        kParamC:
            return "poles";
            break;
        kParamD:
            return "output";
            break;
        kParamE:
            return "dry/wet";
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
            return "Freq";
            break;
        kParamB:
            return "Reso";
            break;
        kParamC:
            return "Poles";
            break;
        kParamD:
            return "Output";
            break;
        kParamE:
            return "Dry/Wet";
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
        kParamE:
            return std::to_string(E);
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
        kParamE:
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

        double alpha = pow(A, 4) + 0.00001;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        double beta = (alpha * pow(B, 2)) + 0.00001;
        alpha += ((1.0 - beta) * pow(A, 3)); // correct for droop in frequency
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        double trend;
        double forecast; // defining these here because we're copying the routine four times
        double aWet = 1.0;
        double bWet = 1.0;
        double cWet = 1.0;
        double dWet = C * 4.0;
        // four-stage wet/dry control using progressive stages that bypass when not engaged
        if (dWet < 1.0) {
            aWet = dWet;
            bWet = 0.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 2.0) {
            bWet = dWet - 1.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 3.0) {
            cWet = dWet - 2.0;
            dWet = 0.0;
        }
        else {
            dWet -= 3.0;
        }
        // this is one way to make a little set of dry/wet stages that are successively added to the
        // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        // beyond that point: this is a way to progressively add a 'black box' sound processing
        // which lets you fall through to simpler processing at lower settings.
        double gain = D;
        double wet = E;
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
            if (aWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleAL) + ((0.999 - beta) * previousTrendAL));
                forecast = previousSampleAL + previousTrendAL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleAL = inputSampleL;
                previousTrendAL = trend;
                inputSampleL = (inputSampleL * aWet) + (drySampleL * (1.0 - aWet));
                trend = (beta * (inputSampleR - previousSampleAR) + ((0.999 - beta) * previousTrendAR));
                forecast = previousSampleAR + previousTrendAR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleAR = inputSampleR;
                previousTrendAR = trend;
                inputSampleR = (inputSampleR * aWet) + (drySampleR * (1.0 - aWet));
            }
            if (bWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleBL) + ((0.999 - beta) * previousTrendBL));
                forecast = previousSampleBL + previousTrendBL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleBL = inputSampleL;
                previousTrendBL = trend;
                inputSampleL = (inputSampleL * bWet) + (previousSampleAL * (1.0 - bWet));
                trend = (beta * (inputSampleR - previousSampleBR) + ((0.999 - beta) * previousTrendBR));
                forecast = previousSampleBR + previousTrendBR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleBR = inputSampleR;
                previousTrendBR = trend;
                inputSampleR = (inputSampleR * bWet) + (previousSampleAR * (1.0 - bWet));
            }
            if (cWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleCL) + ((0.999 - beta) * previousTrendCL));
                forecast = previousSampleCL + previousTrendCL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleCL = inputSampleL;
                previousTrendCL = trend;
                inputSampleL = (inputSampleL * cWet) + (previousSampleBL * (1.0 - cWet));
                trend = (beta * (inputSampleR - previousSampleCR) + ((0.999 - beta) * previousTrendCR));
                forecast = previousSampleCR + previousTrendCR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleCR = inputSampleR;
                previousTrendCR = trend;
                inputSampleR = (inputSampleR * cWet) + (previousSampleBR * (1.0 - cWet));
            }
            if (dWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleDL) + ((0.999 - beta) * previousTrendDL));
                forecast = previousSampleDL + previousTrendDL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleDL = inputSampleL;
                previousTrendDL = trend;
                inputSampleL = (inputSampleL * dWet) + (previousSampleCL * (1.0 - dWet));
                trend = (beta * (inputSampleR - previousSampleDR) + ((0.999 - beta) * previousTrendDR));
                forecast = previousSampleDR + previousTrendDR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleDR = inputSampleR;
                previousTrendDR = trend;
                inputSampleR = (inputSampleR * dWet) + (previousSampleCR * (1.0 - dWet));
            }
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            // clip to 1.2533141373155 to reach maximum output
            if (inputSampleL > 1.2533141373155) {
                inputSampleL = 1.2533141373155;
            }
            if (inputSampleL < -1.2533141373155) {
                inputSampleL = -1.2533141373155;
            }
            if (inputSampleR > 1.2533141373155) {
                inputSampleR = 1.2533141373155;
            }
            if (inputSampleR < -1.2533141373155) {
                inputSampleR = -1.2533141373155;
            }
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::holt
