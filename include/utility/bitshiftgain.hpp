#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::bitshiftgain {

constexpr std::string_view k_name{ "BitShiftGain" };
constexpr std::string_view k_short_description{
    "BitShiftGain is the ‘One Weird Trick’ perfect boost/pad, but in 6db increments ONLY."
};
constexpr std::string_view k_long_description{
    "Here is one final trick for clean gain aficionados.Turns out the only way to get cleaner gain trim than PurestGain, with its high mathematical precision and noise shaping… is not to do any of that. No fancy math, no noise shaping or dither. Just a very narrowly defined boost or cut, in the form of a ‘bit shift’.Doing this means your waveform is scaled up or down by increments of 6 dB exactly. No 3 db, no 9, no 7 or even 6.001! Only 6 or 12 or 18 and so on, up or down. Select the number of bits you want to shift, and BitShiftGain applies the exact number, not even calculating it in floating-point through repeated operations: from a look-up table to make sure it’s absolutely exact and precise.And when it does, all the bits shift neatly to the side inside your audio, and whether you lose the smallest and subtlest or gain up and fill it in with a zero… every single sample in your audio is in exactly, EXACTLY the same relative position to the others. Apart from the gain or loss of the smallest bit, there is literally no change to the audio at all: if there was a noise shaping, it would have nothing to work with.Perfection, at exclusively increments of 6 dB. That’s the catch. You probably can’t mix with gain changes that coarse (though it’s tempting to try!) but here’s what you can do: you can take 24-bit dithers, gain down 8 bits in front and 8 bits up after, and have a perfect 16 bit dither. Or a 17 bit, if that pleases you… or shift 16 bits down so you can hear what your dither’s noise floor acts like (we’ll be doing lots of that when I start bringing out the dithers). +-16 bits of gain trim is a very big boost or cut. The overall range of BitShiftGain is huge. But the real magic of BitShiftGain is the sheer simplicity of the concept. Provided your math is truly, rigorously accurate and your implementation’s perfect, gain trim with bit shift is the only way in digital (fixed OR floating point) where you can apply a change, and the word length of your audio doesn’t have to expand, AND every sample which remains in your audio continues to be in exactly the same relation to all the others.Digital audio is like some crystalline structure: it’s fragile, brittle, and suffers tiny fractures at the tiniest alterations. There’s almost nothing you can do in digital audio that’s not going to cause some damage. But as long as you stick to 6 dB steps and rigidly control the implementation (BitShiftGain doesn’t even store the audio in a temporary variable!), you can chip away at that least significant bit, and the whole minutes-or-hours-long crystalline structure of digital bits can remain perfectly intact above it."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class BitShiftGain final : public Effect<T>
{
    float A;

  public:
    BitShiftGain()
    {
        A = 0.5;
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
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bitshift"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "BitShift"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bits"; break;

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

        int bitshiftGain = (A * 32) - 16;
        double gain = 1.0;
        switch (bitshiftGain)
        {
            case -16: gain = 0.0000152587890625; break;
            case -15: gain = 0.000030517578125; break;
            case -14: gain = 0.00006103515625; break;
            case -13: gain = 0.0001220703125; break;
            case -12: gain = 0.000244140625; break;
            case -11: gain = 0.00048828125; break;
            case -10: gain = 0.0009765625; break;
            case -9: gain = 0.001953125; break;
            case -8: gain = 0.00390625; break;
            case -7: gain = 0.0078125; break;
            case -6: gain = 0.015625; break;
            case -5: gain = 0.03125; break;
            case -4: gain = 0.0625; break;
            case -3: gain = 0.125; break;
            case -2: gain = 0.25; break;
            case -1: gain = 0.5; break;
            case 0: gain = 1.0; break;
            case 1: gain = 2.0; break;
            case 2: gain = 4.0; break;
            case 3: gain = 8.0; break;
            case 4: gain = 16.0; break;
            case 5: gain = 32.0; break;
            case 6: gain = 64.0; break;
            case 7: gain = 128.0; break;
            case 8: gain = 256.0; break;
            case 9: gain = 512.0; break;
            case 10: gain = 1024.0; break;
            case 11: gain = 2048.0; break;
            case 12: gain = 4096.0; break;
            case 13: gain = 8192.0; break;
            case 14: gain = 16384.0; break;
            case 15: gain = 32768.0; break;
            case 16: gain = 65536.0; break;
        }
        // we are directly punching in the gain values rather than calculating them
        while (--sampleFrames >= 0)
        {
            *out1 = *in1 * gain;
            *out2 = *in2 * gain;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::bitshiftgain
