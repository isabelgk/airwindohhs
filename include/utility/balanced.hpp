#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::balanced {

constexpr std::string_view k_name{ "Balanced" };
constexpr std::string_view k_short_description{
    "Balanced lets you run an XLR input into a stereo input, sum it to mono, and use it."
};
constexpr std::string_view k_long_description
{
    "Balanced is inspired by the adorably tiny Teenage Engineering TX-6 mixer. You might know that the TX-6 is notorious for being wildly overpriced as a small line mixer, since you can get a Bastl Dude for one-twelveth the price and have almost as many inputs (five jacks to plug in, rather than six). If you multiply that by twelve you've got sixty jacks, ten times as many inputs per dollar, and pretty much as tiny.But the TX-6 is a digital mixer, not analog. And its inputs are stereo. And it also functions as a ADC, a converter for getting stuff into your DAW (at 48k, granted, but 32 bit audio). And that means it can serve other functions, like replacing other multichannel converters… and it's normal for multichannel converters to be sort of costly.But how much can it replace? Enter Airwindows Balanced (and some patchcords you will have to make).Firstly, with breakout cables (also very expensive, from Teenage Engineering) you can get twelve inputs. You will have to process them in pairs or leave them untouched: the TX-6 will do stuff ITB on input before sending the result to your DAW, but I don't think it can operate on sides of its stereo channels, so the processing has to match.With Airwindows Balanced, you can plug a balanced, professional XLR output into your TX-6 (or ANY consumer grade stereo input of any kind) and turn it into a mono channel with all the benefits of balanced operation. In fact if your poor-quality consumer input hums or buzzes the same on both channels, Airwindows Balanced will cancel that out as if it was interference in the analog domain. It flips one of the sides and then sums to mono, with a handy gain trim based on BitShiftGain to let you gain-stage stuff that might come in very quiet.The reason that's an interesting option is this: with Airwindows Balanced and Teenage Engineering TX-6, you can record certain things at professional quality in multichannel WITHOUT professional mic pres. And that makes TX-6 the budget option. Completely aside from its tininess and portability… or the fact that everything in this portable rig is internally battery powered and can exist, in any combination of professional mic input and stereo line input, miles away from mains power.The key here is that you can get a battery-powered 48v phantom power box (I'm using Xvive P1s, which charge off a laptop or charger) and power a high-headroom and high-output condenser mic (I'm using Roswell Mini K47s), make a XLR to TRS 1/8" cable, and put the mic in front of LOUD things.The key is that the TX - 6(and some other bad quality line inputs on laptops and such) is not professional line level.It 's consumer line level. There' s no headroom, it maxes out way short of professional grade.And that means when you put your mics in front of loud things(and I mean brutally loud drums, stage amps, etc) the peaks won 't be limited by the phantom power box. They won' t clip a mic pre inside the TX - 6 as there isn 't one. And it' s a bit like trying to run your mic into a line input and getting no signal… BUT, with the right signals, you will actually get peaks within 6dB or so of clipping.Hell, if you clip the TX - 6 it doesn 't sound particularly bad, it handles clipping gracefully compared to some boutique ADCs: and it would take impossibly high loudness to do that off a mic.The budget gear you can record with can' t do that, not even close.In the 'loud into condenser mic into DAW' scenario, this setup with the Xvive P1 and TX - 6 compares most closely with APIs and Lavrys, just without 96k capability, and everything on the budget side comes off as congested and flat by comparison.My hope is that Airwindows Balanced also helps other recording situations.You should be able to take a hot XLR condenser mic, a 48V phantom power box, make the cable(these do not exist as far as I know) and a cheap old laptop with a 1 / 8 " stereo line input, and get a respectable and good sound as a mono capture… also entirely on batteries, and anywhere. My hope is that if you're not buying tiny $1000 digital mixers on a whim you can still get value out of this.But if you're able to get the TX-6 and scorned it, check out what I was able to make it do. I could have put up four other mics, all on things that would crush budget pres and converters, and carried the whole rig in a backpack including the laptop to anywhere I wanted to record, and also would never have to set a level because by definition nothing the mics can do can ever overload the inputs in a negative way. That makes setup quicker. Just saying.I can give you Balanced. You gotta make the cables yourself, as I did :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class Balanced final : public Effect<T>
{
    float A;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Balanced()
    {
        A = 0.0;
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
            return "bitshift";
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
            return "BitShift";
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
            return "bits";
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

        int bitshiftBalanced = (A * 8);
        double gain = 1.0;
        switch (bitshiftBalanced)
        {
            case 0: gain = 0.5; break;
            case 1: gain = 1.0; break;
            case 2: gain = 2.0; break;
            case 3: gain = 4.0; break;
            case 4: gain = 8.0; break;
            case 5: gain = 16.0; break;
            case 6: gain = 32.0; break;
            case 7: gain = 64.0; break;
            case 8: gain = 128.0; break;
        }
        // we are directly punching in the gain values rather than calculating them
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
            double side = inputSampleL - inputSampleR;
            // tip is left, to add negative ring (right) to combine 'em is the same as subtracting them
            // end result is, mono output is made up of half of each balanced input combined. Note that we don't just
            // flip the ring input, because we need to combine them to cancel out interference.
            inputSampleL = side * gain;
            inputSampleR = side * gain;
            // assign mono as result of balancing of channels
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
} // namespace airwindohhs::balanced
