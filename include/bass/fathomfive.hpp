#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::fathomfive {

constexpr std::string_view k_name{ "FathomFive" };
constexpr std::string_view k_short_description{
    "FathomFive is a way of supplementing extreme bass that’s not just EQ."
};
constexpr std::string_view k_long_description{
    "FathomFive introduces a special Airwindows algorithm that acts like a combination between an EQ and a bass amp. This plugin can be used in sound design or on isolated tracks, but it’s also part of an Airwindows DAW workflow I outline in the video I’ve made. The idea is this: rather than blur and damage your sound by running through lots of ‘fake analog’ effects on your buss to impart deepness and analog-like tone, use the Airwindows plugins Console4 and FathomFive to get a big-sounding mix while letting most mix elements through with minimal processing. You can run the ‘bass bloom’ behavior on an aux, and feed it with only the elements you want, and then integrate it into the sound with Console4 which both works the way Console wants to be used, and addresses limitations that Console has when used all by itself.It’s all in the rather long video, and of course you can simply download and use the plugin if you prefer making up your own rules. There are no rules, I’m only explaining one very specific use case where FathomFive works symbiotically with Console.(followup: this is a very old plugin. Newer ones that do this type of thing with increasing ease and friendliness are DubSub, DubCenter, and BassKit. FathomFive is wilder, and you probably want to keep an eye on whether it is spitting out excessive DC offsets, as the algorithm these plugins use for the bass is tricky to manage)"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class FathomFive final : public Effect<T>
{
    bool WasNegativeL;
    bool SubOctaveL;
    double iirSampleLA;
    double iirSampleLB;
    double iirSampleLC;
    double iirSampleLD;
    bool WasNegativeR;
    bool SubOctaveR;
    double iirSampleRA;
    double iirSampleRB;
    double iirSampleRC;
    double iirSampleRD;
    float A; // Root Note
    float B; // SubOctave
    float C; // Frequency
    float D; // Dry/Wet
    // parameters. Always 0-1, and we scale/alter them elsewhere.
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    FathomFive()
    {
        A = 1.0;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        WasNegativeL = false;
        SubOctaveL = false;
        iirSampleLA = 0.0;
        iirSampleLB = 0.0;
        iirSampleLC = 0.0;
        iirSampleLD = 0.0;
        WasNegativeR = false;
        SubOctaveR = false;
        iirSampleRA = 0.0;
        iirSampleRB = 0.0;
        iirSampleRC = 0.0;
        iirSampleRD = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "root note"; break;
            case kParamB: return "suboctave"; break;
            case kParamC: return "frequency"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Root Note"; break;
            case kParamB: return "SubOctave"; break;
            case kParamC: return "Frequency"; break;
            case kParamD: return "Dry/Wet"; break;

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

        double EQ = 0.01 + ((pow(C, 4) / Effect<T>::getSampleRate()) * 32000.0);
        double dcblock = EQ / 320.0;
        double wet = D * 2.0;
        double dry = 2.0 - wet;
        double bridgerectifier;
        double tempL;
        double tempR;
        double basstrim = (0.01 / EQ) + 1.0;
        if (wet > 1.0) {
            wet = 1.0;
        }
        if (dry > 1.0) {
            dry = 1.0;
        }
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (inputSampleL > 0)
            {
                if (WasNegativeL) {
                    SubOctaveL = !SubOctaveL;
                }
                WasNegativeL = false;
            }
            else {
                WasNegativeL = true;
            }
            if (inputSampleR > 0)
            {
                if (WasNegativeR) {
                    SubOctaveR = !SubOctaveR;
                }
                WasNegativeR = false;
            }
            else {
                WasNegativeR = true;
            }
            iirSampleLD = (iirSampleLD * (1 - EQ)) + (inputSampleL * EQ);
            bridgerectifier = fabs(iirSampleLD);
            if (SubOctaveL) {
                tempL = bridgerectifier * B;
            }
            else {
                tempL = -bridgerectifier * B;
            }
            iirSampleRD = (iirSampleRD * (1 - EQ)) + (inputSampleR * EQ);
            bridgerectifier = fabs(iirSampleRD);
            if (SubOctaveR) {
                tempR = bridgerectifier * B;
            }
            else {
                tempR = -bridgerectifier * B;
            }
            tempL += (inputSampleL * A);
            tempR += (inputSampleR * A);
            iirSampleLA += (tempL * EQ);
            iirSampleLA -= (iirSampleLA * iirSampleLA * iirSampleLA * EQ);
            if (iirSampleLA > 0) {
                iirSampleLA -= dcblock;
            }
            else {
                iirSampleLA += dcblock;
            }
            tempL = iirSampleLA * basstrim;
            iirSampleRA += (tempR * EQ);
            iirSampleRA -= (iirSampleRA * iirSampleRA * iirSampleRA * EQ);
            if (iirSampleRA > 0) {
                iirSampleRA -= dcblock;
            }
            else {
                iirSampleRA += dcblock;
            }
            tempR = iirSampleRA * basstrim;
            iirSampleLB = (iirSampleLB * (1 - EQ)) + (tempL * EQ);
            tempL = iirSampleLB;
            iirSampleRB = (iirSampleRB * (1 - EQ)) + (tempR * EQ);
            tempR = iirSampleRB;
            iirSampleLC = (iirSampleLC * (1 - EQ)) + (tempL * EQ);
            tempL = iirSampleLC;
            iirSampleRC = (iirSampleRC * (1 - EQ)) + (tempR * EQ);
            tempR = iirSampleRC;
            inputSampleL = (inputSampleL * dry) + (tempL * wet);
            inputSampleR = (inputSampleR * dry) + (tempR * wet);
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::fathomfive
