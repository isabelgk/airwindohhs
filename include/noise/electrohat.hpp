#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::electrohat {

constexpr std::string_view k_name{ "ElectroHat" };
constexpr std::string_view k_short_description{
    "ElectroHat is a hi-hat tone generator that uses the original sound as a control voltage."
};
constexpr std::string_view k_long_description{
    "I’ve always liked this one. ElectroHat uses primitive residue sequences to produce a ‘noise’ like effect that makes the hi-hat, but since it’s such a crude method of generating randomness, you get artifacts and peculiar electronic noises instead of nice pure noise. Peculiar electronic noises turn out to be a lot of fun as hi-hats!You use this by feeding some sort of control voltage to it. It responds very, very quickly, so if there’s any amplitude modulation as part of your wave, you’ll hear it affecting the hat. You can use that on purpose, you can use a real DC control voltage to drive it, or you can simply make the envelope you want using a square wave tone for the underlying signal: it’ll rectify the squarewave to be only positive, and that’ll end up the same as a control voltage."
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class ElectroHat final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedSampleL;
    double storedSampleR;
    double lastSampleL;
    double lastSampleR;
    int tik;
    int lok;
    bool flip;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    ElectroHat()
    {
        A = 0.0;
        B = 0.5;
        C = 1.0;
        D = 0.1;
        E = 1.0;
        storedSampleL = 0.0;
        storedSampleR = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        tik = 3746926;
        lok = 0;
        flip = true;
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
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

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
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.1; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "hihat"; break;
            case kParamB: return "trim"; break;
            case kParamC: return "bright"; break;
            case kParamD: return "output"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "HiHat"; break;
            case kParamB: return "Trim"; break;
            case kParamC: return "Bright"; break;
            case kParamD: return "Output"; break;
            case kParamE: return "Dry/Wet"; break;

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
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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
        bool highSample = false;
        if (Effect<T>::getSampleRate() > 64000) {
            highSample = true;
        }
        // we will go to another dither for 88 and 96K
        double drySampleL;
        double drySampleR;
        double tempSampleL;
        double tempSampleR;
        double inputSampleL;
        double inputSampleR;
        int deSyn = (VstInt32)(A * 5.999) + 1;
        double increment = B;
        double brighten = C;
        double outputlevel = D;
        double wet = E;
        if (deSyn == 4) {
            deSyn = 1;
            increment = 0.411;
            brighten = 0.87;
        }
        // 606 preset
        if (deSyn == 5) {
            deSyn = 2;
            increment = 0.111;
            brighten = 1.0;
        }
        // 808 preset
        if (deSyn == 6) {
            deSyn = 2;
            increment = 0.299;
            brighten = 0.359;
        }
        // 909 preset
        int tok = deSyn + 1;
        increment *= 0.98;
        increment += 0.01;
        increment += (double)tok;
        double fosA = increment;
        double fosB = fosA * increment;
        double fosC = fosB * increment;
        double fosD = fosC * increment;
        double fosE = fosD * increment;
        double fosF = fosE * increment;
        int posA = fosA;
        int posB = fosB;
        int posC = fosC;
        int posD = fosD;
        int posE = fosE;
        int posF = fosF;
        int posG = posF * posE * posD * posC * posB; // factorial
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL = fabs(inputSampleL) * outputlevel;
            inputSampleR = fabs(inputSampleR) * outputlevel;
            if (flip) { // will always be true unless we have high sample rate
                tik++;
                tik = tik % posG;
                tok = tik * tik;
                tok = tok % posF;
                tok *= tok;
                tok = tok % posE;
                tok *= tok;
                tok = tok % posD;
                tok *= tok;
                tok = tok % posC;
                tok *= tok;
                tok = tok % posB;
                tok *= tok;
                tok = tok % posA;
                inputSampleL = tok * inputSampleL;
                if ((abs(lok - tok) < abs(lok + tok)) && (deSyn == 1)) {
                    inputSampleL = -tok * inputSampleL;
                }
                if ((abs(lok - tok) > abs(lok + tok)) && (deSyn == 2)) {
                    inputSampleL = -tok * inputSampleL;
                }
                if ((abs(lok - tok) < abs(lok + tok)) && (deSyn == 3)) {
                    inputSampleL = -tok * inputSampleL;
                }
                inputSampleR = tok * inputSampleR;
                if ((abs(lok - tok) < abs(lok + tok)) && (deSyn == 1)) {
                    inputSampleR = -tok * inputSampleR;
                }
                if ((abs(lok - tok) > abs(lok + tok)) && (deSyn == 2)) {
                    inputSampleR = -tok * inputSampleR;
                }
                if ((abs(lok - tok) < abs(lok + tok)) && (deSyn == 3)) {
                    inputSampleR = -tok * inputSampleR;
                }
                lok = tok;
                tempSampleL = inputSampleL;
                inputSampleL = inputSampleL - (lastSampleL * brighten);
                lastSampleL = tempSampleL;
                tempSampleR = inputSampleR;
                inputSampleR = inputSampleR - (lastSampleR * brighten);
                lastSampleR = tempSampleR;
            }
            else { // we have high sample rate and this is an interpolate sample
                inputSampleL = lastSampleL;
                inputSampleR = lastSampleR;
                // not really interpolating, just sample-and-hold
            }
            if (highSample) {
                flip = !flip;
            }
            else {
                flip = true;
            }
            tempSampleL = inputSampleL;
            inputSampleL += storedSampleL;
            storedSampleL = tempSampleL;
            tempSampleR = inputSampleR;
            inputSampleR += storedSampleR;
            storedSampleR = tempSampleR;
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
} // namespace airwindohhs::electrohat
