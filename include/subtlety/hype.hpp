#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hype {

constexpr std::string_view k_name{ "Hype" };
constexpr std::string_view k_short_description{
    "HypersonX is like UltrasonX but with seven stages instead of five."
};
constexpr std::string_view k_long_description{
    "I may have mentioned that Console8 attempted a treble softening thing that went terribly wrong and acted like an aural exciter?This is it :)No controls, and in fact it fought my attempts to add any. If you use external controls, for instance the dry/wet you can get in Reaper, it will not behave nicely and will do weird things at intermediate settings. I have no idea what will happen if you try and oversample it: it, and you, may die, so you have been warned. This is the Airwindows experiment that went TOO weird to use as the treble softening in Console8, which is already pushing the limits of reasonableness in certain ways.Can’t even describe this, just drop it into your mix anywhere and see what it does. It’s like it tries to do a treble soften, but doesn’t get the concept entirely. In the video I made, you can see that stacking about six to eight of these up gets you into obvious distorted crackling territory. The original use in Console8 had three of these stacked, and I knew there was trouble when instead of softening the tone subtly and nicely, it was like a light turned on inside things. Don’t stack up too many of them if you want things to be more reasonable.I also can’t blend between this and the final Console8 treble softening. Nor am I interested in adding a switch so all Console8 (‘in’ plugs) have this, it’s just too eccentric, more like what Console7Cascade was, except Console8 now leans a little bit in the direction of what Console7Cascade was. That said, simply adding this to Console8 doesn’t replicate what the original experiment did, this is just that part of it stripped out and isolated in a sort of featureless plugin test-tube so you can play with the effect all by itself.Hypersonic is the same as Ultrasonic, except it’s running seven stages of filtering instead of five, and it cuts off a little higher: for these reasons, it’s the Airwindows ultrasonic filter that ought to not soften the super-highs even for golden ears and sparkling youths :) I don’t think the original Ultrasonic sounds bad and I use its type of filtering in my work a lot, but I also like depth in sound and the presentation of a soundstage that’s not too upfront and close. It makes sense that the slight softening of super-bright frequencies that original Ultrasonic gives you, wouldn’t work for everybody: not everyone wants ‘glue’ or ‘analog’ or ‘recording console’, sometimes you just want the purest digital you can get.I think there’s a fair chance this filter will work out for folks whose aesthetic leans toward the superultramegahyperbright, even if that ain’t me. It combines enough filter stages to act like a 14th-order Butterworth, and it cuts off just high enough beyond 20K that you should get everything up to insect-repellent audio, unsoftened.I’m also working on something else that may give rise to big changes (I know, it’s like 2022 is all the promised big changes, but this one might be extra interesting). Compare Ultrasonic to Hypersonic… CPU-wise. I am pretty sure that I’ve got Hypersonic outperforming Ultrasonic on every possible platform, even though it has two more filter stages than Ultrasonic. The question is, am I making a choice that is hurting the sound?A bit of background. There are a lot of devs who like using 32-bit floats for audio math. They’re fast, can be calculated in parallel… but everything you do with them, you erode away louder samples by constantly truncating your internal variables to roughly 25 bits. You’ll never hear that doing it once… but for every little calculation, which could be thousands or tens of thousands? I think that’s a hidden difference between digital audio and analog audio, and I think it matters.So much so, that I run an internal buss in my plugins that is 1,099,511,627,776 times more high-resolution than that. It’s served me well, and I like the sound I get. But I’ve also run across folks who use a slightly different arrangement: they’re not dithering floating point like I am, but they ARE using a high-res buss. How high res? 536,870,912 times better than the more efficient floats, that’s how high res. Now, what I’ve been doing is one trillion ninety-nine billion five hundred eleven million six hundred twenty-seven thousand seven hundred seventy-six times more high-res than your average float. But who’s to say that we can’t get by with only five hundred million times better than your average float? :DAnd so, we have Hypersonic. It’s using more filters than Ultrasonic, but running faster, and it’s still five hundred million times more accurate than your average float-based audio software, and it still dithers to a 32-bit buss such as CoreAudio on the Mac. But hey, if you’re running Reaper or something else with a double precision buss, it doesn’t have to dither at all as it’s working natively at that resolution! So my challenge is this: does anybody, myself included, hear ANYTHING more truncate-ey or degraded about Hypersonic when compared to Ultrasonic, even though it’s running more stages of filtering? I challenge you, tell me if you think you can hear a drop in quality here. I’m all about the overkill, but I suspect I can get there on the native Reaper 64-bit buss: and of course everything AU or otherwise that does run a 32-bit buss gets dithered to that buss whether it’s Mac or PC or Linux, and I’m thinking I can start to bring some really serious performance gains to everything. I will of course keep an archive of all the Airwindows plugins as they existed before re-hacking them in this new way, so you will be able to have access to the previous plugins (of all kinds) that ran the ultra-super-hyper-overkill audio buss. Might be desirable for mastering folks, or those who are incredibly fussy about analog-style sounds. I’ve been doing a fine job of keeping the audio busses cranked wide open to ultimate resolution for years, and the whole Airwindows library is like that.But if my suspicions are correct, we can have that as backup and then also have the whole Airwindows library running WAY faster.And then when we move on to the new format CLAP, and begin to bring out select plugins with GUI and interfaces and metering, they can also run their audio stuff at that nice happy medium between high efficiency and no-compromise audio buss.(followup: I did in fact end up re-hacking the entire Airwindows plugins collection on all platforms as double precision rather than long double. Though on some processors like the Mac M1 it weirdly turned out that the long doubles were fine and didn't cost me extra CPU, hence I didn't gain anything much from the change on my own systems)This one’s pretty straightforward. If you know UltrasonX, this is the same thing but for Hypersonic. These are ultrasonic filters for use at high sample rates such as 96k: for less processing than it would take to oversample a bunch of stuff you can run the whole mix at an elevated sample rate and put filtering between nonlinear stages: it works better to do a little filtering in many places, and it helps the mix from seeming overprocessed.HypersonX is steeper, because it’s seven stages instead of only five. It cuts off a little higher, so it should go right past 20k without hinting at rolling off or softening anything. But since it (like Hypersonic itself) is seven stages, that means you’ve got to find seven spots in your mix to stick the respective stages of the filter.Like UltrasonX (and like future Console versions I do that will use this technology), you have to have one each of every stage of HypersonX, from input to final 2-buss. That means you don’t have to repeat ’em across all channels: for instance, if you’re running stage A and B on the channels, and C, D and E on submixes, and F and G on the 2-buss, that’s way less filters than you’d have if you placed just one Hypersonic on the start of every single channel, AND it will work better because it’ll keep every little nonlinear stage in check. Otherwise, they can seize on small bits of aliasing and alias them further until they combine and become obvious. Single filtering at the input won’t protect you from this. Distributed filtering will.This might not be for everybody. The thing is, if you’re running at high sample rates and you want to resist aliasing and have a warm, analog tone that still retains clarity, this might be for you… and similarly to other Airwindows inventions, the cumbersome nature of HypersonX could become your secret weapon. Who wants to carefully arrange to have one each of seven types of inaudible filtering across your signal path, from channel start to end of 2-buss? Possibly you, if the resulting tone speaks to your musical tastes. The barrier to entry is a setup that’s a pain in the butt and fussy rules for how it works (you can combine it with the original PurestConsole if you’d like to also have the cleanest possible Airwindows Console mix, that’s a whole other set of fussy rules!) but you might find the results a little spectacular.I hope so! The next thing for me is building this into the next version(s) of Console, and I believe it will help things a whole lot. But with HypersonX, you don’t have to wait. Use it on its own or combine it with any earlier version of Console (not Console7 or PurestConsole2, which have their own filtering, but Console 5 or 6 or PurestConsole or Atmosphere or PDConsole which didn’t have ultrasonic filtering built in) and begin constructing your own in-house mixing board, and get your personal sound.(followup: this became the ultrasonic filter tech for Console8)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Hype final : public Effect<T>
{
    double softL[11];
    double softR[11];
    int cycleEnd;
    // from undersampling code, used as a way to space out HF taps
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Hype()
    {
        for (int count = 0; count < 10; count++) {
            softL[count] = 0.0;
            softR[count] = 0.0;
        }
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd == 3) {
            cycleEnd = 4;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 4 for 176 or 192k
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
            if (cycleEnd == 4) {
                softL[8] = softL[7];
                softL[7] = softL[6];
                softL[6] = softL[5];
                softL[5] = softL[4];
                softL[4] = softL[3];
                softL[3] = softL[2];
                softL[2] = softL[1];
                softL[1] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[7];
                softR[7] = softR[6];
                softR[6] = softR[5];
                softR[5] = softR[4];
                softR[4] = softR[3];
                softR[3] = softR[2];
                softR[2] = softR[1];
                softR[1] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 2) {
                softL[8] = softL[6];
                softL[6] = softL[4];
                softL[4] = softL[2];
                softL[2] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[6];
                softR[6] = softR[4];
                softR[4] = softR[2];
                softR[2] = softR[0];
                softR[0] = inputSampleR;
            }
            if (cycleEnd == 1) {
                softL[8] = softL[4];
                softL[4] = softL[0];
                softL[0] = inputSampleL;
                softR[8] = softR[4];
                softR[4] = softR[0];
                softR[0] = inputSampleR;
            }
            softL[9] = ((softL[0] - softL[4]) - (softL[4] - softL[8]));
            if (softL[9] < -1.57079633) {
                softL[9] = -1.57079633;
            }
            if (softL[9] > 1.57079633) {
                softL[9] = 1.57079633;
            }
            inputSampleL = softL[8] + (sin(softL[9]) * 0.61803398);
            softR[9] = ((softR[0] - softR[4]) - (softR[4] - softR[8]));
            if (softR[9] < -1.57079633) {
                softR[9] = -1.57079633;
            }
            if (softR[9] > 1.57079633) {
                softR[9] = 1.57079633;
            }
            inputSampleR = softR[8] + (sin(softR[9]) * 0.61803398);
            // treble softening effect ended up being an aural exciter
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
} // namespace airwindohhs::hype
