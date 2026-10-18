#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::consolex2buss {

constexpr std::string_view k_name{ "ConsoleX2Buss" };
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
class ConsoleX2Buss final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'cx2b';
    float HIG;
    float HMG;
    float LMG;
    float BSG;
    float HIF;
    float HMF;
    float LMF;
    float BSF;
    float THR;
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
    double panA;
    double panB;
    double inTrimA;
    double inTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ConsoleX2Buss()
    {
        HIG = 0.5;
        HMG = 0.5;
        LMG = 0.5;
        BSG = 0.5;
        HIF = 0.5;
        HMF = 0.5;
        LMF = 0.5;
        BSF = 0.5;
        THR = 1.0;
        PAN = 0.5;
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
            bezCompF[x] = 0.0;
            bezCompS[x] = 0.0;
        }
        bezCompF[bez_cycle] = 1.0;
        bezCompS[bez_cycle] = 1.0;
        // Dynamics2
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
        kParamI = 8,
        kParamJ = 9,
        kParamK = 10,
        kNumParameters = 11
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: HIG = value; break;
            case kParamB: HMG = value; break;
            case kParamC: LMG = value; break;
            case kParamD: BSG = value; break;
            case kParamE: HIF = value; break;
            case kParamF: HMF = value; break;
            case kParamG: LMF = value; break;
            case kParamH: BSF = value; break;
            case kParamI: THR = value; break;
            case kParamJ: PAN = value; break;
            case kParamK: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return HIG; break;
            case kParamB: return HMG; break;
            case kParamC: return LMG; break;
            case kParamD: return BSG; break;
            case kParamE: return HIF; break;
            case kParamF: return HMF; break;
            case kParamG: return LMF; break;
            case kParamH: return BSF; break;
            case kParamI: return THR; break;
            case kParamJ: return PAN; break;
            case kParamK: return FAD; break;

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
            case kParamI: return 1.0; break;
            case kParamJ: return 0.5; break;
            case kParamK: return 0.5; break;

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
            case kParamI: return "thresh"; break;
            case kParamJ: return "pan"; break;
            case kParamK: return "fader"; break;

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
            case kParamI: return "Thresh"; break;
            case kParamJ: return "Pan"; break;
            case kParamK: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(HIG, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(HMG, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(LMG, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(BSG, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(HIF, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(HMF, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(LMF, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(BSF, text, kVstMaxParamStrLen); break;
            case kParamI: float2string(THR, text, kVstMaxParamStrLen); break;
            case kParamJ: float2string(PAN, text, kVstMaxParamStrLen); break;
            case kParamK: float2string(FAD, text, kVstMaxParamStrLen); break;
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
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "freq", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "dyn", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamK: vst_strncpy(text, "", kVstMaxParamStrLen); break;
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
        double bezCThresh = pow(1.0 - THR, 6.0) * 8.0;
        double bezRez = pow(1.0 - THR, 12.360679774997898) / overallscale;
        double sloRez = pow(1.0 - THR, 10.0) / overallscale;
        sloRez = fmin(fmax(sloRez, 0.00001), 1.0);
        bezRez = fmin(fmax(bezRez, 0.00001), 1.0);
        // Dynamics2
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
            const double temp = (double)sampleFrames / inFramesToProcess;
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
} // namespace airwindohhs::consolex2buss
