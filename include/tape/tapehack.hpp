#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapehack {

constexpr std::string_view k_name{ "TapeHack" };
constexpr std::string_view k_short_description{
    "TapeHack is a new dimension in tape realism, abstracted to software."
};
constexpr std::string_view k_long_description{
    "It started with a paper by Spencer Walters, on this subject: what's the role of hysteresis in magnetic tape recording? We tend to assume that's fine, we're making it linear, apart from all the soft saturation of course. Even then, that's not quite where it starts: it's research on Dolby HX-Pro that got Spencer thinking. I'm not sure the Dolby people quite understood the implications of what they had, as that was an era of 'remove all distortions'.Gradually, Spencer started to figure out what he had, and there's analog hardware coming out from Walters Audio that I frankly covet. And Spencer wrote a paper, talking of magnetic flux and relative coercivity and hysteresis loops. I frankly don't have the math he does, but I worked with analog tape growing up, and have a good idea of how it responds. And there was this set of diagrams, of a 1kHz wave magnetizing tape, and what you actually get out of it without and with tape bias…It showed there was a dead spot, if you didn't bias. Well, I knew that: I'd coded something like it into ToTape 7 and 8, letting you dial in a section of Type AB distortion if it was 'underbiased'. But look again: that dead spot is still there, if you bias. The recording is sweeping back and forth across it at 100 kHz, but it doesn't stop that dead spot, where magnetic tape's coercivity hasn't kicked in, from still existing. You're just keeping it way busier than before, but a 100k wave still has to sweep through zero each cycle.What if this dead spot was central to tape sound as we know it? What if there's always a hint of it, a bit of antisaturation in there at the opposite dynamic extreme from TAEP PHAT, as shown in Spencer's diagrams? More importantly, could I code something like that?Sure could :)TapeHack does a number of interesting things due to what I came up with. First, it's very efficient: it's a simpler, lower-CPU form of a sin() function, so it has the softclip nature. It's made to have this 'extra zone' by manipulating how an approximation of sin() is made, so I can also do variations on it and asin() and combinations of them, for the purposes of future Console versions. Doing that tends to take away the saturation effects, so there would be additional layers of TapeHack for making 'vintage consoles' sound as we experienced them.The differences in waveform can be described. Unlike sin(), TapeHack hits a soft saturation point and then takes the flat top and begins to extend it, while turning the 'quiet part' of the wave into more of a trapezoid shape that's remarkably persistent in its slope. The steepening curve of the soft-clip gets sharper and sharper while these other factors stay relatively consistent.As a result, TapeHack does 'tape compression' better than anything I've ever made, full stop. Turns out you can make sharp softclip corners, but we hear 'brighter' as a function of how much more steeply that center part of the waveform slopes. This is likely to be exactly the same in the Walters T805 (patent pending) even though that's fully analog: you can see it showing up in oscilloscope traces in Spencer's paper. It's heard as a shockingly believable 'tape bark' on things like drums, it's heard plainly when you run a heavy guitar sound hot to 'tape' to make it sit more up-front, but just as importantly, it livens up a more quietly 'recorded' track by giving it that characteristic dynamism and optimal distribution of peak energy.A side note: I've posted classic vinyl records and measured them, tracking peak vs. RMS energy, as that's been my own parallel pursuit of what went on in those very sonic Seventies, when things just sounded better to me. A common factor is WAY more emphasis on peak energy and crest factor, with it persistently sitting much better dynamically than digital accuracy gives you. Well, this lines up with that observation exactly. Applying this hysteresis (or, like with TapeHack, just using the transfer function directly) produces both the peak distribution and the desired sound.There will be more about this: and yeah, Spencer's aware of what I'm doing and cool with it. In fact he'd like to know if my testimonial (in his thread on the matter) can go on his website. My work on TapeHack suggested to me that he's really on to something with this, I'm happy to say so, and my more abstracted version is something we can all enjoy. (oh: it's also perfectly suited to oversampling for folks with something like Reaper where you can just do that. I won't be, because I think it gains more from the immediacy of direct processing than you lose from what little aliasing it'll produce)I'll be saving up for the hardware Spencer makes, and I'll be using what I learned in TapeHack for many things. Hope you like it!"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class TapeHack final : public Effect<T>
{
    float A;
    float B;
    float C;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    TapeHack()
    {
        A = 0.1;
        B = 1.0;
        C = 1.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.1; break;
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "output"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Output"; break;
            case kParamC: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
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

        double inputGain = A * 10.0;
        double outputGain = B * 0.9239;
        double wet = C;
        while (--sampleFrames >= 0) {
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
            inputSampleL = fmax(fmin(inputSampleL * inputGain, 2.305929007734908), -2.305929007734908);
            double addtwo = inputSampleL * inputSampleL;
            double empower = inputSampleL * addtwo; // inputSample to the third power
            inputSampleL -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleL += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleL -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleL += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleL -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            inputSampleR = fmax(fmin(inputSampleR * inputGain, 2.305929007734908), -2.305929007734908);
            addtwo = inputSampleR * inputSampleR;
            empower = inputSampleR * addtwo; // inputSample to the third power
            inputSampleR -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleR += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleR -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleR += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleR -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            inputSampleL = (inputSampleL * outputGain * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * outputGain * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::tapehack
