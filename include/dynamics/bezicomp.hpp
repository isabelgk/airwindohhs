#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::bezicomp {

constexpr std::string_view k_name{ "BeziComp" };
constexpr std::string_view k_short_description{
    "BeziComp is a radical compressor that eliminates aliasing!"
};
constexpr std::string_view k_long_description{
    "Funny how many of my Bezier curve experiments make plugins with powerful weird colorations… and then, this one is the opposite.Or at least, appears to be the opposite… until you dig a little deeper, and unleash the madness.So this is an experiment. The full range of the experiment is available, meaning 'good' settings might occupy small and fussy settings on the controls. It's Bezier curves as the result of literal sound density, so it's a compressor without attack or release, just a curve that goes wild based on what's under it, audio-wise. There are three controls that can either harness this, or just turn it loose to be weird and unexpected.Comp drives the Bezier curve: it acts like your threshold. Everything you do ends up interacting not so much with 'compressed level', as if you can set the threshold and it'll be smoothly compressed below it… no, BeziComp works largely with HOW you hit this threshold and interact with it, because if you crank Comp way up it will wrap around not to a flat amplitude, but silence. That's intentional, because the output is meant to work with what you might call a 'live' curve rather than flatten things into a featureless dynamic line. So there's a start: Comp is how much BeziComp reacts, but it flattens out to silence, not audio, so you keep Comp in check (unless you are specifically just isolating attacks in Wet using this behavior).Speed is basically DeRez, except the range goes way deeper into subsonic frequencies than usual. It still goes way up into the audio band if you like. The key here is, this sets the energy level of the Bezier curve based on a loudness window that's directly related to the Bezier curve itself. So in theory, it would smooth things out completely, except we often don't use compressors for that, do we? We slow attacks and speed releases to get sonic effects. Turns out that's what happens here, because as BeziComp reacts, the reaction is slowed by having to analyse the audio, and so as you slow it more you get a broader and broader attack on the sound. If Speed is high, it's real twitchy and will jump on transients quickly, but if Speed is real slow, you have a slower 'swing' that can be timed to a beat and used to accentuate the groove. So far, basically normal (ish).Dry/Wet is basically your ratio control, but extra. Since BeziComp wraps around to dynamically invert, Dry/Wet is the only way you can get continued sound if you're pushing Comp real hard. Anything over about halfway gets you into territories that act like vari-mu tones: the 'squish' abilities here are very extreme. Even when keeping Comp and Speed in check, when using BeziComp on something like mix buss, it will probably still be almost all Dry, because that's the only way you can force it to have a relatively low ratio. Expect to not use full Wet in many cases, treat it like full wet is sort of 'isolated delta of the effect', a more exaggerated version of what you want.Now, here's where things start to go off the rails a bit.BeziComp is modulating a Bezier curve, not following an attack and release. Speed does profoundly affect this, but not in the sense of setting a maximum speed for the behavior, instead it fixes a tightest corner with which the curve can TURN… and it's constantly willing to use that sharpest corner, and it will apply that corner to anything.That means if you have full silence followed by full density, BeziComp will attack harder and compress more than if you have just as hot a peak, but less audio behind it. It is NOT a limiter, or even a normal compressor, If you have loud audio and then sudden silence, it will begin swooping up in loudness not instantly, but on the same curve (and minimum curve radius) and then it's gonna put another curve radius on there as it hits silence, rather than simply 'switching' to full volume.This means BeziComp is more free from aliasing than any other compressor, period, even at high Speed settings. Nothing you do can make it suddenly hit the threshold and start to turn down. The amplitude modulation IS the Bezier curve, meaning it can only contain harmonics below what you set, meaning no matter what you do it can't produce an artifact over its own curve radius. And at low Speed settings, that radius is VERY wide.So, BeziComp is both able to make unexpected moves (since its maximum gain-change speed is not an Attack or Release, but whatever its Bezier curve allows it to do) and also hard to hear (because you can't go by artifacts, there are none). It's disgustingly transparent but also capable of being quirky and throwing odd bursts of loudness or silence in there. No matter how extreme you make it act, it hides the extremeness through using the Bezier curve on dynamic modulation… and no matter how well it hides its moves, it's still capable of unexpected quirks, because of that fact that the attack and release speeds aren't really just 'speeds' but curve radiuses.We're not used to using compressors that do that. Time we learned, because I think it'll be good :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class BeziComp final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total];
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BeziComp()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < bez_total; x++) {
            bezComp[x] = 0.0;
        }
        bezComp[bez_cycle] = 1.0;
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "comp"; break;
            case kParamB: return "speed"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Comp"; break;
            case kParamB: return "Speed"; break;
            case kParamC: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double bezCThresh = pow(A, 2.0) * 64.0;
        double bezMakeUp = sqrt(bezCThresh + 1.0);
        double bezRez = (pow(B, 6.0) + 0.0001) / overallscale;
        if (bezRez > 1.0) {
            bezRez = 1.0;
        }
        double wet = C;
        while (--sampleFrames >= 0) {
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
            bezComp[bez_cycle] += bezRez;
            bezComp[bez_SampL] += (fmax(fabs(inputSampleL), fabs(inputSampleR)) * bezRez);
            if (bezComp[bez_cycle] > 1.0) {
                bezComp[bez_cycle] -= 1.0;
                bezComp[bez_CL] = bezComp[bez_BL];
                bezComp[bez_BL] = bezComp[bez_AL];
                bezComp[bez_AL] = bezComp[bez_SampL];
                bezComp[bez_SampL] = 0.0;
            }
            double CBL = (bezComp[bez_CL] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_BL] * bezComp[bez_cycle]);
            double BAL = (bezComp[bez_BL] * (1.0 - bezComp[bez_cycle])) + (bezComp[bez_AL] * bezComp[bez_cycle]);
            double CBAL = (bezComp[bez_BL] + (CBL * (1.0 - bezComp[bez_cycle])) + (BAL * bezComp[bez_cycle])) * 0.5;
            inputSampleL *= 1.0 - (fmin(CBAL * bezCThresh, 1.0));
            inputSampleL *= bezMakeUp;
            inputSampleR *= 1.0 - (fmin(CBAL * bezCThresh, 1.0));
            inputSampleR *= bezMakeUp;
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::bezicomp
