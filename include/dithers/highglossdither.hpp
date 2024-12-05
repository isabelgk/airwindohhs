#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::highglossdither {

constexpr std::string_view k_name{ "HighGlossDither" };
constexpr std::string_view k_short_description{
    "HighGlossDither is a hybrid between shiny dither and truncation! Unnatural science experiment."
};
constexpr std::string_view k_long_description{
    "Here’s an Airwindows science experiment!The idea here was to identify things about truncation that some people (maybe crazy people, but people nonetheless) like, and build them into a dedicated dither. This video includes extensive exploration of truncation, TPDF and flat dithers besides HighGlossDither, so there’s an educational value as well. As for audio value, the important thing to remember is that this one is designed to act like truncation in important ways… so it’s ‘broken’ and you shouldn’t use it for naturalistic things, and you probably shouldn’t use it unless you’ve ever chosen truncation instead of dither, on purpose, because you wanted the edgier, crunchier, different-textured sound of truncation for what you were doing.If that’s ever you, this is an alternate way to get your sounds.The deal with truncation is that it turns the fine details of your mix into a roaring, grunging mess of unnatural noise. The secret of it is, that stuff can act like some horrible sort of parallel compression. It’ll hang onto the tails of notes way longer than it should, and it’ll add intense bit-crusher-like effects to quiet sounds.HighGlossDither uses a special, more uniform-sounding quadratic residue sequence instead of real randomness to sorta ‘diffract’ sounds into crunchier versions of themselves, and applies it at a quieter level than true dither. The result is a hybrid between dither and truncation: rather than dropping quietly into a sea of noise, quiet sounds get hyped and distorted, but they still behave a lot more normally than truncated sounds do. You get the crazy tonal hype, but a better approximation of the proper relative volume levels of the sounds. And you get a sort of noise but it’s quieter than TPDF is, and also serves a purpose of interacting with the sounds.Most people shouldn’t like this. I’m introducing it first to get it out of the way… but who knows? Maybe you’re looking to bring out the next generation of Finnish forest psy trance, and the last thing you want is for the molecular structure of your music to sound natural. Well, HighGlossDither might be right up your alley! You don’t have to resort to truncation to have the finest details of your audio sounding weird and unreal. Take it up another level, with my blessing.Or maybe you’d prefer your audio sounding, you know, good. If so, stay tuned :)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class HighGlossDither final : public Effect<T>
{
    int Position;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    HighGlossDither()
    {
        Position = 99999999;
        flip = false;
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

        int hotbinA;
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
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            Position += 1;
            // Note- uses integer overflow as a 'mod' operator
            hotbinA = Position * Position;
            hotbinA = hotbinA % 170003; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 17011; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 1709; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 173; //% is C++ mod operator
            hotbinA *= hotbinA;
            hotbinA = hotbinA % 17;
            hotbinA *= 0.0635;
            if (flip) {
                hotbinA = -hotbinA;
            }
            inputSampleL += hotbinA;
            inputSampleR += hotbinA;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            // Quadratic dither
            flip = !flip;
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::highglossdither
