#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::woodenbox {

constexpr std::string_view k_name{ "WoodenBox" };
constexpr std::string_view k_short_description{
    "WoodenBox is like a miniature reverb for converting DI to acoustic."
};
constexpr std::string_view k_long_description{
    "The request was for a plugin to convert an electric guitar sound (presumably DI?) to the sound of an acoustic.In a very abstract way, this might be the answer for that?It's more complicated, though, because as usual I'm exploring the larger ideas around that. The very most obvious thing to do would be 'take the impulse response of an acoustic guitar, maybe if you're feeling ambitious try to strip out the original electric's tone quality or put big tone controls on or something, done'. But it might not be that simple…What's an acoustic guitar sound, anyway? Vibrations of wood, extra sonorities, resonances. Except those are called 'wolf tones' and are always bad. And anything we add along those lines will be other forms of wolf tones or resonator-guitar clangs, and will be bad. So what then?One thing about an acoustic guitar is, it's also a miniature room made out of wood. It vibrates, but also it reverberates. And I've been putting all this work into reverbs over the years… so what if I run the same grueling search for interesting, well-balanced rooms, but on miniature spaces? (never mind that I've only recently discovered new ways for the rooms to be better balanced!)And so we have WoodenBox. Like the older reverbs based on ClearCoat, it has a bunch of different spaces/colors on tap. But they're all extremely tiny compared to even a small room. There's enough going on in them to produce a vague stereo-ish quality, but not so much that it's a chorus effect. The tone is dense, confined: it doesn't replace the need for a room or chamber sound. But it doctors and reshapes the tone in the way that a simple room reverb never would. And it can be used on many things beyond guitars… for instance, synth patches, or perhaps electronic drums.I don't know quite where this leads, but it's an interesting start to have made. People who were asking to get smaller rooms out of me… well, you're still getting those, but first you get stuck in a wooden box :)"
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class WoodenBox final : public Effect<T>
{
    float A;
    float B;
    float C;
    int c4AL, c4BL, c4CL, c4DL, c4EL, c4FL, c4GL, c4HL;
    int c4IL, c4JL, c4KL, c4LL, c4ML, c4NL, c4OL, c4PL;
    int c4AR, c4BR, c4CR, c4DR, c4ER, c4FR, c4GR, c4HR;
    int c4IR, c4JR, c4KR, c4LR, c4MR, c4NR, c4OR, c4PR;
    // base stereo reverb
    double b4AL[d4A + 5];
    double b4BL[d4B + 5];
    double b4CL[d4C + 5];
    double b4DL[d4D + 5];
    double b4EL[d4E + 5];
    double b4FL[d4F + 5];
    double b4GL[d4G + 5];
    double b4HL[d4H + 5];
    double b4IL[d4I + 5];
    double b4JL[d4J + 5];
    double b4KL[d4K + 5];
    double b4LL[d4L + 5];
    double b4ML[d4M + 5];
    double b4NL[d4N + 5];
    double b4OL[d4O + 5];
    double b4PL[d4P + 5];
    double b4AR[d4A + 5];
    double b4BR[d4B + 5];
    double b4CR[d4C + 5];
    double b4DR[d4D + 5];
    double b4ER[d4E + 5];
    double b4FR[d4F + 5];
    double b4GR[d4G + 5];
    double b4HR[d4H + 5];
    double b4IR[d4I + 5];
    double b4JR[d4J + 5];
    double b4KR[d4K + 5];
    double b4LR[d4L + 5];
    double b4MR[d4M + 5];
    double b4NR[d4N + 5];
    double b4OR[d4O + 5];
    double b4PR[d4P + 5];
    double g4AL, g4BL, g4CL, g4DL, g4DR, g4HR, g4LR, g4PR;
    // changed letter is the dual mono, with rearranged grid
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];
    int shortA;
    int shortB;
    int shortC;
    int shortD;
    int shortE;
    int shortF;
    int shortG;
    int shortH;
    int shortI;
    int shortJ;
    int shortK;
    int shortL;
    int shortM;
    int shortN;
    int shortO;
    int shortP;
    int prevclearcoat;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    WoodenBox()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        c4AL = c4BL = c4CL = c4DL = c4EL = c4FL = c4GL = c4HL = 1;
        c4IL = c4JL = c4KL = c4LL = c4ML = c4NL = c4OL = c4PL = 1;
        c4AR = c4BR = c4CR = c4DR = c4ER = c4FR = c4GR = c4HR = 1;
        c4IR = c4JR = c4KR = c4LR = c4MR = c4NR = c4OR = c4PR = 1;
        for (int x = 0; x < d4A + 2; x++) {
            b4AL[x] = 0.0;
            b4AR[x] = 0.0;
        }
        for (int x = 0; x < d4B + 2; x++) {
            b4BL[x] = 0.0;
            b4BR[x] = 0.0;
        }
        for (int x = 0; x < d4C + 2; x++) {
            b4CL[x] = 0.0;
            b4CR[x] = 0.0;
        }
        for (int x = 0; x < d4D + 2; x++) {
            b4DL[x] = 0.0;
            b4DR[x] = 0.0;
        }
        for (int x = 0; x < d4E + 2; x++) {
            b4EL[x] = 0.0;
            b4ER[x] = 0.0;
        }
        for (int x = 0; x < d4F + 2; x++) {
            b4FL[x] = 0.0;
            b4FR[x] = 0.0;
        }
        for (int x = 0; x < d4G + 2; x++) {
            b4GL[x] = 0.0;
            b4GR[x] = 0.0;
        }
        for (int x = 0; x < d4H + 2; x++) {
            b4HL[x] = 0.0;
            b4HR[x] = 0.0;
        }
        for (int x = 0; x < d4I + 2; x++) {
            b4IL[x] = 0.0;
            b4IR[x] = 0.0;
        }
        for (int x = 0; x < d4J + 2; x++) {
            b4JL[x] = 0.0;
            b4JR[x] = 0.0;
        }
        for (int x = 0; x < d4K + 2; x++) {
            b4KL[x] = 0.0;
            b4KR[x] = 0.0;
        }
        for (int x = 0; x < d4L + 2; x++) {
            b4LL[x] = 0.0;
            b4LR[x] = 0.0;
        }
        for (int x = 0; x < d4M + 2; x++) {
            b4ML[x] = 0.0;
            b4MR[x] = 0.0;
        }
        for (int x = 0; x < d4N + 2; x++) {
            b4NL[x] = 0.0;
            b4NR[x] = 0.0;
        }
        for (int x = 0; x < d4O + 2; x++) {
            b4OL[x] = 0.0;
            b4OR[x] = 0.0;
        }
        for (int x = 0; x < d4P + 2; x++) {
            b4PL[x] = 0.0;
            b4PR[x] = 0.0;
        }
        g4AL = g4BL = g4CL = g4DL = 0.0;
        g4DR = g4HR = g4LR = g4PR = 0.0;
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
        shortA = 173;
        shortB = 82;
        shortC = 240;
        shortD = 191;
        shortE = 196;
        shortF = 257;
        shortG = 203;
        shortH = 252;
        shortI = 207;
        shortJ = 203;
        shortK = 250;
        shortL = 220;
        shortM = 261;
        shortN = 235;
        shortO = 161;
        shortP = 161;
        prevclearcoat = -1;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "select"; break;
            case kParamB: return "reso"; break;
            case kParamC: return "depth"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Select"; break;
            case kParamB: return "Reso"; break;
            case kParamC: return "Depth"; break;

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
        int clearcoat = (int)(A * 16.999);
        if (clearcoat != prevclearcoat) {
            for (int count = 0; count < d4A + 2; count++) {
                b4AL[count] = 0.0;
                b4AR[count] = 0.0;
            }
            for (int count = 0; count < d4B + 2; count++) {
                b4BL[count] = 0.0;
                b4BR[count] = 0.0;
            }
            for (int count = 0; count < d4C + 2; count++) {
                b4CL[count] = 0.0;
                b4CR[count] = 0.0;
            }
            for (int count = 0; count < d4D + 2; count++) {
                b4DL[count] = 0.0;
                b4DR[count] = 0.0;
            }
            for (int count = 0; count < d4E + 2; count++) {
                b4EL[count] = 0.0;
                b4ER[count] = 0.0;
            }
            for (int count = 0; count < d4F + 2; count++) {
                b4FL[count] = 0.0;
                b4FR[count] = 0.0;
            }
            for (int count = 0; count < d4G + 2; count++) {
                b4GL[count] = 0.0;
                b4GR[count] = 0.0;
            }
            for (int count = 0; count < d4H + 2; count++) {
                b4HL[count] = 0.0;
                b4HR[count] = 0.0;
            }
            for (int count = 0; count < d4I + 2; count++) {
                b4IL[count] = 0.0;
                b4IR[count] = 0.0;
            }
            for (int count = 0; count < d4J + 2; count++) {
                b4JL[count] = 0.0;
                b4JR[count] = 0.0;
            }
            for (int count = 0; count < d4K + 2; count++) {
                b4KL[count] = 0.0;
                b4KR[count] = 0.0;
            }
            for (int count = 0; count < d4L + 2; count++) {
                b4LL[count] = 0.0;
                b4LR[count] = 0.0;
            }
            for (int count = 0; count < d4M + 2; count++) {
                b4ML[count] = 0.0;
                b4MR[count] = 0.0;
            }
            for (int count = 0; count < d4N + 2; count++) {
                b4NL[count] = 0.0;
                b4NR[count] = 0.0;
            }
            for (int count = 0; count < d4O + 2; count++) {
                b4OL[count] = 0.0;
                b4OR[count] = 0.0;
            }
            for (int count = 0; count < d4P + 2; count++) {
                b4PL[count] = 0.0;
                b4PR[count] = 0.0;
            }
            c4AL = 1;
            c4BL = 1;
            c4CL = 1;
            c4DL = 1;
            c4EL = 1;
            c4FL = 1;
            c4GL = 1;
            c4HL = 1;
            c4IL = 1;
            c4JL = 1;
            c4KL = 1;
            c4LL = 1;
            c4ML = 1;
            c4NL = 1;
            c4OL = 1;
            c4PL = 1;
            c4AR = 1;
            c4BR = 1;
            c4CR = 1;
            c4DR = 1;
            c4ER = 1;
            c4FR = 1;
            c4GR = 1;
            c4HR = 1;
            c4IR = 1;
            c4JR = 1;
            c4KR = 1;
            c4LR = 1;
            c4MR = 1;
            c4NR = 1;
            c4OR = 1;
            c4PR = 1;
            switch (clearcoat) {
                case 0:
                    shortA = 17;
                    shortB = 10;
                    shortC = 23;
                    shortD = 3;
                    shortE = 8;
                    shortF = 7;
                    shortG = 41;
                    shortH = 6;
                    shortI = 3;
                    shortJ = 6;
                    shortK = 59;
                    shortL = 61;
                    shortM = 4;
                    shortN = 71;
                    shortO = 5;
                    shortP = 4;
                    break; // 0 to 4 ms, 0 seat room
                case 1:
                    shortA = 12;
                    shortB = 19;
                    shortC = 89;
                    shortD = 25;
                    shortE = 92;
                    shortF = 8;
                    shortG = 41;
                    shortH = 11;
                    shortI = 80;
                    shortJ = 27;
                    shortK = 6;
                    shortL = 4;
                    shortM = 3;
                    shortN = 21;
                    shortO = 7;
                    shortP = 63;
                    break; // 0 to 7 ms, 1 seat room
                case 2:
                    shortA = 35;
                    shortB = 19;
                    shortC = 5;
                    shortD = 7;
                    shortE = 15;
                    shortF = 7;
                    shortG = 41;
                    shortH = 191;
                    shortI = 177;
                    shortJ = 3;
                    shortK = 6;
                    shortL = 22;
                    shortM = 23;
                    shortN = 118;
                    shortO = 4;
                    shortP = 79;
                    break; // 0 to 11 ms, 4 seat room
                case 3:
                    shortA = 17;
                    shortB = 19;
                    shortC = 105;
                    shortD = 135;
                    shortE = 31;
                    shortF = 86;
                    shortG = 41;
                    shortH = 16;
                    shortI = 3;
                    shortJ = 16;
                    shortK = 6;
                    shortL = 151;
                    shortM = 147;
                    shortN = 26;
                    shortO = 3;
                    shortP = 10;
                    break; // 0 to 11 ms, 4 seat room
                case 4:
                    shortA = 134;
                    shortB = 13;
                    shortC = 26;
                    shortD = 10;
                    shortE = 34;
                    shortF = 24;
                    shortG = 4;
                    shortH = 60;
                    shortI = 88;
                    shortJ = 9;
                    shortK = 155;
                    shortL = 11;
                    shortM = 3;
                    shortN = 18;
                    shortO = 9;
                    shortP = 161;
                    break; // 0 to 11 ms, 4 seat room
                case 5:
                    shortA = 17;
                    shortB = 82;
                    shortC = 23;
                    shortD = 29;
                    shortE = 133;
                    shortF = 3;
                    shortG = 41;
                    shortH = 27;
                    shortI = 10;
                    shortJ = 177;
                    shortK = 6;
                    shortL = 37;
                    shortM = 14;
                    shortN = 145;
                    shortO = 4;
                    shortP = 9;
                    break; // 0 to 12 ms, 4 seat room
                case 6:
                    shortA = 31;
                    shortB = 19;
                    shortC = 3;
                    shortD = 29;
                    shortE = 196;
                    shortF = 11;
                    shortG = 10;
                    shortH = 65;
                    shortI = 21;
                    shortJ = 3;
                    shortK = 148;
                    shortL = 4;
                    shortM = 26;
                    shortN = 7;
                    shortO = 161;
                    shortP = 155;
                    break; // 0 to 12 ms, 4 seat room
                case 7:
                    shortA = 17;
                    shortB = 8;
                    shortC = 3;
                    shortD = 37;
                    shortE = 3;
                    shortF = 19;
                    shortG = 41;
                    shortH = 15;
                    shortI = 7;
                    shortJ = 197;
                    shortK = 178;
                    shortL = 22;
                    shortM = 26;
                    shortN = 97;
                    shortO = 16;
                    shortP = 156;
                    break; // 0 to 12 ms, 5 seat room
                case 8:
                    shortA = 17;
                    shortB = 3;
                    shortC = 8;
                    shortD = 29;
                    shortE = 39;
                    shortF = 156;
                    shortG = 7;
                    shortH = 43;
                    shortI = 101;
                    shortJ = 8;
                    shortK = 15;
                    shortL = 169;
                    shortM = 67;
                    shortN = 39;
                    shortO = 154;
                    shortP = 4;
                    break; // 0 to 13 ms, 5 seat room
                case 9:
                    shortA = 18;
                    shortB = 19;
                    shortC = 23;
                    shortD = 5;
                    shortE = 176;
                    shortF = 3;
                    shortG = 41;
                    shortH = 147;
                    shortI = 7;
                    shortJ = 148;
                    shortK = 5;
                    shortL = 15;
                    shortM = 10;
                    shortN = 30;
                    shortO = 119;
                    shortP = 19;
                    break; // 0 to 13 ms, 5 seat room
                case 10:
                    shortA = 173;
                    shortB = 19;
                    shortC = 23;
                    shortD = 27;
                    shortE = 8;
                    shortF = 37;
                    shortG = 7;
                    shortH = 202;
                    shortI = 8;
                    shortJ = 13;
                    shortK = 3;
                    shortL = 174;
                    shortM = 67;
                    shortN = 21;
                    shortO = 73;
                    shortP = 14;
                    break; // 0 to 14 ms, 6 seat room
                case 11:
                    shortA = 17;
                    shortB = 19;
                    shortC = 23;
                    shortD = 25;
                    shortE = 19;
                    shortF = 145;
                    shortG = 9;
                    shortH = 43;
                    shortI = 47;
                    shortJ = 203;
                    shortK = 18;
                    shortL = 180;
                    shortM = 226;
                    shortN = 3;
                    shortO = 73;
                    shortP = 12;
                    break; // 0 to 15 ms, 7 seat room
                case 12:
                    shortA = 17;
                    shortB = 19;
                    shortC = 23;
                    shortD = 3;
                    shortE = 3;
                    shortF = 20;
                    shortG = 203;
                    shortH = 99;
                    shortI = 207;
                    shortJ = 15;
                    shortK = 10;
                    shortL = 61;
                    shortM = 20;
                    shortN = 174;
                    shortO = 33;
                    shortP = 77;
                    break; // 0 to 15 ms, 7 seat room
                case 13:
                    shortA = 17;
                    shortB = 19;
                    shortC = 23;
                    shortD = 29;
                    shortE = 3;
                    shortF = 210;
                    shortG = 183;
                    shortH = 43;
                    shortI = 13;
                    shortJ = 12;
                    shortK = 26;
                    shortL = 220;
                    shortM = 67;
                    shortN = 235;
                    shortO = 11;
                    shortP = 23;
                    break; // 0 to 15 ms, 8 seat room
                case 14:
                    shortA = 17;
                    shortB = 3;
                    shortC = 21;
                    shortD = 191;
                    shortE = 31;
                    shortF = 10;
                    shortG = 41;
                    shortH = 218;
                    shortI = 15;
                    shortJ = 6;
                    shortK = 111;
                    shortL = 29;
                    shortM = 129;
                    shortN = 206;
                    shortO = 4;
                    shortP = 7;
                    break; // 0 to 16 ms, 8 seat room
                case 15:
                    shortA = 17;
                    shortB = 25;
                    shortC = 240;
                    shortD = 29;
                    shortE = 4;
                    shortF = 18;
                    shortG = 41;
                    shortH = 43;
                    shortI = 29;
                    shortJ = 28;
                    shortK = 250;
                    shortL = 12;
                    shortM = 261;
                    shortN = 9;
                    shortO = 5;
                    shortP = 79;
                    break; // 0 to 18 ms, 10 seat room
                case 16:
                default:
                    shortA = 5;
                    shortB = 3;
                    shortC = 23;
                    shortD = 29;
                    shortE = 3;
                    shortF = 257;
                    shortG = 199;
                    shortH = 252;
                    shortI = 132;
                    shortJ = 18;
                    shortK = 11;
                    shortL = 6;
                    shortM = 30;
                    shortN = 27;
                    shortO = 7;
                    shortP = 8;
                    break; // 0 to 19 ms, 11 seat room
            }
            prevclearcoat = clearcoat;
        }
        double reg4n = (1.0 - pow(1.0 - B, 2.0)) * 0.0336;
        double derez = 1.0;
        derez = fmin(fmax(derez / overallscale, 0.0001), 1.0);
        int bezFraction = (int)(1.0 / derez);
        double bezTrim = (double)bezFraction / (bezFraction + 1.0);
        derez = 1.0 / bezFraction;
        bezTrim = 1.0 - (derez * bezTrim);
        // the revision more accurately connects the bezier curves
        double wet = 1.0 - pow(1.0 - C, 2.0);
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
            bez[bez_cycle] += derez;
            bez[bez_SampL] += (inputSampleR * derez);
            bez[bez_SampR] += (inputSampleL * derez); // stereo got reversed somewhere?
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bez[bez_cycle] = 0.0;
                // left verbs
                double dualmonoSampleL = bez[bez_SampL];
                b4AL[c4AL] = dualmonoSampleL + (g4AL * reg4n);
                b4BL[c4BL] = dualmonoSampleL + (g4BL * reg4n);
                b4CL[c4CL] = dualmonoSampleL + (g4CL * reg4n);
                b4DL[c4DL] = dualmonoSampleL + (g4DL * reg4n);
                c4AL++;
                if (c4AL < 0 || c4AL > shortA) {
                    c4AL = 0;
                }
                c4BL++;
                if (c4BL < 0 || c4BL > shortB) {
                    c4BL = 0;
                }
                c4CL++;
                if (c4CL < 0 || c4CL > shortC) {
                    c4CL = 0;
                }
                c4DL++;
                if (c4DL < 0 || c4DL > shortD) {
                    c4DL = 0;
                }
                double hA = b4AL[c4AL - ((c4AL > shortA) ? shortA + 1 : 0)];
                double hB = b4BL[c4BL - ((c4BL > shortB) ? shortB + 1 : 0)];
                double hC = b4CL[c4CL - ((c4CL > shortC) ? shortC + 1 : 0)];
                double hD = b4DL[c4DL - ((c4DL > shortD) ? shortD + 1 : 0)];
                b4EL[c4EL] = hA - (hB + hC + hD);
                b4FL[c4FL] = hB - (hA + hC + hD);
                b4GL[c4GL] = hC - (hA + hB + hD);
                b4HL[c4HL] = hD - (hA + hB + hC);
                c4EL++;
                if (c4EL < 0 || c4EL > shortE) {
                    c4EL = 0;
                }
                c4FL++;
                if (c4FL < 0 || c4FL > shortF) {
                    c4FL = 0;
                }
                c4GL++;
                if (c4GL < 0 || c4GL > shortG) {
                    c4GL = 0;
                }
                c4HL++;
                if (c4HL < 0 || c4HL > shortH) {
                    c4HL = 0;
                }
                hA = b4EL[c4EL - ((c4EL > shortE) ? shortE + 1 : 0)];
                hB = b4FL[c4FL - ((c4FL > shortF) ? shortF + 1 : 0)];
                hC = b4GL[c4GL - ((c4GL > shortG) ? shortG + 1 : 0)];
                hD = b4HL[c4HL - ((c4HL > shortH) ? shortH + 1 : 0)];
                b4IL[c4IL] = hA - (hB + hC + hD);
                b4JL[c4JL] = hB - (hA + hC + hD);
                b4KL[c4KL] = hC - (hA + hB + hD);
                b4LL[c4LL] = hD - (hA + hB + hC);
                c4IL++;
                if (c4IL < 0 || c4IL > shortI) {
                    c4IL = 0;
                }
                c4JL++;
                if (c4JL < 0 || c4JL > shortJ) {
                    c4JL = 0;
                }
                c4KL++;
                if (c4KL < 0 || c4KL > shortK) {
                    c4KL = 0;
                }
                c4LL++;
                if (c4LL < 0 || c4LL > shortL) {
                    c4LL = 0;
                }
                hA = b4IL[c4IL - ((c4IL > shortI) ? shortI + 1 : 0)];
                hB = b4JL[c4JL - ((c4JL > shortJ) ? shortJ + 1 : 0)];
                hC = b4KL[c4KL - ((c4KL > shortK) ? shortK + 1 : 0)];
                hD = b4LL[c4LL - ((c4LL > shortL) ? shortL + 1 : 0)];
                b4ML[c4ML] = hA - (hB + hC + hD);
                b4NL[c4NL] = hB - (hA + hC + hD);
                b4OL[c4OL] = hC - (hA + hB + hD);
                b4PL[c4PL] = hD - (hA + hB + hC);
                c4ML++;
                if (c4ML < 0 || c4ML > shortM) {
                    c4ML = 0;
                }
                c4NL++;
                if (c4NL < 0 || c4NL > shortN) {
                    c4NL = 0;
                }
                c4OL++;
                if (c4OL < 0 || c4OL > shortO) {
                    c4OL = 0;
                }
                c4PL++;
                if (c4PL < 0 || c4PL > shortP) {
                    c4PL = 0;
                }
                hA = b4ML[c4ML - ((c4ML > shortM) ? shortM + 1 : 0)];
                hB = b4NL[c4NL - ((c4NL > shortN) ? shortN + 1 : 0)];
                hC = b4OL[c4OL - ((c4OL > shortO) ? shortO + 1 : 0)];
                hD = b4PL[c4PL - ((c4PL > shortP) ? shortP + 1 : 0)];
                g4AL = hA - (hB + hC + hD);
                g4BL = hB - (hA + hC + hD);
                g4CL = hC - (hA + hB + hD);
                g4DL = hD - (hA + hB + hC);
                dualmonoSampleL = (hA + hB + hC + hD) * 0.125;
                // right verbs
                double dualmonoSampleR = bez[bez_SampR];
                b4DR[c4DR] = dualmonoSampleR + (g4DR * reg4n);
                b4HR[c4HR] = dualmonoSampleR + (g4HR * reg4n);
                b4LR[c4LR] = dualmonoSampleR + (g4LR * reg4n);
                b4PR[c4PR] = dualmonoSampleR + (g4PR * reg4n);
                c4DR++;
                if (c4DR < 0 || c4DR > shortD) {
                    c4DR = 0;
                }
                c4HR++;
                if (c4HR < 0 || c4HR > shortH) {
                    c4HR = 0;
                }
                c4LR++;
                if (c4LR < 0 || c4LR > shortL) {
                    c4LR = 0;
                }
                c4PR++;
                if (c4PR < 0 || c4PR > shortP) {
                    c4PR = 0;
                }
                hA = b4DR[c4DR - ((c4DR > shortD) ? shortD + 1 : 0)];
                hB = b4HR[c4HR - ((c4HR > shortH) ? shortH + 1 : 0)];
                hC = b4LR[c4LR - ((c4LR > shortL) ? shortL + 1 : 0)];
                hD = b4PR[c4PR - ((c4PR > shortP) ? shortP + 1 : 0)];
                b4CR[c4CR] = hA - (hB + hC + hD);
                b4GR[c4GR] = hB - (hA + hC + hD);
                b4KR[c4KR] = hC - (hA + hB + hD);
                b4OR[c4OR] = hD - (hA + hB + hC);
                c4CR++;
                if (c4CR < 0 || c4CR > shortC) {
                    c4CR = 0;
                }
                c4GR++;
                if (c4GR < 0 || c4GR > shortG) {
                    c4GR = 0;
                }
                c4KR++;
                if (c4KR < 0 || c4KR > shortK) {
                    c4KR = 0;
                }
                c4OR++;
                if (c4OR < 0 || c4OR > shortO) {
                    c4OR = 0;
                }
                hA = b4CR[c4CR - ((c4CR > shortC) ? shortC + 1 : 0)];
                hB = b4GR[c4GR - ((c4GR > shortG) ? shortG + 1 : 0)];
                hC = b4KR[c4KR - ((c4KR > shortK) ? shortK + 1 : 0)];
                hD = b4OR[c4OR - ((c4OR > shortO) ? shortO + 1 : 0)];
                b4BR[c4BR] = hA - (hB + hC + hD);
                b4FR[c4FR] = hB - (hA + hC + hD);
                b4JR[c4JR] = hC - (hA + hB + hD);
                b4NR[c4NR] = hD - (hA + hB + hC);
                c4BR++;
                if (c4BR < 0 || c4BR > shortB) {
                    c4BR = 0;
                }
                c4FR++;
                if (c4FR < 0 || c4FR > shortF) {
                    c4FR = 0;
                }
                c4JR++;
                if (c4JR < 0 || c4JR > shortJ) {
                    c4JR = 0;
                }
                c4NR++;
                if (c4NR < 0 || c4NR > shortN) {
                    c4NR = 0;
                }
                hA = b4BR[c4BR - ((c4BR > shortB) ? shortB + 1 : 0)];
                hB = b4FR[c4FR - ((c4FR > shortF) ? shortF + 1 : 0)];
                hC = b4JR[c4JR - ((c4JR > shortJ) ? shortJ + 1 : 0)];
                hD = b4NR[c4NR - ((c4NR > shortN) ? shortN + 1 : 0)];
                b4AR[c4AR] = hA - (hB + hC + hD);
                b4ER[c4ER] = hB - (hA + hC + hD);
                b4IR[c4IR] = hC - (hA + hB + hD);
                b4MR[c4MR] = hD - (hA + hB + hC);
                c4AR++;
                if (c4AR < 0 || c4AR > shortA) {
                    c4AR = 0;
                }
                c4ER++;
                if (c4ER < 0 || c4ER > shortE) {
                    c4ER = 0;
                }
                c4IR++;
                if (c4IR < 0 || c4IR > shortI) {
                    c4IR = 0;
                }
                c4MR++;
                if (c4MR < 0 || c4MR > shortM) {
                    c4MR = 0;
                }
                hA = b4AR[c4AR - ((c4AR > shortA) ? shortA + 1 : 0)];
                hB = b4ER[c4ER - ((c4ER > shortE) ? shortE + 1 : 0)];
                hC = b4IR[c4IR - ((c4IR > shortI) ? shortI + 1 : 0)];
                hD = b4MR[c4MR - ((c4MR > shortM) ? shortM + 1 : 0)];
                g4DR = hA - (hB + hC + hD);
                g4HR = hB - (hA + hC + hD);
                g4LR = hC - (hA + hB + hD);
                g4PR = hD - (hA + hB + hC);
                dualmonoSampleR = (hA + hB + hC + hD) * 0.125;
                bez[bez_CL] = bez[bez_BL];
                bez[bez_BL] = bez[bez_AL];
                bez[bez_AL] = dualmonoSampleR;
                bez[bez_SampL] = 0.0;
                bez[bez_CR] = bez[bez_BR];
                bez[bez_BR] = bez[bez_AR];
                bez[bez_AR] = dualmonoSampleL;
                bez[bez_SampR] = 0.0;
            }
            double X = bez[bez_cycle] * bezTrim;
            double CBL = (bez[bez_CL] * (1.0 - X)) + (bez[bez_BL] * X);
            double CBR = (bez[bez_CR] * (1.0 - X)) + (bez[bez_BR] * X);
            double BAL = (bez[bez_BL] * (1.0 - X)) + (bez[bez_AL] * X);
            double BAR = (bez[bez_BR] * (1.0 - X)) + (bez[bez_AR] * X);
            inputSampleL = (bez[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * 0.125;
            inputSampleR = (bez[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * 0.125;
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::woodenbox
