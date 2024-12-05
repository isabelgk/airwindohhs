#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pyewacket {

constexpr std::string_view k_name{ "Pyewacket" };
constexpr std::string_view k_short_description{
    "Pyewacket is an old school compressor for high definition transients. Adds no fatness, just energy."
};
constexpr std::string_view k_long_description{
    "Pyewacket is a strange beast. It’s inspired by how much I love the 60s/70s recordings out of London’s Olympic Studios, which had and used Pye compressors on many of my favorite classic and prog-rock records. Once you recognize the sound, nothing else will do: the musical event is delineated with hallucinatory intensity.Mind you, for ten or twenty THOUSAND dollars it had damn well better hallucinate musical events on command: these are not compressors normal people can have, not anymore.However, I’m ‘chris from airwindows’, so for me it’s not just a matter of mimicking the faceplate or even the specific behaviors of the device. I want something more original, that can get the essence of that electrifying sound. I might not play like a musical hero, but I want a compressor that can deliver that crackling voltage. And as I was listening to examples of a homebrew Pye replica, it suddenly hit me: I know how to make a compressor cut back just the body of the sound, leaving that energy and transient definition. I can also bring in the ‘brickwall filter’ behavior the Pyes have, as needed. And I have a whole life of devoted music listening off classic vinyl records to guide me. I can get the sound.Introducing Pyewacket. Pyewacket is my compressor familiar. It may or may not have dark magic, but what it does have (demonstrated at the end of my video) is a response and tonality like no compressor you’ve heard. I can contrast it with Pressure4, and have done: where a more ’round and thick’ comp like Pressure4 brings stuff forward, Pyewacket’s soundstage sits back and the energy comes forward, from the highest treble to deep hard-kicking bass, producing a ‘retro’ sound where peak energy absolutely blows away the more thick, tubby RMS loudness. This is a compressor for a new era. We’ve been doing ‘loud and fat’ for decades now, and the loudness war is on its last legs, with automatic playback gain controls rendering it useless. You don’t have to be composing retro to use this. The only requirement is energy and information: whether as a 2-buss comp or to condition individual tracks, Pyewacket brings focus and intensity, and an incredibly clear and articulate attack transient where most compressors mangle and transform the attack beyond recognition.And if you try really hard, yes you can kinda-sorta make it do that ‘Hole In My Shoe’ gratituous pumping thing. Rest assured, though: you probably shouldn’t.Other people can’t do this plugin. You can’t market it in normal ways because it doesn’t do ‘BIG PHAT THICK PHWOAARRR’, you don’t switch it in and have all the music leap forward and become much bigger and in fact it might make things smaller, and an inexperienced kid with softsynths and Apple Loops might think something was amiss and be extremely uninspired. And anyone trying to tie it to the twenty thousand dollar unattainable hardware compressor would be compelled to model every little detail of the very complex and twitchy hardware unit, and that would cause that plugin to be overprocessed and it’d lose most of what made it special.But Pyewacket is important, because it’s the sort of thing I can do when supported by Patreon. I don’t have to restrict myself to what’s going to sell to blind market forces. I can make it the essence of how Airwindows would do this sound, and I have done. As such, it is free in AU, Mac and PC VST form. If I’m poorer than you (go check on the Patreon and see, I get paid monthly) then it might be worth your while to chip in a buck a month (or more if you like).I really, really, really like this one, and maybe you will too :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Pyewacket final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double chase;
    double lastrectifierL;
    double lastrectifierR;
    float A;
    float B;
    float C;

  public:
    Pyewacket()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        chase = 1.0;
        lastrectifierL = 0.0;
        lastrectifierR = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input gain"; break;
            case kParamB: return "release"; break;
            case kParamC: return "output gain"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Gain"; break;
            case kParamB: return "Release"; break;
            case kParamC: return "Output Gain"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return " "; break;
            case kParamC: return "dB"; break;

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
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double bridgerectifier;
        double temprectifier;
        double inputSense;
        double inputGain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
        double attack = ((B + 0.5) * 0.006) / overallscale;
        double decay = ((B + 0.01) * 0.0004) / overallscale;
        double outputGain = pow(10.0, ((C * 24.0) - 12.0) / 20.0);
        double wet;
        double maxblur;
        double blurdry;
        double out;
        double dry;
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
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSense = fabs(inputSampleL);
            if (fabs(inputSampleR) > inputSense) {
                inputSense = fabs(inputSampleR);
            }
            // we will take the greater of either channel and just use that, then apply the result
            // to both stereo channels.
            if (chase < inputSense) {
                chase += attack;
            }
            if (chase > 1.0) {
                chase = 1.0;
            }
            if (chase > inputSense) {
                chase -= decay;
            }
            if (chase < 0.0) {
                chase = 0.0;
            }
            // chase will be between 0 and ? (if input is super hot)
            out = wet = chase;
            if (wet > 1.0) {
                wet = 1.0;
            }
            maxblur = wet * fpNew;
            blurdry = 1.0 - maxblur;
            // scaled back so that blur remains balance of both
            if (out > fpOld) {
                out = fpOld - (out - fpOld);
            }
            if (out < 0.0) {
                out = 0.0;
            }
            dry = 1.0 - wet;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            temprectifier = 1 - cos(bridgerectifier);
            bridgerectifier = ((lastrectifierL * maxblur) + (temprectifier * blurdry));
            lastrectifierL = temprectifier;
            // starved version is also blurred by one sample
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * dry) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * dry) - (bridgerectifier * out);
            }
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            temprectifier = 1 - cos(bridgerectifier);
            bridgerectifier = ((lastrectifierR * maxblur) + (temprectifier * blurdry));
            lastrectifierR = temprectifier;
            // starved version is also blurred by one sample
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * dry) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * dry) - (bridgerectifier * out);
            }
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
} // namespace airwindohhs::pyewacket
