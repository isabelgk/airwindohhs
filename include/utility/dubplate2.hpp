#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::dubplate2 {

constexpr std::string_view k_name{ "DubPlate2" };
constexpr std::string_view k_short_description{
    "DubPlate2 is like an ITB mastering house for electronic music."
};
constexpr std::string_view k_long_description{
    "By request, more DubPlate… and new instructions on how to get this sound.Turned out the audio reference I used to chart a path from digital audio master to the sound of that very dubplate, wasn't as simple as I'd thought it to be. I didn't have the real master… and then, when I was fortunate enough to get it, it turned out there wouldn't be a simple way to go from that to the correct sound. The plate had been cut from an earlier mix, and the mastering engineer had complained about how bright it was, and applied more than a bit of EQ. That meant I couldn't treat it like it was a simple one-step process. I'd have to build in an EQ to do what the engineer did, before I could apply the same stuff DubPlate used. On the bright side, that would let the 'lathe' be cleaner, less aggressive. But there would have to be controls that matched what had happened to my reference in real life.But there was an EQ I hadn't revisited for a while… that turned out to be just right for the job. My Baxandall EQ had gone to version 2 already, stripping out some early 'analog effects' I'd used in the first version.Turned out that adding an input trim control, adding the analog effects back again (I'd used the Console5 processing for this), running this into DubPlate and readjusting it using the real reference audio but this time also doing the EQing and then compensating for the changes that no longer needed to be handled by the lathe… got me what I'd wanted in the first place.Provided, that is, you operate it properly… so here's the new version of how to get a dub plate sound using DubPlate2.Firstly, if your audio is perfect and you run it flat, you only need to pad it down with the input control until you have NO, repeat NO clips. The processing even without EQing can produce overs. Baxandall3 (built into DubPlate) is able to peak some way hotter than 0dB, to the point that you'd need a safety clipper like ADClip8 or ClipOnly2. Do not use one! Don't use a limiter, or anything like that, just pad the Input until there are no clips at all on your output.This is important because you can't put a safety clipper between your audio and the lathe. The corners produced by a clipper are hell on lathes, high frequency energy that can burn the cutting head right out. The EQ and things like the elliptical filter and normal highpasses that are part of the circuitry are able to rearrange your audio a bit, stopping it from having the digital clips and brickwalls it might have, generating smoother peaks that don't follow the usual digital rules for where energy sits. To get a real dub plate sound, get your loudness through midrange, not through clipping the peaks.But you have an EQ, so what if you need to use that? It's a very gentle filter, Baxandall3. You can certainly lift the highs or lows, or indeed cut them if that's what you need. The thing is, you have to follow the same rule. Even more so, as Baxandall3 is an analog-style filter that's capable of saturating when pushed! You can add deep lows to work around the bass loss from the lathe and elliptical filter. This will only get you so far, plus you'll discover that it eats up your loudness without giving you that much in return. You can add extreme highs, and the same thing will happen. You'll be fighting the lathe all to produce a more untrackable record that won't even be loud… since, remember, you have to pad the level until it doesn't clip. Otherwise you'll automatically not get a dubplate-type sound.So, think in terms of midrange, allow the record to act like a record, and you should be able to get your ideal dubplate sound out of DubPlate2. You can use Meter to really dial in your results, but even without it the work should be pretty straightforward. Let it give you the sonic peaks your sound deserves, and DubPlate2 will be able to do its job :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class DubPlate2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        bax_freq,
        bax_reso,
        bax_a0,
        bax_a1,
        bax_a2,
        bax_b1,
        bax_b2,
        bax_sLA1,
        bax_sLA2,
        bax_sRA1,
        bax_sRA2,
        bax_sLB1,
        bax_sLB2,
        bax_sRB1,
        bax_sRB2,
        bax_total
    }; // baxed frequency biquad filter for ultrasonics, stereo
    double baxH[bax_total];
    double baxL[bax_total];
    bool flip;
    double iirA;
    double iirB; // first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
    double iirC; // we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
    double iirD; // then there's a Mid highpass.
    double lastSinewAL;
    double lastSinewAR;
    double lastSinewBL;
    double lastSinewBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DubPlate2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < bax_total; x++) {
            baxH[x] = 0.0;
            baxL[x] = 0.0;
        }
        flip = false;
        iirA = 0.0;
        iirB = 0.0;
        iirC = 0.0;
        iirD = 0.0;
        lastSinewAL = 0.0;
        lastSinewAR = 0.0;
        lastSinewBL = 0.0;
        lastSinewBR = 0.0;
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
            case kParamA: return "input"; break;
            case kParamB: return "treble"; break;
            case kParamC: return "bass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Treble"; break;
            case kParamC: return "Bass"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double inputGain = pow(A * 2.0, 2.0);
        double trebleGain = pow(B * 2.0, 2.0);
        double trebleFreq = ((2000.0 * trebleGain) + 200.0) / Effect<T>::getSampleRate();
        if (trebleFreq > 0.45) {
            trebleFreq = 0.45;
        }
        baxH[bax_freq] = trebleFreq;
        baxH[bax_reso] = 0.57735026919; // bessel second order
        double K = tan(M_PI * baxH[bax_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / baxH[bax_reso] + K * K);
        baxH[bax_a0] = K * K * norm;
        baxH[bax_a1] = 2.0 * baxH[bax_a0];
        baxH[bax_a2] = baxH[bax_a0];
        baxH[bax_b1] = 2.0 * (K * K - 1.0) * norm;
        baxH[bax_b2] = (1.0 - K / baxH[bax_reso] + K * K) * norm;
        // end bax highpass
        double bassGain = pow(C * 2.0, 2.0);
        double bassFreq = pow((1.0 - C) * 2.0, 2.0);
        bassFreq = ((2000.0 * bassFreq) + 200.0) / Effect<T>::getSampleRate();
        if (bassFreq > 0.45) {
            bassFreq = 0.45;
        }
        baxL[bax_freq] = bassFreq;
        baxL[bax_reso] = 0.57735026919; // bessel second order
        K = tan(M_PI * baxL[bax_freq]); // lowpass
        norm = 1.0 / (1.0 + K / baxL[bax_reso] + K * K);
        baxL[bax_a0] = K * K * norm;
        baxL[bax_a1] = 2.0 * baxL[bax_a0];
        baxL[bax_a2] = baxL[bax_a0];
        baxL[bax_b1] = 2.0 * (K * K - 1.0) * norm;
        baxL[bax_b2] = (1.0 - K / baxL[bax_reso] + K * K) * norm;
        // end bax lowpass
        double iirSide = 0.01862 / overallscale;
        double iirMid = 0.01102 / overallscale;
        double threshSinewA = 0.1442 / overallscale;
        double threshSinewB = 0.0274 / overallscale;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL = sin(fmax(fmin(inputSampleL * inputGain, M_PI_2), -M_PI_2));
            inputSampleR = sin(fmax(fmin(inputSampleR * inputGain, M_PI_2), -M_PI_2));
            // encode Console5: good cleanness
            double trebleSampleL;
            double trebleSampleR;
            double bassSampleL;
            double bassSampleR;
            if (flip) {
                trebleSampleL = (inputSampleL * baxH[bax_a0]) + baxH[bax_sLA1];
                baxH[bax_sLA1] = (inputSampleL * baxH[bax_a1]) - (trebleSampleL * baxH[bax_b1]) + baxH[bax_sLA2];
                baxH[bax_sLA2] = (inputSampleL * baxH[bax_a2]) - (trebleSampleL * baxH[bax_b2]);
                trebleSampleL = inputSampleL - trebleSampleL;
                trebleSampleR = (inputSampleR * baxH[bax_a0]) + baxH[bax_sRA1];
                baxH[bax_sRA1] = (inputSampleR * baxH[bax_a1]) - (trebleSampleR * baxH[bax_b1]) + baxH[bax_sRA2];
                baxH[bax_sRA2] = (inputSampleR * baxH[bax_a2]) - (trebleSampleR * baxH[bax_b2]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleL = (inputSampleL * baxL[bax_a0]) + baxL[bax_sLA1];
                baxL[bax_sLA1] = (inputSampleL * baxL[bax_a1]) - (bassSampleL * baxL[bax_b1]) + baxL[bax_sLA2];
                baxL[bax_sLA2] = (inputSampleL * baxL[bax_a2]) - (bassSampleL * baxL[bax_b2]);
                bassSampleR = (inputSampleR * baxL[bax_a0]) + baxL[bax_sRA1];
                baxL[bax_sRA1] = (inputSampleR * baxL[bax_a1]) - (bassSampleR * baxL[bax_b1]) + baxL[bax_sRA2];
                baxL[bax_sRA2] = (inputSampleR * baxL[bax_a2]) - (bassSampleR * baxL[bax_b2]);
            }
            else {
                trebleSampleL = (inputSampleL * baxH[bax_a0]) + baxH[bax_sLB1];
                baxH[bax_sLB1] = (inputSampleL * baxH[bax_a1]) - (trebleSampleL * baxH[bax_b1]) + baxH[bax_sLB2];
                baxH[bax_sLB2] = (inputSampleL * baxH[bax_a2]) - (trebleSampleL * baxH[bax_b2]);
                trebleSampleL = inputSampleL - trebleSampleL;
                trebleSampleR = (inputSampleR * baxH[bax_a0]) + baxH[bax_sRB1];
                baxH[bax_sRB1] = (inputSampleR * baxH[bax_a1]) - (trebleSampleR * baxH[bax_b1]) + baxH[bax_sRB2];
                baxH[bax_sRB2] = (inputSampleR * baxH[bax_a2]) - (trebleSampleR * baxH[bax_b2]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleL = (inputSampleL * baxL[bax_a0]) + baxL[bax_sLB1];
                baxL[bax_sLB1] = (inputSampleL * baxL[bax_a1]) - (bassSampleL * baxL[bax_b1]) + baxL[bax_sLB2];
                baxL[bax_sLB2] = (inputSampleL * baxL[bax_a2]) - (bassSampleL * baxL[bax_b2]);
                bassSampleR = (inputSampleR * baxL[bax_a0]) + baxL[bax_sRB1];
                baxL[bax_sRB1] = (inputSampleR * baxL[bax_a1]) - (bassSampleR * baxL[bax_b1]) + baxL[bax_sRB2];
                baxL[bax_sRB2] = (inputSampleR * baxL[bax_a2]) - (bassSampleR * baxL[bax_b2]);
            }
            trebleSampleL *= trebleGain;
            trebleSampleR *= trebleGain;
            bassSampleL *= bassGain;
            bassSampleR *= bassGain;
            inputSampleL = bassSampleL + trebleSampleL; // interleaved biquad
            inputSampleR = bassSampleR + trebleSampleR; // interleaved biquad
            inputSampleL = asin(fmax(fmin(inputSampleL, 0.99999), -0.99999));
            inputSampleR = asin(fmax(fmin(inputSampleR, 0.99999), -0.99999));
            // amplitude aspect
            double mid = inputSampleL + inputSampleR;
            double side = inputSampleL - inputSampleR;
            // assign mid and side.Between these sections, you can do mid/side processing
            double temp = side;
            double correction;
            if (flip) {
                iirA = (iirA * (1.0 - iirSide)) + (temp * iirSide);
                temp -= iirA;
                correction = iirA;
            }
            else {
                iirB = (iirB * (1.0 - iirSide)) + (temp * iirSide);
                temp -= iirB;
                correction = iirB;
            }
            iirC = (iirC * (1.0 - iirSide)) + (temp * iirSide);
            temp -= iirC;
            correction += (iirC * 0.162);
            side -= sin(correction);
            iirD = (iirD * (1.0 - iirMid)) + (mid * iirMid);
            mid -= sin(iirD);
            // gonna cut those lows a hair
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            // unassign mid and side
            temp = inputSampleL;
            double sinew = threshSinewA * cos(lastSinewAL * lastSinewAL);
            if (inputSampleL - lastSinewAL > sinew) {
                temp = lastSinewAL + sinew;
            }
            if (-(inputSampleL - lastSinewAL) > sinew) {
                temp = lastSinewAL - sinew;
            }
            lastSinewAL = temp;
            if (lastSinewAL > 1.0) {
                lastSinewAL = 1.0;
            }
            if (lastSinewAL < -1.0) {
                lastSinewAL = -1.0;
            }
            inputSampleL = (inputSampleL * 0.5) + (lastSinewAL * 0.5);
            sinew = threshSinewB * cos(lastSinewBL * lastSinewBL);
            if (inputSampleL - lastSinewBL > sinew) {
                temp = lastSinewBL + sinew;
            }
            if (-(inputSampleL - lastSinewBL) > sinew) {
                temp = lastSinewBL - sinew;
            }
            lastSinewBL = temp;
            if (lastSinewBL > 1.0) {
                lastSinewBL = 1.0;
            }
            if (lastSinewBL < -1.0) {
                lastSinewBL = -1.0;
            }
            inputSampleL = (inputSampleL * 0.5) + (lastSinewBL * 0.5);
            temp = inputSampleR;
            sinew = threshSinewA * cos(lastSinewAR * lastSinewAR);
            if (inputSampleR - lastSinewAR > sinew) {
                temp = lastSinewAR + sinew;
            }
            if (-(inputSampleR - lastSinewAR) > sinew) {
                temp = lastSinewAR - sinew;
            }
            lastSinewAR = temp;
            if (lastSinewAR > 1.0) {
                lastSinewAR = 1.0;
            }
            if (lastSinewAR < -1.0) {
                lastSinewAR = -1.0;
            }
            inputSampleR = (inputSampleR * 0.5) + (lastSinewAR * 0.5);
            sinew = threshSinewB * cos(lastSinewBR * lastSinewBR);
            if (inputSampleR - lastSinewBR > sinew) {
                temp = lastSinewBR + sinew;
            }
            if (-(inputSampleR - lastSinewBR) > sinew) {
                temp = lastSinewBR - sinew;
            }
            lastSinewBR = temp;
            if (lastSinewBR > 1.0) {
                lastSinewBR = 1.0;
            }
            if (lastSinewBR < -1.0) {
                lastSinewBR = -1.0;
            }
            inputSampleR = (inputSampleR * 0.5) + (lastSinewBR * 0.5);
            // run Sinew to stop excess slews, two layers to make it more audible
            flip = !flip; // both for Baxandall3 and DubPlate highpass
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
} // namespace airwindohhs::dubplate2
