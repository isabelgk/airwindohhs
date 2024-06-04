#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::desk {

constexpr std::string_view k_name{ "Desk" };
constexpr std::string_view k_short_description{
    "Desk4 is distinctive analog coloration (a tuneable version of the control-less Desk plugins)"
};
constexpr std::string_view k_long_description{
    "Desk is a little bit like a precursor to ‘PurestDrive’. It’s entirely an analog modeler, with a behavior that’s far from a plugin stomp-box distortion. It glues and thickens the sound, with a headroom of about 30 dB, much like a true analog console (those don’t turn into distorto-pedals the instant you hit 0 dB). Desk is the canonical version of this type of sound processing.Note that it’s not tricky to use like Console4. It’s not calibrated to work as a ConsoleChannel replacement (that’s the most recent Desk3, which is one of the Kagi plugins to be released later). It doesn’t have elaborate tone colors added like BussColors3 (also a Kagi plugin). It’s not obvious in normal use, not adjustable like some of these plugins. In normal operation you’re miles from clipping it anyway, and get only a subtle glue and tone shaping.But what it IS… is classic Airwindows tone coloring brought up to 2017 standards. You can stick Desk on any channel, any submix or buss, the 2-buss, or all of the above. The more places you put it, the more obvious the analogifying becomes. And since it’s Airwindows tone and transparency, you can put it up against any ‘analog modeler’ by anybody, at any price or subscription fee, and it should beat everything and give a bigger, punchier sound. If it’s ‘analog warming’ and tone you want, and you require the sound not to turn into digital sludge and glitter, this is the one.As a further note, the reason I've been willing to engage in hype like the above is a fundamental Airwindows principle: less is more. What Desk does is very simple and minimal. Doing less to the digital signal generally hurts the tone less. So, as long as you're getting enough 'analog tone shaping' cues in the particular ways Desk does them, it is actually true that if you find a plugin that does this as minimally as possible, with the fewest math operations, it is likely to sound bigger and punchier and better than more elaborate processing.Though I’ve put out BussColors to mimic existing audio hardware, it was always my intention to create analog-ifying plugins that weren’t about cloning existing gear: that produced their own distinctive sound. The first Desk plugins (Desk, TransDesk, TubeDesk) were made in this way, using audio DSP which isn’t typical.As this line of experimentation evolved, it led me to what we’ve got here. Desk4 is the latest refinement of the Desk line, now for Mac and PC VST (as well as AU)… and free.The drive control is a boost as you might expect. Turn it up for more slam and dirt. It’s very soft, textured, rich-in-nutrients dirt, but it’s basically ‘distortion’.Treble Choke is more unusual: don’t overcrank this control or you’ll generate artifacts such as uncontrolled DC. It’s not a normal algorithm and not a traditional EQ or even a saturation: as you can tell from the weird behavior when you crank it. Use it subtly and you’ll have a brightness conditioner not found outside quality analog gear. Since it’s a plugin, you can also push the extremes of the behavior, just don’t get too carried away. It’s designed to let you break it with extreme settings, so it’ll be flexible across different kinds of audio.The power sag and frequency controls are the heart of some behaviors in the earlier TubeDesk and TransDesk: you can make your imaginary analog hardware overload its power supply. Cranking the frequency slider moves the area of interest down, for tube power supply sag behaviors. Tiny settings work over a tiny range of samples, causing the effect to hit higher frequencies. If you hear an obvious effect, you’re probably applying too much… unless you intentionally want to crap out the audio, in which case this is a uniquely aggressive way of doing that. It’ll add grunge in an entirely different way from simple distortion, so you can do both."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Desk final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double lastOutSampleL;
    double lastSlewL;
    double lastSampleR;
    double lastOutSampleR;
    double lastSlewR;

  public:
    Desk()
    {
        lastSampleL = 0.0;
        lastOutSampleL = 0.0;
        lastSlewL = 0.0;
        lastSampleR = 0.0;
        lastOutSampleR = 0.0;
        lastSlewR = 0.0;
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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double gain = 0.135;
        double slewgain = 0.208;
        double prevslew = 0.333;
        double balanceB = 0.0001;
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        slewgain *= overallscale;
        prevslew *= overallscale;
        balanceB /= overallscale;
        double balanceA = 1.0 - balanceB;
        double slew;
        double bridgerectifier;
        double combsample;
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
            // begin L
            slew = inputSampleL - lastSampleL;
            lastSampleL = inputSampleL;
            // Set up direct reference for slew
            bridgerectifier = fabs(slew * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slew > 0) {
                slew = bridgerectifier / slewgain;
            }
            else {
                slew = -(bridgerectifier / slewgain);
            }
            inputSampleL = (lastOutSampleL * balanceA) + (lastSampleL * balanceB) + slew;
            // go from last slewed, but include some raw values
            lastOutSampleL = inputSampleL;
            // Set up slewed reference
            combsample = fabs(drySampleL * lastSampleL);
            if (combsample > 1.0) {
                combsample = 1.0;
            }
            // bailout for very high input gains
            inputSampleL -= (lastSlewL * combsample * prevslew);
            lastSlewL = slew;
            // slew interaction with previous slew
            inputSampleL *= gain;
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // drive section
            inputSampleL /= gain;
            // end L
            // begin R
            slew = inputSampleR - lastSampleR;
            lastSampleR = inputSampleR;
            // Set up direct reference for slew
            bridgerectifier = fabs(slew * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slew > 0) {
                slew = bridgerectifier / slewgain;
            }
            else {
                slew = -(bridgerectifier / slewgain);
            }
            inputSampleR = (lastOutSampleR * balanceA) + (lastSampleR * balanceB) + slew;
            // go from last slewed, but include some raw values
            lastOutSampleR = inputSampleR;
            // Set up slewed reference
            combsample = fabs(drySampleR * lastSampleR);
            if (combsample > 1.0) {
                combsample = 1.0;
            }
            // bailout for very high input gains
            inputSampleR -= (lastSlewR * combsample * prevslew);
            lastSlewR = slew;
            // slew interaction with previous slew
            inputSampleR *= gain;
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // drive section
            inputSampleR /= gain;
            // end R
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
} // namespace airwindohhs::desk
