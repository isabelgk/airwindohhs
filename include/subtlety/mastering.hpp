#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::mastering {

constexpr std::string_view k_name{ "Mastering" };
constexpr std::string_view k_short_description{
    "Mastering is Airwindows style, and can do things nothing else can!"
};
constexpr std::string_view k_long_description{
    "This plugin was a wild beast when I started working on it: it's now tamed to the point where I don't think you can do lots of harm by accident with it. It began as FOUR bands of Kalman filtering complete with the treble management available in Air4, and I had to run one of the bands backwards to make it work (as far as the direction you moved the control), and it constantly tried to do huge tone-obliterating things with the tiniest movements of the bands against each other.Soon sorted THAT out. Meet Airwindows Mastering. You're meant to use it with Meter, to adjust stuff in the 'peak clouds' that wouldn't be immediately obvious to the ear. This isn't 'an EQ and a limiter/clipper/etc'.The top control is Glue. It defaults to zero. You can turn it up to handle the most brutal unpleasant super-highs, but it's meant to be used around the middle, where it will moderate the 'red spikes' high treble can give you, controlling them without entirely making them go away. The idea is that you still want variation in those, rather than just 'clipping' them all to one point.Scope is like Air4, or the air band on ConsoleX. It's named that, because it's how you control the amount of super-fine detail on things. If you crank this, everything will seem like it's under a micro-Scope. In case you don't want that, you can safely turn it down to retain 'highs' but tame just the glitter. This works in conjunction with Glue.Skronk and Girth are sort of like 'tone' controls, except they are relative to the lower sub-lows band, also done with Kalman filters, which moves separately in conjunction with these. The way it works, if you boost Girth stuff gets way bigger and fuller, to where it'll immediately start filling your peaks region with bassy information. If you cut it, it doesn't seem to remove the bass, but the whole texture of it changes and becomes more leaned out and sparse, without in any way lessening bass impact. Girth is specifically about the sensation of over-fatness or lack of it. Skronk is that, but for aggressive high mids. Cut Skronk and things get a lot more polite. Boost it, and the thickness steps back a bit to allow the aggression to step out. This is not really about frequencies, it's about the interactions with the Kalman filters which don't just stick to certain frequencies (get too lively with these and you'll hear an unusual grind to the sound that's probably best avoided: middle settings are best)And you might think you know what Drive is, but NOPE. In fact, it started out as Airwindows 'Zoom', and then stopped even being anything like that. Drive ended up being a simplified, cleaner form of Zoom that governs, not an output stage, but a trim amount for the individual Zoom controls on EACH other control. Namely, Scope, Skronk and Girth (the subs are left clean as they are what Girth hinges off).So the adjustments to these three controls also affect the subtler forms of Zoom on them, and Drive lets you offset each of those a little more, to finetune the effect. It will also end up boosting, or cutting, the overall level a bit. However, bear in mind this ties in with the three controls above it, so if things are getting over-dense, over-girthy, if Skronk is getting too aggressive, you can use Drive to pull everything back a little. If you're getting a sound, but it's just slamming things too hard, you can dial in the optimal 'peak cloud' keeping everything pretty well maxed but not as just a featureless line of smashed audio.This is not for loudenating. This is for getting your PEAKS to slam in exactly the optimum way, which in fact will make your stuff hit harder relative to loudenated stuff that has been normalized down to matched volume. In a replay gain situation you can do some damage with this if you know what you're doing, and if Meter is showing you where every peak is getting to, and whether they're bassy or loud or bright.Lastly, Dither lets you bypass (for instance, if you're using a later dither to do 16 bit CD audio: this only does 24 bit dither for modern day work), or use Dark (same as Monitoring2), or Ten Nines (same as Monitoring3), or TPDFWide, PaulWide, or Not Just Another Dither (same as the original Monitoring). It's more or less in order of 'liveliness', and since it's 24 bit it's entirely subliminal and you can't really do any harm with it. If you've got a favorite, choose that, or try to get a sense of what suits a given project or track. Other adjustments will have more effect, but you get to choose among Dark/TenNines/TPDFWide/PaulWide/NJAD/Bypass as inspiration for your other adjustments. It's like Ditherbox, but up to date. If you don't want to be fancy and you're producing 24 bit output, just use TPDFWide, which is regulation TPDF that just happens to resist picking random values that act like it's mono dither :)This is not mastering as you know it, but it's Airwindows Mastering. I'm confident this will work for me (I won't be trying to master over headphones in real world use, though!) and if you were interested in getting things bent to your will in Airwindows Meter, you'll find this surprisingly effective. Have fun!"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Mastering final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    enum
    {
        pvAL1,
        pvSL1,
        accSL1,
        acc2SL1,
        pvAL2,
        pvSL2,
        accSL2,
        acc2SL2,
        pvAL3,
        pvSL3,
        accSL3,
        pvAL4,
        pvSL4,
        gndavgL,
        outAL,
        gainAL,
        pvAR1,
        pvSR1,
        accSR1,
        acc2SR1,
        pvAR2,
        pvSR2,
        accSR2,
        acc2SR2,
        pvAR3,
        pvSR3,
        accSR3,
        pvAR4,
        pvSR4,
        gndavgR,
        outAR,
        gainAR,
        air_total
    };
    double air[air_total];
    enum
    {
        prevSampL1,
        prevSlewL1,
        accSlewL1,
        prevSampL2,
        prevSlewL2,
        accSlewL2,
        prevSampL3,
        prevSlewL3,
        accSlewL3,
        kalGainL,
        kalOutL,
        kalAvgL,
        prevSampR1,
        prevSlewR1,
        accSlewR1,
        prevSampR2,
        prevSlewR2,
        accSlewR2,
        prevSampR3,
        prevSlewR3,
        accSlewR3,
        kalGainR,
        kalOutR,
        kalAvgR,
        kal_total
    };
    double kalM[kal_total];
    double kalS[kal_total];
    long double lastSinewL;
    long double lastSinewR;
    // this is overkill, used to run both Zoom and Sinew stages as they are after
    // the summing in StoneFire, which sums three doubles to a long double.
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    int quantA;
    int quantB;
    float expectedSlew;
    float testA;
    float testB;
    double correction;
    double shapedSampleL;
    double shapedSampleR;
    double currentDither;
    double ditherL;
    double ditherR;
    bool cutbinsL;
    bool cutbinsR;
    int hotbinA;
    int hotbinB;
    double benfordize;
    double totalA;
    double totalB;
    double outputSample;
    int expon; // internal dither variables
    double NSOddL; // dither section!
    double NSEvenL;
    double prevShapeL;
    double NSOddR;
    double NSEvenR;
    double prevShapeR;
    bool flip; // VinylDither
    double darkSampleL[100];
    double darkSampleR[100]; // Dark
    double previousDitherL;
    double previousDitherR; // PaulWide
    double bynL[13], bynR[13];
    double noiseShapingL, noiseShapingR; // NJAD
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Mastering()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 1.0;
        for (int x = 0; x < air_total; x++) {
            air[x] = 0.0;
        }
        for (int x = 0; x < kal_total; x++) {
            kalM[x] = 0.0;
            kalS[x] = 0.0;
        }
        lastSinewL = 0.0;
        lastSinewR = 0.0;
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
        quantA = 0;
        quantB = 1;
        expectedSlew = 0.0;
        testA = 0.0;
        testB = 0.0;
        correction = 0.0;
        shapedSampleL = 0.0;
        shapedSampleR = 0.0;
        currentDither = 0.0;
        ditherL = 0.0;
        ditherR = 0.0;
        cutbinsL = false;
        cutbinsR = false;
        hotbinA = 0;
        hotbinB = 0;
        benfordize = 0.0;
        totalA = 0.0;
        totalB = 0.0;
        outputSample = 0.0;
        expon = 0; // internal dither variables
        // these didn't like to be defined inside a case statement
        NSOddL = 0.0;
        NSEvenL = 0.0;
        prevShapeL = 0.0;
        NSOddR = 0.0;
        NSEvenR = 0.0;
        prevShapeR = 0.0;
        flip = true; // Ten Nines
        for (int count = 0; count < 99; count++) {
            darkSampleL[count] = 0;
            darkSampleR[count] = 0;
        } // Dark
        previousDitherL = 0.0;
        previousDitherR = 0.0; // PaulWide
        bynL[0] = 1000.0;
        bynL[1] = 301.0;
        bynL[2] = 176.0;
        bynL[3] = 125.0;
        bynL[4] = 97.0;
        bynL[5] = 79.0;
        bynL[6] = 67.0;
        bynL[7] = 58.0;
        bynL[8] = 51.0;
        bynL[9] = 46.0;
        bynL[10] = 1000.0;
        noiseShapingL = 0.0;
        bynR[0] = 1000.0;
        bynR[1] = 301.0;
        bynR[2] = 176.0;
        bynR[3] = 125.0;
        bynR[4] = 97.0;
        bynR[5] = 79.0;
        bynR[6] = 67.0;
        bynR[7] = 58.0;
        bynR[8] = 51.0;
        bynR[9] = 46.0;
        bynR[10] = 1000.0;
        noiseShapingR = 0.0; // NJAD
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
        kNumParameters = 6
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "glue"; break;
            case kParamB: return "scope"; break;
            case kParamC: return "skronk"; break;
            case kParamD: return "girth"; break;
            case kParamE: return "drive"; break;
            case kParamF: return "dither"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Glue"; break;
            case kParamB: return "Scope"; break;
            case kParamC: return "Skronk"; break;
            case kParamD: return "Girth"; break;
            case kParamE: return "Drive"; break;
            case kParamF: return "Dither"; break;

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
        double threshSinew = (0.25 + ((1.0 - A) * 0.333)) / overallscale;
        double depthSinew = 1.0 - pow(1.0 - A, 2.0);
        double trebleZoom = B - 0.5;
        long double trebleGain = (trebleZoom * fabs(trebleZoom)) + 1.0;
        if (trebleGain > 1.0) {
            trebleGain = pow(trebleGain, 3.0 + sqrt(overallscale));
        }
        // this boost is necessary to adapt to higher sample rates
        double midZoom = C - 0.5;
        long double midGain = (midZoom * fabs(midZoom)) + 1.0;
        double kalMid = 0.35 - (C * 0.25); // crossover frequency between mid/bass
        double kalSub = 0.45 + (C * 0.25); // crossover frequency between bass/sub
        double bassZoom = (D * 0.5) - 0.25;
        long double bassGain = (-bassZoom * fabs(bassZoom)) + 1.0; // control inverted
        long double subGain = ((D * 0.25) - 0.125) + 1.0;
        if (subGain < 1.0) {
            subGain = 1.0; // very small sub shift, only pos.
        }
        long double driveIn = (E - 0.5) + 1.0;
        long double driveOut = (-(E - 0.5) * fabs(E - 0.5)) + 1.0;
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        int dither = (int)(F * 5.999);
        int depth = (int)(17.0 * overallscale);
        if (depth < 3) {
            depth = 3;
        }
        if (depth > 98) {
            depth = 98; // for Dark
        }
        while (--sampleFrames >= 0) {
            long double inputSampleL = *in1;
            long double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= driveIn;
            inputSampleR *= driveIn;
            long double drySampleL = inputSampleL;
            long double drySampleR = inputSampleR;
            // begin Air3L
            air[pvSL4] = air[pvAL4] - air[pvAL3];
            air[pvSL3] = air[pvAL3] - air[pvAL2];
            air[pvSL2] = air[pvAL2] - air[pvAL1];
            air[pvSL1] = air[pvAL1] - inputSampleL;
            air[accSL3] = air[pvSL4] - air[pvSL3];
            air[accSL2] = air[pvSL3] - air[pvSL2];
            air[accSL1] = air[pvSL2] - air[pvSL1];
            air[acc2SL2] = air[accSL3] - air[accSL2];
            air[acc2SL1] = air[accSL2] - air[accSL1];
            air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1]) * 0.5));
            air[gainAL] *= 0.5;
            air[gainAL] += fabs(drySampleL - air[outAL]) * 0.5;
            if (air[gainAL] > 0.3 * sqrt(overallscale)) {
                air[gainAL] = 0.3 * sqrt(overallscale);
            }
            air[pvAL4] = air[pvAL3];
            air[pvAL3] = air[pvAL2];
            air[pvAL2] = air[pvAL1];
            air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
            long double midL = drySampleL - ((air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
            long double temp = (midL + air[gndavgL]) * 0.5;
            air[gndavgL] = midL;
            midL = temp;
            long double trebleL = drySampleL - midL;
            // end Air3L
            // begin Air3R
            air[pvSR4] = air[pvAR4] - air[pvAR3];
            air[pvSR3] = air[pvAR3] - air[pvAR2];
            air[pvSR2] = air[pvAR2] - air[pvAR1];
            air[pvSR1] = air[pvAR1] - inputSampleR;
            air[accSR3] = air[pvSR4] - air[pvSR3];
            air[accSR2] = air[pvSR3] - air[pvSR2];
            air[accSR1] = air[pvSR2] - air[pvSR1];
            air[acc2SR2] = air[accSR3] - air[accSR2];
            air[acc2SR1] = air[accSR2] - air[accSR1];
            air[outAR] = -(air[pvAR1] + air[pvSR3] + air[acc2SR2] - ((air[acc2SR2] + air[acc2SR1]) * 0.5));
            air[gainAR] *= 0.5;
            air[gainAR] += fabs(drySampleR - air[outAR]) * 0.5;
            if (air[gainAR] > 0.3 * sqrt(overallscale)) {
                air[gainAR] = 0.3 * sqrt(overallscale);
            }
            air[pvAR4] = air[pvAR3];
            air[pvAR3] = air[pvAR2];
            air[pvAR2] = air[pvAR1];
            air[pvAR1] = (air[gainAR] * air[outAR]) + drySampleR;
            long double midR = drySampleR - ((air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
            temp = (midR + air[gndavgR]) * 0.5;
            air[gndavgR] = midR;
            midR = temp;
            long double trebleR = drySampleR - midR;
            // end Air3R
            // begin KalmanML
            temp = midL;
            kalM[prevSlewL3] += kalM[prevSampL3] - kalM[prevSampL2];
            kalM[prevSlewL3] *= 0.5;
            kalM[prevSlewL2] += kalM[prevSampL2] - kalM[prevSampL1];
            kalM[prevSlewL2] *= 0.5;
            kalM[prevSlewL1] += kalM[prevSampL1] - midL;
            kalM[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kalM[accSlewL2] += kalM[prevSlewL3] - kalM[prevSlewL2];
            kalM[accSlewL2] *= 0.5;
            kalM[accSlewL1] += kalM[prevSlewL2] - kalM[prevSlewL1];
            kalM[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kalM[accSlewL3] += (kalM[accSlewL2] - kalM[accSlewL1]);
            kalM[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kalM[kalOutL] += kalM[prevSampL1] + kalM[prevSlewL2] + kalM[accSlewL3];
            kalM[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kalM[kalGainL] += fabs(temp - kalM[kalOutL]) * kalMid * 8.0;
            kalM[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kalM[kalGainL] > kalMid * 0.5) {
                kalM[kalGainL] = kalMid * 0.5;
            }
            // attempts to avoid explosions
            kalM[kalOutL] += (temp * (1.0 - (0.68 + (kalMid * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kalM[prevSampL3] = kalM[prevSampL2];
            kalM[prevSampL2] = kalM[prevSampL1];
            kalM[prevSampL1] = (kalM[kalGainL] * kalM[kalOutL]) + ((1.0 - kalM[kalGainL]) * temp);
            // feed the chain of previous samples
            long double bassL = (kalM[kalOutL] + kalM[kalAvgL]) * 0.5;
            kalM[kalAvgL] = kalM[kalOutL];
            midL -= bassL;
            // end KalmanML
            // begin KalmanMR
            temp = midR;
            kalM[prevSlewR3] += kalM[prevSampR3] - kalM[prevSampR2];
            kalM[prevSlewR3] *= 0.5;
            kalM[prevSlewR2] += kalM[prevSampR2] - kalM[prevSampR1];
            kalM[prevSlewR2] *= 0.5;
            kalM[prevSlewR1] += kalM[prevSampR1] - midR;
            kalM[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kalM[accSlewR2] += kalM[prevSlewR3] - kalM[prevSlewR2];
            kalM[accSlewR2] *= 0.5;
            kalM[accSlewR1] += kalM[prevSlewR2] - kalM[prevSlewR1];
            kalM[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kalM[accSlewR3] += (kalM[accSlewR2] - kalM[accSlewR1]);
            kalM[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kalM[kalOutR] += kalM[prevSampR1] + kalM[prevSlewR2] + kalM[accSlewR3];
            kalM[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kalM[kalGainR] += fabs(temp - kalM[kalOutR]) * kalMid * 8.0;
            kalM[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kalM[kalGainR] > kalMid * 0.5) {
                kalM[kalGainR] = kalMid * 0.5;
            }
            // attempts to avoid explosions
            kalM[kalOutR] += (temp * (1.0 - (0.68 + (kalMid * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kalM[prevSampR3] = kalM[prevSampR2];
            kalM[prevSampR2] = kalM[prevSampR1];
            kalM[prevSampR1] = (kalM[kalGainR] * kalM[kalOutR]) + ((1.0 - kalM[kalGainR]) * temp);
            // feed the chain of previous samples
            long double bassR = (kalM[kalOutR] + kalM[kalAvgR]) * 0.5;
            kalM[kalAvgR] = kalM[kalOutR];
            midR -= bassR;
            // end KalmanMR
            // begin KalmanSL
            temp = bassL;
            kalS[prevSlewL3] += kalS[prevSampL3] - kalS[prevSampL2];
            kalS[prevSlewL3] *= 0.5;
            kalS[prevSlewL2] += kalS[prevSampL2] - kalS[prevSampL1];
            kalS[prevSlewL2] *= 0.5;
            kalS[prevSlewL1] += kalS[prevSampL1] - bassL;
            kalS[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kalS[accSlewL2] += kalS[prevSlewL3] - kalS[prevSlewL2];
            kalS[accSlewL2] *= 0.5;
            kalS[accSlewL1] += kalS[prevSlewL2] - kalS[prevSlewL1];
            kalS[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kalS[accSlewL3] += (kalS[accSlewL2] - kalS[accSlewL1]);
            kalS[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kalS[kalOutL] += kalS[prevSampL1] + kalS[prevSlewL2] + kalS[accSlewL3];
            kalS[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kalS[kalGainL] += fabs(temp - kalS[kalOutL]) * kalSub * 8.0;
            kalS[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kalS[kalGainL] > kalSub * 0.5) {
                kalS[kalGainL] = kalSub * 0.5;
            }
            // attempts to avoid explosions
            kalS[kalOutL] += (temp * (1.0 - (0.68 + (kalSub * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kalS[prevSampL3] = kalS[prevSampL2];
            kalS[prevSampL2] = kalS[prevSampL1];
            kalS[prevSampL1] = (kalS[kalGainL] * kalS[kalOutL]) + ((1.0 - kalS[kalGainL]) * temp);
            // feed the chain of previous samples
            long double subL = (kalS[kalOutL] + kalS[kalAvgL]) * 0.5;
            kalS[kalAvgL] = kalS[kalOutL];
            bassL -= subL;
            // end KalmanSL
            // begin KalmanSR
            temp = bassR;
            kalS[prevSlewR3] += kalS[prevSampR3] - kalS[prevSampR2];
            kalS[prevSlewR3] *= 0.5;
            kalS[prevSlewR2] += kalS[prevSampR2] - kalS[prevSampR1];
            kalS[prevSlewR2] *= 0.5;
            kalS[prevSlewR1] += kalS[prevSampR1] - bassR;
            kalS[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kalS[accSlewR2] += kalS[prevSlewR3] - kalS[prevSlewR2];
            kalS[accSlewR2] *= 0.5;
            kalS[accSlewR1] += kalS[prevSlewR2] - kalS[prevSlewR1];
            kalS[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kalS[accSlewR3] += (kalS[accSlewR2] - kalS[accSlewR1]);
            kalS[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kalS[kalOutR] += kalS[prevSampR1] + kalS[prevSlewR2] + kalS[accSlewR3];
            kalS[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kalS[kalGainR] += fabs(temp - kalS[kalOutR]) * kalSub * 8.0;
            kalS[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kalS[kalGainR] > kalSub * 0.5) {
                kalS[kalGainR] = kalSub * 0.5;
            }
            // attempts to avoid explosions
            kalS[kalOutR] += (temp * (1.0 - (0.68 + (kalSub * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kalS[prevSampR3] = kalS[prevSampR2];
            kalS[prevSampR2] = kalS[prevSampR1];
            kalS[prevSampR1] = (kalS[kalGainR] * kalS[kalOutR]) + ((1.0 - kalS[kalGainR]) * temp);
            // feed the chain of previous samples
            long double subR = (kalS[kalOutR] + kalS[kalAvgR]) * 0.5;
            kalS[kalAvgR] = kalS[kalOutR];
            bassR -= subR;
            // end KalmanSR
            inputSampleL = (subL * subGain);
            inputSampleR = (subR * subGain);
            if (bassZoom > 0.0) {
                double closer = bassL * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                bassL = (bassL * (1.0 - bassZoom)) + (sin(closer) * bassZoom);
                closer = bassR * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                bassR = (bassR * (1.0 - bassZoom)) + (sin(closer) * bassZoom);
            } // zooming in will make the body of the sound louder: it's just Density
            if (bassZoom < 0.0) {
                double farther = fabs(bassL) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (bassL > 0.0) {
                    bassL = (bassL * (1.0 + bassZoom)) - (farther * bassZoom * 1.57079633);
                }
                if (bassL < 0.0) {
                    bassL = (bassL * (1.0 + bassZoom)) + (farther * bassZoom * 1.57079633);
                }
                farther = fabs(bassR) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (bassR > 0.0) {
                    bassR = (bassR * (1.0 + bassZoom)) - (farther * bassZoom * 1.57079633);
                }
                if (bassR < 0.0) {
                    bassR = (bassR * (1.0 + bassZoom)) + (farther * bassZoom * 1.57079633);
                }
            } // zooming out boosts the hottest peaks but cuts back softer stuff
            inputSampleL += (bassL * bassGain);
            inputSampleR += (bassR * bassGain);
            if (midZoom > 0.0) {
                double closer = midL * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                midL = (midL * (1.0 - midZoom)) + (sin(closer) * midZoom);
                closer = midR * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                midR = (midR * (1.0 - midZoom)) + (sin(closer) * midZoom);
            } // zooming in will make the body of the sound louder: it's just Density
            if (midZoom < 0.0) {
                double farther = fabs(midL) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (midL > 0.0) {
                    midL = (midL * (1.0 + midZoom)) - (farther * midZoom * 1.57079633);
                }
                if (midL < 0.0) {
                    midL = (midL * (1.0 + midZoom)) + (farther * midZoom * 1.57079633);
                }
                farther = fabs(midR) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (midR > 0.0) {
                    midR = (midR * (1.0 + midZoom)) - (farther * midZoom * 1.57079633);
                }
                if (midR < 0.0) {
                    midR = (midR * (1.0 + midZoom)) + (farther * midZoom * 1.57079633);
                }
            } // zooming out boosts the hottest peaks but cuts back softer stuff
            inputSampleL += (midL * midGain);
            inputSampleR += (midR * midGain);
            if (trebleZoom > 0.0) {
                double closer = trebleL * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                trebleL = (trebleL * (1.0 - trebleZoom)) + (sin(closer) * trebleZoom);
                closer = trebleR * 1.57079633;
                if (closer > 1.57079633) {
                    closer = 1.57079633;
                }
                if (closer < -1.57079633) {
                    closer = -1.57079633;
                }
                trebleR = (trebleR * (1.0 - trebleZoom)) + (sin(closer) * trebleZoom);
            } // zooming in will make the body of the sound louder: it's just Density
            if (trebleZoom < 0.0) {
                double farther = fabs(trebleL) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (trebleL > 0.0) {
                    trebleL = (trebleL * (1.0 + trebleZoom)) - (farther * trebleZoom * 1.57079633);
                }
                if (trebleL < 0.0) {
                    trebleL = (trebleL * (1.0 + trebleZoom)) + (farther * trebleZoom * 1.57079633);
                }
                farther = fabs(trebleR) * 1.57079633;
                if (farther > 1.57079633) {
                    farther = 1.0;
                }
                else {
                    farther = 1.0 - cos(farther);
                }
                if (trebleR > 0.0) {
                    trebleR = (trebleR * (1.0 + trebleZoom)) - (farther * trebleZoom * 1.57079633);
                }
                if (trebleR < 0.0) {
                    trebleR = (trebleR * (1.0 + trebleZoom)) + (farther * trebleZoom * 1.57079633);
                }
            } // zooming out boosts the hottest peaks but cuts back softer stuff
            inputSampleL += (trebleL * trebleGain);
            inputSampleR += (trebleR * trebleGain);
            inputSampleL *= driveOut;
            inputSampleR *= driveOut;
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
            temp = inputSampleL;
            long double sinew = threshSinew * cos(lastSinewL * lastSinewL);
            if (inputSampleL - lastSinewL > sinew) {
                temp = lastSinewL + sinew;
            }
            if (-(inputSampleL - lastSinewL) > sinew) {
                temp = lastSinewL - sinew;
            }
            lastSinewL = temp;
            inputSampleL = (inputSampleL * (1.0 - depthSinew)) + (lastSinewL * depthSinew);
            temp = inputSampleR;
            sinew = threshSinew * cos(lastSinewR * lastSinewR);
            if (inputSampleR - lastSinewR > sinew) {
                temp = lastSinewR + sinew;
            }
            if (-(inputSampleR - lastSinewR) > sinew) {
                temp = lastSinewR - sinew;
            }
            lastSinewR = temp;
            inputSampleR = (inputSampleR * (1.0 - depthSinew)) + (lastSinewR * depthSinew);
            // run Sinew to stop excess slews, but run a dry/wet to allow a range of brights
            switch (dither) {
                case 1:
                    // begin Dark
                    inputSampleL *= 8388608.0;
                    inputSampleR *= 8388608.0; // we will apply the 24 bit Dark
                    // We are doing it first Left, then Right, because the loops may run faster if
                    // they aren't too jammed full of variables. This means re-running code.
                    // begin left
                    quantA = floor(inputSampleL);
                    quantB = floor(inputSampleL + 1.0);
                    // to do this style of dither, we quantize in either direction and then
                    // do a reconstruction of what the result will be for each choice.
                    // We then evaluate which one we like, and keep a history of what we previously had
                    expectedSlew = 0;
                    for (int x = 0; x < depth; x++) {
                        expectedSlew += (darkSampleL[x + 1] - darkSampleL[x]);
                    }
                    expectedSlew /= depth; // we have an average of all recent slews
                    // we are doing that to voice the thing down into the upper mids a bit
                    // it mustn't just soften the brightest treble, it must smooth high mids too
                    testA = fabs((darkSampleL[0] - quantA) - expectedSlew);
                    testB = fabs((darkSampleL[0] - quantB) - expectedSlew);
                    if (testA < testB) {
                        inputSampleL = quantA;
                    }
                    else {
                        inputSampleL = quantB;
                    }
                    // select whichever one departs LEAST from the vector of averaged
                    // reconstructed previous final samples. This will force a kind of dithering
                    // as it'll make the output end up as smooth as possible
                    for (int x = depth; x >= 0; x--) {
                        darkSampleL[x + 1] = darkSampleL[x];
                    }
                    darkSampleL[0] = inputSampleL;
                    // end Dark left
                    // begin right
                    quantA = floor(inputSampleR);
                    quantB = floor(inputSampleR + 1.0);
                    // to do this style of dither, we quantize in either direction and then
                    // do a reconstruction of what the result will be for each choice.
                    // We then evaluate which one we like, and keep a history of what we previously had
                    expectedSlew = 0;
                    for (int x = 0; x < depth; x++) {
                        expectedSlew += (darkSampleR[x + 1] - darkSampleR[x]);
                    }
                    expectedSlew /= depth; // we have an average of all recent slews
                    // we are doing that to voice the thing down into the upper mids a bit
                    // it mustn't just soften the brightest treble, it must smooth high mids too
                    testA = fabs((darkSampleR[0] - quantA) - expectedSlew);
                    testB = fabs((darkSampleR[0] - quantB) - expectedSlew);
                    if (testA < testB) {
                        inputSampleR = quantA;
                    }
                    else {
                        inputSampleR = quantB;
                    }
                    // select whichever one departs LEAST from the vector of averaged
                    // reconstructed previous final samples. This will force a kind of dithering
                    // as it'll make the output end up as smooth as possible
                    for (int x = depth; x >= 0; x--) {
                        darkSampleR[x + 1] = darkSampleR[x];
                    }
                    darkSampleR[0] = inputSampleR;
                    // end Dark right
                    inputSampleL /= 8388608.0;
                    inputSampleR /= 8388608.0;
                    break; // Dark (Monitoring2)
                case 2:
                    // begin Dark	 for Ten Nines
                    inputSampleL *= 8388608.0;
                    inputSampleR *= 8388608.0; // we will apply the 24 bit Dark
                    // We are doing it first Left, then Right, because the loops may run faster if
                    // they aren't too jammed full of variables. This means re-running code.
                    // begin L
                    correction = 0;
                    if (flip) {
                        NSOddL = (NSOddL * 0.9999999999) + prevShapeL;
                        NSEvenL = (NSEvenL * 0.9999999999) - prevShapeL;
                        correction = NSOddL;
                    }
                    else {
                        NSOddL = (NSOddL * 0.9999999999) - prevShapeL;
                        NSEvenL = (NSEvenL * 0.9999999999) + prevShapeL;
                        correction = NSEvenL;
                    }
                    shapedSampleL = inputSampleL + correction;
                    // end Ten Nines L
                    // begin Dark L
                    quantA = floor(shapedSampleL);
                    quantB = floor(shapedSampleL + 1.0);
                    // to do this style of dither, we quantize in either direction and then
                    // do a reconstruction of what the result will be for each choice.
                    // We then evaluate which one we like, and keep a history of what we previously had
                    expectedSlew = 0;
                    for (int x = 0; x < depth; x++) {
                        expectedSlew += (darkSampleL[x + 1] - darkSampleL[x]);
                    }
                    expectedSlew /= depth; // we have an average of all recent slews
                    // we are doing that to voice the thing down into the upper mids a bit
                    // it mustn't just soften the brightest treble, it must smooth high mids too
                    testA = fabs((darkSampleL[0] - quantA) - expectedSlew);
                    testB = fabs((darkSampleL[0] - quantB) - expectedSlew);
                    if (testA < testB) {
                        inputSampleL = quantA;
                    }
                    else {
                        inputSampleL = quantB;
                    }
                    // select whichever one departs LEAST from the vector of averaged
                    // reconstructed previous final samples. This will force a kind of dithering
                    // as it'll make the output end up as smooth as possible
                    for (int x = depth; x >= 0; x--) {
                        darkSampleL[x + 1] = darkSampleL[x];
                    }
                    darkSampleL[0] = inputSampleL;
                    // end Dark L
                    prevShapeL = (floor(shapedSampleL) - inputSampleL) * 0.9999999999;
                    // end Ten Nines L
                    // begin R
                    correction = 0;
                    if (flip) {
                        NSOddR = (NSOddR * 0.9999999999) + prevShapeR;
                        NSEvenR = (NSEvenR * 0.9999999999) - prevShapeR;
                        correction = NSOddR;
                    }
                    else {
                        NSOddR = (NSOddR * 0.9999999999) - prevShapeR;
                        NSEvenR = (NSEvenR * 0.9999999999) + prevShapeR;
                        correction = NSEvenR;
                    }
                    shapedSampleR = inputSampleR + correction;
                    // end Ten Nines R
                    // begin Dark R
                    quantA = floor(shapedSampleR);
                    quantB = floor(shapedSampleR + 1.0);
                    // to do this style of dither, we quantize in either direction and then
                    // do a reconstruction of what the result will be for each choice.
                    // We then evaluate which one we like, and keep a history of what we previously had
                    expectedSlew = 0;
                    for (int x = 0; x < depth; x++) {
                        expectedSlew += (darkSampleR[x + 1] - darkSampleR[x]);
                    }
                    expectedSlew /= depth; // we have an average of all recent slews
                    // we are doing that to voice the thing down into the upper mids a bit
                    // it mustn't just soften the brightest treble, it must smooth high mids too
                    testA = fabs((darkSampleR[0] - quantA) - expectedSlew);
                    testB = fabs((darkSampleR[0] - quantB) - expectedSlew);
                    if (testA < testB) {
                        inputSampleR = quantA;
                    }
                    else {
                        inputSampleR = quantB;
                    }
                    // select whichever one departs LEAST from the vector of averaged
                    // reconstructed previous final samples. This will force a kind of dithering
                    // as it'll make the output end up as smooth as possible
                    for (int x = depth; x >= 0; x--) {
                        darkSampleR[x + 1] = darkSampleR[x];
                    }
                    darkSampleR[0] = inputSampleR;
                    // end Dark R
                    prevShapeR = (floor(shapedSampleR) - inputSampleR) * 0.9999999999;
                    // end Ten Nines
                    flip = !flip;
                    inputSampleL /= 8388608.0;
                    inputSampleR /= 8388608.0;
                    break; // Ten Nines (which goes into Dark in Monitoring3)
                case 3:
                    inputSampleL *= 8388608.0;
                    inputSampleR *= 8388608.0;
                    ditherL = -1.0;
                    ditherL += (double(fpdL) / UINT32_MAX);
                    fpdL ^= fpdL << 13;
                    fpdL ^= fpdL >> 17;
                    fpdL ^= fpdL << 5;
                    ditherL += (double(fpdL) / UINT32_MAX);
                    // TPDF: two 0-1 random noises
                    ditherR = -1.0;
                    ditherR += (double(fpdR) / UINT32_MAX);
                    fpdR ^= fpdR << 13;
                    fpdR ^= fpdR >> 17;
                    fpdR ^= fpdR << 5;
                    ditherR += (double(fpdR) / UINT32_MAX);
                    // TPDF: two 0-1 random noises
                    if (fabs(ditherL - ditherR) < 0.5) {
                        ditherL = -1.0;
                        ditherL += (double(fpdL) / UINT32_MAX);
                        fpdL ^= fpdL << 13;
                        fpdL ^= fpdL >> 17;
                        fpdL ^= fpdL << 5;
                        ditherL += (double(fpdL) / UINT32_MAX);
                    }
                    if (fabs(ditherL - ditherR) < 0.5) {
                        ditherR = -1.0;
                        ditherR += (double(fpdR) / UINT32_MAX);
                        fpdR ^= fpdR << 13;
                        fpdR ^= fpdR >> 17;
                        fpdR ^= fpdR << 5;
                        ditherR += (double(fpdR) / UINT32_MAX);
                    }
                    if (fabs(ditherL - ditherR) < 0.5) {
                        ditherL = -1.0;
                        ditherL += (double(fpdL) / UINT32_MAX);
                        fpdL ^= fpdL << 13;
                        fpdL ^= fpdL >> 17;
                        fpdL ^= fpdL << 5;
                        ditherL += (double(fpdL) / UINT32_MAX);
                    }
                    inputSampleL = floor(inputSampleL + ditherL);
                    inputSampleR = floor(inputSampleR + ditherR);
                    inputSampleL /= 8388608.0;
                    inputSampleR /= 8388608.0;
                    break; // TPDFWide (a good neutral with the width enhancement)
                case 4:
                    inputSampleL *= 8388608.0;
                    inputSampleR *= 8388608.0;
                    // Paul Frindle: It's true that the dither itself can sound different
                    // if it's given a different freq response and you get to hear it.
                    // The one we use most is triangular single pole high pass dither.
                    // It's not freq bent enough to sound odd, but is slightly less audible than
                    // flat dither. It can also be easily made by taking one sample of dither
                    // away from the previous one - this gives you the triangular PDF and the
                    // filtering in one go :-)
                    currentDither = (double(fpdL) / UINT32_MAX);
                    ditherL = currentDither;
                    ditherL -= previousDitherL;
                    previousDitherL = currentDither;
                    // TPDF: two 0-1 random noises
                    currentDither = (double(fpdR) / UINT32_MAX);
                    ditherR = currentDither;
                    ditherR -= previousDitherR;
                    previousDitherR = currentDither;
                    // TPDF: two 0-1 random noises
                    if (fabs(ditherL - ditherR) < 0.5) {
                        fpdL ^= fpdL << 13;
                        fpdL ^= fpdL >> 17;
                        fpdL ^= fpdL << 5;
                        currentDither = (double(fpdL) / UINT32_MAX);
                        ditherL = currentDither;
                        ditherL -= previousDitherL;
                        previousDitherL = currentDither;
                    }
                    if (fabs(ditherL - ditherR) < 0.5) {
                        fpdR ^= fpdR << 13;
                        fpdR ^= fpdR >> 17;
                        fpdR ^= fpdR << 5;
                        currentDither = (double(fpdR) / UINT32_MAX);
                        ditherR = currentDither;
                        ditherR -= previousDitherR;
                        previousDitherR = currentDither;
                    }
                    if (fabs(ditherL - ditherR) < 0.5) {
                        fpdL ^= fpdL << 13;
                        fpdL ^= fpdL >> 17;
                        fpdL ^= fpdL << 5;
                        currentDither = (double(fpdL) / UINT32_MAX);
                        ditherL = currentDither;
                        ditherL -= previousDitherL;
                        previousDitherL = currentDither;
                    }
                    inputSampleL = floor(inputSampleL + ditherL);
                    inputSampleR = floor(inputSampleR + ditherR);
                    inputSampleL /= 8388608.0;
                    inputSampleR /= 8388608.0;
                    break; // PaulWide (brighter neutral that's still TPDF and wide)
                case 5:
                    inputSampleL *= 8388608.0;
                    inputSampleR *= 8388608.0;
                    cutbinsL = false;
                    cutbinsR = false;
                    drySampleL = inputSampleL; // re-using in NJAD
                    inputSampleL -= noiseShapingL;
                    // NJAD L
                    benfordize = floor(inputSampleL);
                    while (benfordize >= 1.0) {
                        benfordize /= 10;
                    }
                    while (benfordize < 1.0 && benfordize > 0.0000001) {
                        benfordize *= 10;
                    }
                    hotbinA = floor(benfordize);
                    // hotbin becomes the Benford bin value for this number floored
                    totalA = 0.0;
                    if ((hotbinA > 0) && (hotbinA < 10)) {
                        bynL[hotbinA] += 1;
                        if (bynL[hotbinA] > 982) {
                            cutbinsL = true;
                        }
                        totalA += (301 - bynL[1]);
                        totalA += (176 - bynL[2]);
                        totalA += (125 - bynL[3]);
                        totalA += (97 - bynL[4]);
                        totalA += (79 - bynL[5]);
                        totalA += (67 - bynL[6]);
                        totalA += (58 - bynL[7]);
                        totalA += (51 - bynL[8]);
                        totalA += (46 - bynL[9]);
                        bynL[hotbinA] -= 1;
                    }
                    else {
                        hotbinA = 10;
                    }
                    // produce total number- smaller is closer to Benford real
                    benfordize = ceil(inputSampleL);
                    while (benfordize >= 1.0) {
                        benfordize /= 10;
                    }
                    while (benfordize < 1.0 && benfordize > 0.0000001) {
                        benfordize *= 10;
                    }
                    hotbinB = floor(benfordize);
                    // hotbin becomes the Benford bin value for this number ceiled
                    totalB = 0.0;
                    if ((hotbinB > 0) && (hotbinB < 10)) {
                        bynL[hotbinB] += 1;
                        if (bynL[hotbinB] > 982) {
                            cutbinsL = true;
                        }
                        totalB += (301 - bynL[1]);
                        totalB += (176 - bynL[2]);
                        totalB += (125 - bynL[3]);
                        totalB += (97 - bynL[4]);
                        totalB += (79 - bynL[5]);
                        totalB += (67 - bynL[6]);
                        totalB += (58 - bynL[7]);
                        totalB += (51 - bynL[8]);
                        totalB += (46 - bynL[9]);
                        bynL[hotbinB] -= 1;
                    }
                    else {
                        hotbinB = 10;
                    }
                    // produce total number- smaller is closer to Benford real
                    if (totalA < totalB) {
                        bynL[hotbinA] += 1;
                        outputSample = floor(inputSampleL);
                    }
                    else {
                        bynL[hotbinB] += 1;
                        outputSample = floor(inputSampleL + 1);
                    }
                    // assign the relevant one to the delay line
                    // and floor/ceil signal accordingly
                    if (cutbinsL) {
                        bynL[1] *= 0.99;
                        bynL[2] *= 0.99;
                        bynL[3] *= 0.99;
                        bynL[4] *= 0.99;
                        bynL[5] *= 0.99;
                        bynL[6] *= 0.99;
                        bynL[7] *= 0.99;
                        bynL[8] *= 0.99;
                        bynL[9] *= 0.99;
                        bynL[10] *= 0.99;
                    }
                    noiseShapingL += outputSample - drySampleL;
                    if (noiseShapingL > fabs(inputSampleL)) {
                        noiseShapingL = fabs(inputSampleL);
                    }
                    if (noiseShapingL < -fabs(inputSampleL)) {
                        noiseShapingL = -fabs(inputSampleL);
                    }
                    inputSampleL /= 8388608.0;
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    // finished NJAD L
                    // NJAD R
                    drySampleR = inputSampleR;
                    inputSampleR -= noiseShapingR;
                    benfordize = floor(inputSampleR);
                    while (benfordize >= 1.0) {
                        benfordize /= 10;
                    }
                    while (benfordize < 1.0 && benfordize > 0.0000001) {
                        benfordize *= 10;
                    }
                    hotbinA = floor(benfordize);
                    // hotbin becomes the Benford bin value for this number floored
                    totalA = 0.0;
                    if ((hotbinA > 0) && (hotbinA < 10)) {
                        bynR[hotbinA] += 1;
                        if (bynR[hotbinA] > 982) {
                            cutbinsR = true;
                        }
                        totalA += (301 - bynR[1]);
                        totalA += (176 - bynR[2]);
                        totalA += (125 - bynR[3]);
                        totalA += (97 - bynR[4]);
                        totalA += (79 - bynR[5]);
                        totalA += (67 - bynR[6]);
                        totalA += (58 - bynR[7]);
                        totalA += (51 - bynR[8]);
                        totalA += (46 - bynR[9]);
                        bynR[hotbinA] -= 1;
                    }
                    else {
                        hotbinA = 10;
                    }
                    // produce total number- smaller is closer to Benford real
                    benfordize = ceil(inputSampleR);
                    while (benfordize >= 1.0) {
                        benfordize /= 10;
                    }
                    while (benfordize < 1.0 && benfordize > 0.0000001) {
                        benfordize *= 10;
                    }
                    hotbinB = floor(benfordize);
                    // hotbin becomes the Benford bin value for this number ceiled
                    totalB = 0.0;
                    if ((hotbinB > 0) && (hotbinB < 10)) {
                        bynR[hotbinB] += 1;
                        if (bynR[hotbinB] > 982) {
                            cutbinsR = true;
                        }
                        totalB += (301 - bynR[1]);
                        totalB += (176 - bynR[2]);
                        totalB += (125 - bynR[3]);
                        totalB += (97 - bynR[4]);
                        totalB += (79 - bynR[5]);
                        totalB += (67 - bynR[6]);
                        totalB += (58 - bynR[7]);
                        totalB += (51 - bynR[8]);
                        totalB += (46 - bynR[9]);
                        bynR[hotbinB] -= 1;
                    }
                    else {
                        hotbinB = 10;
                    }
                    // produce total number- smaller is closer to Benford real
                    if (totalA < totalB) {
                        bynR[hotbinA] += 1;
                        outputSample = floor(inputSampleR);
                    }
                    else {
                        bynR[hotbinB] += 1;
                        outputSample = floor(inputSampleR + 1);
                    }
                    // assign the relevant one to the delay line
                    // and floor/ceil signal accordingly
                    if (cutbinsR) {
                        bynR[1] *= 0.99;
                        bynR[2] *= 0.99;
                        bynR[3] *= 0.99;
                        bynR[4] *= 0.99;
                        bynR[5] *= 0.99;
                        bynR[6] *= 0.99;
                        bynR[7] *= 0.99;
                        bynR[8] *= 0.99;
                        bynR[9] *= 0.99;
                        bynR[10] *= 0.99;
                    }
                    noiseShapingR += outputSample - drySampleR;
                    if (noiseShapingR > fabs(inputSampleR)) {
                        noiseShapingR = fabs(inputSampleR);
                    }
                    if (noiseShapingR < -fabs(inputSampleR)) {
                        noiseShapingR = -fabs(inputSampleR);
                    }
                    inputSampleR /= 8388608.0;
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    break; // NJAD (Monitoring. Brightest)
                case 6:
                    // begin 64 bit stereo floating point dither
                    frexp((double)inputSampleL, &expon);
                    fpdL ^= fpdL << 13;
                    fpdL ^= fpdL >> 17;
                    fpdL ^= fpdL << 5;
                    inputSampleL += ((double(fpdL) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
                    frexp((double)inputSampleR, &expon);
                    fpdR ^= fpdR << 13;
                    fpdR ^= fpdR >> 17;
                    fpdR ^= fpdR << 5;
                    inputSampleR += ((double(fpdR) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
                    // end 64 bit stereo floating point dither
                    break; // Bypass for saving floating point files directly
            }
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::mastering
