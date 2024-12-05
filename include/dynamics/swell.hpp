#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::swell {

constexpr std::string_view k_name{ "Swell" };
constexpr std::string_view k_short_description{
    "Swell is Dial-an-attack, like sidechaining."
};
constexpr std::string_view k_long_description{
    "Every now and then you come up with something a little different. The fun of that is in identifying it.I’ve been listening to house music by Deadmau5, and I took an interest in the way the sidechaining worked. (That’s the way the music, or the reverb, or the pads, get ‘smooshed’ down by the kick drum and then swing back up in volume again.) The Mau5 is pretty good about talking about his techniques, which I appreciate (I’d love to do a coffee run with that guy someday) and he’s spent some time showing people how instead of actually sidechaining, he automates an LFO which he applies to track volume. It gets a great effect.I wondered whether it’d work to do a gate that intentionally opened real slow. Specifically, it seemed possible to treat ‘full volume’ as a ‘floor’ and have a ‘decay’ that falls up toward 1.0 rather than down to 0. It’d never quite get there, but it could have a similar envelope as the fall-off of natural reverb, just upside down.If that sounds odd, don’t worry about it, just try Swell. It has a threshold like a gate, and the Swell control handles the attack speed once the gate opens. The dry/wet works as you’d expect. Sort of like Deadmau5’s LFO trick, it doesn’t have to be a real sidechain, and is independent of whatever kick you have going on. Unlike the LFO trick, it’s also not tied to tempo: Swell reacts to EVERY attack that comes along, provided the threshold’s set right.What this does is very interesting. I expect to see this turn up in an EDM channel strip at some point. Essentially, you can play with the controls in various ways and completely step on the attack of anything you want. This relates to GROOVE and the layering of stuff in a dance mix: any element, no matter what it is, can be turned into a pad and back again, just by squishing away its attack. You can do it live, you can take an element (snare, punchy chord) and manipulate how it hits inside the groove. Extreme values make stuff extra soft and quiet and squishy, and then if you drop the threshold or Swell, you can have the element jump right back out again, and the control is direct, not relative to a sidechained track or LFO setting. It’s a more organic approach, more hands-on, and I think it’ll fit into a lot of people’s mixes. There’s always a place for balancing the intensity of the attacks of your tracks, and that’s literally what Swell does."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Swell final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double swellL;
    double swellR;
    bool louderL;
    bool louderR;
    float A;
    float B;
    float C;

  public:
    Swell()
    {
        A = 0.9;
        B = 0.5;
        C = 1.0;
        swellL = 0.0;
        swellR = 0.0;
        louderL = false;
        louderR = false;
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
            case kParamA: return 0.9; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "thresh"; break;
            case kParamB: return "swell"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thresh"; break;
            case kParamB: return "Swell"; break;
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
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;

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
        double thresholdOn = pow(A, 2) * B;
        double speedOn = (pow(B, 2) * 0.001) / overallscale;
        double thresholdOff = thresholdOn * B;
        double speedOff = (sin(B) * 0.01) / overallscale;
        double wet = C;
        // removed extra dry variable
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            if (fabs(inputSampleL) > thresholdOn && louderL == false) {
                louderL = true;
            }
            if (fabs(inputSampleL) < thresholdOff && louderL == true) {
                louderL = false;
            }
            if (louderL == true) {
                swellL = (swellL * (1.0 - speedOn)) + speedOn;
            }
            else {
                swellL *= (1.0 - speedOff);
            }
            // both poles are a Zeno's arrow: approach but never get to either 1.0 or 0.0
            inputSampleL *= swellL;
            if (fabs(inputSampleR) > thresholdOn && louderR == false) {
                louderR = true;
            }
            if (fabs(inputSampleR) < thresholdOff && louderR == true) {
                louderR = false;
            }
            if (louderR == true) {
                swellR = (swellR * (1.0 - speedOn)) + speedOn;
            }
            else {
                swellR *= (1.0 - speedOff);
            }
            // both poles are a Zeno's arrow: approach but never get to either 1.0 or 0.0
            inputSampleR *= swellR;
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
} // namespace airwindohhs::swell
