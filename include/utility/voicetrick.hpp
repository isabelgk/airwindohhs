#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::voicetrick {

constexpr std::string_view k_name{ "VoiceTrick" };
constexpr std::string_view k_short_description{
    "VoiceTrick lets you record vocals while monitoring over speakers."
};
constexpr std::string_view k_long_description{
    "Record your vocals without headphones with This One Weird Trick! :D…No, really, that’s actually the idea. I have to explain the trick or the plugin will make no sense at all, but if you know what to do you can record vocals with monitor speakers or open-ear headphones and get great results. Here we go.VoiceTrick isn’t meant to be heard by end listeners and doesn’t do anything useful for the sound. Instead, it exploits some quirks of microphones and the human ear to make it so you can put up a mic, blast your backing tracks, and record (mostly) just the vocals and not so much the backing tracks coming out of the speakers. It’s fiddly to do if you want really impressive results, but there are lots of benefits: most of all, allowing singers to hear themselves as if they were singing a capella, because they literally get to ditch the headphones if they like.What it does (it’s a stereo plugin, for the monitor mix) is make the mix into mono, filter it (if you like) and then flip phase on one side.What YOU do is, place your vocal mic EXACTLY between the two speakers, perfect mirror image, pointed away from the speakers and towards the vocalist. The closer you can get to a perfect mirror image the better your results will be: if the bounce off the back wall is still a perfect mirror image, that’ll cancel too. Eventually you’ll get into a room reverb off the speakers, but that’s probably OK, and if you need that room reverb to be darker, that’s what the EQ is for. It’s the lowpass from Airwindows Capacitor. You’ll have NO vocal in the monitors at all, it’s all acoustic volume from the singer. If you take some time and effort to set up the perfect cancellation (Peter Gabriel would take days to do this) you might want to use a heavy sturdy stand that really locks the mic in one position. If you’ve got that perfect, you can sing into the mic and crank it WAY up in the mix without issues, or compress it. If the mic is off-center, you’ll hear a flangey hint of the backing tracks.Another thing you can do is use open-ear headphones the same way. With those, it’s even more likely that the earpieces will move relative to the mic (which still should be exactly centered) and you’ll hear more of the flangey quality. That’s literally what’s happening: it’s like a through-zero flange at the mic position. Your ears will hear more than the mic does, because they reconstruct the sound field in stereo and the mic can only hear what’s exactly at its point of sound collection. If the earpieces move, they won’t be as loud as speakers but they’re much closer to the microphone. On the other hand, they’re closer to each other, so they’ll always cancel a lot of bass and low midrange no matter how off-axis they get. If the flangey stuff is bothersome, use the EQ to roll off the extreme highs until you can live with the results. Pretty simple.This is all the more important in the age of DAWs, because if you’re monitoring through a DAW (especially if you’re tracking through plugins or into a big dense mix) you might be dealing with a lot of latency. Latency in vocal monitoring can completely obliterate your ability to sing or even talk, and the better a singer or performer you are, the worse it will hurt you. VoiceTrick lets you go back to a capella, where there’s no latency or even headphones to interfere with your performance.This One Weird (Voice)Trick is free. It’s real simple, it’s a convenience plugin. Please don’t master through it or everyone will be sad :) if you’d like to support my ability to do this and more, please jump on my Patreon as that is why you still have me here doing this a couple years later. I’ll scale this operation up as much as I can: that depends on you, and I suppose internet virality. In the complete absence of internet virality, I’m happy to say I’m still here and able to give you the proverbial One Weird Trick for vocal tracking. Hope it makes your life better. :)VoiceTrick is in current versions of Monitoring as one of the monitoring options."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class VoiceTrick final : public Effect<T>
{
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

    enum params
    {
        kParamA = 0,
        kNumParameters = 1
    };

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
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "tone"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Tone"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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
} // namespace airwindohhs::voicetrick
