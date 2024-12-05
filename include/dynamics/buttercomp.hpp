#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::buttercomp {

constexpr std::string_view k_name{ "ButterComp" };
constexpr std::string_view k_short_description{
    "ButterComp is my softest, smoothest compressor."
};
constexpr std::string_view k_long_description{
    "Sometimes a plugin can be a sort of cult favorite. That’s the story of ButterComp, a compressor of great subtlety that’s no use for quite a few normal compressor tasks… but still has a fervent following. I’ve been begged for the new version of this one (not merely VST, but revamped with all the current Airwindows sophistication and purity) and I’m delighted to bring it, though I think it might puzzle some people. If the stock Logic comp would do as well, this isn’t the plugin for you. But if you’re looking for a certain thing and thought it unattainable in software, this might be your lucky day.Buttercomp, under the hood, is absolutely unique (or unique, until I start working on variations and until other people try to copy it. They’re free to, but it’ll be possible to test that quite easily with special audio files).It’s a bipolar, interleaved compressor (with rather slow attack and release: a version with access to much faster dynamics is available in CStrip). This one’s the original, the cult classic. What is a bipolar, or an interleaved compressor? For the purposes of Buttercomp, it’s four totally independent compressors per channel. Two are sensitive to positive swing, and flip back and forth every sample. Two are sensitive to negative swing, also flipping back and forth every sample. The compression factor’s reconstructed through combining these poles, through the screen of the interleaved compressors switching back and forth at the Nyquist frequency… that mysterious digital frequency that is on the one hand the literal highest frequency that can be encoded, and the lowest frequency that ought to be totally rejected and filtered out.What happens is this: the tone of things gets some added second harmonic, wherever the compression is more strong on one side than the other. High frequencies take on a particular airy openness, since they too get second harmonic, plus individual sample spikes can only affect one out of four compressors: ButterComp deals with all samples only as samples relative to other samples, and doesn’t get thrown off by isolated samples that don’t represent the actual waveform. It’s got a sound, but the sound is hard to define because of its extreme fluidity and purity.If this sounds like your idea of fun, enjoy ButterComp. If you’re looking for the ‘all buttons in’ mode, I’ll get back to the drawing board and probably do something totally different for you. This compressor is not for everybody. Also, if you start slamming it really hard (demonstrated in the video), it’ll volume invert: you can squish it down to become more quiet than the quiet passages, but still it will retain its tone quality. It’s perhaps best used as a particular kind of ‘glue’ compressor, at which it is exceptional… or maybe I should say, it’s unique. Start working with it and you’ll soon work out whether you’re part of the ButterComp cult. Not everybody will be, and it’s only one type of sound… but nothing else can get that sound, and this is why I’ve kept this purest form of the algorithm available."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class ButterComp final : public Effect<T>
{
    double controlAposL;
    double controlAnegL;
    double controlBposL;
    double controlBnegL;
    double targetposL;
    double targetnegL;
    double controlAposR;
    double controlAnegR;
    double controlBposR;
    double controlBnegR;
    double targetposR;
    double targetnegR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    ButterComp()
    {
        A = 0.0;
        B = 1.0;
        controlAposL = 1.0;
        controlAnegL = 1.0;
        controlBposL = 1.0;
        controlBnegL = 1.0;
        targetposL = 1.0;
        targetnegL = 1.0;
        controlAposR = 1.0;
        controlAnegR = 1.0;
        controlBposR = 1.0;
        controlBnegR = 1.0;
        targetposR = 1.0;
        targetnegR = 1.0;
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
            case kParamA: return "compress"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Compress"; break;
            case kParamB: return "Dry/Wet"; break;

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
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double inputposL;
        double inputnegL;
        double calcposL;
        double calcnegL;
        double outputposL;
        double outputnegL;
        double totalmultiplierL;
        double inputSampleL;
        double drySampleL;
        double inputposR;
        double inputnegR;
        double calcposR;
        double calcnegR;
        double outputposR;
        double outputnegR;
        double totalmultiplierR;
        double inputSampleR;
        double drySampleR;
        double inputgain = pow(10.0, (A * 14.0) / 20.0);
        double wet = B;
        // removed extra dry variable
        double outputgain = inputgain;
        outputgain -= 1.0;
        outputgain /= 1.5;
        outputgain += 1.0;
        double divisor = 0.012 * (A / 135.0);
        divisor /= overallscale;
        double remainder = divisor;
        divisor = 1.0 - divisor;
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
            inputSampleL *= inputgain;
            inputSampleR *= inputgain;
            inputposL = inputSampleL + 1.0;
            if (inputposL < 0.0) {
                inputposL = 0.0;
            }
            outputposL = inputposL / 2.0;
            if (outputposL > 1.0) {
                outputposL = 1.0;
            }
            inputposL *= inputposL;
            targetposL *= divisor;
            targetposL += (inputposL * remainder);
            calcposL = pow((1.0 / targetposL), 2);
            inputnegL = (-inputSampleL) + 1.0;
            if (inputnegL < 0.0) {
                inputnegL = 0.0;
            }
            outputnegL = inputnegL / 2.0;
            if (outputnegL > 1.0) {
                outputnegL = 1.0;
            }
            inputnegL *= inputnegL;
            targetnegL *= divisor;
            targetnegL += (inputnegL * remainder);
            calcnegL = pow((1.0 / targetnegL), 2);
            // now we have mirrored targets for comp
            // outputpos and outputneg go from 0 to 1
            inputposR = inputSampleR + 1.0;
            if (inputposR < 0.0) {
                inputposR = 0.0;
            }
            outputposR = inputposR / 2.0;
            if (outputposR > 1.0) {
                outputposR = 1.0;
            }
            inputposR *= inputposR;
            targetposR *= divisor;
            targetposR += (inputposR * remainder);
            calcposR = pow((1.0 / targetposR), 2);
            inputnegR = (-inputSampleR) + 1.0;
            if (inputnegR < 0.0) {
                inputnegR = 0.0;
            }
            outputnegR = inputnegR / 2.0;
            if (outputnegR > 1.0) {
                outputnegR = 1.0;
            }
            inputnegR *= inputnegR;
            targetnegR *= divisor;
            targetnegR += (inputnegR * remainder);
            calcnegR = pow((1.0 / targetnegR), 2);
            // now we have mirrored targets for comp
            // outputpos and outputneg go from 0 to 1
            if (inputSampleL > 0)
            { // working on pos
                controlAposL *= divisor;
                controlAposL += (calcposL * remainder);
            }
            else
            { // working on neg
                controlAnegL *= divisor;
                controlAnegL += (calcnegL * remainder);
            }
            // this causes each of the four to update only when active and in the correct 'flip'
            if (inputSampleR > 0)
            { // working on pos
                controlAposR *= divisor;
                controlAposR += (calcposR * remainder);
            }
            else
            { // working on neg
                controlAnegR *= divisor;
                controlAnegR += (calcnegR * remainder);
            }
            // this causes each of the four to update only when active and in the correct 'flip'
            totalmultiplierL = (controlAposL * outputposL) + (controlAnegL * outputnegL);
            totalmultiplierR = (controlAposR * outputposR) + (controlAnegR * outputnegR);
            // this combines the sides according to flip, blending relative to the input value
            inputSampleL *= totalmultiplierL;
            inputSampleL /= outputgain;
            inputSampleR *= totalmultiplierR;
            inputSampleR /= outputgain;
            if (wet != 1.0) {
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
} // namespace airwindohhs::buttercomp
