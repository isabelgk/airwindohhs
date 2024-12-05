#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::capacitor {

constexpr std::string_view k_name{ "Capacitor" };
constexpr std::string_view k_short_description{
    "Capacitor is a lowpass/highpass filter of a new type."
};
constexpr std::string_view k_long_description{
    "Sometimes half the fun is just inventing :)People say all filter plugins are just biquads with different GUIs. It’s not true… or at least it’s not true at Airwindows. (actually I have to see what I can get out of biquads: I generally don’t even use ’em at all)This is a new filter. When I say new, I mean that you don’t have it. I can demonstrate that, because due to the way it works it has a little characteristic response quirk at 1/3 the sampling rate: nothing that hurts the sound, just an odd little notch under some circumstances.Because it’s a typically Airwindowsy, not-normal EQ algorithm, nobody has tried it, and because of the little quirk, if anybody did try it they knew they had to reject it, because it wasn’t perfect. It had a quirk! And even if they experimented, they probably weren’t that invested in concepts like the sequence of operations on stuff that, in theory, could be done in any order. Why care so much about the implementation details of an idea that had a quirk anyhow and was therefore useless?More fools they :)Because musically, this filter has tone for DAYS.There’s almost an analog-synth-like effect to it: though it doesn’t accentuate the rolloff point, it behaves almost like a DJ ‘isolator’ in the obviousness of its filtering. It has a highpass, a lowpass, and dry/wet, and the high and lowpass are melded into each other as part of the algorithm to get the sound more fluid. It’s also got new experiments in rejecting zipper noise and clicks, because I’m figuring people will want to automate this one. But, even though it’ll work for crazy effects and ‘isolator’ madness, the fullness of the tone will also let you use it in mastering: if you need to tighten bass or roll off just a hint of brightness, you’ll pay no penalty in the body of the music. And the dry/wet is there to let you treat it as a shelf… or to create a presence boost by isolating an area of interest, and then leaving it mostly dry with a hint of the clarified zone.I’m thrilled with this filter, and nobody else has it: nobody else would be allowed to do this one, because it has a quirk at 1/3 the sampling rate, and typical commercial developers are not allowed DSP quirks. But, since I’m supported by Patreon, I am."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Capacitor final : public Effect<T>
{
    double iirHighpassAL;
    double iirHighpassBL;
    double iirHighpassCL;
    double iirHighpassDL;
    double iirHighpassEL;
    double iirHighpassFL;
    double iirLowpassAL;
    double iirLowpassBL;
    double iirLowpassCL;
    double iirLowpassDL;
    double iirLowpassEL;
    double iirLowpassFL;
    double iirHighpassAR;
    double iirHighpassBR;
    double iirHighpassCR;
    double iirHighpassDR;
    double iirHighpassER;
    double iirHighpassFR;
    double iirLowpassAR;
    double iirLowpassBR;
    double iirLowpassCR;
    double iirLowpassDR;
    double iirLowpassER;
    double iirLowpassFR;
    int count;
    double lowpassChase;
    double highpassChase;
    double wetChase;
    double lowpassAmount;
    double highpassAmount;
    double wet;
    double lastLowpass;
    double lastHighpass;
    double lastWet;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Capacitor()
    {
        A = 1.0;
        B = 0.0;
        C = 1.0;
        iirHighpassAL = 0.0;
        iirHighpassBL = 0.0;
        iirHighpassCL = 0.0;
        iirHighpassDL = 0.0;
        iirHighpassEL = 0.0;
        iirHighpassFL = 0.0;
        iirLowpassAL = 0.0;
        iirLowpassBL = 0.0;
        iirLowpassCL = 0.0;
        iirLowpassDL = 0.0;
        iirLowpassEL = 0.0;
        iirLowpassFL = 0.0;
        iirHighpassAR = 0.0;
        iirHighpassBR = 0.0;
        iirHighpassCR = 0.0;
        iirHighpassDR = 0.0;
        iirHighpassER = 0.0;
        iirHighpassFR = 0.0;
        iirLowpassAR = 0.0;
        iirLowpassBR = 0.0;
        iirLowpassCR = 0.0;
        iirLowpassDR = 0.0;
        iirLowpassER = 0.0;
        iirLowpassFR = 0.0;
        count = 0;
        lowpassChase = 0.0;
        highpassChase = 0.0;
        wetChase = 0.0;
        lowpassAmount = 1.0;
        highpassAmount = 0.0;
        wet = 1.0;
        lastLowpass = 1000.0;
        lastHighpass = 1000.0;
        lastWet = 1000.0;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "lowpass"; break;
            case kParamB: return "highpass"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Lowpass"; break;
            case kParamB: return "Highpass"; break;
            case kParamC: return "Dry/Wet"; break;

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

        lowpassChase = pow(A, 2);
        highpassChase = pow(B, 2);
        wetChase = C;
        // should not scale with sample rate, because values reaching 1 are important
        // to its ability to bypass when set to max
        double lowpassSpeed = 300 / (fabs(lastLowpass - lowpassChase) + 1.0);
        double highpassSpeed = 300 / (fabs(lastHighpass - highpassChase) + 1.0);
        double wetSpeed = 300 / (fabs(lastWet - wetChase) + 1.0);
        lastLowpass = lowpassChase;
        lastHighpass = highpassChase;
        lastWet = wetChase;
        double invLowpass;
        double invHighpass;
        double dry;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            lowpassAmount = (((lowpassAmount * lowpassSpeed) + lowpassChase) / (lowpassSpeed + 1.0));
            invLowpass = 1.0 - lowpassAmount;
            highpassAmount = (((highpassAmount * highpassSpeed) + highpassChase) / (highpassSpeed + 1.0));
            invHighpass = 1.0 - highpassAmount;
            wet = (((wet * wetSpeed) + wetChase) / (wetSpeed + 1.0));
            dry = 1.0 - wet;
            count++;
            if (count > 5) {
                count = 0;
            }
            switch (count)
            {
                case 0:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassBL;
                    iirHighpassDL = (iirHighpassDL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassDL;
                    iirLowpassDL = (iirLowpassDL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassDL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassBR;
                    iirHighpassDR = (iirHighpassDR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassDR;
                    iirLowpassDR = (iirLowpassDR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassDR;
                    break;
                case 1:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassCL;
                    iirHighpassEL = (iirHighpassEL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassEL;
                    iirLowpassEL = (iirLowpassEL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassEL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassCR;
                    iirHighpassER = (iirHighpassER * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassER;
                    iirLowpassER = (iirLowpassER * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassER;
                    break;
                case 2:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassBL;
                    iirHighpassFL = (iirHighpassFL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassFL;
                    iirLowpassFL = (iirLowpassFL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassFL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassBR;
                    iirHighpassFR = (iirHighpassFR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassFR;
                    iirLowpassFR = (iirLowpassFR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassFR;
                    break;
                case 3:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassCL;
                    iirHighpassDL = (iirHighpassDL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassDL;
                    iirLowpassDL = (iirLowpassDL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassDL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassCR;
                    iirHighpassDR = (iirHighpassDR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassDR;
                    iirLowpassDR = (iirLowpassDR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassDR;
                    break;
                case 4:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassBL = (iirHighpassBL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassBL;
                    iirLowpassBL = (iirLowpassBL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassBL;
                    iirHighpassEL = (iirHighpassEL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassEL;
                    iirLowpassEL = (iirLowpassEL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassEL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassBR = (iirHighpassBR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassBR;
                    iirLowpassBR = (iirLowpassBR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassBR;
                    iirHighpassER = (iirHighpassER * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassER;
                    iirLowpassER = (iirLowpassER * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassER;
                    break;
                case 5:
                    iirHighpassAL = (iirHighpassAL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassAL;
                    iirLowpassAL = (iirLowpassAL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassAL;
                    iirHighpassCL = (iirHighpassCL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassCL;
                    iirLowpassCL = (iirLowpassCL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassCL;
                    iirHighpassFL = (iirHighpassFL * invHighpass) + (inputSampleL * highpassAmount);
                    inputSampleL -= iirHighpassFL;
                    iirLowpassFL = (iirLowpassFL * invLowpass) + (inputSampleL * lowpassAmount);
                    inputSampleL = iirLowpassFL;
                    iirHighpassAR = (iirHighpassAR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassAR;
                    iirLowpassAR = (iirLowpassAR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassAR;
                    iirHighpassCR = (iirHighpassCR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassCR;
                    iirLowpassCR = (iirLowpassCR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassCR;
                    iirHighpassFR = (iirHighpassFR * invHighpass) + (inputSampleR * highpassAmount);
                    inputSampleR -= iirHighpassFR;
                    iirLowpassFR = (iirLowpassFR * invLowpass) + (inputSampleR * lowpassAmount);
                    inputSampleR = iirLowpassFR;
                    break;
            }
            // Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
            // steepens the filter after minimizing artifacts.
            inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
            inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
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
} // namespace airwindohhs::capacitor
