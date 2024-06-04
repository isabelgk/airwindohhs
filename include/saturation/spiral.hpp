#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::spiral {

constexpr std::string_view k_name{ "Spiral" };
constexpr std::string_view k_short_description{
    "Spiral2 is Spiral with controls including Presence. Also, a historical post presented in full as it is just after my Dad's death. Post follows:"
};
constexpr std::string_view k_long_description{
    "What if… the building-block sine-based saturation routine I build so many things on, which I thought was the smoothest distortion you could have, is NOT the ideal distortion?I was watching a youtube video by Brady Haran, on his ‘Numberphile’ channel. It was about the golden ratio, and it showed the little drawing you’ve maybe seen, where you take a golden rectangle, and then make a square on it and put a circle segment (like when I use sine curves for my distortions), and then make a smaller square next to it and a circle section on that which connects, and so on ad infinitum. A ‘spiral’ like a seashell, a golden ratio spiral.And Brady says, ‘of course this isn’t actually a spiral’ and my mind: blown.Because of course it isn’t. It’s circle pieces butted up against each other, and only looks like it’s connected because it’s pretty flat going through those points. But the rate of curvature changes really suddenly and drastically at those points… and does it the same way, at the zero crossing of ConsoleChannel, Density, PurestDrive, PurestWarm, Channel, and lots and lots of other plugins I’ve made. It’s part of the ‘fatter, smooth’ sound of some of my distortions. Seemingly really fat and analog-sounding, but there’s a discontinuity as you go through zero, which is why it makes the audio sound obviously different.People do like making the audio sound different, but people HATE the sound of discontinuities. An old version of Channel where I hacked in a ‘flat’ undistorted section, got me a huge blowback of dismay and outrage until I put it back to the simple sine-based one. People are incredibly sensitive to second order discontinuities, where the output number will be 0 but the direction things are changing will suddenly be the opposite. That’s what made the old ‘New Channel’ be such a failure: the center of the wave was flat no-distortion, and then without making a visible discontinuity it would suddenly change to ‘tighter sine-based saturation curve’, both on the negative and positive sides of the wave. It would look perfectly normal but people just hated the sound. Now I know why. And now it turns out the sine based one HAS a discontinuity, at the zero crossing, right where you’d get class AB and B distortion, and it never occurred to me.And I can fix it, and turn the code for that part into ONE line of reinvented original code, which will be open source because I’m Patreon-supported and don’t have to stop people using my good audio code (they only have to credit me when they use it: it will be very possible to tell when they do. They don’t have to pay anything since it’s an MIT license, and they don’t have to open their own source, just publically credit that they used my Spiral code).And of course I did: here, have Spiral, free. You can just install this and listen to it, if the video and the post are too long. This is the proof of concept which can be used in several useful ways. There are no controls, at all. You can gain stage into it and do stuff with it and sit it on the top of every track like it was PurestDrive or Channel, or use it as a 2-buss clipping stage, where it will clip to around -1.4dB with about -0.4 intersample peak maximum (so it is Mastered For iTunes friendly, used as a final clip). It also has a ‘freak out mode’ if you massively overdrive it, and you can do that with things like uncompressed drums, and it makes noises you’ve never heard before, or when used more gently it just sounds like the ultimate analogification.I really had no idea I could do a basic saturation algorithm (which still uses long double precision sines as part of it) that was that good. It’s a considerably bigger sonic improvement than the new noise shaping technique, because what it does is on a far higher level… though of course it also is using the new noise shaping, for good measure. All the latest everything, right here.Bear in mind, the original sin()-based one in Console5, PurestConsole etc. is still optimal for Console5 encode/decode because it can be lossless and has significant effect at low levels. My tape emulation stuff uses the ‘fattening’ effect of that on purpose. I can’t just go through and replace everything because all the sounds will change. I have to re-voice everything that would take advantage of the new code, and I’ll do it, and it’ll take time and effort, and probably become new versions of things so you can still have the ‘sine fatness’ versions if you want them. I don’t like taking sounds away from people, and old tones shouldn’t be removed or made inaccessible.But what if there’s a WAY better analog-sounding distortion effect based on the way that the ‘constructed’ golden ratio seashell/spiral made of circle sections in boxes that get smaller, is NOT correct because much like the simplest sine-based overdrive, it doesn’t start with zero distortion but with the same tiny distortion the whole time (which, in joining to the opposite pole, makes a discontinuity you can’t see much like the golden ratio ‘spiral’ has discontinuities you can’t see?) …and I fixed it?Try Spiral, and I will get to work incorporating this into my library of audio plugins that I make available using my Patreon.I will need to take a week to sort of meditate and settle my mind, but it was important to me to still be giving stuff to my fans and patrons etc. so I was looking around for something fairly easy to do. For instance, take Spiral and give it the ‘Density/Drive’ treatment, including things like pre and post gain and a highpass and a dry/wet. Busywork but nothing particularly innovative.I think Dad would have been proud that I stumbled across something that was kind of innovative anyhow. Now I have Spiral2. It has those controls (that people really wanted: and if you set them to unity/neutral, they bypass so you can have EXACTLY the same as Spiral itself, if you like) but something else happened…It occurred to me, if I was blending between dry signal and the sin() function saturated signal using the signal itself as the blend factor, I was just using a sample. Well, I also knew how to store a sample, and then the next time it’d be ‘lastSample’. And what would happen if I blended between dry and the saturated one… using the PREVIOUS sample?With low frequency stuff, pretty much nothing. But what if there was high frequency stuff? What’d happen then?Turns out, it’s a little like ‘Pyewacket’, my compressor that compresses into a ‘negative Density’ effect that lets peaks through. With that, the front of the wave is unusually pure and punchy, and there’s better articulation of sounds. With Spiral2, it lets onset transients through, especially if they’re happening suddenly out of existing silence (and bear in mind it can be only one sample of silence for it to work). It’s not a super obvious effect… but if you use Spiral to chop off peaks, and add this new effect (which I could only call Presence) then you can get quite the opposite effect: at full Presence, it sounds like everything’s being distorted but the meters show how onset transients are still getting through.If you set it halfway, it becomes very close to peaking at exactly 0 dB. If you set it to 0 you get the original Spiral (note that it still has the ‘continuing around the sin() curve’ effect so if you over-slam it, it’ll choke and go quieter).So, I wanted to do something nice for my peeps since everybody has been so kind and supportive. Turns out my muse thought that was the sweetest intention ever, and really came through for me. Enjoy Spiral2: you’ll find that Presence is quite a striking effect. I think in extreme cases it’s TOO much air, but that’s why it’s on a slider, which is really just a crossfader between the two ‘circuits’, normal and with the one sample delay on the ‘sense’ circuit.See ya soon, and I hope you like Spiral2. If you would have dropped $50 on this without a moment’s hesitation once you hear what it can do by trying it, please do that using the Patreon. I’m looking to keep expanding and be more ambitious, if that’s OK. It’s more fun being ambitious with a budget, and food and shelter and stuff :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Spiral final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Spiral()
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
            // clip to 1.2533141373155 to reach maximum output
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
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
} // namespace airwindohhs::spiral
