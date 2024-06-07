#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::varimu {

constexpr std::string_view k_name{ "VariMu" };
constexpr std::string_view k_short_description{
    "VariMu is a more organic variation on Pressure (a compressor)"
};
constexpr std::string_view k_long_description{
    "I’ve got something nice: everybody likes yet another compressor, right? I’ve got ButterComp, Swell, Logical4, the one in CStrip, Surge, SurgeTide, Pyewacket, Thunder, and of course Pressure4. All those are free (and I’m working on getting them open source as well, but first I want to get the actual plugins out).So clearly what we need is YET ANOTHER free compressor as we plainly don’t have nearly enough. We must run all the compressors, in a row, until everything is so compressed we can’t even stand ourselves! :DNo… there’s a non-joke reason. And that reason is, all those sound very different. They won’t all work for everyone: for instance, Pyewacket is all about retaining the attack transients of things and just stepping on the tails of envelopes. Swell is about stomping out those very attack transients until they’re all gone. Surge is about very smooth gain shifts that are totally transparent and more like an automatic gain control. SurgeTide is the same but more so, it’s almost completely unmanageable. Logical4 is about acting like a hardware stereo 2-buss compressor, and so on.VariMu comes from Pressure, originally. What it does differently is trigger using different math: it uses the square of the input signal, not just the signal alone, and it also handles brief transients differently. This makes it sound very different from Pressure, so if you like Pressure you should try it… and if you didn’t like Pressure, you should also try it, because it sounds different.That’s really the essence of all these Airwindows compressors (perhaps more than any other class of plugin). They’re all different algorithms, often pretty weird ones, and sound very different from each other. So you have to pick which ones fit with your type of music… but also, you don’t have to pick, because they’re all available to you for experimenting with, and if I come up with something weird and non-useful (SurgeTide comes to mind) then thanks to the Patreon I can put it out anyway, without worrying that it would kill the buzz and have people wondering if Airwindows has lost it. Even if it’s useful to just one person, I can release anything, no matter how weird.But this time, I don’t think I’ll be making excuses like that. VariMu has a good sound to it. I hope you like it :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class VariMu final : public Effect<T>
{
    double muVaryL;
    double muAttackL;
    double muNewSpeedL;
    double muSpeedAL;
    double muSpeedBL;
    double muCoefficientAL;
    double muCoefficientBL;
    double previousL;
    double muVaryR;
    double muAttackR;
    double muNewSpeedR;
    double muSpeedAR;
    double muSpeedBR;
    double muCoefficientAR;
    double muCoefficientBR;
    double previousR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    VariMu()
    {
        muSpeedAL = 10000;
        muSpeedBL = 10000;
        muCoefficientAL = 1;
        muCoefficientBL = 1;
        muVaryL = 1;
        previousL = 0.0;
        muSpeedAR = 10000;
        muSpeedBR = 10000;
        muCoefficientAR = 1;
        muCoefficientBR = 1;
        muVaryR = 1;
        previousR = 0.0;
        flip = false;
        A = 0.0;
        B = 0.5;
        C = 1.0;
        D = 1.0;
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
            return 0.5;
            break;
        kParamC:
            return 1.0;
            break;
        kParamD:
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
            return "intensty";
            break;
        kParamB:
            return "speed";
            break;
        kParamC:
            return "output";
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
            return "Intensty";
            break;
        kParamB:
            return "Speed";
            break;
        kParamC:
            return "Output";
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

        double overallscale = 2.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double threshold = 1.001 - (1.0 - pow(1.0 - A, 3));
        double muMakeupGain = sqrt(1.0 / threshold);
        muMakeupGain = (muMakeupGain + sqrt(muMakeupGain)) / 2.0;
        muMakeupGain = sqrt(muMakeupGain);
        double outGain = sqrt(muMakeupGain);
        // gain settings around threshold
        double release = pow((1.15 - B), 5) * 32768.0;
        release /= overallscale;
        double fastest = sqrt(release);
        // speed settings around release
        double coefficient;
        double output = outGain * C;
        double wet = D;
        double squaredSampleL;
        double squaredSampleR;
        // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            noisesourceL = noisesourceL % 1700021;
            noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
                inputSampleL -= applyresidue;
            }
            noisesourceR = noisesourceR % 1700021;
            noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
                inputSampleR -= applyresidue;
            }
            // for live air, we always apply the dither noise. Then, if our result is
            // effectively digital black, we'll subtract it aVariMu. We want a 'air' hiss
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (fabs(inputSampleL) > fabs(previousL)) {
                squaredSampleL = previousL * previousL;
            }
            else {
                squaredSampleL = inputSampleL * inputSampleL;
            }
            previousL = inputSampleL;
            inputSampleL *= muMakeupGain;
            if (fabs(inputSampleR) > fabs(previousR)) {
                squaredSampleR = previousR * previousR;
            }
            else {
                squaredSampleR = inputSampleR * inputSampleR;
            }
            previousR = inputSampleR;
            inputSampleR *= muMakeupGain;
            // adjust coefficients for L
            if (flip)
            {
                if (fabs(squaredSampleL) > threshold)
                {
                    muVaryL = threshold / fabs(squaredSampleL);
                    muAttackL = sqrt(fabs(muSpeedAL));
                    muCoefficientAL = muCoefficientAL * (muAttackL - 1.0);
                    if (muVaryL < threshold)
                    {
                        muCoefficientAL = muCoefficientAL + threshold;
                    }
                    else
                    {
                        muCoefficientAL = muCoefficientAL + muVaryL;
                    }
                    muCoefficientAL = muCoefficientAL / muAttackL;
                }
                else
                {
                    muCoefficientAL = muCoefficientAL * ((muSpeedAL * muSpeedAL) - 1.0);
                    muCoefficientAL = muCoefficientAL + 1.0;
                    muCoefficientAL = muCoefficientAL / (muSpeedAL * muSpeedAL);
                }
                muNewSpeedL = muSpeedAL * (muSpeedAL - 1);
                muNewSpeedL = muNewSpeedL + fabs(squaredSampleL * release) + fastest;
                muSpeedAL = muNewSpeedL / muSpeedAL;
            }
            else
            {
                if (fabs(squaredSampleL) > threshold)
                {
                    muVaryL = threshold / fabs(squaredSampleL);
                    muAttackL = sqrt(fabs(muSpeedBL));
                    muCoefficientBL = muCoefficientBL * (muAttackL - 1);
                    if (muVaryL < threshold)
                    {
                        muCoefficientBL = muCoefficientBL + threshold;
                    }
                    else
                    {
                        muCoefficientBL = muCoefficientBL + muVaryL;
                    }
                    muCoefficientBL = muCoefficientBL / muAttackL;
                }
                else
                {
                    muCoefficientBL = muCoefficientBL * ((muSpeedBL * muSpeedBL) - 1.0);
                    muCoefficientBL = muCoefficientBL + 1.0;
                    muCoefficientBL = muCoefficientBL / (muSpeedBL * muSpeedBL);
                }
                muNewSpeedL = muSpeedBL * (muSpeedBL - 1);
                muNewSpeedL = muNewSpeedL + fabs(squaredSampleL * release) + fastest;
                muSpeedBL = muNewSpeedL / muSpeedBL;
            }
            // got coefficients, adjusted speeds for L
            // adjust coefficients for R
            if (flip)
            {
                if (fabs(squaredSampleR) > threshold)
                {
                    muVaryR = threshold / fabs(squaredSampleR);
                    muAttackR = sqrt(fabs(muSpeedAR));
                    muCoefficientAR = muCoefficientAR * (muAttackR - 1.0);
                    if (muVaryR < threshold)
                    {
                        muCoefficientAR = muCoefficientAR + threshold;
                    }
                    else
                    {
                        muCoefficientAR = muCoefficientAR + muVaryR;
                    }
                    muCoefficientAR = muCoefficientAR / muAttackR;
                }
                else
                {
                    muCoefficientAR = muCoefficientAR * ((muSpeedAR * muSpeedAR) - 1.0);
                    muCoefficientAR = muCoefficientAR + 1.0;
                    muCoefficientAR = muCoefficientAR / (muSpeedAR * muSpeedAR);
                }
                muNewSpeedR = muSpeedAR * (muSpeedAR - 1);
                muNewSpeedR = muNewSpeedR + fabs(squaredSampleR * release) + fastest;
                muSpeedAR = muNewSpeedR / muSpeedAR;
            }
            else
            {
                if (fabs(squaredSampleR) > threshold)
                {
                    muVaryR = threshold / fabs(squaredSampleR);
                    muAttackR = sqrt(fabs(muSpeedBR));
                    muCoefficientBR = muCoefficientBR * (muAttackR - 1);
                    if (muVaryR < threshold)
                    {
                        muCoefficientBR = muCoefficientBR + threshold;
                    }
                    else
                    {
                        muCoefficientBR = muCoefficientBR + muVaryR;
                    }
                    muCoefficientBR = muCoefficientBR / muAttackR;
                }
                else
                {
                    muCoefficientBR = muCoefficientBR * ((muSpeedBR * muSpeedBR) - 1.0);
                    muCoefficientBR = muCoefficientBR + 1.0;
                    muCoefficientBR = muCoefficientBR / (muSpeedBR * muSpeedBR);
                }
                muNewSpeedR = muSpeedBR * (muSpeedBR - 1);
                muNewSpeedR = muNewSpeedR + fabs(squaredSampleR * release) + fastest;
                muSpeedBR = muNewSpeedR / muSpeedBR;
            }
            // got coefficients, adjusted speeds for R
            if (flip)
            {
                coefficient = (muCoefficientAL + pow(muCoefficientAL, 2)) / 2.0;
                inputSampleL *= coefficient;
                coefficient = (muCoefficientAR + pow(muCoefficientAR, 2)) / 2.0;
                inputSampleR *= coefficient;
            }
            else
            {
                coefficient = (muCoefficientBL + pow(muCoefficientBL, 2)) / 2.0;
                inputSampleL *= coefficient;
                coefficient = (muCoefficientBR + pow(muCoefficientBR, 2)) / 2.0;
                inputSampleR *= coefficient;
            }
            // applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
            // applied gain correction to control output level- tends to constrain sound rather than inflate it
            flip = !flip;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
} // namespace airwindohhs::varimu
