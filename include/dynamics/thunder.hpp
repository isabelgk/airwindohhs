#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::thunder {

constexpr std::string_view k_name{ "Thunder" };
constexpr std::string_view k_short_description{
    "Thunder is a compressor that retains or exaggerates subsonic bass when you push it."
};
constexpr std::string_view k_long_description{
    "Compressors are tricky little beasts: they tend to eat low-end, they react differently at low intensities than when you slam ’em, and each sort of compressor has a ‘voice’ that defines how it acts.There’s a crude way to deal with the low-end problem, which is to put in a crossover: either to split into multiband compression, or simply highpass the sense circuit so the compressor can’t compress extreme lows and just lets them through. I’ve been asked to do this and have actually tried it on PurestSquish, but I was never satisfied with the results. It seemed like just half-breaking the compressor and I felt there had to be a better way…Thunder is that better way. Instead of passing through the bass, it transforms it in a way similar to FathomFive, and uses that live, active signal to modulate what the compressor sees, where in turn the compressor’s output is modulated by the intense lows. It’s a little complicated, so you can watch the video or simply download it and try it. Thunder is free (bear in mind that my only compensation is the Patreon I’ve created. To do this onward into the future, that Patreon must succeed)If you’re auditioning Thunder, a word of warning. You’re going to have to monitor the extreme lows. This plugin can prepare music for the hugest sound systems or the finest audiophile playback, but if you can’t hear what it’s doing you may get in trouble with it. Decent headphones ought to suffice (not earbuds!). If you have subwoofers, this will test them. You can also use the mix-check plugin SubsOnly to test how you’re doing, that will spotlight the subs for you.The range of adjustment starts with gentle ‘glue’ compression as the subsonics begin to open up the mix. Then, a bottom octave will appear anchoring everything. Push beyond that and you start to get into more compressed 2-buss, with the extreme lows taking on a punchier, thumpier character. Add more Thunder and the bass gets frisky and aggressive and dominates. Then, when you go even beyond that, we’re talking about ridiculous monstrous mega-bass, and more or less wrecking the sound of everything else (it’s useful to be able to go to weird broken settings in case you want to step back just a bit and have a really extreme effect). This is not a multiband compressor (there’s only one stereo comp in the plugin) but when driven really hard it can go even more bonkers than a multiband compressor.Remember, the low Thunder settings are useful too as a glue comp, for a buss compressor that’s extremely transparent and true to the tone of the mix! The middle settings are just as functional. All of these settings have their own usefulness. Choose wisely (or unwisely, if you prefer)."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Thunder final : public Effect<T>
{
    double muVary;
    double muAttack;
    double muNewSpeed;
    double muSpeedA;
    double muSpeedB;
    double muCoefficientA;
    double muCoefficientB;
    double gateL;
    double gateR;
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleAM;
    double iirSampleBM;
    double iirSampleCM;
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    float A;
    float B;

  public:
    Thunder()
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
        muSpeedA = 10000;
        muSpeedB = 10000;
        muCoefficientA = 1;
        muCoefficientB = 1;
        muVary = 1;
        gateL = 0.0;
        gateR = 0.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleAM = 0.0;
        iirSampleBM = 0.0;
        iirSampleCM = 0.0;
        flip = false;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "thunder"; break;
            case kParamB: return "output trim"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thunder"; break;
            case kParamB: return "Output Trim"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return "dB"; break;

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
        double thunder = A * 0.4;
        double threshold = 1.0 - (thunder * 2.0);
        if (threshold < 0.01) {
            threshold = 0.01;
        }
        double muMakeupGain = 1.0 / threshold;
        double release = pow((1.28 - thunder), 5) * 32768.0;
        release /= overallscale;
        double fastest = sqrt(release);
        double EQ = ((0.0275 / Effect<T>::getSampleRate()) * 32000.0);
        double dcblock = EQ / 300.0;
        double basstrim = (0.01 / EQ) + 1.0;
        // FF parameters also ride off Speed
        double outputGain = B;
        double coefficient;
        double inputSense;
        double resultL;
        double resultR;
        double resultM;
        double resultML;
        double resultMR;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL = inputSampleL * muMakeupGain;
            inputSampleR = inputSampleR * muMakeupGain;
            if (gateL < fabs(inputSampleL)) {
                gateL = inputSampleL;
            }
            else {
                gateL -= dcblock;
            }
            if (gateR < fabs(inputSampleR)) {
                gateR = inputSampleR;
            }
            else {
                gateR -= dcblock;
            }
            // setting up gated DC blocking to control the tendency for rumble and offset
            // begin three FathomFive stages
            iirSampleAL += (inputSampleL * EQ * thunder);
            iirSampleAL -= (iirSampleAL * iirSampleAL * iirSampleAL * EQ);
            if (iirSampleAL > gateL) {
                iirSampleAL -= dcblock;
            }
            if (iirSampleAL < -gateL) {
                iirSampleAL += dcblock;
            }
            resultL = iirSampleAL * basstrim;
            iirSampleBL = (iirSampleBL * (1 - EQ)) + (resultL * EQ);
            resultL = iirSampleBL;
            iirSampleAR += (inputSampleR * EQ * thunder);
            iirSampleAR -= (iirSampleAR * iirSampleAR * iirSampleAR * EQ);
            if (iirSampleAR > gateR) {
                iirSampleAR -= dcblock;
            }
            if (iirSampleAR < -gateR) {
                iirSampleAR += dcblock;
            }
            resultR = iirSampleAR * basstrim;
            iirSampleBR = (iirSampleBR * (1 - EQ)) + (resultR * EQ);
            resultR = iirSampleBR;
            iirSampleAM += ((inputSampleL + inputSampleR) * EQ * thunder);
            iirSampleAM -= (iirSampleAM * iirSampleAM * iirSampleAM * EQ);
            resultM = iirSampleAM * basstrim;
            iirSampleBM = (iirSampleBM * (1 - EQ)) + (resultM * EQ);
            resultM = iirSampleBM;
            iirSampleCM = (iirSampleCM * (1 - EQ)) + (resultM * EQ);
            resultM = fabs(iirSampleCM);
            resultML = fabs(resultL);
            resultMR = fabs(resultR);
            if (resultM > resultML) {
                resultML = resultM;
            }
            if (resultM > resultMR) {
                resultMR = resultM;
            }
            // trying to restrict the buzziness
            if (resultML > 1.0) {
                resultML = 1.0;
            }
            if (resultMR > 1.0) {
                resultMR = 1.0;
            }
            // now we have result L, R and M the trigger modulator which must be 0-1
            // begin compressor section
            inputSampleL -= (iirSampleBL * thunder);
            inputSampleR -= (iirSampleBR * thunder);
            // highpass the comp section by sneaking out what will be the reinforcement
            inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            // we will take the greater of either channel and just use that, then apply the result
            // to both stereo channels.
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
            if (flip)
            {
                coefficient = pow(muCoefficientA, 2);
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            else
            {
                coefficient = pow(muCoefficientB, 2);
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            // applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
            // applied gain correction to control output level- tends to constrain sound rather than inflate it
            inputSampleL += (resultL * resultM);
            inputSampleR += (resultR * resultM);
            // combine the two by adding the summed channnel of lows
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
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
} // namespace airwindohhs::thunder
