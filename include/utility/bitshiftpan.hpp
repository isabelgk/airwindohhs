#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::bitshiftpan {

constexpr std::string_view k_name{ "BitShiftPan" };
constexpr std::string_view k_short_description{
    "BitShiftPan gives you a gain and a pan control that are ONLY done using bit shifts."
};
constexpr std::string_view k_long_description{
    "This is a request from YouTube comments, but I had no idea how well it'd work out! BitShiftGain is a long-standing secret weapon of mine. On almost every video, I'm losslessly dropping 6dB using BitShiftGain. But what would you get if you applied this to pan?You'd get a pan where center was quite a bit louder than sides (there's no 3 dB pan law from bit shifts), but the first steps to left and right are QUITE a lot to the side. It's not at all LCR panning, but if something's center you know it, and if something's to the side it's WAY to the side. Then, you have a succession of further-to-the-side positions that are progressively quieter, all the way to hard L and R. If that was all it was, this would seem really pointless and arbitrary.BUT.If you can construct a mix this way, you can construct a mix where every single gain setting, every pan position, every location in the mix, is Bit Shift Gain: utterly and completely lossless. No requantization, just like with BitShiftGain itself, but in full stereo (within these constraints). You're picking locations, but they're not LCR locations, they're a range of potential locations.There's more. Mixing with BitShiftGain in mono is impossibly crude. 6dB increments are seemingly impossible to mix with, absurd, insulting to even consider. But if you tick a track one step over to the side… that's now 3dB down, not 6. You've losslessly cut one side 6dB while leaving the other one unaltered.If you had two tracks, a doubletrack, and did this with just one of them, your 'track' (that's really two tracks) can shift 1.5 dB down, to the ear. Starting to sound more interesting? If you had four tracks and shifted just one of them, that's an apparent shift of 0.75 dB.Mixing isn't just about taking a single track and making it perfect. Mixing is how tracks sit relative to other tracks. If you have a full mix, and a track 40 dB down steps closer to the center bringing it up 3 dB in total, that makes all the other tracks seem just a tiny bit quieter in contrast. At every step, your ability to fiddle with 0.1dB adjustments is completely hobbled… but the framing of the TOTAL mix can take a whole new form.And at every point, across every track, at every position in the stereo field, the mantissa of every audio sample is EXACTLY as captured by the converter. Once it's mixed, you'll get a composite, but everything being fed to the mix buss at every level in every position is exactly the raw sample… scaled to fit.If you liked BitShiftGain for its utterly uncompromising transparency, beyond anything else even possible… now you have it, but with panning.If this approach, so ruthless in the desire to hang on to raw unprocessed intensity from the original digital captures, seems interesting… next week is Console Zero, built from the ground up to work using almost entirely bit shifts even inside the saturation/antisaturation calculations and anti-alias filtering. BitShiftPan is an ultimately clean gain trim, and apart from the 'steppy' positioning and lack of pan law, it's very normal and approachable. Console Zero… is radical.Thanks for the suggestion to try applying this to panning. Who knew so much would happen as a result?"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class BitShiftPan final : public Effect<T>
{
    float A;
    float B;

  public:
    BitShiftPan()
    {
        A = 0.5;
        B = 0.5;
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
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "vol";
            case kParamB: return "pan";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Vol";
            case kParamB: return "Pan";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
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
        while (--sampleFrames >= 0)
        {
            *out1 = *in1 * gainL; // in this plugin, all the work is done
            *out2 = *in2 * gainR; // before the buffer is processed
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::bitshiftpan
