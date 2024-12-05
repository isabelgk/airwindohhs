#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pressure5 {

constexpr std::string_view k_name{ "Pressure5" };
constexpr std::string_view k_short_description{
    "Pressure5 expands Pressure4 with after-boost and built-in ClipOnly2."
};
constexpr std::string_view k_long_description{
    "This plugin builds on 2017’s Pressure4 in numerous ways. I made it for my own use: I was mixing an album in Console7 that had to follow a previous album I’d mixed on an analog board, and I needed extensive 2-buss management and the ability to deliver a final output and control mix density across a lot of parameters measured by a meter I’ve invented (which isn’t available, it’s just for my personal use and doesn’t work properly in a releasable way). And now Pressure5 is out and you can have it!Pressure is a compressor with some unusual controls: there’s a ‘mewiness’ control that manages the way the ratio engages. You can dial in the intensity with which it ‘dynamic inverts’ and super-squishes the audio. It’s got a very wide speed range that can go real fast for a dense, distorted sort of sound, and it’s now got an additional control that nobody else has: ‘PawClaw’. This manages the way ‘mewiness’ handles transients, the intent being to extra-squish the spiky transients on ‘Paw’ or to let them through more on ‘Claw’. It’s very subtle, but it’s there to tailor the way stuff hits the compression in an entirely unique way. I don’t think anything else has a ‘PawClaw’ control, but now you have one :)The other part of Pressure5 is the output stage: Pressure5’s output control has the capacity to boost, into a built-in ClipOnly2. So it’s got that chunky, non-edgy hard clipping drive, that works at all sample rates, as part of the plugin. I intend Pressure5 to be used as a final buss comp/clip stage letting you dial in whatever you need, and then just taking the output and dithering it and being done. If you don’t want to go direct to final release, pad the output control so you’re not clipping anymore and ClipOnly2 will bypass itself like it’s not even there, serving as only a safety clipper."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pressure5 final : public Effect<T>
{
    double muVary;
    double muAttack;
    double muNewSpeed;
    double muSpeedA;
    double muSpeedB;
    double muCoefficientA;
    double muCoefficientB;
    bool flip; // Pressure
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_lastSampleL,
        fix_lastSampleR,
        fix_total
    };
    double fixA[fix_total];
    double fixB[fix_total]; // fixed frequency biquad filter for ultrasonics, stereo

    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2

    double slewMax; // to adust mewiness

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.
  public:
    Pressure5()
    {
        A = 0.0;
        B = 0.25;
        C = 1.0;
        D = 0.5;
        E = 0.5;
        F = 1.0;
        muSpeedA = 10000;
        muSpeedB = 10000;
        muCoefficientA = 1;
        muCoefficientB = 1;
        muVary = 1;
        flip = false;
        for (int x = 0; x < fix_total; x++) {
            fixA[x] = 0.0;
            fixB[x] = 0.0;
        }
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
        slewMax = 0.0;
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
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.25; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "pressre"; break;
            case kParamB: return "speed"; break;
            case kParamC: return "mewines"; break;
            case kParamD: return "pawclaw"; break;
            case kParamE: return "output"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Pressre"; break;
            case kParamB: return "Speed"; break;
            case kParamC: return "Mewines"; break;
            case kParamD: return "PawClaw"; break;
            case kParamE: return "Output"; break;
            case kParamF: return "Dry/Wet"; break;

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
        double threshold = 1.0 - (A * 0.95);
        double muMakeupGain = 1.0 / threshold;
        // gain settings around threshold
        double release = pow((1.28 - B), 5) * 32768.0;
        double fastest = sqrt(release);
        release /= overallscale;
        fastest /= overallscale;
        // speed settings around release
        double mewinessRef = C;
        double pawsClaws = -(D - 0.5) * 1.618033988749894848204586;
        // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
        double outputGain = pow(E * 2.0, 2); // max 4.0 gain
        double wet = F;
        fixA[fix_freq] = 24000.0 / Effect<T>::getSampleRate();
        fixA[fix_reso] = 0.7071; // butterworth Q
        double K = tan(M_PI * fixA[fix_freq]);
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        // for the fixed-position biquad filter
        for (int x = 0; x < fix_sL1; x++) {
            fixB[x] = fixA[x];
        }
        // make the second filter same as the first, don't use sample slots
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
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            inputSampleL = inputSampleL * muMakeupGain;
            inputSampleR = inputSampleR * muMakeupGain;
            if (fixA[fix_freq] < 0.4999) {
                double temp = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
                fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sL2];
                fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (temp * fixA[fix_b2]);
                inputSampleL = temp;
                temp = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
                fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (temp * fixA[fix_b1]) + fixA[fix_sR2];
                fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (temp * fixA[fix_b2]);
                inputSampleR = temp; // fixed biquad filtering ultrasonics before Pressure
            }
            double inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            // we will take the greater of either channel and just use that, then apply the result
            // to both stereo channels.
            double mewiness = sin(mewinessRef + (slewMax * pawsClaws));
            bool positivemu = true;
            if (mewiness < 0) {
                positivemu = false;
                mewiness = -mewiness;
            }
            if (flip)
            {
                if (inputSense > threshold)
                {
                    muVary = threshold / inputSense;
                    muAttack = sqrt(fabs(muSpeedA));
                    muCoefficientA = muCoefficientA * (muAttack - 1.0);
                    if (muVary < threshold)
                    {
                        muCoefficientA = muCoefficientA + threshold;
                    }
                    else
                    {
                        muCoefficientA = muCoefficientA + muVary;
                    }
                    muCoefficientA = muCoefficientA / muAttack;
                }
                else
                {
                    muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA) - 1.0);
                    muCoefficientA = muCoefficientA + 1.0;
                    muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
                }
                muNewSpeed = muSpeedA * (muSpeedA - 1);
                muNewSpeed = muNewSpeed + fabs(inputSense * release) + fastest;
                muSpeedA = muNewSpeed / muSpeedA;
            }
            else
            {
                if (inputSense > threshold)
                {
                    muVary = threshold / inputSense;
                    muAttack = sqrt(fabs(muSpeedB));
                    muCoefficientB = muCoefficientB * (muAttack - 1);
                    if (muVary < threshold)
                    {
                        muCoefficientB = muCoefficientB + threshold;
                    }
                    else
                    {
                        muCoefficientB = muCoefficientB + muVary;
                    }
                    muCoefficientB = muCoefficientB / muAttack;
                }
                else
                {
                    muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB) - 1.0);
                    muCoefficientB = muCoefficientB + 1.0;
                    muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
                }
                muNewSpeed = muSpeedB * (muSpeedB - 1);
                muNewSpeed = muNewSpeed + fabs(inputSense * release) + fastest;
                muSpeedB = muNewSpeed / muSpeedB;
            }
            // got coefficients, adjusted speeds
            double coefficient;
            if (flip) {
                if (positivemu) {
                    coefficient = pow(muCoefficientA, 2);
                }
                else {
                    coefficient = sqrt(muCoefficientA);
                }
                coefficient = (coefficient * mewiness) + (muCoefficientA * (1.0 - mewiness));
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            else {
                if (positivemu) {
                    coefficient = pow(muCoefficientB, 2);
                }
                else {
                    coefficient = sqrt(muCoefficientB);
                }
                coefficient = (coefficient * mewiness) + (muCoefficientB * (1.0 - mewiness));
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            // applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
            // applied gain correction to control output level- tends to constrain sound rather than inflate it
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
            }
            flip = !flip;
            if (fixB[fix_freq] < 0.49999) {
                double temp = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
                fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sL2];
                fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (temp * fixB[fix_b2]);
                inputSampleL = temp;
                temp = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
                fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (temp * fixB[fix_b1]) + fixB[fix_sR2];
                fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (temp * fixB[fix_b2]);
                inputSampleR = temp; // fixed biquad filtering ultrasonics between Pressure and ClipOnly
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, BEFORE ClipOnly
            slewMax = fabs(inputSampleL - lastSampleL);
            if (slewMax < fabs(inputSampleR - lastSampleR)) {
                slewMax = fabs(inputSampleR - lastSampleR);
            }
            // set up for fiddling with mewiness. Largest amount of slew in any direction
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
            // final clip runs AFTER the Dry/Wet. It serves as a safety clip even if you're not full wet
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
} // namespace airwindohhs::pressure5
