#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestconsole3channel {

constexpr std::string_view k_name{ "PurestConsole3Channel" };
constexpr std::string_view k_short_description{
    "PurestConsole3Channel is an alternate algorithm for minimalist Console."
};
constexpr std::string_view k_long_description{
    "Instead of immediately doing EveryConsole, how about the final Console version it'll contain?This serves two purposes. First, it's an extension of the work I was doing in ConsoleZero: this is a custom sin()/asin() implementation along Airwindows lines, making it a little unusual. It uses a simple sin() approximation, but instead of being mathematically correct it applies all the multiplications and divides only in factors of two: bit shifts, in other words.This is what ConsoleZero did, but more simply. This extends that to improve the sin()/asin() calculation, but then it's not doing that correctly, it's another approximation to make every calculation preserve the mantissas of the numbers as much as possible, every step of the way. Doesn't matter if it seems like a ridiculous thing to do, that's the experiment, and you already have this in mathematically perfect sin()/asin() form anyway: the original PurestConsole is just that.The thing is, there are other gains. These new algorithms do so much with bit shifts that they're blazingly fast. Both this and ConsoleZero run fifteen tracks of mix on my M1 Macbook Pro at about 5% CPU, with a couple bonus plugins in there.And that means if you're running something like current Reaper, as I am… with chain and individual FX oversampling… you can do that. And so I've made a video where I'm demonstrating a 96k project mixed in PurestConsole3, with ALL the Console plugins oversampled to 768k. (unless, because it's starting at 96k, the 16x oversampling is actually giving me 1.536 megahertz oversampling?)And it went from 5% CPU, with presumably 16x oversampling on all Console tracks and the buss, at base 96k sampling rate, to 30% CPU. I could've doubled the project and it would still have run. (probably the original PurestConsole would not be able to do this)And it sounds… different! You'll hear it. You surely can do it. It's got its own sound. Not an Airwindows sound, I think. Sort of smoothed off and glossy. In the video I demonstrate this output, versus non-oversampled PurestConsole3, versus ConsoleZero. And, having heard all the original sounds off my instruments and modular, I think ConsoleZero is rawer and more accurate (that's sort of the idea). But now, if you have a great love for oversampled nonlinear plugins, PurestConsole3 is my little gift, an Airwindows Console version that's designed to work with, not against, what you like.Have fun, and I'll keep new stuff coming as well as I can!"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class PurestConsole3Channel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestConsole3Channel()
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
            inputSampleL += ((pow(inputSampleL, 5) / 128.0) + (pow(inputSampleL, 9) / 262144.0)) - ((pow(inputSampleL, 3) / 8.0) + (pow(inputSampleL, 7) / 4096.0));
            inputSampleR += ((pow(inputSampleR, 5) / 128.0) + (pow(inputSampleR, 9) / 262144.0)) - ((pow(inputSampleR, 3) / 8.0) + (pow(inputSampleR, 7) / 4096.0));
            // crude sine. Note that because modern processors love math more than extra variables, this is optimized
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
} // namespace airwindohhs::purestconsole3channel
