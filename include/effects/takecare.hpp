#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::takecare {

constexpr std::string_view k_name{ "TakeCare" };
constexpr std::string_view k_short_description{
    "TakeCare is a lush chorus ensemble universe."
};
constexpr std::string_view k_long_description{
    "The idea was to revisit plugins like Melt, with an eye to even more disconcerting sonic spaces like the works of The Caretaker. Seemed reasonable to do. I started off with my Melt plugin, to see what would happen.Then, in working on Householder matrices, I found that using a Householder matrix for a 3x3 grid of delays (rather than simply using them as delays, which I'd done before, thinking 'what difference could there possibly be?') made a HUGE difference. This is what's creating early reflections in my plugins post kBeyond (which it turned out I immediately went beyond).So, if you can do that with a 3x3 grid of delays that produces 27 echoes per channel… what if those delays were instead a chorus ensemble machine?This is TakeCare. Most of the work I did on it, was using synth pads in order to make giant illbient soundscapes, and indeed it's quite good at that. You can get real seasick off TakeCare. I've also set it up so the 'buffer' control can be manipulated to intentionally create horrible noises: at full regen, it'll even feed back at small buffer settings, and it's made to go into distortion a little below full scale in case you want to get real crunchy. It's also using an old Console algorithm internally, to expand the space inside the blast of noise.And then it turns out that if you keep it more calm, less regen, very small buffers and the depth not too extreme… it's a gorgeous chorus ensemble on something as revealing as a vintage Fender Rhodes. It generates stereo width, supplies a lovely pad of lushness around the instrument, and doesn't have to be weird at all. This could come in handy on things like synth pads, voices, who knows what? Think 'lush stereo chorus ensemble' and dial it in to whatever your needs are. The range on this is pretty nuts, because it was originally designed for radical uses but cleans up really well.I hope you like it :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class TakeCare final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    double a3AL[32767 + 5];
    double a3BL[32767 + 5];
    double a3CL[32767 + 5];
    double a3DL[32767 + 5];
    double a3EL[32767 + 5];
    double a3FL[32767 + 5];
    double a3GL[32767 + 5];
    double a3HL[32767 + 5];
    double a3IL[32767 + 5];
    double a3AR[32767 + 5];
    double a3BR[32767 + 5];
    double a3CR[32767 + 5];
    double a3DR[32767 + 5];
    double a3ER[32767 + 5];
    double a3FR[32767 + 5];
    double a3GR[32767 + 5];
    double a3HR[32767 + 5];
    double a3IR[32767 + 5];
    int c3AL, c3AR, c3BL, c3BR, c3CL, c3CR, c3DL, c3DR, c3EL, c3ER;
    int c3FL, c3FR, c3GL, c3GR, c3HL, c3HR, c3IL, c3IR;
    double f3AL, f3BL, f3CL, f3CR, f3FR, f3IR;
    double avg3L, avg3R;
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_InL,
        bez_InR,
        bez_UnInL,
        bez_UnInR,
        bez_SampL,
        bez_SampR,
        bez_AvgInSampL,
        bez_AvgInSampR,
        bez_AvgOutSampL,
        bez_AvgOutSampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bez[bez_total];
    double rotate;
    double oldfpd;
    int buf;
    double vibDepth;
    double derezA;
    double derezB;
    double outA;
    double outB;
    double wetA;
    double wetB;
    double lastSampleL;
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    TakeCare()
    {
        A = 0.15;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 1.0;
        H = 1.0;
        for (int x = 0; x < 32767 + 2; x++) {
            a3AL[x] = 0.0;
            a3AR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3BL[x] = 0.0;
            a3BR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3CL[x] = 0.0;
            a3CR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3DL[x] = 0.0;
            a3DR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3EL[x] = 0.0;
            a3ER[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3FL[x] = 0.0;
            a3FR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3GL[x] = 0.0;
            a3GR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3HL[x] = 0.0;
            a3HR[x] = 0.0;
        }
        for (int x = 0; x < 32767 + 2; x++) {
            a3IL[x] = 0.0;
            a3IR[x] = 0.0;
        }
        c3AL = c3BL = c3CL = c3DL = c3EL = c3FL = c3GL = c3HL = c3IL = 1;
        c3AR = c3BR = c3CR = c3DR = c3ER = c3FR = c3GR = c3HR = c3IR = 1;
        f3AL = f3BL = f3CL = 0.0;
        f3CR = f3FR = f3IR = 0.0;
        avg3L = avg3R = 0.0;
        for (int x = 0; x < bez_total; x++) {
            bez[x] = 0.0;
        }
        bez[bez_cycle] = 1.0;
        rotate = 0.0;
        oldfpd = 0.4294967295;
        buf = 8192;
        vibDepth = 0.0;
        outA = 1.0;
        outB = 1.0;
        wetA = 1.0;
        wetB = 1.0;
        derezA = 0.5;
        derezB = 0.5;
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 0.15; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 1.0; break;
            case kParamH: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "speed"; break;
            case kParamB: return "rando"; break;
            case kParamC: return "depth"; break;
            case kParamD: return "regen"; break;
            case kParamE: return "derez"; break;
            case kParamF: return "buffer"; break;
            case kParamG: return "output"; break;
            case kParamH: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Speed"; break;
            case kParamB: return "Rando"; break;
            case kParamC: return "Depth"; break;
            case kParamD: return "Regen"; break;
            case kParamE: return "Derez"; break;
            case kParamF: return "Buffer"; break;
            case kParamG: return "Output"; break;
            case kParamH: return "Dry/Wet"; break;

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
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;

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
            case kParamG: return ""; break;
            case kParamH: return ""; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
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
        double vibSpeed = pow(A, 5.0) * 0.1;
        double vibRandom = pow(B, 3.0) * 0.99;
        double targetDepth = pow(C, 2.0) * 0.5;
        double reg3n = D * 0.0625;
        derezA = derezB;
        derezB = E * 2.0;
        bool stepped = true; // Revised Bezier Undersampling
        if (derezB > 1.0) { // has full rez at center, stepped
            stepped = false; // to left, continuous to right
            derezB = 1.0 - (derezB - 1.0);
        } // if it's set up like that it's the revised algorithm
        derezB = fmin(fmax(derezB / overallscale, 0.0005), 1.0);
        int bezFraction = (int)(1.0 / derezB);
        double bezTrim = (double)bezFraction / (bezFraction + 1.0);
        if (stepped) { // this hard-locks derez to exact subdivisions of 1.0
            derezB = 1.0 / bezFraction;
            bezTrim = 1.0 - (derezB * bezTrim);
        }
        else { // this makes it match the 1.0 case using stepped
            derezB /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
            bezTrim = 1.0 - pow(derezB * 0.5, 1.0 / (derezB * 0.5));
        } // the revision more accurately connects the bezier curves
        int targetBuf = (pow(F, 3.0) * 32510.0) + 256;
        outA = outB;
        outB = G;
        wetA = wetB;
        wetB = 1.0 - pow(1.0 - H, 2.0);
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double derez = (derezA * temp) + (derezB * (1.0 - temp));
            double out = (outA * temp) + (outB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            bez[bez_cycle] += derez;
            bez[bez_SampL] += ((inputSampleL + bez[bez_InL]) * derez);
            bez[bez_SampR] += ((inputSampleR + bez[bez_InR]) * derez);
            bez[bez_InL] = inputSampleL;
            bez[bez_InR] = inputSampleR;
            if (bez[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                if (stepped) {
                    bez[bez_cycle] = 0.0;
                }
                else {
                    bez[bez_cycle] -= 1.0;
                }
                inputSampleL = (bez[bez_SampL] + bez[bez_AvgInSampL]) * 0.5;
                bez[bez_AvgInSampL] = bez[bez_SampL];
                inputSampleR = (bez[bez_SampR] + bez[bez_AvgInSampR]) * 0.5;
                bez[bez_AvgInSampR] = bez[bez_SampR];
                rotate += (oldfpd * vibSpeed);
                if (rotate > (M_PI * 2.0)) {
                    rotate -= (M_PI * 2.0);
                    oldfpd = (1.0 - vibRandom) + ((fpdL * 0.000000000618) * vibRandom);
                }
                vibDepth = (vibDepth * 0.99) + (targetDepth * 0.01);
                if (buf < targetBuf) {
                    buf++;
                }
                if (buf > targetBuf) {
                    buf--;
                }
                double mA = (sin(rotate) + 1.0) * vibDepth * buf;
                double mB = (sin(rotate + (M_PI / 9.0)) + 1.0) * vibDepth * buf;
                double mC = (sin(rotate + ((M_PI / 9.0) * 2.0)) + 1.0) * vibDepth * buf;
                double mD = (sin(rotate + ((M_PI / 9.0) * 3.0)) + 1.0) * vibDepth * buf;
                double mE = (sin(rotate + ((M_PI / 9.0) * 4.0)) + 1.0) * vibDepth * buf;
                double mF = (sin(rotate + ((M_PI / 9.0) * 5.0)) + 1.0) * vibDepth * buf;
                double mG = (sin(rotate + ((M_PI / 9.0) * 6.0)) + 1.0) * vibDepth * buf;
                double mH = (sin(rotate + ((M_PI / 9.0) * 7.0)) + 1.0) * vibDepth * buf;
                double mI = (sin(rotate + ((M_PI / 9.0) * 8.0)) + 1.0) * vibDepth * buf;
                inputSampleL = sin(fmin(fmax(inputSampleL * 0.5, -M_PI_2), M_PI_2));
                inputSampleR = sin(fmin(fmax(inputSampleR * 0.5, -M_PI_2), M_PI_2));
                a3AL[c3AL] = inputSampleL + fmin(fmax(f3AL * reg3n, -M_PI), M_PI);
                a3BL[c3BL] = inputSampleL + fmin(fmax(f3BL * reg3n, -M_PI), M_PI);
                a3CL[c3CL] = inputSampleL + fmin(fmax(f3CL * reg3n, -M_PI), M_PI);
                a3CR[c3CR] = inputSampleR + fmin(fmax(f3CR * reg3n, -M_PI), M_PI);
                a3FR[c3FR] = inputSampleR + fmin(fmax(f3FR * reg3n, -M_PI), M_PI);
                a3IR[c3IR] = inputSampleR + fmin(fmax(f3IR * reg3n, -M_PI), M_PI);
                c3AL++;
                if (c3AL < 0 || c3AL > buf) {
                    c3AL = 0;
                }
                c3BL++;
                if (c3BL < 0 || c3BL > buf) {
                    c3BL = 0;
                }
                c3CL++;
                if (c3CL < 0 || c3CL > buf) {
                    c3CL = 0;
                }
                c3CR++;
                if (c3CR < 0 || c3CR > buf) {
                    c3CR = 0;
                }
                c3FR++;
                if (c3FR < 0 || c3FR > buf) {
                    c3FR = 0;
                }
                c3IR++;
                if (c3IR < 0 || c3IR > buf) {
                    c3IR = 0;
                }
                double o3AL = a3AL[(int)(c3AL + mA - ((c3AL + mA > buf) ? buf + 1 : 0))] * (1.0 - (mA - floor(mA)));
                o3AL += (a3AL[(int)(c3AL + mA + 1 - ((c3AL + mA + 1 > buf) ? buf + 1 : 0))] * ((mA - floor(mA))));
                double o3BL = a3BL[(int)(c3BL + mB - ((c3BL + mB > buf) ? buf + 1 : 0))] * (1.0 - (mB - floor(mB)));
                o3BL += (a3BL[(int)(c3BL + mB + 1 - ((c3BL + mB + 1 > buf) ? buf + 1 : 0))] * ((mB - floor(mB))));
                double o3CL = a3CL[(int)(c3CL + mC - ((c3CL + mC > buf) ? buf + 1 : 0))] * (1.0 - (mC - floor(mC)));
                o3CL += (a3CL[(int)(c3CL + mC + 1 - ((c3CL + mC + 1 > buf) ? buf + 1 : 0))] * ((mC - floor(mC))));
                double o3CR = a3CR[(int)(c3CR + mC - ((c3CR + mC > buf) ? buf + 1 : 0))] * (1.0 - (mC - floor(mC)));
                o3CR += (a3CR[(int)(c3CR + mC + 1 - ((c3CR + mC + 1 > buf) ? buf + 1 : 0))] * ((mC - floor(mC))));
                double o3FR = a3FR[(int)(c3FR + mF - ((c3FR + mF > buf) ? buf + 1 : 0))] * (1.0 - (mF - floor(mF)));
                o3FR += (a3FR[(int)(c3FR + mF + 1 - ((c3FR + mF + 1 > buf) ? buf + 1 : 0))] * ((mF - floor(mF))));
                double o3IR = a3IR[(int)(c3IR + mI - ((c3IR + mI > buf) ? buf + 1 : 0))] * (1.0 - (mI - floor(mI)));
                o3IR += (a3IR[(int)(c3IR + mI + 1 - ((c3IR + mI + 1 > buf) ? buf + 1 : 0))] * ((mI - floor(mI))));
                a3DL[c3DL] = (((o3BL + o3CL) * -2.0) + o3AL);
                a3EL[c3EL] = (((o3AL + o3CL) * -2.0) + o3BL);
                a3FL[c3FL] = (((o3AL + o3BL) * -2.0) + o3CL);
                a3BR[c3BR] = (((o3FR + o3IR) * -2.0) + o3CR);
                a3ER[c3ER] = (((o3CR + o3IR) * -2.0) + o3FR);
                a3HR[c3HR] = (((o3CR + o3FR) * -2.0) + o3IR);
                c3DL++;
                if (c3DL < 0 || c3DL > buf) {
                    c3DL = 0;
                }
                c3EL++;
                if (c3EL < 0 || c3EL > buf) {
                    c3EL = 0;
                }
                c3FL++;
                if (c3FL < 0 || c3FL > buf) {
                    c3FL = 0;
                }
                c3BR++;
                if (c3BR < 0 || c3BR > buf) {
                    c3BR = 0;
                }
                c3ER++;
                if (c3ER < 0 || c3ER > buf) {
                    c3ER = 0;
                }
                c3HR++;
                if (c3HR < 0 || c3HR > buf) {
                    c3HR = 0;
                }
                double o3DL = a3DL[(int)(c3DL + mD - ((c3DL + mD > buf) ? buf + 1 : 0))] * (1.0 - (mD - floor(mD)));
                o3DL += (a3DL[(int)(c3DL + mD + 1 - ((c3DL + mD + 1 > buf) ? buf + 1 : 0))] * ((mD - floor(mD))));
                double o3EL = a3EL[(int)(c3EL + mE - ((c3EL + mE > buf) ? buf + 1 : 0))] * (1.0 - (mE - floor(mE)));
                o3EL += (a3EL[(int)(c3EL + mE + 1 - ((c3EL + mE + 1 > buf) ? buf + 1 : 0))] * ((mE - floor(mE))));
                double o3FL = a3FL[(int)(c3FL + mF - ((c3FL + mF > buf) ? buf + 1 : 0))] * (1.0 - (mF - floor(mF)));
                o3FL += (a3FL[(int)(c3FL + mF + 1 - ((c3FL + mF + 1 > buf) ? buf + 1 : 0))] * ((mF - floor(mF))));
                double o3BR = a3BR[(int)(c3BR + mB - ((c3BR + mB > buf) ? buf + 1 : 0))] * (1.0 - (mB - floor(mB)));
                o3BR += (a3BR[(int)(c3BR + mB + 1 - ((c3BR + mB + 1 > buf) ? buf + 1 : 0))] * ((mB - floor(mB))));
                double o3ER = a3ER[(int)(c3ER + mE - ((c3ER + mE > buf) ? buf + 1 : 0))] * (1.0 - (mE - floor(mE)));
                o3ER += (a3ER[(int)(c3ER + mE + 1 - ((c3ER + mE + 1 > buf) ? buf + 1 : 0))] * ((mE - floor(mE))));
                double o3HR = a3HR[(int)(c3HR + mH - ((c3HR + mH > buf) ? buf + 1 : 0))] * (1.0 - (mH - floor(mH)));
                o3HR += (a3HR[(int)(c3HR + mH + 1 - ((c3HR + mH + 1 > buf) ? buf + 1 : 0))] * ((mH - floor(mH))));
                a3GL[c3GL] = (((o3EL + o3FL) * -2.0) + o3DL);
                a3HL[c3HL] = (((o3DL + o3FL) * -2.0) + o3EL);
                a3IL[c3IL] = (((o3DL + o3EL) * -2.0) + o3FL);
                a3AR[c3AR] = (((o3ER + o3HR) * -2.0) + o3BR);
                a3DR[c3DR] = (((o3BR + o3HR) * -2.0) + o3ER);
                a3GR[c3GR] = (((o3BR + o3ER) * -2.0) + o3HR);
                c3GL++;
                if (c3GL < 0 || c3GL > buf) {
                    c3GL = 0;
                }
                c3HL++;
                if (c3HL < 0 || c3HL > buf) {
                    c3HL = 0;
                }
                c3IL++;
                if (c3IL < 0 || c3IL > buf) {
                    c3IL = 0;
                }
                c3AR++;
                if (c3AR < 0 || c3AR > buf) {
                    c3AR = 0;
                }
                c3DR++;
                if (c3DR < 0 || c3DR > buf) {
                    c3DR = 0;
                }
                c3GR++;
                if (c3GR < 0 || c3GR > buf) {
                    c3GR = 0;
                }
                double o3GL = a3GL[(int)(c3GL + mG - ((c3GL + mG > buf) ? buf + 1 : 0))] * (1.0 - (mG - floor(mG)));
                o3GL += (a3GL[(int)(c3GL + mG + 1 - ((c3GL + mG + 1 > buf) ? buf + 1 : 0))] * ((mG - floor(mG))));
                double o3HL = a3HL[(int)(c3HL + mH - ((c3HL + mH > buf) ? buf + 1 : 0))] * (1.0 - (mH - floor(mH)));
                o3HL += (a3HL[(int)(c3HL + mH + 1 - ((c3HL + mH + 1 > buf) ? buf + 1 : 0))] * ((mH - floor(mH))));
                double o3IL = a3IL[(int)(c3IL + mI - ((c3IL + mI > buf) ? buf + 1 : 0))] * (1.0 - (mI - floor(mI)));
                o3IL += (a3IL[(int)(c3IL + mI + 1 - ((c3IL + mI + 1 > buf) ? buf + 1 : 0))] * ((mI - floor(mI))));
                double o3AR = a3AR[(int)(c3AR + mA - ((c3AR + mA > buf) ? buf + 1 : 0))] * (1.0 - (mA - floor(mA)));
                o3AR += (a3AR[(int)(c3AR + mA + 1 - ((c3AR + mA + 1 > buf) ? buf + 1 : 0))] * ((mA - floor(mA))));
                double o3DR = a3DR[(int)(c3DR + mD - ((c3DR + mD > buf) ? buf + 1 : 0))] * (1.0 - (mD - floor(mD)));
                o3DR += (a3DR[(int)(c3DR + mD + 1 - ((c3DR + mD + 1 > buf) ? buf + 1 : 0))] * ((mD - floor(mD))));
                double o3GR = a3GR[(int)(c3GR + mG - ((c3GR + mG > buf) ? buf + 1 : 0))] * (1.0 - (mG - floor(mG)));
                o3GR += (a3GR[(int)(c3GR + mG + 1 - ((c3GR + mG + 1 > buf) ? buf + 1 : 0))] * ((mG - floor(mG))));
                f3AL = (((o3GR + o3HR) * -2.0) + o3IR);
                f3BL = (((o3GR + o3HR) * -2.0) + o3IR);
                f3CL = (((o3GR + o3HR) * -2.0) + o3IR);
                f3CR = (((o3AL + o3DL) * -2.0) + o3GL);
                f3FR = (((o3AL + o3DL) * -2.0) + o3GL);
                f3IR = (((o3AL + o3DL) * -2.0) + o3GL);
                double inputSampleL = (o3GL + o3HL + o3IL) * 0.03125;
                double inputSampleR = (o3AR + o3DR + o3GR) * 0.03125;
                f3AL = (f3AL + f3AL + f3AL + fabs(avg3L)) * 0.25;
                avg3L = f3AL;
                f3CR = (f3CR + f3CR + f3CR + fabs(avg3R)) * 0.25;
                avg3R = f3CR;
                // manipulating deep reverb tail for realism
                // begin ClipOnly stereo as a little, compressed chunk that can be dropped into undersampled code
                inputSampleL = fmin(fmax(inputSampleL, -4.0), 4.0);
                if (wasPosClipL == true) { // current will be over
                    if (inputSampleL < lastSampleL) {
                        lastSampleL = 0.79 + (inputSampleL * 0.2);
                    }
                    else {
                        lastSampleL = 0.2 + (lastSampleL * 0.79);
                    }
                }
                wasPosClipL = false;
                if (inputSampleL > 0.99) {
                    wasPosClipL = true;
                    inputSampleL = 0.79 + (lastSampleL * 0.2);
                }
                if (wasNegClipL == true) { // current will be -over
                    if (inputSampleL > lastSampleL) {
                        lastSampleL = -0.79 + (inputSampleL * 0.2);
                    }
                    else {
                        lastSampleL = -0.2 + (lastSampleL * 0.79);
                    }
                }
                wasNegClipL = false;
                if (inputSampleL < -0.99) {
                    wasNegClipL = true;
                    inputSampleL = -0.79 + (lastSampleL * 0.2);
                }
                lastSampleL = inputSampleL;
                inputSampleR = fmin(fmax(inputSampleR, -4.0), 4.0);
                if (wasPosClipR == true) { // current will be over
                    if (inputSampleR < lastSampleR) {
                        lastSampleR = 0.79 + (inputSampleR * 0.2);
                    }
                    else {
                        lastSampleR = 0.2 + (lastSampleR * 0.79);
                    }
                }
                wasPosClipR = false;
                if (inputSampleR > 0.99) {
                    wasPosClipR = true;
                    inputSampleR = 0.79 + (lastSampleR * 0.2);
                }
                if (wasNegClipR == true) { // current will be -over
                    if (inputSampleR > lastSampleR) {
                        lastSampleR = -0.79 + (inputSampleR * 0.2);
                    }
                    else {
                        lastSampleR = -0.2 + (lastSampleR * 0.79);
                    }
                }
                wasNegClipR = false;
                if (inputSampleR < -0.99) {
                    wasNegClipR = true;
                    inputSampleR = -0.79 + (lastSampleR * 0.2);
                }
                lastSampleR = inputSampleR;
                // end ClipOnly stereo as a little, compressed chunk that can be dropped into undersampled code
                inputSampleL = asin(inputSampleL * 0.7);
                inputSampleR = asin(inputSampleR * 0.7);
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
            double CBAL = (bez[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * -0.25;
            double CBAR = (bez[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * -0.25;
            inputSampleL = CBAL + bez[bez_AvgOutSampL];
            bez[bez_AvgOutSampL] = CBAL;
            inputSampleR = CBAR + bez[bez_AvgOutSampR];
            bez[bez_AvgOutSampR] = CBAR;
            if (out != 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::takecare
