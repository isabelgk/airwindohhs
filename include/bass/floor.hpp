#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::flooreffect {

constexpr std::string_view k_name{ "Floor" };
constexpr std::string_view k_short_description{
    "Floor is fake bottom octave for fun and profit!"
};
constexpr std::string_view k_long_description{
    "So this is one of those weird ‘considered harmful’ plugins… the ‘Don’t do this’ plugins. I don’t really advocate for this one as I don’t entirely approve: it’s kind of like some of the loudenators in that respect, and indeed it has similar characteristics.Floor does an odd thing that’s like trying to synthesize fake harmonics related to the real bass content, to make you think there’s a lower octave there when there isn’t. It might not be the most perfect implementation of this (I understand there’s a Waves plugin that does this type of processing and I think I must have modeled it on that) but it’s the Airwindows take on reverse-engineering that type of processing, while knowing nothing but the desired effect and the general category of what’s happening.This means it’s now part of the open source toolkit and can find its way into other stuff: here’s hoping real bass continues to be a thing (honestly, so much of what I do with Airwindows serves to improve linearity in the tiny micro-modulations that help us hear extended bass as a satisfying, resonant thing) even with an expanded toolkit around these frequencies. A lot of my recent work around DubSub and BassKit has been about introducing extended bass frequencies in a desirable way. I could’ve tacked the Floor algorithm onto there, and I decided that wasn’t good to do.Why would you want to do fake bass?Because you can get more loudness out of it. (also, maybe you’re just doing something interesting with tonalities, or exploiting the algorithm to make a different sound…) Mostly, it’s just about making it seem like you can go louder with the same content. It’s not really the same, it’s altered, but it’s simulating/faking the effect of an extended bottom octave and restricting the ‘swing’ of those frequencies so they cover the smaller range taken up by a higher frequency, because they’re really NOT the extended frequencies anymore, just some rearranged energy trying to pretend it’s deep bass. (I’m not sure how Floor will work as a DC blocker for RawConsole5 fans: seems like it might have undesirable effects? How do you even fake DC energy?)"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class Floor final : public Effect<T>
{
    bool flip; // end defining of antialiasing variables
    double iirSample1AL;
    double iirSample1BL;
    double iirSample1CL;
    double iirSample1DL;
    double iirSample1EL;
    double iirSample2AL;
    double iirSample2BL;
    double iirSample2CL;
    double iirSample2DL;
    double iirSample2EL;
    double iirSample1AR;
    double iirSample1BR;
    double iirSample1CR;
    double iirSample1DR;
    double iirSample1ER;
    double iirSample2AR;
    double iirSample2BR;
    double iirSample2CR;
    double iirSample2DR;
    double iirSample2ER;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Floor()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        flip = false;
        iirSample1AL = 0.0;
        iirSample1BL = 0.0;
        iirSample1CL = 0.0;
        iirSample1DL = 0.0;
        iirSample1EL = 0.0;
        iirSample2AL = 0.0;
        iirSample2BL = 0.0;
        iirSample2CL = 0.0;
        iirSample2DL = 0.0;
        iirSample2EL = 0.0;
        iirSample1AR = 0.0;
        iirSample1BR = 0.0;
        iirSample1CR = 0.0;
        iirSample1DR = 0.0;
        iirSample1ER = 0.0;
        iirSample2AR = 0.0;
        iirSample2BR = 0.0;
        iirSample2CR = 0.0;
        iirSample2DR = 0.0;
        iirSample2ER = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "floor"; break;
            case kParamB: return "drive"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Floor"; break;
            case kParamB: return "Drive"; break;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double setting = pow(A, 2);
        double iirAmount = (setting / 4.0) / overallscale;
        double tight = -1.0;
        double gaintrim = 1.0 + (setting / 4.0);
        double offset;
        double lows;
        double density = B;
        double bridgerectifier;
        double temp;
        iirAmount += (iirAmount * tight * tight);
        tight /= 3.0;
        if (iirAmount <= 0.0) {
            iirAmount = 0.0;
        }
        if (iirAmount > 1.0) {
            iirAmount = 1.0;
        }
        double wet = C;
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
            // begin left channel
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1AL = (iirSample1AL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
            lows = iirSample1AL;
            inputSampleL -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleL += lows;
            inputSampleL *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1BL = (iirSample1BL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
            lows = iirSample1BL;
            inputSampleL -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleL += lows;
            inputSampleL *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1CL = (iirSample1CL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
            lows = iirSample1CL;
            inputSampleL -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleL += lows;
            inputSampleL *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1DL = (iirSample1DL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
            lows = iirSample1DL;
            inputSampleL -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleL += lows;
            inputSampleL *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1EL = (iirSample1EL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
            lows = iirSample1EL;
            inputSampleL -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleL += lows;
            inputSampleL *= gaintrim;
            // end left channel
            // begin right channel
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1AR = (iirSample1AR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
            lows = iirSample1AR;
            inputSampleR -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleR += lows;
            inputSampleR *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1BR = (iirSample1BR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
            lows = iirSample1BR;
            inputSampleR -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleR += lows;
            inputSampleR *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1CR = (iirSample1CR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
            lows = iirSample1CR;
            inputSampleR -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleR += lows;
            inputSampleR *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1DR = (iirSample1DR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
            lows = iirSample1DR;
            inputSampleR -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleR += lows;
            inputSampleR *= gaintrim;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            iirSample1ER = (iirSample1ER * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
            lows = iirSample1ER;
            inputSampleR -= lows;
            temp = lows;
            if (lows < 0) {
                lows = -sin(-lows * 1.5707963267949);
            }
            if (lows > 0) {
                lows = sin(lows * 1.5707963267949);
            }
            lows -= temp;
            inputSampleR += lows;
            inputSampleR *= gaintrim;
            // end right channel
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            bridgerectifier = fabs(inputSampleL) * 1.57079633;
            bridgerectifier = sin(bridgerectifier) * 1.57079633;
            bridgerectifier = (fabs(inputSampleL) * (1 - density)) + (bridgerectifier * density);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleL = (inputSampleL * (1 - density)) - (bridgerectifier * density);
            }
            // drive section, left
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            bridgerectifier = sin(bridgerectifier) * 1.57079633;
            bridgerectifier = (fabs(inputSampleR) * (1 - density)) + (bridgerectifier * density);
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleR = (inputSampleR * (1 - density)) - (bridgerectifier * density);
            }
            // drive section, right
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs::flooreffect
