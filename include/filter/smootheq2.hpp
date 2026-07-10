#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::smootheq2 {

constexpr std::string_view k_name{ "SmoothEQ2" };
constexpr std::string_view k_short_description{
    "SmoothEQ2 is a wildly intuitive vibe EQ with clarity and intensity."
};
constexpr std::string_view k_long_description{
    "Turns out there was a lot of potential in SmoothEQ.The idea here was to go to a four-band, like the parametric in ConsoleX, but instead of peaks and notches make it fully use shelves in the manner of the original SmoothEQ, so it can deliver ultimate clarity when subtlety is needed.Then, with the addition of a simpler type of filter (thanks to my livestream chat for knowing exactly how to specify these with great accuracy) it was possible to use even higher orders of filtering, even steeper crossovers for the shelves.Then, the question 'still Bessel, or go with Butterworth for the Q factor?' got drastically derailed by the stranger question 'why not just have all these Q factors vary according to the frequency settings of the bands?'Not the settings of the crossovers, no… the settings of the BANDS. Sweeping a band moves both crossovers bordering it. So does that move a flat shelf around, which sounds like a nice thing to do?Nope, because if you make the resonances track how close each center frequency is to each other, you get shelves that are also tilt EQs accentuating where in the band's travel the band is. So if you push high-mid up, the cutoff against high gets steeper, and the cutoff against low-mid gets shallower, more Bessel-like. Hard to explain. You have a frequency band that is also a tilt EQ for just that shelf, and that follows a center 'resonant peak' which isn't actually there at all.What does this mean? Try it. Boost or cut stuff, sweep the bands around, see how they act like sweepable resonances but are actually tilting shelves. If you can (I know it's unusual) and you have the four joystick design I had Yaeltex make for me, put fingers on all four joysticks and morph a sound into whatever you like on all bands simultaneously. The way this tracks your intentions with EQ, I think that would actually work. Or, any other way you can dedicate eight knobs to a bank of EQs, give that a try too. Or it'll work perfectly fine just in the DAW as sliders, it's just that this one lends itself incredibly well to hands-on control. There's controllers for cheap that have tiny vertical faders with panpots over them: have the vertical faders be band volumes, and the panpot above it be the corresponding frequency.This one I'm building into a Console version just for me :) but here it is, way earlier than that, so you can see what's going into a Console version that comes after the promised ConsoleH. I told you I was going to let you swap out many different Consoles on a track-by-track basis, and I've got ideas for just that. Hope you like the preview that is SmoothEQ2."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class SmoothEQ2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    }; // coefficient interpolating bessel filter, stereo
    double highA[biq_total];
    double highB[biq_total];
    double highC[biq_total];
    double highLIIR;
    double highRIIR;
    double midA[biq_total];
    double midB[biq_total];
    double midC[biq_total];
    double midLIIR;
    double midRIIR;
    double lowA[biq_total];
    double lowB[biq_total];
    double lowC[biq_total];
    double lowLIIR;
    double lowRIIR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SmoothEQ2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        for (int x = 0; x < biq_total; x++) {
            highA[x] = 0.0;
            highB[x] = 0.0;
            highC[x] = 0.0;
            midA[x] = 0.0;
            midB[x] = 0.0;
            midC[x] = 0.0;
            lowA[x] = 0.0;
            lowB[x] = 0.0;
            lowC[x] = 0.0;
        }
        highLIIR = 0.0;
        highRIIR = 0.0;
        midLIIR = 0.0;
        midRIIR = 0.0;
        lowLIIR = 0.0;
        lowRIIR = 0.0;
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
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "hmid"; break;
            case kParamC: return "lmid"; break;
            case kParamD: return "bass"; break;
            case kParamE: return "highf"; break;
            case kParamF: return "hmidf"; break;
            case kParamG: return "lmidf"; break;
            case kParamH: return "bassf"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "HMid"; break;
            case kParamC: return "LMid"; break;
            case kParamD: return "Bass"; break;
            case kParamE: return "HighF"; break;
            case kParamF: return "HMidF"; break;
            case kParamG: return "LMidF"; break;
            case kParamH: return "BassF"; break;

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

        double trebleGain = (A - 0.5) * 2.0;
        trebleGain = 1.0 + (trebleGain * fabs(trebleGain) * fabs(trebleGain));
        double highmidGain = (B - 0.5) * 2.0;
        highmidGain = 1.0 + (highmidGain * fabs(highmidGain) * fabs(highmidGain));
        double lowmidGain = (C - 0.5) * 2.0;
        lowmidGain = 1.0 + (lowmidGain * fabs(lowmidGain) * fabs(lowmidGain));
        double bassGain = (D - 0.5) * 2.0;
        bassGain = 1.0 + (bassGain * fabs(bassGain) * fabs(bassGain));
        double trebleRef = E - 0.5;
        double highmidRef = F - 0.5;
        double lowmidRef = G - 0.5;
        double bassRef = H - 0.5;
        double highF = 0.75 + ((trebleRef + trebleRef + trebleRef + highmidRef) * 0.125);
        double bassF = 0.25 + ((lowmidRef + bassRef + bassRef + bassRef) * 0.125);
        double midF = (highF * 0.5) + (bassF * 0.5) + ((highmidRef + lowmidRef) * 0.125);
        double highQ = fmax(fmin(1.0 + (highmidRef - trebleRef), 4.0), 0.125);
        double midQ = fmax(fmin(1.0 + (lowmidRef - highmidRef), 4.0), 0.125);
        double lowQ = fmax(fmin(1.0 + (bassRef - lowmidRef), 4.0), 0.125);
        highA[biq_freq] = ((pow(highF, 3) * 20000.0) / Effect<T>::getSampleRate());
        highC[biq_freq] = highB[biq_freq] = highA[biq_freq] = fmax(fmin(highA[biq_freq], 0.4999), 0.00025);
        double highFreq = pow(highF, 3) * 20000.0;
        double omega = 2.0 * M_PI * (highFreq / Effect<T>::getSampleRate());
        double K = 2.0 - cos(omega);
        double highCoef = -sqrt((K * K) - 1.0) + K;
        highA[biq_reso] = 2.24697960 * highQ;
        highB[biq_reso] = 0.80193774 * highQ;
        highC[biq_reso] = 0.55495813 * highQ;
        midA[biq_freq] = ((pow(midF, 3) * 20000.0) / Effect<T>::getSampleRate());
        midC[biq_freq] = midB[biq_freq] = midA[biq_freq] = fmax(fmin(midA[biq_freq], 0.4999), 0.00025);
        double midFreq = pow(midF, 3) * 20000.0;
        omega = 2.0 * M_PI * (midFreq / Effect<T>::getSampleRate());
        K = 2.0 - cos(omega);
        double midCoef = -sqrt((K * K) - 1.0) + K;
        midA[biq_reso] = 2.24697960 * midQ;
        midB[biq_reso] = 0.80193774 * midQ;
        midC[biq_reso] = 0.55495813 * midQ;
        lowA[biq_freq] = ((pow(bassF, 3) * 20000.0) / Effect<T>::getSampleRate());
        lowC[biq_freq] = lowB[biq_freq] = lowA[biq_freq] = fmax(fmin(lowA[biq_freq], 0.4999), 0.00025);
        double lowFreq = pow(bassF, 3) * 20000.0;
        omega = 2.0 * M_PI * (lowFreq / Effect<T>::getSampleRate());
        K = 2.0 - cos(omega);
        double lowCoef = -sqrt((K * K) - 1.0) + K;
        lowA[biq_reso] = 2.24697960 * lowQ;
        lowB[biq_reso] = 0.80193774 * lowQ;
        lowC[biq_reso] = 0.55495813 * lowQ;
        K = tan(M_PI * highA[biq_freq]);
        double norm = 1.0 / (1.0 + K / highA[biq_reso] + K * K);
        highA[biq_a0] = K * K * norm;
        highA[biq_a1] = 2.0 * highA[biq_a0];
        highA[biq_a2] = highA[biq_a0];
        highA[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        highA[biq_b2] = (1.0 - K / highA[biq_reso] + K * K) * norm;
        K = tan(M_PI * highB[biq_freq]);
        norm = 1.0 / (1.0 + K / highB[biq_reso] + K * K);
        highB[biq_a0] = K * K * norm;
        highB[biq_a1] = 2.0 * highB[biq_a0];
        highB[biq_a2] = highB[biq_a0];
        highB[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        highB[biq_b2] = (1.0 - K / highB[biq_reso] + K * K) * norm;
        K = tan(M_PI * highC[biq_freq]);
        norm = 1.0 / (1.0 + K / highC[biq_reso] + K * K);
        highC[biq_a0] = K * K * norm;
        highC[biq_a1] = 2.0 * highC[biq_a0];
        highC[biq_a2] = highC[biq_a0];
        highC[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        highC[biq_b2] = (1.0 - K / highC[biq_reso] + K * K) * norm;
        K = tan(M_PI * midA[biq_freq]);
        norm = 1.0 / (1.0 + K / midA[biq_reso] + K * K);
        midA[biq_a0] = K * K * norm;
        midA[biq_a1] = 2.0 * midA[biq_a0];
        midA[biq_a2] = midA[biq_a0];
        midA[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        midA[biq_b2] = (1.0 - K / midA[biq_reso] + K * K) * norm;
        K = tan(M_PI * midB[biq_freq]);
        norm = 1.0 / (1.0 + K / midB[biq_reso] + K * K);
        midB[biq_a0] = K * K * norm;
        midB[biq_a1] = 2.0 * midB[biq_a0];
        midB[biq_a2] = midB[biq_a0];
        midB[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        midB[biq_b2] = (1.0 - K / midB[biq_reso] + K * K) * norm;
        K = tan(M_PI * midC[biq_freq]);
        norm = 1.0 / (1.0 + K / midC[biq_reso] + K * K);
        midC[biq_a0] = K * K * norm;
        midC[biq_a1] = 2.0 * midC[biq_a0];
        midC[biq_a2] = midC[biq_a0];
        midC[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        midC[biq_b2] = (1.0 - K / midC[biq_reso] + K * K) * norm;
        K = tan(M_PI * lowA[biq_freq]);
        norm = 1.0 / (1.0 + K / lowA[biq_reso] + K * K);
        lowA[biq_a0] = K * K * norm;
        lowA[biq_a1] = 2.0 * lowA[biq_a0];
        lowA[biq_a2] = lowA[biq_a0];
        lowA[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        lowA[biq_b2] = (1.0 - K / lowA[biq_reso] + K * K) * norm;
        K = tan(M_PI * lowB[biq_freq]);
        norm = 1.0 / (1.0 + K / lowB[biq_reso] + K * K);
        lowB[biq_a0] = K * K * norm;
        lowB[biq_a1] = 2.0 * lowB[biq_a0];
        lowB[biq_a2] = lowB[biq_a0];
        lowB[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        lowB[biq_b2] = (1.0 - K / lowB[biq_reso] + K * K) * norm;
        K = tan(M_PI * lowC[biq_freq]);
        norm = 1.0 / (1.0 + K / lowC[biq_reso] + K * K);
        lowC[biq_a0] = K * K * norm;
        lowC[biq_a1] = 2.0 * lowC[biq_a0];
        lowC[biq_a2] = lowC[biq_a0];
        lowC[biq_b1] = 2.0 * (K * K - 1.0) * norm;
        lowC[biq_b2] = (1.0 - K / lowC[biq_reso] + K * K) * norm;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double trebleL = inputSampleL;
            double outSample = (trebleL * highA[biq_a0]) + highA[biq_sL1];
            highA[biq_sL1] = (trebleL * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sL2];
            highA[biq_sL2] = (trebleL * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidL = outSample;
            trebleL -= highmidL;
            outSample = (highmidL * midA[biq_a0]) + midA[biq_sL1];
            midA[biq_sL1] = (highmidL * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sL2];
            midA[biq_sL2] = (highmidL * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidL = outSample;
            highmidL -= lowmidL;
            outSample = (lowmidL * lowA[biq_a0]) + lowA[biq_sL1];
            lowA[biq_sL1] = (lowmidL * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sL2];
            lowA[biq_sL2] = (lowmidL * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassL = outSample;
            lowmidL -= bassL;
            trebleL = (bassL * bassGain) + (lowmidL * lowmidGain) + (highmidL * highmidGain) + (trebleL * trebleGain);
            // first stage of three crossovers
            outSample = (trebleL * highB[biq_a0]) + highB[biq_sL1];
            highB[biq_sL1] = (trebleL * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sL2];
            highB[biq_sL2] = (trebleL * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidL = outSample;
            trebleL -= highmidL;
            outSample = (highmidL * midB[biq_a0]) + midB[biq_sL1];
            midB[biq_sL1] = (highmidL * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sL2];
            midB[biq_sL2] = (highmidL * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidL = outSample;
            highmidL -= lowmidL;
            outSample = (lowmidL * lowB[biq_a0]) + lowB[biq_sL1];
            lowB[biq_sL1] = (lowmidL * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sL2];
            lowB[biq_sL2] = (lowmidL * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassL = outSample;
            lowmidL -= bassL;
            trebleL = (bassL * bassGain) + (lowmidL * lowmidGain) + (highmidL * highmidGain) + (trebleL * trebleGain);
            // second stage of three crossovers
            outSample = (trebleL * highC[biq_a0]) + highC[biq_sL1];
            highC[biq_sL1] = (trebleL * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sL2];
            highC[biq_sL2] = (trebleL * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidL = outSample;
            trebleL -= highmidL;
            outSample = (highmidL * midC[biq_a0]) + midC[biq_sL1];
            midC[biq_sL1] = (highmidL * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sL2];
            midC[biq_sL2] = (highmidL * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidL = outSample;
            highmidL -= lowmidL;
            outSample = (lowmidL * lowC[biq_a0]) + lowC[biq_sL1];
            lowC[biq_sL1] = (lowmidL * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sL2];
            lowC[biq_sL2] = (lowmidL * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassL = outSample;
            lowmidL -= bassL;
            trebleL = (bassL * bassGain) + (lowmidL * lowmidGain) + (highmidL * highmidGain) + (trebleL * trebleGain);
            // third stage of three crossovers
            highLIIR = (highLIIR * highCoef) + (trebleL * (1.0 - highCoef));
            highmidL = highLIIR;
            trebleL -= highmidL;
            midLIIR = (midLIIR * midCoef) + (highmidL * (1.0 - midCoef));
            lowmidL = midLIIR;
            highmidL -= lowmidL;
            lowLIIR = (lowLIIR * lowCoef) + (lowmidL * (1.0 - lowCoef));
            bassL = lowLIIR;
            lowmidL -= bassL;
            inputSampleL = (bassL * bassGain) + (lowmidL * lowmidGain) + (highmidL * highmidGain) + (trebleL * trebleGain);
            // fourth stage of three crossovers is the exponential filters
            double trebleR = inputSampleR;
            outSample = (trebleR * highA[biq_a0]) + highA[biq_sR1];
            highA[biq_sR1] = (trebleR * highA[biq_a1]) - (outSample * highA[biq_b1]) + highA[biq_sR2];
            highA[biq_sR2] = (trebleR * highA[biq_a2]) - (outSample * highA[biq_b2]);
            double highmidR = outSample;
            trebleR -= highmidR;
            outSample = (highmidR * midA[biq_a0]) + midA[biq_sR1];
            midA[biq_sR1] = (highmidR * midA[biq_a1]) - (outSample * midA[biq_b1]) + midA[biq_sR2];
            midA[biq_sR2] = (highmidR * midA[biq_a2]) - (outSample * midA[biq_b2]);
            double lowmidR = outSample;
            highmidR -= lowmidR;
            outSample = (lowmidR * lowA[biq_a0]) + lowA[biq_sR1];
            lowA[biq_sR1] = (lowmidR * lowA[biq_a1]) - (outSample * lowA[biq_b1]) + lowA[biq_sR2];
            lowA[biq_sR2] = (lowmidR * lowA[biq_a2]) - (outSample * lowA[biq_b2]);
            double bassR = outSample;
            lowmidR -= bassR;
            trebleR = (bassR * bassGain) + (lowmidR * lowmidGain) + (highmidR * highmidGain) + (trebleR * trebleGain);
            // first stage of three crossovers
            outSample = (trebleR * highB[biq_a0]) + highB[biq_sR1];
            highB[biq_sR1] = (trebleR * highB[biq_a1]) - (outSample * highB[biq_b1]) + highB[biq_sR2];
            highB[biq_sR2] = (trebleR * highB[biq_a2]) - (outSample * highB[biq_b2]);
            highmidR = outSample;
            trebleR -= highmidR;
            outSample = (highmidR * midB[biq_a0]) + midB[biq_sR1];
            midB[biq_sR1] = (highmidR * midB[biq_a1]) - (outSample * midB[biq_b1]) + midB[biq_sR2];
            midB[biq_sR2] = (highmidR * midB[biq_a2]) - (outSample * midB[biq_b2]);
            lowmidR = outSample;
            highmidR -= lowmidR;
            outSample = (lowmidR * lowB[biq_a0]) + lowB[biq_sR1];
            lowB[biq_sR1] = (lowmidR * lowB[biq_a1]) - (outSample * lowB[biq_b1]) + lowB[biq_sR2];
            lowB[biq_sR2] = (lowmidR * lowB[biq_a2]) - (outSample * lowB[biq_b2]);
            bassR = outSample;
            lowmidR -= bassR;
            trebleR = (bassR * bassGain) + (lowmidR * lowmidGain) + (highmidR * highmidGain) + (trebleR * trebleGain);
            // second stage of three crossovers
            outSample = (trebleR * highC[biq_a0]) + highC[biq_sR1];
            highC[biq_sR1] = (trebleR * highC[biq_a1]) - (outSample * highC[biq_b1]) + highC[biq_sR2];
            highC[biq_sR2] = (trebleR * highC[biq_a2]) - (outSample * highC[biq_b2]);
            highmidR = outSample;
            trebleR -= highmidR;
            outSample = (highmidR * midC[biq_a0]) + midC[biq_sR1];
            midC[biq_sR1] = (highmidR * midC[biq_a1]) - (outSample * midC[biq_b1]) + midC[biq_sR2];
            midC[biq_sR2] = (highmidR * midC[biq_a2]) - (outSample * midC[biq_b2]);
            lowmidR = outSample;
            highmidR -= lowmidR;
            outSample = (lowmidR * lowC[biq_a0]) + lowC[biq_sR1];
            lowC[biq_sR1] = (lowmidR * lowC[biq_a1]) - (outSample * lowC[biq_b1]) + lowC[biq_sR2];
            lowC[biq_sR2] = (lowmidR * lowC[biq_a2]) - (outSample * lowC[biq_b2]);
            bassR = outSample;
            lowmidR -= bassR;
            trebleR = (bassR * bassGain) + (lowmidR * lowmidGain) + (highmidR * highmidGain) + (trebleR * trebleGain);
            // third stage of three crossovers
            highRIIR = (highRIIR * highCoef) + (trebleR * (1.0 - highCoef));
            highmidR = highRIIR;
            trebleR -= highmidR;
            midRIIR = (midRIIR * midCoef) + (highmidR * (1.0 - midCoef));
            lowmidR = midRIIR;
            highmidR -= lowmidR;
            lowRIIR = (lowRIIR * lowCoef) + (lowmidR * (1.0 - lowCoef));
            bassR = lowRIIR;
            lowmidR -= bassR;
            inputSampleR = (bassR * bassGain) + (lowmidR * lowmidGain) + (highmidR * highmidGain) + (trebleR * trebleGain);
            // fourth stage of three crossovers is the exponential filters
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
} // namespace airwindohhs::smootheq2
