#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestconsolechannel {

constexpr std::string_view k_name{ "PurestConsoleChannel" };
constexpr std::string_view k_short_description{
    "PurestConsoleChannel is the most free from coloration Console system."
};
constexpr std::string_view k_long_description{
    "This is also a historical reference of how I was posting on New Year's, 2018, after the death of my Mom, so I've left the post in its entirety. As follows:So. I think things may improve around here, as far as me reviewing the Console5 launch, and making sense of what the heck went on there. If I can, I’ll also give it the lushness of that original release while preventing the DC offset stuff… and there’s something to do with the AU/VST identities that needs examining. After this, ToVinyl is up for January, and I’ve got some useful variations on Console 5, and PurestDrive is February (I’m entertaining notions of a C5Drive that steals the technique from PurestDrive instead of doing the original C5 slew thing. It would be just ‘darker’, not encode/decode)The reason I think I can get into all this (after probably being sick for a while) is I’ve got some closure. I’ve just returned from visiting family and attending my Mom’s funeral. It was very nice: I sat with my Dad, who built us the Heathkit television we played Atari 2600 on, and with the brother who helped me get the VST ports together, a year ago. I cooked my Dad a big hot curry, and I think he liked it. Got to be with my siblings, and there was some healing, and I’m pretty sure after I rest up I’ll be able to think again.Good thing some plugins are so simple you don’t even have to think! :) (in other words, if THESE are broken just shoot me ;) )PurestConsole is like the dynamics encode/decode out of Console5, without the slew mojo that’s so tricky to get right. It’s a good candidate for the first plugin(s) to be open sourced when I hit the $800/mo. open source goal, along with templates I work from, and my process so people can reproduce my work. In fact I can reveal the guts of the PurestConsole source, without the Airwindows denormalization code and noise shaping to floating point. Here’s the simplest purest form of Console.Channels: inputSample = sin(inputSample);Buss: inputSample = asin(inputSample);Without all the mojo and tone changing, that is IT. Anyone building a DAW can include this (channels post fader! And do not allow the asin() to see values that’ll break it, you can get NaN out of math functions if you break them!).PurestConsole has special properties, besides ‘being in the Console5 family so you can swap them out freely with any Console5 variation and get correct results’. Since the amplitude encode/decode is most important to the effect, stripping it down to THIS simple has an interesting property I demonstrate in the video.If you have only one channel feeding the buss, you get EXACTLY that channel without the slightest alteration. PurestConsole cancels out completely and doesn’t touch the sound AT ALL unless multiple channels are mixing. If any one source becomes the only feed to the buss, it goes to perfect bit-identical fidelity to the extent of what the math function can provide. No previous version of Console can say that because I was trying to use simpler math to save CPU, but PurestConsole (and all Console5) goes for the math functions which include the complementary sin() and asin() or ‘arcsine’. That’s what arcsine is for. :)You can use PurestConsole in its capacity for ‘expanding’ verbs, delays, and EQs. If there’s no change, it’ll cancel out to bit-identical. Then if you’re doing stuff, it’ll kick in. EQ changes are most easily heard in high-Q filters, and it’ll make filters more effective at a given dB boost/cut. Note that you can easily clip PurestConsoleBuss with boosts and peaks, but that might sound OK to you so don’t fear it.I hope this simpler one is good right out of the gate, because I AM going to be sick for a while, but it might be something else to chew on, and if it is in fact so simple as to be flawless, you can work with this one right away :)"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class PurestConsoleChannel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestConsoleChannel()
    {
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

        double inputSampleL;
        double inputSampleR;
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
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
} // namespace airwindohhs::purestconsolechannel
