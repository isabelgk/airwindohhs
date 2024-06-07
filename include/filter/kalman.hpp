#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::kalman {

constexpr std::string_view k_name{ "Kalman" };
constexpr std::string_view k_short_description{
    "Kalman is not a real filter!"
};
constexpr std::string_view k_long_description{
    "So… come with me, if you will, on this journey.There's a huge amount of audacious Airwindowsness that hinges on this little bit of code that was never meant to work for audio purposes. This isn't just from finance or science, it's literally from GPS satellites and their care and feeding.Meet Kalman. To the best of my knowledge, nobody has this as an audio filter up to now. And now you do, and you're staring at it going 'wait, what?', and rightly so. At first glance this ticks none of the boxes, and then it sounds awful.What's happening? Kalman is 'filtering', but really it's trying to predict a 'real' position from what it sees as hopelessly noisy data. The thing is, it's the audio input data. None of it is 'wrong' in any sense. In other situations I go to great lengths to preserve every detail, and here Kalman is, trying to get rid of 'mistakes' that aren't even mistakes.However, this is also how Air3 got its amazing ability to extract or suppress the air band. So what happens when you're sweeping it from the highs right down through the midrange, into bass and beyond?Bad things. It neither knows nor cares what a frequency is, or whether it's producing a filter slope. Really, it's not. It's producing wavelike outputs like it was charting a GPS course and only roughly correlating them to audio frequencies. It won't produce sharp breaks in its curve, but the farther you go down, the more its 'steering' will interfere with the audio, producing a sort of grinding, overbearing solidity, like the audio was freezing solid or turning to stone.This is not even an audio filter, remember. I'm sure anyone who's tried this, abandoned it, immediately, because you set it to full-wet and it immediately sounds awful.More fools they!This thing is best experienced not as a synth-style filter, not some sweepable musical thing, but as a crossover. You can hear either it, or on the 'inv' setting, its opposite. You can subtract its output from the dry signal, and then recombine them again. Works just fine in fact. And that's where things get really interesting.Full wet produces this unbearably heavy, petrifyingly solid, unyielding sound. Up higher it's an exaggerated version of the Air3 roll-off. It makes everything dead, weighty, even when higher frequencies burst through in artifacts it's like they're the bones of the sound you started with. It's a really strong texture.When you subtract it, you've got this hot, lively, aggressive texture. It's fiery, energetic, a little like my old plugin Cojones which identified unpredictable elements in the sound in the treble regions. This will do it at any tonal range: I'd say frequency, but the whole effect defies frequency. You can have opposite tone colors with your opposite frequency ranges: low/stone, high/fire. At the extremes, the effect is so aggressive it's disruptive and sounds broken and wrong.And then… just blend them more subtly, and Kalman shows what it's really for.When both elements are present (in this one, it's inv/wet settings that are near the center and mostly dry, but it'd also work as Fire/Stone faders meant to stay mostly balanced) you can lean the whole texture in different directions across an amazingly wide range of basic tonality. You can fool with just the deep subs, making them monumental or leaning them out. You can darken abrasive upper mids, or mid-mids, or, heck, lower mids if you like. Above the range will be more or less Fire, below will be more or less Stone. The range, however, will never be just an isolated 'cutoff frequency' because the whole thing completely transforms that whole range.If you can transform how you hear away from 'frequency' and hear things in terms of texture, you can apply sound changes completely unattainable by normal filtering, no matter how complex. And the neat thing is, since it's a form of crossover, it excels at subtlety. And since the range options (shown here as simply 'Kalman') are so wide, you can transform texture of all sorts, not just 'stone = heavy = weight = bass'. Heck no! You can adjust a piano to have weightier low midrange if you like, or take a drum room sound and make everything over the bass frequencies more fiery. You can take a big guitar amp, crank the weight, and then just sneak up the Kalman knob until the brutality of the lows are undeniable.There will be more like this, as seen in the video. But Kalman is here now. You're welcome to begin playing with it. If it fights you, don't give up on it too easy. You might be able to do more with less. I think in many cases that inv/wet control is going to hover nearly exactly halfway. It won't take much to get this plugin doing obvious texture shaping. The rest is up to you…"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Kalman final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        prevSampL1,
        prevSlewL1,
        accSlewL1,
        prevSampL2,
        prevSlewL2,
        accSlewL2,
        prevSampL3,
        prevSlewL3,
        accSlewL3,
        kalGainL,
        kalOutL,
        prevSampR1,
        prevSlewR1,
        accSlewR1,
        prevSampR2,
        prevSlewR2,
        accSlewR2,
        prevSampR3,
        prevSlewR3,
        accSlewR3,
        kalGainR,
        kalOutR,
        kal_total

        public :
            Kalman(){
                A = 0.5;
                B = 1.0;
                for (int x = 0; x < kal_total; x++) kal[x] = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 1.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "kalman";
            break;
        kParamB:
            return "inv/wet";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Kalman";
            break;
        kParamB:
            return "Inv/Wet";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;

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
        double kalman = 1.0 - pow(A, 2);
        double wet = (B * 2.0) - 1.0; // inv-dry-wet for highpass
        double dry = 2.0 - (B * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
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
            // begin Kalman Filter
            double dryKal = inputSampleL = inputSampleL * (1.0 - kalman) * 0.777;
            inputSampleL *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2];
            kal[prevSlewL3] *= 0.5;
            kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1];
            kal[prevSlewL2] *= 0.5;
            kal[prevSlewL1] += kal[prevSampL1] - inputSampleL;
            kal[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2];
            kal[accSlewL2] *= 0.5;
            kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1];
            kal[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]);
            kal[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3];
            kal[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainL] += fabs(dryKal - kal[kalOutL]) * kalman * 8.0;
            kal[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainL] > kalman * 0.5) {
                kal[kalGainL] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutL] += (dryKal * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampL3] = kal[prevSampL2];
            kal[prevSampL2] = kal[prevSampL1];
            kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0 - kal[kalGainL]) * dryKal);
            // feed the chain of previous samples
            if (kal[prevSampL1] > 1.0) {
                kal[prevSampL1] = 1.0;
            }
            if (kal[prevSampL1] < -1.0) {
                kal[prevSampL1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleL = (drySampleL * dry) + (kal[kalOutL] * wet * 0.777);
            // now the right channel
            dryKal = inputSampleR = inputSampleR * (1.0 - kalman) * 0.777;
            inputSampleR *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2];
            kal[prevSlewR3] *= 0.5;
            kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1];
            kal[prevSlewR2] *= 0.5;
            kal[prevSlewR1] += kal[prevSampR1] - inputSampleR;
            kal[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2];
            kal[accSlewR2] *= 0.5;
            kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1];
            kal[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]);
            kal[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3];
            kal[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainR] += fabs(dryKal - kal[kalOutR]) * kalman * 8.0;
            kal[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainR] > kalman * 0.5) {
                kal[kalGainR] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutR] += (dryKal * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampR3] = kal[prevSampR2];
            kal[prevSampR2] = kal[prevSampR1];
            kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0 - kal[kalGainR]) * dryKal);
            // feed the chain of previous samples
            if (kal[prevSampR1] > 1.0) {
                kal[prevSampR1] = 1.0;
            }
            if (kal[prevSampR1] < -1.0) {
                kal[prevSampR1] = -1.0;
            }
            // end Kalman Filter, except for trim on output
            inputSampleR = (drySampleR * dry) + (kal[kalOutR] * wet * 0.777);
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
} // namespace airwindohhs::kalman
