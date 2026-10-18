#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::consolex2pre {

constexpr std::string_view k_name{ "ConsoleX2Pre" };
constexpr std::string_view k_short_description{
    ""
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class ConsoleX2Pre final : public Effect<T>
{
    static constexpr int dscBuf = 256;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'cx2p';
    float TRM;
    float MOR;
    float HIG;
    float HMG;
    float LMG;
    float BSG;
    float HIF;
    float HMF;
    float LMF;
    float BSF;
    float THR;
    float ATK;
    float RLS;
    float GAT;
    float LOP;
    float HIP;
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
    // SmoothEQ2
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_CtrlL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_CtrlR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total];
    double bezMaxL;
    double bezMinL;
    double bezGateL;
    double bezMaxR;
    double bezMinR;
    double bezGateR;
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
    double inTrimA;
    double inTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ConsoleX2Pre()
    {
        TRM = 0.25;
        MOR = 0.0;
        HIG = 0.5;
        HMG = 0.5;
        LMG = 0.5;
        BSG = 0.5;
        HIF = 0.5;
        HMF = 0.5;
        LMF = 0.5;
        BSF = 0.5;
        THR = 1.0;
        ATK = 0.5;
        RLS = 0.5;
        GAT = 0.0;
        LOP = 1.0;
        HIP = 0.0;
        FAD = 0.5;
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
        // SmoothEQ2
        for (int x = 0; x < bez_total; x++) {
            bezComp[x] = 0.0;
        }
        bezComp[bez_cycle] = 1.0;
        bezMaxL = 0.0;
        bezMinL = 0.0;
        bezGateL = 2.0;
        bezMaxR = 0.0;
        bezMinR = 0.0;
        bezGateR = 2.0; // Dual mono version
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
        kNumParameters = 17
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: TRM = value; break;
            case kParamB: MOR = value; break;
            case kParamC: HIG = value; break;
            case kParamD: HMG = value; break;
            case kParamE: LMG = value; break;
            case kParamF: BSG = value; break;
            case kParamG: HIF = value; break;
            case kParamH: HMF = value; break;
            case kParamI: LMF = value; break;
            case kParamJ: BSF = value; break;
            case kParamK: THR = value; break;
            case kParamL: ATK = value; break;
            case kParamM: RLS = value; break;
            case kParamN: GAT = value; break;
            case kParamO: LOP = value; break;
            case kParamP: HIP = value; break;
            case kParamQ: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return TRM; break;
            case kParamB: return MOR; break;
            case kParamC: return HIG; break;
            case kParamD: return HMG; break;
            case kParamE: return LMG; break;
            case kParamF: return BSG; break;
            case kParamG: return HIF; break;
            case kParamH: return HMF; break;
            case kParamI: return LMF; break;
            case kParamJ: return BSF; break;
            case kParamK: return THR; break;
            case kParamL: return ATK; break;
            case kParamM: return RLS; break;
            case kParamN: return GAT; break;
            case kParamO: return LOP; break;
            case kParamP: return HIP; break;
            case kParamQ: return FAD; break;

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
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 0.5; break;
            case kParamK: return 1.0; break;
            case kParamL: return 0.5; break;
            case kParamM: return 0.5; break;
            case kParamN: return 0.0; break;
            case kParamO: return 1.0; break;
            case kParamP: return 0.0; break;
            case kParamQ: return 0.5; break;

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
            case kParamD: return "hmid"; break;
            case kParamE: return "lmid"; break;
            case kParamF: return "bass"; break;
            case kParamG: return "highf"; break;
            case kParamH: return "hmidf"; break;
            case kParamI: return "lmidf"; break;
            case kParamJ: return "bassf"; break;
            case kParamK: return "thresh"; break;
            case kParamL: return "attack"; break;
            case kParamM: return "release"; break;
            case kParamN: return "gate"; break;
            case kParamO: return "lowpass"; break;
            case kParamP: return "hipass"; break;
            case kParamQ: return "fader"; break;

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
            case kParamD: return "HMid"; break;
            case kParamE: return "LMid"; break;
            case kParamF: return "Bass"; break;
            case kParamG: return "HighF"; break;
            case kParamH: return "HMidF"; break;
            case kParamI: return "LMidF"; break;
            case kParamJ: return "BassF"; break;
            case kParamK: return "Thresh"; break;
            case kParamL: return "Attack"; break;
            case kParamM: return "Release"; break;
            case kParamN: return "Gate"; break;
            case kParamO: return "Lowpass"; break;
            case kParamP: return "Hipass"; break;
            case kParamQ: return "Fader"; break;

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
            case kParamD: float2string(HMG, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(LMG, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(BSG, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(HIF, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(HMF, text, kVstMaxParamStrLen); break;
            case kParamI: float2string(LMF, text, kVstMaxParamStrLen); break;
            case kParamJ: float2string(BSF, text, kVstMaxParamStrLen); break;
            case kParamK: float2string(THR, text, kVstMaxParamStrLen); break;
            case kParamL: float2string(ATK, text, kVstMaxParamStrLen); break;
            case kParamM: float2string(RLS, text, kVstMaxParamStrLen); break;
            case kParamN: float2string(GAT, text, kVstMaxParamStrLen); break;
            case kParamO: float2string(LOP, text, kVstMaxParamStrLen); break;
            case kParamP: float2string(HIP, text, kVstMaxParamStrLen); break;
            case kParamQ: float2string(FAD, text, kVstMaxParamStrLen); break;
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
            case kParamG: vst_strncpy(text, "freq", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamK: vst_strncpy(text, "dyn", kVstMaxParamStrLen); break;
            case kParamL: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamM: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamN: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamO: vst_strncpy(text, "fltr", kVstMaxParamStrLen); break;
            case kParamP: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamQ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
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
        double highmidGain = (HMG - 0.5) * 2.0;
        highmidGain = 1.0 + (highmidGain * fabs(highmidGain) * fabs(highmidGain));
        double lowmidGain = (LMG - 0.5) * 2.0;
        lowmidGain = 1.0 + (lowmidGain * fabs(lowmidGain) * fabs(lowmidGain));
        double bassGain = (BSG - 0.5) * 2.0;
        bassGain = 1.0 + (bassGain * fabs(bassGain) * fabs(bassGain));
        double highCoef = 0.0;
        double midCoef = 0.0;
        double lowCoef = 0.0;
        bool eqOff = (trebleGain == 1.0 && highmidGain == 1.0 && lowmidGain == 1.0 && bassGain == 1.0);
        // we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
        // it cancels out to bit-identical anyhow, but we get to skip the calculation
        if (!eqOff) {
            double trebleRef = HIF - 0.5;
            double highmidRef = HMF - 0.5;
            double lowmidRef = LMF - 0.5;
            double bassRef = BSF - 0.5;
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
            double biqK = 2.0 - cos(omega);
            highCoef = -sqrt((biqK * biqK) - 1.0) + biqK;
            highA[biq_reso] = 2.24697960 * highQ;
            highB[biq_reso] = 0.80193774 * highQ;
            highC[biq_reso] = 0.55495813 * highQ;
            midA[biq_freq] = ((pow(midF, 3) * 20000.0) / Effect<T>::getSampleRate());
            midC[biq_freq] = midB[biq_freq] = midA[biq_freq] = fmax(fmin(midA[biq_freq], 0.4999), 0.00025);
            double midFreq = pow(midF, 3) * 20000.0;
            omega = 2.0 * M_PI * (midFreq / Effect<T>::getSampleRate());
            biqK = 2.0 - cos(omega);
            midCoef = -sqrt((biqK * biqK) - 1.0) + biqK;
            midA[biq_reso] = 2.24697960 * midQ;
            midB[biq_reso] = 0.80193774 * midQ;
            midC[biq_reso] = 0.55495813 * midQ;
            lowA[biq_freq] = ((pow(bassF, 3) * 20000.0) / Effect<T>::getSampleRate());
            lowC[biq_freq] = lowB[biq_freq] = lowA[biq_freq] = fmax(fmin(lowA[biq_freq], 0.4999), 0.00025);
            double lowFreq = pow(bassF, 3) * 20000.0;
            omega = 2.0 * M_PI * (lowFreq / Effect<T>::getSampleRate());
            biqK = 2.0 - cos(omega);
            lowCoef = -sqrt((biqK * biqK) - 1.0) + biqK;
            lowA[biq_reso] = 2.24697960 * lowQ;
            lowB[biq_reso] = 0.80193774 * lowQ;
            lowC[biq_reso] = 0.55495813 * lowQ;
            biqK = tan(M_PI * highA[biq_freq]);
            double norm = 1.0 / (1.0 + biqK / highA[biq_reso] + biqK * biqK);
            highA[biq_a0] = biqK * biqK * norm;
            highA[biq_a1] = 2.0 * highA[biq_a0];
            highA[biq_a2] = highA[biq_a0];
            highA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            highA[biq_b2] = (1.0 - biqK / highA[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * highB[biq_freq]);
            norm = 1.0 / (1.0 + biqK / highB[biq_reso] + biqK * biqK);
            highB[biq_a0] = biqK * biqK * norm;
            highB[biq_a1] = 2.0 * highB[biq_a0];
            highB[biq_a2] = highB[biq_a0];
            highB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            highB[biq_b2] = (1.0 - biqK / highB[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * highC[biq_freq]);
            norm = 1.0 / (1.0 + biqK / highC[biq_reso] + biqK * biqK);
            highC[biq_a0] = biqK * biqK * norm;
            highC[biq_a1] = 2.0 * highC[biq_a0];
            highC[biq_a2] = highC[biq_a0];
            highC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            highC[biq_b2] = (1.0 - biqK / highC[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * midA[biq_freq]);
            norm = 1.0 / (1.0 + biqK / midA[biq_reso] + biqK * biqK);
            midA[biq_a0] = biqK * biqK * norm;
            midA[biq_a1] = 2.0 * midA[biq_a0];
            midA[biq_a2] = midA[biq_a0];
            midA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            midA[biq_b2] = (1.0 - biqK / midA[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * midB[biq_freq]);
            norm = 1.0 / (1.0 + biqK / midB[biq_reso] + biqK * biqK);
            midB[biq_a0] = biqK * biqK * norm;
            midB[biq_a1] = 2.0 * midB[biq_a0];
            midB[biq_a2] = midB[biq_a0];
            midB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            midB[biq_b2] = (1.0 - biqK / midB[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * midC[biq_freq]);
            norm = 1.0 / (1.0 + biqK / midC[biq_reso] + biqK * biqK);
            midC[biq_a0] = biqK * biqK * norm;
            midC[biq_a1] = 2.0 * midC[biq_a0];
            midC[biq_a2] = midC[biq_a0];
            midC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            midC[biq_b2] = (1.0 - biqK / midC[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * lowA[biq_freq]);
            norm = 1.0 / (1.0 + biqK / lowA[biq_reso] + biqK * biqK);
            lowA[biq_a0] = biqK * biqK * norm;
            lowA[biq_a1] = 2.0 * lowA[biq_a0];
            lowA[biq_a2] = lowA[biq_a0];
            lowA[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lowA[biq_b2] = (1.0 - biqK / lowA[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * lowB[biq_freq]);
            norm = 1.0 / (1.0 + biqK / lowB[biq_reso] + biqK * biqK);
            lowB[biq_a0] = biqK * biqK * norm;
            lowB[biq_a1] = 2.0 * lowB[biq_a0];
            lowB[biq_a2] = lowB[biq_a0];
            lowB[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lowB[biq_b2] = (1.0 - biqK / lowB[biq_reso] + biqK * biqK) * norm;
            biqK = tan(M_PI * lowC[biq_freq]);
            norm = 1.0 / (1.0 + biqK / lowC[biq_reso] + biqK * biqK);
            lowC[biq_a0] = biqK * biqK * norm;
            lowC[biq_a1] = 2.0 * lowC[biq_a0];
            lowC[biq_a2] = lowC[biq_a0];
            lowC[biq_b1] = 2.0 * (biqK * biqK - 1.0) * norm;
            lowC[biq_b2] = (1.0 - biqK / lowC[biq_reso] + biqK * biqK) * norm;
        }
        // SmoothEQ2
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
            if (!eqOff) {
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
            }
            // SmoothEQ2
            if (fabs(inputSampleL) > gate) {
                bezGateL = overallscale / fmin(bezRez, sloRez);
            }
            else {
                bezGateL = fmax(0.000001, bezGateL - fmin(bezRez, sloRez));
            }
            if (fabs(inputSampleR) > gate) {
                bezGateR = overallscale / fmin(bezRez, sloRez);
            }
            else {
                bezGateR = fmax(0.000001, bezGateR - fmin(bezRez, sloRez));
            }
            if (bezThresh > 0.0) {
                inputSampleL *= (bezThresh + 1.0);
                inputSampleR *= (bezThresh + 1.0);
            } // makeup gain
            bezMaxL = fmax(bezMaxL, fabs(inputSampleL));
            bezMinL = fmax(bezMinL - sloRez, fabs(inputSampleL));
            bezMaxR = fmax(bezMaxR, fabs(inputSampleR));
            bezMinR = fmax(bezMinR - sloRez, fabs(inputSampleR));
            bezComp[bez_cycle] += bezRez;
            bezComp[bez_CtrlL] += (bezMinL * bezRez);
            bezComp[bez_CtrlR] += (bezMinR * bezRez); // Dual mono build
            if (bezComp[bez_cycle] > 1.0) {
                if (bezGateL < 1.0) {
                    bezComp[bez_CtrlL] /= bezGateL;
                }
                if (bezGateR < 1.0) {
                    bezComp[bez_CtrlR] /= bezGateR;
                }
                bezComp[bez_cycle] -= 1.0;
                bezComp[bez_CL] = bezComp[bez_BL];
                bezComp[bez_BL] = bezComp[bez_AL];
                bezComp[bez_AL] = bezComp[bez_CtrlL];
                bezComp[bez_CtrlL] = 0.0;
                bezMaxL = 0.0;
                bezComp[bez_CR] = bezComp[bez_BR];
                bezComp[bez_BR] = bezComp[bez_AR];
                bezComp[bez_AR] = bezComp[bez_CtrlR];
                bezComp[bez_CtrlR] = 0.0;
                bezMaxR = 0.0;
            }
            double CBL = (bezComp[bez_CL] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_BL] * bezComp[bez_cycle]);
            double BAL = (bezComp[bez_BL] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_AL] * bezComp[bez_cycle]);
            double CBAL = (bezComp[bez_BL] + (CBL * (1.0 - bezComp[bez_cycle])) + (BAL * bezComp[bez_cycle])) * 0.5;
            double CBR = (bezComp[bez_CR] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_BR] * bezComp[bez_cycle]);
            double BAR = (bezComp[bez_BR] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_AR] * bezComp[bez_cycle]);
            double CBAR = (bezComp[bez_BR] + (CBR * (1.0 - bezComp[bez_cycle])) + (BAR * bezComp[bez_cycle])) * 0.5;
            if (bezThresh > 0.0) {
                inputSampleL *= 1.0 - (fmin(CBAL * bezThresh, 1.0));
                inputSampleR *= 1.0 - (fmin(CBAR * bezThresh, 1.0));
            }
            // Dynamics3
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
                } // blank out highpass if jut switched off
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
            double gain = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            if (gain > 1.0) {
                gain *= gain;
            }
            if (gain < 1.0) {
                gain = 1.0 - pow(1.0 - gain, 2);
            }
            inputSampleL = inputSampleL * gain;
            inputSampleR = inputSampleR * gain;
            // applies pan section, and smoothed fader gain
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
} // namespace airwindohhs::consolex2pre
