#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::chimeydeluxe {

constexpr std::string_view k_name{ "ChimeyDeluxe" };
constexpr std::string_view k_short_description{
    "ChimeyDeluxe is a very flexible compressed DI conditioner."
};
constexpr std::string_view k_long_description{
    "I've been working on this one for a while. It's done! ChimeyDeluxe is based on the same idea behind PointyDeluxe: more finely grained control over a bank of monster EQ bands, with processing stages between all the EQ stages that make the EQ so intense. None of it is normal but all of it is brutal, and PointyDeluxe found favor with industrial metal heads. To make it sound even slightly nice you have to follow it with either Cabs2, or an impulse response if you favor those, or perhaps the highpass and lowpass in ConsoleX2 or ConsoleH (the latter is still on track to be done before 2026).Or you can just run it raw and demolish your mix.I had to completely redesign the compression from ChimeyGuitar and it went through multiple revisions, ending up with 10 bands of EQ and sixteen (!) stages of EQ and compression. For a while there it ran compression from the Pressure algorithm, and then it went back to an upgraded BeziComp. You're met with 10 bands of EQ, labeled like they are in PointyDeluxe (the Slipperman Distorted Guitar nomenclature for frequency ranges) and nothing else. The compression responds to both the largest boost and the deepest cut.Much like with PointyDeluxe, this plugin does not protect you from yourself. Be gentle, or enjoy intensely gnarly resonant overloads. Why was this so important, if it's just a strange noise maker delivering pungent resonances?Because my target was to make the ultimate bass DI plugin, that could compress rather than distort, but also fill up the whole mix with dynamic bass intent. It needed to make notes just pop right out of the mix, and by the time I was done with it, it did. You can sculpt the string attack any way you like by how hard you drive it, and zero in on where in the mix you want this energy delivered. It's the perfect Ted Templeman Trick plugin: you can just zero in on a hole in the kick drum EQ and supply not only frequency, but dynamics and bounciness.It should also work just fine on guitars, keyboards, or whatever. Remember it's trying to deliver amp-like voicings. I got it to make an absolutely brutal close-miked kick sound out of room mics just by obliterating the sound just right. It'll always apply some compression, it's just a matter of how hard you push it with its own EQ on top of that. And again, it doesn't apply bandpassing or speaker emulation at all, so any sound out of ChimeyDeluxe will eat up a lot of mix space unless you do something to stop that.Or, you can just let it push your mix around and make things huge: up to you!"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class ChimeyDeluxe final : public Effect<T>
{
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
    double angSL[18][15];
    double angAL[18][15];
    double angSR[18][15];
    double angAR[18][15];
    double angG[15];
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_InR,
        bez_UnInR,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total][18];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ChimeyDeluxe()
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
            for (int w = 0; w < bez_total; w++) {
                bezComp[w][x] = 0.0;
            }
            bezComp[bez_cycle][x] = 1.0;
            for (int y = 0; y < 14; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 14; y++) {
            angG[y] = 0.0;
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
        switch (static_cast<params>(index)) {
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
            case kParamA: return "hell"; break;
            case kParamB: return "fizz"; break;
            case kParamC: return "pick"; break;
            case kParamD: return "satan"; break;
            case kParamE: return "danger"; break;
            case kParamF: return "crtical"; break;
            case kParamG: return "h meat"; break;
            case kParamH: return "l meat"; break;
            case kParamI: return "swing"; break;
            case kParamJ: return "rarely"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;
            case kParamI: return std::to_string(I); break;
            case kParamJ: return std::to_string(J); break;

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
            case kParamI: return ""; break;
            case kParamJ: return ""; break;

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
        double drive = 1.0;
        double pad = 1.0;
        angG[1] = (A + 1.5) * 0.5;
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
        angG[2] = (B + 1.5) * 0.5;
        if (pad > angG[2]) {
            pad = angG[2];
        }
        if (drive < angG[2]) {
            drive = angG[2];
        }
        angG[3] = (C + 1.5) * 0.5;
        if (pad > angG[3]) {
            pad = angG[3];
        }
        if (drive < angG[3]) {
            drive = angG[3];
        }
        angG[4] = (D + 1.5) * 0.5;
        if (pad > angG[4]) {
            pad = angG[4];
        }
        if (drive < angG[4]) {
            drive = angG[4];
        }
        angG[5] = (E + 1.5) * 0.5;
        if (pad > angG[5]) {
            pad = angG[5];
        }
        if (drive < angG[5]) {
            drive = angG[5];
        }
        angG[6] = (F + 1.5) * 0.5;
        if (pad > angG[6]) {
            pad = angG[6];
        }
        if (drive < angG[6]) {
            drive = angG[6];
        }
        angG[7] = (G + 1.5) * 0.5;
        if (pad > angG[7]) {
            pad = angG[7];
        }
        if (drive < angG[7]) {
            drive = angG[7];
        }
        angG[8] = (H + 1.5) * 0.5;
        if (pad > angG[8]) {
            pad = angG[8];
        }
        if (drive < angG[8]) {
            drive = angG[8];
        }
        angG[9] = (I + 1.5) * 0.5;
        if (pad > angG[9]) {
            pad = angG[9];
        }
        if (drive < angG[9]) {
            drive = angG[9];
        }
        angG[10] = (J + 1.5) * 0.5;
        if (pad > angG[10]) {
            pad = angG[10];
        }
        if (drive < angG[10]) {
            drive = angG[10];
        }
        angG[12] = angG[11] = angG[10];
        if (drive > 1.0) {
            drive = pow(drive, drive * 2.0);
        }
        double tune = 0.618 + (overallscale * 0.0055);
        double bezRez = (pad * drive * 0.0005) / overallscale;
        int bezFreqFraction = (int)(1.0 / bezRez);
        double bezFreqTrim = (double)bezFreqFraction / (bezFreqFraction + 1.0);
        bezRez = 1.0 / bezFreqFraction;
        bezFreqTrim = 1.0 - (bezRez * bezFreqTrim);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            for (int x = 0; x < 16; x++) {
                double fr = (0.92 / overallscale) + (overallscale * 0.01);
                double bandL = inputSampleL;
                inputSampleL = 0.0;
                double bandR = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 12; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((bandL - angSL[x][y]) * fr);
                    double tempL = bandL;
                    bandL = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (bandL * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (bandL * fr);
                    inputSampleL += ((tempL - bandL) * angG[y]);
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((bandR - angSR[x][y]) * fr);
                    double tempR = bandR;
                    bandR = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (bandR * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (bandR * fr);
                    inputSampleR += ((tempR - bandR) * angG[y]);
                    fr *= tune;
                }
                inputSampleL += bandL;
                inputSampleR += bandR; // end of filter part
                bezComp[bez_cycle][x] += bezRez;
                bezComp[bez_SampL][x] += (fabs(inputSampleL) * bezRez);
                bezComp[bez_SampR][x] += (fabs(inputSampleR) * bezRez);
                if (bezComp[bez_cycle][x] > 1.0) {
                    bezComp[bez_cycle][x] = 0.0;
                    bezComp[bez_CL][x] = bezComp[bez_BL][x];
                    bezComp[bez_BL][x] = bezComp[bez_AL][x];
                    bezComp[bez_AL][x] = bezComp[bez_SampL][x];
                    bezComp[bez_SampL][x] = 0.0;
                    bezComp[bez_CR][x] = bezComp[bez_BR][x];
                    bezComp[bez_BR][x] = bezComp[bez_AR][x];
                    bezComp[bez_AR][x] = bezComp[bez_SampR][x];
                    bezComp[bez_SampR][x] = 0.0;
                }
                double z = bezComp[bez_cycle][x] * bezFreqTrim;
                double CBL = (bezComp[bez_CL][x] * (1.0 - z)) + (bezComp[bez_BL][x] * z);
                double BAL = (bezComp[bez_BL][x] * (1.0 - z)) + (bezComp[bez_AL][x] * z);
                double CBAL = (bezComp[bez_BL][x] + (CBL * (1.0 - z)) + (BAL * z));
                double CBR = (bezComp[bez_CR][x] * (1.0 - z)) + (bezComp[bez_BR][x] * z);
                double BAR = (bezComp[bez_BR][x] * (1.0 - z)) + (bezComp[bez_AR][x] * z);
                double CBAR = (bezComp[bez_BR][x] + (CBR * (1.0 - z)) + (BAR * z));
                CBAL = fmin(fmax(CBAL * drive * 0.23, 0.0), M_PI_2);
                CBAR = fmin(fmax(CBAR * drive * 0.23, 0.0), M_PI_2);
                inputSampleL *= 1.0 - sin(CBAL);
                inputSampleR *= 1.0 - sin(CBAR);
            }
            inputSampleL /= drive;
            inputSampleR /= drive;
            inputSampleL = fmin(fmax(inputSampleL * pad, -1.0), 1.0);
            inputSampleR = fmin(fmax(inputSampleR * pad, -1.0), 1.0);
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
} // namespace airwindohhs::chimeydeluxe
