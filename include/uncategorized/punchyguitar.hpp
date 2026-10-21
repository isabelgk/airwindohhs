#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::punchyguitar {

constexpr std::string_view k_name{ "PunchyGuitar" };
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
class PunchyGuitar final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'pugt';
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
    double bip[bip_total][12];
    double angSL[18][12];
    double angAL[18][12];
    double iirHPositionL[37];
    double iirHAngleL[37];
    double iirBPositionL[37];
    double iirBAngleL[37];
    bool WasNegativeL;
    int ZeroCrossL;
    double gaterollerL;
    double gateL;
    double angSR[18][12];
    double angAR[18][12];
    double iirHPositionR[37];
    double iirHAngleR[37];
    double iirBPositionR[37];
    double iirBAngleR[37];
    bool WasNegativeR;
    int ZeroCrossR;
    double gaterollerR;
    double gateR;
    double angG[12];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PunchyGuitar()
    {
        A = 0.7;
        B = 0.1;
        C = 0.8;
        D = 0.6;
        E = 0.4;
        F = 0.1;
        G = 0.8;
        H = 0.5;
        I = 0.0;
        J = 0.3;
        for (int x = 0; x < 17; x++) {
            for (int y = 0; y < 11; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 11; y++) {
            angG[y] = 0.0;
            for (int x = 0; x < bip_total; x++) {
                bip[x][y] = 0.0;
            }
        }
        for (int count = 0; count < 36; count++) {
            iirHPositionL[count] = 0.0;
            iirHAngleL[count] = 0.0;
            iirBPositionL[count] = 0.0;
            iirBAngleL[count] = 0.0;
            iirHPositionR[count] = 0.0;
            iirHAngleR[count] = 0.0;
            iirBPositionR[count] = 0.0;
            iirBAngleR[count] = 0.0;
        }
        WasNegativeL = false;
        ZeroCrossL = 0;
        gaterollerL = 0.0;
        WasNegativeR = false;
        ZeroCrossR = 0;
        gaterollerR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        // this is reset: values being initialized only once. Startup values, whatever they are.
        gateL = 0.0;
        gateR = 0.0;
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
            case kParamA: return 0.7; break;
            case kParamB: return 0.1; break;
            case kParamC: return 0.8; break;
            case kParamD: return 0.6; break;
            case kParamE: return 0.4; break;
            case kParamF: return 0.1; break;
            case kParamG: return 0.8; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.0; break;
            case kParamJ: return 0.3; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive"; break;
            case kParamB: return "presnce"; break;
            case kParamC: return "high"; break;
            case kParamD: return "mid"; break;
            case kParamE: return "low"; break;
            case kParamF: return "sub"; break;
            case kParamG: return "hspeakr"; break;
            case kParamH: return "lspeakr"; break;
            case kParamI: return "gate"; break;
            case kParamJ: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;
            case kParamB: return "Presnce"; break;
            case kParamC: return "High"; break;
            case kParamD: return "Mid"; break;
            case kParamE: return "Low"; break;
            case kParamF: return "Sub"; break;
            case kParamG: return "HSpeakr"; break;
            case kParamH: return "LSpeakr"; break;
            case kParamI: return "Gate"; break;
            case kParamJ: return "Output"; break;

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
        double drive = A + 0.618033988749894;
        angG[0] = sqrt(B * 2.0);
        angG[2] = sqrt(C * 2.0);
        angG[4] = sqrt(D * 2.0);
        angG[6] = sqrt(E * 2.0);
        angG[8] = sqrt(F * 2.0);
        angG[1] = (angG[0] + angG[2]) * 0.5;
        angG[3] = (angG[2] + angG[4]) * 0.5;
        angG[5] = (angG[4] + angG[6]) * 0.5;
        angG[7] = (angG[6] + angG[8]) * 0.5;
        angG[9] = angG[8];
        int poles = (int)(drive * 10.0);
        double hFreq = pow(G, overallscale);
        double lFreq = pow(H, overallscale + 3.0);
        // begin Gate
        double onthreshold = (pow(I, 3) / 3) + 0.00018;
        double offthreshold = onthreshold * 1.1;
        double release = 0.028331119964586;
        double absmax = 220.9;
        // end Gate
        double output = J;
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
            // begin Gate
            if (inputSampleL > 0.0)
            {
                if (WasNegativeL == true) {
                    ZeroCrossL = absmax * 0.3;
                }
                WasNegativeL = false;
            }
            else {
                ZeroCrossL += 1;
                WasNegativeL = true;
            }
            if (ZeroCrossL > absmax) {
                ZeroCrossL = absmax;
            }
            if (gateL == 0.0)
            {
                // if gateL is totally silent
                if (fabs(inputSampleL) > onthreshold)
                {
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
                // gateL is not silent but closing
                if (fabs(inputSampleL) > offthreshold)
                {
                    if (gaterollerL < ZeroCrossL) {
                        gaterollerL = ZeroCrossL;
                    }
                    else {
                        gaterollerL -= release;
                    }
                    // always trigger if gateL is over offthreshold, otherwise close anyway
                }
                else {
                    gaterollerL -= release;
                }
            }
            if (gaterollerL < 0.0) {
                gaterollerL = 0.0;
            }
            for (int x = 0; x < poles; x++) {
                double fr = 0.9 / overallscale;
                double band = inputSampleL;
                inputSampleL = 0.0;
                for (int y = 0; y < 9; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((band - angSL[x][y]) * fr);
                    double temp = band;
                    band = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleL += ((temp - band) * angG[y]);
                    fr *= 0.618033988749894;
                }
                inputSampleL += (band * angG[9]);
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
            }
            if (gaterollerL < 1.0)
            {
                gateL = gaterollerL;
                double bridgerectifier = 1 - cos(fabs(inputSampleL));
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
            // end Gate
            double lowSample = inputSampleL;
            for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
                iirBAngleL[count] = (iirBAngleL[count] * (1.0 - lFreq)) + ((lowSample - iirBPositionL[count]) * lFreq);
                lowSample = ((iirBPositionL[count] + (iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                iirBPositionL[count] = ((iirBPositionL[count] + (iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                inputSampleL -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
            }
            for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
                iirHAngleL[count] = (iirHAngleL[count] * (1.0 - hFreq)) + ((inputSampleL - iirHPositionL[count]) * hFreq);
                inputSampleL = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
                iirHPositionL[count] = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
            } // the lowpass
            inputSampleL *= output;
            // begin Gate
            if (inputSampleR > 0.0)
            {
                if (WasNegativeR == true) {
                    ZeroCrossR = absmax * 0.3;
                }
                WasNegativeR = false;
            }
            else {
                ZeroCrossR += 1;
                WasNegativeR = true;
            }
            if (ZeroCrossR > absmax) {
                ZeroCrossR = absmax;
            }
            if (gateR == 0.0)
            {
                // if gateR is totally silent
                if (fabs(inputSampleR) > onthreshold)
                {
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
                // gateR is not silent but closing
                if (fabs(inputSampleR) > offthreshold)
                {
                    if (gaterollerR < ZeroCrossR) {
                        gaterollerR = ZeroCrossR;
                    }
                    else {
                        gaterollerR -= release;
                    }
                    // always trigger if gateR is over offthreshold, otherwise close anyway
                }
                else {
                    gaterollerR -= release;
                }
            }
            if (gaterollerR < 0.0) {
                gaterollerR = 0.0;
            }
            for (int x = 0; x < poles; x++) {
                double fr = 0.9 / overallscale;
                double band = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 9; y++) {
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((band - angSR[x][y]) * fr);
                    double temp = band;
                    band = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleR += ((temp - band) * angG[y]);
                    fr *= 0.618033988749894;
                }
                inputSampleR += (band * angG[9]);
                inputSampleR *= drive;
                double bip_delta = inputSampleR; // delta can be just local and re-used
                inputSampleR = fmin(fmax(inputSampleR, -M_PI_2), M_PI_2);
                long double X = inputSampleR;
                X *= X; // long double for even
                long double temp = inputSampleR * X; // the initial multiplies
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
                double bip_dvE = bip[bip_pvRD][x] - bip[bip_dvRD][x];
                bip[bip_pvRD][x] = bip[bip_dvRD][x]; // snap
                inputSampleR *= (1.0 + (fabs(bip[bip_dvRC][x]) * 0.0618) + (fabs(bip[bip_dvRD][x]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
            }
            if (gaterollerR < 1.0)
            {
                gateR = gaterollerR;
                double bridgerectifier = 1 - cos(fabs(inputSampleR));
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
            // end Gate
            lowSample = inputSampleR;
            for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
                iirBAngleR[count] = (iirBAngleR[count] * (1.0 - lFreq)) + ((lowSample - iirBPositionR[count]) * lFreq);
                lowSample = ((iirBPositionR[count] + (iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                iirBPositionR[count] = ((iirBPositionR[count] + (iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                inputSampleR -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
            }
            for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
                iirHAngleR[count] = (iirHAngleR[count] * (1.0 - hFreq)) + ((inputSampleR - iirHPositionR[count]) * hFreq);
                inputSampleR = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
                iirHPositionR[count] = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
            } // the lowpass
            inputSampleR *= output;
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
} // namespace airwindohhs::punchyguitar
