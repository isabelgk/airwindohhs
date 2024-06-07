#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::weight {

constexpr std::string_view k_name{ "Weight" };
constexpr std::string_view k_short_description{
    "Weight is a very accurate sub-bass boost based on Holt."
};
constexpr std::string_view k_long_description{
    "Weight is a plugin I made for me. Especially when I record live drums to simple stereo mics, or a DI bass, or indeed a guitar through an iso cab, I'll often want a little extra deep sub-bass. There's plenty of EQs I could reach for, to do that.Recently I worked on Holt2, which Weight is based on, and I found it had an ability to bring up a really vivid, resonant bass boost. It's a somewhat nonlinear algorithm, and with Holt2 I added a bunch of stages, more poles of filtering, with controls for how much resonance you wanted to get to.Weight just focuses on the 'very resonant' zone. I tuned it using Voxengo SPAN (as there's no specific formula for tuning this to any particular frequency, I had to discover what produced the right tunings) and set it up to gently go from pretty resonant, to very resonant at full crank. Weight can be tuned from 20 hz to 120 hz, which should cover a good range of sub-bass. The boost is to be applied by ear, and in many situations will be a change in character, not a big jump in overall bass level. The Weight control goes from 0 to 1, and unlike Holt it's not a dry/wet: it's added to what is otherwise a totally untouched signal, dry to dry-plus.To use this, you should have extremely good subwoofers. I'm not convinced even the best headphones can really represent what this does. The Q of the filtering (zero latency, nonlinear, unusual) comes out so sharp that you can really hunt down finely grained distinctions of bass frequencies. My Monitoring plugins set to 'Subs' or the plugin SubsOnly, can help, by focusing in on the subs in a way that overdrives them and brings the harmonics up into the audible range. But you have to be able to hear what's being done because it's very specific.The concept here is sub-bass boosting in 'areas of power' rather than just 'areas of preponderant energy' (thanks to 'Slipperman' for these concepts). To work with Weight, you will end up finding distinct frequencies for each instrument, in order to bring up subsonic weight in places where it is NOT already obvious. You'll not want to reinforce muddy deep stuff that's already there, Weight is for being able to focus in on spots where the muscle is, not just the rumble.It's a specialty tool, though variations on it are very likely to appear in other things, perhaps alongside a much broader, more easy to hear bass control. You can have Weight now: hope you like it :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Weight final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    double previousSampleL[9];
    double previousTrendL[9];
    double previousSampleR[9];
    double previousTrendR[9];

  public:
    Weight()
    {
        A = 0.5;
        B = 0.0;
        for (int i = 0; i < 8; i++) {
            previousSampleL[i] = 0.0;
            previousTrendL[i] = 0.0;
            previousSampleR[i] = 0.0;
            previousTrendR[i] = 0.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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
            return 0.0;
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
            return "weight";
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
            return "Weight";
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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "hz";
            break;
        kParamB:
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double targetFreq = A;
        // gives us a 0-1 value like the VST will be. For the VST, start with 0-1 and
        // have the plugin display the number as 20-120.
        targetFreq = ((targetFreq + 0.53) * 0.2) / sqrt(overallscale);
        // must use square root of what the real scale would be, to get correct output
        double alpha = pow(targetFreq, 4);
        double out = B;
        double resControl = (out * 0.05) + 0.2;
        double beta = (alpha * pow(resControl, 2));
        alpha += ((1.0 - beta) * pow(targetFreq, 3)); // correct for droop in frequency
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
            // begin Weight
            double trend;
            double forecast;
            for (int i = 0; i < 8; i++) {
                trend = (beta * (inputSampleL - previousSampleL[i]) + ((0.999 - beta) * previousTrendL[i]));
                forecast = previousSampleL[i] + previousTrendL[i];
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleL[i] = inputSampleL;
                previousTrendL[i] = trend;
                trend = (beta * (inputSampleR - previousSampleR[i]) + ((0.999 - beta) * previousTrendR[i]));
                forecast = previousSampleR[i] + previousTrendR[i];
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleR[i] = inputSampleR;
                previousTrendR[i] = trend;
            }
            // inputSample is now the bass boost to be added
            inputSampleL *= out;
            inputSampleR *= out;
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
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
} // namespace airwindohhs::weight
