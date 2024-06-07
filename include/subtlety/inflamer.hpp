#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::inflamer {

constexpr std::string_view k_name{ "Inflamer" };
constexpr std::string_view k_short_description{
    "Inflamer is an Airwindowsized take on the waveshapers in Oxford Inflator."
};
constexpr std::string_view k_long_description{
    "How do you take a famous, beloved plugin and make it better?In a recent livestream, I saw a video make the very convincing argument that the famous Oxford Inflator is the combination of two very simple waveshapers. Like, spectacularly simple. One's a bit more complicated than the other, and one is essentially just second harmonic, nothing more. The person making the video set up the two simple waveshapers and demonstrated that for all manner of settings on the real Inflator, they could make a Melda waveshaper produce identical outputs.Of course, you didn't get the UI. You didn't get the band splitter. There's a clip meter of some sort: also not included. However this meter, these controls work: nope. But… now there are algorithms that are public, some of 'em so simple as to defy property (hard to argue that you can own one line of code that makes things second harmonic, it'd be like me claiming to own sin() )What could you do, to make this better in any way?Well, that's where my recent experiments come in. Meet Airwindows Inflamer (NOT Inflator). It doesn't do the same things, quite. There's no band splitter. There's a curve control, but it doesn't go -50 to +50, it goes from 0 to 1. And while it blends the two waveshapers… it does so in a more cumbersome and possibly more pure way.Inflamer is different because it's using my BitShiftGain style gain trimming, internally, as if it was a dry/wet, but with the two waveshapers instead of a dry and a wet. The Drive control is also bit shifts (very accurate divisions by powers of 2). The Effect control, as with the real Inflator, is normal and is in fact a dry/wet, and there you can have subtle adjustments to what is, I hope, a sonically optimized version of the effect.But you cannot have the band splitting, or fine gradations of Curve. In fact many settings of Curve will end up being a slight volume drop when used at unity gain (Drive in the middle). It is possible a bunch of people will shoot this out against the real, iLoked, for pay, bestseller plugin, and will decide mine is crap because it is often quieter than the real one.GOOD. I'm not trying to rain on anyone's parade. You have to know what you're doing to evaluate this. It's much like how, when I start doing takes on Bricasti, they'll be in my own style. I am not cloning things, even when the underlying algorithms are trivially simple. Inflamer is different and the range of adjustments are in 6 dB steps on Curve and Drive and often you might find the result comes into the mix 3 or more dB quieter than it would from the Sony plugin, and that's as it should be.And if I'm correct that leaning on these insights into digital math gives their own kind of benefits, I've managed to make an Inflamer which is more mastering-grade, more transparent and sonorous, and better sounding (IF you can live with the only settings I allow you to pick) than the real one. That's why I'm restricting it the way I am.There you have it. Inflamer is obviously not Oxford Inflator, has less options and restricted choices, and if you shoot them out head to head without matching levels carefully, it will probably always come out quieter than Oxford Inflator. And it is only some waveshapers, simplified and restricted even more than the original.And for some, it'll be just better, in critical listening. Sometimes it takes radical methods to beat an already stellar plugin. I hope you like Inflamer, and that it doesn't inflame you too much, unless you like that sort of thing."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Inflamer final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Inflamer()
    {
        A = 0.5;
        B = 0.5;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
        kParamB:
            return B;
            break;
        kParamC:
            return C;
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
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
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
            return "drive";
            break;
        kParamB:
            return "curve";
            break;
        kParamC:
            return "effect";
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
            return "Drive";
            break;
        kParamB:
            return "Curve";
            break;
        kParamC:
            return "Effect";
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
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
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
        kParamB:
            return "";
            break;
        kParamC:
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

        double gainControl = (A * 0.5) + 0.05; // 0.0 to 1.0
        int gainBits = 20; // start beyond maximum attenuation
        if (gainControl > 0.0) {
            gainBits = floor(1.0 / gainControl) - 2;
        }
        if (gainBits < -2) {
            gainBits = -2;
        }
        if (gainBits > 17) {
            gainBits = 17;
        }
        double gain = 1.0;
        switch (gainBits)
        {
            case 17: gain = 0.0; break;
            case 16: gain = 0.0000152587890625; break;
            case 15: gain = 0.000030517578125; break;
            case 14: gain = 0.00006103515625; break;
            case 13: gain = 0.0001220703125; break;
            case 12: gain = 0.000244140625; break;
            case 11: gain = 0.00048828125; break;
            case 10: gain = 0.0009765625; break;
            case 9: gain = 0.001953125; break;
            case 8: gain = 0.00390625; break;
            case 7: gain = 0.0078125; break;
            case 6: gain = 0.015625; break;
            case 5: gain = 0.03125; break;
            case 4: gain = 0.0625; break;
            case 3: gain = 0.125; break;
            case 2: gain = 0.25; break;
            case 1: gain = 0.5; break;
            case 0: gain = 1.0; break;
            case -1: gain = 2.0; break;
            case -2: gain = 4.0; break;
        } // now we have our input trim
        int bitshiftL = 1;
        int bitshiftR = 1;
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
        double negacurve = 0.5;
        double curve = 0.5;
        switch (bitshiftL)
        {
            case 17: negacurve = 0.0; break;
            case 16: negacurve = 0.0000152587890625; break;
            case 15: negacurve = 0.000030517578125; break;
            case 14: negacurve = 0.00006103515625; break;
            case 13: negacurve = 0.0001220703125; break;
            case 12: negacurve = 0.000244140625; break;
            case 11: negacurve = 0.00048828125; break;
            case 10: negacurve = 0.0009765625; break;
            case 9: negacurve = 0.001953125; break;
            case 8: negacurve = 0.00390625; break;
            case 7: negacurve = 0.0078125; break;
            case 6: negacurve = 0.015625; break;
            case 5: negacurve = 0.03125; break;
            case 4: negacurve = 0.0625; break;
            case 3: negacurve = 0.125; break;
            case 2: negacurve = 0.25; break;
            case 1: negacurve = 0.5; break;
            case 0: negacurve = 1.0; break;
            case -1: negacurve = 2.0; break;
            case -2: negacurve = 4.0; break;
        }
        switch (bitshiftR)
        {
            case 17: curve = 0.0; break;
            case 16: curve = 0.0000152587890625; break;
            case 15: curve = 0.000030517578125; break;
            case 14: curve = 0.00006103515625; break;
            case 13: curve = 0.0001220703125; break;
            case 12: curve = 0.000244140625; break;
            case 11: curve = 0.00048828125; break;
            case 10: curve = 0.0009765625; break;
            case 9: curve = 0.001953125; break;
            case 8: curve = 0.00390625; break;
            case 7: curve = 0.0078125; break;
            case 6: curve = 0.015625; break;
            case 5: curve = 0.03125; break;
            case 4: curve = 0.0625; break;
            case 3: curve = 0.125; break;
            case 2: curve = 0.25; break;
            case 1: curve = 0.5; break;
            case 0: curve = 1.0; break;
            case -1: curve = 2.0; break;
            case -2: curve = 4.0; break;
        }
        double effectOut = C;
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
            inputSampleL *= gain; // input trim
            double inflamerPlus = (inputSampleL * 2.0) - pow(inputSampleL, 2); //+50, very much just second harmonic
            double inflamerMinus = inputSampleL + (pow(inputSampleL, 3) * 0.25) - ((pow(inputSampleL, 2) + pow(inputSampleL, 4)) * 0.0625); //-50
            inputSampleL = (inflamerPlus * curve) + (inflamerMinus * negacurve);
            inputSampleL = (inputSampleL * effectOut) + (drySampleL * (1.0 - effectOut));
            inputSampleR *= gain; // input trim
            inflamerPlus = (inputSampleR * 2.0) - pow(inputSampleR, 2); //+50, very much just second harmonic
            inflamerMinus = inputSampleR + (pow(inputSampleR, 3) * 0.25) - ((pow(inputSampleR, 2) + pow(inputSampleR, 4)) * 0.0625); //-50
            inputSampleR = (inflamerPlus * curve) + (inflamerMinus * negacurve);
            inputSampleR = (inputSampleR * effectOut) + (drySampleR * (1.0 - effectOut));
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
} // namespace airwindohhs::inflamer
