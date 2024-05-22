#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class VoiceTrick final : public Effect<T>
{
    std::string m_name{ "VoiceTrick" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirLowpassA;
    double iirLowpassB;
    double iirLowpassC;
    double iirLowpassD;
    double iirLowpassE;
    double iirLowpassF;
    int count;
    double lowpassChase;
    double lowpassAmount;
    double lastLowpass;
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    VoiceTrick()
    {
        A = 1.0;
        iirLowpassA = 0.0;
        iirLowpassB = 0.0;
        iirLowpassC = 0.0;
        iirLowpassD = 0.0;
        iirLowpassE = 0.0;
        iirLowpassF = 0.0;
        count = 0;
        lowpassChase = 0.0;
        lowpassAmount = 1.0;
        lastLowpass = 1000.0;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Tone";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        lowpassChase = pow(A, 2);
        // should not scale with sample rate, because values reaching 1 are important
        // to its ability to bypass when set to max
        double lowpassSpeed = 300 / (fabs(lastLowpass - lowpassChase) + 1.0);
        lastLowpass = lowpassChase;
        double invLowpass;
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
            lowpassAmount = (((lowpassAmount * lowpassSpeed) + lowpassChase) / (lowpassSpeed + 1.0));
            invLowpass = 1.0 - lowpassAmount;
            // setting chase functionality of Capacitor Lowpass. I could just use this value directly from the control,
            // but if I say it's the lowpass out of Capacitor it should literally be that in every behavior.
            double inputSample = (inputSampleL + inputSampleR) * 0.5;
            // this is now our mono audio
            count++;
            if (count > 5) {
                count = 0;
            }
            switch (count)
            {
                case 0:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassB;
                    iirLowpassD = (iirLowpassD * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassD;
                    break;
                case 1:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassC;
                    iirLowpassE = (iirLowpassE * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassE;
                    break;
                case 2:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassB;
                    iirLowpassF = (iirLowpassF * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassF;
                    break;
                case 3:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassC;
                    iirLowpassD = (iirLowpassD * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassD;
                    break;
                case 4:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassB;
                    iirLowpassE = (iirLowpassE * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassE;
                    break;
                case 5:
                    iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassA;
                    iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassC;
                    iirLowpassF = (iirLowpassF * invLowpass) + (inputSample * lowpassAmount);
                    inputSample = iirLowpassF;
                    break;
            }
            // Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
            // steepens the filter after minimizing artifacts.
            inputSampleL = -inputSample;
            inputSampleR = inputSample;
            // and now the output is mono, maybe filtered, and phase flipped to cancel at the microphone.
            // The purpose of all this is to allow for recording of lead vocals without use of headphones:
            // or at least sealed headphones. You should be able to use this to record vocals with either
            // open-back headphones, or literally speakers in the room so long as the mic is exactly
            // equidistant from each speaker/headphone side.
            // You'll probably want to not use voice monitoring: just mute the track being recorded, or monitor
            // only reverb and echo for vibe. Direct sound is the singer's direct sound.
            // The filtering is because, if you use open-back headphones and move your head, highs will
            // bleed through first like a through-zero flange coming out of cancellation (which it is).
            // Therefore, you can filter off highs until the bleed isn't annoying.
            // Or just run with it, it shouldn't be that loud.
            // Thanks to Peter Gabriel for many great examples of hit vocals recorded just like this :)
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
