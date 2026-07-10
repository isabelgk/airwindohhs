#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::donut {

constexpr std::string_view k_name{ "Donut" };
constexpr std::string_view k_short_description{
    "Donut is in memory of Dilla."
};
constexpr std::string_view k_long_description{
    "The main reason for this plugin is, one of my livestream people has wanted it for a long time. I'm hoping this brings him joy?Thing is, I've made a plugin in the SPIRIT of what I hear in Dilla, not a clone of a particular effect he may or may not have used on various things. And so, I believe this can be set up to do numerous things you've heard Dilla do… but also, I took pains to help it do other stuff I heard, and it kinda went on from there.This is basically an envelope follower. It's like my newest compressor plugin (as in, not out by itself yet but the one that's in ConsoleH and ConsoleX2), but instead of modulating volume, it modulates a lowpass filter. Except it's not just 'a lowpass filter', it's me trying out a very popular version of those filters known as a state variable filter, and doing some experiments with it which I don't know how common they are, but I loved the sound I got when I did them. The filter will come out by itself too, it's called Dattorro after the guy who wrote it into a textbook, and it's going to do interesting things like help me make a formant filter, later.But the way it works NOW is, you've got the compressor (a Bezier-filter compressor, so it doesn't have attack speed artifacts) with an attack speed that basically has it act like the original BeziComp, and then a release speed that lets you slow it right down. If you want to hear where it's at, you can turn the resonance up until it's basically just a wavering note.Then, there's a filter cutoff, a resonance control, and two more sliders just below those which modulate them according to the envelope, and that's it.If you put the filter in the middle, you can see that the modulator is at zero when in the center. So you can subtract the envelope, OR add it. Same with the resonance. My hope is that this can get you some neat effects, perhaps using Dilla-esque techniques like splitting the tracks and applying this real aggressively to only certain parts. In no way is it a 'push butan for dilla FX', my hope is that it's flexible enough to do a wide range of things with. Remember, if you use a very slow attack and fast release it's more of a BeziComp response, very wiggly.Part of what I found was that Dilla, I think, liked to run stuff into his Moog to process. Donut can do some of that: sweep it low and give it the right amount of resonance and it ought to resynthesize bass real well. It should handle automation nicely: state variable filters are better at that than, for instance, biquads. This is not the steepest of filters, but I set it up to have tone while reshaping things.I hope you like Donut! I have many things to put out but I can put out Dattorro too, if you like. That one comes with debugged versions of the original SVF code (which didn't work perfectly right out of the box, but has more options than I use here)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Donut final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    enum
    {
        bez_A,
        bez_B,
        bez_C,
        bez_Ctrl,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezCompL[bez_total];
    double bezCompR[bez_total];
    double bezMinL, bezMinR;
    double lowL, lowR;
    double bandL, bandR;
    double freqA, freqB;
    double resoA, resoB;
    double outA, outB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Donut()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        D = 0.5;
        E = 0.0;
        F = 0.5;
        G = 1.0;
        for (int x = 0; x < bez_total; x++) {
            bezCompL[x] = 0.0;
            bezCompR[x] = 0.0;
        }
        bezCompL[bez_cycle] = 1.0;
        bezCompR[bez_cycle] = 1.0;
        bezMinL = 0.0;
        bezMinR = 0.0;
        lowL = bandL = lowR = bandR = 0.0;
        freqA = freqB = 0.5;
        resoA = resoB = 0.5;
        outA = outB = 1.0;
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
        kParamF = 5,
        kParamG = 6,
        kNumParameters = 7
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;

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
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.0; break;
            case kParamF: return 0.5; break;
            case kParamG: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "attack"; break;
            case kParamB: return "release"; break;
            case kParamC: return "basefrq"; break;
            case kParamD: return "movefrq"; break;
            case kParamE: return "baseres"; break;
            case kParamF: return "moveres"; break;
            case kParamG: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Attack"; break;
            case kParamB: return "Release"; break;
            case kParamC: return "BaseFrq"; break;
            case kParamD: return "MoveFrq"; break;
            case kParamE: return "BaseRes"; break;
            case kParamF: return "MoveRes"; break;
            case kParamG: return "Output"; break;

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
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;

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
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;

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
        double bezRez = pow(1.0 - A, 6.0) / overallscale;
        double sloRez = pow(1.0 - B, 6.0) / overallscale;
        bezRez = fmin(fmax(bezRez, 0.00001), 1.0);
        sloRez = fmin(fmax(sloRez, 0.00001), 1.0);
        freqA = freqB;
        resoA = resoB;
        outA = outB;
        freqB = pow(C, overallscale + 1.0) * 1.225;
        double movFreq = (D * 2.0) - 1.0;
        resoB = pow(1.0 - E, 2.0);
        if (resoB < 0.001) {
            resoB = 0.001; // q of 0.0 is just a tone
        }
        double movReso = (F * -2.0) + 1.0;
        outB = G / sqrt(resoB);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            const double temp = (double)sampleFrames / inFramesToProcess;
            const double freq = (freqA * temp) + (freqB * (1.0 - temp));
            const double reso = (resoA * temp) + (resoB * (1.0 - temp));
            const double out = (outA * temp) + (outB * (1.0 - temp)); // dezippering
            double ctrl = fabs(inputSampleL);
            bezMinL = fmax(bezMinL - sloRez, ctrl);
            bezCompL[bez_cycle] += bezRez;
            bezCompL[bez_Ctrl] += (bezMinL * bezRez);
            if (bezCompL[bez_cycle] > 1.0) {
                bezCompL[bez_cycle] -= 1.0;
                bezCompL[bez_C] = bezCompL[bez_B];
                bezCompL[bez_B] = bezCompL[bez_A];
                bezCompL[bez_A] = bezCompL[bez_Ctrl];
                bezCompL[bez_Ctrl] = 0.0;
            }
            double CB = (bezCompL[bez_C] * (1.0 - bezCompL[bez_cycle])) + (bezCompL[bez_B] * bezCompL[bez_cycle]);
            double BA = (bezCompL[bez_B] * (1.0 - bezCompL[bez_cycle])) + (bezCompL[bez_A] * bezCompL[bez_cycle]);
            double CBA = (bezCompL[bez_B] + (CB * (1.0 - bezCompL[bez_cycle])) + (BA * bezCompL[bez_cycle])) * 0.5;
            double mFreq = fmin(fmax(freq + (CBA * movFreq), 0.004 / overallscale), 1.225);
            double mReso = fmin(fmax(reso + (CBA * movReso), 0.001), 1.0);
            lowL += mFreq * bandL;
            bandL += mFreq * ((mReso * inputSampleL) - lowL - (mReso * bandL));
            inputSampleL = (lowL - sin(bandL * 0.5)) * out; // airwin-donut
            ctrl = fabs(inputSampleR);
            bezMinR = fmax(bezMinR - sloRez, ctrl);
            bezCompR[bez_cycle] += bezRez;
            bezCompR[bez_Ctrl] += (bezMinR * bezRez);
            if (bezCompR[bez_cycle] > 1.0) {
                bezCompR[bez_cycle] -= 1.0;
                bezCompR[bez_C] = bezCompR[bez_B];
                bezCompR[bez_B] = bezCompR[bez_A];
                bezCompR[bez_A] = bezCompR[bez_Ctrl];
                bezCompR[bez_Ctrl] = 0.0;
            }
            CB = (bezCompR[bez_C] * (1.0 - bezCompR[bez_cycle])) + (bezCompR[bez_B] * bezCompR[bez_cycle]);
            BA = (bezCompR[bez_B] * (1.0 - bezCompR[bez_cycle])) + (bezCompR[bez_A] * bezCompR[bez_cycle]);
            CBA = (bezCompR[bez_B] + (CB * (1.0 - bezCompR[bez_cycle])) + (BA * bezCompR[bez_cycle])) * 0.5;
            mFreq = fmin(fmax(freq + (CBA * movFreq), 0.004 / overallscale), 1.225);
            mReso = fmin(fmax(reso + (CBA * movReso), 0.001), 1.0);
            lowR += mFreq * bandR;
            bandR += mFreq * ((mReso * inputSampleR) - lowR - (mReso * bandR));
            inputSampleR = (lowR - sin(bandR * 0.5)) * out; // airwin-donut
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
} // namespace airwindohhs::donut
