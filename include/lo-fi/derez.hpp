#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::derez {

constexpr std::string_view k_name{ "DeRez" };
constexpr std::string_view k_short_description{
    "DeRez is an analog-style bit and sample rate crusher with continuous adjustments. This entry includes the Patreon blurb from mid-2018, because the things it says about the build systems, what they target and why I'm doing it, are still relevant today. That makes that stuff its own sort of Airwindowspedia entry as part of the DeRez entry."
};
constexpr std::string_view k_long_description{
    "What would an analog bitcrusher even be? It doesn’t even make sense. You’ve got sixteen bits, eight bits, twelve bits: you can’t have, like, eight and a half bits.Sure you can! DeRez is here! Its dark magic can be yours! If you don’t believe in dark magic, the source code can be and is yours under the MIT license. Folks who are constructing strange models of things like obscure old digital gear should find this useful: do whatever compansion thing you had in mind using for instance PurestConsole, then use DeRez to dial in the right amount of bitcrunch.Here’s how you use it: slide the Frequency control down, to continuously sweep the sample-rate crushing. Slide the Resolution control down, to continuously sweep the bit crushing. There are no transition points: the algorithm will always let you do just a tiny bit more, or less, of either, because it’s really a floating-point algorithm at very high resolution. It’s doing a fairly simple samplerate crush and softening the transitions just a tad for an analog feel, and it’s doing the bit crush by chopping away bit-sized amounts and then truncating once it can no longer take away a whole ‘bit’. Due to this decision, a ‘bit’ can be any size at all, so you can sweep it without having transition points. Obviously, you can also automate the controls in your DAW to program continuous sweeps.It should just work, and you’ll never have to lament, “But why can’t I set the bitcrusher to three and five-eighth bits?” Because now YOU CAN.I know there are a few audiophile ears out there who’ll pop a blood vessel over this nasty little toy, but I’m not a ‘bit’ sorry. Those of you who love this will know who you are right away. Those of you who think this is a really irritating and pointless idea, this one is not for you, and there will be things coming up that are more to your taste.This work is supported by Patreon, for those of you who like me making stuff even when I know it will get some flak and hate-comments: the whole point of being on Patreon for me is that I get freedom to do stuff that’s not popular. Mind you, it’s 2018, so this may well be one of the popular ones. Depends on whether bitcrushing and frequency crushing are ‘in’ or ‘out’. Another nice thing about keeping me around doing this stuff is that it stops mattering whether stuff is ‘in’ or ‘out’: I build the plugins on a Windows 7 VM and an OSX 10.6.8 system kept in a time capsule especially so that my stuff works EVERYWHERE, and by that I mean Mac, Windows, Linux and on machines so old they run PPC chips. Yep! The Mac builds are slightly larger because they’re triple binaries and run 32 bit, 64 bit, and PPC. They should probably also work on OSes older than 10.6.8 but I don’t even have any of those, it’s just build settings. So these plugins are as close as I can get to ‘Grandpa’s Tools’ levels of reliability, and you should never be forced to update, alter, or break your system just to keep up with compatibility with Airwindows plugins. This is in spite of Apple (for one) continuously breaking XCode w.r.t building for older OSes. Hence the old build system. Maybe one day I’ll be running that in a virtual machine that runs it ten times faster than the original laptop. Maybe I’ll run the Windows 7 virtual machine inside the 10.6.8 virtual machine inside the new computer. I will note that I have to keep the old Windows cut off from the internet, and have already had to do workarounds to stop Visual Studio breaking itself in a fit of pique that it can’t install Win10 over the Windows 7 and blow itself up, so it’s not only Apple that gets up to this stuff.Support my Patreon, so I can keep on expanding the pool of plugins that don’t break your system demanding updates and wrecking the joint. I know acting that way gets you more money… because I barely have any money, and the folks working that strategy have lots of money. And it’s not a good enough excuse as far as I’m concerned, so as always I will just not do the behavior that I don’t like to see in others. Vote with your dollars, that’s a somewhat practical way to be heard. It won’t fix the world, but it definitely is able to keep me going :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeRez final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double heldSampleL;
    double lastSampleR;
    double heldSampleR;
    double position;
    double incrementA;
    double incrementB;
    float A;
    float B;

  public:
    DeRez()
    {
        A = 1.0;
        B = 1.0;
        lastSampleL = 0.0;
        heldSampleL = 0.0;
        lastSampleR = 0.0;
        heldSampleR = 0.0;
        position = 0.0;
        incrementA = 0.0;
        incrementB = 0.0;
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
        kParamB = 1,
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "rate"; break;
            case kParamB: return "rez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Rate"; break;
            case kParamB: return "Rez"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double targetA = pow(A, 3) + 0.0005;
        if (targetA > 1.0) {
            targetA = 1.0;
        }
        double soften = (1.0 + targetA) / 2;
        double targetB = pow(1.0 - B, 3) / 3;
        targetA /= overallscale;
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23 && (targetB == 0)) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23 && (targetB == 0)) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            incrementA = ((incrementA * 999.0) + targetA) / 1000.0;
            incrementB = ((incrementB * 999.0) + targetB) / 1000.0;
            // incrementA is the frequency derez
            // incrementB is the bit depth derez
            position += incrementA;
            double outputSampleL = heldSampleL;
            double outputSampleR = heldSampleR;
            if (position > 1.0)
            {
                position -= 1.0;
                heldSampleL = (lastSampleL * position) + (inputSampleL * (1.0 - position));
                outputSampleL = (outputSampleL * (1.0 - soften)) + (heldSampleL * soften);
                // softens the edge of the derez
                heldSampleR = (lastSampleR * position) + (inputSampleR * (1.0 - position));
                outputSampleR = (outputSampleR * (1.0 - soften)) + (heldSampleR * soften);
                // softens the edge of the derez
            }
            inputSampleL = outputSampleL;
            inputSampleR = outputSampleR;
            double offset;
            if (incrementB > 0.0005)
            {
                if (inputSampleL > 0)
                {
                    offset = inputSampleL;
                    while (offset > 0) {
                        offset -= incrementB;
                    }
                    inputSampleL -= offset;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleR > 0)
                {
                    offset = inputSampleR;
                    while (offset > 0) {
                        offset -= incrementB;
                    }
                    inputSampleR -= offset;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleL < 0)
                {
                    offset = inputSampleL;
                    while (offset < 0) {
                        offset += incrementB;
                    }
                    inputSampleL -= offset;
                    // it's above 0 so subtracting subtracts the remainder
                }
                if (inputSampleR < 0)
                {
                    offset = inputSampleR;
                    while (offset < 0) {
                        offset += incrementB;
                    }
                    inputSampleR -= offset;
                    // it's above 0 so subtracting subtracts the remainder
                }
                inputSampleL *= (1.0 - incrementB);
                inputSampleR *= (1.0 - incrementB);
            }
            lastSampleL = drySampleL;
            lastSampleR = drySampleR;
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
} // namespace airwindohhs::derez
