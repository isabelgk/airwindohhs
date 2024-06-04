#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::chromeoxide {

constexpr std::string_view k_name{ "ChromeOxide" };
constexpr std::string_view k_short_description{
    "ChromeOxide is an alternate path to vibey old tape sonics."
};
constexpr std::string_view k_long_description{
    "Chrome Oxide was an experiment, one that didn’t go further than this. It is a dual-band tape emulation, where the lows are a bit saturated but the highs are delayed by a random noise warble that can also be biased to delay them a bit further. My pursuits of tape emulation have always gone toward the ‘BETTER than digital’ direction, where I tried to capture the magic without diving into the audio degradation.But revisiting Chrome Oxide (and re-releasing it, with modern wordlength handling etc and dithering to the floating point output buss) showed me a plugin that excels at some tonalities I didn’t even know about when I made it. For instance, your Boards of Canada type stuff, mulch-core audio where it sounds like it’s coming off an old Walkman or Wollensack? This will not do crazy pitch wobbles or dropouts… but you can instantly, effortlessly get the tone of it. The intensity controls a noise effect that is FM, frequency modulating the highs against the lows. Bias further delays the highs, and this sculpts the phase aberrations of the output and the flavor of roll-off… so, without ever getting aggressive or obvious, you can just dial-a-mulch and go as fuzzy and old-sounding as you like, but musically. It is subtle enough to use on anything and aggressive enough to completely change the mood of a track.And now you can have it. Mulch away! You don’t have to obliterate a track to get into the vibe you crave. (and of course some people hate this sort of thing: if you doubly hate this one, I’ll know I’ve done it right :D )"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class ChromeOxide final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double secondSampleL;
    double thirdSampleL;
    double fourthSampleL;
    double fifthSampleL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double secondSampleR;
    double thirdSampleR;
    double fourthSampleR;
    double fifthSampleR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    ChromeOxide()
    {
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        secondSampleL = 0.0;
        thirdSampleL = 0.0;
        fourthSampleL = 0.0;
        fifthSampleL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        secondSampleR = 0.0;
        thirdSampleR = 0.0;
        fourthSampleR = 0.0;
        fifthSampleR = 0.0;
        flip = false;
        A = 0.5;
        B = 0.5;
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
        kParamiirSampleAL = 0,
        kParamiirSampleBL = 1,
        kNumParameters = 2

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: iirSampleAL = value; break;
            case kParamiirSampleBL: iirSampleBL = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return iirSampleAL;
            case kParamiirSampleBL: return iirSampleBL;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return 0.0;
            case kParamiirSampleBL: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return "intense";
            case kParamiirSampleBL: return "bias";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return "Intense";
            case kParamiirSampleBL: return "Bias";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return std::to_string(A);
            case kParamiirSampleBL: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamiirSampleAL: return "";
            case kParamiirSampleBL: return "";
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
        double bassSampleL;
        double bassSampleR;
        double randy;
        double bias = B / 1.31578947368421;
        double intensity = 0.9 + pow(A, 2);
        double iirAmount = pow(1.0 - (intensity / (10 + (bias * 4.0))), 2) / overallscale;
        // make 10 higher for less trashy sound in high settings
        double bridgerectifier;
        double trebleGainTrim = 1.0;
        double indrive = 1.0;
        double densityA = (intensity * 80) + 1.0;
        double noise = intensity / (1.0 + bias);
        double bassGainTrim = 1.0;
        double glitch = 0.0;
        bias *= overallscale;
        noise *= overallscale;
        if (intensity > 1.0)
        {
            glitch = intensity - 1.0;
            indrive = intensity * intensity;
            bassGainTrim /= (intensity * intensity);
            trebleGainTrim = (intensity + 1.0) / 2.0;
        }
        // everything runs off Intensity now
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
            inputSampleL *= indrive;
            inputSampleR *= indrive;
            bassSampleL = inputSampleL;
            bassSampleR = inputSampleR;
            if (flip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // highpass section
            bassSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch));
            bassSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch));
            // overdrive the bass channel
            if (flip)
            {
                iirSampleCL = (iirSampleCL * (1 - iirAmount)) + (bassSampleL * iirAmount);
                iirSampleCR = (iirSampleCR * (1 - iirAmount)) + (bassSampleR * iirAmount);
                bassSampleL = iirSampleCL;
                bassSampleR = iirSampleCR;
            }
            else
            {
                iirSampleDL = (iirSampleDL * (1 - iirAmount)) + (bassSampleL * iirAmount);
                iirSampleDR = (iirSampleDR * (1 - iirAmount)) + (bassSampleR * iirAmount);
                bassSampleL = iirSampleDL;
                bassSampleR = iirSampleDR;
            }
            // bass filter same as high but only after the clipping
            flip = !flip;
            bridgerectifier = inputSampleL;
            // insanity check
            randy = bias + ((double(fpdL) / UINT32_MAX) * noise);
            if ((randy >= 0.0) && (randy < 1.0)) {
                bridgerectifier = (inputSampleL * randy) + (secondSampleL * (1.0 - randy));
            }
            if ((randy >= 1.0) && (randy < 2.0)) {
                bridgerectifier = (secondSampleL * (randy - 1.0)) + (thirdSampleL * (2.0 - randy));
            }
            if ((randy >= 2.0) && (randy < 3.0)) {
                bridgerectifier = (thirdSampleL * (randy - 2.0)) + (fourthSampleL * (3.0 - randy));
            }
            if ((randy >= 3.0) && (randy < 4.0)) {
                bridgerectifier = (fourthSampleL * (randy - 3.0)) + (fifthSampleL * (4.0 - randy));
            }
            fifthSampleL = fourthSampleL;
            fourthSampleL = thirdSampleL;
            thirdSampleL = secondSampleL;
            secondSampleL = inputSampleL;
            // high freq noise/flutter section
            inputSampleL = bridgerectifier;
            // apply overall, or just to the distorted bit? if the latter, below says 'fabs bridgerectifier'
            bridgerectifier = inputSampleR;
            // insanity check
            randy = bias + ((double(fpdR) / UINT32_MAX) * noise);
            if ((randy >= 0.0) && (randy < 1.0)) {
                bridgerectifier = (inputSampleR * randy) + (secondSampleR * (1.0 - randy));
            }
            if ((randy >= 1.0) && (randy < 2.0)) {
                bridgerectifier = (secondSampleR * (randy - 1.0)) + (thirdSampleR * (2.0 - randy));
            }
            if ((randy >= 2.0) && (randy < 3.0)) {
                bridgerectifier = (thirdSampleR * (randy - 2.0)) + (fourthSampleR * (3.0 - randy));
            }
            if ((randy >= 3.0) && (randy < 4.0)) {
                bridgerectifier = (fourthSampleR * (randy - 3.0)) + (fifthSampleR * (4.0 - randy));
            }
            fifthSampleR = fourthSampleR;
            fourthSampleR = thirdSampleR;
            thirdSampleR = secondSampleR;
            secondSampleR = inputSampleR;
            // high freq noise/flutter section
            inputSampleR = bridgerectifier;
            // apply overall, or just to the distorted bit? if the latter, below says 'fabs bridgerectifier'
            bridgerectifier = fabs(inputSampleL) * densityA;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier / densityA;
            }
            else {
                inputSampleL = -bridgerectifier / densityA;
            }
            // blend according to densityA control
            bridgerectifier = fabs(inputSampleR) * densityA;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier / densityA;
            }
            else {
                inputSampleR = -bridgerectifier / densityA;
            }
            // blend according to densityA control
            inputSampleL *= trebleGainTrim;
            inputSampleR *= trebleGainTrim;
            bassSampleL *= bassGainTrim;
            bassSampleR *= bassGainTrim;
            inputSampleL += bassSampleL;
            inputSampleR += bassSampleR;
            // that simple.
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
} // namespace airwindohhs::chromeoxide
