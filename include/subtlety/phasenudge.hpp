#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::phasenudge {

constexpr std::string_view k_name{ "PhaseNudge" };
constexpr std::string_view k_short_description{
    "PhaseNudge is a phase rotator/allpass filter."
};
constexpr std::string_view k_long_description{
    "Here’s a simple little utility plugin, Airwindows-ized. Except, it seems like this isn’t part of typical DAWs and plugin collections. Can’t see why, it’s a pretty basic tool.In radio, there was the need for a phase rotator, to make waveforms more symmetrical for loudness maximizing. In reverbs, you get a thing called an allpass filter (a kind of feedback delay at a specific calibration) which diffuses the sound so it can be fed to delay banks and seem more spatial. Turns out these are the same basic thing! It’s also in phase shifters (mixing the phase-shifted part against dry, or inverse dry).What happens? When you use an allpass filter, what you get is all the frequencies exactly as loud as they were before, but the phases of the frequencies are all different. Specifically, lows get delayed relative to the highs producing an effect where tones are ‘smeared’ across a time stretch, even though the spectral contents are exactly the same. The frequency information’s unaltered (nothing’s out of tune or darkened/brightened) but there’s a blur, possibly a large blur. PhaseNudge is set up to produce delays from really short (normal for a phase rotator) to unusually long, in case you’d like to treat it as a kind of slapback/echo effect.The calibration of PhaseNudge is finer than you usually find in an allpass: 0.618 is the customary number but when I see that I think ‘golden ratio’, so that’s what PhaseNudge is using, to very high accuracy. Also, PhaseNudge uses a variation on the operating principle of Console to expand and deepen the sound. Though typical allpasses use very short delays, I think PhaseNudge does its thing quite well across a broad range of delays. Anywhere you need a ‘defocus’ or ‘blur’ plugin, PhaseNudge should come in handy, whether it’s diffuse pads, overly pointy percussive elements, or even the effects loop of a lead guitar sound (phase shifters have been used for decades, to make the textures of leads more fluid before they hit the actual amp. You’ve heard this on ‘Eruption’ and may not have even known it, because it’s very subtle there)This is a real fundamental building-block tool in digital audio, and if DAW makers will not include it as part of standard equipment, I will. ;)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class PhaseNudge final : public Effect<T>
{
    double dL[1503];
    double dR[1503];
    int one, maxdelay;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    PhaseNudge()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 1502; count++) {
            dL[count] = 0.0;
            dR[count] = 0.0;
        }
        one = 1;
        maxdelay = 9001;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "phase nudge"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Phase Nudge"; break;
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

        int allpasstemp;
        double outallpass = 0.618033988749894848204586; // golden ratio!
        // if you see 0.6180 it's not a wild stretch to wonder whether you are working with a constant
        int maxdelayTarget = (int)(pow(A, 3) * 1501.0);
        double wet = B;
        // removed extra dry variable
        double bridgerectifier;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            inputSampleL /= 4.0;
            inputSampleR /= 4.0;
            bridgerectifier = fabs(inputSampleL);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            bridgerectifier = fabs(inputSampleR);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            if (fabs(maxdelay - maxdelayTarget) > 1500) {
                maxdelay = maxdelayTarget;
            }
            if (maxdelay < maxdelayTarget) {
                maxdelay++;
                dL[maxdelay] = (dL[0] + dL[maxdelay - 1]) / 2.0;
                dR[maxdelay] = (dR[0] + dR[maxdelay - 1]) / 2.0;
            }
            if (maxdelay > maxdelayTarget) {
                maxdelay--;
                dL[maxdelay] = (dL[0] + dL[maxdelay]) / 2.0;
                dR[maxdelay] = (dR[0] + dR[maxdelay]) / 2.0;
            }
            allpasstemp = one - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay) {
                allpasstemp = maxdelay;
            }
            inputSampleL -= dL[allpasstemp] * outallpass;
            inputSampleR -= dR[allpasstemp] * outallpass;
            dL[one] = inputSampleL;
            dR[one] = inputSampleR;
            inputSampleL *= outallpass;
            inputSampleR *= outallpass;
            one--;
            if (one < 0 || one > maxdelay) {
                one = maxdelay;
            }
            inputSampleL += (dL[one]);
            inputSampleR += (dR[one]);
            bridgerectifier = fabs(inputSampleL);
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL -= bridgerectifier;
            }
            else {
                inputSampleL += bridgerectifier;
            }
            bridgerectifier = fabs(inputSampleR);
            bridgerectifier = 1.0 - cos(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR -= bridgerectifier;
            }
            else {
                inputSampleR += bridgerectifier;
            }
            inputSampleL *= 4.0;
            inputSampleR *= 4.0;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
} // namespace airwindohhs::phasenudge
