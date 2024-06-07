#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::brightambience2 {

constexpr std::string_view k_name{ "BrightAmbience2" };
constexpr std::string_view k_short_description{
    "BrightAmbience2 is more BrightAmbience with better tone and more slapbacky effects."
};
constexpr std::string_view k_long_description{
    "This is just like BrightAmbience, except different in pretty much every detail. Techwise (skip to next paragraph if you like) it is using a totally different prime number series, ‘super-primes’ for its delay spacing, and then it’s offsetting alternately left and right delay taps to the next prime number in line, meaning that it’s a dual mono ‘ambience’ but center signals WILL get stereo spread. Also, there’s a kind of regeneration that was real tricky to do, but it means you can get a clean digital slapback, or any degree of ‘fuzzed out’ bright ambient slapback all the way to the wash of sparkly atmosphere, either subtly regenerated or cleanly gated.Okay, but what does it do, and what does it sound like?Super ultra bright ambience space that doesn’t get in the way. You can put it on things like drums for 80s gated verb, you can do dub-like things through using it to make a blurry slapback, but this thing is bonkers at putting Star Quality Vocal Glitter on voices. You don’t have to have it loud, and you don’t have to stretch it out so long that it feels like a reverb. That’s not the point, this is about doing that classic Lexicon thing (without, I might add, using ANY actual Lexicon sounds or algorithms) where you can fill in a bright, glossy atmosphere around the voice that makes it sound like star quality. You probably don’t want to treat it like a reverb, on a send or whatever (maybe on a vocal bus? It’ll handle split harmony vocals very elegantly since it’s dual-mono). Instead you want to use it like your lead vocals alone go into a special chamber. Might also be an inspiring thing to monitor while tracking: I sure had fun playing with it in my headphones, and if it’s on your mix while tracking you might not have to print it on the actual vocal track. Everything about it is evolved from BrightAmbience, even the algorithm that makes it.Oh, also if you’re a coder and want to get your hands on the delay taps, it took hours of looking up and typing in specific prime numbers, as there is no such thing as a ‘list of super-primes except every other one uses the next real prime after the super-prime, making the list pan every little echo to alternate sides using inter-aural delays’. And maybe you never even thought of such a thing. But if you think you can make use of such a thing, in the .h file (for the AU, anyway) is a definition of ‘primeL[]’ and ‘primeR[]’ that you can simply copy and paste. It’s 489 total entries which will get you a half-second or more out from the dry signal, even at 96k, and you just use ‘primeL[]’ and ‘primeR[]’ to specify the delay taps you want, typically in a range (like, entries 40 to 60 will give you a little ambient blur starting at whatever ‘primeL[40]’ is, which is 1031 samples)It’s MIT license so you only have to shout me out and you can do anything you want with it… so don’t say I never gave you nothin’ :D"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class BrightAmbience2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    int gcount;
    float pL[32768];
    float pR[32768];
    double feedbackA;
    double feedbackB;
    double feedbackC;
    float A;
    float B;
    float C;
    float D;

  public:
    BrightAmbience2()
    {
        for (int count = 0; count < 32767; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        feedbackA = feedbackB = feedbackC = 0.0;
        gcount = 0;
        A = 0.2;
        B = 0.2;
        C = 0.0;
        D = 0.5;
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
        kParamD = 3,
        kNumParameters = 4
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
        kParamD:
            D = value;
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
        kParamD:
            return D;
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
            return 0.0;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.0;
            break;
        kParamD:
            return 0.5;
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
            return "start";
            break;
        kParamB:
            return "length";
            break;
        kParamC:
            return "feedbck";
            break;
        kParamD:
            return "dry/wet";
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
            return "Start";
            break;
        kParamB:
            return "Length";
            break;
        kParamC:
            return "Feedbck";
            break;
        kParamD:
            return "Dry/Wet";
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
        kParamD:
            return std::to_string(D);
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
        kParamD:
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

        int start = (int)(pow(A, 2) * 480) + 1;
        int length = (int)(pow(B, 2) * 480) + 1;
        if (start + length > 488) {
            start = 488 - length;
        }
        double feedbackAmount = 1.0 - (pow(1.0 - C, 2));
        double wet = D;
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
            double tempL = 0.0;
            double tempR = 0.0;
            if (gcount < 0 || gcount > 32767) {
                gcount = 32767;
            }
            int count = gcount;
            pL[count] = inputSampleL + ((sin(feedbackA) / sqrt(length + 1)) * feedbackAmount);
            pR[count] = inputSampleR + ((sin(feedbackB) / sqrt(length + 1)) * feedbackAmount);
            for (int offset = start; offset < start + length; offset++) {
                tempL += pL[count + primeL[offset] - ((count + primeL[offset] > 32767) ? 32768 : 0)];
                tempR += pR[count + primeR[offset] - ((count + primeR[offset] > 32767) ? 32768 : 0)];
            }
            inputSampleL = tempL / sqrt(length);
            inputSampleR = tempR / sqrt(length);
            feedbackA = (feedbackA * (1.0 - feedbackAmount)) + (inputSampleR * feedbackAmount);
            feedbackB = (feedbackB * (1.0 - feedbackAmount)) + (inputSampleL * feedbackAmount);
            gcount--;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::brightambience2
