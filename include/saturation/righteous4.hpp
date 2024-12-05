#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::righteous4 {

constexpr std::string_view k_name{ "Righteous4" };
constexpr std::string_view k_short_description{
    "Righteous4 is a final output stage for targeting dynamic range."
};
constexpr std::string_view k_long_description{
    "This one’s pretty eagerly awaited by some… all the more since I’ve incorporated all the latest tech, such as the changes in denormalization and noise shaping to the floating point buss (it uses the ‘more warmth, ease and depth’ version when set to 32 bit) and even the Spiral algorithm in its overdrive section.But you might not end up using that noise shaping, because Righteous4 has NotJustAnotherDither (and the CD version) built right into it, along with an optimized ADClip and code from an old strange experiment I did called ShortBuss. Yep! Righteous4 is your all-purpose final output stage, which handles final clipping, saturating and even the dither to let you output 24 or 16 bit fixed point in the purest, most optimized way. It doesn’t even return to the DAW buss between these stages: everything is ‘long double’, so you put in your mix audio (from conservative levels to peaks that would clip a normal output file: it’ll soak up the peaks) and you get out literally the data you burn to the CD or upload to Bandcamp etc. as hi-res HD audio.But it’s a little more complicated than that. Bear with me. Righteous4 will clog up and audibly distort under some circumstances, and it’s for a reason that might interest you.Many music services these days (possibly all?) incorporate RMS loudness targets. They’ll turn up your stuff and limit it (maybe) which is awful but outside your control… but if you’re doing the loudness wars thing, they will turn you DOWN. And all your efforts to make loud masters will be wasted. Apple will cut stuff back to around -17 LUFS. YouTube goes for -13 LUFS. Spotify and Tidal do -14 LUFS. That’s loudness units short of full scale (similar to how many dB down from clipping your average (root-mean-square, or RMS) loudness is). Loudness war folks often push to -8 LUFS or even more, and that means YouTube will turn you down (and Apple will turn you WAY DOWN) to fit in with their playlists.Here’s the thing: you can have all the peak energy you want (caveat: Apple doesn’t want your bright treble peaking over -0.4 LUFS in any case, so don’t go too bonkers with brightness when aiming for Apple music services). Peak is not RMS. In fact, music sounds great with lots of peak energy pervading it: this is one of the reasons old vinyl records sounded so great! I’ve measured RMS loudness behavior like -27 LUFS off old hit records. The energy pouring out of those old grooves is due to the way peaks and compression were handled: it was a different kind of limiting, handled differently, back then. Peak energy makes the music sound better. Righteous4 handles peak energy by clipping it with ADClip, so you can throw more at it in safety. This is not the audible clipping I’m talking about (at least, not if you’re talking percussive peaks etc. which are pretty cleanly clipped, especially by ADClip)The audible clipping happens when you push your LUFS levels beyond the target. As you mix and set levels, you simply have to listen for whether Righteous4 sounds big and open, or whether it’s getting a little intense, or whether it’s obviously being pushed too far and breaking up (especially on bass, and mix fullness).What’s happening is, the saturation from the Spiral algorithm is being fed into ShortBuss, which fills the energy back into the mix in the form of second harmonic. This is what gives Righteous4 its tone and extra warmth, and it’s a nice fullness and bloom UNLESS you’re slamming it (it’s calibrated using the slider, where you select your target). If you are too loud, you’ll hear it. Your peak energy will still be going up to the real clipping threshold, just as before, but your RMS energy will tell you how loud you should go.It’s that simple. You don’t have to use it to hit LUFS targets, but since it’s got Spiral in there it also broadens the heck out of the ‘target range’ so you can pick whatever seems right to you, and it’ll guide you. If you find you need to push louder, you can set Righteous4 for a higher target, and that will clean it right up and let you push harder into the internal ADClip. If you’re looking for big dynamic range, you can set Righteous4’s target lower, or simply set it for Apple or YouTube loudnesses and then just don’t push into it very hard: this will give you dynamics, but the replay-gain services will be kinder to the result because it’s somewhat saturated and compressed, just in a very gentle way if you’re hitting it that gently. And of course the output’s optimized for whatever bit depth you’ve selected, so you can directly target CD or 24/96 or ’32 bit file to send to further mastering’ if you still need to mix into something more organic than a hard, brittle digital mix buss with all the charm of math :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Righteous4 final : public Effect<T>
{
    double leftSampleA;
    double leftSampleB;
    double leftSampleC;
    double leftSampleD;
    double leftSampleE;
    double leftSampleF;
    double leftSampleG;
    double leftSampleH;
    double leftSampleI;
    double leftSampleJ;
    double leftSampleK;
    double leftSampleL;
    double leftSampleM;
    double leftSampleN;
    double leftSampleO;
    double leftSampleP;
    double leftSampleQ;
    double leftSampleR;
    double leftSampleS;
    double leftSampleT;
    double leftSampleU;
    double leftSampleV;
    double leftSampleW;
    double leftSampleX;
    double leftSampleY;
    double leftSampleZ;
    double rightSampleA;
    double rightSampleB;
    double rightSampleC;
    double rightSampleD;
    double rightSampleE;
    double rightSampleF;
    double rightSampleG;
    double rightSampleH;
    double rightSampleI;
    double rightSampleJ;
    double rightSampleK;
    double rightSampleL;
    double rightSampleM;
    double rightSampleN;
    double rightSampleO;
    double rightSampleP;
    double rightSampleQ;
    double rightSampleR;
    double rightSampleS;
    double rightSampleT;
    double rightSampleU;
    double rightSampleV;
    double rightSampleW;
    double rightSampleX;
    double rightSampleY;
    double rightSampleZ;
    double bynL[13];
    double noiseShapingL;
    double lastSampleL;
    double IIRsampleL;
    double gwPrevL;
    double gwAL;
    double gwBL;
    double bynR[13];
    double noiseShapingR;
    double lastSampleR;
    double IIRsampleR;
    double gwPrevR;
    double gwAR;
    double gwBR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Righteous4()
    {
        A = 0.0;
        B = 0.0;
        leftSampleA = 0.0;
        leftSampleB = 0.0;
        leftSampleC = 0.0;
        leftSampleD = 0.0;
        leftSampleE = 0.0;
        leftSampleF = 0.0;
        leftSampleG = 0.0;
        leftSampleH = 0.0;
        leftSampleI = 0.0;
        leftSampleJ = 0.0;
        leftSampleK = 0.0;
        leftSampleL = 0.0;
        leftSampleM = 0.0;
        leftSampleN = 0.0;
        leftSampleO = 0.0;
        leftSampleP = 0.0;
        leftSampleQ = 0.0;
        leftSampleR = 0.0;
        leftSampleS = 0.0;
        leftSampleT = 0.0;
        leftSampleU = 0.0;
        leftSampleV = 0.0;
        leftSampleW = 0.0;
        leftSampleX = 0.0;
        leftSampleY = 0.0;
        leftSampleZ = 0.0;
        rightSampleA = 0.0;
        rightSampleB = 0.0;
        rightSampleC = 0.0;
        rightSampleD = 0.0;
        rightSampleE = 0.0;
        rightSampleF = 0.0;
        rightSampleG = 0.0;
        rightSampleH = 0.0;
        rightSampleI = 0.0;
        rightSampleJ = 0.0;
        rightSampleK = 0.0;
        rightSampleL = 0.0;
        rightSampleM = 0.0;
        rightSampleN = 0.0;
        rightSampleO = 0.0;
        rightSampleP = 0.0;
        rightSampleQ = 0.0;
        rightSampleR = 0.0;
        rightSampleS = 0.0;
        rightSampleT = 0.0;
        rightSampleU = 0.0;
        rightSampleV = 0.0;
        rightSampleW = 0.0;
        rightSampleX = 0.0;
        rightSampleY = 0.0;
        rightSampleZ = 0.0;
        bynL[0] = 1000;
        bynL[1] = 301;
        bynL[2] = 176;
        bynL[3] = 125;
        bynL[4] = 97;
        bynL[5] = 79;
        bynL[6] = 67;
        bynL[7] = 58;
        bynL[8] = 51;
        bynL[9] = 46;
        bynL[10] = 1000;
        noiseShapingL = 0.0;
        lastSampleL = 0.0;
        IIRsampleL = 0.0;
        gwPrevL = 0.0;
        gwAL = 0.0;
        gwBL = 0.0;
        bynR[0] = 1000;
        bynR[1] = 301;
        bynR[2] = 176;
        bynR[3] = 125;
        bynR[4] = 97;
        bynR[5] = 79;
        bynR[6] = 67;
        bynR[7] = 58;
        bynR[8] = 51;
        bynR[9] = 46;
        bynR[10] = 1000;
        noiseShapingR = 0.0;
        lastSampleR = 0.0;
        IIRsampleR = 0.0;
        gwPrevR = 0.0;
        gwAR = 0.0;
        gwBR = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "ltarget"; break;
            case kParamB: return "btdepth"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "LTarget"; break;
            case kParamB: return "BtDepth"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return "bit"; break;

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
        double IIRscaleback = 0.0002597; // scaleback of harmonic avg
        IIRscaleback /= overallscale;
        IIRscaleback = 1.0 - IIRscaleback;
        double target = (A * 24.0) - 28.0;
        target += 17; // gives us scaled distortion factor based on test conditions
        target = pow(10.0, target / 20.0); // we will multiply and divide by this
        // ShortBuss section
        if (target == 0) {
            target = 1; // insanity check
        }
        int bitDepth = (VstInt32)(B * 2.999) + 1; // +1 for Reaper bug workaround
        double fusswithscale = 149940.0; // corrected
        double cutofffreq = 20; // was 46/2.0
        double midAmount = (cutofffreq) / fusswithscale;
        midAmount /= overallscale;
        double midaltAmount = 1.0 - midAmount;
        double gwAfactor = 0.718;
        gwAfactor -= (overallscale * 0.05); // 0.2 at 176K, 0.1 at 88.2K, 0.05 at 44.1K
        // reduce slightly to not less than 0.5 to increase effect
        double gwBfactor = 1.0 - gwAfactor;
        double softness = 0.2135;
        double hardness = 1.0 - softness;
        double refclip = pow(10.0, -0.0058888);
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
            // begin the whole distortion dealiebop
            inputSampleL /= target;
            inputSampleR /= target;
            // running shortbuss on direct sample
            IIRsampleL *= IIRscaleback;
            double secondharmonicL = sin((2.0 * inputSampleL * inputSampleL) * IIRsampleL);
            IIRsampleR *= IIRscaleback;
            double secondharmonicR = sin((2.0 * inputSampleR * inputSampleR) * IIRsampleR);
            // secondharmonic is calculated before IIRsample is updated, to delay reaction
            double bridgerectifier = inputSampleL;
            if (bridgerectifier > 1.2533141373155) {
                bridgerectifier = 1.2533141373155;
            }
            if (bridgerectifier < -1.2533141373155) {
                bridgerectifier = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((bridgerectifier == 0.0) ? 1 : fabs(bridgerectifier));
            if (inputSampleL > bridgerectifier) {
                IIRsampleL += ((inputSampleL - bridgerectifier) * 0.0009);
            }
            if (inputSampleL < -bridgerectifier) {
                IIRsampleL += ((inputSampleL + bridgerectifier) * 0.0009);
            }
            // manipulate IIRSampleL
            inputSampleL = bridgerectifier;
            // apply the distortion transform for reals. Has been converted back to -1/1
            bridgerectifier = inputSampleR;
            if (bridgerectifier > 1.2533141373155) {
                bridgerectifier = 1.2533141373155;
            }
            if (bridgerectifier < -1.2533141373155) {
                bridgerectifier = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            bridgerectifier = sin(bridgerectifier * fabs(bridgerectifier)) / ((bridgerectifier == 0.0) ? 1 : fabs(bridgerectifier));
            if (inputSampleR > bridgerectifier) {
                IIRsampleR += ((inputSampleR - bridgerectifier) * 0.0009);
            }
            if (inputSampleR < -bridgerectifier) {
                IIRsampleR += ((inputSampleR + bridgerectifier) * 0.0009);
            }
            // manipulate IIRSampleR
            inputSampleR = bridgerectifier;
            // apply the distortion transform for reals. Has been converted back to -1/1
            // apply resonant highpass L
            double tempSample = inputSampleL;
            leftSampleA = (leftSampleA * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleA;
            double correction = leftSampleA;
            leftSampleB = (leftSampleB * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleB;
            correction += leftSampleB;
            leftSampleC = (leftSampleC * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleC;
            correction += leftSampleC;
            leftSampleD = (leftSampleD * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleD;
            correction += leftSampleD;
            leftSampleE = (leftSampleE * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleE;
            correction += leftSampleE;
            leftSampleF = (leftSampleF * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleF;
            correction += leftSampleF;
            leftSampleG = (leftSampleG * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleG;
            correction += leftSampleG;
            leftSampleH = (leftSampleH * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleH;
            correction += leftSampleH;
            leftSampleI = (leftSampleI * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleI;
            correction += leftSampleI;
            leftSampleJ = (leftSampleJ * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleJ;
            correction += leftSampleJ;
            leftSampleK = (leftSampleK * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleK;
            correction += leftSampleK;
            leftSampleL = (leftSampleL * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleL;
            correction += leftSampleL;
            leftSampleM = (leftSampleM * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleM;
            correction += leftSampleM;
            leftSampleN = (leftSampleN * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleN;
            correction += leftSampleN;
            leftSampleO = (leftSampleO * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleO;
            correction += leftSampleO;
            leftSampleP = (leftSampleP * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleP;
            correction += leftSampleP;
            leftSampleQ = (leftSampleQ * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleQ;
            correction += leftSampleQ;
            leftSampleR = (leftSampleR * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleR;
            correction += leftSampleR;
            leftSampleS = (leftSampleS * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleS;
            correction += leftSampleS;
            leftSampleT = (leftSampleT * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleT;
            correction += leftSampleT;
            leftSampleU = (leftSampleU * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleU;
            correction += leftSampleU;
            leftSampleV = (leftSampleV * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleV;
            correction += leftSampleV;
            leftSampleW = (leftSampleW * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleW;
            correction += leftSampleW;
            leftSampleX = (leftSampleX * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleX;
            correction += leftSampleX;
            leftSampleY = (leftSampleY * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleY;
            correction += leftSampleY;
            leftSampleZ = (leftSampleZ * midaltAmount) + (tempSample * midAmount);
            tempSample -= leftSampleZ;
            correction += leftSampleZ;
            correction *= fabs(secondharmonicL);
            // scale it directly by second harmonic: DC block is now adding harmonics too
            correction -= secondharmonicL * fpOld;
            // apply the shortbuss processing to output DCblock by subtracting it
            // we are not a peak limiter! not using it to clip or nothin'
            // adding it inversely, it's the same as adding to inputsample only we are accumulating 'stuff' in 'correction'
            inputSampleL -= correction;
            if (inputSampleL < 0) {
                inputSampleL = (inputSampleL * fpNew) - (sin(-inputSampleL) * fpOld);
            }
            // lastly, class A clipping on the negative to combat the one-sidedness
            // uses bloom/antibloom to dial in previous unconstrained behavior
            // end the whole distortion dealiebop
            inputSampleL *= target;
            // begin simplified Groove Wear, outside the scaling
            // varies depending on what sample rate you're at:
            // high sample rate makes it more airy
            gwBL = gwAL;
            gwAL = tempSample = (inputSampleL - gwPrevL);
            tempSample *= gwAfactor;
            tempSample += (gwBL * gwBfactor);
            correction = (inputSampleL - gwPrevL) - tempSample;
            gwPrevL = inputSampleL;
            inputSampleL -= correction;
            // simplified Groove Wear L
            // apply resonant highpass R
            tempSample = inputSampleR;
            rightSampleA = (rightSampleA * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleA;
            correction = rightSampleA;
            rightSampleB = (rightSampleB * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleB;
            correction += rightSampleB;
            rightSampleC = (rightSampleC * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleC;
            correction += rightSampleC;
            rightSampleD = (rightSampleD * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleD;
            correction += rightSampleD;
            rightSampleE = (rightSampleE * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleE;
            correction += rightSampleE;
            rightSampleF = (rightSampleF * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleF;
            correction += rightSampleF;
            rightSampleG = (rightSampleG * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleG;
            correction += rightSampleG;
            rightSampleH = (rightSampleH * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleH;
            correction += rightSampleH;
            rightSampleI = (rightSampleI * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleI;
            correction += rightSampleI;
            rightSampleJ = (rightSampleJ * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleJ;
            correction += rightSampleJ;
            rightSampleK = (rightSampleK * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleK;
            correction += rightSampleK;
            rightSampleL = (rightSampleL * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleL;
            correction += rightSampleL;
            rightSampleM = (rightSampleM * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleM;
            correction += rightSampleM;
            rightSampleN = (rightSampleN * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleN;
            correction += rightSampleN;
            rightSampleO = (rightSampleO * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleO;
            correction += rightSampleO;
            rightSampleP = (rightSampleP * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleP;
            correction += rightSampleP;
            rightSampleQ = (rightSampleQ * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleQ;
            correction += rightSampleQ;
            rightSampleR = (rightSampleR * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleR;
            correction += rightSampleR;
            rightSampleS = (rightSampleS * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleS;
            correction += rightSampleS;
            rightSampleT = (rightSampleT * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleT;
            correction += rightSampleT;
            rightSampleU = (rightSampleU * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleU;
            correction += rightSampleU;
            rightSampleV = (rightSampleV * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleV;
            correction += rightSampleV;
            rightSampleW = (rightSampleW * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleW;
            correction += rightSampleW;
            rightSampleX = (rightSampleX * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleX;
            correction += rightSampleX;
            rightSampleY = (rightSampleY * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleY;
            correction += rightSampleY;
            rightSampleZ = (rightSampleZ * midaltAmount) + (tempSample * midAmount);
            tempSample -= rightSampleZ;
            correction += rightSampleZ;
            correction *= fabs(secondharmonicR);
            // scale it directly by second harmonic: DC block is now adding harmonics too
            correction -= secondharmonicR * fpOld;
            // apply the shortbuss processing to output DCblock by subtracting it
            // we are not a peak limiter! not using it to clip or nothin'
            // adding it inversely, it's the same as adding to inputsample only we are accumulating 'stuff' in 'correction'
            inputSampleR -= correction;
            if (inputSampleR < 0) {
                inputSampleR = (inputSampleR * fpNew) - (sin(-inputSampleR) * fpOld);
            }
            // lastly, class A clipping on the negative to combat the one-sidedness
            // uses bloom/antibloom to dial in previous unconstrained behavior
            // end the whole distortion dealiebop
            inputSampleR *= target;
            // begin simplified Groove Wear, outside the scaling
            // varies depending on what sample rate you're at:
            // high sample rate makes it more airy
            gwBR = gwAR;
            gwAR = tempSample = (inputSampleR - gwPrevR);
            tempSample *= gwAfactor;
            tempSample += (gwBR * gwBfactor);
            correction = (inputSampleR - gwPrevR) - tempSample;
            gwPrevR = inputSampleR;
            inputSampleR -= correction;
            // simplified Groove Wear R
            // begin simplified ADClip L
            drySampleL = inputSampleL;
            if (lastSampleL >= refclip)
            {
                if (inputSampleL < refclip)
                {
                    lastSampleL = ((refclip * hardness) + (inputSampleL * softness));
                }
                else {
                    lastSampleL = refclip;
                }
            }
            if (lastSampleL <= -refclip)
            {
                if (inputSampleL > -refclip)
                {
                    lastSampleL = ((-refclip * hardness) + (inputSampleL * softness));
                }
                else {
                    lastSampleL = -refclip;
                }
            }
            if (inputSampleL > refclip)
            {
                if (lastSampleL < refclip)
                {
                    inputSampleL = ((refclip * hardness) + (lastSampleL * softness));
                }
                else {
                    inputSampleL = refclip;
                }
            }
            if (inputSampleL < -refclip)
            {
                if (lastSampleL > -refclip)
                {
                    inputSampleL = ((-refclip * hardness) + (lastSampleL * softness));
                }
                else {
                    inputSampleL = -refclip;
                }
            }
            lastSampleL = drySampleL;
            // begin simplified ADClip R
            drySampleR = inputSampleR;
            if (lastSampleR >= refclip)
            {
                if (inputSampleR < refclip)
                {
                    lastSampleR = ((refclip * hardness) + (inputSampleR * softness));
                }
                else {
                    lastSampleR = refclip;
                }
            }
            if (lastSampleR <= -refclip)
            {
                if (inputSampleR > -refclip)
                {
                    lastSampleR = ((-refclip * hardness) + (inputSampleR * softness));
                }
                else {
                    lastSampleR = -refclip;
                }
            }
            if (inputSampleR > refclip)
            {
                if (lastSampleR < refclip)
                {
                    inputSampleR = ((refclip * hardness) + (lastSampleR * softness));
                }
                else {
                    inputSampleR = refclip;
                }
            }
            if (inputSampleR < -refclip)
            {
                if (lastSampleR > -refclip)
                {
                    inputSampleR = ((-refclip * hardness) + (lastSampleR * softness));
                }
                else {
                    inputSampleR = -refclip;
                }
            }
            lastSampleR = drySampleR;
            // output dither section
            if (bitDepth == 3) {
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
            }
            else {
                // entire Naturalize section used when not on 32 bit out
                inputSampleL -= noiseShapingL;
                inputSampleR -= noiseShapingR;
                if (bitDepth == 2) {
                    inputSampleL *= 8388608.0; // go to dither at 24 bit
                    inputSampleR *= 8388608.0; // go to dither at 24 bit
                }
                if (bitDepth == 1) {
                    inputSampleL *= 32768.0; // go to dither at 16 bit
                    inputSampleR *= 32768.0; // go to dither at 16 bit
                }
                // begin L
                double benfordize = floor(inputSampleL);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                int hotbinA = floor(benfordize);
                // hotbin becomes the Benford bin value for this number floored
                double totalA = 0;
                if ((hotbinA > 0) && (hotbinA < 10))
                {
                    bynL[hotbinA] += 1;
                    totalA += (301 - bynL[1]);
                    totalA += (176 - bynL[2]);
                    totalA += (125 - bynL[3]);
                    totalA += (97 - bynL[4]);
                    totalA += (79 - bynL[5]);
                    totalA += (67 - bynL[6]);
                    totalA += (58 - bynL[7]);
                    totalA += (51 - bynL[8]);
                    totalA += (46 - bynL[9]);
                    bynL[hotbinA] -= 1;
                }
                else {
                    hotbinA = 10;
                }
                // produce total number- smaller is closer to Benford real
                benfordize = ceil(inputSampleL);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                int hotbinB = floor(benfordize);
                // hotbin becomes the Benford bin value for this number ceiled
                double totalB = 0;
                if ((hotbinB > 0) && (hotbinB < 10))
                {
                    bynL[hotbinB] += 1;
                    totalB += (301 - bynL[1]);
                    totalB += (176 - bynL[2]);
                    totalB += (125 - bynL[3]);
                    totalB += (97 - bynL[4]);
                    totalB += (79 - bynL[5]);
                    totalB += (67 - bynL[6]);
                    totalB += (58 - bynL[7]);
                    totalB += (51 - bynL[8]);
                    totalB += (46 - bynL[9]);
                    bynL[hotbinB] -= 1;
                }
                else {
                    hotbinB = 10;
                }
                // produce total number- smaller is closer to Benford real
                if (totalA < totalB)
                {
                    bynL[hotbinA] += 1;
                    inputSampleL = floor(inputSampleL);
                }
                else
                {
                    bynL[hotbinB] += 1;
                    inputSampleL = ceil(inputSampleL);
                }
                // assign the relevant one to the delay line
                // and floor/ceil signal accordingly
                totalA = bynL[1] + bynL[2] + bynL[3] + bynL[4] + bynL[5] + bynL[6] + bynL[7] + bynL[8] + bynL[9];
                totalA /= 1000;
                totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
                bynL[1] /= totalA;
                bynL[2] /= totalA;
                bynL[3] /= totalA;
                bynL[4] /= totalA;
                bynL[5] /= totalA;
                bynL[6] /= totalA;
                bynL[7] /= totalA;
                bynL[8] /= totalA;
                bynL[9] /= totalA;
                bynL[10] /= 2; // catchall for garbage data
                // end L
                // begin R
                benfordize = floor(inputSampleR);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                hotbinA = floor(benfordize);
                // hotbin becomes the Benford bin value for this number floored
                totalA = 0;
                if ((hotbinA > 0) && (hotbinA < 10))
                {
                    bynR[hotbinA] += 1;
                    totalA += (301 - bynR[1]);
                    totalA += (176 - bynR[2]);
                    totalA += (125 - bynR[3]);
                    totalA += (97 - bynR[4]);
                    totalA += (79 - bynR[5]);
                    totalA += (67 - bynR[6]);
                    totalA += (58 - bynR[7]);
                    totalA += (51 - bynR[8]);
                    totalA += (46 - bynR[9]);
                    bynR[hotbinA] -= 1;
                }
                else {
                    hotbinA = 10;
                }
                // produce total number- smaller is closer to Benford real
                benfordize = ceil(inputSampleR);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                hotbinB = floor(benfordize);
                // hotbin becomes the Benford bin value for this number ceiled
                totalB = 0;
                if ((hotbinB > 0) && (hotbinB < 10))
                {
                    bynR[hotbinB] += 1;
                    totalB += (301 - bynR[1]);
                    totalB += (176 - bynR[2]);
                    totalB += (125 - bynR[3]);
                    totalB += (97 - bynR[4]);
                    totalB += (79 - bynR[5]);
                    totalB += (67 - bynR[6]);
                    totalB += (58 - bynR[7]);
                    totalB += (51 - bynR[8]);
                    totalB += (46 - bynR[9]);
                    bynR[hotbinB] -= 1;
                }
                else {
                    hotbinB = 10;
                }
                // produce total number- smaller is closer to Benford real
                if (totalA < totalB)
                {
                    bynR[hotbinA] += 1;
                    inputSampleR = floor(inputSampleR);
                }
                else
                {
                    bynR[hotbinB] += 1;
                    inputSampleR = ceil(inputSampleR);
                }
                // assign the relevant one to the delay line
                // and floor/ceil signal accordingly
                totalA = bynR[1] + bynR[2] + bynR[3] + bynR[4] + bynR[5] + bynR[6] + bynR[7] + bynR[8] + bynR[9];
                totalA /= 1000;
                totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
                bynR[1] /= totalA;
                bynR[2] /= totalA;
                bynR[3] /= totalA;
                bynR[4] /= totalA;
                bynR[5] /= totalA;
                bynR[6] /= totalA;
                bynR[7] /= totalA;
                bynR[8] /= totalA;
                bynR[9] /= totalA;
                bynR[10] /= 2; // catchall for garbage data
                // end R
                if (bitDepth == 2) {
                    inputSampleL /= 8388608.0;
                    inputSampleR /= 8388608.0;
                }
                if (bitDepth == 1) {
                    inputSampleL /= 32768.0;
                    inputSampleR /= 32768.0;
                }
                noiseShapingL += inputSampleL - drySampleL;
                noiseShapingR += inputSampleR - drySampleR;
            }
            if (inputSampleL > refclip) {
                inputSampleL = refclip;
            }
            if (inputSampleL < -refclip) {
                inputSampleL = -refclip;
            }
            // iron bar prohibits any overs
            if (inputSampleR > refclip) {
                inputSampleR = refclip;
            }
            if (inputSampleR < -refclip) {
                inputSampleR = -refclip;
            }
            // iron bar prohibits any overs
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::righteous4
