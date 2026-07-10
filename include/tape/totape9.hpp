#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::totape9 {

constexpr std::string_view k_name{ "ToTape9" };
constexpr std::string_view k_short_description{
    "ToTape9 is Airwindows tape emulation and brightness compression."
};
constexpr std::string_view k_long_description{
    "ToTape is one of the most successful plugins I've ever had. Step by step, it's come along and incorporated more and more things, but always in an original way. It picked up flutter, bias, Dubly, and then adapted all these things to the needs of the musician. So if you don't know how to calibrate Dubly units for heavy music, you have a Tilt that will give you more of a leaning towards brightness, or darkness… but it's implemented via the Dubly controls so it acts like adjustments on a tape machine. Shape works likewise, going for either more old-school sonority or more big-studio chill. Flutter behaves itself unless you don't want it to, and the input and output controls make it very simple to manage ToTape9's gain staging, for gentle use or aggressive slam.But then the heart of ToTape is completely replaced by what I learned making TapeHack2, itself a giant leap over the original TapeHack, which was itself a huge discovery. When I tried this in the guts of ToTape I was just completely shocked by how huge it sounded. The scale of the sound put the previous version to shame. You do not have to slam this to have it sound amazing, but if slamming's your thing I think this will bring delight and joy.And then it's topped off by not the latest ClipOnly… but the NEXT ClipOnly. The one where I ran with the concept, not completely unlike my old Chrome Oxide plugin but in a new perspective, of 'dithering aliasing'. It turns out you can't actually dither aliasing but there's a large number of interesting effects you can produce by noising up clipping. So when I experimented, I ended up applying it in a way nobody else was doing, introducing noise just faintly as clipping begins to happen, ramping it up, and then as hard clipping takes over, shifting it to the 'edge samples' akin to what ClipOnly and ClipOnly2 have always done, meaning the noise goes back away again (and is never all that loud: sorry, no special FX here)And combining this with the existing ClipOnly2 created a monster.So that's what's on the end of ToTape9. Output at 0.5 means you're just barely bumping this final clip stage. Boost it and you're hitting the output clip harder, cut it back and it's purely the ToTape section. I would imagine mostly leaving it at 0.5 is what most people will do, but it's yours to experiment with. It produces a 'beyond tape slam' that is shockingly big and deep and loud, it applies its work only when clipping is actively happening, it's all within a very tiny amplitude window so it stays clean on most everything, and it gracefully handles Gibbs effect overslew (intersample clipping) without ever applying upsampling or downsampling, so the sound is aggressive and raw the way I like my tape effects to be.Try ToTape9. It's the most intensely tapey plugin I've ever done, with new abilities like the ability to soak up highs like the real thing. It's got all the little details you could ask of a no-compromises real tape model, but adapted to musical purposes so you can think 'tilt towards more brightness' rather than 'should I go with a 355 nanoWebers per meter calibration'. And it's got the most killer output clip stage I've ever made, just to let you use it as a kind of loudness maximizer right out of the box.I hope this brightens your day :)"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class ToTape9 final : public Effect<T>
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
    double iirEncL;
    double iirDecL;
    double compEncL;
    double compDecL;
    double avgEncL;
    double avgDecL;
    double iirEncR;
    double iirDecR;
    double compEncR;
    double compDecR;
    double avgEncR;
    double avgDecR;
    double dL[1002];
    double dR[1002];
    double sweepL;
    double sweepR;
    double nextmaxL;
    double nextmaxR;
    int gcount;
    enum
    {
        prevSampL1,
        prevSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        threshold9,
        gslew_total
    }; // fixed frequency pear filter for ultrasonics, stereo
    double gslew[gslew_total]; // end bias
    double hysteresisL;
    double hysteresisR;
    double headBumpL;
    double headBumpR;
    enum
    {
        hdb_freq,
        hdb_reso,
        hdb_a0,
        hdb_a1,
        hdb_a2,
        hdb_b1,
        hdb_b2,
        hdb_sL1,
        hdb_sL2,
        hdb_sR1,
        hdb_sR2,
        hdb_total
    }; // fixed frequency biquad filter for ultrasonics, stereo
    double hdbA[hdb_total];
    double hdbB[hdb_total];
    double avg32L[33];
    double avg32R[33];
    double avg16L[17];
    double avg16R[17];
    double avg8L[9];
    double avg8R[9];
    double avg4L[5];
    double avg4R[5];
    double avg2L[3];
    double avg2R[3];
    double post32L[33];
    double post32R[33];
    double post16L[17];
    double post16R[17];
    double post8L[9];
    double post8R[9];
    double post4L[5];
    double post4R[5];
    double post2L[3];
    double post2R[3];
    double lastDarkL;
    double lastDarkR;
    int avgPos;
    // preTapeHack
    double lastSampleL;
    double intermediateL[18];
    double slewL[34];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[18];
    double slewR[34];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly3
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ToTape9()
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
        iirEncL = 0.0;
        iirDecL = 0.0;
        compEncL = 1.0;
        compDecL = 1.0;
        avgEncL = 0.0;
        avgDecL = 0.0;
        iirEncR = 0.0;
        iirDecR = 0.0;
        compEncR = 1.0;
        compDecR = 1.0;
        avgEncR = 0.0;
        avgDecR = 0.0;
        for (int temp = 0; temp < 1001; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        sweepL = M_PI;
        sweepR = M_PI;
        nextmaxL = 0.5;
        nextmaxR = 0.5;
        gcount = 0;
        for (int x = 0; x < gslew_total; x++) {
            gslew[x] = 0.0;
        }
        hysteresisL = 0.0;
        hysteresisR = 0.0;
        headBumpL = 0.0;
        headBumpR = 0.0;
        for (int x = 0; x < hdb_total; x++) {
            hdbA[x] = 0.0;
            hdbB[x] = 0.0;
        }
        // from ZBandpass, so I can use enums with it
        for (int x = 0; x < 33; x++) {
            avg32L[x] = 0.0;
            post32L[x] = 0.0;
            avg32R[x] = 0.0;
            post32R[x] = 0.0;
        }
        for (int x = 0; x < 17; x++) {
            avg16L[x] = 0.0;
            post16L[x] = 0.0;
            avg16R[x] = 0.0;
            post16R[x] = 0.0;
        }
        for (int x = 0; x < 9; x++) {
            avg8L[x] = 0.0;
            post8L[x] = 0.0;
            avg8R[x] = 0.0;
            post8R[x] = 0.0;
        }
        for (int x = 0; x < 5; x++) {
            avg4L[x] = 0.0;
            post4L[x] = 0.0;
            avg4R[x] = 0.0;
            post4R[x] = 0.0;
        }
        for (int x = 0; x < 3; x++) {
            avg2L[x] = 0.0;
            post2L[x] = 0.0;
            avg2R[x] = 0.0;
            post2R[x] = 0.0;
        }
        avgPos = 0;
        lastDarkL = 0.0;
        lastDarkR = 0.0;
        // preTapeHack
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 17; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
        for (int x = 0; x < 33; x++) {
            slewL[x] = 0.0;
            slewR[x] = 0.0;
        }
        // this is reset: values being initialized only once. Startup values, whatever they are.
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
        kNumParameters = 9
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "tilt"; break;
            case kParamC: return "shape"; break;
            case kParamD: return "flutter"; break;
            case kParamE: return "flutspd"; break;
            case kParamF: return "bias"; break;
            case kParamG: return "headbmp"; break;
            case kParamH: return "headfrq"; break;
            case kParamI: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Tilt"; break;
            case kParamC: return "Shape"; break;
            case kParamD: return "Flutter"; break;
            case kParamE: return "FlutSpd"; break;
            case kParamF: return "Bias"; break;
            case kParamG: return "HeadBmp"; break;
            case kParamH: return "HeadFrq"; break;
            case kParamI: return "Output"; break;

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
            case kParamH: return "hz"; break;
            case kParamI: return ""; break;

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
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        int slewsing = floor(overallscale * 2.0);
        if (slewsing < 2) {
            slewsing = 2;
        }
        if (slewsing > 32) {
            slewsing = 32;
        }
        double inputGain = pow(A * 2.0, 2.0);
        double dublyAmount = B * 2.0;
        double outlyAmount = (1.0 - B) * -2.0;
        if (outlyAmount < -1.0) {
            outlyAmount = -1.0;
        }
        double iirEncFreq = (1.0 - C) / overallscale;
        double iirDecFreq = C / overallscale;
        double flutDepth = pow(D, 6) * overallscale * 50;
        if (flutDepth > 498.0) {
            flutDepth = 498.0;
        }
        double flutFrequency = (0.02 * pow(E, 3)) / overallscale;
        double bias = (F * 2.0) - 1.0;
        double underBias = (pow(bias, 4) * 0.25) / overallscale;
        double overBias = pow(1.0 - bias, 3) / overallscale;
        if (bias > 0.0) {
            underBias = 0.0;
        }
        if (bias < 0.0) {
            overBias = 1.0 / overallscale;
        }
        gslew[threshold9] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold8] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold7] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold6] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold5] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold4] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold3] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold2] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold1] = overBias;
        overBias *= 1.618033988749894848204586;
        double headBumpDrive = (G * 0.1) / overallscale;
        double headBumpMix = G * 0.5;
        hdbA[hdb_freq] = (((H * H) * 175.0) + 25.0) / Effect<T>::getSampleRate();
        hdbB[hdb_freq] = hdbA[hdb_freq] * 0.9375;
        hdbB[hdb_reso] = hdbA[hdb_reso] = 0.618033988749894848204586;
        hdbB[hdb_a1] = hdbA[hdb_a1] = 0.0;
        double K = tan(M_PI * hdbA[hdb_freq]);
        double norm = 1.0 / (1.0 + K / hdbA[hdb_reso] + K * K);
        hdbA[hdb_a0] = K / hdbA[hdb_reso] * norm;
        hdbA[hdb_a2] = -hdbA[hdb_a0];
        hdbA[hdb_b1] = 2.0 * (K * K - 1.0) * norm;
        hdbA[hdb_b2] = (1.0 - K / hdbA[hdb_reso] + K * K) * norm;
        K = tan(M_PI * hdbB[hdb_freq]);
        norm = 1.0 / (1.0 + K / hdbB[hdb_reso] + K * K);
        hdbB[hdb_a0] = K / hdbB[hdb_reso] * norm;
        hdbB[hdb_a2] = -hdbB[hdb_a0];
        hdbB[hdb_b1] = 2.0 * (K * K - 1.0) * norm;
        hdbB[hdb_b2] = (1.0 - K / hdbB[hdb_reso] + K * K) * norm;
        double outputGain = I * 2.0;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            // Dubly encode
            iirEncL = (iirEncL * (1.0 - iirEncFreq)) + (inputSampleL * iirEncFreq);
            double highPart = ((inputSampleL - iirEncL) * 2.848);
            highPart += avgEncL;
            avgEncL = (inputSampleL - iirEncL) * 1.152;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            double dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compEncL = (compEncL * (1.0 - iirEncFreq)) + (dubly * iirEncFreq);
                inputSampleL += ((highPart * compEncL) * dublyAmount);
            } // end Dubly encode L
            iirEncR = (iirEncR * (1.0 - iirEncFreq)) + (inputSampleR * iirEncFreq);
            highPart = ((inputSampleR - iirEncR) * 2.848);
            highPart += avgEncR;
            avgEncR = (inputSampleR - iirEncR) * 1.152;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compEncR = (compEncR * (1.0 - iirEncFreq)) + (dubly * iirEncFreq);
                inputSampleR += ((highPart * compEncR) * dublyAmount);
            } // end Dubly encode R
            // begin Flutter
            if (flutDepth > 0.0) {
                if (gcount < 0 || gcount > 999) {
                    gcount = 999;
                }
                dL[gcount] = inputSampleL;
                int count = gcount;
                double offset = flutDepth + (flutDepth * sin(sweepL));
                sweepL += nextmaxL * flutFrequency;
                if (sweepL > (M_PI * 2.0)) {
                    sweepL -= M_PI * 2.0;
                    double flutA = 0.24 + (fpdL / (double)UINT32_MAX * 0.74);
                    fpdL ^= fpdL << 13;
                    fpdL ^= fpdL >> 17;
                    fpdL ^= fpdL << 5;
                    double flutB = 0.24 + (fpdL / (double)UINT32_MAX * 0.74);
                    if (fabs(flutA - sin(sweepR + nextmaxR)) < fabs(flutB - sin(sweepR + nextmaxR))) {
                        nextmaxL = flutA;
                    }
                    else {
                        nextmaxL = flutB;
                    }
                }
                count += (int)floor(offset);
                inputSampleL = (dL[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
                inputSampleL += (dL[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));
                dR[gcount] = inputSampleR;
                count = gcount;
                offset = flutDepth + (flutDepth * sin(sweepR));
                sweepR += nextmaxR * flutFrequency;
                if (sweepR > (M_PI * 2.0)) {
                    sweepR -= M_PI * 2.0;
                    double flutA = 0.24 + (fpdR / (double)UINT32_MAX * 0.74);
                    fpdR ^= fpdR << 13;
                    fpdR ^= fpdR >> 17;
                    fpdR ^= fpdR << 5;
                    double flutB = 0.24 + (fpdR / (double)UINT32_MAX * 0.74);
                    if (fabs(flutA - sin(sweepL + nextmaxL)) < fabs(flutB - sin(sweepL + nextmaxL))) {
                        nextmaxR = flutA;
                    }
                    else {
                        nextmaxR = flutB;
                    }
                }
                count += (int)floor(offset);
                inputSampleR = (dR[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
                inputSampleR += (dR[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));
                gcount--;
            }
            // end Flutter
            // start bias routine
            if (fabs(bias) > 0.001) {
                for (int x = 0; x < gslew_total; x += 3) {
                    if (underBias > 0.0) {
                        double stuck = fabs(inputSampleL - (gslew[x] / 0.975)) / underBias;
                        if (stuck < 1.0) {
                            inputSampleL = (inputSampleL * stuck) + ((gslew[x] / 0.975) * (1.0 - stuck));
                        }
                        stuck = fabs(inputSampleR - (gslew[x + 1] / 0.975)) / underBias;
                        if (stuck < 1.0) {
                            inputSampleR = (inputSampleR * stuck) + ((gslew[x + 1] / 0.975) * (1.0 - stuck));
                        }
                    }
                    if ((inputSampleL - gslew[x]) > gslew[x + 2]) {
                        inputSampleL = gslew[x] + gslew[x + 2];
                    }
                    if (-(inputSampleL - gslew[x]) > gslew[x + 2]) {
                        inputSampleL = gslew[x] - gslew[x + 2];
                    }
                    gslew[x] = inputSampleL * 0.975;
                    if ((inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                        inputSampleR = gslew[x + 1] + gslew[x + 2];
                    }
                    if (-(inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                        inputSampleR = gslew[x + 1] - gslew[x + 2];
                    }
                    gslew[x + 1] = inputSampleR * 0.975;
                }
            }
            // end bias routine
            // begin tiny hysteresis
            double applyHysteresis = (1.0 - fabs(inputSampleL)) * (1.0 - fabs(inputSampleL)) * 0.012;
            hysteresisL = fmax(fmin(hysteresisL + ((inputSampleL * fabs(inputSampleL))), 0.011449), -0.011449) * 0.999;
            inputSampleL += (hysteresisL * applyHysteresis);
            applyHysteresis = (1.0 - fabs(inputSampleR)) * (1.0 - fabs(inputSampleR)) * 0.012;
            hysteresisR = fmax(fmin(hysteresisR + ((inputSampleR * fabs(inputSampleR))), 0.011449), -0.011449) * 0.999;
            inputSampleR += (hysteresisR * applyHysteresis);
            // begin TapeHack2
            double darkSampleL = inputSampleL;
            double darkSampleR = inputSampleR;
            if (avgPos > 31) {
                avgPos = 0;
            }
            if (slewsing > 31) {
                avg32L[avgPos] = darkSampleL;
                avg32R[avgPos] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {
                    darkSampleL += avg32L[x];
                    darkSampleR += avg32R[x];
                }
                darkSampleL /= 32.0;
                darkSampleR /= 32.0;
            }
            if (slewsing > 15) {
                avg16L[avgPos % 16] = darkSampleL;
                avg16R[avgPos % 16] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {
                    darkSampleL += avg16L[x];
                    darkSampleR += avg16R[x];
                }
                darkSampleL /= 16.0;
                darkSampleR /= 16.0;
            }
            if (slewsing > 7) {
                avg8L[avgPos % 8] = darkSampleL;
                avg8R[avgPos % 8] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {
                    darkSampleL += avg8L[x];
                    darkSampleR += avg8R[x];
                }
                darkSampleL /= 8.0;
                darkSampleR /= 8.0;
            }
            if (slewsing > 3) {
                avg4L[avgPos % 4] = darkSampleL;
                avg4R[avgPos % 4] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {
                    darkSampleL += avg4L[x];
                    darkSampleR += avg4R[x];
                }
                darkSampleL /= 4.0;
                darkSampleR /= 4.0;
            }
            if (slewsing > 1) {
                avg2L[avgPos % 2] = darkSampleL;
                avg2R[avgPos % 2] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {
                    darkSampleL += avg2L[x];
                    darkSampleR += avg2R[x];
                }
                darkSampleL /= 2.0;
                darkSampleR /= 2.0;
            } // only update avgPos after the post-distortion filter stage
            double avgSlewL = fmin(fabs(lastDarkL - inputSampleL) * 0.12 * overallscale, 1.0);
            avgSlewL = 1.0 - (1.0 - avgSlewL * 1.0 - avgSlewL);
            inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
            lastDarkL = darkSampleL;
            double avgSlewR = fmin(fabs(lastDarkR - inputSampleR) * 0.12 * overallscale, 1.0);
            avgSlewR = 1.0 - (1.0 - avgSlewR * 1.0 - avgSlewR);
            inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
            lastDarkR = darkSampleR;
            // begin TapeHack
            inputSampleL = fmax(fmin(inputSampleL, 2.305929007734908), -2.305929007734908);
            double addtwo = inputSampleL * inputSampleL;
            double empower = inputSampleL * addtwo; // inputSample to the third power
            inputSampleL -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleL += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleL -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleL += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleL -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            inputSampleR = fmax(fmin(inputSampleR, 2.305929007734908), -2.305929007734908);
            addtwo = inputSampleR * inputSampleR;
            empower = inputSampleR * addtwo; // inputSample to the third power
            inputSampleR -= (empower / 6.0);
            empower *= addtwo; // to the fifth power
            inputSampleR += (empower / 69.0);
            empower *= addtwo; // seventh
            inputSampleR -= (empower / 2530.08);
            empower *= addtwo; // ninth
            inputSampleR += (empower / 224985.6);
            empower *= addtwo; // eleventh
            inputSampleR -= (empower / 9979200.0f);
            // this is a degenerate form of a Taylor Series to approximate sin()
            darkSampleL = inputSampleL;
            darkSampleR = inputSampleR;
            if (avgPos > 31) {
                avgPos = 0;
            }
            if (slewsing > 31) {
                post32L[avgPos] = darkSampleL;
                post32R[avgPos] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {
                    darkSampleL += post32L[x];
                    darkSampleR += post32R[x];
                }
                darkSampleL /= 32.0;
                darkSampleR /= 32.0;
            }
            if (slewsing > 15) {
                post16L[avgPos % 16] = darkSampleL;
                post16R[avgPos % 16] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {
                    darkSampleL += post16L[x];
                    darkSampleR += post16R[x];
                }
                darkSampleL /= 16.0;
                darkSampleR /= 16.0;
            }
            if (slewsing > 7) {
                post8L[avgPos % 8] = darkSampleL;
                post8R[avgPos % 8] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {
                    darkSampleL += post8L[x];
                    darkSampleR += post8R[x];
                }
                darkSampleL /= 8.0;
                darkSampleR /= 8.0;
            }
            if (slewsing > 3) {
                post4L[avgPos % 4] = darkSampleL;
                post4R[avgPos % 4] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {
                    darkSampleL += post4L[x];
                    darkSampleR += post4R[x];
                }
                darkSampleL /= 4.0;
                darkSampleR /= 4.0;
            }
            if (slewsing > 1) {
                post2L[avgPos % 2] = darkSampleL;
                post2R[avgPos % 2] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {
                    darkSampleL += post2L[x];
                    darkSampleR += post2R[x];
                }
                darkSampleL /= 2.0;
                darkSampleR /= 2.0;
            }
            avgPos++;
            inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
            inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
            // use the previously calculated depth of the filter
            // begin HeadBump
            double headBumpSampleL = 0.0;
            double headBumpSampleR = 0.0;
            if (headBumpMix > 0.0) {
                headBumpL += (inputSampleL * headBumpDrive);
                headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618 / sqrt(overallscale)));
                headBumpR += (inputSampleR * headBumpDrive);
                headBumpR -= (headBumpR * headBumpR * headBumpR * (0.0618 / sqrt(overallscale)));
                double headBiqSampleL = (headBumpL * hdbA[hdb_a0]) + hdbA[hdb_sL1];
                hdbA[hdb_sL1] = (headBumpL * hdbA[hdb_a1]) - (headBiqSampleL * hdbA[hdb_b1]) + hdbA[hdb_sL2];
                hdbA[hdb_sL2] = (headBumpL * hdbA[hdb_a2]) - (headBiqSampleL * hdbA[hdb_b2]);
                headBumpSampleL = (headBiqSampleL * hdbB[hdb_a0]) + hdbB[hdb_sL1];
                hdbB[hdb_sL1] = (headBiqSampleL * hdbB[hdb_a1]) - (headBumpSampleL * hdbB[hdb_b1]) + hdbB[hdb_sL2];
                hdbB[hdb_sL2] = (headBiqSampleL * hdbB[hdb_a2]) - (headBumpSampleL * hdbB[hdb_b2]);
                double headBiqSampleR = (headBumpR * hdbA[hdb_a0]) + hdbA[hdb_sR1];
                hdbA[hdb_sR1] = (headBumpR * hdbA[hdb_a1]) - (headBiqSampleR * hdbA[hdb_b1]) + hdbA[hdb_sR2];
                hdbA[hdb_sR2] = (headBumpR * hdbA[hdb_a2]) - (headBiqSampleR * hdbA[hdb_b2]);
                headBumpSampleR = (headBiqSampleR * hdbB[hdb_a0]) + hdbB[hdb_sR1];
                hdbB[hdb_sR1] = (headBiqSampleR * hdbB[hdb_a1]) - (headBumpSampleR * hdbB[hdb_b1]) + hdbB[hdb_sR2];
                hdbB[hdb_sR2] = (headBiqSampleR * hdbB[hdb_a2]) - (headBumpSampleR * hdbB[hdb_b2]);
            }
            // end HeadBump
            inputSampleL += (headBumpSampleL * headBumpMix);
            inputSampleR += (headBumpSampleR * headBumpMix);
            // Dubly decode
            iirDecL = (iirDecL * (1.0 - iirDecFreq)) + (inputSampleL * iirDecFreq);
            highPart = ((inputSampleL - iirDecL) * 2.628);
            highPart += avgDecL;
            avgDecL = (inputSampleL - iirDecL) * 1.372;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compDecL = (compDecL * (1.0 - iirDecFreq)) + (dubly * iirDecFreq);
                inputSampleL += ((highPart * compDecL) * outlyAmount);
            } // end Dubly decode L
            iirDecR = (iirDecR * (1.0 - iirDecFreq)) + (inputSampleR * iirDecFreq);
            highPart = ((inputSampleR - iirDecR) * 2.628);
            highPart += avgDecR;
            avgDecR = (inputSampleR - iirDecR) * 1.372;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compDecR = (compDecR * (1.0 - iirDecFreq)) + (dubly * iirDecFreq);
                inputSampleR += ((highPart * compDecR) * outlyAmount);
            } // end Dubly decode R
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
            }
            // begin ClipOnly3 as a little, compressed chunk that can be dropped into code
            double noise = 1.0 - ((double(fpdL) / UINT32_MAX) * 0.076);
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = (0.9085097 * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = 0.94; //~-0.2dB to nearly match ClipOnly and ClipOnly2
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9085097) {
                wasPosClipL = true;
                inputSampleL = (0.9085097 * noise) + (lastSampleL * (1.0 - noise));
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = (-0.9085097 * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = -0.94;
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9085097) {
                wasNegClipL = true;
                inputSampleL = (-0.9085097 * noise) + (lastSampleL * (1.0 - noise));
            }
            slewL[spacing * 2] = fabs(lastSampleL - inputSampleL);
            for (int x = spacing * 2; x > 0; x--) {
                slewL[x - 1] = slewL[x];
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL;
            // latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0];
            if (wasPosClipL || wasNegClipL) {
                for (int x = spacing; x > 0; x--) {
                    lastSampleL += intermediateL[x];
                }
                lastSampleL /= spacing;
            }
            double finalSlew = 0.0;
            for (int x = spacing * 2; x >= 0; x--) {
                if (finalSlew < slewL[x]) {
                    finalSlew = slewL[x];
                }
            }
            double postclip = 0.94 / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleL > postclip) {
                inputSampleL = postclip;
            }
            if (inputSampleL < -postclip) {
                inputSampleL = -postclip;
            }
            noise = 1.0 - ((double(fpdR) / UINT32_MAX) * 0.076);
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = (0.9085097 * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = 0.94; //~-0.2dB to nearly match ClipOnly and ClipOnly2
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9085097) {
                wasPosClipR = true;
                inputSampleR = (0.9085097 * noise) + (lastSampleR * (1.0 - noise));
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = (-0.9085097 * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = -0.94;
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9085097) {
                wasNegClipR = true;
                inputSampleR = (-0.9085097 * noise) + (lastSampleR * (1.0 - noise));
            }
            slewR[spacing * 2] = fabs(lastSampleR - inputSampleR);
            for (int x = spacing * 2; x > 0; x--) {
                slewR[x - 1] = slewR[x];
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR;
            // latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0];
            if (wasPosClipR || wasNegClipR) {
                for (int x = spacing; x > 0; x--) {
                    lastSampleR += intermediateR[x];
                }
                lastSampleR /= spacing;
            }
            finalSlew = 0.0;
            for (int x = spacing * 2; x >= 0; x--) {
                if (finalSlew < slewR[x]) {
                    finalSlew = slewR[x];
                }
            }
            postclip = 0.94 / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleR > postclip) {
                inputSampleR = postclip;
            }
            if (inputSampleR < -postclip) {
                inputSampleR = -postclip;
            }
            // end ClipOnly3 as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::totape9
