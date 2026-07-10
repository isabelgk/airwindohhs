#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::dubplate {

constexpr std::string_view k_name{ "DubPlate" };
constexpr std::string_view k_short_description{
    "DubPlate is ruthless dubplate mastering for electronic music."
};
constexpr std::string_view k_long_description{
    "It's possible you need this to make your music sound its best. I've used this literal plugin to put out a record! Just now, on something that I'd tried to get right for years!It's also possible this is a retro-themed blast from the past with no use in a modern world of downloadable hyperaudio and that you'll immediately know it's trash not to be taken seriously.Let's explain. A dub plate is like a vinyl record except it's cut from acetate, which wears out quicker. These are traditionally known as 'masters' and electroplated to make vinyl-stamping machinery, but you can also play them on a record player directly. The UK electronic music scene went through a phase where dubplates were extremely popular, with mastering houses booked solid and people waiting in lines to get their dubplates made from their DAT tapes. The experience of going to clubs and listening and dancing to this music was a golden age all its own, separate from the golden age of rock or metal or prog, etc. And there's a distinct dubplate sound that can be exploited, one that's wildly different from what you typically get out of DAWs or stuff like lofi hip-hop, even though people like to bring in lofi elements to resist the 'DAW sound'.What's this dubplate sound made of? There's an unexpected reason beyond just the lacquer material. Airwindows DubPlate makes NO effort to mimic surface noise, or groove wear, or any of that. Instead, it acts like a mastering chain that's so bulletproof it can resist any terrible audio thrown at it. There's three ways it does this and gets the dubplate sound (it was dialed in using Airwindows Meter to track what's actually happening sonically)First, you can't just throw any synthetic noise at a vinyl record and have it play. You can even break a lathe if you throw too much high frequency energy at it. So, DubPlate uses the 'glue' control in Mastering… except it uses an intensity Mastering can't even reach, and it uses two of them each set differently, just to get to what was coming off the dubplate example I had as a real life reference.Second, the side channel is highpassed very aggressively! Dub plate houses had to handle anything that came in the door no matter how insane it was, and there was no time to fiddle with a troublesome DAT tape. If the guy had thrown in a full volume 808 kick distorted and out of phase, well, the mastering house had to turn it into a dubplate that played, and so the side channel doesn't let any bass in. (There's a special technique that might warrant more exploration, which I included to help things stay punchy even when filtering that aggressively).Lastly, even the mid channel isn't safe! Making competitive dub plates, you couldn't let people swamp their audio with subsonic rumble no matter what they made their synths do. The gear of the time was already capable of getting you into trouble and burying everything in useless energy-sapping sub-bass, and musicians could be expected to get this wrong as easily as they could get it right. (Monitoring contains a zero cross section and a reference 40dB line that can show you whether your bass is unproductive in a live setting.) Plus, simply running into the analog amps and lathes of the day, the mastering chain, could drastically alter the subs. And so, Dubplate goes after extreme digital lows as well, reining things in until you could cut the output of the plugin to a lacquer and be pretty safe from trouble no matter what audio you put in. This one also uses the technique for having the force of the bass stick around while being reined in.And that's the secret. If you have a fantastic playback system, killer subs, and years of mixing experience, then maybe you can get creative and come out with something that will be awesome, and then tweak it with Mastering and Meter, and you get to explore the farthest realms of audio creativity without setting a foot wrong.If on the other hand your reach exceeds your grasp, you can put together creative exciting stuff that also happens to be real obnoxious on multiple levels. You're distorting too much, and you didn't notice that you went over-bright, and a bunch of things in your bass are freaking out, and it's too complicated and to make it safe takes away the heart of your creative decisions.So, go into DubPlate… pad your stuff down with PurestGain if you have to, the output might peak louder even if it is pulling the subs back… and let the uncaring mastering chain strip down your sound into the gutsy heart of its intention. If it's hyperpop you wanted, maybe you check out right there. But if you're looking for size and depth and power and the sound of the dubplate era calls to you… maybe this brings your music the rest of the way there.Hope you like it! I put out a record (Dragons Dub Plate Edition) specifically because I had music literally from that era made using those methods, and the plugin instantly turned it into the experience it was always meant to be. That record's at the usual bandcamp price but DubPlate is open source and free! See if it helps you out :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class DubPlate final : public Effect<T>
{
    double iirA;
    double iirB; // first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
    double iirC; // we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
    double iirD; // then there's a Mid highpass.
    bool fpFlip;
    double lastSinewAL;
    double lastSinewAR;
    double lastSinewBL;
    double lastSinewBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DubPlate()
    {
        iirA = 0.0;
        iirB = 0.0;
        iirC = 0.0;
        iirD = 0.0;
        fpFlip = true;
        lastSinewAL = 0.0;
        lastSinewAR = 0.0;
        lastSinewBL = 0.0;
        lastSinewBR = 0.0;
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
        switch (static_cast<params>(index)) {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {

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
        double rangescale = 0.1 / overallscale;
        double iirSide = 0.287496 * rangescale;
        double iirMid = 0.20123 * rangescale;
        double threshSinewA = 0.15 / overallscale;
        double threshSinewB = 0.127 / overallscale;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double mid = inputSampleL + inputSampleR;
            double side = inputSampleL - inputSampleR;
            // assign mid and side.Between these sections, you can do mid/side processing
            double temp = side;
            double correction;
            if (fpFlip) {
                iirA = (iirA * (1.0 - iirSide)) + (temp * iirSide);
                temp -= iirA;
                correction = iirA;
            }
            else {
                iirB = (iirB * (1.0 - iirSide)) + (temp * iirSide);
                temp -= iirB;
                correction = iirB;
            }
            iirC = (iirC * (1.0 - iirSide)) + (temp * iirSide);
            temp -= iirC;
            correction += (iirC * 0.162);
            side -= sin(correction);
            fpFlip = !fpFlip;
            iirD = (iirD * (1.0 - iirMid)) + (mid * iirMid);
            mid -= sin(iirD);
            // gonna cut those lows a hair
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            // unassign mid and side
            temp = inputSampleL;
            double sinew = threshSinewA * cos(lastSinewAL * lastSinewAL);
            if (inputSampleL - lastSinewAL > sinew) {
                temp = lastSinewAL + sinew;
            }
            if (-(inputSampleL - lastSinewAL) > sinew) {
                temp = lastSinewAL - sinew;
            }
            lastSinewAL = temp;
            if (lastSinewAL > 1.0) {
                lastSinewAL = 1.0;
            }
            if (lastSinewAL < -1.0) {
                lastSinewAL = -1.0;
            }
            inputSampleL = (inputSampleL * 0.5) + (lastSinewAL * 0.5);
            sinew = threshSinewB * cos(lastSinewBL * lastSinewBL);
            if (inputSampleL - lastSinewBL > sinew) {
                temp = lastSinewBL + sinew;
            }
            if (-(inputSampleL - lastSinewBL) > sinew) {
                temp = lastSinewBL - sinew;
            }
            lastSinewBL = temp;
            if (lastSinewBL > 1.0) {
                lastSinewBL = 1.0;
            }
            if (lastSinewBL < -1.0) {
                lastSinewBL = -1.0;
            }
            inputSampleL = (inputSampleL * 0.414) + (lastSinewBL * 0.586);
            temp = inputSampleR;
            sinew = threshSinewA * cos(lastSinewAR * lastSinewAR);
            if (inputSampleR - lastSinewAR > sinew) {
                temp = lastSinewAR + sinew;
            }
            if (-(inputSampleR - lastSinewAR) > sinew) {
                temp = lastSinewAR - sinew;
            }
            lastSinewAR = temp;
            if (lastSinewAR > 1.0) {
                lastSinewAR = 1.0;
            }
            if (lastSinewAR < -1.0) {
                lastSinewAR = -1.0;
            }
            inputSampleR = (inputSampleR * 0.5) + (lastSinewAR * 0.5);
            sinew = threshSinewB * cos(lastSinewBR * lastSinewBR);
            if (inputSampleR - lastSinewBR > sinew) {
                temp = lastSinewBR + sinew;
            }
            if (-(inputSampleR - lastSinewBR) > sinew) {
                temp = lastSinewBR - sinew;
            }
            lastSinewBR = temp;
            if (lastSinewBR > 1.0) {
                lastSinewBR = 1.0;
            }
            if (lastSinewBR < -1.0) {
                lastSinewBR = -1.0;
            }
            inputSampleR = (inputSampleR * 0.414) + (lastSinewBR * 0.586);
            // run Sinew to stop excess slews, two layers to make it more audible
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
} // namespace airwindohhs::dubplate
