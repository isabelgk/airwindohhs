#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::c5rawchannel {

constexpr std::string_view k_name{ "C5RawChannel" };
constexpr std::string_view k_short_description{
    "C5RawConsole is the original Console5 algorithm, with optional very gentle DC suppression."
};
constexpr std::string_view k_long_description{
    "Back in December of 2017, I launched Console5. It had some innovations I’d come up with that got it to a height of sonic deliciousness I’d not reached before. I put it out Xmas morning… and immediately panicked and scrambled, because I’d been testing it with a sort of ‘choir’ and hadn’t thrown enough softsynths, etc. at it. And when you hit it with a intensely asymmetrical slew (how fast the waveform moves) it flipped out and began producing powerful DC voltages. Ever since, I’ve been doing variations on Console5 (which all interoperate with each other) that avoid the problem. That same day, I rushed out a bugfix. I tried an alternate method for getting that tone, which wasn’t as good but was more resilient to DC offset. I came up with a good workaround for Console5, which watches for silences as a good place to pull back on DC, and set up the final Console5 that way. I released PurestConsole to offer a cleaner, clearer version that totally avoids the problem in the first place. I released PDConsole, because if you can’t have the original Console5 tone algorithm, why not have PurestConsole crossed with PurestDrive (which doesn’t have the DC issue)? All the time, there were SOME sorts of audio that were fine… or mostly fine… with the original, special-toned Console5. Technically, nothing’s perfect: if you went for long enough under the original Console5, the DC would inevitably drift, unless the tones were really perfectly symmetrical. But it was a lost opportunity: the DC bug was so severe that the original version was forever lost, banished for endangering DC-coupled equipment and never to be heard again. UNTIL NOW :D Don’t use this Console5 version if you’re not willing to work with it and keep an eye on it. Much like the plugin DC Voltage, which simply turns the output into a fixed DC offset for signal-processing or testing purposes, C5RawConsole is capable of giving you signals that are bad for your gear. The final Console5 is very nice and automatically handles DC problems, PurestConsole can’t possibly give you DC issues, neither can PDConsole, and you can mix and match these if you like. There’s plenty of Console5 stuff out there if you just want to get some sounds and go. But, that original algorithm captured people’s hearts. The way it handled the fabric of sound was something a little special… so now, that original Console5 lives again. The Xmas morning one, the ‘wow factor’, dangerous one. With one tiny detail added, that can make it tame if you know how to handle it. The control ‘Centering Force’ defaults to 0.0, which is the ‘Xmas Morning’ setting. If you turn it up to maybe 0.7 or 0.8, you have the first bugfix, where the correcting force took away some bass just to make sure the problem was fixed. Cranked up, it’s a little bit like a highpass and you can use it to tighten non-bass tracks, seeing as it’s there… it’s like a free highpass, but very gentle slope. But if you keep it at 0.5 or below, Centering Force reacts so very slowly that it won’t touch your bass. I don’t think it’s really perceptible at 0.5, but you can always be even more cautious and go to 0.25 or so. At settings close to 0.0, it will take minutes to erode away any DC offset that sneaks in. And at 0.0 it’s completely defeated and there’s no centering force at all. This is for the people who wanted that very first version back. You can couple the plugins with other, outboard DC correction filters if you like, or you can use the Centering Force (especially if you see voltages lingering anywhere in your mix, that tells you that you need to apply some). You can treat it like there’s a built-in highpass as part of the circuitry of the Console, and tune the dominance of your subs very conveniently without having to run an extra highpass (much less a multi-pole, steep-cutoff highpass). I guarantee C5RawConsole can be used for this, because it dials between the extremes nicely, from noticeable subs reduction at 1.0 to absolutely no reduction at 0.0, with a really really extreme exaggeratedly logarithmic curve making it so the middle setting is still pretty much no bass rolloff. I think this should give people a chance to play. I could’ve stuck with the final Console5, in practical terms I think that’s the one most people should use. But Airwindows people like pushing the limits, so I needed to make that possible. Now, if you use C5RawConsole and leave the centering force off, you have that untame, primitive version that came out first and was gone in mere hours in a frenzy of bugfixing. It’s what I made when testing obsessively against the ‘choir’ sounds, trying to get something special while still mourning the loss of my Mom, who sang in a choir. That’s where that first Console5 came from, and it pleases me to (with caution) bring back that original tone, and let people have access to it. Just be careful how you apply it, and if you see voltages hanging around (on tracks or the buss), put some of the Centering Force in! We could all use some of that :)"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class C5RawChannel final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'conn';
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastFXChannelL;
    double lastSampleChannelL;
    double lastFXChannelR;
    double lastSampleChannelR;
    float A;

  public:
    C5RawChannel()
    {
        A = 0.0;
        lastFXChannelL = 0.0;
        lastSampleChannelL = 0.0;
        lastFXChannelR = 0.0;
        lastSampleChannelR = 0.0;
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
        switch (index) {
            case kParamA: A = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "center"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Center"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(A, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double centering = A * 0.5;
        centering = 1.0 - pow(centering, 5);
        // we can set our centering force from zero to rather high, but
        // there's a really intense taper on it forcing it to mostly be almost 1.0.
        // If it's literally 1.0, we don't even apply it, and you get the original
        // Xmas Morning bugged-out Console5, which is the default setting for Raw Console5.
        double differenceL;
        double differenceR;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0) {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            differenceL = lastSampleChannelL - inputSampleL;
            lastSampleChannelL = inputSampleL;
            // derive slew part off direct sample measurement + from last time
            differenceR = lastSampleChannelR - inputSampleR;
            lastSampleChannelR = inputSampleR;
            // derive slew part off direct sample measurement + from last time
            if (differenceL > 1.0) {
                differenceL = 1.0;
            }
            if (differenceL < -1.0) {
                differenceL = -1.0;
            }
            if (differenceR > 1.0) {
                differenceR = 1.0;
            }
            if (differenceR < -1.0) {
                differenceR = -1.0;
            }
            inputSampleL = lastFXChannelL + asin(differenceL);
            lastFXChannelL = inputSampleL;
            if (centering < 1.0) {
                lastFXChannelL *= centering;
            }
            // if we're using the crude centering force, it's applied here
            inputSampleR = lastFXChannelR + asin(differenceR);
            lastFXChannelR = inputSampleR;
            if (centering < 1.0) {
                lastFXChannelR *= centering;
            }
            // if we're using the crude centering force, it's applied here
            if (lastFXChannelL > 1.0) {
                lastFXChannelL = 1.0;
            }
            if (lastFXChannelL < -1.0) {
                lastFXChannelL = -1.0;
            }
            if (lastFXChannelR > 1.0) {
                lastFXChannelR = 1.0;
            }
            if (lastFXChannelR < -1.0) {
                lastFXChannelR = -1.0;
            }
            // build new signal off what was present in output last time
            // slew aspect
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
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
} // namespace airwindohhs::c5rawchannel
