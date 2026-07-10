#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::angleeq {

constexpr std::string_view k_name{ "AngleEQ" };
constexpr std::string_view k_short_description{
    "AngleEQ is a strange and colorful EQ."
};
constexpr std::string_view k_long_description{
    "This would have been amazing, had it worked. Instead, it's astonishing, and is never going to be your main EQ, or mine, ever.Seriously, it'd be a world of hurt, and don't do it. Not even while sandwiching it with uLaw plugins.Since you can't use it for real work, what good is it? Also, why can't you use it for real work?It's not just that its response is irregular and non-flat. You could deal with that. The trouble is, it's a new attempt at filtering, meant to do a super-sharp but non-resonant lowpass for a crossover.And it does a lot of that! But in the process, it scrambles the audio so pervasively that if you try to assemble an EQ out of subtracting it from the dry signal (a neat trick for making 'flat' be pristine beyond all reason) the 'highpass' you get from subtracting the lowpass, is a complete mess full of phase-rotated bass.So I came up with a way to get a real highpass, even with multiple stages of this filtering. And I got one, and even that is decidedly strange.So, AngleEQ is a highpass for a treble band, a lowpass for a bass band, and a midrange that has a separate highpass and lowpass, just because they will not combine in any suitable way anyhow. Then you've got a dry/wet because combining any of these bands with dry brings even more havoc, and then the dry/wet also is an attenuverter and lets you apply the EQ inverted because it wasn't doing enough damage already.How does it sound? Very opaque, weirdly resonant, perhaps like the largest color-style EQ on the biggest most overdesigned mixing board ever. Nothing about it is well behaved, it cannot do 'clean' to save its life, it exists only to make sonic trouble and produce strange pungent tones full of resonances and cancellations.Just because it's not a proper EQ doesn't mean you can't have fun. Hope you enjoy AngleEQ!"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class AngleEQ final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    double iirHPositionL[37];
    double iirHAngleL[37];
    double iirMHPositionL[37];
    double iirMHAngleL[37];
    double iirMLPositionL[37];
    double iirMLAngleL[37];
    double iirLPositionL[37];
    double iirLAngleL[37];
    double iirHPositionR[37];
    double iirHAngleR[37];
    double iirMHPositionR[37];
    double iirMHAngleR[37];
    double iirMLPositionR[37];
    double iirMLAngleR[37];
    double iirLPositionR[37];
    double iirLAngleR[37];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    AngleEQ()
    {
        A = 0.5;
        B = 0.4;
        C = 0.5;
        D = 0.8;
        E = 0.8;
        F = 0.2;
        G = 0.2;
        H = 1.0;
        for (int count = 0; count < 36; count++) {
            iirHPositionL[count] = 0.0;
            iirHAngleL[count] = 0.0;
            iirMHPositionL[count] = 0.0;
            iirMHAngleL[count] = 0.0;
            iirMLPositionL[count] = 0.0;
            iirMLAngleL[count] = 0.0;
            iirLPositionL[count] = 0.0;
            iirLAngleL[count] = 0.0;
            iirHPositionR[count] = 0.0;
            iirHAngleR[count] = 0.0;
            iirMHPositionR[count] = 0.0;
            iirMHAngleR[count] = 0.0;
            iirMLPositionR[count] = 0.0;
            iirMLAngleR[count] = 0.0;
            iirLPositionR[count] = 0.0;
            iirLAngleR[count] = 0.0;
        }
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
        kParamH = 7,
        kNumParameters = 8
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
            case kParamH: H = value; break;

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
            case kParamH: return H; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.4; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.8; break;
            case kParamE: return 0.8; break;
            case kParamF: return 0.2; break;
            case kParamG: return 0.2; break;
            case kParamH: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "mid"; break;
            case kParamC: return "low"; break;
            case kParamD: return "hifreq"; break;
            case kParamE: return "mhifreq"; break;
            case kParamF: return "mlofreq"; break;
            case kParamG: return "lofreq"; break;
            case kParamH: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "Mid"; break;
            case kParamC: return "Low"; break;
            case kParamD: return "HiFreq"; break;
            case kParamE: return "MHiFreq"; break;
            case kParamF: return "MLoFreq"; break;
            case kParamG: return "LoFreq"; break;
            case kParamH: return "Inv/Wet"; break;

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
            case kParamH: return std::to_string(H); break;

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
            case kParamH: return ""; break;

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
        double highVol = pow(A * 2.0, 2.0);
        double midVol = pow(B * 2.0, 2.0);
        double lowVol = pow(C * 2.0, 2.0);
        double hFreq = pow(D, overallscale);
        double mhFreq = pow(E, overallscale);
        double mlFreq = pow(F, overallscale + 3.0);
        double lFreq = pow(G, overallscale + 3.0);
        double dry = 2.0 - (H * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
        double wet = (H * 2.0) - 1.0; // inv-dry-wet for highpass
        while (--sampleFrames >= 0) {
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
            double midSampleL = inputSampleL;
            double midSampleR = inputSampleR;
            double highSampleL = inputSampleL;
            double highSampleR = inputSampleR;
            for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
                iirHAngleL[count] = (iirHAngleL[count] * (1.0 - hFreq)) + ((inputSampleL - iirHPositionL[count]) * hFreq);
                inputSampleL = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
                iirHPositionL[count] = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
                highSampleL -= (inputSampleL * (1.0 / (3.0 + (hFreq * 32.0))));
                iirHAngleR[count] = (iirHAngleR[count] * (1.0 - hFreq)) + ((inputSampleR - iirHPositionR[count]) * hFreq);
                inputSampleR = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
                iirHPositionR[count] = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
                highSampleR -= (inputSampleR * (1.0 / (3.0 + (hFreq * 32.0))));
            } // highpass point of treble band
            inputSampleL = midSampleL; // restore for second highpass
            inputSampleR = midSampleR; // restore for second highpass
            for (int count = 0; count < (3.0 + (mlFreq * 32.0)); count++) {
                iirMHAngleL[count] = (iirMHAngleL[count] * (1.0 - mlFreq)) + ((inputSampleL - iirMHPositionL[count]) * mlFreq);
                inputSampleL = ((iirMHPositionL[count] + (iirMHAngleL[count] * mlFreq)) * (1.0 - mlFreq)) + (inputSampleL * mlFreq);
                iirMHPositionL[count] = ((iirMHPositionL[count] + (iirMHAngleL[count] * mlFreq)) * (1.0 - mlFreq)) + (inputSampleL * mlFreq);
                midSampleL -= (inputSampleL * (1.0 / (3.0 + (mlFreq * 32.0))));
                iirMHAngleR[count] = (iirMHAngleR[count] * (1.0 - mlFreq)) + ((inputSampleR - iirMHPositionR[count]) * mlFreq);
                inputSampleR = ((iirMHPositionR[count] + (iirMHAngleR[count] * mlFreq)) * (1.0 - mlFreq)) + (inputSampleR * mlFreq);
                iirMHPositionR[count] = ((iirMHPositionR[count] + (iirMHAngleR[count] * mlFreq)) * (1.0 - mlFreq)) + (inputSampleR * mlFreq);
                midSampleR -= (inputSampleR * (1.0 / (3.0 + (mlFreq * 32.0))));
            } // highpass point of mid-to-low band
            for (int count = 0; count < (3.0 + (mhFreq * 32.0)); count++) {
                iirMLAngleL[count] = (iirMLAngleL[count] * (1.0 - mhFreq)) + ((midSampleL - iirMLPositionL[count]) * mhFreq);
                midSampleL = ((iirMLPositionL[count] + (iirMLAngleL[count] * mhFreq)) * (1.0 - mhFreq)) + (midSampleL * mhFreq);
                iirMLPositionL[count] = ((iirMLPositionL[count] + (iirMLAngleL[count] * mhFreq)) * (1.0 - mhFreq)) + (midSampleL * mhFreq);
                iirMLAngleR[count] = (iirMLAngleR[count] * (1.0 - mhFreq)) + ((midSampleR - iirMLPositionR[count]) * mhFreq);
                midSampleR = ((iirMLPositionR[count] + (iirMLAngleR[count] * mhFreq)) * (1.0 - mhFreq)) + (midSampleR * mhFreq);
                iirMLPositionR[count] = ((iirMLPositionR[count] + (iirMLAngleR[count] * mhFreq)) * (1.0 - mhFreq)) + (midSampleR * mhFreq);
            } // lowpass point of mid-to-high band
            double lowSampleL = inputSampleL;
            double lowSampleR = inputSampleR;
            for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
                iirLAngleL[count] = (iirLAngleL[count] * (1.0 - lFreq)) + ((lowSampleL - iirLPositionL[count]) * lFreq);
                lowSampleL = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSampleL * lFreq);
                iirLPositionL[count] = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSampleL * lFreq);
                iirLAngleR[count] = (iirLAngleR[count] * (1.0 - lFreq)) + ((lowSampleR - iirLPositionR[count]) * lFreq);
                lowSampleR = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSampleR * lFreq);
                iirLPositionR[count] = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSampleR * lFreq);
            } // lowpass point of low band
            inputSampleL = (drySampleL * dry) + (((highSampleL * highVol) + (midSampleL * midVol) + (lowSampleL * lowVol)) * wet);
            inputSampleR = (drySampleR * dry) + (((highSampleR * highVol) + (midSampleR * midVol) + (lowSampleR * lowVol)) * wet);
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
} // namespace airwindohhs::angleeq
