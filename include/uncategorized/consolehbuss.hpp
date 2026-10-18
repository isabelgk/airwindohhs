#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::consolehbuss {

constexpr std::string_view k_name{ "ConsoleHBuss" };
constexpr std::string_view k_short_description{
    "ConsoleHBuss is the Airwindows console for hip-hop and music reinvention."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class ConsoleHBuss final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'chpb';
    float HIG;
    float MID;
    float LOW;
    float THR;
    float LOP;
    float HIP;
    float PAN;
    float FAD;
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
    }; // coefficient interpolating filter, stereo
    double highFast[biq_total];
    double lowFast[biq_total];
    double highFastLIIR;
    double highFastRIIR;
    double lowFastLIIR;
    double lowFastRIIR;
    // SmoothEQ3
    enum
    {
        bez_A,
        bez_B,
        bez_C,
        bez_Ctrl,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezCompF[bez_total];
    double bezCompS[bez_total];
    // Dynamics2 custom for buss
    enum
    {
        hilp_freq,
        hilp_temp,
        hilp_a0,
        hilp_aA0,
        hilp_aB0,
        hilp_a1,
        hilp_aA1,
        hilp_aB1,
        hilp_b1,
        hilp_bA1,
        hilp_bB1,
        hilp_b2,
        hilp_bA2,
        hilp_bB2,
        hilp_c0,
        hilp_cA0,
        hilp_cB0,
        hilp_c1,
        hilp_cA1,
        hilp_cB1,
        hilp_d1,
        hilp_dA1,
        hilp_dB1,
        hilp_d2,
        hilp_dA2,
        hilp_dB2,
        hilp_e0,
        hilp_eA0,
        hilp_eB0,
        hilp_e1,
        hilp_eA1,
        hilp_eB1,
        hilp_f1,
        hilp_fA1,
        hilp_fB1,
        hilp_f2,
        hilp_fA2,
        hilp_fB2,
        hilp_aL1,
        hilp_aL2,
        hilp_aR1,
        hilp_aR2,
        hilp_cL1,
        hilp_cL2,
        hilp_cR1,
        hilp_cR2,
        hilp_eL1,
        hilp_eL2,
        hilp_eR1,
        hilp_eR2,
        hilp_total
    };
    double highpass[hilp_total];
    double lowpass[hilp_total];
    double panA;
    double panB;
    double inTrimA;
    double inTrimB;
    double avg32L[33];
    double avg32R[33];
    double avg16L[17];
    double avg16R[17];
    double avg8L[9];
    double avg8R[9];
    double avg4L[5];
    double avg4R[5];
    double avg2L[3];
    double avg2R[3];
    int avgPos;
    double lastSlewL;
    double lastSlewR;
    double lastSlewpleL;
    double lastSlewpleR;
    // preTapeHack
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ConsoleHBuss()
    {
        HIG = 0.5;
        MID = 0.5;
        LOW = 0.5;
        THR = 1.0;
        LOP = 1.0;
        HIP = 0.0;
        PAN = 0.5;
        FAD = 0.5;
        for (int x = 0; x < biq_total; x++) {
            highFast[x] = 0.0;
            lowFast[x] = 0.0;
        }
        highFastLIIR = 0.0;
        highFastRIIR = 0.0;
        lowFastLIIR = 0.0;
        lowFastRIIR = 0.0;
        // SmoothEQ3
        for (int x = 0; x < bez_total; x++) {
            bezCompF[x] = 0.0;
            bezCompS[x] = 0.0;
        }
        bezCompF[bez_cycle] = 1.0;
        bezCompS[bez_cycle] = 1.0;
        // Dynamics2
        for (int x = 0; x < hilp_total; x++) {
            highpass[x] = 0.0;
            lowpass[x] = 0.0;
        }
        for (int x = 0; x < 33; x++) {
            avg32L[x] = 0.0;
            avg32R[x] = 0.0;
        }
        for (int x = 0; x < 17; x++) {
            avg16L[x] = 0.0;
            avg16R[x] = 0.0;
        }
        for (int x = 0; x < 9; x++) {
            avg8L[x] = 0.0;
            avg8R[x] = 0.0;
        }
        for (int x = 0; x < 5; x++) {
            avg4L[x] = 0.0;
            avg4R[x] = 0.0;
        }
        for (int x = 0; x < 3; x++) {
            avg2L[x] = 0.0;
            avg2R[x] = 0.0;
        }
        avgPos = 0;
        lastSlewL = 0.0;
        lastSlewR = 0.0;
        lastSlewpleL = 0.0;
        lastSlewpleR = 0.0;
        // preTapeHack
        panA = 0.5;
        panB = 0.5;
        inTrimA = 0.5;
        inTrimB = 0.5;
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
        switch (index) {
            case kParamA: HIG = value; break;
            case kParamB: MID = value; break;
            case kParamC: LOW = value; break;
            case kParamD: THR = value; break;
            case kParamE: LOP = value; break;
            case kParamF: HIP = value; break;
            case kParamG: PAN = value; break;
            case kParamH: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return HIG; break;
            case kParamB: return MID; break;
            case kParamC: return LOW; break;
            case kParamD: return THR; break;
            case kParamE: return LOP; break;
            case kParamF: return HIP; break;
            case kParamG: return PAN; break;
            case kParamH: return FAD; break;

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
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;
            case kParamF: return 0.0; break;
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
            case kParamB: return "mid"; break;
            case kParamC: return "low"; break;
            case kParamD: return "thresh"; break;
            case kParamE: return "lowpass"; break;
            case kParamF: return "hipass"; break;
            case kParamG: return "pan"; break;
            case kParamH: return "fader"; break;

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
            case kParamD: return "Thresh"; break;
            case kParamE: return "Lowpass"; break;
            case kParamF: return "Hipass"; break;
            case kParamG: return "Pan"; break;
            case kParamH: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(HIG, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(MID, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(LOW, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(THR, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(LOP, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(HIP, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(PAN, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(FAD, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "eq", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "dyn", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "fltr", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
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
        int spacing = floor(overallscale * 2.0);
        if (spacing < 2) {
            spacing = 2;
        }
        if (spacing > 32) {
            spacing = 32;
        }
        double trebleGain = (HIG - 0.5) * 2.0;
        trebleGain = 1.0 + (trebleGain * fabs(trebleGain) * fabs(trebleGain));
        double midGain = (MID - 0.5) * 2.0;
        midGain = 1.0 + (midGain * fabs(midGain) * fabs(midGain));
        double bassGain = (LOW - 0.5) * 2.0;
        bassGain = 1.0 + (bassGain * fabs(bassGain) * fabs(bassGain));
        // separate from filtering stage, this is amplitude, centered on 1.0 unity gain
        double highCoef = 0.0;
        double lowCoef = 0.0;
        double omega = 0.0;
        double biqK = 0.0;
        double norm = 0.0;
        bool eqOff = (trebleGain == 1.0 && midGain == 1.0 && bassGain == 1.0);
        // we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
        // it cancels out to bit-identical anyhow, but we get to skip the calculation
        if (!eqOff) {
            // SmoothEQ3 is how to get 3rd order steepness at very low CPU.
            // because sample rate varies, you could also vary the crossovers
            // you can't vary Q because math is simplified to take advantage of
            // how the accurate Q value for this filter is always exactly 1.0.
            highFast[biq_freq] = (4000.0 / Effect<T>::getSampleRate());
            omega = 2.0 * M_PI * (4000.0 / Effect<T>::getSampleRate()); // mid-high crossover freq
            biqK = 2.0 - cos(omega);
            highCoef = -sqrt(biqK * biqK - 1.0) + biqK;
            lowFast[biq_freq] = (200.0 / Effect<T>::getSampleRate());
            omega = 2.0 * M_PI * (200.0 / Effect<T>::getSampleRate()); // low-mid crossover freq
            biqK = 2.0 - cos(omega);
            lowCoef = -sqrt(biqK * biqK - 1.0) + biqK;
            // exponential IIR filter as part of an accurate 3rd order Butterworth filter
            biqK = tan(M_PI * highFast[biq_freq]);
            norm = 1.0 / (1.0 + biqK + biqK * biqK);
            highFast[biq_a0] = biqK * biqK * norm;
            highFast[biq_a1] = 2.0 * highFast[biq_a0];
            highFast[biq_a2] = highFast[biq_a0];
            highFast[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            highFast[biq_b2] = (1.0 - biqK + biqK * biqK) * norm;
            biqK = tan(M_PI * lowFast[biq_freq]);
            norm = 1.0 / (1.0 + biqK + biqK * biqK);
            lowFast[biq_a0] = biqK * biqK * norm;
            lowFast[biq_a1] = 2.0 * lowFast[biq_a0];
            lowFast[biq_a2] = lowFast[biq_a0];
            lowFast[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lowFast[biq_b2] = (1.0 - biqK + biqK * biqK) * norm;
            // custom biquad setup with Q = 1.0 gets to omit some divides
        }
        // SmoothEQ3
        double bezCThresh = pow(1.0 - THR, 6.0) * 8.0;
        double bezRez = pow(1.0 - THR, 12.360679774997898) / overallscale;
        double sloRez = pow(1.0 - THR, 10.0) / overallscale;
        sloRez = fmin(fmax(sloRez, 0.00001), 1.0);
        bezRez = fmin(fmax(bezRez, 0.00001), 1.0);
        // Dynamics2
        highpass[hilp_freq] = ((pow(HIP, 3) * 24000.0) + 10.0) / Effect<T>::getSampleRate();
        if (highpass[hilp_freq] > 0.495) {
            highpass[hilp_freq] = 0.495;
        }
        bool highpassEngage = true;
        if (HIP == 0.0) {
            highpassEngage = false;
        }
        lowpass[hilp_freq] = ((pow(LOP, 3) * 24000.0) + 10.0) / Effect<T>::getSampleRate();
        if (lowpass[hilp_freq] > 0.495) {
            lowpass[hilp_freq] = 0.495;
        }
        bool lowpassEngage = true;
        if (LOP == 1.0) {
            lowpassEngage = false;
        }
        highpass[hilp_aA0] = highpass[hilp_aB0];
        highpass[hilp_aA1] = highpass[hilp_aB1];
        highpass[hilp_bA1] = highpass[hilp_bB1];
        highpass[hilp_bA2] = highpass[hilp_bB2];
        highpass[hilp_cA0] = highpass[hilp_cB0];
        highpass[hilp_cA1] = highpass[hilp_cB1];
        highpass[hilp_dA1] = highpass[hilp_dB1];
        highpass[hilp_dA2] = highpass[hilp_dB2];
        highpass[hilp_eA0] = highpass[hilp_eB0];
        highpass[hilp_eA1] = highpass[hilp_eB1];
        highpass[hilp_fA1] = highpass[hilp_fB1];
        highpass[hilp_fA2] = highpass[hilp_fB2];
        lowpass[hilp_aA0] = lowpass[hilp_aB0];
        lowpass[hilp_aA1] = lowpass[hilp_aB1];
        lowpass[hilp_bA1] = lowpass[hilp_bB1];
        lowpass[hilp_bA2] = lowpass[hilp_bB2];
        lowpass[hilp_cA0] = lowpass[hilp_cB0];
        lowpass[hilp_cA1] = lowpass[hilp_cB1];
        lowpass[hilp_dA1] = lowpass[hilp_dB1];
        lowpass[hilp_dA2] = lowpass[hilp_dB2];
        lowpass[hilp_eA0] = lowpass[hilp_eB0];
        lowpass[hilp_eA1] = lowpass[hilp_eB1];
        lowpass[hilp_fA1] = lowpass[hilp_fB1];
        lowpass[hilp_fA2] = lowpass[hilp_fB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        // On the buss, highpass and lowpass are isolators meant to be moved,
        // so they are interpolated where the channels are not
        biqK = tan(M_PI * highpass[hilp_freq]); // highpass
        norm = 1.0 / (1.0 + biqK / 1.93185165 + biqK * biqK);
        highpass[hilp_aB0] = norm;
        highpass[hilp_aB1] = -2.0 * highpass[hilp_aB0];
        highpass[hilp_bB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highpass[hilp_bB2] = (1.0 - biqK / 1.93185165 + biqK * biqK) * norm;
        norm = 1.0 / (1.0 + biqK / 0.70710678 + biqK * biqK);
        highpass[hilp_cB0] = norm;
        highpass[hilp_cB1] = -2.0 * highpass[hilp_cB0];
        highpass[hilp_dB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highpass[hilp_dB2] = (1.0 - biqK / 0.70710678 + biqK * biqK) * norm;
        norm = 1.0 / (1.0 + biqK / 0.51763809 + biqK * biqK);
        highpass[hilp_eB0] = norm;
        highpass[hilp_eB1] = -2.0 * highpass[hilp_eB0];
        highpass[hilp_fB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        highpass[hilp_fB2] = (1.0 - biqK / 0.51763809 + biqK * biqK) * norm;
        biqK = tan(M_PI * lowpass[hilp_freq]); // lowpass
        norm = 1.0 / (1.0 + biqK / 1.93185165 + biqK * biqK);
        lowpass[hilp_aB0] = biqK * biqK * norm;
        lowpass[hilp_aB1] = 2.0 * lowpass[hilp_aB0];
        lowpass[hilp_bB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowpass[hilp_bB2] = (1.0 - biqK / 1.93185165 + biqK * biqK) * norm;
        norm = 1.0 / (1.0 + biqK / 0.70710678 + biqK * biqK);
        lowpass[hilp_cB0] = biqK * biqK * norm;
        lowpass[hilp_cB1] = 2.0 * lowpass[hilp_cB0];
        lowpass[hilp_dB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowpass[hilp_dB2] = (1.0 - biqK / 0.70710678 + biqK * biqK) * norm;
        norm = 1.0 / (1.0 + biqK / 0.51763809 + biqK * biqK);
        lowpass[hilp_eB0] = biqK * biqK * norm;
        lowpass[hilp_eB1] = 2.0 * lowpass[hilp_eB0];
        lowpass[hilp_fB1] = 2.0 * (biqK * biqK - 1.0) * norm;
        lowpass[hilp_fB2] = (1.0 - biqK / 0.51763809 + biqK * biqK) * norm;
        if (highpass[hilp_aA0] == 0.0) { // if we have just started, start directly with raw info
            highpass[hilp_aA0] = highpass[hilp_aB0];
            highpass[hilp_aA1] = highpass[hilp_aB1];
            highpass[hilp_bA1] = highpass[hilp_bB1];
            highpass[hilp_bA2] = highpass[hilp_bB2];
            highpass[hilp_cA0] = highpass[hilp_cB0];
            highpass[hilp_cA1] = highpass[hilp_cB1];
            highpass[hilp_dA1] = highpass[hilp_dB1];
            highpass[hilp_dA2] = highpass[hilp_dB2];
            highpass[hilp_eA0] = highpass[hilp_eB0];
            highpass[hilp_eA1] = highpass[hilp_eB1];
            highpass[hilp_fA1] = highpass[hilp_fB1];
            highpass[hilp_fA2] = highpass[hilp_fB2];
            lowpass[hilp_aA0] = lowpass[hilp_aB0];
            lowpass[hilp_aA1] = lowpass[hilp_aB1];
            lowpass[hilp_bA1] = lowpass[hilp_bB1];
            lowpass[hilp_bA2] = lowpass[hilp_bB2];
            lowpass[hilp_cA0] = lowpass[hilp_cB0];
            lowpass[hilp_cA1] = lowpass[hilp_cB1];
            lowpass[hilp_dA1] = lowpass[hilp_dB1];
            lowpass[hilp_dA2] = lowpass[hilp_dB2];
            lowpass[hilp_eA0] = lowpass[hilp_eB0];
            lowpass[hilp_eA1] = lowpass[hilp_eB1];
            lowpass[hilp_fA1] = lowpass[hilp_fB1];
            lowpass[hilp_fA2] = lowpass[hilp_fB2];
        }
        panA = panB;
        panB = PAN * 1.57079633;
        inTrimA = inTrimB;
        inTrimB = FAD * 2.0;
        // Console
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
            highpass[hilp_a0] = (highpass[hilp_aA0] * temp) + (highpass[hilp_aB0] * (1.0 - temp));
            highpass[hilp_a1] = (highpass[hilp_aA1] * temp) + (highpass[hilp_aB1] * (1.0 - temp));
            highpass[hilp_b1] = (highpass[hilp_bA1] * temp) + (highpass[hilp_bB1] * (1.0 - temp));
            highpass[hilp_b2] = (highpass[hilp_bA2] * temp) + (highpass[hilp_bB2] * (1.0 - temp));
            highpass[hilp_c0] = (highpass[hilp_cA0] * temp) + (highpass[hilp_cB0] * (1.0 - temp));
            highpass[hilp_c1] = (highpass[hilp_cA1] * temp) + (highpass[hilp_cB1] * (1.0 - temp));
            highpass[hilp_d1] = (highpass[hilp_dA1] * temp) + (highpass[hilp_dB1] * (1.0 - temp));
            highpass[hilp_d2] = (highpass[hilp_dA2] * temp) + (highpass[hilp_dB2] * (1.0 - temp));
            highpass[hilp_e0] = (highpass[hilp_eA0] * temp) + (highpass[hilp_eB0] * (1.0 - temp));
            highpass[hilp_e1] = (highpass[hilp_eA1] * temp) + (highpass[hilp_eB1] * (1.0 - temp));
            highpass[hilp_f1] = (highpass[hilp_fA1] * temp) + (highpass[hilp_fB1] * (1.0 - temp));
            highpass[hilp_f2] = (highpass[hilp_fA2] * temp) + (highpass[hilp_fB2] * (1.0 - temp));
            lowpass[hilp_a0] = (lowpass[hilp_aA0] * temp) + (lowpass[hilp_aB0] * (1.0 - temp));
            lowpass[hilp_a1] = (lowpass[hilp_aA1] * temp) + (lowpass[hilp_aB1] * (1.0 - temp));
            lowpass[hilp_b1] = (lowpass[hilp_bA1] * temp) + (lowpass[hilp_bB1] * (1.0 - temp));
            lowpass[hilp_b2] = (lowpass[hilp_bA2] * temp) + (lowpass[hilp_bB2] * (1.0 - temp));
            lowpass[hilp_c0] = (lowpass[hilp_cA0] * temp) + (lowpass[hilp_cB0] * (1.0 - temp));
            lowpass[hilp_c1] = (lowpass[hilp_cA1] * temp) + (lowpass[hilp_cB1] * (1.0 - temp));
            lowpass[hilp_d1] = (lowpass[hilp_dA1] * temp) + (lowpass[hilp_dB1] * (1.0 - temp));
            lowpass[hilp_d2] = (lowpass[hilp_dA2] * temp) + (lowpass[hilp_dB2] * (1.0 - temp));
            lowpass[hilp_e0] = (lowpass[hilp_eA0] * temp) + (lowpass[hilp_eB0] * (1.0 - temp));
            lowpass[hilp_e1] = (lowpass[hilp_eA1] * temp) + (lowpass[hilp_eB1] * (1.0 - temp));
            lowpass[hilp_f1] = (lowpass[hilp_fA1] * temp) + (lowpass[hilp_fB1] * (1.0 - temp));
            lowpass[hilp_f2] = (lowpass[hilp_fA2] * temp) + (lowpass[hilp_fB2] * (1.0 - temp));
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_a0]) + highpass[hilp_aL1];
                highpass[hilp_aL1] = (inputSampleL * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aL2];
                highpass[hilp_aL2] = (inputSampleL * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_a0]) + highpass[hilp_aR1];
                highpass[hilp_aR1] = (inputSampleR * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aR2];
                highpass[hilp_aR2] = (inputSampleR * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_aR1] = highpass[hilp_aR2] = highpass[hilp_aL1] = highpass[hilp_aL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_a0]) + lowpass[hilp_aL1];
                lowpass[hilp_aL1] = (inputSampleL * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aL2];
                lowpass[hilp_aL2] = (inputSampleL * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_a0]) + lowpass[hilp_aR1];
                lowpass[hilp_aR1] = (inputSampleR * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aR2];
                lowpass[hilp_aR2] = (inputSampleR * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_aR1] = lowpass[hilp_aR2] = lowpass[hilp_aL1] = lowpass[hilp_aL2] = 0.0;
            }
            // first Highpass/Lowpass blocks aliasing before the nonlinearity of ConsoleXBuss and Parametric
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            else if (inputSampleL > 0.0) {
                inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949));
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            else if (inputSampleL < 0.0) {
                inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949));
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            else if (inputSampleR > 0.0) {
                inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949));
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            else if (inputSampleR < 0.0) {
                inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949));
            }
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_c0]) + highpass[hilp_cL1];
                highpass[hilp_cL1] = (inputSampleL * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cL2];
                highpass[hilp_cL2] = (inputSampleL * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_c0]) + highpass[hilp_cR1];
                highpass[hilp_cR1] = (inputSampleR * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cR2];
                highpass[hilp_cR2] = (inputSampleR * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_cR1] = highpass[hilp_cR2] = highpass[hilp_cL1] = highpass[hilp_cL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_c0]) + lowpass[hilp_cL1];
                lowpass[hilp_cL1] = (inputSampleL * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cL2];
                lowpass[hilp_cL2] = (inputSampleL * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_c0]) + lowpass[hilp_cR1];
                lowpass[hilp_cR1] = (inputSampleR * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cR2];
                lowpass[hilp_cR2] = (inputSampleR * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_cR1] = lowpass[hilp_cR2] = lowpass[hilp_cL1] = lowpass[hilp_cL2] = 0.0;
            }
            // another stage of Highpass/Lowpass before bringing in the parametric bands
            if (!eqOff) {
                double trebleFastL = inputSampleL;
                double outSample = (trebleFastL * highFast[biq_a0]) + highFast[biq_sL1];
                highFast[biq_sL1] = (trebleFastL * highFast[biq_a1]) - (outSample * highFast[biq_b1]) + highFast[biq_sL2];
                highFast[biq_sL2] = (trebleFastL * highFast[biq_a2]) - (outSample * highFast[biq_b2]);
                double midFastL = outSample;
                trebleFastL -= midFastL;
                outSample = (midFastL * lowFast[biq_a0]) + lowFast[biq_sL1];
                lowFast[biq_sL1] = (midFastL * lowFast[biq_a1]) - (outSample * lowFast[biq_b1]) + lowFast[biq_sL2];
                lowFast[biq_sL2] = (midFastL * lowFast[biq_a2]) - (outSample * lowFast[biq_b2]);
                double bassFastL = outSample;
                midFastL -= bassFastL;
                trebleFastL = (bassFastL * bassGain) + (midFastL * midGain) + (trebleFastL * trebleGain);
                // first stage of two crossovers is biquad of exactly 1.0 Q
                highFastLIIR = (highFastLIIR * highCoef) + (trebleFastL * (1.0 - highCoef));
                midFastL = highFastLIIR;
                trebleFastL -= midFastL;
                lowFastLIIR = (lowFastLIIR * lowCoef) + (midFastL * (1.0 - lowCoef));
                bassFastL = lowFastLIIR;
                midFastL -= bassFastL;
                inputSampleL = (bassFastL * bassGain) + (midFastL * midGain) + (trebleFastL * trebleGain);
                // second stage of two crossovers is the exponential filters
                // this produces a slightly steeper Butterworth filter very cheaply
                double trebleFastR = inputSampleR;
                outSample = (trebleFastR * highFast[biq_a0]) + highFast[biq_sR1];
                highFast[biq_sR1] = (trebleFastR * highFast[biq_a1]) - (outSample * highFast[biq_b1]) + highFast[biq_sR2];
                highFast[biq_sR2] = (trebleFastR * highFast[biq_a2]) - (outSample * highFast[biq_b2]);
                double midFastR = outSample;
                trebleFastR -= midFastR;
                outSample = (midFastR * lowFast[biq_a0]) + lowFast[biq_sR1];
                lowFast[biq_sR1] = (midFastR * lowFast[biq_a1]) - (outSample * lowFast[biq_b1]) + lowFast[biq_sR2];
                lowFast[biq_sR2] = (midFastR * lowFast[biq_a2]) - (outSample * lowFast[biq_b2]);
                double bassFastR = outSample;
                midFastR -= bassFastR;
                trebleFastR = (bassFastR * bassGain) + (midFastR * midGain) + (trebleFastR * trebleGain);
                // first stage of two crossovers is biquad of exactly 1.0 Q
                highFastRIIR = (highFastRIIR * highCoef) + (trebleFastR * (1.0 - highCoef));
                midFastR = highFastRIIR;
                trebleFastR -= midFastR;
                lowFastRIIR = (lowFastRIIR * lowCoef) + (midFastR * (1.0 - lowCoef));
                bassFastR = lowFastRIIR;
                midFastR -= bassFastR;
                inputSampleR = (bassFastR * bassGain) + (midFastR * midGain) + (trebleFastR * trebleGain);
                // second stage of two crossovers is the exponential filters
                // this produces a slightly steeper Butterworth filter very cheaply
            }
            // SmoothEQ3
            if (bezCThresh > 0.0) {
                inputSampleL *= ((bezCThresh * 0.5) + 1.0);
                inputSampleR *= ((bezCThresh * 0.5) + 1.0);
                bezCompF[bez_cycle] += bezRez;
                bezCompF[bez_Ctrl] += (fmax(fabs(inputSampleL), fabs(inputSampleR)) * bezRez);
                if (bezCompF[bez_cycle] > 1.0) {
                    bezCompF[bez_cycle] -= 1.0;
                    bezCompF[bez_C] = bezCompF[bez_B];
                    bezCompF[bez_B] = bezCompF[bez_A];
                    bezCompF[bez_A] = bezCompF[bez_Ctrl];
                    bezCompF[bez_Ctrl] = 0.0;
                }
                bezCompS[bez_cycle] += sloRez;
                bezCompS[bez_Ctrl] += (fmax(fabs(inputSampleL), fabs(inputSampleR)) * sloRez);
                if (bezCompS[bez_cycle] > 1.0) {
                    bezCompS[bez_cycle] -= 1.0;
                    bezCompS[bez_C] = bezCompS[bez_B];
                    bezCompS[bez_B] = bezCompS[bez_A];
                    bezCompS[bez_A] = bezCompS[bez_Ctrl];
                    bezCompS[bez_Ctrl] = 0.0;
                }
                double CBF = (bezCompF[bez_C] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_B] * bezCompF[bez_cycle]);
                double BAF = (bezCompF[bez_B] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_A] * bezCompF[bez_cycle]);
                double CBAF = (bezCompF[bez_B] + (CBF * (1.0 - bezCompF[bez_cycle])) + (BAF * bezCompF[bez_cycle])) * 0.5;
                double CBS = (bezCompS[bez_C] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_B] * bezCompS[bez_cycle]);
                double BAS = (bezCompS[bez_B] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_A] * bezCompS[bez_cycle]);
                double CBAS = (bezCompS[bez_B] + (CBS * (1.0 - bezCompS[bez_cycle])) + (BAS * bezCompS[bez_cycle])) * 0.5;
                double CBAMax = fmax(CBAS, CBAF);
                if (CBAMax > 0.0) {
                    CBAMax = 1.0 / CBAMax;
                }
                double CBAFade = ((CBAS * -CBAMax) + (CBAF * CBAMax) + 1.0) * 0.5;
                inputSampleL *= 1.0 - (fmin(((CBAS * (1.0 - CBAFade)) + (CBAF * CBAFade)) * bezCThresh, 1.0));
                inputSampleR *= 1.0 - (fmin(((CBAS * (1.0 - CBAFade)) + (CBAF * CBAFade)) * bezCThresh, 1.0));
            }
            else {
                bezCompF[bez_Ctrl] = 0.0;
                bezCompS[bez_Ctrl] = 0.0;
            }
            // Dynamics2 custom version for buss
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_e0]) + highpass[hilp_eL1];
                highpass[hilp_eL1] = (inputSampleL * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eL2];
                highpass[hilp_eL2] = (inputSampleL * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_e0]) + highpass[hilp_eR1];
                highpass[hilp_eR1] = (inputSampleR * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eR2];
                highpass[hilp_eR2] = (inputSampleR * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_eR1] = highpass[hilp_eR2] = highpass[hilp_eL1] = highpass[hilp_eL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_e0]) + lowpass[hilp_eL1];
                lowpass[hilp_eL1] = (inputSampleL * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eL2];
                lowpass[hilp_eL2] = (inputSampleL * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_e0]) + lowpass[hilp_eR1];
                lowpass[hilp_eR1] = (inputSampleR * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eR2];
                lowpass[hilp_eR2] = (inputSampleR * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_eR1] = lowpass[hilp_eR2] = lowpass[hilp_eL1] = lowpass[hilp_eL2] = 0.0;
            }
            // final Highpass/Lowpass continues to address aliasing
            // final stacked biquad section is the softest Q for smoothness
            double gainR = (panA * temp) + (panB * (1.0 - temp));
            double gainL = 1.57079633 - gainR;
            gainR = sin(gainR);
            gainL = sin(gainL);
            double gain = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            if (gain > 1.0) {
                gain *= gain;
            }
            if (gain < 1.0) {
                gain = 1.0 - pow(1.0 - gain, 2);
            }
            gain *= 2.0;
            inputSampleL = inputSampleL * gainL * gain;
            inputSampleR = inputSampleR * gainR * gain;
            // applies pan section, and smoothed fader gain
            double darkSampleL = inputSampleL;
            double darkSampleR = inputSampleR;
            if (avgPos > 31) {
                avgPos = 0;
            }
            if (spacing > 31) {
                avg32L[avgPos] = darkSampleL;
                avg32R[avgPos] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {
                    darkSampleL += avg32L[x];
                    darkSampleR += avg32R[x];
                }
                darkSampleL /= 32.0;
                darkSampleR /= 32.0;
            }
            if (spacing > 15) {
                avg16L[avgPos % 16] = darkSampleL;
                avg16R[avgPos % 16] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {
                    darkSampleL += avg16L[x];
                    darkSampleR += avg16R[x];
                }
                darkSampleL /= 16.0;
                darkSampleR /= 16.0;
            }
            if (spacing > 7) {
                avg8L[avgPos % 8] = darkSampleL;
                avg8R[avgPos % 8] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {
                    darkSampleL += avg8L[x];
                    darkSampleR += avg8R[x];
                }
                darkSampleL /= 8.0;
                darkSampleR /= 8.0;
            }
            if (spacing > 3) {
                avg4L[avgPos % 4] = darkSampleL;
                avg4R[avgPos % 4] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {
                    darkSampleL += avg4L[x];
                    darkSampleR += avg4R[x];
                }
                darkSampleL /= 4.0;
                darkSampleR /= 4.0;
            }
            if (spacing > 1) {
                avg2L[avgPos % 2] = darkSampleL;
                avg2R[avgPos % 2] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {
                    darkSampleL += avg2L[x];
                    darkSampleR += avg2R[x];
                }
                darkSampleL /= 2.0;
                darkSampleR /= 2.0;
            }
            avgPos++;
            lastSlewL += fabs(lastSlewpleL - inputSampleL);
            lastSlewpleL = inputSampleL;
            double avgSlewL = fmin(lastSlewL * lastSlewL * (0.0635 - (overallscale * 0.0018436)), 1.0);
            lastSlewL = fmax(lastSlewL * 0.78, 2.39996322972865332223);
            lastSlewR += fabs(lastSlewpleR - inputSampleR);
            lastSlewpleR = inputSampleR;
            double avgSlewR = fmin(lastSlewR * lastSlewR * (0.0635 - (overallscale * 0.0018436)), 1.0);
            lastSlewR = fmax(lastSlewR * 0.78, 2.39996322972865332223); // look up Golden Angle, it's cool
            inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
            inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
            inputSampleL = fmin(fmax(inputSampleL, -2.032610446872596), 2.032610446872596);
            long double X = inputSampleL * inputSampleL;
            long double sat = inputSampleL * X;
            inputSampleL -= (sat * 0.125);
            sat *= X;
            inputSampleL += (sat * 0.0078125);
            sat *= X;
            inputSampleL -= (sat * 0.000244140625);
            sat *= X;
            inputSampleL += (sat * 0.000003814697265625);
            sat *= X;
            inputSampleL -= (sat * 0.0000000298023223876953125);
            sat *= X;
            // purestsaturation: sine, except all the corrections
            // retain mantissa of a long double increasing power function
            inputSampleR = fmin(fmax(inputSampleR, -2.032610446872596), 2.032610446872596);
            X = inputSampleR * inputSampleR;
            sat = inputSampleR * X;
            inputSampleR -= (sat * 0.125);
            sat *= X;
            inputSampleR += (sat * 0.0078125);
            sat *= X;
            inputSampleR -= (sat * 0.000244140625);
            sat *= X;
            inputSampleR += (sat * 0.000003814697265625);
            sat *= X;
            inputSampleR -= (sat * 0.0000000298023223876953125);
            sat *= X;
            // purestsaturation: sine, except all the corrections
            // retain mantissa of a long double increasing power function
            // we are leaving it as a clip that will go over 0dB.
            // it is a softclip so it will give you a more forgiving experience,
            // but you are meant to not drive the softclip for just level.
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
} // namespace airwindohhs::consolehbuss
