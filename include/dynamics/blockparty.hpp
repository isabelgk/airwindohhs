#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class BlockParty final : public Effect<T>
{
    const std::string m_name{ "BlockParty" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    double muVaryL;
    double muAttackL;
    double muNewSpeedL;
    double muSpeedAL;
    double muSpeedBL;
    double muSpeedCL;
    double muSpeedDL;
    double muSpeedEL;
    double muCoefficientAL;
    double muCoefficientBL;
    double muCoefficientCL;
    double muCoefficientDL;
    double muCoefficientEL;
    double lastCoefficientAL;
    double lastCoefficientBL;
    double lastCoefficientCL;
    double lastCoefficientDL;
    double mergedCoefficientsL;
    double thresholdL;
    double thresholdBL;
    double muVaryR;
    double muAttackR;
    double muNewSpeedR;
    double muSpeedAR;
    double muSpeedBR;
    double muSpeedCR;
    double muSpeedDR;
    double muSpeedER;
    double muCoefficientAR;
    double muCoefficientBR;
    double muCoefficientCR;
    double muCoefficientDR;
    double muCoefficientER;
    double lastCoefficientAR;
    double lastCoefficientBR;
    double lastCoefficientCR;
    double lastCoefficientDR;
    double mergedCoefficientsR;
    double thresholdR;
    double thresholdBR;
    int count;
    bool fpFlip;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2

    };

  public:
    BlockParty()
    {
        A = 0.0;
        B = 1.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        muSpeedAL = 10000;
        muSpeedBL = 10000;
        muSpeedCL = 10000;
        muSpeedDL = 10000;
        muSpeedEL = 10000;
        muCoefficientAL = 1;
        muCoefficientBL = 1;
        muCoefficientCL = 1;
        muCoefficientDL = 1;
        muCoefficientEL = 1;
        lastCoefficientAL = 1;
        lastCoefficientBL = 1;
        lastCoefficientCL = 1;
        lastCoefficientDL = 1;
        mergedCoefficientsL = 1;
        thresholdL = 1.0;
        thresholdBL = 1.0;
        muVaryL = 1;
        muSpeedAR = 10000;
        muSpeedBR = 10000;
        muSpeedCR = 10000;
        muSpeedDR = 10000;
        muSpeedER = 10000;
        muCoefficientAR = 1;
        muCoefficientBR = 1;
        muCoefficientCR = 1;
        muCoefficientDR = 1;
        muCoefficientER = 1;
        lastCoefficientAR = 1;
        lastCoefficientBR = 1;
        lastCoefficientCR = 1;
        lastCoefficientDR = 1;
        mergedCoefficientsR = 1;
        thresholdR = 1.0;
        thresholdBR = 1.0;
        muVaryR = 1;
        count = 1;
        fpFlip = true;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Pound";
            case kParamB: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double targetthreshold = 1.01 - (1.0 - pow(1.0 - (A * 0.5), 4));
        double wet = B;
        double voicing = 0.618033988749894848204586;
        if (overallscale > 0.0) {
            voicing /= overallscale;
        }
        // translate to desired sample rate, 44.1K is the base
        if (voicing < 0.0) {
            voicing = 0.0;
        }
        if (voicing > 1.0) {
            voicing = 1.0;
        }
        // some insanity checking
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
            double muMakeupGainL = 1.0 / thresholdL;
            double outMakeupGainL = sqrt(muMakeupGainL);
            muMakeupGainL += outMakeupGainL;
            muMakeupGainL *= 0.5;
            // gain settings around threshold
            double releaseL = mergedCoefficientsL * 32768.0;
            releaseL /= overallscale;
            double fastestL = sqrt(releaseL);
            // speed settings around release
            double lastCorrectionL = mergedCoefficientsL;
            // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
            double muMakeupGainR = 1.0 / thresholdR;
            double outMakeupGainR = sqrt(muMakeupGainR);
            muMakeupGainR += outMakeupGainR;
            muMakeupGainR *= 0.5;
            // gain settings around threshold
            double releaseR = mergedCoefficientsR * 32768.0;
            releaseR /= overallscale;
            double fastestR = sqrt(releaseR);
            // speed settings around release
            double lastCorrectionR = mergedCoefficientsR;
            // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
            if (muMakeupGainL != 1.0) {
                inputSampleL = inputSampleL * muMakeupGainL;
            }
            if (muMakeupGainR != 1.0) {
                inputSampleR = inputSampleR * muMakeupGainR;
            }
            if (count < 1 || count > 3) {
                count = 1;
            }
            switch (count)
            {
                case 1:
                    // begin L
                    if (fabs(inputSampleL) > thresholdL)
                    {
                        if (inputSampleL > 0.0) {
                            inputSampleL = (inputSampleL * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        else {
                            inputSampleL = (inputSampleL * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        muVaryL = targetthreshold / fabs(inputSampleL);
                        muAttackL = sqrt(fabs(muSpeedAL));
                        muCoefficientAL = muCoefficientAL * (muAttackL - 1.0);
                        if (muVaryL < thresholdL)
                        {
                            muCoefficientAL = muCoefficientAL + targetthreshold;
                        }
                        else
                        {
                            muCoefficientAL = muCoefficientAL + muVaryL;
                        }
                        muCoefficientAL = muCoefficientAL / muAttackL;
                    }
                    else
                    {
                        thresholdL = targetthreshold;
                        muCoefficientAL = muCoefficientAL * ((muSpeedAL * muSpeedAL) - 1.0);
                        muCoefficientAL = muCoefficientAL + 1.0;
                        muCoefficientAL = muCoefficientAL / (muSpeedAL * muSpeedAL);
                    }
                    muNewSpeedL = muSpeedAL * (muSpeedAL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(inputSampleL * releaseL) + fastestL;
                    muSpeedAL = muNewSpeedL / muSpeedAL;
                    lastCoefficientAL = pow(muCoefficientAL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientAL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    if (fabs(inputSampleR) > thresholdR)
                    {
                        if (inputSampleR > 0.0) {
                            inputSampleR = (inputSampleR * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        else {
                            inputSampleR = (inputSampleR * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        muVaryR = targetthreshold / fabs(inputSampleR);
                        muAttackR = sqrt(fabs(muSpeedAR));
                        muCoefficientAR = muCoefficientAR * (muAttackR - 1.0);
                        if (muVaryR < thresholdR)
                        {
                            muCoefficientAR = muCoefficientAR + targetthreshold;
                        }
                        else
                        {
                            muCoefficientAR = muCoefficientAR + muVaryR;
                        }
                        muCoefficientAR = muCoefficientAR / muAttackR;
                    }
                    else
                    {
                        thresholdR = targetthreshold;
                        muCoefficientAR = muCoefficientAR * ((muSpeedAR * muSpeedAR) - 1.0);
                        muCoefficientAR = muCoefficientAR + 1.0;
                        muCoefficientAR = muCoefficientAR / (muSpeedAR * muSpeedAR);
                    }
                    muNewSpeedR = muSpeedAR * (muSpeedAR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(inputSampleR * releaseR) + fastestR;
                    muSpeedAR = muNewSpeedR / muSpeedAR;
                    lastCoefficientAR = pow(muCoefficientAR, 2);
                    mergedCoefficientsR = lastCoefficientBR;
                    mergedCoefficientsR += lastCoefficientAR;
                    lastCoefficientAR *= (1.0 - lastCorrectionR);
                    lastCoefficientAR += (muCoefficientAR * lastCorrectionR);
                    lastCoefficientBR = lastCoefficientAR;
                    // end R
                    break;
                case 2:
                    // begin L
                    if (fabs(inputSampleL) > thresholdL)
                    {
                        if (inputSampleL > 0.0) {
                            inputSampleL = (inputSampleL * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        else {
                            inputSampleL = (inputSampleL * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        muVaryL = targetthreshold / fabs(inputSampleL);
                        muAttackL = sqrt(fabs(muSpeedBL));
                        muCoefficientBL = muCoefficientBL * (muAttackL - 1);
                        if (muVaryL < thresholdL)
                        {
                            muCoefficientBL = muCoefficientBL + targetthreshold;
                        }
                        else
                        {
                            muCoefficientBL = muCoefficientBL + muVaryL;
                        }
                        muCoefficientBL = muCoefficientBL / muAttackL;
                    }
                    else
                    {
                        thresholdL = targetthreshold;
                        muCoefficientBL = muCoefficientBL * ((muSpeedBL * muSpeedBL) - 1.0);
                        muCoefficientBL = muCoefficientBL + 1.0;
                        muCoefficientBL = muCoefficientBL / (muSpeedBL * muSpeedBL);
                    }
                    muNewSpeedL = muSpeedBL * (muSpeedBL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(inputSampleL * releaseL) + fastestL;
                    muSpeedBL = muNewSpeedL / muSpeedBL;
                    lastCoefficientAL = pow(muCoefficientBL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientBL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    if (fabs(inputSampleR) > thresholdR)
                    {
                        if (inputSampleR > 0.0) {
                            inputSampleR = (inputSampleR * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        else {
                            inputSampleR = (inputSampleR * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        muVaryR = targetthreshold / fabs(inputSampleR);
                        muAttackR = sqrt(fabs(muSpeedBR));
                        muCoefficientBR = muCoefficientBR * (muAttackR - 1);
                        if (muVaryR < thresholdR)
                        {
                            muCoefficientBR = muCoefficientBR + targetthreshold;
                        }
                        else
                        {
                            muCoefficientBR = muCoefficientBR + muVaryR;
                        }
                        muCoefficientBR = muCoefficientBR / muAttackR;
                    }
                    else
                    {
                        thresholdR = targetthreshold;
                        muCoefficientBR = muCoefficientBR * ((muSpeedBR * muSpeedBR) - 1.0);
                        muCoefficientBR = muCoefficientBR + 1.0;
                        muCoefficientBR = muCoefficientBR / (muSpeedBR * muSpeedBR);
                    }
                    muNewSpeedR = muSpeedBR * (muSpeedBR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(inputSampleR * releaseR) + fastestR;
                    muSpeedBR = muNewSpeedR / muSpeedBR;
                    lastCoefficientAR = pow(muCoefficientBR, 2);
                    mergedCoefficientsR = lastCoefficientBR;
                    mergedCoefficientsR += lastCoefficientAR;
                    lastCoefficientAR *= (1.0 - lastCorrectionR);
                    lastCoefficientAR += (muCoefficientBR * lastCorrectionR);
                    lastCoefficientBR = lastCoefficientAR;
                    // end R
                    break;
                case 3:
                    // begin L
                    if (fabs(inputSampleL) > thresholdL)
                    {
                        if (inputSampleL > 0.0) {
                            inputSampleL = (inputSampleL * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        else {
                            inputSampleL = (inputSampleL * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdL = fabs(inputSampleL);
                        }
                        muVaryL = targetthreshold / fabs(inputSampleL);
                        muAttackL = sqrt(fabs(muSpeedCL));
                        muCoefficientCL = muCoefficientCL * (muAttackL - 1);
                        if (muVaryL < thresholdL)
                        {
                            muCoefficientCL = muCoefficientCL + targetthreshold;
                        }
                        else
                        {
                            muCoefficientCL = muCoefficientCL + muVaryL;
                        }
                        muCoefficientCL = muCoefficientCL / muAttackL;
                    }
                    else
                    {
                        thresholdL = targetthreshold;
                        muCoefficientCL = muCoefficientCL * ((muSpeedCL * muSpeedCL) - 1.0);
                        muCoefficientCL = muCoefficientCL + 1.0;
                        muCoefficientCL = muCoefficientCL / (muSpeedCL * muSpeedCL);
                    }
                    muNewSpeedL = muSpeedCL * (muSpeedCL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(inputSampleL * releaseL) + fastestL;
                    muSpeedCL = muNewSpeedL / muSpeedCL;
                    lastCoefficientAL = pow(muCoefficientCL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientCL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    if (fabs(inputSampleR) > thresholdR)
                    {
                        if (inputSampleR > 0.0) {
                            inputSampleR = (inputSampleR * voicing) + (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        else {
                            inputSampleR = (inputSampleR * voicing) - (targetthreshold * (1.0 - voicing));
                            thresholdR = fabs(inputSampleR);
                        }
                        muVaryR = targetthreshold / fabs(inputSampleR);
                        muAttackR = sqrt(fabs(muSpeedCR));
                        muCoefficientCR = muCoefficientCR * (muAttackR - 1);
                        if (muVaryR < thresholdR)
                        {
                            muCoefficientCR = muCoefficientCR + targetthreshold;
                        }
                        else
                        {
                            muCoefficientCR = muCoefficientCR + muVaryR;
                        }
                        muCoefficientCR = muCoefficientCR / muAttackR;
                    }
                    else
                    {
                        thresholdR = targetthreshold;
                        muCoefficientCR = muCoefficientCR * ((muSpeedCR * muSpeedCR) - 1.0);
                        muCoefficientCR = muCoefficientCR + 1.0;
                        muCoefficientCR = muCoefficientCR / (muSpeedCR * muSpeedCR);
                    }
                    muNewSpeedR = muSpeedCR * (muSpeedCR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(inputSampleR * releaseR) + fastestR;
                    muSpeedCR = muNewSpeedR / muSpeedCR;
                    lastCoefficientAR = pow(muCoefficientCR, 2);
                    mergedCoefficientsR = lastCoefficientBR;
                    mergedCoefficientsR += lastCoefficientAR;
                    lastCoefficientAR *= (1.0 - lastCorrectionR);
                    lastCoefficientAR += (muCoefficientCR * lastCorrectionR);
                    lastCoefficientBR = lastCoefficientAR;
                    // end R
                    break;
            }
            count++;
            // applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
            // applied gain correction to control output level- tends to constrain sound rather than inflate it
            if (fpFlip) {
                // begin L
                if (fabs(inputSampleL) > thresholdBL)
                {
                    if (inputSampleL > 0.0) {
                        inputSampleL = (inputSampleL * voicing) + (targetthreshold * (1.0 - voicing));
                        thresholdBL = fabs(inputSampleL);
                    }
                    else {
                        inputSampleL = (inputSampleL * voicing) - (targetthreshold * (1.0 - voicing));
                        thresholdBL = fabs(inputSampleL);
                    }
                    muVaryL = targetthreshold / fabs(inputSampleL);
                    muAttackL = sqrt(fabs(muSpeedDL));
                    muCoefficientDL = muCoefficientDL * (muAttackL - 1.0);
                    if (muVaryL < thresholdBL)
                    {
                        muCoefficientDL = muCoefficientDL + targetthreshold;
                    }
                    else
                    {
                        muCoefficientDL = muCoefficientDL + muVaryL;
                    }
                    muCoefficientDL = muCoefficientDL / muAttackL;
                }
                else
                {
                    thresholdBL = targetthreshold;
                    muCoefficientDL = muCoefficientDL * ((muSpeedDL * muSpeedDL) - 1.0);
                    muCoefficientDL = muCoefficientDL + 1.0;
                    muCoefficientDL = muCoefficientDL / (muSpeedDL * muSpeedDL);
                }
                muNewSpeedL = muSpeedDL * (muSpeedDL - 1);
                muNewSpeedL = muNewSpeedL + fabs(inputSampleL * releaseL) + fastestL;
                muSpeedDL = muNewSpeedL / muSpeedDL;
                lastCoefficientCL = pow(muCoefficientEL, 2);
                mergedCoefficientsL += lastCoefficientDL;
                mergedCoefficientsL += lastCoefficientCL;
                lastCoefficientCL *= (1.0 - lastCorrectionL);
                lastCoefficientCL += (muCoefficientDL * lastCorrectionL);
                lastCoefficientDL = lastCoefficientCL;
                // end L
                // begin R
                if (fabs(inputSampleR) > thresholdBR)
                {
                    if (inputSampleR > 0.0) {
                        inputSampleR = (inputSampleR * voicing) + (targetthreshold * (1.0 - voicing));
                        thresholdBR = fabs(inputSampleR);
                    }
                    else {
                        inputSampleR = (inputSampleR * voicing) - (targetthreshold * (1.0 - voicing));
                        thresholdBR = fabs(inputSampleR);
                    }
                    muVaryR = targetthreshold / fabs(inputSampleR);
                    muAttackR = sqrt(fabs(muSpeedDR));
                    muCoefficientDR = muCoefficientDR * (muAttackR - 1.0);
                    if (muVaryR < thresholdBR)
                    {
                        muCoefficientDR = muCoefficientDR + targetthreshold;
                    }
                    else
                    {
                        muCoefficientDR = muCoefficientDR + muVaryR;
                    }
                    muCoefficientDR = muCoefficientDR / muAttackR;
                }
                else
                {
                    thresholdBR = targetthreshold;
                    muCoefficientDR = muCoefficientDR * ((muSpeedDR * muSpeedDR) - 1.0);
                    muCoefficientDR = muCoefficientDR + 1.0;
                    muCoefficientDR = muCoefficientDR / (muSpeedDR * muSpeedDR);
                }
                muNewSpeedR = muSpeedDR * (muSpeedDR - 1);
                muNewSpeedR = muNewSpeedR + fabs(inputSampleR * releaseR) + fastestR;
                muSpeedDR = muNewSpeedR / muSpeedDR;
                lastCoefficientCR = pow(muCoefficientER, 2);
                mergedCoefficientsR += lastCoefficientDR;
                mergedCoefficientsR += lastCoefficientCR;
                lastCoefficientCR *= (1.0 - lastCorrectionR);
                lastCoefficientCR += (muCoefficientDR * lastCorrectionR);
                lastCoefficientDR = lastCoefficientCR;
                // end R
            }
            else {
                // begin L
                if (fabs(inputSampleL) > thresholdBL)
                {
                    if (inputSampleL > 0.0) {
                        inputSampleL = (inputSampleL * voicing) + (targetthreshold * (1.0 - voicing));
                        thresholdBL = fabs(inputSampleL);
                    }
                    else {
                        inputSampleL = (inputSampleL * voicing) - (targetthreshold * (1.0 - voicing));
                        thresholdBL = fabs(inputSampleL);
                    }
                    muVaryL = targetthreshold / fabs(inputSampleL);
                    muAttackL = sqrt(fabs(muSpeedEL));
                    muCoefficientEL = muCoefficientEL * (muAttackL - 1.0);
                    if (muVaryL < thresholdBL)
                    {
                        muCoefficientEL = muCoefficientEL + targetthreshold;
                    }
                    else
                    {
                        muCoefficientEL = muCoefficientEL + muVaryL;
                    }
                    muCoefficientEL = muCoefficientEL / muAttackL;
                }
                else
                {
                    thresholdBL = targetthreshold;
                    muCoefficientEL = muCoefficientEL * ((muSpeedEL * muSpeedEL) - 1.0);
                    muCoefficientEL = muCoefficientEL + 1.0;
                    muCoefficientEL = muCoefficientEL / (muSpeedEL * muSpeedEL);
                }
                muNewSpeedL = muSpeedEL * (muSpeedEL - 1);
                muNewSpeedL = muNewSpeedL + fabs(inputSampleL * releaseL) + fastestL;
                muSpeedEL = muNewSpeedL / muSpeedEL;
                lastCoefficientCL = pow(muCoefficientEL, 2);
                mergedCoefficientsL += lastCoefficientDL;
                mergedCoefficientsL += lastCoefficientCL;
                lastCoefficientCL *= (1.0 - lastCorrectionL);
                lastCoefficientCL += (muCoefficientEL * lastCorrectionL);
                lastCoefficientDL = lastCoefficientCL;
                // end L
                // begin R
                if (fabs(inputSampleR) > thresholdBR)
                {
                    if (inputSampleR > 0.0) {
                        inputSampleR = (inputSampleR * voicing) + (targetthreshold * (1.0 - voicing));
                        thresholdBR = fabs(inputSampleR);
                    }
                    else {
                        inputSampleR = (inputSampleR * voicing) - (targetthreshold * (1.0 - voicing));
                        thresholdBR = fabs(inputSampleR);
                    }
                    muVaryR = targetthreshold / fabs(inputSampleR);
                    muAttackR = sqrt(fabs(muSpeedER));
                    muCoefficientER = muCoefficientER * (muAttackR - 1.0);
                    if (muVaryR < thresholdBR)
                    {
                        muCoefficientER = muCoefficientER + targetthreshold;
                    }
                    else
                    {
                        muCoefficientER = muCoefficientER + muVaryR;
                    }
                    muCoefficientER = muCoefficientER / muAttackR;
                }
                else
                {
                    thresholdBR = targetthreshold;
                    muCoefficientER = muCoefficientER * ((muSpeedER * muSpeedER) - 1.0);
                    muCoefficientER = muCoefficientER + 1.0;
                    muCoefficientER = muCoefficientER / (muSpeedER * muSpeedER);
                }
                muNewSpeedR = muSpeedER * (muSpeedER - 1);
                muNewSpeedR = muNewSpeedR + fabs(inputSampleR * releaseR) + fastestR;
                muSpeedER = muNewSpeedR / muSpeedER;
                lastCoefficientCR = pow(muCoefficientER, 2);
                mergedCoefficientsR += lastCoefficientDR;
                mergedCoefficientsR += lastCoefficientCR;
                lastCoefficientCR *= (1.0 - lastCorrectionR);
                lastCoefficientCR += (muCoefficientER * lastCorrectionR);
                lastCoefficientDR = lastCoefficientCR;
                // end R
            }
            mergedCoefficientsL *= 0.25;
            inputSampleL *= mergedCoefficientsL;
            mergedCoefficientsR *= 0.25;
            inputSampleR *= mergedCoefficientsR;
            if (outMakeupGainL != 1.0) {
                inputSampleL = inputSampleL * outMakeupGainL;
            }
            if (outMakeupGainR != 1.0) {
                inputSampleR = inputSampleR * outMakeupGainR;
            }
            fpFlip = !fpFlip;
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            if (inputSampleL > 0.999) {
                inputSampleL = 0.999;
            }
            if (inputSampleL < -0.999) {
                inputSampleL = -0.999;
            }
            // iron bar clip comes after the dry/wet: alternate way to clean things up
            if (inputSampleR > 0.999) {
                inputSampleR = 0.999;
            }
            if (inputSampleR < -0.999) {
                inputSampleR = -0.999;
            }
            // iron bar clip comes after the dry/wet: alternate way to clean things up
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs
