#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::guitarconditioner {

constexpr std::string_view k_name{ "GuitarConditioner" };
constexpr std::string_view k_short_description{
    "GuitarConditioner is like a Tube Screamer voicing without the squishiness and indistinctness."
};
constexpr std::string_view k_long_description{
    "I designed this plugin when I saw what Slew was capable of. It uses a combination of Slew and Highpass in a sort of parallel matrix arrangement, with heavy use of my Highpass’s unusual tone shaping features (extreme lows get Tight tone shaping behavior, and a boosted mids circuit is set to Loose rolloff for the proper texture). Both aspects hit their own Slew at different levels and with different voicings, causing a distinct tone quality.This is not a Tube Screamer plugin at all. It’s an alternate tone for doing similar things but with more sonority, focus and impact. It’s intended for Djent style sounds and people who can play much, much better and more aggressively than me :)The reason Slew is relevant to a Tube Screamer effect is, the guitar pedal uses op-amps that can be limited in their ability to deliver slew and bass. Therefore, they don't only distort and make louder, they also restrict treble and bass in a nonlinear way, and that's the same thing my Slew effect does. So Guitar Conditioner is Airwindows techniques addressing the same purpose."
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class GuitarConditioner final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    bool fpFlip;
    double lastSampleTL;
    double lastSampleBL; // for Slews
    double iirSampleTAL;
    double iirSampleTBL;
    double iirSampleBAL;
    double iirSampleBBL; // for Highpasses
    double lastSampleTR;
    double lastSampleBR; // for Slews
    double iirSampleTAR;
    double iirSampleTBR;
    double iirSampleBAR;
    double iirSampleBBR; // for Highpasses

  public:
    GuitarConditioner()
    {
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        fpFlip = true;
        lastSampleTL = 0.0;
        lastSampleBL = 0.0; // for Slews. T for treble, B for bass
        iirSampleTAL = 0.0;
        iirSampleTBL = 0.0;
        iirSampleBAL = 0.0;
        iirSampleBBL = 0.0; // for Highpasses
        // this is reset: values being initialized only once. Startup values, whatever they are.
        lastSampleTR = 0.0;
        lastSampleBR = 0.0; // for Slews. T for treble, B for bass
        iirSampleTAR = 0.0;
        iirSampleTBR = 0.0;
        iirSampleBAR = 0.0;
        iirSampleBBR = 0.0; // for Highpasses
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
        double inputSampleL;
        double inputSampleR;
        double trebleL;
        double bassL;
        double trebleR;
        double bassR;
        double iirTreble = 0.287496 / overallscale; // tight is -1
        double iirBass = 0.085184 / overallscale; // tight is 1
        iirTreble += iirTreble;
        iirBass += iirBass; // simple double when tight is -1 or 1
        double tightBass = 0.6666666666;
        double tightTreble = -0.3333333333;
        double offset;
        double clamp;
        double threshTreble = 0.0081 / overallscale;
        double threshBass = 0.0256 / overallscale;
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
            trebleL = bassL = inputSampleL;
            trebleR = bassR = inputSampleR;
            trebleL += trebleL; //+3dB on treble as the highpass is higher
            trebleR += trebleR; //+3dB on treble as the highpass is higher
            offset = (1 + tightTreble) + ((1 - fabs(trebleL)) * tightTreble); // treble HP
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1; // made offset for HP
            }
            if (fpFlip) {
                iirSampleTAL = (iirSampleTAL * (1 - (offset * iirTreble))) + (trebleL * (offset * iirTreble));
                trebleL = trebleL - iirSampleTAL;
            }
            else {
                iirSampleTBL = (iirSampleTBL * (1 - (offset * iirTreble))) + (trebleL * (offset * iirTreble));
                trebleL = trebleL - iirSampleTBL;
            } // done trebleL HP
            offset = (1 + tightTreble) + ((1 - fabs(trebleR)) * tightTreble); // treble HP
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1; // made offset for HP
            }
            if (fpFlip) {
                iirSampleTAR = (iirSampleTAR * (1 - (offset * iirTreble))) + (trebleR * (offset * iirTreble));
                trebleR = trebleR - iirSampleTAR;
            }
            else {
                iirSampleTBR = (iirSampleTBR * (1 - (offset * iirTreble))) + (trebleR * (offset * iirTreble));
                trebleR = trebleR - iirSampleTBR;
            } // done trebleR HP
            offset = (1 - tightBass) + (fabs(bassL) * tightBass); // bass HP
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip) {
                iirSampleBAL = (iirSampleBAL * (1 - (offset * iirBass))) + (bassL * (offset * iirBass));
                bassL = bassL - iirSampleBAL;
            }
            else {
                iirSampleBBL = (iirSampleBBL * (1 - (offset * iirBass))) + (bassL * (offset * iirBass));
                bassL = bassL - iirSampleBBL;
            } // done bassL HP
            offset = (1 - tightBass) + (fabs(bassR) * tightBass); // bass HP
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip) {
                iirSampleBAR = (iirSampleBAR * (1 - (offset * iirBass))) + (bassR * (offset * iirBass));
                bassR = bassR - iirSampleBAR;
            }
            else {
                iirSampleBBR = (iirSampleBBR * (1 - (offset * iirBass))) + (bassR * (offset * iirBass));
                bassR = bassR - iirSampleBBR;
            } // done bassR HP
            inputSampleL = trebleL;
            clamp = inputSampleL - lastSampleTL;
            if (clamp > threshTreble) {
                trebleL = lastSampleTL + threshTreble;
            }
            if (-clamp > threshTreble) {
                trebleL = lastSampleTL - threshTreble;
            }
            lastSampleTL = trebleL; // trebleL slew
            inputSampleR = trebleR;
            clamp = inputSampleR - lastSampleTR;
            if (clamp > threshTreble) {
                trebleR = lastSampleTR + threshTreble;
            }
            if (-clamp > threshTreble) {
                trebleR = lastSampleTR - threshTreble;
            }
            lastSampleTR = trebleR; // trebleR slew
            inputSampleL = bassL;
            clamp = inputSampleL - lastSampleBL;
            if (clamp > threshBass) {
                bassL = lastSampleBL + threshBass;
            }
            if (-clamp > threshBass) {
                bassL = lastSampleBL - threshBass;
            }
            lastSampleBL = bassL; // bassL slew
            inputSampleR = bassR;
            clamp = inputSampleR - lastSampleBR;
            if (clamp > threshBass) {
                bassR = lastSampleBR + threshBass;
            }
            if (-clamp > threshBass) {
                bassR = lastSampleBR - threshBass;
            }
            lastSampleBR = bassR; // bassR slew
            inputSampleL = trebleL + bassL; // final merge
            inputSampleR = trebleR + bassR; // final merge
            fpFlip = !fpFlip;
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
} // namespace airwindohhs::guitarconditioner
