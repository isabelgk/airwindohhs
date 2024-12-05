#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::highimpact {

constexpr std::string_view k_name{ "HighImpact" };
constexpr std::string_view k_short_description{
    "HighImpact is distorted grit and punch without fatness. This is also another little window into Airwindows history, as this is the point where I began putting out the for-pay plugins for free through Patreon. The post is presented in its entirety as a picture of where I was at on April 9, 2017. By now I have put out more than 300 plugins and I'm still going, but this was when I'd done 47 and all the most sought-after ones were still being held back as goals… here is the original post."
};
constexpr std::string_view k_long_description{
    "Sometimes it’s all in the name.High Impact has been one of the Airwindows secret weapons for a long time. It’s a pretty basic idea: know how Density distorts and makes audio huge and fat, and Drive distorts and doesn’t make stuff as fat? High Impact answers the question, ‘what if you could distort and not make things fatter AT ALL’.This is not EQ I’m talking about, though it can come off like EQ depending on what you feed into it. High Impact combines a distortion and anti-distortion circuit to reshape the transfer function of the audio, as follows: crank it up, and quiet subtle samples aren’t turned up much. Then medium samples are turned up a LOT, and then the loudest samples are distorted and hit a wall past which they can’t go. The result is an obvious distort which doesn’t bloat things. It’s a ‘dial-a-rasp’, or ‘dial-a-slam’ for percussive noises, and though the concept is two different kinds of overdrive combined, it’s an Airwindows plugin so it’s not overprocessed, it’s super raw.That suits this plugin super well, and that’s why the AU-only High Impact has been a secret weapon for many people for years—and now it’s out for Mac and PC VST.A word about the ‘secret weapon’ thing: I hate that. Do as I do, and communicate openly. Airwindows plugins are not preset things that you hide somewhere to add magic sauce to your mixes. They require intention, and there’s no one right way to use them: Airwindows plugins are like if a certain mic modeler you can rent that offers ‘silky expensive microphone models’ had just a blank ugly faceplate with just one knob, ‘silky’. And you could use it as a guitar stompbox if you wanted, not even a microphone anymore, and it was no longer connected to a ‘magic gear item’ so you’d have to ask, ‘HOW silky does this sound need to be, in my mix?’ because there was no one right answer.That’s what Airwindows is like. For High Impact, read ‘raspy’ or ‘mean’ or ‘grindy’, whatever describes the sound for you. Now you’ve got an extra parameter: instead of just bass, treble, loud, soft (or even fat and thin) you’ve got ‘aggro’ on a knob. That can relate to any bit of audio you’re working with… and your ‘secret sauce’ is not the plugin, but your sense of taste (or tastelessness) in using and abusing it. So, don’t keep Airwindows plugins a secret weapon. I hate that. Tell people what you’re using, so I can get more supporters and keep doing all this, on a bigger and bigger scale.Speaking of which, I have a Patreon milestone! As of a couple days ago, I hit $600 a month, and I’ve recently changed that to be the point where I start releasing the Kagi catalog! Now, people sometimes bail as the first of the month rolls around (and also I don’t get paid quite the full amount shown). So, it’s possible this milestone will go poof as we reach May, in which case we’re still waiting. BUT, if the dust settles and it’s still over $600 going into May, I will put out Iron Oxide 4 (the second most popular plugin I have EVER made) as free AU/VST!(oh my God, I look so young in that video o_O )If I’m still over $600 when May begins, you get THAT plugin free. My hope is that, as I reach this stage, people get more of an idea of what’s possible out of my Patreon project. All this time, I’ve been putting out more than 47 plugins entirely from the ‘freebie pile’, and not touching the ones that kept me in business a decade. Now, we start to get into the serious ones, the ones worth $50 to a lot of people. I’ve even drawn up a timeline on the Patreon, showing when each plugin will come out if I stay above $600. (if I clear $1000 they go twice as fast and I’ll revise the timeline accordingly!)Here’s where things REALLY get interesting. Hang on to your DAWs, because we’re about to go full warp drive :)"
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class HighImpact final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double lastSampleR;
    float A;
    float B;
    float C;

  public:
    HighImpact()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "impact"; break;
            case kParamB: return "output level"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Impact"; break;
            case kParamB: return "Output Level"; break;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double density = A * 5.0;
        double out = density / 5.0;
        double sustain = 1.0 - (1.0 / (1.0 + (density * A)));
        double bridgerectifier;
        double count;
        double output = B;
        double wet = C;
        double dry = 1.0 - wet;
        double clamp;
        double threshold = (1.25 - out);
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
            count = density;
            while (count > 1.0)
            {
                bridgerectifier = fabs(inputSampleL) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleL > 0.0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleR > 0.0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
                count = count - 1.0;
            }
            // we have now accounted for any really high density settings.
            while (out > 1.0) {
                out = out - 1.0;
            }
            bridgerectifier = fabs(inputSampleL) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (density > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (density > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            // done first density. Next, sustain-reducer
            bridgerectifier = fabs(inputSampleL) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = 1 - cos(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - sustain)) + (bridgerectifier * sustain);
            }
            else {
                inputSampleL = (inputSampleL * (1 - sustain)) - (bridgerectifier * sustain);
            }
            // done sustain removing, converted to Slew inputs
            // done first density. Next, sustain-reducer
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = 1 - cos(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - sustain)) + (bridgerectifier * sustain);
            }
            else {
                inputSampleR = (inputSampleR * (1 - sustain)) - (bridgerectifier * sustain);
            }
            // done sustain removing, converted to Slew inputs
            clamp = inputSampleL - lastSampleL;
            if (clamp > threshold) {
                inputSampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleL - threshold;
            }
            lastSampleL = inputSampleL;
            clamp = inputSampleR - lastSampleR;
            if (clamp > threshold) {
                inputSampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleR - threshold;
            }
            lastSampleR = inputSampleR;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
} // namespace airwindohhs::highimpact
