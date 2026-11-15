#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::punchydeluxe2 {

constexpr std::string_view k_name{ "PunchyDeluxe2" };
constexpr std::string_view k_short_description{
    "PunchyDeluxe2 is more useable PunchyDeluxe."
};
constexpr std::string_view k_long_description{
    "No more fiddling with little sliders unless you actually want to! PunchyDeluxe2 is hot on the heels of PunchyDeluxe, because I found some stuff to do that made it a lot better. Instead of a Hell (as in, Slipperman called this band 'road to hell, lose a windshield up here') there's a Top control. Instead of Rarely (Slipperman said 'never say never, just say rarely') there's a Bottom control. And they still control the bands they used to control. Only now, they set up a baseline where all the other controls start… and that changes everything. If you want some top boost and bright crunchiness, you can start by just increasing Top. It'll take care of everything. Lows too intense, need to rein them in? Previously a headache, now you can just turn Bottom down. It'll scale anything else you had set up, letting you just bring it into focus. PunchyDeluxe2 uses fewer gain stages than PunchyDeluxe did. That's partly to cut back on the CPU hungriness slightly, but it's also because the new version lends itself to traditional amp sounds. Since its saturation is so extremely smooth, this gives you a more useable adjustment range for all the controls, making it easier to dial in less high-saturation sounds. It's still the 'ten band crazy guitar amp monstrosity' at heart, but now through making the bands simply be offsets on a giant tilt EQ set up by Top and Bottom, everything gets way easier to dial in. And of course once you've got the basic idea roughed in, you can still go bonkers adjusting every little band. But now it's way quicker to get started, and easier to find where you were going :)"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class PunchyDeluxe2 final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'pdx2';
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
    enum
    {
        bip_dvLA,
        bip_dvLB,
        bip_dvLC,
        bip_dvLD,
        bip_pvLA,
        bip_pvLB,
        bip_pvLC,
        bip_pvLD,
        bip_dvRA,
        bip_dvRB,
        bip_dvRC,
        bip_dvRD,
        bip_pvRA,
        bip_pvRB,
        bip_pvRC,
        bip_pvRD,
        bip_total // each distortion section can have one of these, it stacks well
    }; // not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total][10];
    double bop[bip_total];
    double angSL[10][17];
    double angAL[10][17];
    double angSR[10][17];
    double angAR[10][17];
    double angG[17];
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PunchyDeluxe2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 0.5;
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 16; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
            for (int y = 0; y < bip_total; y++) {
                bip[y][x] = 0.0;
            }
        }
        for (int y = 0; y < 16; y++) {
            angG[y] = 0.0;
        }
        for (int x = 0; x < bip_total; x++) {
            bop[x] = 0.0;
        }
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
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
            case kParamI: return 0.5; break;
            case kParamJ: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "top"; break;
            case kParamB: return "fizz"; break;
            case kParamC: return "pick"; break;
            case kParamD: return "satan"; break;
            case kParamE: return "danger"; break;
            case kParamF: return "crtical"; break;
            case kParamG: return "hmeat"; break;
            case kParamH: return "lmeat"; break;
            case kParamI: return "swing"; break;
            case kParamJ: return "bottom"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Top"; break;
            case kParamB: return "Fizz"; break;
            case kParamC: return "Pick"; break;
            case kParamD: return "Satan"; break;
            case kParamE: return "Danger"; break;
            case kParamF: return "Crtical"; break;
            case kParamG: return "H Meat"; break;
            case kParamH: return "L Meat"; break;
            case kParamI: return "Swing"; break;
            case kParamJ: return "Bottom"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(A, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(B, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(C, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(D, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(E, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(F, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(G, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(H, text, kVstMaxParamStrLen); break;
            case kParamI: float2string(I, text, kVstMaxParamStrLen); break;
            case kParamJ: float2string(J, text, kVstMaxParamStrLen); break;
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
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
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
        overallscale *= Effect<T>::getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        angG[0] = 1.0;
        angG[1] = (A + 0.5);
        angG[10] = (J + 0.5);
        double stepValue = (angG[10] - angG[1]) * 0.1;
        for (int y = 1; y < 15; y++) {
            angG[y] = fmax(angG[1] + (stepValue * y), 0.0);
        }
        double drive = angG[1];
        angG[2] *= B + 0.5;
        if (drive < angG[2]) {
            drive = angG[2];
        }
        angG[3] *= C + 0.5;
        if (drive < angG[3]) {
            drive = angG[3];
        }
        angG[4] *= D + 0.5;
        if (drive < angG[4]) {
            drive = angG[4];
        }
        angG[5] *= E + 0.5;
        if (drive < angG[5]) {
            drive = angG[5];
        }
        angG[6] *= F + 0.5;
        if (drive < angG[6]) {
            drive = angG[6];
        }
        angG[7] *= G + 0.5;
        if (drive < angG[7]) {
            drive = angG[7];
        }
        angG[8] *= H + 0.5;
        if (drive < angG[8]) {
            drive = angG[8];
        }
        angG[9] *= I + 0.5;
        if (drive < angG[9]) {
            drive = angG[9];
        }
        double tune = 0.618 + (overallscale * 0.0055);
        double pad = sqrt(1.0 / drive);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            for (int x = 0; x < 9; x++) {
                inputSampleL *= drive;
                double bip_delta = inputSampleL; // delta can be just local and re-used
                inputSampleL = fmin(fmax(inputSampleL, -M_PI_2), M_PI_2);
                long double X = inputSampleL;
                X *= X; // long double for even
                long double temp = inputSampleL * X; // the initial multiplies
                inputSampleL -= temp * 0.16666666666666666666666666666666666;
                temp *= X;
                inputSampleL += temp * 0.00833333333333333333333333333333333;
                temp *= X;
                inputSampleL -= temp * 0.00019841269841269841269841269841269;
                temp *= X;
                inputSampleL += temp * 0.00000275573192239858906525573192239;
                temp *= X;
                inputSampleL -= temp * 0.00000002505210838544171877521083854;
                temp *= X;
                inputSampleL += temp * 0.00000000016059043836821614599392377;
                temp *= X;
                inputSampleL -= temp * 0.00000000000076471637318198164759011;
                temp *= X;
                inputSampleL += temp * 0.00000000000000281145725434552076319;
                temp *= X;
                inputSampleL -= temp * 0.00000000000000000822063524662432971;
                temp *= X;
                inputSampleL += temp * 0.00000000000000000001957294106339126;
                // retain mantissa of a long double increasing power function
                // long double probably doesn't handle more than 36 digits or so
                bip[bip_dvLA][x] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
                bip[bip_dvLB][x] = bip[bip_pvLA][x] - bip[bip_dvLA][x];
                bip[bip_pvLA][x] = bip[bip_dvLA][x]; // velocity
                bip[bip_dvLC][x] = bip[bip_pvLB][x] - bip[bip_dvLB][x];
                bip[bip_pvLB][x] = bip[bip_dvLB][x]; // acceleration
                bip[bip_dvLD][x] = bip[bip_pvLC][x] - bip[bip_dvLC][x];
                bip[bip_pvLC][x] = bip[bip_dvLC][x]; // jerk
                double bip_dvE = bip[bip_pvLD][x] - bip[bip_dvLD][x];
                bip[bip_pvLD][x] = bip[bip_dvLD][x]; // snap
                inputSampleL *= (1.0 + (fabs(bip[bip_dvLC][x]) * 0.0618) + (fabs(bip[bip_dvLD][x]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
                double fr = (0.92 / overallscale) + (overallscale * 0.01);
                double band = inputSampleL;
                inputSampleL = 0.0;
                for (int y = 0; y < 14; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((band - angSL[x][y]) * fr);
                    double temp = band;
                    band = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleL += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleL += band * angG[14];
                inputSampleR *= drive;
                bip_delta = inputSampleR; // delta can be just local and re-used
                inputSampleR = fmin(fmax(inputSampleR, -M_PI_2), M_PI_2);
                X = inputSampleR;
                X *= X; // long double for even
                temp = inputSampleR * X; // the initial multiplies
                inputSampleR -= temp * 0.16666666666666666666666666666666666;
                temp *= X;
                inputSampleR += temp * 0.00833333333333333333333333333333333;
                temp *= X;
                inputSampleR -= temp * 0.00019841269841269841269841269841269;
                temp *= X;
                inputSampleR += temp * 0.00000275573192239858906525573192239;
                temp *= X;
                inputSampleR -= temp * 0.00000002505210838544171877521083854;
                temp *= X;
                inputSampleR += temp * 0.00000000016059043836821614599392377;
                temp *= X;
                inputSampleR -= temp * 0.00000000000076471637318198164759011;
                temp *= X;
                inputSampleR += temp * 0.00000000000000281145725434552076319;
                temp *= X;
                inputSampleR -= temp * 0.00000000000000000822063524662432971;
                temp *= X;
                inputSampleR += temp * 0.00000000000000000001957294106339126;
                // retain mantissa of a long double increasing power function
                // long double probably doesn't handle more than 36 digits or so
                bip[bip_dvRA][x] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
                bip[bip_dvRB][x] = bip[bip_pvRA][x] - bip[bip_dvRA][x];
                bip[bip_pvRA][x] = bip[bip_dvRA][x]; // velocity
                bip[bip_dvRC][x] = bip[bip_pvRB][x] - bip[bip_dvRB][x];
                bip[bip_pvRB][x] = bip[bip_dvRB][x]; // acceleration
                bip[bip_dvRD][x] = bip[bip_pvRC][x] - bip[bip_dvRC][x];
                bip[bip_pvRC][x] = bip[bip_dvRC][x]; // jerk
                bip_dvE = bip[bip_pvRD][x] - bip[bip_dvRD][x];
                bip[bip_pvRD][x] = bip[bip_dvRD][x]; // snap
                inputSampleR *= (1.0 + (fabs(bip[bip_dvRC][x]) * 0.0618) + (fabs(bip[bip_dvRD][x]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
                fr = (0.92 / overallscale) + (overallscale * 0.01);
                band = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 14; y++) {
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((band - angSR[x][y]) * fr);
                    double temp = band;
                    band = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleR += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleR += band * angG[14];
            }
            if (pad < 1.0) {
                inputSampleL *= pad;
                inputSampleR *= pad;
            }
            double bop_delta = inputSampleL; // delta can be just local and re-used
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            bop[bip_dvLA] = bop_delta - inputSampleL; // these are derivatives: raw clip is position
            bop[bip_dvLB] = bop[bip_pvLA] - bop[bip_dvLA];
            bop[bip_pvLA] = bop[bip_dvLA]; // velocity
            bop[bip_dvLC] = bop[bip_pvLB] - bop[bip_dvLB];
            bop[bip_pvLB] = bop[bip_dvLB]; // acceleration
            bop[bip_dvLD] = bop[bip_pvLC] - bop[bip_dvLC];
            bop[bip_pvLC] = bop[bip_dvLC]; // jerk
            double bop_dvE = bop[bip_pvLD] - bop[bip_dvLD];
            bop[bip_pvLD] = bop[bip_dvLD]; // snap
            inputSampleL *= (1.0 + (fabs(bop[bip_dvLC]) * 0.0618) + (fabs(bop[bip_dvLD]) * -0.05982) + (fabs(bop_dvE) * 0.0206));
            bop_delta = inputSampleR; // delta can be just local and re-used
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            bop[bip_dvRA] = bop_delta - inputSampleR; // these are derivatives: raw clip is position
            bop[bip_dvRB] = bop[bip_pvRA] - bop[bip_dvRA];
            bop[bip_pvRA] = bop[bip_dvRA]; // velocity
            bop[bip_dvRC] = bop[bip_pvRB] - bop[bip_dvRB];
            bop[bip_pvRB] = bop[bip_dvRB]; // acceleration
            bop[bip_dvRD] = bop[bip_pvRC] - bop[bip_dvRC];
            bop[bip_pvRC] = bop[bip_dvRC]; // jerk
            bop_dvE = bop[bip_pvRD] - bop[bip_dvRD];
            bop[bip_pvRD] = bop[bip_dvRD]; // snap
            inputSampleR *= (1.0 + (fabs(bop[bip_dvRC]) * 0.0618) + (fabs(bop[bip_dvRD]) * -0.05982) + (fabs(bop_dvE) * 0.0206));
            if (pad < 1.0) {
                inputSampleL *= pad;
                inputSampleR *= pad;
            }
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::punchydeluxe2
