#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::kwoodroom {

constexpr std::string_view k_name{ "kWoodRoom" };
constexpr std::string_view k_short_description{
    "kWoodRoom is a small wooden performing space."
};
constexpr std::string_view k_long_description{
    "This concludes a series of plugin releases around Xmas and thereabouts! First there was ChimeyDeluxe, then ConsoleX2 with a bit of music to go along with it, and now the final shoe to drop is the unreleased reverb that was used to make what was seemingly a Dead show from 1972 or so!kWoodRoom was more stumbled upon than searched for. I'm continuing to pursue reverb things, doing better and better at delivering a really convincing virtual place to be in, but kWoodRoom popped up when searching for something else, and it's the last example of where my reverb work stood in 2025.It's got all the same controls as recent Airwindows reverbs, and it's the tone that is unusual. From the first instant I heard it I went 'oh hey a wood paneled room, that's weird'. It didn't seem special in any other way, didn't max out any of the things I was aiming for, but there it was, sounding like a thing. I've learned to pay attention to that, especially when there's no real explanation for it.That's because the reverbs I make search through hundreds of millions of possible delay time options to find optimal settings that balance many different requirements. They're purely Householder matrixes using comb filters alone, and relying upon matrix size to produce a smooth sound… unless I'm guiding them towards a rough sound, or a sound like a series of concrete arches, or what have you. I get to seek out all kinds of different qualities in the countless hours of computer time spent running genetic algorithms and permutations of delay lines.And then in the middle of all that, there'll be a kWoodRoom, more recognized than created. It just had a personality. Will it be a personality you like? I don't know, but if you give it a shot you will probably hear what it's like right away, and then it's up to you :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class kWoodRoom final : public Effect<T>
{
    static constexpr int d3A = 581;
    static constexpr int d3B = 831;
    static constexpr int d3C = 832;
    static constexpr int d3D = 574;
    static constexpr int d3E = 598;
    static constexpr int d3F = 685;
    static constexpr int d3G = 499;
    static constexpr int d3H = 573;
    static constexpr int d3I = 655;
    static constexpr int d6A = 154;
    static constexpr int d6B = 832;
    static constexpr int d6C = 109;
    static constexpr int d6D = 685;
    static constexpr int d6E = 33;
    static constexpr int d6F = 12;
    static constexpr int d6G = 27;
    static constexpr int d6H = 30;
    static constexpr int d6I = 339;
    static constexpr int d6J = 499;
    static constexpr int d6K = 296;
    static constexpr int d6L = 169;
    static constexpr int d6M = 169;
    static constexpr int d6N = 831;
    static constexpr int d6O = 15;
    static constexpr int d6P = 411;
    static constexpr int d6Q = 238;
    static constexpr int d6R = 68;
    static constexpr int d6S = 0;
    static constexpr int d6T = 8;
    static constexpr int d6U = 655;
    static constexpr int d6V = 581;
    static constexpr int d6W = 465;
    static constexpr int d6X = 173;
    static constexpr int d6Y = 3;
    static constexpr int d6ZA = 96;
    static constexpr int d6ZB = 573;
    static constexpr int d6ZC = 243;
    static constexpr int d6ZD = 30;
    static constexpr int d6ZE = 188;
    static constexpr int d6ZF = 291;
    static constexpr int d6ZG = 11;
    static constexpr int d6ZH = 372;
    static constexpr int d6ZI = 574;
    static constexpr int d6ZJ = 100;
    static constexpr int d6ZK = 598;
    static constexpr int early[] = { 0, 3, 8, 11, 12, 15, 27, 30, 30, 33, 68, 96, 100, 109, 154, 169, 169, 173, 188, 238, 243, 291, 296, 339, 372, 411, 465, 499, 573, 574, 581, 598, 655, 685, 831, 832 };
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'kwdr';
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    double a3AL[d3A + 5];
    double a3BL[d3B + 5];
    double a3CL[d3C + 5];
    double a3DL[d3D + 5];
    double a3EL[d3E + 5];
    double a3FL[d3F + 5];
    double a3GL[d3G + 5];
    double a3HL[d3H + 5];
    double a3IL[d3I + 5];
    double a3AR[d3A + 5];
    double a3BR[d3B + 5];
    double a3CR[d3C + 5];
    double a3DR[d3D + 5];
    double a3ER[d3E + 5];
    double a3FR[d3F + 5];
    double a3GR[d3G + 5];
    double a3HR[d3H + 5];
    double a3IR[d3I + 5];
    int c3AL, c3AR, c3BL, c3BR, c3CL, c3CR, c3DL, c3DR, c3EL, c3ER;
    int c3FL, c3FR, c3GL, c3GR, c3HL, c3HR, c3IL, c3IR;
    double a6AL[d6A + 5];
    double a6BL[d6B + 5];
    double a6CL[d6C + 5];
    double a6DL[d6D + 5];
    double a6EL[d6E + 5];
    double a6FL[d6F + 5];
    double a6GL[d6G + 5];
    double a6HL[d6H + 5];
    double a6IL[d6I + 5];
    double a6JL[d6J + 5];
    double a6KL[d6K + 5];
    double a6LL[d6L + 5];
    double a6ML[d6M + 5];
    double a6NL[d6N + 5];
    double a6OL[d6O + 5];
    double a6PL[d6P + 5];
    double a6QL[d6Q + 5];
    double a6RL[d6R + 5];
    double a6SL[d6S + 5];
    double a6TL[d6T + 5];
    double a6UL[d6U + 5];
    double a6VL[d6V + 5];
    double a6WL[d6W + 5];
    double a6XL[d6X + 5];
    double a6YL[d6Y + 5];
    double a6ZAL[d6ZA + 5];
    double a6ZBL[d6ZB + 5];
    double a6ZCL[d6ZC + 5];
    double a6ZDL[d6ZD + 5];
    double a6ZEL[d6ZE + 5];
    double a6ZFL[d6ZF + 5];
    double a6ZGL[d6ZG + 5];
    double a6ZHL[d6ZH + 5];
    double a6ZIL[d6ZI + 5];
    double a6ZJL[d6ZJ + 5];
    double a6ZKL[d6ZK + 5];
    double a6AR[d6A + 5];
    double a6BR[d6B + 5];
    double a6CR[d6C + 5];
    double a6DR[d6D + 5];
    double a6ER[d6E + 5];
    double a6FR[d6F + 5];
    double a6GR[d6G + 5];
    double a6HR[d6H + 5];
    double a6IR[d6I + 5];
    double a6JR[d6J + 5];
    double a6KR[d6K + 5];
    double a6LR[d6L + 5];
    double a6MR[d6M + 5];
    double a6NR[d6N + 5];
    double a6OR[d6O + 5];
    double a6PR[d6P + 5];
    double a6QR[d6Q + 5];
    double a6RR[d6R + 5];
    double a6SR[d6S + 5];
    double a6TR[d6T + 5];
    double a6UR[d6U + 5];
    double a6VR[d6V + 5];
    double a6WR[d6W + 5];
    double a6XR[d6X + 5];
    double a6YR[d6Y + 5];
    double a6ZAR[d6ZA + 5];
    double a6ZBR[d6ZB + 5];
    double a6ZCR[d6ZC + 5];
    double a6ZDR[d6ZD + 5];
    double a6ZER[d6ZE + 5];
    double a6ZFR[d6ZF + 5];
    double a6ZGR[d6ZG + 5];
    double a6ZHR[d6ZH + 5];
    double a6ZIR[d6ZI + 5];
    double a6ZJR[d6ZJ + 5];
    double a6ZKR[d6ZK + 5];
    int c6AL, c6BL, c6CL, c6DL, c6EL, c6FL, c6GL, c6HL, c6IL;
    int c6JL, c6KL, c6LL, c6ML, c6NL, c6OL, c6PL, c6QL, c6RL;
    int c6SL, c6TL, c6UL, c6VL, c6WL, c6XL, c6YL, c6ZAL, c6ZBL;
    int c6ZCL, c6ZDL, c6ZEL, c6ZFL, c6ZGL, c6ZHL, c6ZIL, c6ZJL, c6ZKL;
    int c6AR, c6BR, c6CR, c6DR, c6ER, c6FR, c6GR, c6HR, c6IR;
    int c6JR, c6KR, c6LR, c6MR, c6NR, c6OR, c6PR, c6QR, c6RR;
    int c6SR, c6TR, c6UR, c6VR, c6WR, c6XR, c6YR, c6ZAR, c6ZBR;
    int c6ZCR, c6ZDR, c6ZER, c6ZFR, c6ZGR, c6ZHR, c6ZIR, c6ZJR, c6ZKR;
    double f6AL, f6BL, f6CL, f6DL, f6EL, f6FL;
    double f6FR, f6LR, f6RR, f6XR, f6ZER, f6ZKR;
    double avg6L, avg6R;
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
        bez_IIRL,
        bez_IIRR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];
    double bezF[bez_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    kWoodRoom()
    {
        A = 0.5;
        B = 0.5;
        C = 0.25;
        D = 0.5;
        E = 0.75;
        F = 0.5;
        for (int x = 0; x < d3A + 2; x++) {
            a3AL[x] = 0.0;
            a3AR[x] = 0.0;
        }
        for (int x = 0; x < d3B + 2; x++) {
            a3BL[x] = 0.0;
            a3BR[x] = 0.0;
        }
        for (int x = 0; x < d3C + 2; x++) {
            a3CL[x] = 0.0;
            a3CR[x] = 0.0;
        }
        for (int x = 0; x < d3D + 2; x++) {
            a3DL[x] = 0.0;
            a3DR[x] = 0.0;
        }
        for (int x = 0; x < d3E + 2; x++) {
            a3EL[x] = 0.0;
            a3ER[x] = 0.0;
        }
        for (int x = 0; x < d3F + 2; x++) {
            a3FL[x] = 0.0;
            a3FR[x] = 0.0;
        }
        for (int x = 0; x < d3G + 2; x++) {
            a3GL[x] = 0.0;
            a3GR[x] = 0.0;
        }
        for (int x = 0; x < d3H + 2; x++) {
            a3HL[x] = 0.0;
            a3HR[x] = 0.0;
        }
        for (int x = 0; x < d3I + 2; x++) {
            a3IL[x] = 0.0;
            a3IR[x] = 0.0;
        }
        c3AL = c3BL = c3CL = c3DL = c3EL = c3FL = c3GL = c3HL = c3IL = 1;
        c3AR = c3BR = c3CR = c3DR = c3ER = c3FR = c3GR = c3HR = c3IR = 1;
        for (int x = 0; x < d6A + 2; x++) {
            a6AL[x] = 0.0;
            a6AR[x] = 0.0;
        }
        for (int x = 0; x < d6B + 2; x++) {
            a6BL[x] = 0.0;
            a6BR[x] = 0.0;
        }
        for (int x = 0; x < d6C + 2; x++) {
            a6CL[x] = 0.0;
            a6CR[x] = 0.0;
        }
        for (int x = 0; x < d6D + 2; x++) {
            a6DL[x] = 0.0;
            a6DR[x] = 0.0;
        }
        for (int x = 0; x < d6E + 2; x++) {
            a6EL[x] = 0.0;
            a6ER[x] = 0.0;
        }
        for (int x = 0; x < d6F + 2; x++) {
            a6FL[x] = 0.0;
            a6FR[x] = 0.0;
        }
        for (int x = 0; x < d6G + 2; x++) {
            a6GL[x] = 0.0;
            a6GR[x] = 0.0;
        }
        for (int x = 0; x < d6H + 2; x++) {
            a6HL[x] = 0.0;
            a6HR[x] = 0.0;
        }
        for (int x = 0; x < d6I + 2; x++) {
            a6IL[x] = 0.0;
            a6IR[x] = 0.0;
        }
        for (int x = 0; x < d6J + 2; x++) {
            a6JL[x] = 0.0;
            a6JR[x] = 0.0;
        }
        for (int x = 0; x < d6K + 2; x++) {
            a6KL[x] = 0.0;
            a6KR[x] = 0.0;
        }
        for (int x = 0; x < d6L + 2; x++) {
            a6LL[x] = 0.0;
            a6LR[x] = 0.0;
        }
        for (int x = 0; x < d6M + 2; x++) {
            a6ML[x] = 0.0;
            a6MR[x] = 0.0;
        }
        for (int x = 0; x < d6N + 2; x++) {
            a6NL[x] = 0.0;
            a6NR[x] = 0.0;
        }
        for (int x = 0; x < d6O + 2; x++) {
            a6OL[x] = 0.0;
            a6OR[x] = 0.0;
        }
        for (int x = 0; x < d6P + 2; x++) {
            a6PL[x] = 0.0;
            a6PR[x] = 0.0;
        }
        for (int x = 0; x < d6Q + 2; x++) {
            a6QL[x] = 0.0;
            a6QR[x] = 0.0;
        }
        for (int x = 0; x < d6R + 2; x++) {
            a6RL[x] = 0.0;
            a6RR[x] = 0.0;
        }
        for (int x = 0; x < d6S + 2; x++) {
            a6SL[x] = 0.0;
            a6SR[x] = 0.0;
        }
        for (int x = 0; x < d6T + 2; x++) {
            a6TL[x] = 0.0;
            a6TR[x] = 0.0;
        }
        for (int x = 0; x < d6U + 2; x++) {
            a6UL[x] = 0.0;
            a6UR[x] = 0.0;
        }
        for (int x = 0; x < d6V + 2; x++) {
            a6VL[x] = 0.0;
            a6VR[x] = 0.0;
        }
        for (int x = 0; x < d6W + 2; x++) {
            a6WL[x] = 0.0;
            a6WR[x] = 0.0;
        }
        for (int x = 0; x < d6X + 2; x++) {
            a6XL[x] = 0.0;
            a6XR[x] = 0.0;
        }
        for (int x = 0; x < d6Y + 2; x++) {
            a6YL[x] = 0.0;
            a6YR[x] = 0.0;
        }
        for (int x = 0; x < d6ZA + 2; x++) {
            a6ZAL[x] = 0.0;
            a6ZAR[x] = 0.0;
        }
        for (int x = 0; x < d6ZB + 2; x++) {
            a6ZBL[x] = 0.0;
            a6ZBR[x] = 0.0;
        }
        for (int x = 0; x < d6ZC + 2; x++) {
            a6ZCL[x] = 0.0;
            a6ZCR[x] = 0.0;
        }
        for (int x = 0; x < d6ZD + 2; x++) {
            a6ZDL[x] = 0.0;
            a6ZDR[x] = 0.0;
        }
        for (int x = 0; x < d6ZE + 2; x++) {
            a6ZEL[x] = 0.0;
            a6ZER[x] = 0.0;
        }
        for (int x = 0; x < d6ZF + 2; x++) {
            a6ZFL[x] = 0.0;
            a6ZFR[x] = 0.0;
        }
        for (int x = 0; x < d6ZG + 2; x++) {
            a6ZGL[x] = 0.0;
            a6ZGR[x] = 0.0;
        }
        for (int x = 0; x < d6ZH + 2; x++) {
            a6ZHL[x] = 0.0;
            a6ZHR[x] = 0.0;
        }
        for (int x = 0; x < d6ZI + 2; x++) {
            a6ZIL[x] = 0.0;
            a6ZIR[x] = 0.0;
        }
        for (int x = 0; x < d6ZJ + 2; x++) {
            a6ZJL[x] = 0.0;
            a6ZJR[x] = 0.0;
        }
        for (int x = 0; x < d6ZK + 2; x++) {
            a6ZKL[x] = 0.0;
            a6ZKR[x] = 0.0;
        }
        c6AL = c6BL = c6CL = c6DL = c6EL = c6FL = c6GL = c6HL = c6IL = 1;
        c6JL = c6KL = c6LL = c6ML = c6NL = c6OL = c6PL = c6QL = c6RL = 1;
        c6SL = c6TL = c6UL = c6VL = c6WL = c6XL = c6YL = c6ZAL = c6ZBL = 1;
        c6ZCL = c6ZDL = c6ZEL = c6ZFL = c6ZGL = c6ZHL = c6ZIL = c6ZJL = c6ZKL = 1;
        c6AR = c6BR = c6CR = c6DR = c6ER = c6FR = c6GR = c6HR = c6IR = 1;
        c6JR = c6KR = c6LR = c6MR = c6NR = c6OR = c6PR = c6QR = c6RR = 1;
        c6SR = c6TR = c6UR = c6VR = c6WR = c6XR = c6YR = c6ZAR = c6ZBR = 1;
        c6ZCR = c6ZDR = c6ZER = c6ZFR = c6ZGR = c6ZHR = c6ZIR = c6ZJR = c6ZKR = 1;
        f6AL = f6BL = f6CL = f6DL = f6EL = f6FL = 0.0;
        f6FR = f6LR = f6RR = f6XR = f6ZER = f6ZKR = 0.0;
        avg6L = avg6R = 0.0;
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
            bezF[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
        bezF[bez_cycle] = 1.0;
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
        kNumParameters = 6
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.25; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.75; break;
            case kParamF: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "regen"; break;
            case kParamB: return "derez"; break;
            case kParamC: return "filter"; break;
            case kParamD: return "earlyrf"; break;
            case kParamE: return "positin"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Regen"; break;
            case kParamB: return "Derez"; break;
            case kParamC: return "Filter"; break;
            case kParamD: return "EarlyRF"; break;
            case kParamE: return "Positin"; break;
            case kParamF: return "Dry/Wet"; break;

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
        double fdb6ck = (0.0009765625 + 0.0009765625 + 0.001953125) * 0.3333333;
        double reg6n = (1.0 - pow(1.0 - A, 1.618033988749894)) * fdb6ck;
        // start this but pad it in the loop by volume of output?
        double derez = B * 2.0;
        bool stepped = true; // Revised Bezier Undersampling
        if (derez > 1.0) { // has full rez at center, stepped
            stepped = false; // to left, continuous to right
            derez = 1.0 - (derez - 1.0);
        } // if it's set up like that it's the revised algorithm
        derez = fmin(fmax(derez / overallscale, 0.0005), 1.0);
        int bezFraction = (int)(1.0 / derez);
        double bezTrim = (double)bezFraction / (bezFraction + 1.0);
        if (stepped) { // this hard-locks derez to exact subdivisions of 1.0
            derez = 1.0 / bezFraction;
            bezTrim = 1.0 - (derez * bezTrim);
        }
        else { // this makes it match the 1.0 case using stepped
            derez /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
            bezTrim = 1.0 - pow(derez * 0.5, 1.0 / (derez * 0.5));
        } // the revision more accurately connects the bezier curves
        double derezFreq = C * 2.0;
        bool steppedFreq = true; // Revised Bezier Undersampling
        if (derezFreq > 1.0) { // has full rez at center, stepped
            steppedFreq = false; // to left, continuous to right
            derezFreq = 1.0 - (derezFreq - 1.0);
        } // if it's set up like that it's the revised algorithm
        derezFreq = fmin(fmax(derezFreq, 0.0005), 1.0); // note: no overallscale, already inside undersampling
        int bezFreqFraction = (int)(1.0 / derezFreq);
        double bezFreqTrim = (double)bezFreqFraction / (bezFreqFraction + 1.0);
        if (steppedFreq) { // this hard-locks derez to exact subdivisions of 1.0
            derezFreq = 1.0 / bezFreqFraction;
            bezFreqTrim = 1.0 - (derezFreq * bezFreqTrim);
        }
        else { // this makes it match the 1.0 case using stepped
            bezFreqTrim = 1.0 - pow(derezFreq * 0.5, 1.0 / (derezFreq * 0.5));
        } // the revision more accurately connects the bezier curves
        double earlyLoudness = pow(D, 2.0);
        int start = (int)(E * 27.0);
        int ld3G = early[start];
        int ld3H = early[start + 1];
        int ld3D = early[start + 2];
        int ld3A = early[start + 3];
        int ld3E = early[start + 4];
        int ld3I = early[start + 5];
        int ld3F = early[start + 6];
        int ld3B = early[start + 7];
        int ld3C = early[start + 8];
        double wet = F;
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
            bez[bez_SampL] += (inputSampleL * derez);
            bez[bez_SampR] += (inputSampleR * derez);
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                if (stepped) {
                    bez[bez_cycle] = 0.0;
                }
                else {
                    bez[bez_cycle] -= 1.0;
                }
                inputSampleL = (bez[bez_SampL]);
                inputSampleR = (bez[bez_SampR]);
                a3AL[c3AL] = inputSampleL; // + (f3AL * reg3n);
                a3BL[c3BL] = inputSampleL; // + (f3BL * reg3n);
                a3CL[c3CL] = inputSampleL; // + (f3CL * reg3n);
                a3CR[c3CR] = inputSampleR; // + (f3CR * reg3n);
                a3FR[c3FR] = inputSampleR; // + (f3FR * reg3n);
                a3IR[c3IR] = inputSampleR; // + (f3IR * reg3n);
                c3AL++;
                if (c3AL > ld3A) {
                    c3AL = 0;
                }
                c3BL++;
                if (c3BL > ld3B) {
                    c3BL = 0;
                }
                c3CL++;
                if (c3CL > ld3C) {
                    c3CL = 0;
                }
                c3CR++;
                if (c3CR > ld3C) {
                    c3CR = 0;
                }
                c3FR++;
                if (c3FR > ld3F) {
                    c3FR = 0;
                }
                c3IR++;
                if (c3IR > ld3I) {
                    c3IR = 0;
                }
                double hA = a3AL[c3AL - ((c3AL > ld3A) ? c3AL + 1 : 0)];
                double hB = a3BL[c3BL - ((c3BL > ld3B) ? c3BL + 1 : 0)];
                double hC = a3CL[c3CL - ((c3CL > ld3C) ? c3CL + 1 : 0)];
                double hD = a3CR[c3CR - ((c3CR > ld3C) ? c3CR + 1 : 0)];
                double hE = a3FR[c3FR - ((c3FR > ld3F) ? c3FR + 1 : 0)];
                double hF = a3IR[c3IR - ((c3IR > ld3I) ? c3IR + 1 : 0)];
                a3DL[c3DL] = (((hB + hC) * -2.0) + hA);
                a3EL[c3EL] = (((hA + hC) * -2.0) + hB);
                a3FL[c3FL] = (((hA + hB) * -2.0) + hC);
                a3BR[c3BR] = (((hE + hF) * -2.0) + hD);
                a3ER[c3ER] = (((hD + hF) * -2.0) + hE);
                a3HR[c3HR] = (((hD + hE) * -2.0) + hF);
                c3DL++;
                if (c3DL > ld3D) {
                    c3DL = 0;
                }
                c3EL++;
                if (c3EL > ld3E) {
                    c3EL = 0;
                }
                c3FL++;
                if (c3FL > ld3F) {
                    c3FL = 0;
                }
                c3BR++;
                if (c3BR > ld3B) {
                    c3BR = 0;
                }
                c3ER++;
                if (c3ER > ld3E) {
                    c3ER = 0;
                }
                c3HR++;
                if (c3HR > ld3H) {
                    c3HR = 0;
                }
                hA = a3DL[c3DL - ((c3DL > ld3D) ? c3DL + 1 : 0)];
                hB = a3EL[c3EL - ((c3EL > ld3E) ? c3EL + 1 : 0)];
                hC = a3FL[c3FL - ((c3FL > ld3F) ? c3FL + 1 : 0)];
                hD = a3BR[c3BR - ((c3BR > ld3B) ? c3BR + 1 : 0)];
                hE = a3ER[c3ER - ((c3ER > ld3E) ? c3ER + 1 : 0)];
                hF = a3HR[c3HR - ((c3HR > ld3H) ? c3HR + 1 : 0)];
                a3GL[c3GL] = (((hB + hC) * -2.0) + hA);
                a3HL[c3HL] = (((hA + hC) * -2.0) + hB);
                a3IL[c3IL] = (((hA + hB) * -2.0) + hC);
                a3AR[c3AR] = (((hE + hF) * -2.0) + hD);
                a3DR[c3DR] = (((hD + hF) * -2.0) + hE);
                a3GR[c3GR] = (((hD + hE) * -2.0) + hF);
                c3GL++;
                if (c3GL > ld3G) {
                    c3GL = 0;
                }
                c3HL++;
                if (c3HL > ld3H) {
                    c3HL = 0;
                }
                c3IL++;
                if (c3IL > ld3I) {
                    c3IL = 0;
                }
                c3AR++;
                if (c3AR > ld3A) {
                    c3AR = 0;
                }
                c3DR++;
                if (c3DR > ld3D) {
                    c3DR = 0;
                }
                c3GR++;
                if (c3GR > ld3G) {
                    c3GR = 0;
                }
                hA = a3GL[c3GL - ((c3GL > ld3G) ? c3GL + 1 : 0)];
                hB = a3HL[c3HL - ((c3HL > ld3H) ? c3HL + 1 : 0)];
                hC = a3IL[c3IL - ((c3IL > ld3I) ? c3IL + 1 : 0)];
                hD = a3AR[c3AR - ((c3AR > ld3A) ? c3AR + 1 : 0)];
                hE = a3DR[c3DR - ((c3DR > ld3D) ? c3DR + 1 : 0)];
                hF = a3GR[c3GR - ((c3GR > ld3G) ? c3GR + 1 : 0)];
                double earlyReflectionL = (((hB + hC) * -2.0) + hA) * -0.0625;
                double earlyReflectionR = (((hE + hF) * -2.0) + hD) * -0.0625;
                inputSampleL -= earlyReflectionL;
                inputSampleR -= earlyReflectionR;
                a6AL[c6AL] = inputSampleL + (f6BL * reg6n);
                a6BL[c6BL] = inputSampleL + (f6CL * reg6n);
                a6CL[c6CL] = inputSampleL + (f6DL * reg6n);
                a6DL[c6DL] = inputSampleL + (f6EL * reg6n);
                a6EL[c6EL] = inputSampleL + (f6FL * reg6n);
                a6FL[c6FL] = inputSampleL + (f6AL * reg6n);
                a6FR[c6FR] = inputSampleR + (f6LR * reg6n);
                a6LR[c6LR] = inputSampleR + (f6RR * reg6n);
                a6RR[c6RR] = inputSampleR + (f6XR * reg6n);
                a6XR[c6XR] = inputSampleR + (f6ZER * reg6n);
                a6ZER[c6ZER] = inputSampleR + (f6ZKR * reg6n);
                a6ZKR[c6ZKR] = inputSampleR + (f6FR * reg6n);
                // left verb
                c6AL++;
                if (c6AL > d6A) {
                    c6AL = 0;
                }
                c6BL++;
                if (c6BL > d6B) {
                    c6BL = 0;
                }
                c6CL++;
                if (c6CL > d6C) {
                    c6CL = 0;
                }
                c6DL++;
                if (c6DL > d6D) {
                    c6DL = 0;
                }
                c6EL++;
                if (c6EL > d6E) {
                    c6EL = 0;
                }
                c6FL++;
                if (c6FL > d6F) {
                    c6FL = 0;
                }
                hA = a6AL[c6AL - ((c6AL > d6A) ? d6A + 1 : 0)];
                hB = a6BL[c6BL - ((c6BL > d6B) ? d6B + 1 : 0)];
                hC = a6CL[c6CL - ((c6CL > d6C) ? d6C + 1 : 0)];
                hD = a6DL[c6DL - ((c6DL > d6D) ? d6D + 1 : 0)];
                hE = a6EL[c6EL - ((c6EL > d6E) ? d6E + 1 : 0)];
                hF = a6FL[c6FL - ((c6FL > d6F) ? d6F + 1 : 0)];
                a6GL[c6GL] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6HL[c6HL] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6IL[c6IL] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6JL[c6JL] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6KL[c6KL] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6LL[c6LL] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6GL++;
                if (c6GL > d6G) {
                    c6GL = 0;
                }
                c6HL++;
                if (c6HL > d6H) {
                    c6HL = 0;
                }
                c6IL++;
                if (c6IL > d6I) {
                    c6IL = 0;
                }
                c6JL++;
                if (c6JL > d6J) {
                    c6JL = 0;
                }
                c6KL++;
                if (c6KL > d6K) {
                    c6KL = 0;
                }
                c6LL++;
                if (c6LL > d6L) {
                    c6LL = 0;
                }
                hA = a6GL[c6GL - ((c6GL > d6G) ? d6G + 1 : 0)];
                hB = a6HL[c6HL - ((c6HL > d6H) ? d6H + 1 : 0)];
                hC = a6IL[c6IL - ((c6IL > d6I) ? d6I + 1 : 0)];
                hD = a6JL[c6JL - ((c6JL > d6J) ? d6J + 1 : 0)];
                hE = a6KL[c6KL - ((c6KL > d6K) ? d6K + 1 : 0)];
                hF = a6LL[c6LL - ((c6LL > d6L) ? d6L + 1 : 0)];
                a6ML[c6ML] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6NL[c6NL] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6OL[c6OL] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6PL[c6PL] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6QL[c6QL] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6RL[c6RL] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6ML++;
                if (c6ML > d6M) {
                    c6ML = 0;
                }
                c6NL++;
                if (c6NL > d6N) {
                    c6NL = 0;
                }
                c6OL++;
                if (c6OL > d6O) {
                    c6OL = 0;
                }
                c6PL++;
                if (c6PL > d6P) {
                    c6PL = 0;
                }
                c6QL++;
                if (c6QL > d6Q) {
                    c6QL = 0;
                }
                c6RL++;
                if (c6RL > d6R) {
                    c6RL = 0;
                }
                hA = a6ML[c6ML - ((c6ML > d6M) ? d6M + 1 : 0)];
                hB = a6NL[c6NL - ((c6NL > d6N) ? d6N + 1 : 0)];
                hC = a6OL[c6OL - ((c6OL > d6O) ? d6O + 1 : 0)];
                hD = a6PL[c6PL - ((c6PL > d6P) ? d6P + 1 : 0)];
                hE = a6QL[c6QL - ((c6QL > d6Q) ? d6Q + 1 : 0)];
                hF = a6RL[c6RL - ((c6RL > d6R) ? d6R + 1 : 0)];
                a6SL[c6SL] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6TL[c6TL] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6UL[c6UL] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6VL[c6VL] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6WL[c6WL] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6XL[c6XL] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6SL++;
                if (c6SL > d6S) {
                    c6SL = 0;
                }
                c6TL++;
                if (c6TL > d6T) {
                    c6TL = 0;
                }
                c6UL++;
                if (c6UL > d6U) {
                    c6UL = 0;
                }
                c6VL++;
                if (c6VL > d6V) {
                    c6VL = 0;
                }
                c6WL++;
                if (c6WL > d6W) {
                    c6WL = 0;
                }
                c6XL++;
                if (c6XL > d6X) {
                    c6XL = 0;
                }
                hA = a6SL[c6SL - ((c6SL > d6S) ? d6S + 1 : 0)];
                hB = a6TL[c6TL - ((c6TL > d6T) ? d6T + 1 : 0)];
                hC = a6UL[c6UL - ((c6UL > d6U) ? d6U + 1 : 0)];
                hD = a6VL[c6VL - ((c6VL > d6V) ? d6V + 1 : 0)];
                hE = a6WL[c6WL - ((c6WL > d6W) ? d6W + 1 : 0)];
                hF = a6XL[c6XL - ((c6XL > d6X) ? d6X + 1 : 0)];
                a6YL[c6YL] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6ZAL[c6ZAL] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6ZBL[c6ZBL] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6ZCL[c6ZCL] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZDL[c6ZDL] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZEL[c6ZEL] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6YL++;
                if (c6YL > d6Y) {
                    c6YL = 0;
                }
                c6ZAL++;
                if (c6ZAL > d6ZA) {
                    c6ZAL = 0;
                }
                c6ZBL++;
                if (c6ZBL > d6ZB) {
                    c6ZBL = 0;
                }
                c6ZCL++;
                if (c6ZCL > d6ZC) {
                    c6ZCL = 0;
                }
                c6ZDL++;
                if (c6ZDL > d6ZD) {
                    c6ZDL = 0;
                }
                c6ZEL++;
                if (c6ZEL > d6ZE) {
                    c6ZEL = 0;
                }
                hA = a6YL[c6YL - ((c6YL > d6Y) ? d6Y + 1 : 0)];
                hB = a6ZAL[c6ZAL - ((c6ZAL > d6ZA) ? d6ZA + 1 : 0)];
                hC = a6ZBL[c6ZBL - ((c6ZBL > d6ZB) ? d6ZB + 1 : 0)];
                hD = a6ZCL[c6ZCL - ((c6ZCL > d6ZC) ? d6ZC + 1 : 0)];
                hE = a6ZDL[c6ZDL - ((c6ZDL > d6ZD) ? d6ZD + 1 : 0)];
                hF = a6ZEL[c6ZEL - ((c6ZEL > d6ZE) ? d6ZE + 1 : 0)];
                a6ZFL[c6ZFL] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6ZGL[c6ZGL] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6ZHL[c6ZHL] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6ZIL[c6ZIL] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZJL[c6ZJL] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZKL[c6ZKL] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6ZFL++;
                if (c6ZFL > d6ZF) {
                    c6ZFL = 0;
                }
                c6ZGL++;
                if (c6ZGL > d6ZG) {
                    c6ZGL = 0;
                }
                c6ZHL++;
                if (c6ZHL > d6ZH) {
                    c6ZHL = 0;
                }
                c6ZIL++;
                if (c6ZIL > d6ZI) {
                    c6ZIL = 0;
                }
                c6ZJL++;
                if (c6ZJL > d6ZJ) {
                    c6ZJL = 0;
                }
                c6ZKL++;
                if (c6ZKL > d6ZK) {
                    c6ZKL = 0;
                }
                hA = a6ZFL[c6ZFL - ((c6ZFL > d6ZF) ? d6ZF + 1 : 0)];
                hB = a6ZGL[c6ZGL - ((c6ZGL > d6ZG) ? d6ZG + 1 : 0)];
                hC = a6ZHL[c6ZHL - ((c6ZHL > d6ZH) ? d6ZH + 1 : 0)];
                hD = a6ZIL[c6ZIL - ((c6ZIL > d6ZI) ? d6ZI + 1 : 0)];
                hE = a6ZJL[c6ZJL - ((c6ZJL > d6ZJ) ? d6ZJ + 1 : 0)];
                hF = a6ZKL[c6ZKL - ((c6ZKL > d6ZK) ? d6ZK + 1 : 0)];
                f6FR = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                f6LR = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                f6RR = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                f6XR = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                f6ZER = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                f6ZKR = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                inputSampleL = ((hA * 2.0) - (hB + hC + hD + hE + hF)) * 0.001953125;
                // right verb
                c6FR++;
                if (c6FR > d6F) {
                    c6FR = 0;
                }
                c6LR++;
                if (c6LR > d6L) {
                    c6LR = 0;
                }
                c6RR++;
                if (c6RR > d6R) {
                    c6RR = 0;
                }
                c6XR++;
                if (c6XR > d6X) {
                    c6XR = 0;
                }
                c6ZER++;
                if (c6ZER > d6ZE) {
                    c6ZER = 0;
                }
                c6ZKR++;
                if (c6ZKR > d6ZK) {
                    c6ZKR = 0;
                }
                hA = a6FR[c6FR - ((c6FR > d6F) ? d6F + 1 : 0)];
                hB = a6LR[c6LR - ((c6LR > d6L) ? d6L + 1 : 0)];
                hC = a6RR[c6RR - ((c6RR > d6R) ? d6R + 1 : 0)];
                hD = a6XR[c6XR - ((c6XR > d6X) ? d6X + 1 : 0)];
                hE = a6ZER[c6ZER - ((c6ZER > d6ZE) ? d6ZE + 1 : 0)];
                hF = a6ZKR[c6ZKR - ((c6ZKR > d6ZK) ? d6ZK + 1 : 0)];
                a6ER[c6ER] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6KR[c6KR] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6QR[c6QR] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6WR[c6WR] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZDR[c6ZDR] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZJR[c6ZJR] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6ER++;
                if (c6ER > d6E) {
                    c6ER = 0;
                }
                c6KR++;
                if (c6KR > d6K) {
                    c6KR = 0;
                }
                c6QR++;
                if (c6QR > d6Q) {
                    c6QR = 0;
                }
                c6WR++;
                if (c6WR > d6W) {
                    c6WR = 0;
                }
                c6ZDR++;
                if (c6ZDR > d6ZD) {
                    c6ZDR = 0;
                }
                c6ZJR++;
                if (c6ZJR > d6ZJ) {
                    c6ZJR = 0;
                }
                hA = a6ER[c6ER - ((c6ER > d6E) ? d6E + 1 : 0)];
                hB = a6KR[c6KR - ((c6KR > d6K) ? d6K + 1 : 0)];
                hC = a6QR[c6QR - ((c6QR > d6Q) ? d6Q + 1 : 0)];
                hD = a6WR[c6WR - ((c6WR > d6W) ? d6W + 1 : 0)];
                hE = a6ZDR[c6ZDR - ((c6ZDR > d6ZD) ? d6ZD + 1 : 0)];
                hF = a6ZJR[c6ZJR - ((c6ZJR > d6ZJ) ? d6ZJ + 1 : 0)];
                a6DR[c6DR] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6JR[c6JR] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6PR[c6PR] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6VR[c6VR] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZCR[c6ZCR] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZIR[c6ZIR] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6DR++;
                if (c6DR > d6D) {
                    c6DR = 0;
                }
                c6JR++;
                if (c6JR > d6J) {
                    c6JR = 0;
                }
                c6PR++;
                if (c6PR > d6P) {
                    c6PR = 0;
                }
                c6VR++;
                if (c6VR > d6V) {
                    c6VR = 0;
                }
                c6ZCR++;
                if (c6ZCR > d6ZC) {
                    c6ZCR = 0;
                }
                c6ZIR++;
                if (c6ZIR > d6ZI) {
                    c6ZIR = 0;
                }
                hA = a6DR[c6DR - ((c6DR > d6D) ? d6D + 1 : 0)];
                hB = a6JR[c6JR - ((c6JR > d6J) ? d6J + 1 : 0)];
                hC = a6PR[c6PR - ((c6PR > d6P) ? d6P + 1 : 0)];
                hD = a6VR[c6VR - ((c6VR > d6V) ? d6V + 1 : 0)];
                hE = a6ZCR[c6ZCR - ((c6ZCR > d6ZC) ? d6ZC + 1 : 0)];
                hF = a6ZIR[c6ZIR - ((c6ZIR > d6ZI) ? d6ZI + 1 : 0)];
                a6CR[c6CR] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6IR[c6IR] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6OR[c6OR] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6UR[c6UR] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZBR[c6ZBR] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZHR[c6ZHR] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6CR++;
                if (c6CR > d6C) {
                    c6CR = 0;
                }
                c6IR++;
                if (c6IR > d6I) {
                    c6IR = 0;
                }
                c6OR++;
                if (c6OR > d6O) {
                    c6OR = 0;
                }
                c6UR++;
                if (c6UR > d6U) {
                    c6UR = 0;
                }
                c6ZBR++;
                if (c6ZBR > d6ZB) {
                    c6ZBR = 0;
                }
                c6ZHR++;
                if (c6ZHR > d6ZH) {
                    c6ZHR = 0;
                }
                hA = a6CR[c6CR - ((c6CR > d6C) ? d6C + 1 : 0)];
                hB = a6IR[c6IR - ((c6IR > d6I) ? d6I + 1 : 0)];
                hC = a6OR[c6OR - ((c6OR > d6O) ? d6O + 1 : 0)];
                hD = a6UR[c6UR - ((c6UR > d6U) ? d6U + 1 : 0)];
                hE = a6ZBR[c6ZBR - ((c6ZBR > d6ZB) ? d6ZB + 1 : 0)];
                hF = a6ZHR[c6ZHR - ((c6ZHR > d6ZH) ? d6ZH + 1 : 0)];
                a6BR[c6BR] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6HR[c6HR] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6NR[c6NR] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6TR[c6TR] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6ZAR[c6ZAR] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZGR[c6ZGR] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6BR++;
                if (c6BR > d6B) {
                    c6BR = 0;
                }
                c6HR++;
                if (c6HR > d6H) {
                    c6HR = 0;
                }
                c6NR++;
                if (c6NR > d6N) {
                    c6NR = 0;
                }
                c6TR++;
                if (c6TR > d6T) {
                    c6TR = 0;
                }
                c6ZBR++;
                if (c6ZBR > d6ZB) {
                    c6ZBR = 0;
                }
                c6ZGR++;
                if (c6ZGR > d6ZG) {
                    c6ZGR = 0;
                }
                hA = a6BR[c6BR - ((c6BR > d6B) ? d6B + 1 : 0)];
                hB = a6HR[c6HR - ((c6HR > d6H) ? d6H + 1 : 0)];
                hC = a6NR[c6NR - ((c6NR > d6N) ? d6N + 1 : 0)];
                hD = a6TR[c6TR - ((c6TR > d6T) ? d6T + 1 : 0)];
                hE = a6ZAR[c6ZAR - ((c6ZAR > d6ZA) ? d6ZA + 1 : 0)];
                hF = a6ZGR[c6ZGR - ((c6ZGR > d6ZG) ? d6ZG + 1 : 0)];
                a6AR[c6AR] = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                a6GR[c6GR] = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                a6MR[c6MR] = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                a6SR[c6SR] = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                a6YR[c6YR] = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                a6ZFR[c6ZFR] = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                c6AR++;
                if (c6AR > d6A) {
                    c6AR = 0;
                }
                c6GR++;
                if (c6GR > d6G) {
                    c6GR = 0;
                }
                c6MR++;
                if (c6MR > d6M) {
                    c6MR = 0;
                }
                c6SR++;
                if (c6SR > d6S) {
                    c6SR = 0;
                }
                c6YR++;
                if (c6YR > d6Y) {
                    c6YR = 0;
                }
                c6ZFR++;
                if (c6ZFR > d6ZF) {
                    c6ZFR = 0;
                }
                hA = a6AR[c6AR - ((c6AR > d6A) ? d6A + 1 : 0)];
                hB = a6GR[c6GR - ((c6GR > d6G) ? d6G + 1 : 0)];
                hC = a6MR[c6MR - ((c6MR > d6M) ? d6M + 1 : 0)];
                hD = a6SR[c6SR - ((c6SR > d6S) ? d6S + 1 : 0)];
                hE = a6YR[c6YR - ((c6YR > d6Y) ? d6Y + 1 : 0)];
                hF = a6ZFR[c6ZFR - ((c6ZFR > d6ZF) ? d6ZF + 1 : 0)];
                f6AL = ((hA * 2.0) - (hB + hC + hD + hE + hF));
                f6BL = ((hB * 2.0) - (hA + hC + hD + hE + hF));
                f6CL = ((hC * 2.0) - (hA + hB + hD + hE + hF));
                f6DL = ((hD * 2.0) - (hA + hB + hC + hE + hF));
                f6EL = ((hE * 2.0) - (hA + hB + hC + hD + hF));
                f6FL = ((hF * 2.0) - (hA + hB + hC + hD + hE));
                inputSampleR = ((hA * 2.0) - (hB + hC + hD + hE + hF)) * 0.001953125;
                bezF[bez_cycle] += derezFreq;
                bezF[bez_SampL] += (inputSampleL * derezFreq);
                bezF[bez_SampR] += (inputSampleR * derezFreq);
                if (bezF[bez_cycle] > 1.0) { // hit the end point and we do a filter sample
                    if (steppedFreq) {
                        bezF[bez_cycle] = 0.0;
                    }
                    else {
                        bezF[bez_cycle] -= 1.0;
                    }
                    bezF[bez_CL] = bezF[bez_BL];
                    bezF[bez_BL] = bezF[bez_AL];
                    bezF[bez_AL] = (bezF[bez_SampL]);
                    bezF[bez_SampL] = 0.0;
                    bezF[bez_CR] = bezF[bez_BR];
                    bezF[bez_BR] = bezF[bez_AR];
                    bezF[bez_AR] = (bezF[bez_SampR]);
                    bezF[bez_SampR] = 0.0;
                }
                double X = bezF[bez_cycle] * bezFreqTrim;
                double CBLfreq = (bezF[bez_CL] * (1.0 - X)) + (bezF[bez_BL] * X);
                double BALfreq = (bezF[bez_BL] * (1.0 - X)) + (bezF[bez_AL] * X);
                inputSampleL = (bezF[bez_BL] + (CBLfreq * (1.0 - X)) + (BALfreq * X)) * 0.125;
                double CBRfreq = (bezF[bez_CR] * (1.0 - X)) + (bezF[bez_BR] * X);
                double BARfreq = (bezF[bez_BR] * (1.0 - X)) + (bezF[bez_AR] * X);
                inputSampleR = (bezF[bez_BR] + (CBRfreq * (1.0 - X)) + (BARfreq * X)) * 0.125;
                inputSampleL = bezF[bez_IIRL] = (inputSampleL * derezFreq) + (bezF[bez_IIRL] * (1.0 - derezFreq));
                inputSampleR = bezF[bez_IIRR] = (inputSampleR * derezFreq) + (bezF[bez_IIRR] * (1.0 - derezFreq));
                inputSampleL += (earlyReflectionL * earlyLoudness);
                inputSampleR += (earlyReflectionR * earlyLoudness);
                bez[bez_CL] = bez[bez_BL];
                bez[bez_BL] = bez[bez_AL];
                bez[bez_AL] = inputSampleL;
                bez[bez_SampL] = 0.0;
                bez[bez_CR] = bez[bez_BR];
                bez[bez_BR] = bez[bez_AR];
                bez[bez_AR] = inputSampleR;
                bez[bez_SampR] = 0.0;
            }
            double X = bez[bez_cycle] * bezTrim;
            double CBL = (bez[bez_CL] * (1.0 - X)) + (bez[bez_BL] * X);
            double CBR = (bez[bez_CR] * (1.0 - X)) + (bez[bez_BR] * X);
            double BAL = (bez[bez_BL] * (1.0 - X)) + (bez[bez_AL] * X);
            double BAR = (bez[bez_BR] * (1.0 - X)) + (bez[bez_AR] * X);
            inputSampleL = (bez[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * -0.25;
            inputSampleR = (bez[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * -0.25;
            inputSampleL = bez[bez_IIRL] = (inputSampleL * derez) + (bez[bez_IIRL] * (1.0 - derez));
            inputSampleR = bez[bez_IIRR] = (inputSampleR * derez) + (bez[bez_IIRR] * (1.0 - derez));
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
} // namespace airwindohhs::kwoodroom
