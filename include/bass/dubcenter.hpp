#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubcenter {

constexpr std::string_view k_name{ "DubCenter" };
constexpr std::string_view k_short_description{
    "DubCenter is a version of DubSub where the bass reinforcement is purely mono."
};
constexpr std::string_view k_long_description{
    "So here’s DubSub (below) with mono bass. This isn’t the last you’ll hear of this tool as I have BassKit coming out (which is the more approachable, well-behaved version of DubCenter) but this is the one that will let you get the most extreme. If you were using DubSub to its fullest, this one lets you do the same only with the bass and sub outputs centered.The reason you’d want to do that is, whether for sound reinforcement or vinyl mastering there’s little reason to have stereo bass. It just makes the woofers fight each other, below a certain frequency (which depends on how far apart your speakers are). This is why elliptical EQs exist.And the thing with DubCenter is, you don’t have to filter the original audio or mid/side it! All you have to do is use DubCenter to reinforce the bass, and it’ll automatically make that added content mono. This is even better than using (for instance) ToTape and its head bump mechanics to reinforce bass, because that (like a real tape deck) produces a stereo head bump. This produces the same fullness with the same algorithm, but it’s strictly mono so you get the effect of an elliptical EQ without having to run one! Only the super-deep stuff gets reinforced and the information and phase relationships of your original mix go untampered with.Again, BassKit will do this in a super-convenient way with much of the tweeky functionality simplified or taken out (for that one, there is no chance of abusing the sub-octave to do weird stuff as it’s restricted to only convincing subs content) but DubCenter is the one like DubSub, where you can make it do crazy things. You’ll find it in your plugin menu next to DubSub, most likely. Have fun!"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class DubCenter final : public Effect<T>
{
    double iirDriveSampleAL;
    double iirDriveSampleBL;
    double iirDriveSampleCL;
    double iirDriveSampleDL;
    double iirDriveSampleEL;
    double iirDriveSampleFL;
    double iirDriveSampleAR;
    double iirDriveSampleBR;
    double iirDriveSampleCR;
    double iirDriveSampleDR;
    double iirDriveSampleER;
    double iirDriveSampleFR;
    bool flip; // drive things
    int bflip;
    bool WasNegative;
    bool SubOctave;
    double iirHeadBumpA;
    double iirHeadBumpB;
    double iirHeadBumpC;
    double iirSubBumpA;
    double iirSubBumpB;
    double iirSubBumpC;
    double lastHeadBump;
    double lastSubBump;
    double iirSampleA;
    double iirSampleB;
    double iirSampleC;
    double iirSampleD;
    double iirSampleE;
    double iirSampleF;
    double iirSampleG;
    double iirSampleH;
    double iirSampleI;
    double iirSampleJ;
    double iirSampleK;
    double iirSampleL;
    double iirSampleM;
    double iirSampleN;
    double iirSampleO;
    double iirSampleP;
    double iirSampleQ;
    double iirSampleR;
    double iirSampleS;
    double iirSampleT;
    double iirSampleU;
    double iirSampleV;
    double iirSampleW;
    double iirSampleX;
    double iirSampleY;
    double iirSampleZ;
    double oscGate;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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
    DubCenter()
    {
        A = 0.9;
        B = 0.5;
        C = 0.74;
        D = 1.0;
        E = 0.95;
        F = 0.5;
        G = 0.2;
        H = 0.2;
        I = 0.5;
        J = 1.0;
        WasNegative = false;
        SubOctave = false;
        flip = false;
        bflip = 0;
        iirDriveSampleAL = 0.0;
        iirDriveSampleBL = 0.0;
        iirDriveSampleCL = 0.0;
        iirDriveSampleDL = 0.0;
        iirDriveSampleEL = 0.0;
        iirDriveSampleFL = 0.0;
        iirDriveSampleAR = 0.0;
        iirDriveSampleBR = 0.0;
        iirDriveSampleCR = 0.0;
        iirDriveSampleDR = 0.0;
        iirDriveSampleER = 0.0;
        iirDriveSampleFR = 0.0;
        iirHeadBumpA = 0.0;
        iirHeadBumpB = 0.0;
        iirHeadBumpC = 0.0;
        iirSubBumpA = 0.0;
        iirSubBumpB = 0.0;
        iirSubBumpC = 0.0;
        lastHeadBump = 0.0;
        lastSubBump = 0.0;
        iirSampleA = 0.0;
        iirSampleB = 0.0;
        iirSampleC = 0.0;
        iirSampleD = 0.0;
        iirSampleE = 0.0;
        iirSampleF = 0.0;
        iirSampleG = 0.0;
        iirSampleH = 0.0;
        iirSampleI = 0.0;
        iirSampleJ = 0.0;
        iirSampleK = 0.0;
        iirSampleL = 0.0;
        iirSampleM = 0.0;
        iirSampleN = 0.0;
        iirSampleO = 0.0;
        iirSampleP = 0.0;
        iirSampleQ = 0.0;
        iirSampleR = 0.0;
        iirSampleS = 0.0;
        iirSampleT = 0.0;
        iirSampleU = 0.0;
        iirSampleV = 0.0;
        iirSampleW = 0.0;
        iirSampleX = 0.0;
        iirSampleY = 0.0;
        iirSampleZ = 0.0;
        oscGate = 1.0;
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
            return 0.9;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.74;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
            return 0.95;
            break;
        kParamF:
            return 0.5;
            break;
        kParamG:
            return 0.2;
            break;
        kParamH:
            return 0.2;
            break;
        kParamI:
            return 0.5;
            break;
        kParamJ:
            return 1.0;
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
            return "tgrind";
            break;
        kParamB:
            return "grd/out";
            break;
        kParamC:
            return "xover";
            break;
        kParamD:
            return "bsdrive";
            break;
        kParamE:
            return "bsvoice";
            break;
        kParamF:
            return "bassout";
            break;
        kParamG:
            return "sbdrive";
            break;
        kParamH:
            return "sbvoice";
            break;
        kParamI:
            return "subout";
            break;
        kParamJ:
            return "dry/wet";
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
            return "TGrind";
            break;
        kParamB:
            return "Grd/Out";
            break;
        kParamC:
            return "XOver";
            break;
        kParamD:
            return "BsDrive";
            break;
        kParamE:
            return "BsVoice";
            break;
        kParamF:
            return "BassOut";
            break;
        kParamG:
            return "SbDrive";
            break;
        kParamH:
            return "SbVoice";
            break;
        kParamI:
            return "SubOut";
            break;
        kParamJ:
            return "Dry/Wet";
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
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;
        kParamE:
            return "";
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
        overallscale *= Effect<T>::getSampleRate();
        double driveone = pow(A * 3.0, 2);
        double driveoutput = (B * 2.0) - 1.0;
        double iirAmount = ((C * 0.33) + 0.1) / overallscale;
        double ataLowpassL;
        double ataLowpassR;
        double ataLowpass;
        double randy;
        double invrandy;
        double HeadBump = 0.0;
        double BassGain = D * 0.1;
        double HeadBumpFreq = ((E * 0.1) + 0.0001) / overallscale;
        double iirBmount = HeadBumpFreq / 44.1;
        double altBmount = 1.0 - iirBmount;
        double BassOutGain = (F * 2.0) - 1.0;
        double SubBump = 0.0;
        double SubGain = G * 0.1;
        double SubBumpFreq = ((H * 0.1) + 0.0001) / overallscale;
        double iirCmount = SubBumpFreq / 44.1;
        double altCmount = 1.0 - iirCmount;
        double SubOutGain = (I * 2.0) - 1.0;
        double clamp = 0.0;
        double out;
        double fuzz = 0.111;
        double wet = J;
        double glitch = 0.60;
        double tempSample;
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
            // here's the plan.
            // Grind Boost
            // Grind Output Level
            // Bass Split Freq
            // Bass Drive
            // Bass Voicing
            // Bass Output Level
            // Sub Oct Drive
            // Sub Voicing
            // Sub Output Level
            // Dry/Wet
            oscGate += fabs((inputSampleL + inputSampleR) * 5.0);
            oscGate -= 0.001;
            if (oscGate > 1.0) {
                oscGate = 1.0;
            }
            if (oscGate < 0) {
                oscGate = 0;
            }
            // got a value that only goes down low when there's silence or near silence on input
            clamp = 1.0 - oscGate;
            clamp *= 0.00001;
            // set up the thing to choke off oscillations- belt and suspenders affair
            if (flip)
            {
                tempSample = inputSampleL;
                iirDriveSampleAL = (iirDriveSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleAL;
                iirDriveSampleCL = (iirDriveSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleCL;
                iirDriveSampleEL = (iirDriveSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleEL;
                ataLowpassL = tempSample - inputSampleL;
                tempSample = inputSampleR;
                iirDriveSampleAR = (iirDriveSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleAR;
                iirDriveSampleCR = (iirDriveSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleCR;
                iirDriveSampleER = (iirDriveSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleER;
                ataLowpassR = tempSample - inputSampleR;
            }
            else
            {
                tempSample = inputSampleL;
                iirDriveSampleBL = (iirDriveSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleBL;
                iirDriveSampleDL = (iirDriveSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleDL;
                iirDriveSampleFL = (iirDriveSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL -= iirDriveSampleFL;
                ataLowpassL = tempSample - inputSampleL;
                tempSample = inputSampleR;
                iirDriveSampleBR = (iirDriveSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleBR;
                iirDriveSampleDR = (iirDriveSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleDR;
                iirDriveSampleFR = (iirDriveSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR -= iirDriveSampleFR;
                ataLowpassR = tempSample - inputSampleR;
            }
            // highpass section
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            out = driveone;
            while (out > glitch)
            {
                out -= glitch;
                inputSampleL -= (inputSampleL * (fabs(inputSampleL) * glitch) * (fabs(inputSampleL) * glitch));
                inputSampleL *= (1.0 + glitch);
            }
            // that's taken care of the really high gain stuff
            inputSampleL -= (inputSampleL * (fabs(inputSampleL) * out) * (fabs(inputSampleL) * out));
            inputSampleL *= (1.0 + out);
            out = driveone;
            while (out > glitch)
            {
                out -= glitch;
                inputSampleR -= (inputSampleR * (fabs(inputSampleR) * glitch) * (fabs(inputSampleR) * glitch));
                inputSampleR *= (1.0 + glitch);
            }
            // that's taken care of the really high gain stuff
            inputSampleR -= (inputSampleR * (fabs(inputSampleR) * out) * (fabs(inputSampleR) * out));
            inputSampleR *= (1.0 + out);
            ataLowpass = (ataLowpassL + ataLowpassR) / 2.0;
            if (ataLowpass > 0)
            {
                if (WasNegative) {
                    SubOctave = !SubOctave;
                }
                WasNegative = false;
            }
            else {
                WasNegative = true;
            }
            // set up polarities for sub-bass version
            randy = (double(fpdL) / UINT32_MAX) * fuzz; // 0 to 1 the noise, may not be needed
            invrandy = (1.0 - randy);
            randy /= 2.0;
            // set up the noise
            iirSampleA = (iirSampleA * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleA;
            iirSampleB = (iirSampleB * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleB;
            iirSampleC = (iirSampleC * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleC;
            iirSampleD = (iirSampleD * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleD;
            iirSampleE = (iirSampleE * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleE;
            iirSampleF = (iirSampleF * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleF;
            iirSampleG = (iirSampleG * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleG;
            iirSampleH = (iirSampleH * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleH;
            iirSampleI = (iirSampleI * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleI;
            iirSampleJ = (iirSampleJ * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleJ;
            iirSampleK = (iirSampleK * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleK;
            iirSampleL = (iirSampleL * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleL;
            iirSampleM = (iirSampleM * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleM;
            iirSampleN = (iirSampleN * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleN;
            iirSampleO = (iirSampleO * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleO;
            iirSampleP = (iirSampleP * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleP;
            iirSampleQ = (iirSampleQ * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleQ;
            iirSampleR = (iirSampleR * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleR;
            iirSampleS = (iirSampleS * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleS;
            iirSampleT = (iirSampleT * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleT;
            iirSampleU = (iirSampleU * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleU;
            iirSampleV = (iirSampleV * altBmount) + (ataLowpass * iirBmount);
            ataLowpass -= iirSampleV;
            switch (bflip)
            {
                case 1:
                    iirHeadBumpA += (ataLowpass * BassGain);
                    iirHeadBumpA -= (iirHeadBumpA * iirHeadBumpA * iirHeadBumpA * HeadBumpFreq);
                    iirHeadBumpA = (invrandy * iirHeadBumpA) + (randy * iirHeadBumpB) + (randy * iirHeadBumpC);
                    if (iirHeadBumpA > 0) {
                        iirHeadBumpA -= clamp;
                    }
                    if (iirHeadBumpA < 0) {
                        iirHeadBumpA += clamp;
                    }
                    HeadBump = iirHeadBumpA;
                    break;
                case 2:
                    iirHeadBumpB += (ataLowpass * BassGain);
                    iirHeadBumpB -= (iirHeadBumpB * iirHeadBumpB * iirHeadBumpB * HeadBumpFreq);
                    iirHeadBumpB = (randy * iirHeadBumpA) + (invrandy * iirHeadBumpB) + (randy * iirHeadBumpC);
                    if (iirHeadBumpB > 0) {
                        iirHeadBumpB -= clamp;
                    }
                    if (iirHeadBumpB < 0) {
                        iirHeadBumpB += clamp;
                    }
                    HeadBump = iirHeadBumpB;
                    break;
                case 3:
                    iirHeadBumpC += (ataLowpass * BassGain);
                    iirHeadBumpC -= (iirHeadBumpC * iirHeadBumpC * iirHeadBumpC * HeadBumpFreq);
                    iirHeadBumpC = (randy * iirHeadBumpA) + (randy * iirHeadBumpB) + (invrandy * iirHeadBumpC);
                    if (iirHeadBumpC > 0) {
                        iirHeadBumpC -= clamp;
                    }
                    if (iirHeadBumpC < 0) {
                        iirHeadBumpC += clamp;
                    }
                    HeadBump = iirHeadBumpC;
                    break;
            }
            iirSampleW = (iirSampleW * altBmount) + (HeadBump * iirBmount);
            HeadBump -= iirSampleW;
            iirSampleX = (iirSampleX * altBmount) + (HeadBump * iirBmount);
            HeadBump -= iirSampleX;
            SubBump = HeadBump;
            iirSampleY = (iirSampleY * altCmount) + (SubBump * iirCmount);
            SubBump -= iirSampleY;
            SubBump = fabs(SubBump);
            if (SubOctave == false) {
                SubBump = -SubBump;
            }
            switch (bflip)
            {
                case 1:
                    iirSubBumpA += (SubBump * SubGain);
                    iirSubBumpA -= (iirSubBumpA * iirSubBumpA * iirSubBumpA * SubBumpFreq);
                    iirSubBumpA = (invrandy * iirSubBumpA) + (randy * iirSubBumpB) + (randy * iirSubBumpC);
                    if (iirSubBumpA > 0) {
                        iirSubBumpA -= clamp;
                    }
                    if (iirSubBumpA < 0) {
                        iirSubBumpA += clamp;
                    }
                    SubBump = iirSubBumpA;
                    break;
                case 2:
                    iirSubBumpB += (SubBump * SubGain);
                    iirSubBumpB -= (iirSubBumpB * iirSubBumpB * iirSubBumpB * SubBumpFreq);
                    iirSubBumpB = (randy * iirSubBumpA) + (invrandy * iirSubBumpB) + (randy * iirSubBumpC);
                    if (iirSubBumpB > 0) {
                        iirSubBumpB -= clamp;
                    }
                    if (iirSubBumpB < 0) {
                        iirSubBumpB += clamp;
                    }
                    SubBump = iirSubBumpB;
                    break;
                case 3:
                    iirSubBumpC += (SubBump * SubGain);
                    iirSubBumpC -= (iirSubBumpC * iirSubBumpC * iirSubBumpC * SubBumpFreq);
                    iirSubBumpC = (randy * iirSubBumpA) + (randy * iirSubBumpB) + (invrandy * iirSubBumpC);
                    if (iirSubBumpC > 0) {
                        iirSubBumpC -= clamp;
                    }
                    if (iirSubBumpC < 0) {
                        iirSubBumpC += clamp;
                    }
                    SubBump = iirSubBumpC;
                    break;
            }
            iirSampleZ = (iirSampleZ * altCmount) + (SubBump * iirCmount);
            SubBump -= iirSampleZ;
            inputSampleL *= driveoutput; // start with the drive section then add lows and subs
            inputSampleR *= driveoutput; // start with the drive section then add lows and subs
            inputSampleL += ((HeadBump + lastHeadBump) * BassOutGain);
            inputSampleL += ((SubBump + lastSubBump) * SubOutGain);
            inputSampleR += ((HeadBump + lastHeadBump) * BassOutGain);
            inputSampleR += ((SubBump + lastSubBump) * SubOutGain);
            lastHeadBump = HeadBump;
            lastSubBump = SubBump;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
            flip = !flip;
            bflip++;
            if (bflip < 1 || bflip > 3) {
                bflip = 1;
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
} // namespace airwindohhs::dubcenter
