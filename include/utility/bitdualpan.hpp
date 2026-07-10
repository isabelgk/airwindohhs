#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdlib>

namespace airwindohhs::bitdualpan {

constexpr std::string_view k_name{ "BitDualPan" };
constexpr std::string_view k_short_description{
    "BitDualPan is dual pan that's almost BitShiftGain."
};
constexpr std::string_view k_long_description{
    "This is by request, and also a sort of experiment.There's a PurestDualPan already. It's out, it's free, it's yours. There is also a BitShiftPan, which is a lot like BitShiftGain except you can control the sides independently. The first, PurestDualPan, is simply a dual pan in long double precision and dithered to the audio buss. This is unreasonable overkill but I've always been fine with that. I'm trying to get a sound and only have bits to work with. So in the 'bit' versions, they manage to get the sound of 'utterly pristine and undiminished' through gain changes by applying simple gain changes but only in 6dB increments.In floating point, whether it's 32 bit or 64 bit double precision, if you do that something special happens. A floating point number has two parts: a mantissa and an exponent. The mantissa is literally a tiny chunk of fixed point audio data (well, unless it's just math data, but the same thing applies). The exponent? That's how loud to play the first chunk. All the 'bit' plugins do is alter only the exponent, and pass through the mantissa untouched. Perfect audio… in 6.08dB increments.For BitDualPan, that's what you get… almost.If you leave the sides hard-panned, the mantissas are untouched. If you swap the sides, same deal! If you set one side, or the other, to zero (silence) and then do anything at all with the other side, same deal. And then, if you pan in the sides (in 6dB increments, still) and leave both audible, that means both sides will be altered. They're added together, so they're a floating point add from two sources both of which were bit-shift gain staged, meaning the mantissa will be altered, and that means… what?I honestly don't know. I'm sure I don't have the nicest playback system of anybody who uses my plugins, or the best ear: I'm just willing to try stuff and this becomes a new form of minimal processing. Instead of dithered multiplies, it's summing a couple of floating-point inputs in such a way that there is only an add, nothing else. It's certainly not gonna be any worse than any ordinary DAW summing, but does it gain anything from setting up the source audio so both sources are the exact mantissa of the input channels? It reduces the whole thing to a single add (per channel) and no multiplies, which are normally what you see in this sort of thing.If you feel this one, use it in good spirits. It manages to cut the digital math to an absolute minimum but then apply it in a way I don't often see, and it sits in the 'dual pan' role that I've long overlooked. And it uses the mechanic the Bit plugins enjoy: when you're not touching the mantissa, you also don't dither because the output's already perfect. So, BitDualPan also doesn't floating-point dither, to see if there's a sound that comes out of the only mechanic being a floating-point add.I'll get right back to plugins that have more going on. There's only so much you can do with 'what is the sound of one floating point add per channel?' But here you are, to compare with PurestDualPan. Now you can pick whichever you like best :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class BitDualPan final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    // default stuff

  public:
    BitDualPan()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "l vol"; break;
            case kParamB: return "l pan"; break;
            case kParamC: return "r vol"; break;
            case kParamD: return "r pan"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "L Vol"; break;
            case kParamB: return "L Pan"; break;
            case kParamC: return "R Vol"; break;
            case kParamD: return "R Pan"; break;

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
            case kParamD: return std::to_string(D); break;

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
            case kParamD: return ""; break;

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
        double LgainL = 1.0;
        double LgainR = 1.0;
        switch (bitshiftL) {
            case 17: LgainL = 0.0; break;
            case 16: LgainL = 0.0000152587890625; break;
            case 15: LgainL = 0.000030517578125; break;
            case 14: LgainL = 0.00006103515625; break;
            case 13: LgainL = 0.0001220703125; break;
            case 12: LgainL = 0.000244140625; break;
            case 11: LgainL = 0.00048828125; break;
            case 10: LgainL = 0.0009765625; break;
            case 9: LgainL = 0.001953125; break;
            case 8: LgainL = 0.00390625; break;
            case 7: LgainL = 0.0078125; break;
            case 6: LgainL = 0.015625; break;
            case 5: LgainL = 0.03125; break;
            case 4: LgainL = 0.0625; break;
            case 3: LgainL = 0.125; break;
            case 2: LgainL = 0.25; break;
            case 1: LgainL = 0.5; break;
            case 0: LgainL = 1.0; break;
            case -1: LgainL = 2.0; break;
            case -2: LgainL = 4.0; break;
        }
        switch (bitshiftR) {
            case 17: LgainR = 0.0; break;
            case 16: LgainR = 0.0000152587890625; break;
            case 15: LgainR = 0.000030517578125; break;
            case 14: LgainR = 0.00006103515625; break;
            case 13: LgainR = 0.0001220703125; break;
            case 12: LgainR = 0.000244140625; break;
            case 11: LgainR = 0.00048828125; break;
            case 10: LgainR = 0.0009765625; break;
            case 9: LgainR = 0.001953125; break;
            case 8: LgainR = 0.00390625; break;
            case 7: LgainR = 0.0078125; break;
            case 6: LgainR = 0.015625; break;
            case 5: LgainR = 0.03125; break;
            case 4: LgainR = 0.0625; break;
            case 3: LgainR = 0.125; break;
            case 2: LgainR = 0.25; break;
            case 1: LgainR = 0.5; break;
            case 0: LgainR = 1.0; break;
            case -1: LgainR = 2.0; break;
            case -2: LgainR = 4.0; break;
        }
        gainControl = (C * 0.5) + 0.05; // 0.0 to 1.0
        gainBits = 20; // start beyond maximum attenuation
        if (gainControl > 0.0) {
            gainBits = floor(1.0 / gainControl);
        }
        bitshiftL = gainBits - 3;
        bitshiftR = gainBits - 3;
        panControl = (D * 2.0) - 1.0; //-1.0 to 1.0
        panAttenuation = (1.0 - fabs(panControl));
        panBits = 20; // start centered
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
        double RgainL = 1.0;
        double RgainR = 1.0;
        switch (bitshiftL) {
            case 17: RgainL = 0.0; break;
            case 16: RgainL = 0.0000152587890625; break;
            case 15: RgainL = 0.000030517578125; break;
            case 14: RgainL = 0.00006103515625; break;
            case 13: RgainL = 0.0001220703125; break;
            case 12: RgainL = 0.000244140625; break;
            case 11: RgainL = 0.00048828125; break;
            case 10: RgainL = 0.0009765625; break;
            case 9: RgainL = 0.001953125; break;
            case 8: RgainL = 0.00390625; break;
            case 7: RgainL = 0.0078125; break;
            case 6: RgainL = 0.015625; break;
            case 5: RgainL = 0.03125; break;
            case 4: RgainL = 0.0625; break;
            case 3: RgainL = 0.125; break;
            case 2: RgainL = 0.25; break;
            case 1: RgainL = 0.5; break;
            case 0: RgainL = 1.0; break;
            case -1: RgainL = 2.0; break;
            case -2: RgainL = 4.0; break;
        }
        switch (bitshiftR) {
            case 17: RgainR = 0.0; break;
            case 16: RgainR = 0.0000152587890625; break;
            case 15: RgainR = 0.000030517578125; break;
            case 14: RgainR = 0.00006103515625; break;
            case 13: RgainR = 0.0001220703125; break;
            case 12: RgainR = 0.000244140625; break;
            case 11: RgainR = 0.00048828125; break;
            case 10: RgainR = 0.0009765625; break;
            case 9: RgainR = 0.001953125; break;
            case 8: RgainR = 0.00390625; break;
            case 7: RgainR = 0.0078125; break;
            case 6: RgainR = 0.015625; break;
            case 5: RgainR = 0.03125; break;
            case 4: RgainR = 0.0625; break;
            case 3: RgainR = 0.125; break;
            case 2: RgainR = 0.25; break;
            case 1: RgainR = 0.5; break;
            case 0: RgainR = 1.0; break;
            case -1: RgainR = 2.0; break;
            case -2: RgainR = 4.0; break;
        }
        while (--sampleFrames >= 0) {
            double monoL = *in1;
            double monoR = *in2;
            *out1 = (monoL * LgainL) + (monoR * RgainL);
            *out2 = (monoL * LgainR) + (monoR * RgainR);
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::bitdualpan
