#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pop2 {

constexpr std::string_view k_name{ "Pop2" };
constexpr std::string_view k_short_description{
    "Pop2 adds control and punch to Pop."
};
constexpr std::string_view k_long_description{
    "Back in the day, I said I would get an Allen and Heath compressor to do Pop2. That is not what happened here, and Pop2 is not a hardware emulation (even though I'm demonstrating it against a Heritage Audio Successor using a sidechain filter on the hardware comp that Pop2 doesn't even have)What happened is this: Pop was the best testbed for some experiments I needed to do. I needed to split out attack and decay speeds (as much as I could, on this crazy algorithm!) and I needed to build ClipOnly2 into the output stage.This is because there's a trick you can do if you know how. If you compress in such a way that a huge spikey attack pokes out, and then you clip that attack, you can get and control a sort of distorted 'splat' on the attack that highlights it usefully. (I didn't make this up: Paul Frindle talked about it publically as a useful thing to do)The result of these experiments is Pop2, a software compressor so intense that you have to shoot it out with a rackmount compressor which will set you back $1700… and even then, it's not a given which wins. The hardware will give you more midrange sonority (that Successor is fully transformer balanced and a really nice piece of kit) and hangs on to decays in a characteristic way… but Pop2 stands up for itself damn well, plus you can run one on all 16 inserts of your hybrid analog/digital recording setup, plus there's a bit of a price difference.On a personal note I apologize for the delay and lack of plugin last week: I was at a hospital but not over myself or my lady, in this case it's my best friend out here in Vermont. Suffice to say I am studio building like a madman because it is my way of coping with grief and loss, and I'll rest at times too, but he would appreciate me turning to my lifework at such a time, as he's been an inspiration to me in turn: and that's enough of that for now. Just know: maybe sometimes go for your dreams and don't wait, because you never know when your life or those near you will be just taken away.I am proud to still be here helping you go for YOUR dreams, and hope that Pop2 helps you do that. Please help support my Patreon and I'll continue to charge forth doing everything I can, and it also helps me be there for my friends and loved ones when I need to drop everything and drive or fly to somebody's side (which we will hope does not become a constant refrain!)Love you folks, talk to you later :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pop2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double muVaryL;
    double muAttackL;
    double muNewSpeedL;
    double muSpeedAL;
    double muSpeedBL;
    double muCoefficientAL;
    double muCoefficientBL;
    double muVaryR;
    double muAttackR;
    double muNewSpeedR;
    double muSpeedAR;
    double muSpeedBR;
    double muCoefficientAR;
    double muCoefficientBR;
    bool flip;
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Pop2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
        muVaryL = 0.0;
        muAttackL = 0.0;
        muNewSpeedL = 1000.0;
        muSpeedAL = 1000.0;
        muSpeedBL = 1000.0;
        muCoefficientAL = 1.0;
        muCoefficientBL = 1.0;
        muVaryR = 0.0;
        muAttackR = 0.0;
        muNewSpeedR = 1000.0;
        muSpeedAR = 1000.0;
        muSpeedBR = 1000.0;
        muCoefficientAR = 1.0;
        muCoefficientBR = 1.0;
        flip = false;
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
        kParamE:
            E = value;
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
        kParamE:
            return E;
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
            return 0.5;
            break;
        kParamD:
            return 0.5;
            break;
        kParamE:
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
            return "compres";
            break;
        kParamB:
            return "attack";
            break;
        kParamC:
            return "release";
            break;
        kParamD:
            return "drive";
            break;
        kParamE:
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
            return "Compres";
            break;
        kParamB:
            return "Attack";
            break;
        kParamC:
            return "Release";
            break;
        kParamD:
            return "Drive";
            break;
        kParamE:
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
        kParamE:
            return std::to_string(E);
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
        kParamE:
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double threshold = 1.0 - ((1.0 - pow(1.0 - A, 2)) * 0.9);
        double attack = ((pow(B, 4) * 100000.0) + 10.0) * overallscale;
        double release = ((pow(C, 5) * 2000000.0) + 20.0) * overallscale;
        double maxRelease = release * 4.0;
        double muPreGain = 1.0 / threshold;
        double muMakeupGain = sqrt(1.0 / threshold) * D;
        double wet = E;
        // compressor section
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
            // begin compressor section
            inputSampleL *= muPreGain;
            inputSampleR *= muPreGain;
            // adjust coefficients for L
            if (flip) {
                if (fabs(inputSampleL) > threshold) {
                    muVaryL = threshold / fabs(inputSampleL);
                    muAttackL = sqrt(fabs(muSpeedAL));
                    muCoefficientAL = muCoefficientAL * (muAttackL - 1.0);
                    if (muVaryL < threshold) {
                        muCoefficientAL = muCoefficientAL + threshold;
                    }
                    else {
                        muCoefficientAL = muCoefficientAL + muVaryL;
                    }
                    muCoefficientAL = muCoefficientAL / muAttackL;
                    muNewSpeedL = muSpeedAL * (muSpeedAL - 1.0);
                    muNewSpeedL = muNewSpeedL + release;
                    muSpeedAL = muNewSpeedL / muSpeedAL;
                    if (muSpeedAL > maxRelease) {
                        muSpeedAL = maxRelease;
                    }
                }
                else {
                    muCoefficientAL = muCoefficientAL * ((muSpeedAL * muSpeedAL) - 1.0);
                    muCoefficientAL = muCoefficientAL + 1.0;
                    muCoefficientAL = muCoefficientAL / (muSpeedAL * muSpeedAL);
                    muNewSpeedL = muSpeedAL * (muSpeedAL - 1.0);
                    muNewSpeedL = muNewSpeedL + attack;
                    muSpeedAL = muNewSpeedL / muSpeedAL;
                }
            }
            else {
                if (fabs(inputSampleL) > threshold) {
                    muVaryL = threshold / fabs(inputSampleL);
                    muAttackL = sqrt(fabs(muSpeedBL));
                    muCoefficientBL = muCoefficientBL * (muAttackL - 1);
                    if (muVaryL < threshold) {
                        muCoefficientBL = muCoefficientBL + threshold;
                    }
                    else {
                        muCoefficientBL = muCoefficientBL + muVaryL;
                    }
                    muCoefficientBL = muCoefficientBL / muAttackL;
                    muNewSpeedL = muSpeedBL * (muSpeedBL - 1.0);
                    muNewSpeedL = muNewSpeedL + release;
                    muSpeedBL = muNewSpeedL / muSpeedBL;
                    if (muSpeedBL > maxRelease) {
                        muSpeedBL = maxRelease;
                    }
                }
                else {
                    muCoefficientBL = muCoefficientBL * ((muSpeedBL * muSpeedBL) - 1.0);
                    muCoefficientBL = muCoefficientBL + 1.0;
                    muCoefficientBL = muCoefficientBL / (muSpeedBL * muSpeedBL);
                    muNewSpeedL = muSpeedBL * (muSpeedBL - 1.0);
                    muNewSpeedL = muNewSpeedL + attack;
                    muSpeedBL = muNewSpeedL / muSpeedBL;
                }
            }
            // got coefficients, adjusted speeds for L
            // adjust coefficients for R
            if (flip) {
                if (fabs(inputSampleR) > threshold) {
                    muVaryR = threshold / fabs(inputSampleR);
                    muAttackR = sqrt(fabs(muSpeedAR));
                    muCoefficientAR = muCoefficientAR * (muAttackR - 1.0);
                    if (muVaryR < threshold) {
                        muCoefficientAR = muCoefficientAR + threshold;
                    }
                    else {
                        muCoefficientAR = muCoefficientAR + muVaryR;
                    }
                    muCoefficientAR = muCoefficientAR / muAttackR;
                    muNewSpeedR = muSpeedAR * (muSpeedAR - 1.0);
                    muNewSpeedR = muNewSpeedR + release;
                    muSpeedAR = muNewSpeedR / muSpeedAR;
                    if (muSpeedAR > maxRelease) {
                        muSpeedAR = maxRelease;
                    }
                }
                else {
                    muCoefficientAR = muCoefficientAR * ((muSpeedAR * muSpeedAR) - 1.0);
                    muCoefficientAR = muCoefficientAR + 1.0;
                    muCoefficientAR = muCoefficientAR / (muSpeedAR * muSpeedAR);
                    muNewSpeedR = muSpeedAR * (muSpeedAR - 1.0);
                    muNewSpeedR = muNewSpeedR + attack;
                    muSpeedAR = muNewSpeedR / muSpeedAR;
                }
            }
            else {
                if (fabs(inputSampleR) > threshold) {
                    muVaryR = threshold / fabs(inputSampleR);
                    muAttackR = sqrt(fabs(muSpeedBR));
                    muCoefficientBR = muCoefficientBR * (muAttackR - 1);
                    if (muVaryR < threshold) {
                        muCoefficientBR = muCoefficientBR + threshold;
                    }
                    else {
                        muCoefficientBR = muCoefficientBR + muVaryR;
                    }
                    muCoefficientBR = muCoefficientBR / muAttackR;
                    muNewSpeedR = muSpeedBR * (muSpeedBR - 1.0);
                    muNewSpeedR = muNewSpeedR + release;
                    muSpeedBR = muNewSpeedR / muSpeedBR;
                    if (muSpeedBR > maxRelease) {
                        muSpeedBR = maxRelease;
                    }
                }
                else {
                    muCoefficientBR = muCoefficientBR * ((muSpeedBR * muSpeedBR) - 1.0);
                    muCoefficientBR = muCoefficientBR + 1.0;
                    muCoefficientBR = muCoefficientBR / (muSpeedBR * muSpeedBR);
                    muNewSpeedR = muSpeedBR * (muSpeedBR - 1.0);
                    muNewSpeedR = muNewSpeedR + attack;
                    muSpeedBR = muNewSpeedR / muSpeedBR;
                }
            }
            // got coefficients, adjusted speeds for R
            if (flip) {
                inputSampleL *= pow(muCoefficientAL, 2);
                inputSampleR *= pow(muCoefficientAR, 2);
            }
            else {
                inputSampleL *= pow(muCoefficientBL, 2);
                inputSampleR *= pow(muCoefficientBR, 2);
            }
            inputSampleL *= muMakeupGain;
            inputSampleR *= muMakeupGain;
            flip = !flip;
            // end compressor section
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::pop2
