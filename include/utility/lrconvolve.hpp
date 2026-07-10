#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::lrconvolve {

constexpr std::string_view k_name{ "LRConvolve" };
constexpr std::string_view k_short_description{
    "LRConvolve multiplies each channel by the other!"
};
constexpr std::string_view k_long_description{
    "This experiment created a monster. And yet it's so simple…The idea was, can you convolve one channel by the other. L convolves R. And, since multiplication is commutative, R can convolve L right back! And what do you get when you do this?Swedish Chef from Hell, apparently! See the example video (or convolve any vocal with a simple 1k sine tone in the other channel). But why is it doing this terrible thing?When you use a sine wave as one of the channels, it is multiplying the one by the other. (Technically I have a method for taking the square root of the result so it doesn't simply change the density of the sound too much: this is not complicated, it's just making sure the positives and negatives are still what they 'should' be.) And when you're modulating the polarity of a vocal track at audio rates… you can get very weird distortions of the tonality and vowels.It gets worse: if you use lower tones, you can go full Dalek. That's because this is a nasty form of a ring modulator, when you ask it to be. I'll be working on some more variations, it'll give me something to do :)But what happens when you get bolder? For instance, convolve drums with a heavy guitar, or a race car? Not what you'd think. Remember, polarity flips at the frequency of whatever normal sound you feed in. If there's silence, everything will go silent. But if there's noisy drum sounds, it'll turn everything noisy: you won't hear the guitar, and then if the guitar's midrangey, you also won't hear the bass on the drums. It really hybridizes the sound to become the worst of both worlds, and this particular version is specialized to find the most extreme combination of both sounds, which will bring out the noisiest aspects of both. If you had a track that was just positive (or negative) control voltages or envelopes, you would indeed get a normal VCA out of this. …but what fun is that?This plugin has no controls and will show up as a blank space, possibly with its name written on it. All you do is run one thing into one side, and something else into the other (perhaps with track routing: it's simple to do in Reaper, just send to a track with this on it). If you send mono to it, you will get an odd sound which is the sound full-wave rectified, because the negative wave times negative wave equals positive (because of the square root, no other change happens). If you send a completely out of phase signal to it, you get the sound full-wave rectified only to the negative side (note that it doesn't clip off the sections of audio being rectified, so you can't 'split' into positive and negative waves this way, they would sum to 'no audio').There will be offshoots of this that are more normal. This is just the raw craziness of it in its purest form. If this is the plugin for you, you know who you are! And are probably already playing with it and not listening to me anymore.So, carry on, and I'll come up with some more variations that do different things :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class LRConvolve final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    LRConvolve()
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

        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // blame Jannik Asfaig (BoyXx76) for this (and me) :D
            double out = 0.0;
            if (inputSampleL > 0.0 && inputSampleR > 0.0) {
                out = sqrt(fabs(inputSampleL) * fabs(inputSampleR));
            }
            if (inputSampleL < 0.0 && inputSampleR > 0.0) {
                out = -sqrt(fabs(inputSampleL) * fabs(inputSampleR));
            }
            if (inputSampleL > 0.0 && inputSampleR < 0.0) {
                out = -sqrt(fabs(inputSampleL) * fabs(inputSampleR));
            }
            if (inputSampleL < 0.0 && inputSampleR < 0.0) {
                out = sqrt(fabs(inputSampleL) * fabs(inputSampleR));
            }
            inputSampleL = inputSampleR = out;
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
} // namespace airwindohhs::lrconvolve
