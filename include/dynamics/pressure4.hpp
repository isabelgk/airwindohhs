#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pressure4 {

constexpr std::string_view k_name{ "Pressure4" };
constexpr std::string_view k_short_description{
    "Pressure4 is a compressor adjustable between vari-mu and ‘new york’ peak-retaining behaviors."
};
constexpr std::string_view k_long_description{
    "What can I even say? This is the golden ear favorite. Version 4 brings new functionality that has never existed in Pressure before: the stereo version (default for VST, and the Audio Unit that doesn’t specifically say ‘Mono’) uses a special linked mode based on diade bridges in hardware compressors. That’s not to say that it is ‘analog modeling’ because it isn’t. From the beginning, Pressure has been made out of a lucky algorithm with a particular organic, pleasing quality, and part of Pressure4 is knowing what to strip down, how to simplify that algorithm until it lets all the music through.But then, when you explore the way Pressure4 squishes up depending on how hard you drive it, and then start listening to the textures of different speed control settings and what that does, and then begin exploring what the ‘µ-iness’ control does… and it turns out that each one of those things gives specific and controllable shapings of the sound, but in ways very difficult to put into words, yet you can learn what it does and make the plugin do what you intend even if it’s tough to articulate exactly what you’re going for…This is why we turn to odd little tools like this. The whole character of Pressure4 can change with tiny adjustments of the controls. It can do about twelve wholly distinct things when set up right, but they’re all inherent in that one curiously simple, but chaotically strange, algorithm. And now the linked stereo form of Pressure does all that with a naturalness and fluidity never before seen with this plugin."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pressure4 final : public Effect<T>
{
    double muVary;
    double muAttack;
    double muNewSpeed;
    double muSpeedA;
    double muSpeedB;
    double muCoefficientA;
    double muCoefficientB;
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    float A;
    float B;
    float C; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float D;

  public:
    Pressure4()
    {
        A = 0.0;
        B = 0.2;
        C = 1.0;
        D = 1.0;
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
        flip = false;
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
            case kParamB: return 0.2; break;
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
            case kParamA: return "pressure"; break;
            case kParamB: return "speed"; break;
            case kParamC: return "mewiness"; break;
            case kParamD: return "output gain"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Pressure"; break;
            case kParamB: return "Speed"; break;
            case kParamC: return "Mewiness"; break;
            case kParamD: return "Output Gain"; break;

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
            case kParamA: return " "; break;
            case kParamB: return " "; break;
            case kParamC: return " "; break;
            case kParamD: return " "; break;

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* inputL = inputs[0];
        T* inputR = inputs[1];
        T* outputL = outputs[0];
        T* outputR = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double threshold = 1.0 - (A * 0.95);
        double muMakeupGain = 1.0 / threshold;
        // gain settings around threshold
        double release = pow((1.28 - B), 5) * 32768.0;
        release /= overallscale;
        double fastest = sqrt(release);
        // speed settings around release
        double bridgerectifier;
        double coefficient;
        double inputSense;
        double mewiness = (C * 2.0) - 1.0;
        double unmewiness;
        double outputGain = D;
        bool positivemu;
        if (mewiness >= 0)
        {
            positivemu = true;
            unmewiness = 1.0 - mewiness;
        }
        else
        {
            positivemu = false;
            mewiness = -mewiness;
            unmewiness = 1.0 - mewiness;
        }
        // µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *inputL;
            inputSampleR = *inputR;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL = inputSampleL * muMakeupGain;
            inputSampleR = inputSampleR * muMakeupGain;
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
                if (positivemu) {
                    coefficient = pow(muCoefficientA, 2);
                }
                else {
                    coefficient = sqrt(muCoefficientA);
                }
                coefficient = (coefficient * mewiness) + (muCoefficientA * unmewiness);
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            else
            {
                if (positivemu) {
                    coefficient = pow(muCoefficientB, 2);
                }
                else {
                    coefficient = sqrt(muCoefficientB);
                }
                coefficient = (coefficient * mewiness) + (muCoefficientB * unmewiness);
                inputSampleL *= coefficient;
                inputSampleR *= coefficient;
            }
            // applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
            // applied gain correction to control output level- tends to constrain sound rather than inflate it
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
            }
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
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
            *outputL = inputSampleL;
            *outputR = inputSampleR;
            *inputL++;
            *inputR++;
            *outputL++;
            *outputR++;
        }
    }
};
} // namespace airwindohhs::pressure4
