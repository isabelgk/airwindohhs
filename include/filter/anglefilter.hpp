#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::anglefilter {

constexpr std::string_view k_name{ "AngleFilter" };
constexpr std::string_view k_short_description{
    "AngleFilter is the synth-style extension of AngleEQ."
};
constexpr std::string_view k_long_description{
    "So while I do stuff like make new forms of reverb (going from 5x5 matrices to 6x6!) and try to invent genres of music, there's this funny little filter…AngleFilter is an offshoot of AngleEQ, which was too strange by itself but ended up turning into the EQs used in PointyGuitar and ChimeyGuitar. This is probably why those can get weird when you set the controls too strangely, and AngleFilter gets even weirder. It was meant to be a nonresonant filter, just a very steep brickwall type thing, but instead it does crazy things with phase around the cutoff, and grows steeper and more intense the more you lower that cutoff.Since it was so untameable I just put a full-on waveshaper on the output, so its excesses won't blow up to huge dB spikes. There's a Hard control, and what it does is it makes life hard for you in setting the other controls. Mostly it goes insane over lower bassy settings, but it can be set to produce a dull roar at higher frequencies too, and the whole design of the plugin is for letting you modulate the cutoff hyper-aggressively without problems.I'm working on things like very serious reverb upgrades, but sometimes you just gotta have fun too :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class AngleFilter final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double primaryA, primaryB;
    double secondaryA, secondaryB;
    double tertiaryA, tertiaryB;
    double iirPositionL[37];
    double iirAngleL[37];
    double iirPositionR[37];
    double iirAngleR[37];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    AngleFilter()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        D = 0.5;
        for (int count = 0; count < 36; count++) {
            iirPositionL[count] = 0.0;
            iirAngleL[count] = 0.0;
            iirPositionR[count] = 0.0;
            iirAngleR[count] = 0.0;
        }
        primaryA = primaryB = 0.0;
        secondaryA = secondaryB = 0.0;
        tertiaryA = tertiaryB = 0.0;
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
            case kParamB: return 0.5; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "freq"; break;
            case kParamB: return "hard"; break;
            case kParamC: return "reso"; break;
            case kParamD: return "poles"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Freq"; break;
            case kParamB: return "Hard"; break;
            case kParamC: return "Reso"; break;
            case kParamD: return "Poles"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        primaryA = primaryB;
        primaryB = pow(A, overallscale + 2.0);
        secondaryA = secondaryB;
        secondaryB = B;
        tertiaryA = tertiaryB;
        tertiaryB = fmin(primaryB + C, 1.0);
        double poles = D;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double temp = (double)sampleFrames / inFramesToProcess;
            double primary = (primaryA * temp) + (primaryB * (1.0 - temp));
            double secondary = (secondaryA * temp) + (secondaryB * (1.0 - temp));
            double tertiary = (tertiaryA * temp) + (tertiaryB * (1.0 - temp));
            for (int count = 0; count < poles * 32; count++) {
                double FT = fmax(primary + (fabs(inputSampleL) * (secondary - 0.5)), 0.0);
                double FR = (FT * (1.0 - tertiary)) + tertiary;
                iirAngleL[count] = (iirAngleL[count] * (1.0 - FT)) + ((inputSampleL - iirPositionL[count]) * FT);
                inputSampleL = ((iirPositionL[count] + (iirAngleL[count] * FT * FR)) * (1.0 - FT)) + (inputSampleL * FT);
                iirPositionL[count] = ((iirPositionL[count] + (iirAngleL[count] * FT * FR)) * (1.0 - FT)) + (inputSampleL * FT);
                FT = fmax(primary + (fabs(inputSampleR) * (secondary - 0.5)), 0.0);
                FR = (FT * (1.0 - tertiary)) + tertiary;
                iirAngleR[count] = (iirAngleR[count] * (1.0 - FT)) + ((inputSampleR - iirPositionR[count]) * FT);
                inputSampleR = ((iirPositionR[count] + (iirAngleR[count] * FT * FR)) * (1.0 - FT)) + (inputSampleR * FT);
                iirPositionR[count] = ((iirPositionR[count] + (iirAngleR[count] * FT * FR)) * (1.0 - FT)) + (inputSampleR * FT);
            }
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR); // straight up just wavefold it
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
} // namespace airwindohhs::anglefilter
