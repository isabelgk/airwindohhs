#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::fateq {

constexpr std::string_view k_name{ "FatEQ" };
constexpr std::string_view k_short_description{
    "FatEQ balances bands only with density."
};
constexpr std::string_view k_long_description{
    "This experiment has always worked, but I'm not sure people understand why they gravitate to it.Usually when we do an EQ, we take frequency bands and turn them up and down, uniformly. A normal sound will get louder, a loud one louder still. Or, a sound will get quieter, a quieter sound quieter still.FatEQ uses the functions I use in Density (including an upgrade to negative Density settings) so that, if you're boosting, a normal sound will get louder, but a loud one will softclip. If you're cutting, to a certain extent a normal sound will get quieter but a quieter sound will get LOTS quieter. It uses variations on sin() functions to mess with the transfer functions in these ways that are tied to what you're doing with the slider.It's easy to hear what this does by just turning all the bands up. It's super-dense, overdriven, a multiband distortion, and you can totally do that.But as you do it, notice: doesn't everything also seem closer? That's the heart of FatEQ, more than its ability to just crank and overdrive stuff.The way these controls work, if you're making small adjustments you're not just making loudness changes. The softclips and antisaturations have the effect of moving sounds toward you, or farther away. Cutting any control to zero will totally remove that band, but smaller adjustments lean heavily into the 'we're attenuating the quiet detail/we're overdriving the loudest stuff'. As a result, you can EQ spatially: if you have unpleasant low-mids muddying things up, your adjustment can not only turn them down but make them step physically back. You can dial it in based on the sound coming into focus at a desired distance. Top-end too forward? Small adjustments to sit it at the position of the rest of the sound. Lows receding? Well, bring them right out until they're part of the instrument the way you wanted.It's a little unusual, but every time I've done anything like this, including just making EQ-like things that had saturation in 'em, people have loved the result. So, FatEQ just directly delivers what you might like in an EQ. See if it works out for you!"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class FatEQ final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        pear_max,
        figL,
        figR,
        gndL,
        gndR,
        slew,
        freq,
        levl,
        pear_total
    }; // new pear filter
    double pearA[pear_total];
    double pearB[pear_total];
    double pearC[pear_total];
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
    FatEQ()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        for (int x = 0; x < pear_total; x++) {
            pearA[x] = 0.0;
            pearB[x] = 0.0;
            pearC[x] = 0.0;
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
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "high"; break;
            case kParamB: return "hmid"; break;
            case kParamC: return "lmid"; break;
            case kParamD: return "bass"; break;
            case kParamE: return "out"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return "HMid"; break;
            case kParamC: return "LMid"; break;
            case kParamD: return "Bass"; break;
            case kParamE: return "Out"; break;

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
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double topLevl = A * 2.0;
        if (topLevl < 1.0) {
            topLevl = sqrt(topLevl);
        }
        pearA[levl] = B * 2.0;
        if (pearA[levl] < 1.0) {
            pearA[levl] = sqrt(pearA[levl]);
        }
        pearB[levl] = C * 2.0;
        if (pearB[levl] < 1.0) {
            pearB[levl] = sqrt(pearB[levl]);
        }
        pearC[levl] = D * 2.0;
        if (pearC[levl] < 1.0) {
            pearC[levl] = sqrt(pearC[levl]);
        }
        double out = E;
        double freqFactor = sqrt(overallscale) + (overallscale * 0.5);
        pearA[freq] = pow(0.564, freqFactor + 0.85);
        pearB[freq] = pow(0.564, freqFactor + 4.1);
        pearC[freq] = pow(0.564, freqFactor + 7.1);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            for (int x = 0; x < pear_max; x += 4) {
                // begin Pear filter stages
                pearA[figL] = inputSampleL;
                pearA[figR] = inputSampleR;
                pearA[slew] = ((pearA[figL] - pearA[x]) + pearA[x + 1]) * pearA[freq] * 0.5;
                pearA[x] = pearA[figL] = (pearA[freq] * pearA[figL]) + ((1.0 - pearA[freq]) * (pearA[x] + pearA[x + 1]));
                pearA[x + 1] = pearA[slew];
                pearA[slew] = ((pearA[figR] - pearA[x + 2]) + pearA[x + 3]) * pearA[freq] * 0.5;
                pearA[x + 2] = pearA[figR] = (pearA[freq] * pearA[figR]) + ((1.0 - pearA[freq]) * (pearA[x + 2] + pearA[x + 3]));
                pearA[x + 3] = pearA[slew];
                inputSampleL -= pearA[figL];
                inputSampleR -= pearA[figR];
                pearB[figL] = pearA[figL];
                pearB[figR] = pearA[figR];
                pearB[slew] = ((pearB[figL] - pearB[x]) + pearB[x + 1]) * pearB[freq] * 0.5;
                pearB[x] = pearB[figL] = (pearB[freq] * pearA[figL]) + ((1.0 - pearB[freq]) * (pearB[x] + pearB[x + 1]));
                pearB[x + 1] = pearB[slew];
                pearB[slew] = ((pearB[figR] - pearB[x + 2]) + pearB[x + 3]) * pearB[freq] * 0.5;
                pearB[x + 2] = pearB[figR] = (pearB[freq] * pearA[figR]) + ((1.0 - pearB[freq]) * (pearB[x + 2] + pearB[x + 3]));
                pearB[x + 3] = pearB[slew];
                pearA[figL] -= pearB[figL];
                pearA[figR] -= pearB[figR];
                pearC[figL] = pearB[figL];
                pearC[figR] = pearB[figR];
                pearC[slew] = ((pearC[figL] - pearC[x]) + pearC[x + 1]) * pearC[freq] * 0.5;
                pearC[x] = pearC[figL] = (pearC[freq] * pearB[figL]) + ((1.0 - pearC[freq]) * (pearC[x] + pearC[x + 1]));
                pearC[x + 1] = pearC[slew];
                pearC[slew] = ((pearC[figR] - pearC[x + 2]) + pearC[x + 3]) * pearC[freq] * 0.5;
                pearC[x + 2] = pearC[figR] = (pearC[freq] * pearB[figR]) + ((1.0 - pearC[freq]) * (pearC[x + 2] + pearC[x + 3]));
                pearC[x + 3] = pearC[slew];
                pearB[figL] -= pearC[figL];
                pearB[figR] -= pearC[figR];
                // begin FatEQL
                double altered = inputSampleL;
                if (topLevl > 1.0) {
                    altered = fmax(fmin(inputSampleL * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (topLevl < 1.0) {
                    altered = fmax(fmin(inputSampleL, 1.0), -1.0);
                    double polarity = altered;
                    double X = inputSampleL * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleL = (inputSampleL * (1.0 - fabs(topLevl - 1.0))) + (altered * fabs(topLevl - 1.0));
                altered = pearA[figL];
                if (pearA[levl] > 1.0) {
                    altered = fmax(fmin(pearA[figL] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearA[levl] < 1.0) {
                    altered = fmax(fmin(pearA[figL], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearA[figL] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleL += (pearA[figL] * (1.0 - fabs(pearA[levl] - 1.0))) + (altered * fabs(pearA[levl] - 1.0));
                altered = pearB[figL];
                if (pearB[levl] > 1.0) {
                    altered = fmax(fmin(pearB[figL] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearB[levl] < 1.0) {
                    altered = fmax(fmin(pearB[figL], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearB[figL] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleL += (pearB[figL] * (1.0 - fabs(pearB[levl] - 1.0))) + (altered * fabs(pearB[levl] - 1.0));
                altered = pearC[figL];
                if (pearC[levl] > 1.0) {
                    altered = fmax(fmin(pearC[figL] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearC[levl] < 1.0) {
                    altered = fmax(fmin(pearC[figL], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearC[figL] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleL += (pearC[figL] * (1.0 - fabs(pearC[levl] - 1.0))) + (altered * fabs(pearC[levl] - 1.0));
                // end FatEQ L
                // begin FatEQ R
                altered = inputSampleR;
                if (topLevl > 1.0) {
                    altered = fmax(fmin(inputSampleR * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (topLevl < 1.0) {
                    altered = fmax(fmin(inputSampleR, 1.0), -1.0);
                    double polarity = altered;
                    double X = inputSampleR * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleR = (inputSampleR * (1.0 - fabs(topLevl - 1.0))) + (altered * fabs(topLevl - 1.0));
                altered = pearA[figR];
                if (pearA[levl] > 1.0) {
                    altered = fmax(fmin(pearA[figR] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearA[levl] < 1.0) {
                    altered = fmax(fmin(pearA[figR], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearA[figR] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleR += (pearA[figR] * (1.0 - fabs(pearA[levl] - 1.0))) + (altered * fabs(pearA[levl] - 1.0));
                altered = pearB[figR];
                if (pearB[levl] > 1.0) {
                    altered = fmax(fmin(pearB[figR] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearB[levl] < 1.0) {
                    altered = fmax(fmin(pearB[figR], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearB[figR] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleR += (pearB[figR] * (1.0 - fabs(pearB[levl] - 1.0))) + (altered * fabs(pearB[levl] - 1.0));
                altered = pearC[figR];
                if (pearC[levl] > 1.0) {
                    altered = fmax(fmin(pearC[figR] * M_PI_2, M_PI_2), -M_PI_2);
                    double X = altered * altered;
                    double temp = altered * X;
                    altered -= (temp / 6.0);
                    temp *= X;
                    altered += (temp / 120.0);
                    temp *= X;
                    altered -= (temp / 5040.0);
                    temp *= X;
                    altered += (temp / 362880.0);
                    temp *= X;
                    altered -= (temp / 39916800.0);
                }
                if (pearC[levl] < 1.0) {
                    altered = fmax(fmin(pearC[figR], 1.0), -1.0);
                    double polarity = altered;
                    double X = pearC[figR] * altered;
                    double temp = X;
                    altered = (temp / 2.0);
                    temp *= X;
                    altered -= (temp / 24.0);
                    temp *= X;
                    altered += (temp / 720.0);
                    temp *= X;
                    altered -= (temp / 40320.0);
                    temp *= X;
                    altered += (temp / 3628800.0);
                    altered *= ((polarity < 0.0) ? -1.0 : 1.0);
                }
                inputSampleR += (pearC[figR] * (1.0 - fabs(pearC[levl] - 1.0))) + (altered * fabs(pearC[levl] - 1.0));
                // end FatEQ R
            }
            inputSampleL *= out;
            inputSampleR *= out;
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
} // namespace airwindohhs::fateq
