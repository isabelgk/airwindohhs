#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestsquish {

constexpr std::string_view k_name{ "PurestSquish" };
constexpr std::string_view k_short_description{
    "PurestSquish is an open-sounding compressor with bass bloom."
};
constexpr std::string_view k_long_description{
    "PurestSquish is a compressor, with its own sound. In the video I compare it to Pressure4, Logical4, and SurgeTide, and also show how it can be used in conjunction with SurgeTide (a real ‘sleeper’ plugin not easily understood) to produce amazingly transparent dynamics control. That said, this is not at all a normal compressor plugin and won't act normal.I also spent some time torturing it with sine sweeps, showing how Pressure and Logical are more like ‘analog emulations’ and produce harmonics, while PurestSquish instead does a weird thing when you turn off and on signal generators. So if you’re looking for ways to say ‘this is broken forever!’ watch those parts of the video :)If this doesn’t worry you, PurestSquish also has a bass bloom control that lets you pass subsonics or bass notes through uncompressed, to taste. If it does worry you, chalk it up to PurestSquish running simultaneous two-and-three-sample-interleaved compressors, much like Capacitor runs two-and-three-sample-interleaved filters, and use one of the other compressors I’ve put out, perhaps one of the three also featured in the PurestSquish video."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class PurestSquish final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
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
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double lastCoefficientAL;
    double lastCoefficientBL;
    double lastCoefficientCL;
    double lastCoefficientDL;
    double mergedCoefficientsL;
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
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double lastCoefficientAR;
    double lastCoefficientBR;
    double lastCoefficientCR;
    double lastCoefficientDR;
    double mergedCoefficientsR;
    int count;
    bool fpFlip;

  public:
    PurestSquish()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        D = 1.0;
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
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        lastCoefficientAL = 1;
        lastCoefficientBL = 1;
        lastCoefficientCL = 1;
        lastCoefficientDL = 1;
        mergedCoefficientsL = 1;
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
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        lastCoefficientAR = 1;
        lastCoefficientBR = 1;
        lastCoefficientCR = 1;
        lastCoefficientDR = 1;
        mergedCoefficientsR = 1;
        muVaryR = 1;
        count = 1;
        fpFlip = true;
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
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "squish"; break;
            case kParamB: return "bassblm"; break;
            case kParamC: return "output"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Squish"; break;
            case kParamB: return "BassBlm"; break;
            case kParamC: return "Output"; break;
            case kParamD: return "Dry/Wet"; break;

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
        double highpassSampleL;
        double highpassSampleBL;
        double highpassSampleR;
        double highpassSampleBR;
        double threshold = 1.01 - (1.0 - pow(1.0 - (A * 0.5), 4));
        double iirAmount = pow(B, 4) / overallscale;
        double output = C;
        double wet = D;
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
            double muMakeupGainL = 1.0 / threshold;
            double outMakeupGainL = sqrt(muMakeupGainL);
            muMakeupGainL += outMakeupGainL;
            muMakeupGainL *= 0.5;
            outMakeupGainL *= 0.5;
            // gain settings around threshold
            double releaseL = mergedCoefficientsL * 32768.0;
            releaseL /= overallscale;
            double fastestL = sqrt(releaseL);
            // speed settings around release
            double lastCorrectionL = mergedCoefficientsL;
            // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
            double muMakeupGainR = 1.0 / threshold;
            double outMakeupGainR = sqrt(muMakeupGainR);
            muMakeupGainR += outMakeupGainR;
            muMakeupGainR *= 0.5;
            outMakeupGainR *= 0.5;
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
            highpassSampleBL = highpassSampleL = inputSampleL;
            highpassSampleBR = highpassSampleR = inputSampleR;
            if (count < 1 || count > 3) {
                count = 1;
            }
            switch (count)
            {
                case 1:
                    // begin L
                    iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (highpassSampleL * iirAmount);
                    highpassSampleL -= iirSampleAL;
                    if (fabs(highpassSampleL) > threshold)
                    {
                        muVaryL = threshold / fabs(highpassSampleL);
                        muAttackL = sqrt(fabs(muSpeedAL));
                        muCoefficientAL = muCoefficientAL * (muAttackL - 1.0);
                        if (muVaryL < threshold)
                        {
                            muCoefficientAL = muCoefficientAL + threshold;
                        }
                        else
                        {
                            muCoefficientAL = muCoefficientAL + muVaryL;
                        }
                        muCoefficientAL = muCoefficientAL / muAttackL;
                    }
                    else
                    {
                        muCoefficientAL = muCoefficientAL * ((muSpeedAL * muSpeedAL) - 1.0);
                        muCoefficientAL = muCoefficientAL + 1.0;
                        muCoefficientAL = muCoefficientAL / (muSpeedAL * muSpeedAL);
                    }
                    muNewSpeedL = muSpeedAL * (muSpeedAL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(highpassSampleL * releaseL) + fastestL;
                    muSpeedAL = muNewSpeedL / muSpeedAL;
                    lastCoefficientAL = pow(muCoefficientAL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientAL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (highpassSampleR * iirAmount);
                    highpassSampleR -= iirSampleAR;
                    if (fabs(highpassSampleR) > threshold)
                    {
                        muVaryR = threshold / fabs(highpassSampleR);
                        muAttackR = sqrt(fabs(muSpeedAR));
                        muCoefficientAR = muCoefficientAR * (muAttackR - 1.0);
                        if (muVaryR < threshold)
                        {
                            muCoefficientAR = muCoefficientAR + threshold;
                        }
                        else
                        {
                            muCoefficientAR = muCoefficientAR + muVaryR;
                        }
                        muCoefficientAR = muCoefficientAR / muAttackR;
                    }
                    else
                    {
                        muCoefficientAR = muCoefficientAR * ((muSpeedAR * muSpeedAR) - 1.0);
                        muCoefficientAR = muCoefficientAR + 1.0;
                        muCoefficientAR = muCoefficientAR / (muSpeedAR * muSpeedAR);
                    }
                    muNewSpeedR = muSpeedAR * (muSpeedAR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(highpassSampleR * releaseR) + fastestR;
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
                    iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (highpassSampleL * iirAmount);
                    highpassSampleL -= iirSampleBL;
                    if (fabs(highpassSampleL) > threshold)
                    {
                        muVaryL = threshold / fabs(highpassSampleL);
                        muAttackL = sqrt(fabs(muSpeedBL));
                        muCoefficientBL = muCoefficientBL * (muAttackL - 1);
                        if (muVaryL < threshold)
                        {
                            muCoefficientBL = muCoefficientBL + threshold;
                        }
                        else
                        {
                            muCoefficientBL = muCoefficientBL + muVaryL;
                        }
                        muCoefficientBL = muCoefficientBL / muAttackL;
                    }
                    else
                    {
                        muCoefficientBL = muCoefficientBL * ((muSpeedBL * muSpeedBL) - 1.0);
                        muCoefficientBL = muCoefficientBL + 1.0;
                        muCoefficientBL = muCoefficientBL / (muSpeedBL * muSpeedBL);
                    }
                    muNewSpeedL = muSpeedBL * (muSpeedBL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(highpassSampleL * releaseL) + fastestL;
                    muSpeedBL = muNewSpeedL / muSpeedBL;
                    lastCoefficientAL = pow(muCoefficientBL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientBL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (highpassSampleR * iirAmount);
                    highpassSampleR -= iirSampleBR;
                    if (fabs(highpassSampleR) > threshold)
                    {
                        muVaryR = threshold / fabs(highpassSampleR);
                        muAttackR = sqrt(fabs(muSpeedBR));
                        muCoefficientBR = muCoefficientBR * (muAttackR - 1);
                        if (muVaryR < threshold)
                        {
                            muCoefficientBR = muCoefficientBR + threshold;
                        }
                        else
                        {
                            muCoefficientBR = muCoefficientBR + muVaryR;
                        }
                        muCoefficientBR = muCoefficientBR / muAttackR;
                    }
                    else
                    {
                        muCoefficientBR = muCoefficientBR * ((muSpeedBR * muSpeedBR) - 1.0);
                        muCoefficientBR = muCoefficientBR + 1.0;
                        muCoefficientBR = muCoefficientBR / (muSpeedBR * muSpeedBR);
                    }
                    muNewSpeedR = muSpeedBR * (muSpeedBR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(highpassSampleR * releaseR) + fastestR;
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
                    iirSampleCL = (iirSampleCL * (1 - iirAmount)) + (highpassSampleL * iirAmount);
                    highpassSampleL -= iirSampleCL;
                    if (fabs(highpassSampleL) > threshold)
                    {
                        muVaryL = threshold / fabs(highpassSampleL);
                        muAttackL = sqrt(fabs(muSpeedCL));
                        muCoefficientCL = muCoefficientCL * (muAttackL - 1);
                        if (muVaryL < threshold)
                        {
                            muCoefficientCL = muCoefficientCL + threshold;
                        }
                        else
                        {
                            muCoefficientCL = muCoefficientCL + muVaryL;
                        }
                        muCoefficientCL = muCoefficientCL / muAttackL;
                    }
                    else
                    {
                        muCoefficientCL = muCoefficientCL * ((muSpeedCL * muSpeedCL) - 1.0);
                        muCoefficientCL = muCoefficientCL + 1.0;
                        muCoefficientCL = muCoefficientCL / (muSpeedCL * muSpeedCL);
                    }
                    muNewSpeedL = muSpeedCL * (muSpeedCL - 1);
                    muNewSpeedL = muNewSpeedL + fabs(highpassSampleL * releaseL) + fastestL;
                    muSpeedCL = muNewSpeedL / muSpeedCL;
                    lastCoefficientAL = pow(muCoefficientCL, 2);
                    mergedCoefficientsL = lastCoefficientBL;
                    mergedCoefficientsL += lastCoefficientAL;
                    lastCoefficientAL *= (1.0 - lastCorrectionL);
                    lastCoefficientAL += (muCoefficientCL * lastCorrectionL);
                    lastCoefficientBL = lastCoefficientAL;
                    // end L
                    // begin R
                    iirSampleCR = (iirSampleCR * (1 - iirAmount)) + (highpassSampleR * iirAmount);
                    highpassSampleR -= iirSampleCR;
                    if (fabs(highpassSampleR) > threshold)
                    {
                        muVaryR = threshold / fabs(highpassSampleR);
                        muAttackR = sqrt(fabs(muSpeedCR));
                        muCoefficientCR = muCoefficientCR * (muAttackR - 1);
                        if (muVaryR < threshold)
                        {
                            muCoefficientCR = muCoefficientCR + threshold;
                        }
                        else
                        {
                            muCoefficientCR = muCoefficientCR + muVaryR;
                        }
                        muCoefficientCR = muCoefficientCR / muAttackR;
                    }
                    else
                    {
                        muCoefficientCR = muCoefficientCR * ((muSpeedCR * muSpeedCR) - 1.0);
                        muCoefficientCR = muCoefficientCR + 1.0;
                        muCoefficientCR = muCoefficientCR / (muSpeedCR * muSpeedCR);
                    }
                    muNewSpeedR = muSpeedCR * (muSpeedCR - 1);
                    muNewSpeedR = muNewSpeedR + fabs(highpassSampleR * releaseR) + fastestR;
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
                iirSampleDL = (iirSampleDL * (1 - iirAmount)) + (highpassSampleBL * iirAmount);
                highpassSampleBL -= iirSampleDL;
                if (fabs(highpassSampleBL) > threshold)
                {
                    muVaryL = threshold / fabs(highpassSampleBL);
                    muAttackL = sqrt(fabs(muSpeedDL));
                    muCoefficientDL = muCoefficientDL * (muAttackL - 1.0);
                    if (muVaryL < threshold)
                    {
                        muCoefficientDL = muCoefficientDL + threshold;
                    }
                    else
                    {
                        muCoefficientDL = muCoefficientDL + muVaryL;
                    }
                    muCoefficientDL = muCoefficientDL / muAttackL;
                }
                else
                {
                    muCoefficientDL = muCoefficientDL * ((muSpeedDL * muSpeedDL) - 1.0);
                    muCoefficientDL = muCoefficientDL + 1.0;
                    muCoefficientDL = muCoefficientDL / (muSpeedDL * muSpeedDL);
                }
                muNewSpeedL = muSpeedDL * (muSpeedDL - 1);
                muNewSpeedL = muNewSpeedL + fabs(highpassSampleBL * releaseL) + fastestL;
                muSpeedDL = muNewSpeedL / muSpeedDL;
                lastCoefficientCL = pow(muCoefficientEL, 2);
                mergedCoefficientsL += lastCoefficientDL;
                mergedCoefficientsL += lastCoefficientCL;
                lastCoefficientCL *= (1.0 - lastCorrectionL);
                lastCoefficientCL += (muCoefficientDL * lastCorrectionL);
                lastCoefficientDL = lastCoefficientCL;
                // end L
                // begin R
                iirSampleDR = (iirSampleDR * (1 - iirAmount)) + (highpassSampleBR * iirAmount);
                highpassSampleBR -= iirSampleDR;
                if (fabs(highpassSampleBR) > threshold)
                {
                    muVaryR = threshold / fabs(highpassSampleBR);
                    muAttackR = sqrt(fabs(muSpeedDR));
                    muCoefficientDR = muCoefficientDR * (muAttackR - 1.0);
                    if (muVaryR < threshold)
                    {
                        muCoefficientDR = muCoefficientDR + threshold;
                    }
                    else
                    {
                        muCoefficientDR = muCoefficientDR + muVaryR;
                    }
                    muCoefficientDR = muCoefficientDR / muAttackR;
                }
                else
                {
                    muCoefficientDR = muCoefficientDR * ((muSpeedDR * muSpeedDR) - 1.0);
                    muCoefficientDR = muCoefficientDR + 1.0;
                    muCoefficientDR = muCoefficientDR / (muSpeedDR * muSpeedDR);
                }
                muNewSpeedR = muSpeedDR * (muSpeedDR - 1);
                muNewSpeedR = muNewSpeedR + fabs(highpassSampleBR * releaseR) + fastestR;
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
                iirSampleEL = (iirSampleEL * (1 - iirAmount)) + (highpassSampleBL * iirAmount);
                highpassSampleBL -= iirSampleEL;
                if (fabs(highpassSampleBL) > threshold)
                {
                    muVaryL = threshold / fabs(highpassSampleBL);
                    muAttackL = sqrt(fabs(muSpeedEL));
                    muCoefficientEL = muCoefficientEL * (muAttackL - 1.0);
                    if (muVaryL < threshold)
                    {
                        muCoefficientEL = muCoefficientEL + threshold;
                    }
                    else
                    {
                        muCoefficientEL = muCoefficientEL + muVaryL;
                    }
                    muCoefficientEL = muCoefficientEL / muAttackL;
                }
                else
                {
                    muCoefficientEL = muCoefficientEL * ((muSpeedEL * muSpeedEL) - 1.0);
                    muCoefficientEL = muCoefficientEL + 1.0;
                    muCoefficientEL = muCoefficientEL / (muSpeedEL * muSpeedEL);
                }
                muNewSpeedL = muSpeedEL * (muSpeedEL - 1);
                muNewSpeedL = muNewSpeedL + fabs(highpassSampleBL * releaseL) + fastestL;
                muSpeedEL = muNewSpeedL / muSpeedEL;
                lastCoefficientCL = pow(muCoefficientEL, 2);
                mergedCoefficientsL += lastCoefficientDL;
                mergedCoefficientsL += lastCoefficientCL;
                lastCoefficientCL *= (1.0 - lastCorrectionL);
                lastCoefficientCL += (muCoefficientEL * lastCorrectionL);
                lastCoefficientDL = lastCoefficientCL;
                // end L
                // begin R
                iirSampleER = (iirSampleER * (1 - iirAmount)) + (highpassSampleBR * iirAmount);
                highpassSampleBR -= iirSampleER;
                if (fabs(highpassSampleBR) > threshold)
                {
                    muVaryR = threshold / fabs(highpassSampleBR);
                    muAttackR = sqrt(fabs(muSpeedER));
                    muCoefficientER = muCoefficientER * (muAttackR - 1.0);
                    if (muVaryR < threshold)
                    {
                        muCoefficientER = muCoefficientER + threshold;
                    }
                    else
                    {
                        muCoefficientER = muCoefficientER + muVaryR;
                    }
                    muCoefficientER = muCoefficientER / muAttackR;
                }
                else
                {
                    muCoefficientER = muCoefficientER * ((muSpeedER * muSpeedER) - 1.0);
                    muCoefficientER = muCoefficientER + 1.0;
                    muCoefficientER = muCoefficientER / (muSpeedER * muSpeedER);
                }
                muNewSpeedR = muSpeedER * (muSpeedER - 1);
                muNewSpeedR = muNewSpeedR + fabs(highpassSampleBR * releaseR) + fastestR;
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
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs::purestsquish
