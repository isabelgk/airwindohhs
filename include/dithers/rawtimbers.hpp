#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::rawtimbers {

constexpr std::string_view k_name{ "RawTimbers" };
constexpr std::string_view k_short_description{
    "RawTimbers is just the quantization from Dither Me Timbers… and the opposite, as a brightener."
};
constexpr std::string_view k_long_description{
    "So, Dither Me Timbers isn’t really a dither. I keep saying that even though it’s got noise shaping and gets a sound that stands up to comparison with Not Just Another Dither: it’s different, but good.This is ‘essence of Dither Me Timbers’, no noise shaping… and the inverse, just because I can.All RawTimbers does, is choose between truncating to fixed point upwards or downwards, from any given point. Like Dither Me Timbers, it runs one sample of latency in order to do this choosing. Unlike Dither Me Timbers, it has an opposite plugin: RawGlitters. I see this stuff as ‘level-dependent EQ’ and always thought it seemed useful to dull and soften the quietest sounds and let loud noises seem brighter: it didn’t make sense to me to brighten the quietest sounds and let loud noises be duller (might be worth a rethink of this, but I’ve been sick this week and can only do so much: this is after all vanguard stuff and I need to get it right, or try harder to get it right)Anyhow, this is RawTimbers and RawGlitters. It does the exact same thing as Ditherbox ‘Truncation, 24 bit’ except it’s an EQ: RawTimbers softens the audio and rolls off highs (and actually generates more and different bright quantization noise, so it’s not like it makes the noise floor itself seem darker), and RawGlitters brightens the hell out of the audio and sort of merges it with the noise floor. Both drastically change the nature and quality of the truncation noise floor: they’re undithered, and each is super different from plain old truncation. I discovered making the video that if you hit RawGlitters first and run that into RawTimbers, you get a third sound that’s interesting and pretty good… and if you run RawTimbers into RawGlitters, it’s pretty terrible but I’m not your mom, go ahead if you want. :)In listening to these, remember the intense EQ effect (as in, cut treble up to 90% relative to mids and bass) happens ONLY at a fixed loudness that is barely over the 24 bit noise floor. If you play around with BitShiftGain you can check it out more easily, as I did on the video. All the effects only touch the noise floor area. These don’t have noise shaping so they can’t change overs or add excess energy: they’re ‘safe’ in that they’re predictable behavior, though you might have an interesting time exploring what they do to the tone.They can be used for 16 bit CD-making as seen in the video: BitShiftGain -8 bits, RawTimbers/Glitters, BitShiftGain +8 bits gives you exact CD dithering.They can also be used for conclusive double blind testing: since this is strictly two opposite ways of quantizing audio to the same fixed point output, you can make matching files for use in ABX testers. Use BitShiftGain to dial in the output bit depth you want, save all the results at 24 bit if you like (the quantizations will be the same) and listen for the brightening and difference in atmosphere of RawGlitters. On an ABX tester you’ll be able to audition Timbers and Glitters all you like, and compare to X, which will be one of the two. You can use quiet sounds if you like, to help hear what’s happening: it’s totally legit to tailor audio that will reveal this stuff more easily, as it’s the threshold you’re interested in. I think it shows class to use volume levels where you could still stand the loudness of normal audio and not blow your speakers, even if the actual test audio is way quieter (cranking it up will help, and you don’t have to blow your eardrums up as part of the comparison process, just keep it sane enough that the loud bits wouldn’t be damaging)If you do this and correctly predict X for whatever bit depth you’re attempting, you’ve conclusively shown that you can always hear stuff at that bit depth and nobody can contradict you. I’m pretty sure literally everybody will hear 12 bit even over YouTube on whatever casual listening stuff you’re using. I also think 14 bit is relatively trivial, and 16 bit is doable given the right example audio and some care and attention (this is a contentious claim, but I still think so). I don’t think anybody can actually do this with full-on 24 bit audio… but if you did, over whatever fantastic (and very treble-accurate, and very low-noise) mastering rig you used, it would be conclusive proof of it.And you still ought to dither, even to 24 bit, ‘cos it’s the principle of the thing. But this is something different to do, and it’s a legitimate choice. I suspect there’ll be a lot of electronic musicians who take a liking to RawGlitters just because it airs up the digital noise floor in an interesting way…"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class RawTimbers final : public Effect<T>
{
    double lastSampleL;
    double lastSample2L;
    double lastSampleR;
    double lastSample2R;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    RawTimbers()
    {
        A = 1.0;
        B = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        lastSampleL = 0.0;
        lastSample2L = 0.0;
        lastSampleR = 0.0;
        lastSample2R = 0.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "quant"; break;
            case kParamB: return "derez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Quant"; break;
            case kParamB: return "DeRez"; break;

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

        int processing = (VstInt32)(A * 1.999);
        bool highres = false;
        if (processing == 1) {
            highres = true;
        }
        float scaleFactor;
        if (highres) {
            scaleFactor = 8388608.0;
        }
        else {
            scaleFactor = 32768.0;
        }
        float derez = B;
        if (derez > 0.0) {
            scaleFactor *= pow(1.0 - derez, 6);
        }
        if (scaleFactor < 0.0001) {
            scaleFactor = 0.0001;
        }
        float outScale = scaleFactor;
        if (outScale < 8.0) {
            outScale = 8.0;
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            double outputSampleL;
            double outputSampleR;
            inputSampleL += 0.381966011250105;
            inputSampleR += 0.381966011250105;
            if ((lastSampleL + lastSampleL) >= (inputSampleL + lastSample2L)) {
                outputSampleL = floor(lastSampleL);
            }
            else {
                outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
            }
            if ((lastSampleR + lastSampleR) >= (inputSampleR + lastSample2R)) {
                outputSampleR = floor(lastSampleR);
            }
            else {
                outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
            }
            lastSample2L = lastSampleL;
            lastSampleL = inputSampleL; // we retain three samples in a row
            lastSample2R = lastSampleR;
            lastSampleR = inputSampleR; // we retain three samples in a row
            *out1 = outputSampleL / outScale;
            *out2 = outputSampleR / outScale;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::rawtimbers
