#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hypersoft {

constexpr std::string_view k_name{ "Hypersoft" };
constexpr std::string_view k_short_description{
    "Hypersoft is a more extreme form of soft-clipper."
};
constexpr std::string_view k_long_description{
    "A funny thing happened on the way to posting the video for this…Hypersoft is a new softclipper. Its purpose was to answer the question 'what if a softclipper, that develops a sharper and sharper corner as you overdrive it, placed that corner NOT up at the maximum volume, but somewhere else?'I feel like this isn't investigated because it's 'wasting' the distortion, assigning it to quieter parts of the sound, and that it's been such an assumption that everything be LOUD, that nobody would even try such a weird experiment. But I've been working on Airwindows Meter, and looking at how hit records in my collection produce these 'clouds' of peak energy and slew energy that always take up more space than modern mixes allow. And so, I wondered: what even do you get, if you used soft-clipping and techniques like wavefolding to produce a 'curve' where the 'flat-topping' instead became a circle or something, and it tries to get progressively sharper somewhere in the middle?So I started working with test tones, sine waves, and hashed it out for a while on livestreams and made a thing that produced this curve, plus the wavefolding caused the 'flat top' that you eventually hit, to stick around longer than it normally would, and then curve down into a strange point. When you send a ramping-up louder and louder sine into this, the harmonics it produces go from very soft and mellow, to more sharp and high-frequency, based on how loud the sine was. (compare to a hardclip, where you get highs immediately and then they mostly don't change.)On music, it's a wildly grungey dirt-factory that's almost as colorful as class AB distortion, for some of the same reasons. The place that the 'curve' changes most sharply is no longer at the top. But it's still a softclip, so quiet sounds don't grind as they would in class AB distortion. It goes from subtly punchy and textural, all the way to aggressively gnarly.And then I made a video and used a simple room-miced drum beat, a basic boom-bap played as well as I could, just two mics placed in front of the kit, and cranked up Hypersoft until I thought it sounded pretty amazing, thought I'd be able to use something like that.And ContentID flagged it as Rammstein, 'Das Alte Leid' and refused to let me monetize it. (ads that run are out of my hands now, and go to Rammstein.) With no other processing at all, my simple drum playing has apparently become Rammstein enough to fool YouTube. The raw mic feed wouldn't do that at all: Rammstein is a highly processed modern-metal sound. But there it was.What's happening is this: drum impacts are being allowed to persist (like in the highly sculpted sound) but the body of the sound is brought up and reshaped interestingly, producing a 'big' effect that acts kind of like the sound is experienced in real life. Pushed hard enough the dynamics will invert, but it'll take a lot to do that. It's a new form of softclip which I think lends itself to drums, but will probably also suit basses.You get Input, Depth, Bright and Output. 0.5 is as neutral as it gets though the effect will add a little boost just by reshaping. Depth specifies how many additional wavefoldings to apply (each runs its own sin() function so high settings will cost a bit more CPU) and more Depth intensifies the effect, making it more focused and colorful. Adding Bright means all the wavefoldings will be used even if they're going to alias: less Bright means the wavefoldings start to bail out when they risk aliasing. Note that this WILL NOT fix aliasing! It changes the tone in such a way that you can dial in the amount of treble hype you get out of Hypersoft, or leave it as a darker effect.This will not be as effective a loudenator as just clipping the crud out of something, before you ask. This adds harmonics and intensity while retaining dynamics. In fact it tries to heighten dynamics by throwing more overtones when the overload is greater. I think it's going to be really, really useful but not for simple loudenating: if you're after intensification and impact, Hypersoft might come in handy. I hope you find it helpful :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Hypersoft final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Hypersoft()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;
            case kParamB: return "depth"; break;
            case kParamC: return "bright"; break;
            case kParamD: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;
            case kParamB: return "Depth"; break;
            case kParamC: return "Bright"; break;
            case kParamD: return "Output"; break;

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

        double inputGain = A * 2.0;
        if (inputGain > 1.0) {
            inputGain *= inputGain;
        }
        else {
            inputGain = 1.0 - pow(1.0 - inputGain, 2);
        }
        // this is the fader curve from ConsoleX with 0.5 being unity gain
        int stages = (int)(B * 12.0) + 2;
        // each stage brings in an additional layer of harmonics on the waveshaping
        double bright = (1.0 - C) * 0.15;
        // higher slews suppress these higher harmonics when they are sure to just alias
        double outputGain = D * 2.0;
        if (outputGain > 1.0) {
            outputGain *= outputGain;
        }
        else {
            outputGain = 1.0 - pow(1.0 - outputGain, 2);
        }
        outputGain *= 0.68;
        // this is the fader curve from ConsoleX, rescaled to work with Hypersoft
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
            inputSampleL *= inputGain;
            inputSampleR *= inputGain;
            inputSampleL = sin(inputSampleL);
            inputSampleL += (sin(inputSampleL * 2.0) / 2.0);
            inputSampleR = sin(inputSampleR);
            inputSampleR += (sin(inputSampleR * 2.0) / 2.0);
            for (int count = 2; count < stages; count++) {
                inputSampleL += ((sin(inputSampleL * (double)count) / (double)pow(count, 3)) * fmax(0.0, 1.0 - fabs((inputSampleL - lastSampleL) * bright * (double)(count * count))));
                inputSampleR += ((sin(inputSampleR * (double)count) / (double)pow(count, 3)) * fmax(0.0, 1.0 - fabs((inputSampleR - lastSampleR) * bright * (double)(count * count))));
            }
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            inputSampleL *= outputGain;
            inputSampleR *= outputGain;
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
} // namespace airwindohhs::hypersoft
