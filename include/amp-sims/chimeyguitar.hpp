#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::chimeyguitar {

constexpr std::string_view k_name{ "ChimeyGuitar" };
constexpr std::string_view k_short_description{
    "ChimeyGuitar is a supremely compressible instrument amp."
};
constexpr std::string_view k_long_description{
    "Here's a followup to PointyGuitar. What if, instead of distort, the imaginary amp compressed?The tone stack works the same: in fact, it's exactly as it was in PointyGuitar, as is the cab simulation (a highpass and lowpass made out of AngleEQ, which is able to be very resonant and colorful). That's on purpose. I want it to be familiar, so the way the tone shaping works is exactly like PointyGuitar, and if you can dial one in, you can dial the other.But instead of the basic distortion as found in 'FireAmp', what's there? BeziComp. Not even a normal compressor, no, it's the new experimental one that turns the amplification factor into a Bezier curve… but BeziComp has one instance of itself in play. ChimeyGuitar?ChimeyGuitar has eight, at full crank.Stacking compressors like this isn't unheard of: the FMR Really Nice Compressor has 'Super Nice' mode, which cascades three compressors in series. It's just that ChimeyGuitar does eight, of a new design which I don't think existed until I started it. That also means it's on me to sort out what the strengths and weaknesses of this new kind of compression are, seeing as we have as many as eight of them in between every EQ stage now.First, it can react very quickly, but refuses to alias since it will not apply a volume 'corner' above its minimum radius. This matters less used inside ChimeyGuitar because plugins like this repeatedly filter out aliasing harmonics anyhow, but it's significant because usually as you have compression kick in you hear continued interaction between the sound, and the speed of attack. That lets you dial in a squished, unvarying sound by hearing that overtone as if it was a form of distortion (which it is), but BeziComp and ChimeyGuitar don't have any of that compression artifact at all.Instead, you'll hear an odd warble when you push ChimeyGuitar too far. It's similar to when you're using DeRez3: while Bezier curves can sound like a brickwall filter, there's an strange resonance associated with it. In a compressor, when pushed hard, we hear this as tremblings of the loudness, as if trying to squish the signal makes it more jittery. It can come off like an old Arp Pro Soloist trying to imitate a trombone, but the thing to bear in mind is that you can always back off the Compres control until it cleans up again. The transparency of BeziComp means it'll clean up a whole bunch while still being compressed.This is my go-to for articulate guitars and basses that don't seem to have saturation or distortion. It's got the flexibility of PointyGuitar, but super clean, or with strange new forms of saturation that are like derezzing. Hope you like it!"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class ChimeyGuitar final : public Effect<T>
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
    double angSL[22][12];
    double angAL[22][12];
    double iirHPositionL[37];
    double iirHAngleL[37];
    double iirBPositionL[37];
    double iirBAngleL[37];
    double angSR[22][12];
    double angAR[22][12];
    double iirHPositionR[37];
    double iirHAngleR[37];
    double iirBPositionR[37];
    double iirBAngleR[37];
    double angG[12];
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_cycleL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_InR,
        bez_UnInR,
        bez_SampR,
        bez_cycleR,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total][12];
    double bezRezA, bezRezB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ChimeyGuitar()
    {
        A = 0.5;
        B = 0.8;
        C = 0.7;
        D = 0.5;
        E = 0.7;
        F = 0.4;
        G = 0.8;
        H = 0.4;
        I = 0.3;
        J = 1.0;
        for (int x = 0; x < 21; x++) {
            for (int y = 0; y < 11; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 11; y++) {
            angG[y] = 0.0;
            for (int x = 0; x < bez_total; x++) {
                bezComp[x][y] = 0.0;
            }
            bezComp[bez_cycleL][y] = 1.0;
            bezComp[bez_cycleR][y] = 1.0;
        }
        bezRezA = bezRezB = 0.0002;
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
            case kParamB: return 0.8; break;
            case kParamC: return 0.7; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.7; break;
            case kParamF: return 0.4; break;
            case kParamG: return 0.8; break;
            case kParamH: return 0.4; break;
            case kParamI: return 0.3; break;
            case kParamJ: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "compres"; break;
            case kParamB: return "presnce"; break;
            case kParamC: return "high"; break;
            case kParamD: return "mid"; break;
            case kParamE: return "low"; break;
            case kParamF: return "sub"; break;
            case kParamG: return "hspeakr"; break;
            case kParamH: return "lspeakr"; break;
            case kParamI: return "speed"; break;
            case kParamJ: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Compres"; break;
            case kParamB: return "Presnce"; break;
            case kParamC: return "High"; break;
            case kParamD: return "Mid"; break;
            case kParamE: return "Low"; break;
            case kParamF: return "Sub"; break;
            case kParamG: return "HSpeakr"; break;
            case kParamH: return "LSpeakr"; break;
            case kParamI: return "Speed"; break;
            case kParamJ: return "Output"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        int poles = (int)(pow(A, 2) * 20.0);
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
        double hFreq = pow(G, overallscale);
        double lFreq = pow(H, overallscale + 3.0);
        bezRezA = bezRezB;
        bezRezB = ((pow(I, 4.0) * 0.5) + 0.0002) / overallscale;
        if (bezRezB > 1.0) {
            bezRezB = 1.0;
        }
        double output = J;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double temp = (double)sampleFrames / inFramesToProcess;
            double bezRez = (bezRezA * temp) + (bezRezB * (1.0 - temp));
            double CBAL = 0.0;
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
                    bezComp[bez_cycleL][y] += bezRez;
                    bezComp[bez_SampL][y] += (fabs(inputSampleL) * bezRez);
                    if (bezComp[bez_cycleL][y] > 1.0) {
                        bezComp[bez_cycleL][y] -= 1.0;
                        bezComp[bez_CL][y] = bezComp[bez_BL][y];
                        bezComp[bez_BL][y] = bezComp[bez_AL][y];
                        bezComp[bez_AL][y] = bezComp[bez_SampL][y];
                        bezComp[bez_SampL][y] = 0.0;
                    }
                    double CBL = (bezComp[bez_CL][y] * (1.0 - bezComp[bez_cycleL][y])) + (bezComp[bez_BL][y] * bezComp[bez_cycleL][y]);
                    double BAL = (bezComp[bez_BL][y] * (1.0 - bezComp[bez_cycleL][y])) + (bezComp[bez_AL][y] * bezComp[bez_cycleL][y]);
                    CBAL += (bezComp[bez_BL][y] + (CBL * (1.0 - bezComp[bez_cycleL][y])) + (BAL * bezComp[bez_cycleL][y])) * 0.5;
                    inputSampleL *= 1.0 - (fmin(CBAL * 0.01, 1.0));
                }
                inputSampleL += (band * angG[9]);
            }
            inputSampleL = sin(inputSampleL);
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
            double CBAR = 0.0;
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
                    bezComp[bez_cycleR][y] += bezRez;
                    bezComp[bez_SampR][y] += (fabs(inputSampleR) * bezRez);
                    if (bezComp[bez_cycleR][y] > 1.0) {
                        bezComp[bez_cycleR][y] -= 1.0;
                        bezComp[bez_CR][y] = bezComp[bez_BR][y];
                        bezComp[bez_BR][y] = bezComp[bez_AR][y];
                        bezComp[bez_AR][y] = bezComp[bez_SampR][y];
                        bezComp[bez_SampR][y] = 0.0;
                    }
                    double CBR = (bezComp[bez_CR][y] * (1.0 - bezComp[bez_cycleR][y])) + (bezComp[bez_BR][y] * bezComp[bez_cycleR][y]);
                    double BAR = (bezComp[bez_BR][y] * (1.0 - bezComp[bez_cycleR][y])) + (bezComp[bez_AR][y] * bezComp[bez_cycleR][y]);
                    CBAR += (bezComp[bez_BR][y] + (CBR * (1.0 - bezComp[bez_cycleR][y])) + (BAR * bezComp[bez_cycleR][y])) * 0.5;
                    inputSampleR *= 1.0 - (fmin(CBAR * 0.01, 1.0));
                }
                inputSampleR += (band * angG[9]);
            }
            inputSampleR = sin(inputSampleR);
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
} // namespace airwindohhs::chimeyguitar
