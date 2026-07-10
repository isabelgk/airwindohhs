#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::console4channel {

constexpr std::string_view k_name{ "Console4Channel" };
constexpr std::string_view k_short_description{
    "Console4Channel is the original Airwindows ITB mixing system."
};
constexpr std::string_view k_long_description{
    "Console4 is a two-plugin mix buss replacement for DAWs. This was also the release plugin for my switch to Patreon, and the announcement that Kagi had gone out of business owing me hundreds of dollars, so for this Airwindopedia entry from August 3, 2016, I'm including the Patreon blurbs and explanation of what my business was going to do. Turns out everything I promised, I delivered on, and now everything is MIT-licensed open source. Over to the 2016 post!Console 4 is the most recent version of basically my flagship Airwindows plugin. It works through a channel plugin on every sound source, feeding at unity gain into the buss plugin which overrides the digital summing with Airwindows summing (more internal space, more depth, and with Console 4 it now has glue making the top-end more listenable).You can also watch the half-hour long introduction video that goes into a lot more detail about how it’s used, how it works, how you make ‘big kicks and snares’—Console doesn’t let you crank any given channel up to full volume, but it’s designed to layer stuff so if you need giant sounds the best way is to make them composite sounds, everything layered from separate channels in the mix. Rather than layer samples and put them on a single loud track, keep the layers accessible in mix. That does mean if you wanted things like sines to be superloud you’d have to get creative, but the principle’s clear.Speaking of principles…You may notice no demo link, and also there’s no Kagi shopping cart link. There’s a reason for that. Kagi went bust Sunday. I may still get my last two months of sales out of whoever’s divvying up the assets, but as of this Monday you cannot pay me for my work through Kagi, because there is no Kagi. They served me for close to ten years, always with perfect efficiency and scrupulous honesty, but the commission off my work wasn’t enough to keep ’em going.I could go find the newest-trendiest shopping cart e-commerce people, but when I started shopping-cart shopping I found them all horrible, tacky hypemongers offering to do things like find people who’d left a cart un-checked-out, and spamming them with reminder emails like ‘Hi, I’m your shopping cart and I’m looonely! Do you miss me? I miss you! Surely you just forgot me?”I ran away before I threw up all over their e-commerce portals. After all, I have never spammed or bugged people, never advertised, and what’s more I give people free updates for as long as I live, none of which ‘makes sense’ in this happy future of badgering people for every cent, preferably by hyping them into a rental arrangement and DRMing the stuff until it’s nearly ready to explode all by itself, never mind when the rent is due or the authorization servers are having a bad day. (but I digress)There’s something completely different I can do—something I would never have done, except Kagi went out of business and I have no reason not to be completely rebellious and flip the marketplace table.Ever heard of a thing called Patreon? It’s not for discovering new artists. Really, it’s more of a… payment processor. For people who are already well established, who are appreciated for what they do, and who are busy devoting their lives to giving the world something for ‘free’ (like comics, or perhaps music, or art). Much like I already do for existing customers: I’ve promised all Console owners that they will have all Console updates for free.There you have it. I am going all in on Patreon, and that is the full release version of Console 4, with the new Mac and PC VST builds, for free. Please remember this when I have figured out the details and started my Patreon. From now on, I will be relying completely on that to survive. I’ve seen more than one person (for instance, Vechs, or Jim Sterling) who are doing great with Patreon, usually because they too are rebelling against some commercial thing and making a bid for total freedom from obligation.From now on all Airwindows plugins will be ported to Windows and Mac VST as well as being Audio Unit, and they will all be free from now on, and if I get enough Patreon support I will release all source code under the MIT license and document it as I go so that everybody can use the tools and concepts I’ve built to create their own software. That will be my legacy, and if that ain’t a worthy Patreon goal I don’t know what is.This includes old versions of plugins for when people preferred a certain version, so the total number of plugins to cover is over 250. That means if I do one a month it will take me more than 20 years. If I work like a maniac (well, more like a maniac) and put out one a week, that’s still around five years just to turn Airwindows into a sort of audio plugin library and DSP school. And it will all be AU/VST with VST covering Mac VST2 and Win VST2 (built on older OS versions so the plugins work on the very widest range of hosts). I feel this isn’t a bad strategy because if I set up the Patreon so I can actually survive on it, I can make the rate of these ports (and free releases of the existing AUs) conditional on whether I was able to eat that month, which seems fair. :)That’s not counting new research and new plugins… but I have a lot of VST plugins to do, to catch up. Over 250 of them. I’d also ask pirates and haxxors (who don’t have any VST airwindows but what I make, and don’t have most of the AUs) to please leave this ‘making them all free’ process to me through Patreon and my own website. If all goes well, all the plugins will be free in the end (even with source code!) so if you could not mess this up I’d be grateful. ;) leave it to me, please.Oh, and the VST versions do ‘double replacing’, which means they noise shape to the 64 bit floating point buss. Technically that makes them higher sound quality than Apple’s CoreAudio can offer, though I promise you really won’t notice (and the VST versions also do 32 bit ‘replacing’ so they’ll work on all hosts)Consider Console 4 an advance on this new concept. If it works, and if I can live for five or twenty years doing it, every possible Airwindows plugin will be part of everyone’s toolbox and the code will be out there making people’s products better."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console4Channel final : public Effect<T>
{
    double gainchase;
    double settingchase;
    double chasespeed;
    uint32_t fpdL;
    uint32_t fpdR;
    double fpNShapeL;
    double fpNShapeR;
    // default stuff
    float gain;

  public:
    Console4Channel()
    {
        gain = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        chasespeed = 350.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        // TODO: uncomment canDo entries according to your plugin's capabilities
    }

    enum params
    {
        kParamA = 0,
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: gain = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return gain; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "trim"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Trim"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(gain); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return " "; break;

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
        double half;
        double falf;
        if (settingchase != gain) {
            chasespeed *= 2.0;
            settingchase = gain;
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        if (gainchase < 0.0) {
            gainchase = gain;
        }
        while (--sampleFrames >= 0) {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + gain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= gainchase;
                inputSampleR *= gainchase;
            }
            // done with trim control
            half = inputSampleL;
            falf = fabs(half);
            half *= falf;
            half *= falf;
            inputSampleL -= half;
            half = inputSampleR;
            falf = fabs(half);
            half *= falf;
            half *= falf;
            inputSampleR -= half;
            // entire audio code. kthxbai!
            // this is part of the Purest line: stuff that is on every track
            // needs to be DAMN LOW ON MATH srsly guys
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
} // namespace airwindohhs::console4channel
