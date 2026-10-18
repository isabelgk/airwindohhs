#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::cstrip {

constexpr std::string_view k_name{ "CStrip" };
constexpr std::string_view k_short_description{
    "CStrip is an Airwindows channel strip."
};
constexpr std::string_view k_long_description{
    "Sometimes, Airwindows dabbles with what you might call ‘the normal’ plugins: for instance, CStrip. I was asked to make a channel strip, and while I usually prefer to do things in a more modular sense, it seemed like a fine idea, and so now there’s an Airwindows channel strip, with sort of the usual things you might find there. Sort of. ;) It’s never quite that simple with Airwindows. So, you’ve got a three band EQ, but if you boost the top you get traces of the ‘Energy’ plugin just to add some aggressiveness to the extreme highs for heavy boosts. You’ve got crossover frequencies, you’ve got highpass and lowpass, but the highpass and lowpass aren’t normal algorithms either: they’re designed for ‘trapping in’ already bandlimited sounds to get the most out of them. There’s a gate, but it’s an Airwindows gate where the release is designed to pull the audio back in the sound picture, not just volume-ramp it. There’s a time delay control that exists only to give the track a micro-delay relative to other tracks for groove purposes. And there’s the compressor… CStrip uses the ButterComp algorithm plus a speed control that lets you do odd things like increase the speed hugely. ButterComp compresses interleaved samples in Class AB, so that’s four independent compressors per channel. If you crank the speed under heavy compression you can get artifacts (for normal behavior, keep the speed a lot lower). The whole idea with CStrip was to do the channel strip, but take it out into stranger realms where it can be used for various sonic destruction. It may not be the last of such plugins: some new stuff coming out is giving me ideas for other ‘combination’ plugins, especially ones where the algorithms can be interwoven to produce effects you literally couldn’t have out of discrete plugins. (one day, I’ll be open sourcing all of that and telling other plugin coders all about it) CStrip also uses a technique I came to rely on, where if a component of the plugin isn’t being used (for instance, the lowpass and highpass when set to their extremes switch off) then it gets completely bypassed, and not even the math of the component is used: this is sort of like how ‘unity gain’ in code isn’t the same as ‘bypass’. (if you’re in floating point and you’re multiplying by 1.0, you’re also doing a math operation at a given exponent and this can wipe out floating point values at very different levels of detail) But that’s getting too wonky: I hope you enjoy CStrip."
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class CStrip final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'cstr';
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double last2SampleL;
    double lastSampleR;
    double last2SampleR;
    // begin EQ
    double iirHighSampleLA;
    double iirHighSampleLB;
    double iirHighSampleLC;
    double iirHighSampleLD;
    double iirHighSampleLE;
    double iirLowSampleLA;
    double iirLowSampleLB;
    double iirLowSampleLC;
    double iirLowSampleLD;
    double iirLowSampleLE;
    double iirHighSampleL;
    double iirLowSampleL;
    double iirHighSampleRA;
    double iirHighSampleRB;
    double iirHighSampleRC;
    double iirHighSampleRD;
    double iirHighSampleRE;
    double iirLowSampleRA;
    double iirLowSampleRB;
    double iirLowSampleRC;
    double iirLowSampleRD;
    double iirLowSampleRE;
    double iirHighSampleR;
    double iirLowSampleR;
    double tripletLA;
    double tripletLB;
    double tripletLC;
    double tripletFactorL;
    double tripletRA;
    double tripletRB;
    double tripletRC;
    double tripletFactorR;
    double lowpassSampleLAA;
    double lowpassSampleLAB;
    double lowpassSampleLBA;
    double lowpassSampleLBB;
    double lowpassSampleLCA;
    double lowpassSampleLCB;
    double lowpassSampleLDA;
    double lowpassSampleLDB;
    double lowpassSampleLE;
    double lowpassSampleLF;
    double lowpassSampleLG;
    double lowpassSampleRAA;
    double lowpassSampleRAB;
    double lowpassSampleRBA;
    double lowpassSampleRBB;
    double lowpassSampleRCA;
    double lowpassSampleRCB;
    double lowpassSampleRDA;
    double lowpassSampleRDB;
    double lowpassSampleRE;
    double lowpassSampleRF;
    double lowpassSampleRG;
    double highpassSampleLAA;
    double highpassSampleLAB;
    double highpassSampleLBA;
    double highpassSampleLBB;
    double highpassSampleLCA;
    double highpassSampleLCB;
    double highpassSampleLDA;
    double highpassSampleLDB;
    double highpassSampleLE;
    double highpassSampleLF;
    double highpassSampleRAA;
    double highpassSampleRAB;
    double highpassSampleRBA;
    double highpassSampleRBB;
    double highpassSampleRCA;
    double highpassSampleRCB;
    double highpassSampleRDA;
    double highpassSampleRDB;
    double highpassSampleRE;
    double highpassSampleRF;
    bool flip;
    int flipthree;
    // end EQ
    // begin Gate
    bool WasNegativeL;
    int ZeroCrossL;
    double gaterollerL;
    double gateL;
    bool WasNegativeR;
    int ZeroCrossR;
    double gaterollerR;
    double gateR;
    // end Gate
    // begin Timing
    double pL[4099];
    double pR[4099];
    int count;
    // end Timing
    // begin ButterComp
    double controlAposL;
    double controlAnegL;
    double controlBposL;
    double controlBnegL;
    double targetposL;
    double targetnegL;
    double avgLA;
    double avgLB;
    double nvgLA;
    double nvgLB;
    double controlAposR;
    double controlAnegR;
    double controlBposR;
    double controlBnegR;
    double targetposR;
    double targetnegR;
    double avgRA;
    double avgRB;
    double nvgRA;
    double nvgRB;
    // end ButterComp
    // flip is already covered in EQ
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;
    float J;
    float K;
    float L;

  public:
    CStrip()
    {
        A = 0.5; // Treble -12 to 12
        B = 0.5; // Mid -12 to 12
        C = 0.5; // Bass -12 to 12
        D = 1.0; // Lowpass 16.0K log 1 to 16 defaulting to 16K
        E = 0.4; // TrebFrq 6.0 log 1 to 16 defaulting to 6K
        F = 0.4; // BassFrq 100.0 log 30 to 1600 defaulting to 100 hz
        G = 0.0; // Hipass 30.0 log 30 to 1600 defaulting to 30
        H = 0.0; // Gate 0-1
        I = 0.0; // Compres 0-1
        J = 0.0; // CompSpd 0-1
        K = 0.0; // TimeLag 0-1
        L = 0.5; // OutGain -18 to 18
        lastSampleL = 0.0;
        last2SampleL = 0.0;
        lastSampleR = 0.0;
        last2SampleR = 0.0;
        iirHighSampleLA = 0.0;
        iirHighSampleLB = 0.0;
        iirHighSampleLC = 0.0;
        iirHighSampleLD = 0.0;
        iirHighSampleLE = 0.0;
        iirLowSampleLA = 0.0;
        iirLowSampleLB = 0.0;
        iirLowSampleLC = 0.0;
        iirLowSampleLD = 0.0;
        iirLowSampleLE = 0.0;
        iirHighSampleL = 0.0;
        iirLowSampleL = 0.0;
        iirHighSampleRA = 0.0;
        iirHighSampleRB = 0.0;
        iirHighSampleRC = 0.0;
        iirHighSampleRD = 0.0;
        iirHighSampleRE = 0.0;
        iirLowSampleRA = 0.0;
        iirLowSampleRB = 0.0;
        iirLowSampleRC = 0.0;
        iirLowSampleRD = 0.0;
        iirLowSampleRE = 0.0;
        iirHighSampleR = 0.0;
        iirLowSampleR = 0.0;
        tripletLA = 0.0;
        tripletLB = 0.0;
        tripletLC = 0.0;
        tripletFactorL = 0.0;
        tripletRA = 0.0;
        tripletRB = 0.0;
        tripletRC = 0.0;
        tripletFactorR = 0.0;
        lowpassSampleLAA = 0.0;
        lowpassSampleLAB = 0.0;
        lowpassSampleLBA = 0.0;
        lowpassSampleLBB = 0.0;
        lowpassSampleLCA = 0.0;
        lowpassSampleLCB = 0.0;
        lowpassSampleLDA = 0.0;
        lowpassSampleLDB = 0.0;
        lowpassSampleLE = 0.0;
        lowpassSampleLF = 0.0;
        lowpassSampleLG = 0.0;
        lowpassSampleRAA = 0.0;
        lowpassSampleRAB = 0.0;
        lowpassSampleRBA = 0.0;
        lowpassSampleRBB = 0.0;
        lowpassSampleRCA = 0.0;
        lowpassSampleRCB = 0.0;
        lowpassSampleRDA = 0.0;
        lowpassSampleRDB = 0.0;
        lowpassSampleRE = 0.0;
        lowpassSampleRF = 0.0;
        lowpassSampleRG = 0.0;
        highpassSampleLAA = 0.0;
        highpassSampleLAB = 0.0;
        highpassSampleLBA = 0.0;
        highpassSampleLBB = 0.0;
        highpassSampleLCA = 0.0;
        highpassSampleLCB = 0.0;
        highpassSampleLDA = 0.0;
        highpassSampleLDB = 0.0;
        highpassSampleLE = 0.0;
        highpassSampleLF = 0.0;
        highpassSampleRAA = 0.0;
        highpassSampleRAB = 0.0;
        highpassSampleRBA = 0.0;
        highpassSampleRBB = 0.0;
        highpassSampleRCA = 0.0;
        highpassSampleRCB = 0.0;
        highpassSampleRDA = 0.0;
        highpassSampleRDB = 0.0;
        highpassSampleRE = 0.0;
        highpassSampleRF = 0.0;
        flip = false;
        flipthree = 0;
        // end EQ
        // begin Gate
        WasNegativeL = false;
        ZeroCrossL = 0;
        gaterollerL = 0.0;
        gateL = 0.0;
        WasNegativeR = false;
        ZeroCrossR = 0;
        gaterollerR = 0.0;
        gateR = 0.0;
        // end Gate
        // begin Timing
        for (int fcount = 0; fcount < 4098; fcount++) {
            pL[fcount] = 0.0;
            pR[fcount] = 0.0;
        }
        count = 0;
        // end Timing
        // begin ButterComp
        controlAposL = 1.0;
        controlAnegL = 1.0;
        controlBposL = 1.0;
        controlBnegL = 1.0;
        targetposL = 1.0;
        targetnegL = 1.0;
        avgLA = avgLB = 0.0;
        nvgLA = nvgLB = 0.0;
        controlAposR = 1.0;
        controlAnegR = 1.0;
        controlBposR = 1.0;
        controlBnegR = 1.0;
        targetposR = 1.0;
        targetnegR = 1.0;
        avgRA = avgRB = 0.0;
        nvgRA = nvgRB = 0.0;
        // end ButterComp
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
        kNumParameters = 12
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            case kParamI: I = value; break;
            case kParamJ: J = value; break;
            case kParamK: K = value; break;
            case kParamL: L = value; break;
            default: break; // unknown parameter, shouldn't happen!
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
            case kParamI: return I; break;
            case kParamJ: return J; break;
            case kParamK: return K; break;
            case kParamL: return L; break;

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
            case kParamE: return 0.4; break;
            case kParamF: return 0.4; break;
            case kParamG: return 0.0; break;
            case kParamH: return 0.0; break;
            case kParamI: return 0.0; break;
            case kParamJ: return 0.0; break;
            case kParamK: return 0.0; break;
            case kParamL: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "treble"; break;
            case kParamB: return "mid"; break;
            case kParamC: return "bass"; break;
            case kParamD: return "lowpass"; break;
            case kParamE: return "trebfrq"; break;
            case kParamF: return "bassfrq"; break;
            case kParamG: return "hipass"; break;
            case kParamH: return "gate"; break;
            case kParamI: return "compres"; break;
            case kParamJ: return "compspd"; break;
            case kParamK: return "timelag"; break;
            case kParamL: return "outgain"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Treble"; break;
            case kParamB: return "Mid"; break;
            case kParamC: return "Bass"; break;
            case kParamD: return "Lowpass"; break;
            case kParamE: return "TrebFrq"; break;
            case kParamF: return "BassFrq"; break;
            case kParamG: return "Hipass"; break;
            case kParamH: return "Gate"; break;
            case kParamI: return "Compres"; break;
            case kParamJ: return "CompSpd"; break;
            case kParamK: return "TimeLag"; break;
            case kParamL: return "OutGain"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string((A * 24.0) - 12.0, text, kVstMaxParamStrLen); break; // Treble -12 to 12
            case kParamB: float2string((B * 24.0) - 12.0, text, kVstMaxParamStrLen); break; // Mid -12 to 12
            case kParamC: float2string((C * 24.0) - 12.0, text, kVstMaxParamStrLen); break; // Bass -12 to 12
            case kParamD: float2string((D * D * 15.0) + 1.0, text, kVstMaxParamStrLen); break; // Lowpass 16.0K log 1 to 16 defaulting to 16K
            case kParamE: float2string((E * E * 15.0) + 1.0, text, kVstMaxParamStrLen); break; // TrebFrq 6.0 log 1 to 16 defaulting to 6K
            case kParamF: float2string((F * F * 1570.0) + 30.0, text, kVstMaxParamStrLen); break; // BassFrq 100.0 log 30 to 1600 defaulting to 100 hz
            case kParamG: float2string((G * G * 1570.0) + 30.0, text, kVstMaxParamStrLen); break; // Hipass 30.0 log 30 to 1600 defaulting to 30
            case kParamH: float2string(H, text, kVstMaxParamStrLen); break; // Gate 0-1
            case kParamI: float2string(I, text, kVstMaxParamStrLen); break; // Compres 0-1
            case kParamJ: float2string(J, text, kVstMaxParamStrLen); break; // CompSpd 0-1
            case kParamK: float2string(K, text, kVstMaxParamStrLen); break; // TimeLag 0-1
            case kParamL: float2string((L * 36.0) - 18.0, text, kVstMaxParamStrLen); break; // OutGain -18 to 18
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "dB", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "dB", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "dB", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "Khz", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "Khz", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "hz", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "hz", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamK: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamL: vst_strncpy(text, "dB", kVstMaxParamStrLen); break;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        double compscale = overallscale;
        overallscale = Effect<T>::getSampleRate();
        compscale = compscale * overallscale;
        // compscale is the one that's 1 or something like 2.2 for 96K rates
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        double inputSampleL;
        double inputSampleR;
        double highSampleL = 0.0;
        double midSampleL = 0.0;
        double bassSampleL = 0.0;
        double highSampleR = 0.0;
        double midSampleR = 0.0;
        double bassSampleR = 0.0;
        double densityA = (A * 12.0) - 6.0;
        double densityB = (B * 12.0) - 6.0;
        double densityC = (C * 12.0) - 6.0;
        bool engageEQ = true;
        if ((0.0 == densityA) && (0.0 == densityB) && (0.0 == densityC)) {
            engageEQ = false;
        }
        densityA = pow(10.0, densityA / 20.0) - 1.0;
        densityB = pow(10.0, densityB / 20.0) - 1.0;
        densityC = pow(10.0, densityC / 20.0) - 1.0;
        // convert to 0 to X multiplier with 1.0 being O db
        // minus one gives nearly -1 to ? (should top out at 1)
        // calibrate so that X db roughly equals X db with maximum topping out at 1 internally
        double tripletIntensity = -densityA;
        double iirAmountC = (((D * D * 15.0) + 1.0) * 0.0188) + 0.7;
        if (iirAmountC > 1.0) {
            iirAmountC = 1.0;
        }
        bool engageLowpass = false;
        if (((D * D * 15.0) + 1.0) < 15.99) {
            engageLowpass = true;
        }
        double iirAmountA = (((E * E * 15.0) + 1.0) * 1000) / overallscale;
        double iirAmountB = (((F * F * 1570.0) + 30.0) * 10) / overallscale;
        double iirAmountD = (((G * G * 1570.0) + 30.0) * 1.0) / overallscale;
        bool engageHighpass = false;
        if (((G * G * 1570.0) + 30.0) > 30.01) {
            engageHighpass = true;
        }
        // bypass the highpass and lowpass if set to extremes
        double bridgerectifier;
        double outA = fabs(densityA);
        double outB = fabs(densityB);
        double outC = fabs(densityC);
        // end EQ
        // begin Gate
        double onthreshold = (pow(H, 4) / 3) + 0.00018;
        double offthreshold = onthreshold * 1.1;
        bool engageGate = false;
        if (onthreshold > 0.00018) {
            engageGate = true;
        }
        double release = 0.028331119964586;
        double absmax = 220.9;
        // speed to be compensated w.r.t sample rate
        // end Gate
        // begin Timing
        double offset = pow(K, 5) * 700;
        int nearVal = (int)floor(fabs(offset));
        double farLevel = fabs(offset) - nearVal;
        int farVal = nearVal + 1;
        double nearLevel = 1.0 - farLevel;
        bool engageTiming = false;
        if (offset > 0.0) {
            engageTiming = true;
        }
        // end Timing
        // begin ButterComp
        double inputpos;
        double inputneg;
        double calcpos;
        double calcneg;
        double outputpos;
        double outputneg;
        double totalmultiplier;
        double inputgain = (pow(I, 4) * 35) + 1.0;
        double compoutgain = inputgain;
        compoutgain -= 1.0;
        compoutgain /= 1.2;
        compoutgain += 1.0;
        double divisor = (0.008 * pow(J, 2)) + 0.0004;
        // originally 0.012
        divisor /= compscale;
        double remainder = divisor;
        divisor = 1.0 - divisor;
        bool engageComp = false;
        if (inputgain > 1.0) {
            engageComp = true;
        }
        // end ButterComp
        double outputgain = pow(10.0, ((L * 36.0) - 18.0) / 20.0);
        while (--sampleFrames >= 0) {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            last2SampleL = lastSampleL;
            lastSampleL = inputSampleL;
            last2SampleR = lastSampleR;
            lastSampleR = inputSampleR;
            // begin Gate
            if (engageGate) {
                if (inputSampleL > 0) {
                    if (WasNegativeL == true) {
                        ZeroCrossL = absmax * 0.3;
                    }
                    WasNegativeL = false;
                }
                else {
                    ZeroCrossL += 1;
                    WasNegativeL = true;
                }
                if (inputSampleR > 0) {
                    if (WasNegativeR == true) {
                        ZeroCrossR = absmax * 0.3;
                    }
                    WasNegativeR = false;
                }
                else {
                    ZeroCrossR += 1;
                    WasNegativeR = true;
                }
                if (ZeroCrossL > absmax) {
                    ZeroCrossL = absmax;
                }
                if (ZeroCrossR > absmax) {
                    ZeroCrossR = absmax;
                }
                if (gateL == 0.0) {
                    // if gate is totally silent
                    if (fabs(inputSampleL) > onthreshold) {
                        if (gaterollerL == 0.0) {
                            gaterollerL = ZeroCrossL;
                        }
                        else {
                            gaterollerL -= release;
                        }
                        // trigger from total silence only- if we're active then signal must clear offthreshold
                    }
                    else {
                        gaterollerL -= release;
                    }
                }
                else {
                    // gate is not silent but closing
                    if (fabs(inputSampleL) > offthreshold) {
                        if (gaterollerL < ZeroCrossL) {
                            gaterollerL = ZeroCrossL;
                        }
                        else {
                            gaterollerL -= release;
                        }
                        // always trigger if gate is over offthreshold, otherwise close anyway
                    }
                    else {
                        gaterollerL -= release;
                    }
                }
                if (gateR == 0.0) {
                    // if gate is totally silent
                    if (fabs(inputSampleR) > onthreshold) {
                        if (gaterollerR == 0.0) {
                            gaterollerR = ZeroCrossR;
                        }
                        else {
                            gaterollerR -= release;
                        }
                        // trigger from total silence only- if we're active then signal must clear offthreshold
                    }
                    else {
                        gaterollerR -= release;
                    }
                }
                else {
                    // gate is not silent but closing
                    if (fabs(inputSampleR) > offthreshold) {
                        if (gaterollerR < ZeroCrossR) {
                            gaterollerR = ZeroCrossR;
                        }
                        else {
                            gaterollerR -= release;
                        }
                        // always trigger if gate is over offthreshold, otherwise close anyway
                    }
                    else {
                        gaterollerR -= release;
                    }
                }
                if (gaterollerL < 0.0) {
                    gaterollerL = 0.0;
                }
                if (gaterollerR < 0.0) {
                    gaterollerR = 0.0;
                }
                if (gaterollerL < 1.0) {
                    gateL = gaterollerL;
                    bridgerectifier = 1 - cos(fabs(inputSampleL));
                    if (inputSampleL > 0) {
                        inputSampleL = (inputSampleL * gateL) + (bridgerectifier * (1.0 - gateL));
                    }
                    else {
                        inputSampleL = (inputSampleL * gateL) - (bridgerectifier * (1.0 - gateL));
                    }
                    if (gateL == 0.0) {
                        inputSampleL = 0.0;
                    }
                }
                else {
                    gateL = 1.0;
                }
                if (gaterollerR < 1.0) {
                    gateR = gaterollerR;
                    bridgerectifier = 1 - cos(fabs(inputSampleR));
                    if (inputSampleR > 0) {
                        inputSampleR = (inputSampleR * gateR) + (bridgerectifier * (1.0 - gateR));
                    }
                    else {
                        inputSampleR = (inputSampleR * gateR) - (bridgerectifier * (1.0 - gateR));
                    }
                    if (gateR == 0.0) {
                        inputSampleR = 0.0;
                    }
                }
                else {
                    gateR = 1.0;
                }
            }
            // end Gate, begin antialiasing
            flip = !flip;
            flipthree++;
            if (flipthree < 1 || flipthree > 3) {
                flipthree = 1;
            }
            // counters
            // begin highpass
            if (engageHighpass) {
                if (flip) {
                    highpassSampleLAA = (highpassSampleLAA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLAA;
                    highpassSampleLBA = (highpassSampleLBA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLBA;
                    highpassSampleLCA = (highpassSampleLCA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLCA;
                    highpassSampleLDA = (highpassSampleLDA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLDA;
                }
                else {
                    highpassSampleLAB = (highpassSampleLAB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLAB;
                    highpassSampleLBB = (highpassSampleLBB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLBB;
                    highpassSampleLCB = (highpassSampleLCB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLCB;
                    highpassSampleLDB = (highpassSampleLDB * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLDB;
                }
                highpassSampleLE = (highpassSampleLE * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                inputSampleL -= highpassSampleLE;
                highpassSampleLF = (highpassSampleLF * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                inputSampleL -= highpassSampleLF;
                if (flip) {
                    highpassSampleRAA = (highpassSampleRAA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRAA;
                    highpassSampleRBA = (highpassSampleRBA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRBA;
                    highpassSampleRCA = (highpassSampleRCA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRCA;
                    highpassSampleRDA = (highpassSampleRDA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRDA;
                }
                else {
                    highpassSampleRAB = (highpassSampleRAB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRAB;
                    highpassSampleRBB = (highpassSampleRBB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRBB;
                    highpassSampleRCB = (highpassSampleRCB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRCB;
                    highpassSampleRDB = (highpassSampleRDB * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRDB;
                }
                highpassSampleRE = (highpassSampleRE * (1 - iirAmountD)) + (inputSampleR * iirAmountD);
                inputSampleR -= highpassSampleRE;
                highpassSampleRF = (highpassSampleRF * (1 - iirAmountD)) + (inputSampleR * iirAmountD);
                inputSampleR -= highpassSampleRF;
            }
            // end highpass
            // begin compressor
            if (engageComp) {
                // begin L
                inputSampleL *= inputgain;
                inputpos = (inputSampleL * fpOld) + (avgLA * fpNew) + 1.0;
                avgLA = inputSampleL;
                if (inputpos < 0.0) {
                    inputpos = 0.0;
                }
                outputpos = inputpos / 2.0;
                if (outputpos > 1.0) {
                    outputpos = 1.0;
                }
                inputpos *= inputpos;
                targetposL *= divisor;
                targetposL += (inputpos * remainder);
                calcpos = pow((1.0 / targetposL), 2);
                inputneg = (-inputSampleL * fpOld) + (nvgLA * fpNew) + 1.0;
                nvgLA = -inputSampleL;
                if (inputneg < 0.0) {
                    inputneg = 0.0;
                }
                outputneg = inputneg / 2.0;
                if (outputneg > 1.0) {
                    outputneg = 1.0;
                }
                inputneg *= inputneg;
                targetnegL *= divisor;
                targetnegL += (inputneg * remainder);
                calcneg = pow((1.0 / targetnegL), 2);
                // now we have mirrored targets for comp
                // outputpos and outputneg go from 0 to 1
                if (inputSampleL > 0) { // working on pos
                    if (true == flip) {
                        controlAposL *= divisor;
                        controlAposL += (calcpos * remainder);
                    }
                    else {
                        controlBposL *= divisor;
                        controlBposL += (calcpos * remainder);
                    }
                }
                else { // working on neg
                    if (true == flip) {
                        controlAnegL *= divisor;
                        controlAnegL += (calcneg * remainder);
                    }
                    else {
                        controlBnegL *= divisor;
                        controlBnegL += (calcneg * remainder);
                    }
                }
                // this causes each of the four to update only when active and in the correct 'flip'
                if (true == flip) {
                    totalmultiplier = (controlAposL * outputpos) + (controlAnegL * outputneg);
                }
                else {
                    totalmultiplier = (controlBposL * outputpos) + (controlBnegL * outputneg);
                }
                // this combines the sides according to flip, blending relative to the input value
                inputSampleL *= totalmultiplier;
                inputSampleL /= compoutgain;
                // end L
                // begin R
                inputSampleR *= inputgain;
                inputpos = (inputSampleR * fpOld) + (avgRA * fpNew) + 1.0;
                avgRA = inputSampleR;
                if (inputpos < 0.0) {
                    inputpos = 0.0;
                }
                outputpos = inputpos / 2.0;
                if (outputpos > 1.0) {
                    outputpos = 1.0;
                }
                inputpos *= inputpos;
                targetposR *= divisor;
                targetposR += (inputpos * remainder);
                calcpos = pow((1.0 / targetposR), 2);
                inputneg = (-inputSampleR * fpOld) + (nvgRA * fpNew) + 1.0;
                nvgRA = -inputSampleR;
                if (inputneg < 0.0) {
                    inputneg = 0.0;
                }
                outputneg = inputneg / 2.0;
                if (outputneg > 1.0) {
                    outputneg = 1.0;
                }
                inputneg *= inputneg;
                targetnegR *= divisor;
                targetnegR += (inputneg * remainder);
                calcneg = pow((1.0 / targetnegR), 2);
                // now we have mirrored targets for comp
                // outputpos and outputneg go from 0 to 1
                if (inputSampleR > 0) { // working on pos
                    if (true == flip) {
                        controlAposR *= divisor;
                        controlAposR += (calcpos * remainder);
                    }
                    else {
                        controlBposR *= divisor;
                        controlBposR += (calcpos * remainder);
                    }
                }
                else { // working on neg
                    if (true == flip) {
                        controlAnegR *= divisor;
                        controlAnegR += (calcneg * remainder);
                    }
                    else {
                        controlBnegR *= divisor;
                        controlBnegR += (calcneg * remainder);
                    }
                }
                // this causes each of the four to update only when active and in the correct 'flip'
                if (true == flip) {
                    totalmultiplier = (controlAposR * outputpos) + (controlAnegR * outputneg);
                }
                else {
                    totalmultiplier = (controlBposR * outputpos) + (controlBnegR * outputneg);
                }
                // this combines the sides according to flip, blending relative to the input value
                inputSampleR *= totalmultiplier;
                inputSampleR /= compoutgain;
                // end R
            }
            // end compressor
            // begin EQ
            if (engageEQ) {
                switch (flipthree) {
                    case 1:
                        tripletFactorL = last2SampleL - inputSampleL;
                        tripletLA += tripletFactorL;
                        tripletLC -= tripletFactorL;
                        tripletFactorL = tripletLA * tripletIntensity;
                        iirHighSampleLC = (iirHighSampleLC * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
                        highSampleL = inputSampleL - iirHighSampleLC;
                        iirLowSampleLC = (iirLowSampleLC * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
                        bassSampleL = iirLowSampleLC;
                        tripletFactorR = last2SampleR - inputSampleR;
                        tripletRA += tripletFactorR;
                        tripletRC -= tripletFactorR;
                        tripletFactorR = tripletRA * tripletIntensity;
                        iirHighSampleRC = (iirHighSampleRC * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
                        highSampleR = inputSampleR - iirHighSampleRC;
                        iirLowSampleRC = (iirLowSampleRC * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
                        bassSampleR = iirLowSampleRC;
                        break;
                    case 2:
                        tripletFactorL = last2SampleL - inputSampleL;
                        tripletLB += tripletFactorL;
                        tripletLA -= tripletFactorL;
                        tripletFactorL = tripletLB * tripletIntensity;
                        iirHighSampleLD = (iirHighSampleLD * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
                        highSampleL = inputSampleL - iirHighSampleLD;
                        iirLowSampleLD = (iirLowSampleLD * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
                        bassSampleL = iirLowSampleLD;
                        tripletFactorR = last2SampleR - inputSampleR;
                        tripletRB += tripletFactorR;
                        tripletRA -= tripletFactorR;
                        tripletFactorR = tripletRB * tripletIntensity;
                        iirHighSampleRD = (iirHighSampleRD * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
                        highSampleR = inputSampleR - iirHighSampleRD;
                        iirLowSampleRD = (iirLowSampleRD * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
                        bassSampleR = iirLowSampleRD;
                        break;
                    case 3:
                        tripletFactorL = last2SampleL - inputSampleL;
                        tripletLC += tripletFactorL;
                        tripletLB -= tripletFactorL;
                        tripletFactorL = tripletLC * tripletIntensity;
                        iirHighSampleLE = (iirHighSampleLE * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
                        highSampleL = inputSampleL - iirHighSampleLE;
                        iirLowSampleLE = (iirLowSampleLE * (1.0 - iirAmountB)) + (inputSampleL * iirAmountB);
                        bassSampleL = iirLowSampleLE;
                        tripletFactorR = last2SampleR - inputSampleR;
                        tripletRC += tripletFactorR;
                        tripletRB -= tripletFactorR;
                        tripletFactorR = tripletRC * tripletIntensity;
                        iirHighSampleRE = (iirHighSampleRE * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
                        highSampleR = inputSampleR - iirHighSampleRE;
                        iirLowSampleRE = (iirLowSampleRE * (1.0 - iirAmountB)) + (inputSampleR * iirAmountB);
                        bassSampleR = iirLowSampleRE;
                        break;
                }
                tripletLA /= 2.0;
                tripletLB /= 2.0;
                tripletLC /= 2.0;
                highSampleL = highSampleL + tripletFactorL;
                tripletRA /= 2.0;
                tripletRB /= 2.0;
                tripletRC /= 2.0;
                highSampleR = highSampleR + tripletFactorR;
                if (flip) {
                    iirHighSampleLA = (iirHighSampleLA * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
                    highSampleL -= iirHighSampleLA;
                    iirLowSampleLA = (iirLowSampleLA * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
                    bassSampleL = iirLowSampleLA;
                    iirHighSampleRA = (iirHighSampleRA * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
                    highSampleR -= iirHighSampleRA;
                    iirLowSampleRA = (iirLowSampleRA * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
                    bassSampleR = iirLowSampleRA;
                }
                else {
                    iirHighSampleLB = (iirHighSampleLB * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
                    highSampleL -= iirHighSampleLB;
                    iirLowSampleLB = (iirLowSampleLB * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
                    bassSampleL = iirLowSampleLB;
                    iirHighSampleRB = (iirHighSampleRB * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
                    highSampleR -= iirHighSampleRB;
                    iirLowSampleRB = (iirLowSampleRB * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
                    bassSampleR = iirLowSampleRB;
                }
                iirHighSampleL = (iirHighSampleL * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
                highSampleL -= iirHighSampleL;
                iirLowSampleL = (iirLowSampleL * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
                bassSampleL = iirLowSampleL;
                iirHighSampleR = (iirHighSampleR * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
                highSampleR -= iirHighSampleR;
                iirLowSampleR = (iirLowSampleR * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
                bassSampleR = iirLowSampleR;
                midSampleL = (inputSampleL - bassSampleL) - highSampleL;
                midSampleR = (inputSampleR - bassSampleR) - highSampleR;
                // drive section
                highSampleL *= (densityA + 1.0);
                bridgerectifier = fabs(highSampleL) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityA > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (highSampleL > 0) {
                    highSampleL = (highSampleL * (1 - outA)) + (bridgerectifier * outA);
                }
                else {
                    highSampleL = (highSampleL * (1 - outA)) - (bridgerectifier * outA);
                }
                // blend according to densityA control
                highSampleR *= (densityA + 1.0);
                bridgerectifier = fabs(highSampleR) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityA > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (highSampleR > 0) {
                    highSampleR = (highSampleR * (1 - outA)) + (bridgerectifier * outA);
                }
                else {
                    highSampleR = (highSampleR * (1 - outA)) - (bridgerectifier * outA);
                }
                // blend according to densityA control
                midSampleL *= (densityB + 1.0);
                bridgerectifier = fabs(midSampleL) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityB > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (midSampleL > 0) {
                    midSampleL = (midSampleL * (1 - outB)) + (bridgerectifier * outB);
                }
                else {
                    midSampleL = (midSampleL * (1 - outB)) - (bridgerectifier * outB);
                }
                // blend according to densityB control
                midSampleR *= (densityB + 1.0);
                bridgerectifier = fabs(midSampleR) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityB > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (midSampleR > 0) {
                    midSampleR = (midSampleR * (1 - outB)) + (bridgerectifier * outB);
                }
                else {
                    midSampleR = (midSampleR * (1 - outB)) - (bridgerectifier * outB);
                }
                // blend according to densityB control
                bassSampleL *= (densityC + 1.0);
                bridgerectifier = fabs(bassSampleL) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityC > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (bassSampleL > 0) {
                    bassSampleL = (bassSampleL * (1 - outC)) + (bridgerectifier * outC);
                }
                else {
                    bassSampleL = (bassSampleL * (1 - outC)) - (bridgerectifier * outC);
                }
                // blend according to densityC control
                bassSampleR *= (densityC + 1.0);
                bridgerectifier = fabs(bassSampleR) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityC > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (bassSampleR > 0) {
                    bassSampleR = (bassSampleR * (1 - outC)) + (bridgerectifier * outC);
                }
                else {
                    bassSampleR = (bassSampleR * (1 - outC)) - (bridgerectifier * outC);
                }
                // blend according to densityC control
                inputSampleL = midSampleL;
                inputSampleL += highSampleL;
                inputSampleL += bassSampleL;
                inputSampleR = midSampleR;
                inputSampleR += highSampleR;
                inputSampleR += bassSampleR;
            }
            // end EQ
            // begin Timing
            if (engageTiming) {
                if (count < 1 || count > 2048) {
                    count = 2048;
                }
                pL[count + 2048] = pL[count] = inputSampleL;
                pR[count + 2048] = pR[count] = inputSampleR;
                inputSampleL = pL[count + nearVal] * nearLevel;
                inputSampleR = pR[count + nearVal] * nearLevel;
                inputSampleL += pL[count + farVal] * farLevel;
                inputSampleR += pR[count + farVal] * farLevel;
                count -= 1;
                // consider adding third sample just to bring out superhighs subtly, like old interpolation hacks
                // or third and fifth samples, ditto
            }
            // end Timing
            // EQ lowpass is after all processing like the compressor that might produce hash
            if (engageLowpass) {
                if (flip) {
                    lowpassSampleLAA = (lowpassSampleLAA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLAA;
                    lowpassSampleLBA = (lowpassSampleLBA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLBA;
                    lowpassSampleLCA = (lowpassSampleLCA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLCA;
                    lowpassSampleLDA = (lowpassSampleLDA * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLDA;
                    lowpassSampleLE = (lowpassSampleLE * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLE;
                    lowpassSampleRAA = (lowpassSampleRAA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRAA;
                    lowpassSampleRBA = (lowpassSampleRBA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRBA;
                    lowpassSampleRCA = (lowpassSampleRCA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRCA;
                    lowpassSampleRDA = (lowpassSampleRDA * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRDA;
                    lowpassSampleRE = (lowpassSampleRE * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRE;
                }
                else {
                    lowpassSampleLAB = (lowpassSampleLAB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLAB;
                    lowpassSampleLBB = (lowpassSampleLBB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLBB;
                    lowpassSampleLCB = (lowpassSampleLCB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLCB;
                    lowpassSampleLDB = (lowpassSampleLDB * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLDB;
                    lowpassSampleLF = (lowpassSampleLF * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                    inputSampleL = lowpassSampleLF;
                    lowpassSampleRAB = (lowpassSampleRAB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRAB;
                    lowpassSampleRBB = (lowpassSampleRBB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRBB;
                    lowpassSampleRCB = (lowpassSampleRCB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRCB;
                    lowpassSampleRDB = (lowpassSampleRDB * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRDB;
                    lowpassSampleRF = (lowpassSampleRF * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                    inputSampleR = lowpassSampleRF;
                }
                lowpassSampleLG = (lowpassSampleLG * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                lowpassSampleRG = (lowpassSampleRG * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
                inputSampleL = (lowpassSampleLG * (1.0 - iirAmountC)) + (inputSampleL * iirAmountC);
                inputSampleR = (lowpassSampleRG * (1.0 - iirAmountC)) + (inputSampleR * iirAmountC);
            }
            // built in output trim and dry/wet if desired
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::cstrip
