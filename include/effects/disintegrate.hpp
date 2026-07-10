#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::disintegrate {

constexpr std::string_view k_name{ "Disintegrate" };
constexpr std::string_view k_short_description{
    "Disintegrate is Discontinuity on steroids."
};
constexpr std::string_view k_long_description{
    "So what if you had a distortion that wasn't actually about distortion?Disintegrate extends Discontinuity to the point of total unreality.This algorithm was invented to emulate the asymmetrical compression of air, and already exists in Discontinuity and ConsoleX. You set it according to what at the point of clipping, would be the physical loudness in air, and it applies a delay modulation that distorts the sound. It's meant to reproduce what happens when a huge rocket launch, heard from miles away, turns into strange air crackles (a natural phenomenon having to do with pressure discontinuities in the speed of sound). This happens in all sound transmitted through air, all of the time, and Discontinuity scales it appropriately.Thing is, Discontinuity is designed to sound nice and not too crazy. So… why not go for broke?All the controls on Disintegrate are things that are under the hood on Discontinuity. It's designed to let you get inappropriate noises, unnatural sounds. Typically it sounds like distortions, but it's not: it's a stack of modulating delay lines, but rather than modulating by an LFO, the sound's modulated by itself, over and over. When you get into distortion-like edge, that's mostly the frequencies stacking up, though it does have the ability to overdrive internally. In particular, Disintegrate takes pains to overdrive internally, the better to sonically obliterate and destroy.Hope you like Disintegrate! I'll work on more realistic things soon! :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Disintegrate final : public Effect<T>
{
    static constexpr int dscBufMax = 180;
    static constexpr int layersMax = 22;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'disi';
    float A;
    float B;
    float C;
    float D;
    float E;
    double dBaL[dscBufMax + 5][layersMax];
    double dBaPosL[layersMax];
    double dBaPosBL[layersMax];
    int dBaXL[layersMax];
    double outFilterL;
    double dBaR[dscBufMax + 5][layersMax];
    double dBaPosR[layersMax];
    double dBaPosBR[layersMax];
    int dBaXR[layersMax];
    double outFilterR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Disintegrate()
    {
        A = 0.5;
        B = 0.5;
        C = 0.2;
        D = 0.5;
        E = 1.0;
        for (int stage = 0; stage < layersMax; stage++) {
            for (int count = 0; count < dscBufMax + 2; count++) {
                dBaL[count][stage] = 0.0;
                dBaR[count][stage] = 0.0;
            }
            dBaPosL[stage] = 0.0;
            dBaPosBL[stage] = 0.0;
            dBaXL[stage] = 1;
            dBaPosR[stage] = 0.0;
            dBaPosBR[stage] = 0.0;
            dBaXR[stage] = 1;
        }
        outFilterL = 0.0;
        outFilterR = 0.0;
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
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.2; break;
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "top db"; break;
            case kParamB: return "bufsize"; break;
            case kParamC: return "layers"; break;
            case kParamD: return "filter"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Top dB"; break;
            case kParamB: return "BufSize"; break;
            case kParamC: return "Layers"; break;
            case kParamD: return "Filter"; break;
            case kParamE: return "Dry/Wet"; break;

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
        double refdB = 60.0 + (A * 80.0);
        double topdB = 0.000000064 * pow(10.0, refdB / 20.0) * overallscale;
        int dscBuf = (B * (double)(dscBufMax - 1)) + 1;
        int layers = (C * 20.0);
        double f = pow(D, 2);
        double boost = 1.0 + (f / (layers + 1));
        if (f == 0.0) {
            f = 0.000001;
        }
        double wet = E;
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
            inputSampleL *= topdB;
            inputSampleR *= topdB;
            for (int x = 0; x < layers; x++) {
                inputSampleL *= boost;
                if (inputSampleL < -0.222) {
                    inputSampleL = -0.222;
                }
                if (inputSampleL > 0.222) {
                    inputSampleL = 0.222;
                }
                dBaL[dBaXL[x]][x] = inputSampleL;
                dBaPosL[x] *= (1.0 - f);
                dBaPosL[x] += (dBaPosBL[x] * f);
                dBaPosBL[x] *= (1.0 - f);
                dBaPosBL[x] += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * f);
                int dBdly = floor(dBaPosL[x] * dscBuf);
                double dBi = (dBaPosL[x] * dscBuf) - dBdly;
                inputSampleL = dBaL[dBaXL[x] - dBdly + ((dBaXL[x] - dBdly < 0) ? dscBuf : 0)][x] * (1.0 - dBi);
                dBdly++;
                inputSampleL += dBaL[dBaXL[x] - dBdly + ((dBaXL[x] - dBdly < 0) ? dscBuf : 0)][x] * dBi;
                dBaXL[x]++;
                if (dBaXL[x] < 0 || dBaXL[x] >= dscBuf) {
                    dBaXL[x] = 0;
                }
            }
            for (int x = 0; x < layers; x++) {
                inputSampleR *= boost;
                if (inputSampleR < -0.222) {
                    inputSampleR = -0.222;
                }
                if (inputSampleR > 0.222) {
                    inputSampleR = 0.222;
                }
                dBaR[dBaXR[x]][x] = inputSampleR;
                dBaPosR[x] *= (1.0 - f);
                dBaPosR[x] += (dBaPosBR[x] * f);
                dBaPosBR[x] *= (1.0 - f);
                dBaPosBR[x] += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * f);
                int dBdly = floor(dBaPosR[x] * dscBuf);
                double dBi = (dBaPosR[x] * dscBuf) - dBdly;
                inputSampleR = dBaR[dBaXR[x] - dBdly + ((dBaXR[x] - dBdly < 0) ? dscBuf : 0)][x] * (1.0 - dBi);
                dBdly++;
                inputSampleR += dBaR[dBaXR[x] - dBdly + ((dBaXR[x] - dBdly < 0) ? dscBuf : 0)][x] * dBi;
                dBaXR[x]++;
                if (dBaXR[x] < 0 || dBaXR[x] >= dscBuf) {
                    dBaXR[x] = 0;
                }
            } // This is being done this way, rather than all together in one loop, because
            // the hope is that all this repetitive processing on a small group of variables
            // can be more easily cached and optimized if we don't act like they must be done together.
            outFilterL *= f;
            outFilterL += (inputSampleL * (1.0 - f));
            inputSampleL = outFilterL;
            inputSampleL /= topdB;
            inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
            outFilterR *= f;
            outFilterR += (inputSampleR * (1.0 - f));
            inputSampleR = outFilterR;
            inputSampleR /= topdB;
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
} // namespace airwindohhs::disintegrate
