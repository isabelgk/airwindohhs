#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::derez3 {

constexpr std::string_view k_name{ "DeRez3" };
constexpr std::string_view k_short_description{
    "DeRez3 reinvents retro digital tones."
};
constexpr std::string_view k_long_description{
    "I've long been interested in old school lo-fi digital.It's partly because I've had ancient reverbs and things, stuff that produced a vibe far better than newer replacements, and it's partly because all my own efforts have been so much the opposite: working out how to dither 32-bit floating point and using it in all plugins, turning to 96k sampling rate and working out distributed filtering so the processing can be simpler than the wild overprocessing of oversampling: I've gone farther and farther towards ultra-resolution and learned how to adapt it musically so it doesn't just sound super-DAW-y and clinical.But all the time, I've known of the great retro samplers. Some I've even bought, but not really known what to do with, a rock/prog guy like me. Some are out of my range and I can't afford them. I've even picked up some seminal records done with old samplers like that, or discovered they played a role in stuff I loved.And then I started trying to improve on my undersampling for reverbs… and made a breakthrough that changed everything.You see, when I undersample a reverb, I'm taking a sample only every two or four samples, and interpolating the result to give a high output sample rate. I've been sticking to exactly 2x or 4x sampling rate, and doing a linear reconstruction: you can hear what that sounds like in CrunchCoat, which is also fun to play with… but in essence it's taking the idea of interpolating, and going 'let's just take a reverb sample every X samples. What's X? Anything!'So you got to swoop the reverb down to a gritty, low-fi mess. So far so good (or bad: but that's the point, it was called 'cursed retro digital' by my livestream and obviously I had to put it out as a plugin) But then, what would you do if you had a sequence of lo-fi samples, irregularly spaced, and you wanted to draw a smooth line through them, not a pointy line?Graphics has a handy technique called Bezier curves. It lets you draw a smooth line between points. Depending on the Bezier curve, you might go through the points, or around them, depending on how you set it up. But the important thing is, it's not an audio calculation. The higher harmonics you might generate have nothing to do with the sound. It doesn't know it is a sound. It's just trying to draw the seamless, smoothest curve between some points.Initial experiments with the reverbs went strangely. It would act like a cursed brickwall filter, but with a strange resonance unlike anything I'd heard before. In the ballpark, but always ruining the cleanness of the reverb and making horrible (but very smooth) artifacts… until I hit on using perfectly even divisors of the sample rate and that got me a plugin called CreamCoat and a whole batch of new reverbs I'm already beginning to use for everything.But then… what happened to the horrible but very smooth artifacts? The Bezier curve reconstruction that isn't so careful to sound nice, that throws out strange artifacts never before experienced, but always very mellow and smooth like some kind of cursed brickwall or isolator filter?Meet DeRez3. That's your Rate control. Unlike CrunchCoat it doesn't click when taken to zero, largely because it just goes to subsonics and never really to zero so it can't trap energy by mistake. Every parameter is control-smoothed because I expect this thing to be played like a synth filter… It's got a Rez control that's tweaked so at extreme low bit, it throws in a gating behavior that can be used in conjunction with the Rate to produce strange gatey effects on sounds. It's got a Dry/Wet that is actually set up like my Wetness controls: with full dry you can sneak in small amounts of DeRez without affecting dry level, with full wet you can sneak in traces of dry without cutting wet level. 0.5 gives you both.This is an alternate way of dialing in those retro digital sounds without 'emulation' of all that analog stuff. No added noise, no simulated analog stages. Instead, it is the refinement of a concept for reconstructing lo-fi using Bezier curves, and only gets better the more rez you've got to hold it. 96k, double precision? Bring it on, it will just further optimize the vibe being produced by the algorithm. It's HI-FI low-fi.I'm still working on the rock/prog dream of perfecting ConsoleX, with all those filters and things, tailored to get the most out of music that doesn't often sound great in the ordinary DAW… but this is a window onto something else. Where, it seems, there might be another kind of Console that could exist in the worlds of samples and low-bit and lo-fi, the isolator filters and digital overtones taking the place of detailed parametrics and guitar amp tweakings. It used to be that there was a big difference between golden age hip-hop and the newer stuff as more advanced samplers started to come out. It's a big enough deal that I really cannot get, say, an SP-1200. It'd be easier for me to get a Marshall Stack.Thing is, I bet it's possible to set up a Console system so it gives you everything you'd enjoy out of a real-deal SP-1200, possibly even including the delicate timing of the pads (very fine-grained quantization of time that is not quite as finely grained as sample-accurate DAWs…) but free and open source.For now, I hope you enjoy DeRez3, because it's a glimpse into that future, and it might just be a window into a hi-fi lo-fi that perfectly fits your sound. I mean, I'm a rock guy with no sampler experience and even I am tempted. I feel like this one might really break out of my prog-rock box. Give it a try :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeRez3 final : public Effect<T>
{
    double rezA;
    double rezB;
    double bitA;
    double bitB;
    double wetA;
    double wetB;
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_InR,
        bez_UnInR,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];

    float A;
    float B;
    float C;

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeRez3()
    {
        A = 1.0;
        B = 1.0;
        C = 1.0;
        rezA = 1.0;
        rezB = 1.0;
        bitA = 1.0;
        bitB = 1.0;
        wetA = 1.0;
        wetB = 1.0;
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
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
        kParamC = 2,
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

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
            case kParamC: return 1.0; break;

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
            case kParamC: return "dry/wet"; break;

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
            case kParamC: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;

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
            case kParamC: return ""; break;

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
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        rezA = rezB;
        rezB = pow(A, 3.0) / overallscale;
        bitA = bitB;
        bitB = (B * 15.0) + 1.0;
        wetA = wetB;
        wetB = C * 2.0;
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            double temp = (double)sampleFrames / inFramesToProcess;
            double rez = (rezA * temp) + (rezB * (1.0 - temp));
            double bit = (bitA * temp) + (bitB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            if (rez < 0.0005) {
                rez = 0.0005;
            }
            double bitFactor = pow(2.0, bit);
            double dry = 2.0 - wet;
            if (wet > 1.0) {
                wet = 1.0;
            }
            if (wet < 0.0) {
                wet = 0.0;
            }
            if (dry > 1.0) {
                dry = 1.0;
            }
            if (dry < 0.0) {
                dry = 0.0;
            }
            // this bitcrush makes 50% full dry AND full wet, not crossfaded.
            // that's so it can be on tracks without cutting back dry channel when adjusted
            inputSampleL *= bitFactor;
            inputSampleL = floor(inputSampleL + (0.5 / bitFactor));
            inputSampleL /= bitFactor;
            inputSampleR *= bitFactor;
            inputSampleR = floor(inputSampleR + (0.5 / bitFactor));
            inputSampleR /= bitFactor;
            bez[bez_cycle] += rez;
            bez[bez_SampL] += (inputSampleL * rez);
            bez[bez_SampR] += (inputSampleR * rez);
            if (bez[bez_cycle] > 1.0) {
                bez[bez_cycle] -= 1.0;
                bez[bez_CL] = bez[bez_BL];
                bez[bez_BL] = bez[bez_AL];
                bez[bez_AL] = inputSampleL;
                bez[bez_SampL] = 0.0;
                bez[bez_CR] = bez[bez_BR];
                bez[bez_BR] = bez[bez_AR];
                bez[bez_AR] = inputSampleR;
                bez[bez_SampR] = 0.0;
            }
            double CBL = (bez[bez_CL] * (1.0 - bez[bez_cycle])) + (bez[bez_BL] * bez[bez_cycle]);
            double CBR = (bez[bez_CR] * (1.0 - bez[bez_cycle])) + (bez[bez_BR] * bez[bez_cycle]);
            double BAL = (bez[bez_BL] * (1.0 - bez[bez_cycle])) + (bez[bez_AL] * bez[bez_cycle]);
            double BAR = (bez[bez_BR] * (1.0 - bez[bez_cycle])) + (bez[bez_AR] * bez[bez_cycle]);
            double CBAL = (bez[bez_BL] + (CBL * (1.0 - bez[bez_cycle])) + (BAL * bez[bez_cycle])) * 0.5;
            double CBAR = (bez[bez_BR] + (CBR * (1.0 - bez[bez_cycle])) + (BAR * bez[bez_cycle])) * 0.5;
            inputSampleL = (wet * CBAL) + (dry * drySampleL);
            inputSampleR = (wet * CBAR) + (dry * drySampleR);
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
} // namespace airwindohhs::derez3
