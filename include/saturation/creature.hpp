#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::creature {

constexpr std::string_view k_name{ "Creature" };
constexpr std::string_view k_short_description{
    "Creature is a soft slew saturator, a new class of noisechanger."
};
constexpr std::string_view k_long_description{
    "So here's something new: didn't exist before, even I didn't have it.Creature is a soft slew saturator. It's a way of working with applying a sin() function to slew clipping. In fact it applies an unbounded sin() function, so it might be technically considered a slew wavefolder? Because that's what everybody needed, was a slew wavefolder. How useful, nerdy, and pointless.Not so much. Listen to this little monster.Creature is up to 32 (or more, at high sample rates) soft slew saturators, stacked up like the poles of a filter. It acts like a distortion, except it's not a distortion. It acts like a filter, but it's even less of a filter. Its interaction with sample rate is really strange (has to scale up with the square root of the sample rate multiplier!)And what Creature really does, is roar.As you keep adding Depth, the gain and the thunder increase unreasonably. The total force on tap is pretty ridiculous, and it keeps getting harder to control as you turn it up. There's an Inv control that can give you a really interesting cancellation that acts like a highpass-ish, but not like any highpass you've ever heard. Using it in phase, in Wet mode, unleashes a monstrous overdrive with humongous bass that refuses to lose weight even at impossibly high gains (real interesting on drum rooms!)There is no overdrive. There is no EQ. There is no highpass.It's just Creature, which is very much its own beast. It's also a very, very simple algorithm (isn't that so often the way?) so especially at low Depth settings, all this monstrousness can be yours for almost no CPU. I'll be finding ways to put this to use, but as always, you've got it fresh from the plugin forges. Be careful, and have fun with your new Creature."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Creature final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double slewL[102]; // probably worth just using a number here
    double slewR[102]; // probably worth just using a number here
    float A;
    float B;
    float C;

  public:
    Creature()
    {
        A = 0.26;
        B = 0.26;
        C = 1.0;
        for (int x = 0; x < 101; x++) {
            slewL[x] = 0.0;
            slewR[x] = 0.0;
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
            case kParamA: return 0.26; break;
            case kParamB: return 0.26; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive"; break;
            case kParamB: return "depth"; break;
            case kParamC: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;
            case kParamB: return "Depth"; break;
            case kParamC: return "Inv/Wet"; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double source = 1.0 - pow(1.0 - A, 5);
        int stages = (pow(B, 2) * 32.0 * sqrt(overallscale)) + 1;
        double wet = (C * 2.0) - 1.0; // inv-dry-wet for highpass
        double dry = 2.0 - (C * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
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
            for (int x = 0; x < stages; x++) {
                inputSampleL = (slewL[x] + (sin(slewL[x] - inputSampleL) * 0.5)) * source;
                slewL[x] = inputSampleL * 0.5;
                inputSampleR = (slewR[x] + (sin(slewR[x] - inputSampleR) * 0.5)) * source;
                slewR[x] = inputSampleR * 0.5;
            }
            if (stages % 2 > 0) {
                inputSampleL = -inputSampleL;
                inputSampleR = -inputSampleR;
            }
            inputSampleL *= wet;
            inputSampleR *= wet;
            drySampleL *= dry;
            drySampleR *= dry;
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
} // namespace airwindohhs::creature
