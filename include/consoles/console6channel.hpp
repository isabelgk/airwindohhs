#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console6channel {

constexpr std::string_view k_name{ "Console6Channel" };
constexpr std::string_view k_short_description{
    "Console6Channel is a new more intense Console that peaks at lower dB."
};
constexpr std::string_view k_long_description{
    "A little while ago I got a note from veteran GSposter torridgristle, entitled “Simple Encode/Decode (Inverse Square)”. It was a very simple little note.“Something I’d been using in GLSL shaders for brightening the image turns out to work pretty well for an encoder / decoder pair when demoed with Maths. It may already have a name but I refer to it as inverse square. 1-(1-x)^2 and 1-(1-x)^0.5 https://www.desmos.com/calculator/3vhxkwjjyi The quiet parts are shaped like a plain multiplication, just a simple scale up or down, and then it curves back toward 1.”Now, this wasn’t quite code yet (for audio purposes, it has to be able to do negative voltages, perhaps suppress inputs that would give broken outputs) but I asked if I could have it under MIT license anyway, and torridgristle said yes, and off I went to code it into a Console-like thing to see what happened.Right away, I noticed that it was kicking in WAY harder. You could still put full-scale audio through it as one track, but for a dense mix, it broke up a lot sooner than Console5 or any previous Console had done. The channel plugin made stuff a lot louder and more aggressive, and the buss plugin cut stuff back harder. I had to pad my choir demo back a lot simply to stop it exploding in distorty noise. However, when I did… it was a hell of a big, intense, Console-style sound. This one wasn’t subtle at all.And then, when I remembered how many times people had wanted a Console that did NOT have to peak near 0dBFS… I knew me and torridgristle, whom I’ve never met and don’t even know the first thing about… were onto something.That’s the neat thing about open source when people are willing to use it. I mean, yes Patreon, you wouldn’t have this at all if not for that, you wouldn’t have future stuff inspired by this etc etc… but the heart of it is, you have the newest greatest Console because two complete strangers got together and cooperated. torridgristle used this in shaders and has never heard what it sounds like until today, and had no idea what it would do or what it would sonically mean. I know an incredible sound when I hear one, and I know it’s useful for people who are NOT trying to mix super-loud into their DAW buss, but I didn’t think up the very simple and elegant little algorithm. torridgristle didn’t code the Mac AU and VST Mac/Win/Linux ports, I didn’t code the GLSL shaders and have never seen what they do.But the whole is greater than the sum of its parts, and without either of us being too troubled with great struggling, Console6 happened, and here it is.So the bottom line here is simply usability: mix into this like it’s an analog mixing console that can be overstressed. You can still put full scale single tracks through there, losslessly, because that’s what any Console does. You can mix into it provided you don’t stress the buss too hard (or don’t mind the distortion you get). Gain stage before hitting the Channel plugins, one Console6Channel plugin on each sound source with unity gain through the mix structure into the Console6Buss plugin. If you need to slam stuff louder afterward you must do it AFTER the Console6Buss plugin, in whatever way you like. Console is always a digital mix buss replacement without an EQ-style sound of its own, so if you want further color you need to use other plugins outside Console. If you’re daring or just curious you can use things between the plugins (‘inside’ Console) such as a DAW EQ, echoes, subtle reverb etc. If you choose wrongly, it’ll distort or be weird, and things like EQs will be much more sensitive than usual to boosts and cuts."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console6Channel final : public Effect<T>
{
    double inTrimA;
    double inTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console6Channel()
    {
        A = 1.0;
        inTrimA = 1.0;
        inTrimB = 1.0;
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
            return 1.0;
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
            return "input";
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
            return "Input";
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
            return "";
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        inTrimA = inTrimB;
        inTrimB = A;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double inTrim = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
            // encode/decode courtesy of torridgristle under the MIT license
            // Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5
            // Reformulated using 'Herbie' for better accuracy near zero
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            else if (inputSampleL > 0.0) {
                inputSampleL = inputSampleL * (2.0 - inputSampleL);
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            else if (inputSampleL < 0.0) {
                inputSampleL = inputSampleL * (inputSampleL + 2.0);
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            else if (inputSampleR > 0.0) {
                inputSampleR = inputSampleR * (2.0 - inputSampleR);
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            else if (inputSampleR < 0.0) {
                inputSampleR = inputSampleR * (inputSampleR + 2.0);
            }
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
} // namespace airwindohhs::console6channel
