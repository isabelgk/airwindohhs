#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::adclip7 {

constexpr std::string_view k_name{ "ADClip7" };
constexpr std::string_view k_short_description{
    "ADClip7 is the ultimate Airwindows loudness maximizer/ peak control."
};
constexpr std::string_view k_long_description{
    "Finally! The newest version of Airwindows’ famous ADClip is out, and it’s Mac and PC VST for the first time, and it’s free! If you have dense, busy mixes and you want to push their loudness to the max, this is the one you want. And, oddly, if you’re mastering for streaming services or iTunes and don’t want to do a thing besides convey your mix at a set LUFS and intersample peak level, this is still the one you want :)Here’s how it works.You’ve got a boost control, a soften control, an enhance control, and a popup or multi-function control with three options: normal, gain matched, and Clip Only. These are all interactive, so I’ll explain them in the context of that ‘mode selector’ popup.In normal mode, boost makes things louder. This is a clipper, and no more polite method can ever make things louder than a clipper: you need to either give it a busy and complicated mix to mask the clip artifacts, or use it to snip off non-tonal percussive peaks, at which it excels. A clipper does NOT produce ‘limiter-like unvarying block of sound’, and you shouldn’t try to achieve that. A clipper gives you punchy but LOUD, and tries to retain all the dynamics you’re feeding it, rather than smooth them out for a ‘clean’ sound.The soften control manages the way that high frequencies enter and exit the clips. It algorithmically reshapes the edges of your clip, stopping it from getting digital glare and fizz. This is the heart of ADClip (also present in my simpler clippers, not counting One Corner Clip, which is still upcoming).The soften control also balances the outputs of two separate energy-fill algorithms, one for bass and one for highs. This was the response to a certain other loudness maximizer that launched proclaiming clippers were dead, and which is still promising its version 2 (and some bugfixes) while ADClip has gone far beyond it. Turned out the secret of that one was an elaborate way of massaging clipped-off loudness back into the signal, in a way that was supposed to be transparent but ‘cracked’ into artifacts when pushed too hard. The algorithms were presumably very sophisticated, which tends to just make the breaking point more obvious when you hit it.The Airwindows version is a completely different, cruder and more direct version of doing the same thing, so when you slam the heck out of ADClip7, you get a deep bass slam that ‘overhangs’ a bit to add weight, and still fits into the clipped output. And you get the softened, analog-style clips to add ‘heat’ and overtones, but you also get a high-mids reinforcement that normally just highlights bright transients that would otherwise be lost to the clipping. And the Enhance control lets you go between purely ‘analog clip’ energy, and these added reinforcements.The gain-matched mode has two uses. You could use it to ‘set your slam level’ in a way guaranteed not to trick you into thinking louder is better: it turns the output down, so instead of hearing everything get bigger and louder, you just listen for the point that the clipping’s adversely affecting the sound. Then you can flip back to normal mode, if that’s what you wanted (maximum loudness without blatant grunge). Or, you can ignore the slam and use gain-matched mode as I demonstrate in the video: ADClip7 already suppresses intersample peaks when they’re part of clipping, already reinforces energy lost to clipping, so you can use it in conjunction with a tool like Youlean’s loudness meter to dial in a specific intersample peak level for iTunes or other such picky streaming services. It’ll work like padding the output. and if you’re already in the ballpark LUFS-wise, ADClip is a far more sophisticated tool than just limiting and then padding the output to get to your ‘true peak’ target.The last mode is Clip Only, and rather than selecting the various algorithm outputs individually, this version of ADClip gives you them at their respective loudnesses, combined. That means you can engage this mode to hear ONLY the clips, and check that you’re not hearing any recognizable ‘scrunch’ of continuing clippage. But since you’re also hearing the enhance outputs, you can adjust softness and enhance level to balance the stuff being introduced to the sound. My recommendation is to set the controls so no one type of artifact predominates: it’s not necessarily great to throw in a bunch of ‘enhance’ bass just because you can. If you’re hearing that much of it that you’re tempted to use it as an effect, you’re definitely also over-slamming your music.So my recommendation in 2017 heading to 2018, is to use ADClip7 in gain-matched mode, to keep that ‘true peak’ measurement within the Mastered For iTunes requirements. I’m sure not everybody will stick to that, but I’m happy to say it is actually quite good for doing that, and if people want to smash stuff with it and enjoy the bass thud, that’s their affair. Remember a clipper makes stuff dynamic and punchy, not ‘flattened out’: use a limiter if you need dynamically flat, or perhaps both. In this modern era of replay gain, I’m going to suggest that dynamic and punchy is where it’s at. Learn the lessons of the LUFS meter! They’re available to us all, now."
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ADClip7 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL;
    double lastSampleR;
    float bL[22200];
    float bR[22200];
    int gcount;
    double lowsL;
    double lowsR;
    double iirLowsAL;
    double iirLowsAR;
    double iirLowsBL;
    double iirLowsBR;
    double refclipL;
    double refclipR;
    float A;
    float B;
    float C;
    float D;

  public:
    ADClip7()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        for (int count = 0; count < 22199; count++) {
            bL[count] = 0;
            bR[count] = 0;
        }
        gcount = 0;
        lowsL = 0;
        lowsR = 0;
        refclipL = 0.99;
        refclipR = 0.99;
        iirLowsAL = 0.0;
        iirLowsAR = 0.0;
        iirLowsBL = 0.0;
        iirLowsBR = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "boost"; break;
            case kParamB: return "soften"; break;
            case kParamC: return "enhance"; break;
            case kParamD: return "mode"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Boost"; break;
            case kParamB: return "Soften"; break;
            case kParamC: return "Enhance"; break;
            case kParamD: return "Mode"; break;

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
            case kParamA: return "dB"; break;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        double inputGain = pow(10.0, (A * 18.0) / 20.0);
        double softness = B * fpNew;
        double hardness = 1.0 - softness;
        double highslift = 0.307 * C;
        double adjust = pow(highslift, 3) * 0.416;
        double subslift = 0.796 * C;
        double calibsubs = subslift / 53;
        double invcalibsubs = 1.0 - calibsubs;
        double subs = 0.81 + (calibsubs * 2);
        double bridgerectifier;
        int mode = (int)floor(D * 2.999) + 1;
        double overshootL;
        double overshootR;
        double offsetH1 = 1.84;
        offsetH1 *= overallscale;
        double offsetH2 = offsetH1 * 1.9;
        double offsetH3 = offsetH1 * 2.7;
        double offsetL1 = 612;
        offsetL1 *= overallscale;
        double offsetL2 = offsetL1 * 2.0;
        int refH1 = (int)floor(offsetH1);
        int refH2 = (int)floor(offsetH2);
        int refH3 = (int)floor(offsetH3);
        int refL1 = (int)floor(offsetL1);
        int refL2 = (int)floor(offsetL2);
        int temp;
        double fractionH1 = offsetH1 - floor(offsetH1);
        double fractionH2 = offsetH2 - floor(offsetH2);
        double fractionH3 = offsetH3 - floor(offsetH3);
        double minusH1 = 1.0 - fractionH1;
        double minusH2 = 1.0 - fractionH2;
        double minusH3 = 1.0 - fractionH3;
        double highsL = 0.0;
        double highsR = 0.0;
        int count = 0;
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
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            overshootL = fabs(inputSampleL) - refclipL;
            overshootR = fabs(inputSampleR) - refclipR;
            if (overshootL < 0.0) {
                overshootL = 0.0;
            }
            if (overshootR < 0.0) {
                overshootR = 0.0;
            }
            if (gcount < 0 || gcount > 11020) {
                gcount = 11020;
            }
            count = gcount;
            bL[count + 11020] = bL[count] = overshootL;
            bR[count + 11020] = bR[count] = overshootR;
            gcount--;
            if (highslift > 0.0)
            {
                // we have a big pile of b[] which is overshoots
                temp = count + refH3;
                highsL = -(bL[temp] * minusH3); // less as value moves away from .0
                highsL -= bL[temp + 1]; // we can assume always using this in one way or another?
                highsL -= (bL[temp + 2] * fractionH3); // greater as value moves away from .0
                highsL += (((bL[temp] - bL[temp + 1]) - (bL[temp + 1] - bL[temp + 2])) / 50); // interpolation hacks 'r us
                highsL *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 3 is a negative add
                highsR = -(bR[temp] * minusH3); // less as value moves away from .0
                highsR -= bR[temp + 1]; // we can assume always using this in one way or another?
                highsR -= (bR[temp + 2] * fractionH3); // greater as value moves away from .0
                highsR += (((bR[temp] - bR[temp + 1]) - (bR[temp + 1] - bR[temp + 2])) / 50); // interpolation hacks 'r us
                highsR *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 3 is a negative add
                temp = count + refH2;
                highsL += (bL[temp] * minusH2); // less as value moves away from .0
                highsL += bL[temp + 1]; // we can assume always using this in one way or another?
                highsL += (bL[temp + 2] * fractionH2); // greater as value moves away from .0
                highsL -= (((bL[temp] - bL[temp + 1]) - (bL[temp + 1] - bL[temp + 2])) / 50); // interpolation hacks 'r us
                highsL *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 2 is a positive feedback of the overshoot
                highsR += (bR[temp] * minusH2); // less as value moves away from .0
                highsR += bR[temp + 1]; // we can assume always using this in one way or another?
                highsR += (bR[temp + 2] * fractionH2); // greater as value moves away from .0
                highsR -= (((bR[temp] - bR[temp + 1]) - (bR[temp + 1] - bR[temp + 2])) / 50); // interpolation hacks 'r us
                highsR *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 2 is a positive feedback of the overshoot
                temp = count + refH1;
                highsL -= (bL[temp] * minusH1); // less as value moves away from .0
                highsL -= bL[temp + 1]; // we can assume always using this in one way or another?
                highsL -= (bL[temp + 2] * fractionH1); // greater as value moves away from .0
                highsL += (((bL[temp] - bL[temp + 1]) - (bL[temp + 1] - bL[temp + 2])) / 50); // interpolation hacks 'r us
                highsL *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 1 is a negative feedback of the overshoot
                highsR -= (bR[temp] * minusH1); // less as value moves away from .0
                highsR -= bR[temp + 1]; // we can assume always using this in one way or another?
                highsR -= (bR[temp + 2] * fractionH1); // greater as value moves away from .0
                highsR += (((bR[temp] - bR[temp + 1]) - (bR[temp + 1] - bR[temp + 2])) / 50); // interpolation hacks 'r us
                highsR *= adjust; // add in the kernel elements backwards saves multiplies
                // stage 1 is a negative feedback of the overshoot
                // done with interpolated mostly negative feedback of the overshoot
            }
            bridgerectifier = sin(fabs(highsL) * hardness);
            // this will wrap around and is scaled back by softness
            // wrap around is the same principle as Fracture: no top limit to sin()
            if (highsL > 0) {
                highsL = bridgerectifier;
            }
            else {
                highsL = -bridgerectifier;
            }
            bridgerectifier = sin(fabs(highsR) * hardness);
            // this will wrap around and is scaled back by softness
            // wrap around is the same principle as Fracture: no top limit to sin()
            if (highsR > 0) {
                highsR = bridgerectifier;
            }
            else {
                highsR = -bridgerectifier;
            }
            if (subslift > 0.0)
            {
                lowsL *= subs;
                lowsR *= subs;
                // going in we'll reel back some of the swing
                temp = count + refL1;
                lowsL -= bL[temp + 127];
                lowsL -= bL[temp + 113];
                lowsL -= bL[temp + 109];
                lowsL -= bL[temp + 107];
                lowsL -= bL[temp + 103];
                lowsL -= bL[temp + 101];
                lowsL -= bL[temp + 97];
                lowsL -= bL[temp + 89];
                lowsL -= bL[temp + 83];
                lowsL -= bL[temp + 79];
                lowsL -= bL[temp + 73];
                lowsL -= bL[temp + 71];
                lowsL -= bL[temp + 67];
                lowsL -= bL[temp + 61];
                lowsL -= bL[temp + 59];
                lowsL -= bL[temp + 53];
                lowsL -= bL[temp + 47];
                lowsL -= bL[temp + 43];
                lowsL -= bL[temp + 41];
                lowsL -= bL[temp + 37];
                lowsL -= bL[temp + 31];
                lowsL -= bL[temp + 29];
                lowsL -= bL[temp + 23];
                lowsL -= bL[temp + 19];
                lowsL -= bL[temp + 17];
                lowsL -= bL[temp + 13];
                lowsL -= bL[temp + 11];
                lowsL -= bL[temp + 7];
                lowsL -= bL[temp + 5];
                lowsL -= bL[temp + 3];
                lowsL -= bL[temp + 2];
                lowsL -= bL[temp + 1];
                // initial negative lobe
                lowsR -= bR[temp + 127];
                lowsR -= bR[temp + 113];
                lowsR -= bR[temp + 109];
                lowsR -= bR[temp + 107];
                lowsR -= bR[temp + 103];
                lowsR -= bR[temp + 101];
                lowsR -= bR[temp + 97];
                lowsR -= bR[temp + 89];
                lowsR -= bR[temp + 83];
                lowsR -= bR[temp + 79];
                lowsR -= bR[temp + 73];
                lowsR -= bR[temp + 71];
                lowsR -= bR[temp + 67];
                lowsR -= bR[temp + 61];
                lowsR -= bR[temp + 59];
                lowsR -= bR[temp + 53];
                lowsR -= bR[temp + 47];
                lowsR -= bR[temp + 43];
                lowsR -= bR[temp + 41];
                lowsR -= bR[temp + 37];
                lowsR -= bR[temp + 31];
                lowsR -= bR[temp + 29];
                lowsR -= bR[temp + 23];
                lowsR -= bR[temp + 19];
                lowsR -= bR[temp + 17];
                lowsR -= bR[temp + 13];
                lowsR -= bR[temp + 11];
                lowsR -= bR[temp + 7];
                lowsR -= bR[temp + 5];
                lowsR -= bR[temp + 3];
                lowsR -= bR[temp + 2];
                lowsR -= bR[temp + 1];
                // initial negative lobe
                lowsL *= subs;
                lowsL *= subs;
                lowsR *= subs;
                lowsR *= subs;
                // twice, to minimize the suckout in low boost situations
                temp = count + refL2;
                lowsL += bL[temp + 127];
                lowsL += bL[temp + 113];
                lowsL += bL[temp + 109];
                lowsL += bL[temp + 107];
                lowsL += bL[temp + 103];
                lowsL += bL[temp + 101];
                lowsL += bL[temp + 97];
                lowsL += bL[temp + 89];
                lowsL += bL[temp + 83];
                lowsL += bL[temp + 79];
                lowsL += bL[temp + 73];
                lowsL += bL[temp + 71];
                lowsL += bL[temp + 67];
                lowsL += bL[temp + 61];
                lowsL += bL[temp + 59];
                lowsL += bL[temp + 53];
                lowsL += bL[temp + 47];
                lowsL += bL[temp + 43];
                lowsL += bL[temp + 41];
                lowsL += bL[temp + 37];
                lowsL += bL[temp + 31];
                lowsL += bL[temp + 29];
                lowsL += bL[temp + 23];
                lowsL += bL[temp + 19];
                lowsL += bL[temp + 17];
                lowsL += bL[temp + 13];
                lowsL += bL[temp + 11];
                lowsL += bL[temp + 7];
                lowsL += bL[temp + 5];
                lowsL += bL[temp + 3];
                lowsL += bL[temp + 2];
                lowsL += bL[temp + 1];
                // followup positive lobe
                lowsR += bR[temp + 127];
                lowsR += bR[temp + 113];
                lowsR += bR[temp + 109];
                lowsR += bR[temp + 107];
                lowsR += bR[temp + 103];
                lowsR += bR[temp + 101];
                lowsR += bR[temp + 97];
                lowsR += bR[temp + 89];
                lowsR += bR[temp + 83];
                lowsR += bR[temp + 79];
                lowsR += bR[temp + 73];
                lowsR += bR[temp + 71];
                lowsR += bR[temp + 67];
                lowsR += bR[temp + 61];
                lowsR += bR[temp + 59];
                lowsR += bR[temp + 53];
                lowsR += bR[temp + 47];
                lowsR += bR[temp + 43];
                lowsR += bR[temp + 41];
                lowsR += bR[temp + 37];
                lowsR += bR[temp + 31];
                lowsR += bR[temp + 29];
                lowsR += bR[temp + 23];
                lowsR += bR[temp + 19];
                lowsR += bR[temp + 17];
                lowsR += bR[temp + 13];
                lowsR += bR[temp + 11];
                lowsR += bR[temp + 7];
                lowsR += bR[temp + 5];
                lowsR += bR[temp + 3];
                lowsR += bR[temp + 2];
                lowsR += bR[temp + 1];
                // followup positive lobe
                lowsL *= subs;
                lowsR *= subs;
                // now we have the lows content to use
            }
            bridgerectifier = sin(fabs(lowsL) * softness);
            // this will wrap around and is scaled back by hardness: hard = less bass push, more treble
            // wrap around is the same principle as Fracture: no top limit to sin()
            if (lowsL > 0) {
                lowsL = bridgerectifier;
            }
            else {
                lowsL = -bridgerectifier;
            }
            bridgerectifier = sin(fabs(lowsR) * softness);
            // this will wrap around and is scaled back by hardness: hard = less bass push, more treble
            // wrap around is the same principle as Fracture: no top limit to sin()
            if (lowsR > 0) {
                lowsR = bridgerectifier;
            }
            else {
                lowsR = -bridgerectifier;
            }
            iirLowsAL = (iirLowsAL * invcalibsubs) + (lowsL * calibsubs);
            lowsL = iirLowsAL;
            bridgerectifier = sin(fabs(lowsL));
            if (lowsL > 0) {
                lowsL = bridgerectifier;
            }
            else {
                lowsL = -bridgerectifier;
            }
            iirLowsAR = (iirLowsAR * invcalibsubs) + (lowsR * calibsubs);
            lowsR = iirLowsAR;
            bridgerectifier = sin(fabs(lowsR));
            if (lowsR > 0) {
                lowsR = bridgerectifier;
            }
            else {
                lowsR = -bridgerectifier;
            }
            iirLowsBL = (iirLowsBL * invcalibsubs) + (lowsL * calibsubs);
            lowsL = iirLowsBL;
            bridgerectifier = sin(fabs(lowsL)) * 2.0;
            if (lowsL > 0) {
                lowsL = bridgerectifier;
            }
            else {
                lowsL = -bridgerectifier;
            }
            iirLowsBR = (iirLowsBR * invcalibsubs) + (lowsR * calibsubs);
            lowsR = iirLowsBR;
            bridgerectifier = sin(fabs(lowsR)) * 2.0;
            if (lowsR > 0) {
                lowsR = bridgerectifier;
            }
            else {
                lowsR = -bridgerectifier;
            }
            if (highslift > 0.0) {
                inputSampleL += (highsL * (1.0 - fabs(inputSampleL * hardness)));
            }
            if (subslift > 0.0) {
                inputSampleL += (lowsL * (1.0 - fabs(inputSampleL * softness)));
            }
            if (highslift > 0.0) {
                inputSampleR += (highsR * (1.0 - fabs(inputSampleR * hardness)));
            }
            if (subslift > 0.0) {
                inputSampleR += (lowsR * (1.0 - fabs(inputSampleR * softness)));
            }
            if (inputSampleL > refclipL && refclipL > 0.9) {
                refclipL -= 0.01;
            }
            if (inputSampleL < -refclipL && refclipL > 0.9) {
                refclipL -= 0.01;
            }
            if (refclipL < 0.99) {
                refclipL += 0.00001;
            }
            // adjust clip level on the fly
            if (inputSampleR > refclipR && refclipR > 0.9) {
                refclipR -= 0.01;
            }
            if (inputSampleR < -refclipR && refclipR > 0.9) {
                refclipR -= 0.01;
            }
            if (refclipR < 0.99) {
                refclipR += 0.00001;
            }
            // adjust clip level on the fly
            if (lastSampleL >= refclipL)
            {
                if (inputSampleL < refclipL) {
                    lastSampleL = ((refclipL * hardness) + (inputSampleL * softness));
                }
                else {
                    lastSampleL = refclipL;
                }
            }
            if (lastSampleR >= refclipR)
            {
                if (inputSampleR < refclipR) {
                    lastSampleR = ((refclipR * hardness) + (inputSampleR * softness));
                }
                else {
                    lastSampleR = refclipR;
                }
            }
            if (lastSampleL <= -refclipL)
            {
                if (inputSampleL > -refclipL) {
                    lastSampleL = ((-refclipL * hardness) + (inputSampleL * softness));
                }
                else {
                    lastSampleL = -refclipL;
                }
            }
            if (lastSampleR <= -refclipR)
            {
                if (inputSampleR > -refclipR) {
                    lastSampleR = ((-refclipR * hardness) + (inputSampleR * softness));
                }
                else {
                    lastSampleR = -refclipR;
                }
            }
            if (inputSampleL > refclipL)
            {
                if (lastSampleL < refclipL) {
                    inputSampleL = ((refclipL * hardness) + (lastSampleL * softness));
                }
                else {
                    inputSampleL = refclipL;
                }
            }
            if (inputSampleR > refclipR)
            {
                if (lastSampleR < refclipR) {
                    inputSampleR = ((refclipR * hardness) + (lastSampleR * softness));
                }
                else {
                    inputSampleR = refclipR;
                }
            }
            if (inputSampleL < -refclipL)
            {
                if (lastSampleL > -refclipL) {
                    inputSampleL = ((-refclipL * hardness) + (lastSampleL * softness));
                }
                else {
                    inputSampleL = -refclipL;
                }
            }
            if (inputSampleR < -refclipR)
            {
                if (lastSampleR > -refclipR) {
                    inputSampleR = ((-refclipR * hardness) + (lastSampleR * softness));
                }
                else {
                    inputSampleR = -refclipR;
                }
            }
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            switch (mode)
            {
                case 1: break; // Normal
                case 2:
                    inputSampleL /= inputGain;
                    inputSampleR /= inputGain;
                    break; // Gain Match
                case 3:
                    inputSampleL = overshootL + highsL + lowsL;
                    inputSampleR = overshootR + highsR + lowsR;
                    break; // Clip Only
            }
            // this is our output mode switch, showing the effects
            if (inputSampleL > refclipL) {
                inputSampleL = refclipL;
            }
            if (inputSampleL < -refclipL) {
                inputSampleL = -refclipL;
            }
            if (inputSampleR > refclipR) {
                inputSampleR = refclipR;
            }
            if (inputSampleR < -refclipR) {
                inputSampleR = -refclipR;
            }
            // final iron bar
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
} // namespace airwindohhs::adclip7
