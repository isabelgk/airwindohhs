#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::punchydeluxe {

constexpr std::string_view k_name{ "PunchyDeluxe" };
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
class PunchyDeluxe final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'pudx';
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
    double bip[bip_total][16];
    double bop[bip_total];
    double angSL[18][16];
    double angAL[18][16];
    double angSR[18][16];
    double angAR[18][16];
    double angG[16];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PunchyDeluxe()
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
        for (int x = 0; x < 17; x++) {
            for (int y = 0; y < 14; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 14; y++) {
            angG[y] = 0.0;
            for (int x = 0; x < bip_total; x++) {
                bip[x][y] = 0.0;
            }
        }
        for (int x = 0; x < bip_total; x++) {
            bop[x] = 0.0;
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
            case kParamI: return I; break;
            case kParamJ: return J; break;

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
        switch (static_cast<params>(index))
        {
            case kParamA: return "hell"; break;
            case kParamB: return "fizz"; break;
            case kParamC: return "pick"; break;
            case kParamD: return "satan"; break;
            case kParamE: return "danger"; break;
            case kParamF: return "crtical"; break;
            case kParamG: return "hmeat"; break;
            case kParamH: return "lmeat"; break;
            case kParamI: return "swing"; break;
            case kParamJ: return "rarely"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Hell"; break;
            case kParamB: return "Fizz"; break;
            case kParamC: return "Pick"; break;
            case kParamD: return "Satan"; break;
            case kParamE: return "Danger"; break;
            case kParamF: return "Crtical"; break;
            case kParamG: return "H Meat"; break;
            case kParamH: return "L Meat"; break;
            case kParamI: return "Swing"; break;
            case kParamJ: return "Rarely"; break;

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
        double drive = 1.0;
        double pad = 1.0;
        angG[1] = sqrt(A * 2.0);
        if (pad > angG[1]) {
            pad = angG[1];
        }
        if (drive < angG[1]) {
            drive = angG[1];
        }
        angG[0] = (1.0 + angG[1]) * 0.5; // if boost, boost half
        if (angG[0] > angG[1]) {
            angG[0] = angG[1];
        }
        angG[2] = sqrt(B * 2.0);
        if (pad > angG[2]) {
            pad = angG[2];
        }
        if (drive < angG[2]) {
            drive = angG[2];
        }
        angG[3] = sqrt(C * 2.0);
        if (pad > angG[3]) {
            pad = angG[3];
        }
        if (drive < angG[3]) {
            drive = angG[3];
        }
        angG[4] = sqrt(D * 2.0);
        if (pad > angG[4]) {
            pad = angG[4];
        }
        if (drive < angG[4]) {
            drive = angG[4];
        }
        angG[5] = sqrt(E * 2.0);
        if (pad > angG[5]) {
            pad = angG[5];
        }
        if (drive < angG[5]) {
            drive = angG[5];
        }
        angG[6] = sqrt(F * 2.0);
        if (pad > angG[6]) {
            pad = angG[6];
        }
        if (drive < angG[6]) {
            drive = angG[6];
        }
        angG[7] = sqrt(G * 2.0);
        if (pad > angG[7]) {
            pad = angG[7];
        }
        if (drive < angG[7]) {
            drive = angG[7];
        }
        angG[8] = sqrt(H * 2.0);
        if (pad > angG[8]) {
            pad = angG[8];
        }
        if (drive < angG[8]) {
            drive = angG[8];
        }
        angG[9] = sqrt(I * 2.0);
        if (pad > angG[9]) {
            pad = angG[9];
        }
        if (drive < angG[9]) {
            drive = angG[9];
        }
        angG[10] = sqrt(J * 2.0);
        if (pad > angG[10]) {
            pad = angG[10];
        }
        if (drive < angG[10]) {
            drive = angG[10];
        }
        angG[11] = 1.0;
        angG[12] = 1.0;
        pad = sqrt(pad);
        int poles = (int)(drive * 11.0);
        double tune = 0.618 + (overallscale * 0.0055);
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
            for (int x = 0; x < poles; x++) {
                double fr = (0.92 / overallscale) + (overallscale * 0.01);
                double band = inputSampleL;
                inputSampleL = 0.0;
                for (int y = 0; y < 12; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((band - angSL[x][y]) * fr);
                    double temp = band;
                    band = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleL += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleL += band;
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
                fr = (0.92 / overallscale) + (overallscale * 0.01);
                band = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 12; y++) {
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((band - angSR[x][y]) * fr);
                    double temp = band;
                    band = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleR += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleR += band;
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
            }
            if (pad < 1.0) {
                inputSampleL *= pad;
                inputSampleR *= pad;
            }
            double bop_delta = inputSampleL; // delta can be just local and re-used
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
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::punchydeluxe
