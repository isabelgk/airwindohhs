#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::consolehchannel {

constexpr std::string_view k_name{ "ConsoleHChannel" };
constexpr std::string_view k_short_description{
    "ConsoleHChannel is the Airwindows console for hip-hop and music reinvention."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class ConsoleHChannel final : public Effect<T>
{
    static constexpr int dscBuf = 256;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'chpc';
    float TRM;
    float MOR;
    float HIG;
    float MID;
    float LOW;
    float CRS;
    float TRF;
    float TRG;
    float TRB;
    float HMF;
    float HMG;
    float HMB;
    float LMF;
    float LMG;
    float LMB;
    float BSF;
    float BSG;
    float BSB;
    float THR;
    float ATK;
    float RLS;
    float GAT;
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
        biqs_freq,
        biqs_reso,
        biqs_level,
        biqs_temp,
        biqs_bit,
        biqs_a0,
        biqs_a1,
        biqs_b1,
        biqs_b2,
        biqs_c0,
        biqs_c1,
        biqs_d1,
        biqs_d2,
        biqs_aL1,
        biqs_aL2,
        biqs_aR1,
        biqs_aR2,
        biqs_cL1,
        biqs_cL2,
        biqs_cR1,
        biqs_cR2,
        biqs_outL,
        biqs_outR,
        biqs_total
    };
    double high[biqs_total];
    double hmid[biqs_total];
    double lmid[biqs_total];
    double bass[biqs_total];
    // HipCrush with four bands
    enum
    {
        bez_A,
        bez_B,
        bez_C,
        bez_Ctrl,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total];
    double bezMax;
    double bezMin;
    double bezGate;
    // Dynamics3
    double iirHPositionL[23];
    double iirHAngleL[23];
    double iirHPositionR[23];
    double iirHAngleR[23];
    bool hBypass;
    double iirLPositionL[15];
    double iirLAngleL[15];
    double iirLPositionR[15];
    double iirLAngleR[15];
    bool lBypass;
    double lFreqA;
    double lFreqB; // the lowpass
    double hFreqA;
    double hFreqB; // the highpass
    // Cabs2
    double dBaL[dscBuf + 5];
    double dBaPosL;
    int dBaXL;
    double dBaR[dscBuf + 5];
    double dBaPosR;
    int dBaXR;
    // Discontapeity
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
    double post32L[33];
    double post32R[33];
    double post16L[17];
    double post16R[17];
    double post8L[9];
    double post8R[9];
    double post4L[5];
    double post4R[5];
    double post2L[3];
    double post2R[3];
    double lastDarkL;
    double lastDarkR;
    int avgPos;
    // preTapeHack
    double panA;
    double panB;
    double inTrimA;
    double inTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ConsoleHChannel()
    {
        TRM = 0.25;
        MOR = 0.0;
        HIG = 0.5;
        MID = 0.5;
        LOW = 0.5;
        CRS = 0.0;
        TRF = 0.5;
        TRG = 0.0;
        TRB = 0.5;
        HMF = 0.5;
        HMG = 0.0;
        HMB = 0.5;
        LMF = 0.5;
        LMG = 0.0;
        LMB = 0.5;
        BSF = 0.5;
        BSG = 0.0;
        BSB = 0.5;
        THR = 1.0;
        ATK = 0.5;
        RLS = 0.5;
        GAT = 0.0;
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
        for (int x = 0; x < biqs_total; x++) {
            high[x] = 0.0;
            hmid[x] = 0.0;
            lmid[x] = 0.0;
            bass[x] = 0.0;
        }
        // HipCrush with four bands
        for (int x = 0; x < bez_total; x++) {
            bezComp[x] = 0.0;
        }
        bezComp[bez_cycle] = 1.0;
        bezMax = 0.0;
        bezMin = 0.0;
        bezGate = 2.0;
        // Dynamics3
        for (int count = 0; count < 22; count++) {
            iirHPositionL[count] = 0.0;
            iirHAngleL[count] = 0.0;
            iirHPositionR[count] = 0.0;
            iirHAngleR[count] = 0.0;
        }
        hBypass = false;
        for (int count = 0; count < 14; count++) {
            iirLPositionL[count] = 0.0;
            iirLAngleL[count] = 0.0;
            iirLPositionR[count] = 0.0;
            iirLAngleR[count] = 0.0;
        }
        lBypass = false;
        // Cabs2
        for (int count = 0; count < dscBuf + 2; count++) {
            dBaL[count] = 0.0;
            dBaR[count] = 0.0;
        }
        dBaPosL = 0.0;
        dBaPosR = 0.0;
        dBaXL = 1;
        dBaXR = 1;
        // Discontapeity
        for (int x = 0; x < 33; x++) {
            avg32L[x] = 0.0;
            post32L[x] = 0.0;
            avg32R[x] = 0.0;
            post32R[x] = 0.0;
        }
        for (int x = 0; x < 17; x++) {
            avg16L[x] = 0.0;
            post16L[x] = 0.0;
            avg16R[x] = 0.0;
            post16R[x] = 0.0;
        }
        for (int x = 0; x < 9; x++) {
            avg8L[x] = 0.0;
            post8L[x] = 0.0;
            avg8R[x] = 0.0;
            post8R[x] = 0.0;
        }
        for (int x = 0; x < 5; x++) {
            avg4L[x] = 0.0;
            post4L[x] = 0.0;
            avg4R[x] = 0.0;
            post4R[x] = 0.0;
        }
        for (int x = 0; x < 3; x++) {
            avg2L[x] = 0.0;
            post2L[x] = 0.0;
            avg2R[x] = 0.0;
            post2R[x] = 0.0;
        }
        avgPos = 0;
        lastDarkL = 0.0;
        lastDarkR = 0.0;
        // preTapeHack
        lFreqA = 1.0;
        lFreqB = 1.0;
        hFreqA = 0.0;
        hFreqB = 0.0;
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
        kParamI = 8,
        kParamJ = 9,
        kParamK = 10,
        kParamL = 11,
        kParamM = 12,
        kParamN = 13,
        kParamO = 14,
        kParamP = 15,
        kParamQ = 16,
        kParamR = 17,
        kParamS = 18,
        kParamT = 19,
        kParamU = 20,
        kParamV = 21,
        kParamW = 22,
        kParamX = 23,
        kParamY = 24,
        kParamZ = 25,
        kNumParameters = 26
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: TRM = value; break;
            case kParamB: MOR = value; break;
            case kParamC: HIG = value; break;
            case kParamD: MID = value; break;
            case kParamE: LOW = value; break;
            case kParamF: CRS = value; break;
            case kParamG: TRF = value; break;
            case kParamH: TRG = value; break;
            case kParamI: TRB = value; break;
            case kParamJ: HMF = value; break;
            case kParamK: HMG = value; break;
            case kParamL: HMB = value; break;
            case kParamM: LMF = value; break;
            case kParamN: LMG = value; break;
            case kParamO: LMB = value; break;
            case kParamP: BSF = value; break;
            case kParamQ: BSG = value; break;
            case kParamR: BSB = value; break;
            case kParamS: THR = value; break;
            case kParamT: ATK = value; break;
            case kParamU: RLS = value; break;
            case kParamV: GAT = value; break;
            case kParamW: LOP = value; break;
            case kParamX: HIP = value; break;
            case kParamY: PAN = value; break;
            case kParamZ: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return TRM; break;
            case kParamB: return MOR; break;
            case kParamC: return HIG; break;
            case kParamD: return MID; break;
            case kParamE: return LOW; break;
            case kParamF: return CRS; break;
            case kParamG: return TRF; break;
            case kParamH: return TRG; break;
            case kParamI: return TRB; break;
            case kParamJ: return HMF; break;
            case kParamK: return HMG; break;
            case kParamL: return HMB; break;
            case kParamM: return LMF; break;
            case kParamN: return LMG; break;
            case kParamO: return LMB; break;
            case kParamP: return BSF; break;
            case kParamQ: return BSG; break;
            case kParamR: return BSB; break;
            case kParamS: return THR; break;
            case kParamT: return ATK; break;
            case kParamU: return RLS; break;
            case kParamV: return GAT; break;
            case kParamW: return LOP; break;
            case kParamX: return HIP; break;
            case kParamY: return PAN; break;
            case kParamZ: return FAD; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.25; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.0; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.0; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 0.5; break;
            case kParamK: return 0.0; break;
            case kParamL: return 0.5; break;
            case kParamM: return 0.5; break;
            case kParamN: return 0.0; break;
            case kParamO: return 0.5; break;
            case kParamP: return 0.5; break;
            case kParamQ: return 0.0; break;
            case kParamR: return 0.5; break;
            case kParamS: return 1.0; break;
            case kParamT: return 0.5; break;
            case kParamU: return 0.5; break;
            case kParamV: return 0.0; break;
            case kParamW: return 1.0; break;
            case kParamX: return 0.0; break;
            case kParamY: return 0.5; break;
            case kParamZ: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "trim"; break;
            case kParamB: return "more"; break;
            case kParamC: return "high"; break;
            case kParamD: return "mid"; break;
            case kParamE: return "low"; break;
            case kParamF: return "crossfd"; break;
            case kParamG: return "trfreq"; break;
            case kParamH: return "treble"; break;
            case kParamI: return "trcrush"; break;
            case kParamJ: return "hmfreq"; break;
            case kParamK: return "highmid"; break;
            case kParamL: return "hmcrush"; break;
            case kParamM: return "lmfreq"; break;
            case kParamN: return "lowmid"; break;
            case kParamO: return "lmcrush"; break;
            case kParamP: return "bsfreq"; break;
            case kParamQ: return "bass"; break;
            case kParamR: return "bscrush"; break;
            case kParamS: return "thresh"; break;
            case kParamT: return "attack"; break;
            case kParamU: return "release"; break;
            case kParamV: return "gate"; break;
            case kParamW: return "lowpass"; break;
            case kParamX: return "hipass"; break;
            case kParamY: return "pan"; break;
            case kParamZ: return "fader"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Trim"; break;
            case kParamB: return "More"; break;
            case kParamC: return "High"; break;
            case kParamD: return "Mid"; break;
            case kParamE: return "Low"; break;
            case kParamF: return "CrossFd"; break;
            case kParamG: return "Tr Freq"; break;
            case kParamH: return "Treble"; break;
            case kParamI: return "TrCrush"; break;
            case kParamJ: return "HM Freq"; break;
            case kParamK: return "HighMid"; break;
            case kParamL: return "HMCrush"; break;
            case kParamM: return "LM Freq"; break;
            case kParamN: return "LowMid"; break;
            case kParamO: return "LMCrush"; break;
            case kParamP: return "Bs Freq"; break;
            case kParamQ: return "Bass"; break;
            case kParamR: return "BsCrush"; break;
            case kParamS: return "Thresh"; break;
            case kParamT: return "Attack"; break;
            case kParamU: return "Release"; break;
            case kParamV: return "Gate"; break;
            case kParamW: return "Lowpass"; break;
            case kParamX: return "Hipass"; break;
            case kParamY: return "Pan"; break;
            case kParamZ: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: int2string((int)(TRM * 4.0), text, kVstMaxParamStrLen); break;
            case kParamB: float2string(MOR, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(HIG, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(MID, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(LOW, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(CRS, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(TRF, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(TRG, text, kVstMaxParamStrLen); break;
            case kParamI: float2string(TRB, text, kVstMaxParamStrLen); break;
            case kParamJ: float2string(HMF, text, kVstMaxParamStrLen); break;
            case kParamK: float2string(HMG, text, kVstMaxParamStrLen); break;
            case kParamL: float2string(HMB, text, kVstMaxParamStrLen); break;
            case kParamM: float2string(LMF, text, kVstMaxParamStrLen); break;
            case kParamN: float2string(LMG, text, kVstMaxParamStrLen); break;
            case kParamO: float2string(LMB, text, kVstMaxParamStrLen); break;
            case kParamP: float2string(BSF, text, kVstMaxParamStrLen); break;
            case kParamQ: float2string(BSG, text, kVstMaxParamStrLen); break;
            case kParamR: float2string(BSB, text, kVstMaxParamStrLen); break;
            case kParamS: float2string(THR, text, kVstMaxParamStrLen); break;
            case kParamT: float2string(ATK, text, kVstMaxParamStrLen); break;
            case kParamU: float2string(RLS, text, kVstMaxParamStrLen); break;
            case kParamV: float2string(GAT, text, kVstMaxParamStrLen); break;
            case kParamW: float2string(LOP, text, kVstMaxParamStrLen); break;
            case kParamX: float2string(HIP, text, kVstMaxParamStrLen); break;
            case kParamY: float2string(PAN, text, kVstMaxParamStrLen); break;
            case kParamZ: float2string(FAD, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "eq", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "crush", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamK: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamL: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamM: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamN: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamO: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamP: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamQ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamR: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamS: vst_strncpy(text, "dyn", kVstMaxParamStrLen); break;
            case kParamT: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamU: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamV: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamW: vst_strncpy(text, "fltr", kVstMaxParamStrLen); break;
            case kParamX: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamY: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamZ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
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
        double moreTapeHack = (MOR * 2.0) + 1.0;
        bool tapehackOff = (MOR == 0.0);
        switch ((int)(TRM * 4.0)) {
            case 0: moreTapeHack *= 0.5; break;
            case 1: break;
            case 2: moreTapeHack *= 2.0; break;
            case 3: moreTapeHack *= 4.0; break;
            case 4: moreTapeHack *= 8.0; break;
        }
        double moreDiscontinuity = fmax(pow(MOR * 0.42, 3.0) * overallscale, 0.00001);
        // Discontapeity
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
        double crossFade = CRS;
        bool hipcrushOff = (crossFade == 0.0);
        if (!hipcrushOff) {
            high[biqs_freq] = (((pow(TRF, 2.0) * 16000.0) + 1000.0) / Effect<T>::getSampleRate());
            if (high[biqs_freq] < 0.0001) {
                high[biqs_freq] = 0.0001;
            }
            high[biqs_bit] = (TRB * 2.0) - 1.0;
            high[biqs_level] = (1.0 - pow(1.0 - TRG, 2.0)) * 1.618033988749894848204586;
            high[biqs_reso] = pow(TRG + 0.618033988749894848204586, 2.0);
            biqK = tan(M_PI * high[biqs_freq]);
            norm = 1.0 / (1.0 + biqK / (high[biqs_reso] * 0.618033988749894848204586) + biqK * biqK);
            high[biqs_a0] = biqK / (high[biqs_reso] * 0.618033988749894848204586) * norm;
            high[biqs_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            high[biqs_b2] = (1.0 - biqK / (high[biqs_reso] * 0.618033988749894848204586) + biqK * biqK) * norm;
            norm = 1.0 / (1.0 + biqK / (high[biqs_reso] * 1.618033988749894848204586) + biqK * biqK);
            high[biqs_c0] = biqK / (high[biqs_reso] * 1.618033988749894848204586) * norm;
            high[biqs_d1] = 2.0 * (biqK * biqK - 1.0) * norm;
            high[biqs_d2] = (1.0 - biqK / (high[biqs_reso] * 1.618033988749894848204586) + biqK * biqK) * norm;
            // high
            hmid[biqs_freq] = (((pow(HMF, 3.0) * 7000.0) + 300.0) / Effect<T>::getSampleRate());
            if (hmid[biqs_freq] < 0.0001) {
                hmid[biqs_freq] = 0.0001;
            }
            hmid[biqs_bit] = (HMB * 2.0) - 1.0;
            hmid[biqs_level] = (1.0 - pow(1.0 - HMG, 2.0)) * 1.618033988749894848204586;
            hmid[biqs_reso] = pow(HMG + 0.618033988749894848204586, 2.0);
            biqK = tan(M_PI * hmid[biqs_freq]);
            norm = 1.0 / (1.0 + biqK / (hmid[biqs_reso] * 0.618033988749894848204586) + biqK * biqK);
            hmid[biqs_a0] = biqK / (hmid[biqs_reso] * 0.618033988749894848204586) * norm;
            hmid[biqs_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            hmid[biqs_b2] = (1.0 - biqK / (hmid[biqs_reso] * 0.618033988749894848204586) + biqK * biqK) * norm;
            norm = 1.0 / (1.0 + biqK / (hmid[biqs_reso] * 1.618033988749894848204586) + biqK * biqK);
            hmid[biqs_c0] = biqK / (hmid[biqs_reso] * 1.618033988749894848204586) * norm;
            hmid[biqs_d1] = 2.0 * (biqK * biqK - 1.0) * norm;
            hmid[biqs_d2] = (1.0 - biqK / (hmid[biqs_reso] * 1.618033988749894848204586) + biqK * biqK) * norm;
            // hmid
            lmid[biqs_freq] = (((pow(LMF, 3.0) * 3000.0) + 40.0) / Effect<T>::getSampleRate());
            if (lmid[biqs_freq] < 0.00001) {
                lmid[biqs_freq] = 0.00001;
            }
            lmid[biqs_bit] = (LMB * 2.0) - 1.0;
            lmid[biqs_level] = (1.0 - pow(1.0 - LMG, 2.0)) * 1.618033988749894848204586;
            lmid[biqs_reso] = pow(LMG + 0.618033988749894848204586, 2.0);
            biqK = tan(M_PI * lmid[biqs_freq]);
            norm = 1.0 / (1.0 + biqK / (lmid[biqs_reso] * 0.618033988749894848204586) + biqK * biqK);
            lmid[biqs_a0] = biqK / (lmid[biqs_reso] * 0.618033988749894848204586) * norm;
            lmid[biqs_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lmid[biqs_b2] = (1.0 - biqK / (lmid[biqs_reso] * 0.618033988749894848204586) + biqK * biqK) * norm;
            norm = 1.0 / (1.0 + biqK / (lmid[biqs_reso] * 1.618033988749894848204586) + biqK * biqK);
            lmid[biqs_c0] = biqK / (lmid[biqs_reso] * 1.618033988749894848204586) * norm;
            lmid[biqs_d1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lmid[biqs_d2] = (1.0 - biqK / (lmid[biqs_reso] * 1.618033988749894848204586) + biqK * biqK) * norm;
            // lmid
            bass[biqs_freq] = (((pow(BSF, 4.0) * 1000.0) + 20.0) / Effect<T>::getSampleRate());
            if (bass[biqs_freq] < 0.00001) {
                bass[biqs_freq] = 0.00001;
            }
            bass[biqs_bit] = (BSB * 2.0) - 1.0;
            bass[biqs_level] = (1.0 - pow(1.0 - BSG, 2.0)) * 1.618033988749894848204586;
            bass[biqs_reso] = pow(BSG + 0.618033988749894848204586, 2.0);
            biqK = tan(M_PI * bass[biqs_freq]);
            norm = 1.0 / (1.0 + biqK / (bass[biqs_reso] * 0.618033988749894848204586) + biqK * biqK);
            bass[biqs_a0] = biqK / (bass[biqs_reso] * 0.618033988749894848204586) * norm;
            bass[biqs_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            bass[biqs_b2] = (1.0 - biqK / (bass[biqs_reso] * 0.618033988749894848204586) + biqK * biqK) * norm;
            norm = 1.0 / (1.0 + biqK / (bass[biqs_reso] * 1.618033988749894848204586) + biqK * biqK);
            bass[biqs_c0] = biqK / (bass[biqs_reso] * 1.618033988749894848204586) * norm;
            bass[biqs_d1] = 2.0 * (biqK * biqK - 1.0) * norm;
            bass[biqs_d2] = (1.0 - biqK / (bass[biqs_reso] * 1.618033988749894848204586) + biqK * biqK) * norm;
            // bass
        }
        // HipCrush with four bands
        double bezThresh = pow(1.0 - THR, 4.0) * 8.0;
        double bezRez = pow(1.0 - ATK, 4.0) / overallscale;
        double sloRez = pow(1.0 - RLS, 4.0) / overallscale;
        double gate = pow(GAT, 4.0);
        bezRez = fmin(fmax(bezRez, 0.0001), 1.0);
        sloRez = fmin(fmax(sloRez, 0.0001), 1.0);
        // Dynamics3
        lFreqA = lFreqB;
        lFreqB = pow(fmax(LOP, 0.002), overallscale); // the lowpass
        hFreqA = hFreqB;
        hFreqB = pow(HIP, overallscale + 2.0); // the highpass
        // Cabs2
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
            inputSampleL *= moreTapeHack;
            inputSampleR *= moreTapeHack;
            // trim control gets to work even when MORE is off
            if (!tapehackOff) {
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
                } // only update avgPos after the post-distortion filter stage
                double avgSlewL = fmin(fabs(lastDarkL - inputSampleL) * 0.12 * overallscale, 1.0);
                avgSlewL = 1.0 - (1.0 - avgSlewL * 1.0 - avgSlewL);
                inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
                lastDarkL = darkSampleL;
                double avgSlewR = fmin(fabs(lastDarkR - inputSampleR) * 0.12 * overallscale, 1.0);
                avgSlewR = 1.0 - (1.0 - avgSlewR * 1.0 - avgSlewR);
                inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
                lastDarkR = darkSampleR;
                // begin Discontinuity section
                inputSampleL *= moreDiscontinuity;
                dBaL[dBaXL] = inputSampleL;
                dBaPosL *= 0.5;
                dBaPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
                dBaPosL = fmin(dBaPosL, 1.0);
                int dBdly = floor(dBaPosL * dscBuf);
                double dBi = (dBaPosL * dscBuf) - dBdly;
                inputSampleL = dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
                dBdly++;
                inputSampleL += dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * dBi;
                dBaXL++;
                if (dBaXL < 0 || dBaXL >= dscBuf) {
                    dBaXL = 0;
                }
                inputSampleL /= moreDiscontinuity;
                // end Discontinuity section, begin TapeHack section
                inputSampleL = fmax(fmin(inputSampleL, 2.305929007734908), -2.305929007734908);
                double addtwo = inputSampleL * inputSampleL;
                double empower = inputSampleL * addtwo; // inputSampleL to the third power
                inputSampleL -= (empower / 6.0);
                empower *= addtwo; // to the fifth power
                inputSampleL += (empower / 69.0);
                empower *= addtwo; // seventh
                inputSampleL -= (empower / 2530.08);
                empower *= addtwo; // ninth
                inputSampleL += (empower / 224985.6);
                empower *= addtwo; // eleventh
                inputSampleL -= (empower / 9979200.0f);
                // this is a degenerate form of a Taylor Series to approximate sin()
                // end TapeHack section
                // begin Discontinuity section
                inputSampleR *= moreDiscontinuity;
                dBaR[dBaXR] = inputSampleR;
                dBaPosR *= 0.5;
                dBaPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
                dBaPosR = fmin(dBaPosR, 1.0);
                dBdly = floor(dBaPosR * dscBuf);
                dBi = (dBaPosR * dscBuf) - dBdly;
                inputSampleR = dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
                dBdly++;
                inputSampleR += dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * dBi;
                dBaXR++;
                if (dBaXR < 0 || dBaXR >= dscBuf) {
                    dBaXR = 0;
                }
                inputSampleR /= moreDiscontinuity;
                // end Discontinuity section, begin TapeHack section
                inputSampleR = fmax(fmin(inputSampleR, 2.305929007734908), -2.305929007734908);
                addtwo = inputSampleR * inputSampleR;
                empower = inputSampleR * addtwo; // inputSampleR to the third power
                inputSampleR -= (empower / 6.0);
                empower *= addtwo; // to the fifth power
                inputSampleR += (empower / 69.0);
                empower *= addtwo; // seventh
                inputSampleR -= (empower / 2530.08);
                empower *= addtwo; // ninth
                inputSampleR += (empower / 224985.6);
                empower *= addtwo; // eleventh
                inputSampleR -= (empower / 9979200.0f);
                // this is a degenerate form of a Taylor Series to approximate sin()
                // end TapeHack section
                // Discontapeity
                darkSampleL = inputSampleL;
                darkSampleR = inputSampleR;
                if (avgPos > 31) {
                    avgPos = 0;
                }
                if (spacing > 31) {
                    post32L[avgPos] = darkSampleL;
                    post32R[avgPos] = darkSampleR;
                    darkSampleL = 0.0;
                    darkSampleR = 0.0;
                    for (int x = 0; x < 32; x++) {
                        darkSampleL += post32L[x];
                        darkSampleR += post32R[x];
                    }
                    darkSampleL /= 32.0;
                    darkSampleR /= 32.0;
                }
                if (spacing > 15) {
                    post16L[avgPos % 16] = darkSampleL;
                    post16R[avgPos % 16] = darkSampleR;
                    darkSampleL = 0.0;
                    darkSampleR = 0.0;
                    for (int x = 0; x < 16; x++) {
                        darkSampleL += post16L[x];
                        darkSampleR += post16R[x];
                    }
                    darkSampleL /= 16.0;
                    darkSampleR /= 16.0;
                }
                if (spacing > 7) {
                    post8L[avgPos % 8] = darkSampleL;
                    post8R[avgPos % 8] = darkSampleR;
                    darkSampleL = 0.0;
                    darkSampleR = 0.0;
                    for (int x = 0; x < 8; x++) {
                        darkSampleL += post8L[x];
                        darkSampleR += post8R[x];
                    }
                    darkSampleL /= 8.0;
                    darkSampleR /= 8.0;
                }
                if (spacing > 3) {
                    post4L[avgPos % 4] = darkSampleL;
                    post4R[avgPos % 4] = darkSampleR;
                    darkSampleL = 0.0;
                    darkSampleR = 0.0;
                    for (int x = 0; x < 4; x++) {
                        darkSampleL += post4L[x];
                        darkSampleR += post4R[x];
                    }
                    darkSampleL /= 4.0;
                    darkSampleR /= 4.0;
                }
                if (spacing > 1) {
                    post2L[avgPos % 2] = darkSampleL;
                    post2R[avgPos % 2] = darkSampleR;
                    darkSampleL = 0.0;
                    darkSampleR = 0.0;
                    for (int x = 0; x < 2; x++) {
                        darkSampleL += post2L[x];
                        darkSampleR += post2R[x];
                    }
                    darkSampleL /= 2.0;
                    darkSampleR /= 2.0;
                }
                avgPos++;
                inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
                inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
                // use the previously calculated depth of the filter
            }
            double smoothEQL = inputSampleL;
            double smoothEQR = inputSampleR;
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
                smoothEQL = (bassFastL * bassGain) + (midFastL * midGain) + (trebleFastL * trebleGain);
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
                smoothEQR = (bassFastR * bassGain) + (midFastR * midGain) + (trebleFastR * trebleGain);
                // second stage of two crossovers is the exponential filters
                // this produces a slightly steeper Butterworth filter very cheaply
            }
            // SmoothEQ3
            double parametricL = 0.0;
            double parametricR = 0.0;
            if (!hipcrushOff) {
                // begin Stacked Biquad With Reversed Neutron Flow L
                high[biqs_outL] = inputSampleL * fabs(high[biqs_level]);
                high[biqs_temp] = (high[biqs_outL] * high[biqs_a0]) + high[biqs_aL1];
                high[biqs_aL1] = high[biqs_aL2] - (high[biqs_temp] * high[biqs_b1]);
                high[biqs_aL2] = (high[biqs_outL] * -high[biqs_a0]) - (high[biqs_temp] * high[biqs_b2]);
                high[biqs_outL] = high[biqs_temp];
                if (high[biqs_bit] != 0.0) {
                    double bitFactor = high[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    high[biqs_outL] *= bitFactor;
                    high[biqs_outL] = floor(high[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                    high[biqs_outL] /= bitFactor;
                }
                high[biqs_temp] = (high[biqs_outL] * high[biqs_c0]) + high[biqs_cL1];
                high[biqs_cL1] = high[biqs_cL2] - (high[biqs_temp] * high[biqs_d1]);
                high[biqs_cL2] = (high[biqs_outL] * -high[biqs_c0]) - (high[biqs_temp] * high[biqs_d2]);
                high[biqs_outL] = high[biqs_temp];
                high[biqs_outL] *= high[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow L
                // begin Stacked Biquad With Reversed Neutron Flow L
                hmid[biqs_outL] = inputSampleL * fabs(hmid[biqs_level]);
                hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_a0]) + hmid[biqs_aL1];
                hmid[biqs_aL1] = hmid[biqs_aL2] - (hmid[biqs_temp] * hmid[biqs_b1]);
                hmid[biqs_aL2] = (hmid[biqs_outL] * -hmid[biqs_a0]) - (hmid[biqs_temp] * hmid[biqs_b2]);
                hmid[biqs_outL] = hmid[biqs_temp];
                if (hmid[biqs_bit] != 0.0) {
                    double bitFactor = hmid[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    hmid[biqs_outL] *= bitFactor;
                    hmid[biqs_outL] = floor(hmid[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                    hmid[biqs_outL] /= bitFactor;
                }
                hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_c0]) + hmid[biqs_cL1];
                hmid[biqs_cL1] = hmid[biqs_cL2] - (hmid[biqs_temp] * hmid[biqs_d1]);
                hmid[biqs_cL2] = (hmid[biqs_outL] * -hmid[biqs_c0]) - (hmid[biqs_temp] * hmid[biqs_d2]);
                hmid[biqs_outL] = hmid[biqs_temp];
                hmid[biqs_outL] *= hmid[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow L
                // begin Stacked Biquad With Reversed Neutron Flow L
                lmid[biqs_outL] = inputSampleL * fabs(lmid[biqs_level]);
                lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_a0]) + lmid[biqs_aL1];
                lmid[biqs_aL1] = lmid[biqs_aL2] - (lmid[biqs_temp] * lmid[biqs_b1]);
                lmid[biqs_aL2] = (lmid[biqs_outL] * -lmid[biqs_a0]) - (lmid[biqs_temp] * lmid[biqs_b2]);
                lmid[biqs_outL] = lmid[biqs_temp];
                if (lmid[biqs_bit] != 0.0) {
                    double bitFactor = lmid[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    lmid[biqs_outL] *= bitFactor;
                    lmid[biqs_outL] = floor(lmid[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                    lmid[biqs_outL] /= bitFactor;
                }
                lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_c0]) + lmid[biqs_cL1];
                lmid[biqs_cL1] = lmid[biqs_cL2] - (lmid[biqs_temp] * lmid[biqs_d1]);
                lmid[biqs_cL2] = (lmid[biqs_outL] * -lmid[biqs_c0]) - (lmid[biqs_temp] * lmid[biqs_d2]);
                lmid[biqs_outL] = lmid[biqs_temp];
                lmid[biqs_outL] *= lmid[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow L
                // begin Stacked Biquad With Reversed Neutron Flow L
                bass[biqs_outL] = inputSampleL * fabs(bass[biqs_level]);
                bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_a0]) + bass[biqs_aL1];
                bass[biqs_aL1] = bass[biqs_aL2] - (bass[biqs_temp] * bass[biqs_b1]);
                bass[biqs_aL2] = (bass[biqs_outL] * -bass[biqs_a0]) - (bass[biqs_temp] * bass[biqs_b2]);
                bass[biqs_outL] = bass[biqs_temp];
                if (bass[biqs_bit] != 0.0) {
                    double bitFactor = bass[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    bass[biqs_outL] *= bitFactor;
                    bass[biqs_outL] = floor(bass[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                    bass[biqs_outL] /= bitFactor;
                }
                bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_c0]) + bass[biqs_cL1];
                bass[biqs_cL1] = bass[biqs_cL2] - (bass[biqs_temp] * bass[biqs_d1]);
                bass[biqs_cL2] = (bass[biqs_outL] * -bass[biqs_c0]) - (bass[biqs_temp] * bass[biqs_d2]);
                bass[biqs_outL] = bass[biqs_temp];
                bass[biqs_outL] *= bass[biqs_level];
                parametricL = high[biqs_outL] + hmid[biqs_outL] + lmid[biqs_outL] + bass[biqs_outL];
                // end Stacked Biquad With Reversed Neutron Flow L
                // begin Stacked Biquad With Reversed Neutron Flow R
                high[biqs_outR] = inputSampleR * fabs(high[biqs_level]);
                high[biqs_temp] = (high[biqs_outR] * high[biqs_a0]) + high[biqs_aR1];
                high[biqs_aR1] = high[biqs_aR2] - (high[biqs_temp] * high[biqs_b1]);
                high[biqs_aR2] = (high[biqs_outR] * -high[biqs_a0]) - (high[biqs_temp] * high[biqs_b2]);
                high[biqs_outR] = high[biqs_temp];
                if (high[biqs_bit] != 0.0) {
                    double bitFactor = high[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    high[biqs_outR] *= bitFactor;
                    high[biqs_outR] = floor(high[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                    high[biqs_outR] /= bitFactor;
                }
                high[biqs_temp] = (high[biqs_outR] * high[biqs_c0]) + high[biqs_cR1];
                high[biqs_cR1] = high[biqs_cR2] - (high[biqs_temp] * high[biqs_d1]);
                high[biqs_cR2] = (high[biqs_outR] * -high[biqs_c0]) - (high[biqs_temp] * high[biqs_d2]);
                high[biqs_outR] = high[biqs_temp];
                high[biqs_outR] *= high[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow R
                // begin Stacked Biquad With Reversed Neutron Flow R
                hmid[biqs_outR] = inputSampleR * fabs(hmid[biqs_level]);
                hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_a0]) + hmid[biqs_aR1];
                hmid[biqs_aR1] = hmid[biqs_aR2] - (hmid[biqs_temp] * hmid[biqs_b1]);
                hmid[biqs_aR2] = (hmid[biqs_outR] * -hmid[biqs_a0]) - (hmid[biqs_temp] * hmid[biqs_b2]);
                hmid[biqs_outR] = hmid[biqs_temp];
                if (hmid[biqs_bit] != 0.0) {
                    double bitFactor = hmid[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    hmid[biqs_outR] *= bitFactor;
                    hmid[biqs_outR] = floor(hmid[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                    hmid[biqs_outR] /= bitFactor;
                }
                hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_c0]) + hmid[biqs_cR1];
                hmid[biqs_cR1] = hmid[biqs_cR2] - (hmid[biqs_temp] * hmid[biqs_d1]);
                hmid[biqs_cR2] = (hmid[biqs_outR] * -hmid[biqs_c0]) - (hmid[biqs_temp] * hmid[biqs_d2]);
                hmid[biqs_outR] = hmid[biqs_temp];
                hmid[biqs_outR] *= hmid[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow R
                // begin Stacked Biquad With Reversed Neutron Flow R
                lmid[biqs_outR] = inputSampleR * fabs(lmid[biqs_level]);
                lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_a0]) + lmid[biqs_aR1];
                lmid[biqs_aR1] = lmid[biqs_aR2] - (lmid[biqs_temp] * lmid[biqs_b1]);
                lmid[biqs_aR2] = (lmid[biqs_outR] * -lmid[biqs_a0]) - (lmid[biqs_temp] * lmid[biqs_b2]);
                lmid[biqs_outR] = lmid[biqs_temp];
                if (lmid[biqs_bit] != 0.0) {
                    double bitFactor = lmid[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    lmid[biqs_outR] *= bitFactor;
                    lmid[biqs_outR] = floor(lmid[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                    lmid[biqs_outR] /= bitFactor;
                }
                lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_c0]) + lmid[biqs_cR1];
                lmid[biqs_cR1] = lmid[biqs_cR2] - (lmid[biqs_temp] * lmid[biqs_d1]);
                lmid[biqs_cR2] = (lmid[biqs_outR] * -lmid[biqs_c0]) - (lmid[biqs_temp] * lmid[biqs_d2]);
                lmid[biqs_outR] = lmid[biqs_temp];
                lmid[biqs_outR] *= lmid[biqs_level];
                // end Stacked Biquad With Reversed Neutron Flow R
                // begin Stacked Biquad With Reversed Neutron Flow R
                bass[biqs_outR] = inputSampleR * fabs(bass[biqs_level]);
                bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_a0]) + bass[biqs_aR1];
                bass[biqs_aR1] = bass[biqs_aR2] - (bass[biqs_temp] * bass[biqs_b1]);
                bass[biqs_aR2] = (bass[biqs_outR] * -bass[biqs_a0]) - (bass[biqs_temp] * bass[biqs_b2]);
                bass[biqs_outR] = bass[biqs_temp];
                if (bass[biqs_bit] != 0.0) {
                    double bitFactor = bass[biqs_bit];
                    bool crushGate = (bitFactor < 0.0);
                    bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                    bass[biqs_outR] *= bitFactor;
                    bass[biqs_outR] = floor(bass[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                    bass[biqs_outR] /= bitFactor;
                }
                bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_c0]) + bass[biqs_cR1];
                bass[biqs_cR1] = bass[biqs_cR2] - (bass[biqs_temp] * bass[biqs_d1]);
                bass[biqs_cR2] = (bass[biqs_outR] * -bass[biqs_c0]) - (bass[biqs_temp] * bass[biqs_d2]);
                bass[biqs_outR] = bass[biqs_temp];
                bass[biqs_outR] *= bass[biqs_level];
                parametricR = high[biqs_outR] + hmid[biqs_outR] + lmid[biqs_outR] + bass[biqs_outR];
                // end Stacked Biquad With Reversed Neutron Flow R
            }
            // end HipCrush as four band
            if (fmax(fabs(inputSampleL), fabs(inputSampleR)) > gate) {
                bezGate = overallscale / fmin(bezRez, sloRez);
            }
            else {
                bezGate = bezGate = fmax(0.000001, bezGate - fmin(bezRez, sloRez));
            }
            if (bezThresh > 0.0) {
                inputSampleL *= (bezThresh + 1.0);
                inputSampleR *= (bezThresh + 1.0);
                smoothEQL *= (bezThresh + 1.0);
                smoothEQR *= (bezThresh + 1.0);
                parametricL *= (bezThresh + 1.0);
                parametricR *= (bezThresh + 1.0);
            } // makeup gain
            double ctrl = fmax(fabs(inputSampleL), fabs(inputSampleR));
            bezMax = fmax(bezMax, ctrl);
            bezMin = fmax(bezMin - sloRez, ctrl);
            bezComp[bez_cycle] += bezRez;
            bezComp[bez_Ctrl] += (bezMin * bezRez);
            if (bezComp[bez_cycle] > 1.0) {
                if (bezGate < 1.0) {
                    bezComp[bez_Ctrl] /= bezGate;
                }
                bezComp[bez_cycle] -= 1.0;
                bezComp[bez_C] = bezComp[bez_B];
                bezComp[bez_B] = bezComp[bez_A];
                bezComp[bez_A] = bezComp[bez_Ctrl];
                bezComp[bez_Ctrl] = 0.0;
                bezMax = 0.0;
            }
            double CB = (bezComp[bez_C] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_B] * bezComp[bez_cycle]);
            double BA = (bezComp[bez_B] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_A] * bezComp[bez_cycle]);
            double CBA = (bezComp[bez_B] + (CB * (1.0 - bezComp[bez_cycle])) + (BA * bezComp[bez_cycle])) * 0.5;
            // switch over to the EQed or HipCrushed sound and compress
            inputSampleL = (smoothEQL * (1.0 - crossFade)) + (parametricL * crossFade);
            inputSampleR = (smoothEQR * (1.0 - crossFade)) + (parametricR * crossFade);
            if (bezThresh > 0.0) {
                inputSampleL *= 1.0 - (fmin(CBA * bezThresh, 1.0));
                inputSampleR *= 1.0 - (fmin(CBA * bezThresh, 1.0));
            }
            // Dynamics3, but with crossfade over EQ or HipCrush
            const double temp = (double)sampleFrames / inFramesToProcess;
            const double hFreq = (hFreqA * temp) + (hFreqB * (1.0 - temp));
            if (hFreq > 0.0) {
                double lowSampleL = inputSampleL;
                double lowSampleR = inputSampleR;
                for (int count = 0; count < 21; count++) {
                    iirHAngleL[count] = (iirHAngleL[count] * (1.0 - hFreq)) + ((lowSampleL - iirHPositionL[count]) * hFreq);
                    lowSampleL = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleL * hFreq);
                    iirHPositionL[count] = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleL * hFreq);
                    inputSampleL -= (lowSampleL * (1.0 / 21.0)); // left
                    iirHAngleR[count] = (iirHAngleR[count] * (1.0 - hFreq)) + ((lowSampleR - iirHPositionR[count]) * hFreq);
                    lowSampleR = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleR * hFreq);
                    iirHPositionR[count] = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleR * hFreq);
                    inputSampleR -= (lowSampleR * (1.0 / 21.0)); // right
                } // the highpass
                hBypass = false;
            }
            else {
                if (!hBypass) {
                    hBypass = true;
                    for (int count = 0; count < 22; count++) {
                        iirHPositionL[count] = 0.0;
                        iirHAngleL[count] = 0.0;
                        iirHPositionR[count] = 0.0;
                        iirHAngleR[count] = 0.0;
                    }
                } // blank out highpass if just switched off
            }
            const double lFreq = (lFreqA * temp) + (lFreqB * (1.0 - temp));
            if (lFreq < 1.0) {
                for (int count = 0; count < 13; count++) {
                    iirLAngleL[count] = (iirLAngleL[count] * (1.0 - lFreq)) + ((inputSampleL - iirLPositionL[count]) * lFreq);
                    inputSampleL = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleL * lFreq);
                    iirLPositionL[count] = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleL * lFreq); // left
                    iirLAngleR[count] = (iirLAngleR[count] * (1.0 - lFreq)) + ((inputSampleR - iirLPositionR[count]) * lFreq);
                    inputSampleR = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleR * lFreq);
                    iirLPositionR[count] = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleR * lFreq); // right
                } // the lowpass
                lBypass = false;
            }
            else {
                if (!lBypass) {
                    lBypass = true;
                    for (int count = 0; count < 14; count++) {
                        iirLPositionL[count] = 0.0;
                        iirLAngleL[count] = 0.0;
                        iirLPositionR[count] = 0.0;
                        iirLAngleR[count] = 0.0;
                    }
                } // blank out lowpass if just switched off
            }
            // Cabs2
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
            inputSampleL = inputSampleL * gainL * gain;
            inputSampleR = inputSampleR * gainR * gain;
            // applies pan section, and smoothed fader gain
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            else if (inputSampleL > 0.0) {
                inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895));
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            else if (inputSampleL < 0.0) {
                inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895));
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            else if (inputSampleR > 0.0) {
                inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895));
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            else if (inputSampleR < 0.0) {
                inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895));
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
} // namespace airwindohhs::consolehchannel
