#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tube {

constexpr std::string_view k_name{ "Tube" };
constexpr std::string_view k_short_description{
    "TubeDesk is a tube recording console type tone coloring."
};
constexpr std::string_view k_long_description{
    "Introducing… Airwindows TUBE.This is your new go-to soundbetterizer! It will make anything you put it on HUGE, and is a new algorithm that’s going to inform everything I do going forward, as well as building on everything I’ve learned to date!Just try it. Either keep it restrained, or boost it a little, or CRANK it all the way, and then TURN IT ON. No matter what your audio is, this should blow your mind :DGo try! If you want the full soundbetterizer experience go now and use it! It will never let you down! :D :D ;D:)Okay, good. Who’s still here? For those of you who’re still reading, and those who like to know how things work, let me nerd out a little and explain (a) why all that is true, and (b) exactly how it’s done. Tube is a combination of things. It’s the fruit of some work I’m doing on dialing in distortion types, based on stuff I made for Mackity etc. and it lets you dial in the same clip style I used for Mackity, but scale it up and down, make it simpler or more and more complex and linear. The maximum linearity it can do is when it’s set to zero: then it’s a soft-clip with a nice clean center region. The minimum linearity it can do is at full crank, and then it’s inputSample – inputSample*fabs(inputSample), scaled just right… and then gain-adjusted right back up again.And that’s the whole secret. It’s a distortion… a very simple distortion with the fewest possible calculations, even simpler than using sin() to distort… and at full crank it distorts a LOT and then applies makeup gain. Most of what you’re hearing is marketing volume. Not only that, since it’s a distortion, there’s a carefully calculated pre-boost in there too, and it’s set up so that at full crank, it takes the RMS loudness of a triangle wave (not unlike music content) and boosts it EXACTLY to where it’s now the RMS loudness of a sine wave. Everything else is just applying these things with the Airwindows house sound… in fact it’s a new high-water mark for the Airwindows house sound, nothing short of BitShiftGain is quite as good as far as ‘minimal processing to get the result, and insane overkill for word length and linearity’.It goes even beyond that. Unlike my normal ‘mimic the sound of hardware’ plugins, which use (more complicated) biquad filters to get exact voicings of tone, Tube is designed to be dropped in literally anywhere. So, instead of the usual approach, I’ve got a radical approach to aliasing suppression: since it’s already so soft, at 88.2k and up we just do a single averaging of adjacent samples before the clipping, and then a single averaging after the clipping. Period. That’s all. This is shall we say not as effective as brickwall filters and Ultrasonic… but it’s applying a linearizing effect, twice, that applies to all the audible frequencies and does basically nothing else. It’s not the ideal thing for handling problem aliasing cases, but for your tubey midrange lushness there is NO other possible approach that performs as well. And, again, the most aggressively minimal approach you could have. That’s key to how I get plugins to sound right.So, there you have it… and you do, I hope you enjoy Tube. I’ll be using the algorithms a lot, and using it to dial in more sophisticated plugins. And some folks will lose their minds and believe the magic… and some will get cross I didn’t set it up for proper A/Bing, which kind of defeats the purpose for the first crew… and in the final analysis, what TUBE gives you is this: some of the most extremely Airwindows tone you could possibly have, free and easy to drop into any track, mix, or mastering that you just want… bigger.Hi again! :DI took such a beating over TUUUBBEEEE!!!11!! that I’m following up immediately with another version. Not as a replacement, but just pursuing a LOT of the things people were talking about… while retaining some of the bloody-mindedness that makes people so mad at me.Folks who actually know what they’re talking about had me digging up copies of the RCA Receiving Tube Manual, to study how the electron field’s impedance can fluctuate changing the behavior of preceding stages, and how this is affected by unavoidable time delays as the electrons transit from cathode to plate through grid. (If I have that backwards, please turn your DAW upside down to compensate). There’s good reason to expect second harmonics both on the low end and as higher harmonics: all that’s in there. I did my best to find algorithms that’d sharpen corners going one direction and loosen them going the other. It’s been quite a ride. It did motivate me to code an update to Monitoring that you’ll be seeing pretty soon: adding the ‘Tube used as a safety clipper) tiny pad for appropriate output levels, and switching the new Monitoring to use Dark as its wordlength reducer. You’re hearing all that in the video too.Most of all: this is the version of Tube that’s pretty close to level matched if you have the input trim at 0.5. You can pad it more if you like. Tube2 still lets you make the audio REALLY BIG, because that’s what it’s for: I needed a safety clipper stage before I needed anything else, and it’s still designed to accurately top out at 0dB exactly, and anything you hit it with from well below to quite a bit above should all sound right.If you need more gain than you can get from cranking the input pad until it’s wide open, THAT is when you should break out Tube (1) and use it as a feeder for Tube (2). I don’t think there’s much to be gained from running Tube2 into Tube2, though I’m not your Mom and you can do as you like. I’m just saying the whole thing’s designed around finishing up in Tube2, with whatever degree of ‘Tube’ you see fit, and similar behaviors of the control: regardless of what your levels are doing, more TUBE means softer and more saturated distortion, plus all the new behaviors making stuff interesting. When you back off the TUBE control, you’re going for more linearity in every sense, and you should be able to dial in the right vibe without trouble.I have no idea whether this is gonna get me a fresh new wave of, uh, criticism :) I’m not even sure whether that would be good or whether it’s more likely to be SO good for people that they rush off and use it as intended, don’t find time to get mad, and then the whole thing dies a media death. It is said that scandal drives page views… but my own drives are rather simpler.I just want to make stuff that sounds good, along the lines I’ve been developing over more than ten years working on getting digital audio to not sound like DAW hell.This is an upgrade. Hope you like it :)More classic Airwindows analog modeling with TubeDesk!Tubes aren’t necessarily ‘mellow’. They’re also known for clarity, realism. TubeDesk isn’t a mud-ifier, but it might bring you some effects reminiscent of vintage recordings.Like the other Desk plugins, it’s got a kind of saturation going on, a distinct flavor to how it distorts. However, unlike TransDesk, its power supply is very different. TubeDesk is so old school it acts like there’s a vintage tube rectifier, imparting an obvious rectifier sag. This conditions the sound, affecting how dynamics work through the plugin.You have a distinct ‘analog modeling’ tone then, which is no specific console, no arbitrary color: just sort of retro vibe, generalized. It’s not calibrated to work with Console (that’s the for-pay version of Desk that consolidated these and calibrated them all) and it doesn’t have special requirements for where it should go. Place it where you want that style of tone coloring: like Desk, you can use it on auxes and submixes (including ones ‘inside’ Console) to better emulate running through a lot of circuitry."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Tube final : public Effect<T>
{
    double previousSampleA;
    double previousSampleB;
    double previousSampleC;
    double previousSampleD;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Tube()
    {
        A = 0.0;
        previousSampleA = 0.0;
        previousSampleB = 0.0;
        previousSampleC = 0.0;
        previousSampleD = 0.0;
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
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "tube";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "TUBE";

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double gain = 1.0 + (A * 0.2246161992650486);
        // this maxes out at +1.76dB, which is the exact difference between what a triangle/saw wave
        // would be, and a sine (the fullest possible wave at the same peak amplitude). Why do this?
        // Because the nature of this plugin is the 'more FAT TUUUUBE fatness!' knob, and because
        // sticking to this amount maximizes that effect on a 'normal' sound that is itself unclipped
        // while confining the resulting 'clipped' area to what is already 'fattened' into a flat
        // and distorted region. You can always put a gain trim in front of it for more distortion,
        // or cascade them in the DAW for more distortion.
        double iterations = 1.0 - A;
        int powerfactor = (5.0 * iterations) + 1;
        double gainscaling = 1.0 / (double)(powerfactor + 1);
        double outputscaling = 1.0 + (1.0 / (double)(powerfactor));
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
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleA;
                previousSampleA = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleB;
                previousSampleB = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
            inputSampleL *= gain;
            inputSampleR *= gain;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            double factor = inputSampleL; // Left channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleL;
            }
            // this applies more and more of a 'curve' to the transfer function
            if ((powerfactor % 2 == 1) && (inputSampleL != 0.0)) {
                factor = (factor / inputSampleL) * fabs(inputSampleL);
            }
            // if we would've got an asymmetrical effect this undoes the last step, and then
            // redoes it using an absolute value to make the effect symmetrical again
            factor *= gainscaling;
            inputSampleL -= factor;
            inputSampleL *= outputscaling;
            factor = inputSampleR; // Right channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleR;
            }
            // this applies more and more of a 'curve' to the transfer function
            if ((powerfactor % 2 == 1) && (inputSampleR != 0.0)) {
                factor = (factor / inputSampleR) * fabs(inputSampleR);
            }
            // if we would've got an asymmetrical effect this undoes the last step, and then
            // redoes it using an absolute value to make the effect symmetrical again
            factor *= gainscaling;
            inputSampleR -= factor;
            inputSampleR *= outputscaling;
            /*  This is the simplest raw form of the 'fattest' TUBE boost between -1.0 and 1.0
             if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
             inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*2.0;
             */
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleC;
                previousSampleC = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleD;
                previousSampleD = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
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
} // namespace airwindohhs::tube
