#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::cstrip2 {

constexpr std::string_view k_name{ "CStrip2" };
constexpr std::string_view k_short_description{
    "CStrip2 refines and optimizes CStrip, by request!"
};
constexpr std::string_view k_long_description{
    "It's been a while since we've seen an Airwindows channel strip! Here's why CStrip2 is here.First, I was asked to do it. Specifically, I was asked to take the delay-trim and gate out of CStrip, and also to replace the highpass and lowpass with Capacitor, and keep the compressor exactly as it is, and also could I put a saturation effect on the end? That sort of thing doesn't always click with me, but hang on a moment, there's more.Second, we've got an Airwindows port to VCV Rack (which might expand to a CLAP, or more, along the same lines) but it's limited to ten controls. There are only two Airwindows plugins with more than ten controls. One is Pafnuty (which would be well suited to Rack or Rack-like environments). The other… is CStrip.Or WAS, because CStrip2 is here!There are also related things. It seems to me the EQ technique I use might fit in future versions of Console that include built-in EQ, and model famous recording desks, especially old ones. That's not to say the CStrip EQ is designed to do that, because it's not: but it covers some interesting bases, like saturating boosts to bring them forward and unsaturating cuts to drop them back, and the relatively shallow slopes lend themselves to fixed-frequency built in EQ bands. There are classic desk topologies where the channels and busses have idiosyncratic choices for the EQ bands, and to model that would tend to bring outputs into the realms of classic albums done on those desks. I've got more Console summing algorithms in the works to support this exploration.Oh, and that output saturation goes like this: 0 to 1/3 is dry signal, 1/3 to 2/3 crossfades into Spiral like it is on the plugin Channel (versions 7, 8 and 9 have this) and 2/3 to 1 crossfades into the Density algorithm for maximum fatness and drive. This is probably going to be fun for people to play with, or leave it below 1/3 if you want clean output.That's CStrip2! I hope you like it :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class CStrip2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double last2SampleL;
    double lastSampleR;
    double last2SampleR;
    // begin Capacitor
    double iirHighpassAL;
    double iirHighpassBL;
    double iirHighpassCL;
    double iirHighpassDL;
    double iirHighpassEL;
    double iirHighpassFL;
    double iirLowpassAL;
    double iirLowpassBL;
    double iirLowpassCL;
    double iirLowpassDL;
    double iirLowpassEL;
    double iirLowpassFL;
    double iirHighpassAR;
    double iirHighpassBR;
    double iirHighpassCR;
    double iirHighpassDR;
    double iirHighpassER;
    double iirHighpassFR;
    double iirLowpassAR;
    double iirLowpassBR;
    double iirLowpassCR;
    double iirLowpassDR;
    double iirLowpassER;
    double iirLowpassFR;
    int count;
    // end Capacitor
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
    bool flip;
    int flipthree;
    // end EQ
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

  public:
    CStrip2()
    {
        A = 0.5; // Treble -12 to 12
        B = 0.5; // Mid -12 to 12
        C = 0.5; // Bass -12 to 12
        D = 0.5; // TrebFrq 6.0 log 1 to 16 defaulting to 6K
        E = 0.5; // BassFrq 100.0 log 30 to 1600 defaulting to 100 hz
        F = 1.0; // Low Cap
        G = 0.0; // High Cap
        H = 0.0; // Compres 0-1
        I = 0.0; // CompSpd 0-1
        J = 0.33; // Output
        iirHighpassAL = 0.0;
        iirHighpassBL = 0.0;
        iirHighpassCL = 0.0;
        iirHighpassDL = 0.0;
        iirHighpassEL = 0.0;
        iirHighpassFL = 0.0;
        iirLowpassAL = 0.0;
        iirLowpassBL = 0.0;
        iirLowpassCL = 0.0;
        iirLowpassDL = 0.0;
        iirLowpassEL = 0.0;
        iirLowpassFL = 0.0;
        iirHighpassAR = 0.0;
        iirHighpassBR = 0.0;
        iirHighpassCR = 0.0;
        iirHighpassDR = 0.0;
        iirHighpassER = 0.0;
        iirHighpassFR = 0.0;
        iirLowpassAR = 0.0;
        iirLowpassBR = 0.0;
        iirLowpassCR = 0.0;
        iirLowpassDR = 0.0;
        iirLowpassER = 0.0;
        iirLowpassFR = 0.0;
        count = 0;
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
        flip = false;
        flipthree = 0;
        // end EQ
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
        kNumParameters = 10
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
        kParamE:
            E = value;
            break;
        kParamF:
            F = value;
            break;
        kParamG:
            G = value;
            break;
        kParamH:
            H = value;
            break;
        kParamI:
            I = value;
            break;
        kParamJ:
            J = value;
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
        kParamE:
            return E;
            break;
        kParamF:
            return F;
            break;
        kParamG:
            return G;
            break;
        kParamH:
            return H;
            break;
        kParamI:
            return I;
            break;
        kParamJ:
            return J;
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
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 0.5;
            break;
        kParamE:
            return 0.5;
            break;
        kParamF:
            return 1.0;
            break;
        kParamG:
            return 0.0;
            break;
        kParamH:
            return 0.0;
            break;
        kParamI:
            return 0.0;
            break;
        kParamJ:
            return 0.33;
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
            return "treble";
            break;
        kParamB:
            return "mid";
            break;
        kParamC:
            return "bass";
            break;
        kParamD:
            return "trebfrq";
            break;
        kParamE:
            return "bassfrq";
            break;
        kParamF:
            return "lowcap";
            break;
        kParamG:
            return "hicap";
            break;
        kParamH:
            return "compres";
            break;
        kParamI:
            return "compspd";
            break;
        kParamJ:
            return "output";
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
            return "Treble";
            break;
        kParamB:
            return "Mid";
            break;
        kParamC:
            return "Bass";
            break;
        kParamD:
            return "TrebFrq";
            break;
        kParamE:
            return "BassFrq";
            break;
        kParamF:
            return "LowCap";
            break;
        kParamG:
            return "HiCap";
            break;
        kParamH:
            return "Compres";
            break;
        kParamI:
            return "CompSpd";
            break;
        kParamJ:
            return "Output";
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
        kParamE:
            return std::to_string(E);
            break;
        kParamF:
            return std::to_string(F);
            break;
        kParamG:
            return std::to_string(G);
            break;
        kParamH:
            return std::to_string(H);
            break;
        kParamI:
            return std::to_string(I);
            break;
        kParamJ:
            return std::to_string(J);
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
            return "dB";
            break;
        kParamB:
            return "dB";
            break;
        kParamC:
            return "dB";
            break;
        kParamD:
            return "Khz";
            break;
        kParamE:
            return "hz";
            break;
        kParamF:
            return "";
            break;
        kParamG:
            return "";
            break;
        kParamH:
            return "";
            break;
        kParamI:
            return "";
            break;
        kParamJ:
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        double compscale = overallscale;
        overallscale = getSampleRate();
        compscale = compscale * overallscale;
        // compscale is the one that's 1 or something like 2.2 for 96K rates
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
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
        double iirAmountA = (((D * D * 15.0) + 1.0) * 1000) / overallscale;
        double iirAmountB = (((E * E * 1570.0) + 30.0) * 10) / overallscale;
        double bridgerectifier;
        double outA = fabs(densityA);
        double outB = fabs(densityB);
        double outC = fabs(densityC);
        // end EQ
        double lowpassAmount = pow(F, 2);
        double highpassAmount = pow(G, 2);
        bool engageCapacitor = false;
        if ((lowpassAmount < 1.0) || (highpassAmount > 0.0)) {
            engageCapacitor = true;
        }
        // end Capacitor
        // begin ButterComp
        double inputpos;
        double inputneg;
        double calcpos;
        double calcneg;
        double outputpos;
        double outputneg;
        double totalmultiplier;
        double inputgain = (pow(H, 4) * 35) + 1.0;
        double compoutgain = inputgain;
        compoutgain -= 1.0;
        compoutgain /= 1.2;
        compoutgain += 1.0;
        double divisor = (0.008 * pow(I, 2)) + 0.0004;
        // originally 0.012
        divisor /= compscale;
        double remainder = divisor;
        divisor = 1.0 - divisor;
        bool engageComp = false;
        if (inputgain > 1.0) {
            engageComp = true;
        }
        // end ButterComp
        double outputgain = J * 3.0; // 0-2
        double density = outputgain - 1.0; // allow for output 0-1 to be clean, 1-3 all boosts
        if (density < 0.0) {
            density = 0.0;
        }
        double phattity = density - 1.0;
        if (density > 1.0) {
            density = 1.0; // max out at full wet for Spiral aspect
        }
        if (phattity < 0.0) {
            phattity = 0.0;
        }
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
            if (engageCapacitor)
            {
                count++;
                if (count > 5) {
                    count = 0;
                }
                switch (count)
                {
                    case 0:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassBL = (iirHighpassBL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassBL;
                        iirLowpassBL = (iirLowpassBL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassBL;
                        iirHighpassDL = (iirHighpassDL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassDL;
                        iirLowpassDL = (iirLowpassDL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassDL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassBR = (iirHighpassBR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassBR;
                        iirLowpassBR = (iirLowpassBR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassBR;
                        iirHighpassDR = (iirHighpassDR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassDR;
                        iirLowpassDR = (iirLowpassDR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassDR;
                        break;
                    case 1:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassCL = (iirHighpassCL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassCL;
                        iirLowpassCL = (iirLowpassCL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassCL;
                        iirHighpassEL = (iirHighpassEL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassEL;
                        iirLowpassEL = (iirLowpassEL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassEL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassCR = (iirHighpassCR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassCR;
                        iirLowpassCR = (iirLowpassCR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassCR;
                        iirHighpassER = (iirHighpassER * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassER;
                        iirLowpassER = (iirLowpassER * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassER;
                        break;
                    case 2:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassBL = (iirHighpassBL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassBL;
                        iirLowpassBL = (iirLowpassBL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassBL;
                        iirHighpassFL = (iirHighpassFL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassFL;
                        iirLowpassFL = (iirLowpassFL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassFL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassBR = (iirHighpassBR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassBR;
                        iirLowpassBR = (iirLowpassBR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassBR;
                        iirHighpassFR = (iirHighpassFR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassFR;
                        iirLowpassFR = (iirLowpassFR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassFR;
                        break;
                    case 3:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassCL = (iirHighpassCL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassCL;
                        iirLowpassCL = (iirLowpassCL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassCL;
                        iirHighpassDL = (iirHighpassDL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassDL;
                        iirLowpassDL = (iirLowpassDL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassDL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassCR = (iirHighpassCR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassCR;
                        iirLowpassCR = (iirLowpassCR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassCR;
                        iirHighpassDR = (iirHighpassDR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassDR;
                        iirLowpassDR = (iirLowpassDR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassDR;
                        break;
                    case 4:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassBL = (iirHighpassBL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassBL;
                        iirLowpassBL = (iirLowpassBL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassBL;
                        iirHighpassEL = (iirHighpassEL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassEL;
                        iirLowpassEL = (iirLowpassEL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassEL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassBR = (iirHighpassBR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassBR;
                        iirLowpassBR = (iirLowpassBR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassBR;
                        iirHighpassER = (iirHighpassER * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassER;
                        iirLowpassER = (iirLowpassER * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassER;
                        break;
                    case 5:
                        iirHighpassAL = (iirHighpassAL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassAL;
                        iirLowpassAL = (iirLowpassAL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassAL;
                        iirHighpassCL = (iirHighpassCL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassCL;
                        iirLowpassCL = (iirLowpassCL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassCL;
                        iirHighpassFL = (iirHighpassFL * (1.0 - highpassAmount)) + (inputSampleL * highpassAmount);
                        inputSampleL -= iirHighpassFL;
                        iirLowpassFL = (iirLowpassFL * (1.0 - lowpassAmount)) + (inputSampleL * lowpassAmount);
                        inputSampleL = iirLowpassFL;
                        iirHighpassAR = (iirHighpassAR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassAR;
                        iirLowpassAR = (iirLowpassAR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassAR;
                        iirHighpassCR = (iirHighpassCR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassCR;
                        iirLowpassCR = (iirLowpassCR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassCR;
                        iirHighpassFR = (iirHighpassFR * (1.0 - highpassAmount)) + (inputSampleR * highpassAmount);
                        inputSampleR -= iirHighpassFR;
                        iirLowpassFR = (iirLowpassFR * (1.0 - lowpassAmount)) + (inputSampleR * lowpassAmount);
                        inputSampleR = iirLowpassFR;
                        break;
                }
                // Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
                // steepens the filter after minimizing artifacts.
            }
            // begin compressor
            if (engageComp)
            {
                flip = !flip;
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
                if (inputSampleL > 0)
                { // working on pos
                    if (true == flip)
                    {
                        controlAposL *= divisor;
                        controlAposL += (calcpos * remainder);
                    }
                    else
                    {
                        controlBposL *= divisor;
                        controlBposL += (calcpos * remainder);
                    }
                }
                else
                { // working on neg
                    if (true == flip)
                    {
                        controlAnegL *= divisor;
                        controlAnegL += (calcneg * remainder);
                    }
                    else
                    {
                        controlBnegL *= divisor;
                        controlBnegL += (calcneg * remainder);
                    }
                }
                // this causes each of the four to update only when active and in the correct 'flip'
                if (true == flip)
                {
                    totalmultiplier = (controlAposL * outputpos) + (controlAnegL * outputneg);
                }
                else
                {
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
                if (inputSampleR > 0)
                { // working on pos
                    if (true == flip)
                    {
                        controlAposR *= divisor;
                        controlAposR += (calcpos * remainder);
                    }
                    else
                    {
                        controlBposR *= divisor;
                        controlBposR += (calcpos * remainder);
                    }
                }
                else
                { // working on neg
                    if (true == flip)
                    {
                        controlAnegR *= divisor;
                        controlAnegR += (calcneg * remainder);
                    }
                    else
                    {
                        controlBnegR *= divisor;
                        controlBnegR += (calcneg * remainder);
                    }
                }
                // this causes each of the four to update only when active and in the correct 'flip'
                if (true == flip)
                {
                    totalmultiplier = (controlAposR * outputpos) + (controlAnegR * outputneg);
                }
                else
                {
                    totalmultiplier = (controlBposR * outputpos) + (controlBnegR * outputneg);
                }
                // this combines the sides according to flip, blending relative to the input value
                inputSampleR *= totalmultiplier;
                inputSampleR /= compoutgain;
                // end R
            }
            // end compressor
            // begin EQ
            if (engageEQ)
            {
                last2SampleL = lastSampleL;
                lastSampleL = inputSampleL;
                last2SampleR = lastSampleR;
                lastSampleR = inputSampleR;
                flipthree++;
                if (flipthree < 1 || flipthree > 3) {
                    flipthree = 1;
                }
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
            // built in output trim and dry/wet if desired
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
                double rawSampleL = inputSampleL; // Spiral crossfades from unity gain to boosted
                double rawSampleR = inputSampleR; // Spiral crossfades from unity gain to boosted
                if (inputSampleL > 1.0) {
                    inputSampleL = 1.0;
                }
                if (inputSampleL < -1.0) {
                    inputSampleL = -1.0; // all boosts clipped to 0dB
                }
                if (inputSampleR > 1.0) {
                    inputSampleR = 1.0;
                }
                if (inputSampleR < -1.0) {
                    inputSampleR = -1.0; // all boosts clipped to 0dB
                }
                double phatSampleL = sin(inputSampleL * 1.57079633);
                double phatSampleR = sin(inputSampleR * 1.57079633);
                inputSampleL *= 1.2533141373155;
                inputSampleR *= 1.2533141373155;
                // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
                double distSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1.0 : fabs(inputSampleL));
                double distSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1.0 : fabs(inputSampleR));
                inputSampleL = distSampleL; // purest form is full Spiral
                inputSampleR = distSampleR; // purest form is full Spiral
                if (density < 1.0) {
                    inputSampleL = (rawSampleL * (1.0 - density)) + (distSampleL * density);
                    inputSampleR = (rawSampleR * (1.0 - density)) + (distSampleR * density);
                } // fade Spiral aspect
                if (phattity > 0.0) {
                    inputSampleL = (inputSampleL * (1.0 - phattity)) + (phatSampleL * phattity);
                    inputSampleR = (inputSampleR * (1.0 - phattity)) + (phatSampleR * phattity);
                } // apply original Density on top
                // output section is a pad from 0 to 1, Spiral from 1 to 2, Density from 2 to 3
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
} // namespace airwindohhs::cstrip2
