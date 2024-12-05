#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::eq {

constexpr std::string_view k_name{ "EQ" };
constexpr std::string_view k_short_description{
    "EQ is just the EQ parts of CStrip."
};
constexpr std::string_view k_long_description{
    "CStrip actually came out of this: a set of EQs specially coded to work together. It’s a lossless three-band (as in, the bands are made by different IIR filtrations being subtracted from each other, so if it’s flat it’s totally bit-identical output and also it has no pre-echo) with a special highpass and lowpass. Each of these things gets switched out of the circuit if not in use (much like CStrip). That makes EQ a very nice default EQ for broad-stroke filtering.The slopes aren’t super high, but that just helps it sound more natural (for a more striking-sounding filter, try Capacitor which is a more aggressively sloped highpass and lowpass). I could have given it set frequencies, but it seems like that’s kind of handy. This plugin is given to you (in AU, and Mac/Win/Linux VST) by request, as I’ve had a user ask for it even though CStrip is already out. So, for a simpler and more approachable Airwindows EQ, here’s EQ :)This work is supported by my Patreon, and I’m happy to say I’m back in the top 50 of the ‘Music’ section at ‘Graphtreon‘: I always like that, feels like I’m getting somewhere with all this. I also like something else, too: I’m definitely giving you folks ‘Aura’ this month. It’s thanks in large part to a mysterious creature known as Slipperman who got involved, and in his honor, next month you’re getting ‘Golem’. Remember, the bigger a success the Patreon is, the more I’m able to persuade people that my way of doing things is good. So if you want this sort of thing to catch on, throw money as that’s all people pay attention to these days…Other stuff I’m working on is Atmosphere, DeRez, and the latest Righteous, Righteous4. Also, if anybody wants to meet me, and also enjoy a rather special academic experience, I’m attending a scholarly lecture by a certain Doctor Bill Bruford in Albany NY this Tuesday, which I’m very excited for. I have no idea how well this’ll go over but I have a smaller version of the famous bent cymbal he discovered (the real one tragically broke after much use), and I mean to give it to him as a gift in honor of his creativity in the field of timbre. Anyway, wild horses wouldn’t keep me away from there, so if my car behaves itself you can meet both me and a REAL great person ;)(as a follow-up, by 2022 I'm hanging down just above 100 in the global Patreon music rankings, and I did in fact get to Albany and the lecture by Bill Bruford. I got in late, and confused him with my cymbal gift after the lecture (Bruford: 'but it's not the one!' which, true, it was not, and I got quite flustered. I hope he understood in some way that I just wanted to give him something, somehow. And no Airwindows people were there, which simplified things I guess)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class EQ final : public Effect<T>
{
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
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;

  public:
    EQ()
    {
        A = 0.5; // Treble -12 to 12
        B = 0.5; // Mid -12 to 12
        C = 0.5; // Bass -12 to 12
        D = 1.0; // Lowpass 16.0K log 1 to 16 defaulting to 16K
        E = 0.4; // TrebFrq 6.0 log 1 to 16 defaulting to 6K
        F = 0.4; // BassFrq 100.0 log 30 to 1600 defaulting to 100 hz
        G = 0.0; // Hipass 30.0 log 30 to 1600 defaulting to 30
        H = 0.5; // OutGain -18 to 18
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
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;
            case kParamE: return 0.4; break;
            case kParamF: return 0.4; break;
            case kParamG: return 0.0; break;
            case kParamH: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "treble"; break;
            case kParamB: return "mid"; break;
            case kParamC: return "bass"; break;
            case kParamD: return "lowpass"; break;
            case kParamE: return "trebfrq"; break;
            case kParamF: return "bassfrq"; break;
            case kParamG: return "hipass"; break;
            case kParamH: return "outgain"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Treble"; break;
            case kParamB: return "Mid"; break;
            case kParamC: return "Bass"; break;
            case kParamD: return "Lowpass"; break;
            case kParamE: return "TrebFrq"; break;
            case kParamF: return "BassFrq"; break;
            case kParamG: return "Hipass"; break;
            case kParamH: return "OutGain"; break;

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
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB"; break;
            case kParamB: return "dB"; break;
            case kParamC: return "dB"; break;
            case kParamD: return "Khz"; break;
            case kParamE: return "Khz"; break;
            case kParamF: return "hz"; break;
            case kParamG: return "hz"; break;
            case kParamH: return "dB"; break;

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
        double compscale = overallscale;
        overallscale = Effect<T>::getSampleRate();
        compscale = compscale * overallscale;
        // compscale is the one that's 1 or something like 2.2 for 96K rates
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
        double outputgain = pow(10.0, ((H * 36.0) - 18.0) / 20.0);
        while (--sampleFrames >= 0)
        {
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
            flip = !flip;
            flipthree++;
            if (flipthree < 1 || flipthree > 3) {
                flipthree = 1;
            }
            // counters
            // begin highpass
            if (engageHighpass)
            {
                if (flip)
                {
                    highpassSampleLAA = (highpassSampleLAA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLAA;
                    highpassSampleLBA = (highpassSampleLBA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLBA;
                    highpassSampleLCA = (highpassSampleLCA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLCA;
                    highpassSampleLDA = (highpassSampleLDA * (1.0 - iirAmountD)) + (inputSampleL * iirAmountD);
                    inputSampleL -= highpassSampleLDA;
                }
                else
                {
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
                if (flip)
                {
                    highpassSampleRAA = (highpassSampleRAA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRAA;
                    highpassSampleRBA = (highpassSampleRBA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRBA;
                    highpassSampleRCA = (highpassSampleRCA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRCA;
                    highpassSampleRDA = (highpassSampleRDA * (1.0 - iirAmountD)) + (inputSampleR * iirAmountD);
                    inputSampleR -= highpassSampleRDA;
                }
                else
                {
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
            // begin EQ
            if (engageEQ)
            {
                switch (flipthree)
                {
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
                if (flip)
                {
                    iirHighSampleLA = (iirHighSampleLA * (1.0 - iirAmountA)) + (highSampleL * iirAmountA);
                    highSampleL -= iirHighSampleLA;
                    iirLowSampleLA = (iirLowSampleLA * (1.0 - iirAmountB)) + (bassSampleL * iirAmountB);
                    bassSampleL = iirLowSampleLA;
                    iirHighSampleRA = (iirHighSampleRA * (1.0 - iirAmountA)) + (highSampleR * iirAmountA);
                    highSampleR -= iirHighSampleRA;
                    iirLowSampleRA = (iirLowSampleRA * (1.0 - iirAmountB)) + (bassSampleR * iirAmountB);
                    bassSampleR = iirLowSampleRA;
                }
                else
                {
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
            // EQ lowpass is after all processing like the compressor that might produce hash
            if (engageLowpass)
            {
                if (flip)
                {
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
                else
                {
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
} // namespace airwindohhs::eq
