#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::chimeyguitar2 {

constexpr std::string_view k_name{ "ChimeyGuitar2" };
constexpr std::string_view k_short_description{
    "ChimeyGuitar2 is a wilder ChimeyGuitar."
};
constexpr std::string_view k_long_description{
    "This is the Latest chimeyguitar plugin, but it is not going to be in the Basic or Recommended categories, though in my heart it is definitely Recommended. Why? Because it's borderline dangerous. The amp can pop and explode when set incorrectly.Still here? OK, let's talk what's different this time. The original ChimeyGuitar used many instances of BeziComp to compress, which was mostly good at clamping down and compressing the signal. If you overcompress, you get an unpleasant warbling, and it didn't really react like traditional compressors. It's pretty safe and 'normal' even while not being normal. It didn't work great as ChimeyDeluxe, so I put a lot of work into developing a more intense version to use in ChimeyDeluxe, and that's what came out.An important difference is that ChimeyDeluxe is ten EQ-like controls with the compression built in. No extra boost, no dedicated compression control, no cab filters (I use the ones in ConsoleX2), so it's inherently restricted. ChimeyDeluxe can go wild, but that's when you're cranking the sliders around and making wild variations between adjacent sliders. It doesn't have… surprises.ChimeyGuitar2 is the raw material of that with the cab filter and an unrestricted compression-speed control and an even more unrestricted boost control… so if you wanted to turn specifically ChimeyDeluxe into a monster, here you go.Watch out for the boost: it will go up to total compressed squish, but if you push beyond that it can snap into a 'blown output stage' mode that kinda barks extra loud on the loudest notes. The filter section is much the same as ChimeyGuitar, as is the cab filtering. But, remember the gain staging of this plugin includes any gain boosts inside the filtering, It should behave any time the combined gains don't add up to infinite, but they multiply by each other so this is NOT a plugin for cranking things out carelessly. Think of it like a Dumble tube amp: go carefully and you won't hurt yourself, but it can scare you if you're careless.The compression speed is the key to a lot of interestingly messy tones. It's a different compression than the version in ChimeyGuitar, which just turns down and has that funny warbling effect when pushed. This one doesn't warble like that, but it includes makeup gain so infinite sustain is very possible, and if you do that with a fast (to the left) compression speed, you get a distinct distortion type, an edge on the tone that's very distortionlike. Pulling gain back or slowing (to the right) the compression speed moderates this effect, and slowing the speed a LOT is what you need to do to really avoid distorty qualities. That, and back off on the boost slider up top.But what fun is that? So, if you're going to embrace the wildness, dial in the gain with the speed set faster. Remember the EQ bands multiply with this. Once you're in the zone, tweak the compression speed to shape the voicing of the distorty stuff, and if gain is high enough and you're pushing the system hard enough, you can make the 'bark' touch responsive with very tiny adjustments right around the infinite compression point, right where it starts to bark and spark.Or you can always use a normal amp sim if this seems like too much trouble. A normal person wouldn't deal with this nonsense. I need to spend more time playing with it, because when I got reacquainted with ChimeyGuitar2 I immediately wanted to dial in all the twitchiest settings and play leads through it :)"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class ChimeyGuitar2 final : public Effect<T>
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
    double angSL[18][12];
    double angAL[18][12];
    double iirHPositionL[37];
    double iirHAngleL[37];
    double iirBPositionL[37];
    double iirBAngleL[37];
    double muCompL;
    double muSpdL;
    double angSR[18][12];
    double angAR[18][12];
    double iirHPositionR[37];
    double iirHAngleR[37];
    double iirBPositionR[37];
    double iirBAngleR[37];
    double muCompR;
    double muSpdR;
    double angG[12];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ChimeyGuitar2()
    {
        A = 0.6;
        B = 0.8;
        C = 0.7;
        D = 0.5;
        E = 0.7;
        F = 0.4;
        G = 0.8;
        H = 0.4;
        I = 0.8;
        J = 0.8;
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
        muCompL = 1.0;
        muSpdL = 100.0;
        muCompR = 1.0;
        muSpdR = 100.0;
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
            case kParamA: return 0.6; break;
            case kParamB: return 0.8; break;
            case kParamC: return 0.7; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.7; break;
            case kParamF: return 0.4; break;
            case kParamG: return 0.8; break;
            case kParamH: return 0.4; break;
            case kParamI: return 0.8; break;
            case kParamJ: return 0.8; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        int poles = (int)(pow(A, 3.0) * 20.0);
        double threshold = 1.0 - pow(A * 0.9, 2.0);
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
        double adjSpd = ((I * 180.0) + 20.0) * overallscale;
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
                inputSampleL *= (muCompL / threshold);
                if (fabs(inputSampleL) > threshold) {
                    muCompL *= muSpdL;
                    if (threshold / fabs(inputSampleL) < threshold) {
                        muCompL += threshold * fabs(inputSampleL);
                    }
                    else {
                        muCompL -= threshold / fabs(inputSampleL);
                    }
                    muCompL /= muSpdL;
                }
                else {
                    muCompL *= (muSpdL * muSpdL);
                    muCompL += ((1.1 + threshold) - fabs(inputSampleL));
                    muCompL /= (muSpdL * muSpdL);
                }
                muCompL = fmax(fmin(muCompL, 1.0), threshold);
                inputSampleL *= (muCompL * muCompL);
                muSpdL = fmax(fmin(((muSpdL * (muSpdL - 1.0)) + (fabs(inputSampleL * adjSpd))) / muSpdL, adjSpd * 2.0), adjSpd);
            }
            inputSampleL = sin(fmin(fmax(inputSampleL, -M_PI_2), M_PI_2));
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
                inputSampleR *= (muCompR / threshold);
                if (fabs(inputSampleR) > threshold) {
                    muCompR *= muSpdR;
                    if (threshold / fabs(inputSampleR) < threshold) {
                        muCompR += threshold * fabs(inputSampleR);
                    }
                    else {
                        muCompR -= threshold / fabs(inputSampleR);
                    }
                    muCompR /= muSpdR;
                }
                else {
                    muCompR *= (muSpdR * muSpdR);
                    muCompR += ((1.1 + threshold) - fabs(inputSampleR));
                    muCompR /= (muSpdR * muSpdR);
                }
                muCompR = fmax(fmin(muCompR, 1.0), threshold);
                inputSampleR *= (muCompR * muCompR);
                muSpdR = fmax(fmin(((muSpdR * (muSpdR - 1.0)) + (fabs(inputSampleR * adjSpd))) / muSpdR, adjSpd * 2.0), adjSpd);
            }
            inputSampleR = sin(fmin(fmax(inputSampleR, -M_PI_2), M_PI_2));
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
} // namespace airwindohhs::chimeyguitar2
