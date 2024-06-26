#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dithermetimbers {

constexpr std::string_view k_name{ "DitherMeTimbers" };
constexpr std::string_view k_short_description{
    "DitherMeTimbers is dark deep 'dither' in 24 bit version."
};
constexpr std::string_view k_long_description{
    "What is a dither? Dither is a way of changing one type of noise, quantization, to a different type, just plain noise. It’s all about manipulating a situation where your waveform must decide between two options, ‘up’ or ‘down’, and pick one of a very limited number of positions in a lower-resolution space. In the strictest sense, dither is adding two sources of calibrated noise in order to make the resulting noise floor completely unrelated to the audio signal.Or, if you’re me, ‘dither’ can be an affair of tracking the Benford Realness calculations of each option, and always choosing the direction that will most closely approximate real sampled data, then noise shaping the result to produce a bright airy hiss for the background noise, and an open, detailed sound picture far more revealing than normal ‘dither’ can be. And that’s Not Just Another Dither, which uses a completely different approach to selecting ‘up’ or ‘down’. And then there was last Monday, my first Airwindows tech-support livestream, and a little diagram I drew to explain the sampling theorem… and an idea.What if you just picked whichever option smoothed the signal out most?Introducing Dither Me Timbers, the tonal opposite to Not Just Another Dither. Although it has dither in the name, and works like a dither, and occupies the same place in your DAW as a dither, it’s not a dither at all (though it does have a noise shaper). It’s a filter. It exists to take the tiny microdetails in the sampling theorem, and make them darker and deeper. The functionality is very simple: it runs one sample of latency (it’s an output stage, so that shouldn’t be too much of a problem in practice) and, for every sample, asks whether it’s larger or smaller than its surroundings. If it’s the top of a corner or a spike of treble, it simply picks whichever ’rounding’ will smooth out the treble the most. How much? It doesn’t care. It just always picks the least treble at any given moment, no matter what.The noise shaping is gentler than that in Not Just Another Dither. Instead of establishing a bed of subtly hissing noise like NJAD, Dither Me Timbers uses its noise shaping to transform what is not even a dither, into a behavior. It doesn’t try to decorrelate the noise, or present a clean signal behind it. Instead, it does this: it works to make whatever noise is produced, as loud as the original sound would have been. This is all at superfaint levels, and interacts with the treble-darkening effect. It sounds like analog mechanical noise from some sort of playback system that’s part of the audio. As it drops way below the noise floor of what TPDF dither would have been (and I mean WAY below that noise floor) it replaces faint musical content with faint rustles and sputterings, not unlike a vinyl record’s artifacts. It’s entirely correlated with the audio, and closely matched in volume to what the audio would have been… far below what we’re used to experiencing as a digital noise floor. And it’s combined with the EQ behavior of Dither Me Timbers, producing a behavior where the audio goes first dark, then quiet, as it drops beneath perception.This produces an effect you can’t get anywhere else, which contrasts with Not Just Another Dither completely. Instead of sparkling detail, you get depth like no other digital medium can produce. Ambient stuff, distant sounds, are twice as deep and twice as dark. If you’re going for natural organic tones, they’ll feel all the more solid, all the more real. There’s an ease to the presentation, a blackness and silence to the background, as if distant reflective surfaces became velvet curtains. Quiet musical notes take on body, lose sparkle, sit back in space as if they’re on a distant stage.There’s also a version that does this to 16 bit, Dither Me Diskers. I don’t do that with many dither experiments: NJAD has its CD version, and now there’s finally a follow-up for those who’d like to make seriously warm, deep, organic sounding CDs. If you’re not pushing the frequency limits of human hearing with your content, it’s possible to get CDs to sound surprisingly good, and Dither Me Diskers takes that to another level. You won’t feel a lack of depth and distance from your CD with this. There’s hints of sound way way below what you’re using to thinking of as the grain of 16 bit digital audio, and you can’t feel the edges of it at all. Everything’s more dark and mellow.And lastly, some of you will hate it. This is a filter. It takes the most delicate subtleties of the digital waveform and darkens them up, on purpose (‘cos you gotta do something, when you’re quantizing). You know who you are: if you’re not panting for that depth perception and analog smoothness, if you’re not secretly into the resonance and power of the best old vinyl, this is not for you. Try NJAD, which will give you all the airy detail you could ask for.If you want to sink into the music like it was soup, if you think digital would sound better through tubes or tape or anything to cut the dryness and shallowness of it… odds are you’re going to love Dither Me Timbers. It is as wrong as a pirate at a garden party: it’s not even a dither at all, but a filter and a bizarre noise shaper. It’s a trick, a stunt, a mockery of ‘correct dither behavior’. It’s an EQ, a tone-changer.And there’s nothing else like it… and it’s yours. Enjoy :)Note: there is a more sophisticated version of this behavior in Airwindows Dark. The difference is, Dither Me Timbers only looks for a tiny micro-environment, within three samples. Dark extrapolates outward to work out which option will best make the WHOLE sound darker, not just a tiny three-sample area."
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class DitherMeTimbers final : public Effect<T>
{
    double noiseShapingL;
    double noiseShapingR;
    double lastSampleL;
    double lastSample2L;
    double lastSampleR;
    double lastSample2R;

  public:
    DitherMeTimbers()
    {
        noiseShapingL = 0.0;
        noiseShapingR = 0.0;
        lastSampleL = 0.0;
        lastSample2L = 0.0;
        lastSampleR = 0.0;
        lastSample2R = 0.0;
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

        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1 * 8388608.0;
            double inputSampleR = *in2 * 8388608.0;
            double outputSampleL;
            double outputSampleR;
            lastSampleL -= (noiseShapingL * 0.125);
            lastSampleR -= (noiseShapingR * 0.125);
            if ((lastSampleL + lastSampleL) >= (inputSampleL + lastSample2L)) {
                outputSampleL = floor(lastSampleL);
            }
            else {
                outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
            }
            if ((lastSampleR + lastSampleR) >= (inputSampleR + lastSample2R)) {
                outputSampleR = floor(lastSampleR);
            }
            else {
                outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
            }
            lastSample2L = lastSampleL;
            lastSampleL = inputSampleL; // we retain three samples in a row
            lastSample2R = lastSampleR;
            lastSampleR = inputSampleR; // we retain three samples in a row
            noiseShapingL += outputSampleL;
            noiseShapingL -= lastSampleL;
            noiseShapingR += outputSampleR;
            noiseShapingR -= lastSampleR;
            if (outputSampleL > 8388600.0) {
                outputSampleL = 8388600.0;
                noiseShapingL *= 0.5;
            }
            if (outputSampleR > 8388600.0) {
                outputSampleR = 8388600.0;
                noiseShapingR *= 0.5;
            }
            if (outputSampleL < -8388600.0) {
                outputSampleL = -8388600.0;
                noiseShapingL *= 0.5;
            }
            if (outputSampleR < -8388600.0) {
                outputSampleR = -8388600.0;
                noiseShapingR *= 0.5;
            }
            *out1 = outputSampleL / 8388608.0;
            *out2 = outputSampleR / 8388608.0;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::dithermetimbers
