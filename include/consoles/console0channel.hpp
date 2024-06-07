#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console0channel {

constexpr std::string_view k_name{ "Console0Channel" };
constexpr std::string_view k_short_description{
    "Console0Channel is a radical, minimalist Console system."
};
constexpr std::string_view k_long_description{
    "If the weirdness of Airwindows Console systems tends to throw you, it's probably good to skip this one. You'll either hear what it's about… or you won't. This is sort of an extreme position of a line of inquiry that not everybody buys into. It's really quite clear what's going on and why, but people disagree very intensely on whether it matters. Rather than make allowances, I thought I'd push it to the most radical extreme possible to see what happened.Meet Console Zero.There will probably never be anything that goes farther in this direction… not in the purely digital domain, anyway. Not as free open source plugins that everyone can have. It's easy to get external hardware to do some of these things, but that's costly. Console Zero can be used by everybody… if, that is, you can deal with the demands it makes.Console Zero is the Airwindows Console concept, crossed with the BitShiftGain concept, and pushed literally as far as it can possibly go. It has built-in gain and pan… kinda. It has aliasing-suppression filtering… kinda. It does the saturate on channels and then anti-saturate on the buss… kinda.Everything, everything is sacrificed to the Mantissa Gods. The idea is, with many analog-to-digital converters, with even fairly humble ones you can get quite a lot out of them if you just pass the audio straight through. Analog to digital straight to analog again? Often, it'll capture a really good sound. It all goes to hell when you start trying to work with it in the box.Even on a system like Console8, with NO other processing, there might be hundreds of mathematical operations on every channel, thousands. Things like sine and arcsine functions do a lot of processing to be accurate. Biquad filters can get you nice accurate filtering, but require lots of math operations to function. The stuff modern mix topologies get up to could be tens of thousands of calculations, PER sample, PER channel on the way to the final mix. We take this for granted and nobody tries to make the opposite approach work, certainly not while including filtering and analog emulation.Console Zero does maybe eight math operations, per sample, per channel, between input and the final mix output, that touch the mantissa of the input values. INCLUDING the actual mix. Including the Console saturation/antisaturation system. Including the aliasing-suppresion filtering. Eight, if I remember correctly… for the ENTIRE chain.How? By leaning on bit shifting, to an insane, excessive degree. It's a giant trade-off. All level and pan is done with built-in BitShiftPan. (even on the buss, and you shouldn't touch it… but why not? The point is that it doesn't alter any mantissas). That is mixing in 6 dB increments, 3 if you count moving stuff one notch to the side to make it quieter. The filtering is strictly simple averaging. One operation, and a bit shift to get back the original gain: the bit shift doesn't touch the tone. The Console system is simplified so much that it's just one calculation and another bit shift. Everything is traded off for minimalism. The saturation produces slightly more harmonics than something like PurestConsole, but almost without calculation.You get a mix together in Console Zero through arrangement, through broad strokes. You CANNOT fuss with it. It's almost LCR panning, except there are obvious left-center and right-center points included, and a range of pannings nearer the edges. All these points sound exactly like unprocessed raw digital audio, all of them pass through the unaltered mantissa from the input sample. The entire mix, with all its levels and pannings, puts through every single track as if it was the untouched raw signal without even a gain change… because there is not a single mantissa change to any track on any channel, going into the Console processing.This may mean NOTHING to you. If you got here and that's you, thank you for the great patience. And… does it sound good? All of this is in pursuit of a particular KIND of sound, very unlike typical DAW sound. Do you hear that in my example, do you hear it if you attempt a Console Zero mix? If you hear nothing unusual, move on, this is not for you.Some of you are going to lose your minds over how good this can sound. This is for you :)Wait, it gets worse! You have to run 96k this time, 192k if you really want extended highs. If you're at 44.1k, the averaging will still work. It will be REAL DARK. Sorry! That means some people could do mixes at 44.1k with some of the desirable qualities we're seeking here, just so long as it's meant to be dark. Plenty of genres that can do that. But you should be running 96k. These plugins are SO lightweight, it's hard to even express how light they are: again, eight operations across the entire mix buss, not thousands or hundreds of thousands, per sample. That's from input to mix output. That will stand up to a lot of tracks… and the other secret is, Console Zero LOVES high track counts. The more stuff you have layered, the more freedom you have to do subtle loudness adjustments: a 3 dB nudge on one layer of one track made of four layers means the total loudness of those doubles, is the same as adjusting all of them by 0.75 dB. It will nudge one of the layers sideways in the stereo picture. And? The task becomes managing aggregates of tracks, thinking only of the big picture, letting some stuff be buried in the mix because you simply can't nudge it up in volume to balance.And it can all come together and work, better than you'd ever imagine, and quicker… and sound like life, not like a DAW.Either this will work for you, or it absolutely won't. Either it'll blow away anything you've tried, or completely frustrate you. This one's gonna be polarizing. It goes so far in the directions I try to achieve, that I'm certain it'll blow some of you away, and it's gonna make other people really mad. What price is an untouched mantissa? How much can you hear the inevitable digital erosion of overprocessing… and just how little processing can you do in a mix and still have a mix at all?Console Zero… if you dare :) I will return to more accessible stuff, analog emulations etc, now. There will be no beating Console Zero for what it does. The rest is doing interesting tone colors, which I promise I'll bring you… but if this is your jam, you're done. There won't be any better from me or anybody else. It will beat many external summing busses. Console Zero is yours now."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console0Channel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double avgAL;
    double avgAR;
    double avgBL;
    double avgBR;
    float A;
    float B;

  public:
    Console0Channel()
    {
        A = 0.5;
        B = 0.5;
        avgAL = avgAR = avgBL = avgBR = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "vol"; break;
            case kParamB: return "pan"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Vol"; break;
            case kParamB: return "Pan"; break;

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

        double gainControl = (A * 0.5) + 0.05; // 0.0 to 1.0
        int gainBits = 20; // start beyond maximum attenuation
        if (gainControl > 0.0) {
            gainBits = floor(1.0 / gainControl);
        }
        int bitshiftL = gainBits - 3;
        int bitshiftR = gainBits - 3;
        double panControl = (B * 2.0) - 1.0; //-1.0 to 1.0
        double panAttenuation = (1.0 - fabs(panControl));
        int panBits = 20; // start centered
        if (panAttenuation > 0.0) {
            panBits = floor(1.0 / panAttenuation);
        }
        if (panControl > 0.25) {
            bitshiftL += panBits;
        }
        if (panControl < -0.25) {
            bitshiftR += panBits;
        }
        if (bitshiftL < -2) {
            bitshiftL = -2;
        }
        if (bitshiftL > 17) {
            bitshiftL = 17;
        }
        if (bitshiftR < -2) {
            bitshiftR = -2;
        }
        if (bitshiftR > 17) {
            bitshiftR = 17;
        }
        double gainL = 1.0;
        double gainR = 1.0;
        switch (bitshiftL)
        {
            case 17: gainL = 0.0; break;
            case 16: gainL = 0.0000152587890625; break;
            case 15: gainL = 0.000030517578125; break;
            case 14: gainL = 0.00006103515625; break;
            case 13: gainL = 0.0001220703125; break;
            case 12: gainL = 0.000244140625; break;
            case 11: gainL = 0.00048828125; break;
            case 10: gainL = 0.0009765625; break;
            case 9: gainL = 0.001953125; break;
            case 8: gainL = 0.00390625; break;
            case 7: gainL = 0.0078125; break;
            case 6: gainL = 0.015625; break;
            case 5: gainL = 0.03125; break;
            case 4: gainL = 0.0625; break;
            case 3: gainL = 0.125; break;
            case 2: gainL = 0.25; break;
            case 1: gainL = 0.5; break;
            case 0: gainL = 1.0; break;
            case -1: gainL = 2.0; break;
            case -2: gainL = 4.0; break;
        }
        switch (bitshiftR)
        {
            case 17: gainR = 0.0; break;
            case 16: gainR = 0.0000152587890625; break;
            case 15: gainR = 0.000030517578125; break;
            case 14: gainR = 0.00006103515625; break;
            case 13: gainR = 0.0001220703125; break;
            case 12: gainR = 0.000244140625; break;
            case 11: gainR = 0.00048828125; break;
            case 10: gainR = 0.0009765625; break;
            case 9: gainR = 0.001953125; break;
            case 8: gainR = 0.00390625; break;
            case 7: gainR = 0.0078125; break;
            case 6: gainR = 0.015625; break;
            case 5: gainR = 0.03125; break;
            case 4: gainR = 0.0625; break;
            case 3: gainR = 0.125; break;
            case 2: gainR = 0.25; break;
            case 1: gainR = 0.5; break;
            case 0: gainR = 1.0; break;
            case -1: gainR = 2.0; break;
            case -2: gainR = 4.0; break;
        }
        double temp;
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
            temp = inputSampleL;
            inputSampleL = (inputSampleL + avgAL) * 0.5;
            avgAL = temp;
            temp = inputSampleR;
            inputSampleR = (inputSampleR + avgAR) * 0.5;
            avgAR = temp;
            inputSampleL *= gainL;
            inputSampleR *= gainR;
            if (inputSampleL > 1.4137166941154) {
                inputSampleL = 1.4137166941154;
            }
            if (inputSampleL < -1.4137166941154) {
                inputSampleL = -1.4137166941154;
            }
            if (inputSampleL > 0.0) {
                inputSampleL = (inputSampleL / 2.0) * (2.8274333882308 - inputSampleL);
            }
            else {
                inputSampleL = -(inputSampleL / -2.0) * (2.8274333882308 + inputSampleL);
            }
            if (inputSampleR > 1.4137166941154) {
                inputSampleR = 1.4137166941154;
            }
            if (inputSampleR < -1.4137166941154) {
                inputSampleR = -1.4137166941154;
            }
            if (inputSampleR > 0.0) {
                inputSampleR = (inputSampleR / 2.0) * (2.8274333882308 - inputSampleR);
            }
            else {
                inputSampleR = -(inputSampleR / -2.0) * (2.8274333882308 + inputSampleR);
            }
            // BigFastSin channel stage
            temp = inputSampleL;
            inputSampleL = (inputSampleL + avgBL) * 0.5;
            avgBL = temp;
            temp = inputSampleR;
            inputSampleR = (inputSampleR + avgBR) * 0.5;
            avgBR = temp;
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
} // namespace airwindohhs::console0channel
