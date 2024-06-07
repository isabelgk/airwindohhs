#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console7channel {

constexpr std::string_view k_name{ "Console7Channel" };
constexpr std::string_view k_short_description{
    "Console7Channel adds anti-alias filtering and special saturation curves tied to the slider position."
};
constexpr std::string_view k_long_description{
    "If you don’t already know what Console is: it’s the Airwindows digital mix buss. You put the channel plugin on every channel, feeding directly (at unity gain) into the 2-buss where the buss plugin lives. It applies saturation and anti-saturation functions so that, for individual sounds, there is no change, but when there’s two signals interfering with each other, it makes the channels saturate easier if the buss’s ‘input impedance’ is fluctuating based on other signals coming in. You set it up, and then mix with gain trim controls or the controls on the plugins because to change the faders would violate the need for unity gain between the plugins.That’s been the case for six previous versions of Console, and now it’s Console7. Here’s what’s new, that I didn’t have before.Every stage of the Console system now runs ultrasonic filtering. Not ‘the Ultrasonic filter’, which is heavier in CPU and steeper: it’s a system designed and built for Console, optimised for use with Console. It’s a gentler, less phase-smeary version equivalent to the Isolator filter across the entire Console system, but set up backwards: the Channel plugins lead off with the steepest stage of filtering, causing highs to hit the saturation in a particular way. Then, on the Buss plugins, the remaining two stages use decreasing resonances, so the end result is as flat as Isolator’s fifth-order Butterworth filtering: but one stage runs before the processing, and one after. Doing this causes aliasing to be repeatedly removed at every step it might occur, rather than trying to whack it completely on input and then expecting the whole chain to be clean. You can still drop Ultrasonic in there, anyplace that you think needs extra attention… but this is actually better. Especially if you’re working at 192k (but it’s designed to be fantastic at 96k).Every channel and the buss now gets a dedicated seed value for the dithering to the floating point buss. This might seem (and in fact is) a mighty subtle point, but it turned out to be fine to do at no cost to the CPU of the actual mix (it’s just a little extra getting done as each plugin loads). In a (real, though kind of theoretical) sense, that means every single channel produces its own dedicated noise for dithering, even though it’s just to the floating point buss. No previous Airwindows plugin has done this, but it worked so well that it’s now the new standard for how they’re built.Every channel and the buss now has a dedicated saturation/anti-saturation algorithm that ONLY exists in Console7. It’s based off of a blend of Spiral, and Density, with the first instance of Spiral run as a ConsoleBuss algorithm, ever. They go to the trouble of blending between this new Spiral/antiSpiral sort of Console, and the Density-based one as seen in Console5 and PurestConsole, because doing this allowed a tweak in the way channels hit saturation, where the harmonics are generated in a balanced way, a smoother onset of saturation than I’ve ever had before in a plugin. Console7 channels saturate in an incredibly sweet, non-edgy way, and that’s before the ultrasonic filtering.All the channel plugins now default to 0.772 on the gain control. That, not 1.0, is ‘unity gain’. Why? Because you can now push Console channels into the red in a special way. For the first time, the gain staging is flexible and tied to the Fader controls on the plugins (the Master control on the buss also does this in its own way, but that’s normally kept at 1.0). Unlike any previous Console, and opposite to what you get if you use the DAW faders, these channels saturate MORE as you push them, and saturate LESS if you pull them back. By the way the Density algorithm works, that means stuff tends to come forward as you nudge the gain up, and drop back into the soundstage if you pull the gain back. It opens up in a very literal way when you pull channels back, like some idealized analog console. What that means is, if you use these controls (they are smoothed for zero zipper noise) stuff will practically mix itself: the mix ought to fall into place more easily and quickly, plus if you’re whacking around the controls in some mad dubby way it ought to romp with you quite delightfully! They are simple 0-1 controls specifically to get you to set them by ear: there is no such thing as ‘dB’ with these, and even if there was, you’re adjusting the saturation curves so it’s completely down to what sounds right. I recommend using control surfaces to ride these Fader controls in the plugins: this is another way to get back to (automatable) analog console days."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console7Channel final : public Effect<T>
{
    double gainchase;
    double chasespeed;
    double biquadA[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console7Channel()
    {
        A = 0.772;
        gainchase = -1.0;
        chasespeed = 64.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
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
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.772;
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
            return "fader";
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
            return "Fader";
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

        double inputgain = A * 1.272019649514069;
        // which is, in fact, the square root of 1.618033988749894848204586...
        // this happens to give us a boost factor where the track continues to get louder even
        // as it saturates and loses a bit of peak energy. Console7Channel channels go to 12! (.272,etc)
        // Neutral gain through the whole system with a full scale sine ia 0.772 on the gain knob
        if (gainchase != inputgain) {
            chasespeed *= 2.0;
        }
        if (chasespeed > sampleFrames) {
            chasespeed = sampleFrames;
        }
        if (gainchase < 0.0) {
            gainchase = inputgain;
        }
        biquadA[0] = 20000.0 / Effect<T>::getSampleRate();
        biquadA[1] = 1.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
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
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 64.0) {
                chasespeed = 64.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + inputgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= pow(gainchase, 3);
                inputSampleR *= pow(gainchase, 3);
            }
            // this trim control cuts back extra hard because we will amplify after the distortion
            // that will shift the distortion/antidistortion curve, in order to make faded settings
            // slightly 'expanded' and fall back in the soundstage, subtly
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // this is a version of Spiral blended 80/20 with regular Density ConsoleChannel.
            // It's blending between two different harmonics in the overtones of the algorithm
            if (1.0 != gainchase && 0.0 != gainchase) {
                inputSampleL /= gainchase;
                inputSampleR /= gainchase;
            }
            // we re-amplify after the distortion relative to how much we cut back previously.
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
} // namespace airwindohhs::console7channel
