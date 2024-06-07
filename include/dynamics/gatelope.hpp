#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::gatelope {

constexpr std::string_view k_name{ "Gatelope" };
constexpr std::string_view k_short_description{
    "Gatelope is a special gate that applies filters."
};
constexpr std::string_view k_long_description{
    "Gatelope was initially developed for Ola Sonmark, to solve the following problem: can you gate a tom mic in such a way that it rejects cymbal bleed, but lets the lows sustain longer, and then transitions into silence gracefully?It just so happens that in developing that, I also wanted to do the opposite: reject low frequency rumble and sustain the highs more. I thought it might be useful for tightening up spot mics on kick drums. And the result… does both those things, and anywhere in between, and various other effects besides. It’s existed as a secret, Mac-only, AU-only weapon for long enough. I didn’t want to wait any longer, so enjoy Gatelope now (the Mac AU build contains an extra plugin, Gatelinked, which works like the VSTs: Gatelope in AU is ‘N to N’ and meant to be used on mono tracks, and the VSTs and Gatelinked are exactly the same, but linked stereo to prevent the stereo image from going to the side randomly)The way to use Gatelope is, find the proper gating threshold with the top slider. Attack Speed might help depending on where you find that point. Then, Treble Sustain and Bass Sustain can be reduced to let the gate start gating. If you reduce Treble Sustain, it will be opening fully on an attack and then rolling off the treble as it closes. If you reduce Bass Sustain, it'll be rolling off bass as it closes. When these two roll-offs sweep past each other, your signal is gated and quiet: use the dry/wet to make the effect be at a lower ratio and let through some natural sound."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Gatelope final : public Effect<T>
{
    double iirLowpassAR;
    double iirLowpassBR;
    double iirHighpassAR;
    double iirHighpassBR;
    double iirLowpassAL;
    double iirLowpassBL;
    double iirHighpassAL;
    double iirHighpassBL;
    double treblefreq;
    double bassfreq;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Gatelope()
    {
        A = 0.0;
        B = 1.0;
        C = 0.5;
        D = 0.0;
        E = 1.0;
        iirLowpassAL = 0.0;
        iirLowpassBL = 0.0;
        iirHighpassAL = 0.0;
        iirHighpassBL = 0.0;
        iirLowpassAR = 0.0;
        iirLowpassBR = 0.0;
        iirHighpassAR = 0.0;
        iirHighpassBR = 0.0;
        treblefreq = 1.0;
        bassfreq = 0.0;
        flip = false;
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
            case kParamB: return 1.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "thresh"; break;
            case kParamB: return "trebsus"; break;
            case kParamC: return "basssus"; break;
            case kParamD: return "attacks"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thresh"; break;
            case kParamB: return "TrebSus"; break;
            case kParamC: return "BassSus"; break;
            case kParamD: return "AttackS"; break;
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
        // speed settings around release
        double threshold = pow(A, 2);
        // gain settings around threshold
        double trebledecay = pow(1.0 - B, 2) / 4196.0;
        double bassdecay = pow(1.0 - C, 2) / 8192.0;
        double slowAttack = (pow(D, 3) * 3) + 0.003;
        double wet = E;
        slowAttack /= overallscale;
        trebledecay /= overallscale;
        bassdecay /= overallscale;
        trebledecay += 1.0;
        bassdecay += 1.0;
        double attackSpeed;
        double highestSample;
        // this VST version comes from the AU, Gatelinked, because it's stereo.
        // if used on a mono track it'll act like the mono N to N
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
            if (fabs(inputSampleL) > fabs(inputSampleR)) {
                attackSpeed = slowAttack - (fabs(inputSampleL) * slowAttack * 0.5);
                highestSample = fabs(inputSampleL);
            }
            else {
                attackSpeed = slowAttack - (fabs(inputSampleR) * slowAttack * 0.5); // we're triggering off the highest amplitude
                highestSample = fabs(inputSampleR); // and making highestSample the abs() of that amplitude
            }
            if (attackSpeed < 0.0) {
                attackSpeed = 0.0;
            }
            // softening onset click depending on how hard we're getting it
            if (flip)
            {
                if (highestSample > threshold)
                {
                    treblefreq += attackSpeed;
                    if (treblefreq > 2.0) {
                        treblefreq = 2.0;
                    }
                    bassfreq -= attackSpeed;
                    bassfreq -= attackSpeed;
                    if (bassfreq < 0.0) {
                        bassfreq = 0.0;
                    }
                    iirLowpassAL = iirLowpassBL = inputSampleL;
                    iirHighpassAL = iirHighpassBL = 0.0;
                    iirLowpassAR = iirLowpassBR = inputSampleR;
                    iirHighpassAR = iirHighpassBR = 0.0;
                }
                else
                {
                    treblefreq -= bassfreq;
                    treblefreq /= trebledecay;
                    treblefreq += bassfreq;
                    bassfreq -= treblefreq;
                    bassfreq /= bassdecay;
                    bassfreq += treblefreq;
                }
                if (treblefreq >= 1.0) {
                    iirLowpassAL = inputSampleL;
                    iirLowpassAR = inputSampleR;
                }
                else {
                    iirLowpassAL = (iirLowpassAL * (1.0 - treblefreq)) + (inputSampleL * treblefreq);
                    iirLowpassAR = (iirLowpassAR * (1.0 - treblefreq)) + (inputSampleR * treblefreq);
                }
                if (bassfreq > 1.0) {
                    bassfreq = 1.0;
                }
                if (bassfreq > 0.0) {
                    iirHighpassAL = (iirHighpassAL * (1.0 - bassfreq)) + (inputSampleL * bassfreq);
                    iirHighpassAR = (iirHighpassAR * (1.0 - bassfreq)) + (inputSampleR * bassfreq);
                }
                else {
                    iirHighpassAL = 0.0;
                    iirHighpassAR = 0.0;
                }
                if (treblefreq > bassfreq) {
                    inputSampleL = (iirLowpassAL - iirHighpassAL);
                    inputSampleR = (iirLowpassAR - iirHighpassAR);
                }
                else {
                    inputSampleL = 0.0;
                    inputSampleR = 0.0;
                }
            }
            else
            {
                if (highestSample > threshold)
                {
                    treblefreq += attackSpeed;
                    if (treblefreq > 2.0) {
                        treblefreq = 2.0;
                    }
                    bassfreq -= attackSpeed;
                    bassfreq -= attackSpeed;
                    if (bassfreq < 0.0) {
                        bassfreq = 0.0;
                    }
                    iirLowpassAL = iirLowpassBL = inputSampleL;
                    iirHighpassAL = iirHighpassBL = 0.0;
                    iirLowpassAR = iirLowpassBR = inputSampleR;
                    iirHighpassAR = iirHighpassBR = 0.0;
                }
                else
                {
                    treblefreq -= bassfreq;
                    treblefreq /= trebledecay;
                    treblefreq += bassfreq;
                    bassfreq -= treblefreq;
                    bassfreq /= bassdecay;
                    bassfreq += treblefreq;
                }
                if (treblefreq >= 1.0) {
                    iirLowpassBL = inputSampleL;
                    iirLowpassBR = inputSampleR;
                }
                else {
                    iirLowpassBL = (iirLowpassBL * (1.0 - treblefreq)) + (inputSampleL * treblefreq);
                    iirLowpassBR = (iirLowpassBR * (1.0 - treblefreq)) + (inputSampleR * treblefreq);
                }
                if (bassfreq > 1.0) {
                    bassfreq = 1.0;
                }
                if (bassfreq > 0.0) {
                    iirHighpassBL = (iirHighpassBL * (1.0 - bassfreq)) + (inputSampleL * bassfreq);
                    iirHighpassBR = (iirHighpassBR * (1.0 - bassfreq)) + (inputSampleR * bassfreq);
                }
                else {
                    iirHighpassBL = 0.0;
                    iirHighpassBR = 0.0;
                }
                if (treblefreq > bassfreq) {
                    inputSampleL = (iirLowpassBL - iirHighpassBL);
                    inputSampleR = (iirLowpassBR - iirHighpassBR);
                }
                else {
                    inputSampleL = 0.0;
                    inputSampleR = 0.0;
                }
            }
            // done full gated envelope filtered effect
            inputSampleL = ((1 - wet) * drySampleL) + (wet * inputSampleL);
            inputSampleR = ((1 - wet) * drySampleR) + (wet * inputSampleR);
            // we're going to set up a dry/wet control instead of a min. threshold
            flip = !flip;
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
} // namespace airwindohhs::gatelope
