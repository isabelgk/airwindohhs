#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::totape7 {

constexpr std::string_view k_name{ "ToTape7" };
constexpr std::string_view k_short_description{
    "ToTape7 is Airwindows tape emulation with Bias and Dubly."
};
constexpr std::string_view k_long_description{
    "So about that simplification of ToTape? Nah. But… set everything to the middle and you can ignore all the many controls.ToTape7 not only upgrades pretty much every single thing about ToTape, but it brings new functionality beyond any previous version (even when they're good) and it lets you go completely bonkers tailoring signature new sounds however you like them. This is the ToTape for people who wanted all the controls, and more. It's the ToTape for salvaging any sort of audio no matter how DAW-like it is… and it's the ToTape that can be stripped right down to serve as its own FromTape, anytime you like, or if you're trying to run lots of channels of it because you want to tape all the things and then mix them down also to tape. Which it's great at, by the way, as it soaks up loudness super well and even has a ClipOnly2 built in so it can serve, standalone, as your ultimate loudness clipper.ToTape7 has exactly as many controls as you can fit in Airwindows Consolidated, which means it'll also run in VCV Rack. That means there are some possible controls it skips, and exactly one control that's dual-ganged with two things riding on one single knob. It's just a different knob this time: used to be dual-ganged on the knob marked Soften. The controls from top to bottom operate Dubly Encode, Drive, Flutter, Bias, Head Bump, and Dubly Decode. Again, set everything to the middle for 'normal', but you can go way past normal if you want.Dubly Encode (not the same thing but the same concept as a famous noise reduction system) applies a brightened and compressed boost to the audio pre-tape, with the amount governed by Amt and the highs cutoff by Freq. (Dubly Decode is exactly the same, except it simply subtracts the effect it makes: this is the secret Seventies tape sound, especially when you tune the Dublies to produce effects). Match them to get mostly 'normal' sound, mismatch them for effect: mind that you don't boost Decode Amt too much or it will produce sort of anti-crunch sound, going past 'cancellation' to obvious distortion. (or do it if you feel like it, I'm not your boss)Tape Drive is your boost (or pad). 0.5 is unity gain. Boost if you want lots of tape drive, or to loudenate. You should have a good ability to do this and make it sound the way you want. Since Dubly was added, it's even better at allowing for clean gain here, as Dubly tends to suppress harmonics from distortion.Flutter is like Flutter2, but is NOT exactly the same. It's updated, even since Flutter2, for the purpose of letting the '3D tape' emulation (letting tape bend laterally as well as stretch) be more accurate. It's also toned down a bit so 0.5 gives you a reasonable, real-world flutter effect. The two channels will use their randomness to try and chase each other a little, much like how TPDFWide does the same thing to be LESS correlated. Turns out with default settings this is really, really good at getting a spacious tape realism without letting the imaging go too weird. You can also bypass the whole thing by setting Flutter to zero: if you're running at low sample rate, consider doing that if you're running lots of ToTape and losing the extreme highs. I might not run Flutter on multitracks, just the buss.Bias is usually set to slight overbiasing. It's like GoldenSlew, but it's simplified and it's being run between Dubly encode and decode, so it's acting differently from the standalone GoldenSlew. Underbiasing also works but is a different algorithm that's probably not going to be people's first choice, but you can do it. Slight overbiasing is your best weapon against unpleasant tizzy highs.Head Bump is like DubSub2, but it too is updated even more to get more out of ToTape7. There's a highpass built in at 0.5 setting that balances the bump against the rest of the audio, making it so if you stack up the plugin it doesn't get too messy, modeled after references of real tape machines doing multiple generations. Anywhere you set the head bump frequency should get reasonable results with Head Bump at 0.5. If you pull it back OR boost it, you begin to drop the highpass as you do that, meaning it will either crossfade over to the original digital bass (with extended subs) as you reduce the bump, or it'll start adding even more exaggerated bump to the original bass as you crank. So, it's a special voicing for doing all of the things, with 0.5 as the 'peak realism' and most restrictive setting. Ouside that, just pick whether you want less 'tape bass', or 'mega bass' by combining the source and the bump.Said bump still does the thing from DubSub2 where it's also giving a mild notch at double the frequency, which is characteristic of pretty much all real tape machines. You get to pick the frequency, because why would you be tied to a given machine when they're all different frequencies already, and when the heart of the effect is not getting the frequency right, but the unusual Airwindows head bump algorithm that'll work just fine however you voice it?And Dubly Decode helps you get even smoother, more compressed sound out of the tape saturation, and lets you dial it in by both amount and frequency. Be aware that it's a really gentle treble-slope, so making subtle changes to frequency will have an effect on the character of the sound but won't produce wild effects. Basically, if Dubly mostly balances but Enc is crossing over lower than Dec, there will be a sort of lower midrange hype that comes through: if Dec is lower than Enc, it's going to be drying up that energy and making it a bit tighter. They're supposed to cancel out, but half the reason classic records all sound different from each other is that this system in real life required a lot of fussy tuning, and some people set it up by ear and ended up with distinct tone qualities for their studios when they did. It's not so much 'EQ' qualities as texture. Experiment with it to see if you have preferences.And that's ToTape7! Ought to hold people for a while, and be a decent upgrade from ToTape6, even though people really like that one (and it's still there so you can still have it anytime you want)."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class ToTape7 final : public Effect<T>
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
    double iirEncL;
    double iirEncR;
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

    double iirMidRollerL;
    double iirLowCutoffL;
    double iirMidRollerR;
    double iirLowCutoffR;

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

    double iirDecL;
    double iirDecR;

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
    ToTape7()
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
        iirEncL = 0.0;
        iirEncR = 0.0;
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
        iirMidRollerL = 0.0;
        iirLowCutoffL = 0.0;
        iirMidRollerR = 0.0;
        iirLowCutoffR = 0.0;
        headBumpL = 0.0;
        headBumpR = 0.0;
        for (int x = 0; x < hdb_total; x++) {
            hdbA[x] = 0.0;
            hdbB[x] = 0.0;
        }
        // from ZBandpass, so I can use enums with it
        iirDecL = 0.0;
        iirDecR = 0.0;
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
        kParamJ = 9,
        kNumParameters = 10
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
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
            case kParamA: return "encamt"; break;
            case kParamB: return "encfreq"; break;
            case kParamC: return "tapedrv"; break;
            case kParamD: return "flutter"; break;
            case kParamE: return "flutspd"; break;
            case kParamF: return "bias"; break;
            case kParamG: return "headbmp"; break;
            case kParamH: return "headfrq"; break;
            case kParamI: return "decamt"; break;
            case kParamJ: return "decfreq"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "EncAmt"; break;
            case kParamB: return "EncFreq"; break;
            case kParamC: return "TapeDrv"; break;
            case kParamD: return "Flutter"; break;
            case kParamE: return "FlutSpd"; break;
            case kParamF: return "Bias"; break;
            case kParamG: return "HeadBmp"; break;
            case kParamH: return "HeadFrq"; break;
            case kParamI: return "DecAmt"; break;
            case kParamJ: return "DecFreq"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return "hz"; break;
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
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double dublyAmount = pow(A, 3) * 0.105468;
        double iirEncFreq = B / overallscale;
        double iirMidFreq = ((B * 0.618) + 0.382) / overallscale;
        double inputGain = pow(C * 2.0, 2.0);
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
        double outlyAmount = pow(I, 3) * 0.109744;
        double iirDecFreq = J / overallscale;
        double subCurve = sin(G * M_PI);
        double iirSubFreq = (subCurve * 0.008) / overallscale;
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
            // begin Dubly encode
            double doubly = 0.0;
            if (dublyAmount > 0.0) {
                iirEncL = (iirEncL * (1.0 - iirEncFreq)) + (inputSampleL * iirEncFreq);
                double doubly = inputSampleL - iirEncL;
                if (doubly > 1.0) {
                    doubly = 1.0;
                }
                if (doubly < -1.0) {
                    doubly = -1.0;
                }
                if (doubly > 0) {
                    doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                if (doubly < 0) {
                    doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                inputSampleL += doubly * dublyAmount;
                iirEncR = (iirEncR * (1.0 - iirEncFreq)) + (inputSampleR * iirEncFreq);
                doubly = inputSampleR - iirEncR;
                if (doubly > 1.0) {
                    doubly = 1.0;
                }
                if (doubly < -1.0) {
                    doubly = -1.0;
                }
                if (doubly > 0) {
                    doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                if (doubly < 0) {
                    doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                inputSampleR += doubly * dublyAmount;
            }
            // end Dubly encode
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
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
            // toTape basic algorithm L
            iirMidRollerL = (iirMidRollerL * (1.0 - iirMidFreq)) + (inputSampleL * iirMidFreq);
            double HighsSampleL = inputSampleL - iirMidRollerL;
            double LowsSampleL = iirMidRollerL;
            if (iirSubFreq > 0.0) {
                iirLowCutoffL = (iirLowCutoffL * (1.0 - iirSubFreq)) + (LowsSampleL * iirSubFreq);
                LowsSampleL -= iirLowCutoffL;
            }
            if (LowsSampleL > 1.57079633) {
                LowsSampleL = 1.57079633;
            }
            if (LowsSampleL < -1.57079633) {
                LowsSampleL = -1.57079633;
            }
            LowsSampleL = sin(LowsSampleL);
            double thinnedHighSample = fabs(HighsSampleL) * 1.57079633;
            if (thinnedHighSample > 1.57079633) {
                thinnedHighSample = 1.57079633;
            }
            thinnedHighSample = 1.0 - cos(thinnedHighSample);
            if (HighsSampleL < 0) {
                thinnedHighSample = -thinnedHighSample;
            }
            HighsSampleL -= thinnedHighSample;
            // toTape basic algorithm R
            iirMidRollerR = (iirMidRollerR * (1.0 - iirMidFreq)) + (inputSampleR * iirMidFreq);
            double HighsSampleR = inputSampleR - iirMidRollerR;
            double LowsSampleR = iirMidRollerR;
            if (iirSubFreq > 0.0) {
                iirLowCutoffR = (iirLowCutoffR * (1.0 - iirSubFreq)) + (LowsSampleR * iirSubFreq);
                LowsSampleR -= iirLowCutoffR;
            }
            if (LowsSampleR > 1.57079633) {
                LowsSampleR = 1.57079633;
            }
            if (LowsSampleR < -1.57079633) {
                LowsSampleR = -1.57079633;
            }
            LowsSampleR = sin(LowsSampleR);
            thinnedHighSample = fabs(HighsSampleR) * 1.57079633;
            if (thinnedHighSample > 1.57079633) {
                thinnedHighSample = 1.57079633;
            }
            thinnedHighSample = 1.0 - cos(thinnedHighSample);
            if (HighsSampleR < 0) {
                thinnedHighSample = -thinnedHighSample;
            }
            HighsSampleR -= thinnedHighSample;
            // begin HeadBump
            double headBumpSampleL = 0.0;
            double headBumpSampleR = 0.0;
            if (headBumpMix > 0.0) {
                headBumpL += (LowsSampleL * headBumpDrive);
                headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618 / sqrt(overallscale)));
                headBumpR += (LowsSampleR * headBumpDrive);
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
            inputSampleL = LowsSampleL + HighsSampleL + (headBumpSampleL * headBumpMix);
            inputSampleR = LowsSampleR + HighsSampleR + (headBumpSampleR * headBumpMix);
            // begin Dubly decode
            if (outlyAmount > 0.0) {
                iirDecL = (iirDecL * (1.0 - iirDecFreq)) + (inputSampleL * iirDecFreq);
                doubly = inputSampleL - iirDecL;
                if (doubly > 1.0) {
                    doubly = 1.0;
                }
                if (doubly < -1.0) {
                    doubly = -1.0;
                }
                if (doubly > 0) {
                    doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                if (doubly < 0) {
                    doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                inputSampleL -= doubly * outlyAmount;
                iirDecR = (iirDecR * (1.0 - iirDecFreq)) + (inputSampleR * iirDecFreq);
                doubly = inputSampleR - iirDecR;
                if (doubly > 1.0) {
                    doubly = 1.0;
                }
                if (doubly < -1.0) {
                    doubly = -1.0;
                }
                if (doubly > 0) {
                    doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                if (doubly < 0) {
                    doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
                }
                inputSampleR -= doubly * outlyAmount;
            }
            // end Dubly decode
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::totape7
