#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::decrackle {

constexpr std::string_view k_name{ "DeCrackle" };
constexpr std::string_view k_short_description{
    "DeCrackle isolates clicks and vinyl crackles to remove them."
};
constexpr std::string_view k_long_description{
    "This was a lot of work, for a good purpose (cleaning up vinyl records), being asked for (ever since I did the livestreams developing this, I've been hearing about it) and getting put out as it is. There might well be good ways to inappropriately use it, and I might well get something out of it in its intended use, but all the same I have to establish expectations.The fantasy of a decrackle plugin is that you can throw a scratchy ol' record at it, and get a lovely noiseless version back out, that is perfect and has all the tone you want, with none of the noise. Seemed worth trying. I could try to isolate bursts of energy that were the right kind (more side energy than mid, energy higher than the average energy happening at the time, and so on) and mute them. Or, apply a filter, and snap to the filter while the click is happening, so the lows remain unaltered, then back to the raw audio right away.Turned out to be a lot more tricky to get it to work… but there's a problem. The whole idea was to let the audio through totally untouched, rapidly triggering the decrackle for every instant it's needed. This is happening, but when there's heavy crackle, the noise is so constant that there's no way to play the raw audio between the crackles… and there's always a lot of quieter crackle happening between the louder pops and clicks. So the concept worked, but it didn't work in the sense of making all the noise go away. And since it barely touches or affects the rest of the audio, you get a powerful sense of whatever crackle remains… minus the REALLY loud bits, that get neatly removed as if they weren't there.Filter goes from bass-only to full frequency and is the audio you replace clicks with. You tune it to hide the transitions (full bass might not be the ideal setting, you're looking for minimal disruption). Window is important: it'll go from very narrow, to extremely wide. It's set up so it can be abused to let through a big section of the click's surroundings, which can be used for other purposes. Threshld gets lowered to increase the effect, and requires delicate adjustment: be careful about letting it kick in on actual music as that will sound bad. Surface goes from normal at zero, to intense treble filtering. It's not purely a lowpass, it tries to respond to micro-crackles but not underlying high frequencies, and you can use it to subtly mute general surface noise in quiet sections. Finally, Dry/Wet has a special trick: if you set it to 0.0, full dry, it switches to delta monitoring so you can hear ONLY the clicks. If you hear music coming through that, you know Threshld has to be higher.Since this probably isn't going to excite would-be decracklers unless they're really committed to keeping the tone of the audio at all costs, including 'sounding like you didn't bother decrackling' (certain music might be more resilient to lower thresholds), I have an alternate suggestion for amusingly misusing DeCrackle.Use high Window settings and full Dry to make it a neat little percussive gate which is also a highpass, and which isolates the attack (because it's treating it as a click where it has to remove the entire onset of the sound). It will apply a fair amount of latency: DeCrackle is FAR from zero latency, unlike most of my plugins. That does let it gate around a peak while presenting the whole peak without cropping the front off.Or, use the Dry/Wet to reinvent the attack of percussive sounds, from very high frequency spikes to bassy thumps. Filter controls how much of the attack is allowed to be bright, Window controls how much of the attack you grab, Threshld interacts with that and Dry/Wet blends how much of the attack is removed. This is going to work well for darkening and refining bright attacks on kicks you want to make heavier, since it interacts with the Filter up top plus the Window setting plus the processing latency of the plugin.DeCrackle is a lot of tricky, hard-to-balance processing tuned to invisibly remove just the right crackles and clicks from only certain vinyl recordings. If it works for you like that, I'm thrilled! If it works when used on totally different things, I guess the trickiness of it makes the process more interesting or gives you more sophisticated results. Your mileage may vary. Hope you're enjoying kBeyond, I'm working every day on following it up with even better stuff :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class DeCrackle final : public Effect<T>
{
    static constexpr int kshort = 1600;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'dcrk';
    float A;
    float B;
    float C;
    float D;
    float E;
    double aAL[kshort + 5];
    double aBL[kshort + 5];
    double aAR[kshort + 5];
    double aBR[kshort + 5];
    double aCL[kshort + 5];
    int count;
    double iirLSample[18];
    double iirRSample[18];
    double iirLAngle[18];
    double iirRAngle[18];
    double iirCSample[18];
    double iirTargetL;
    double iirTargetR;
    double iirClickL;
    double iirClickR;
    double prevSampleL;
    double prevSampleR;
    double prevSurfaceL;
    double prevSurfaceR;
    double prevOutL;
    double prevOutR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeCrackle()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        for (int x = 0; x < kshort + 2; x++) {
            aAL[x] = 0.0;
            aAR[x] = 0.0;
        }
        for (int x = 0; x < kshort + 2; x++) {
            aBL[x] = 0.0;
            aBR[x] = 0.0;
        }
        for (int x = 0; x < kshort + 2; x++) {
            aCL[x] = 0.0;
        }
        count = 1;
        for (int x = 0; x < 17; x++) {
            iirLSample[x] = 0.0;
            iirRSample[x] = 0.0;
            iirCSample[x] = 0.0;
            iirLAngle[x] = 0.0;
            iirRAngle[x] = 0.0;
        }
        iirTargetL = 0.0;
        iirTargetR = 0.0;
        iirClickL = 0.0;
        iirClickR = 0.0;
        prevSampleL = 0.0;
        prevSampleR = 0.0;
        prevSurfaceL = 0.0;
        prevSurfaceR = 0.0;
        prevOutL = 0.0;
        prevOutR = 0.0;
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
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

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
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "filter"; break;
            case kParamB: return "window"; break;
            case kParamC: return "thresld"; break;
            case kParamD: return "surface"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Filter"; break;
            case kParamB: return "Window"; break;
            case kParamC: return "Thresld"; break;
            case kParamD: return "Surface"; break;
            case kParamE: return "Dry/Wet"; break;

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
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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
        int offset = (int)(overallscale * 1.1);
        double maxHeight = 1.0 * overallscale;
        double filterOut = pow((A * 0.618) + 0.1, 2.0) / overallscale;
        double filterRef = pow(((1.0 - B) * 0.618) + 0.1, 2.0) / overallscale;
        double iirCut = (pow(1.0 - B, 2.0) * 0.2) / overallscale;
        int adjDelay = (int)(B * ((double)(kshort / 8) * overallscale)) - 2.0;
        if (adjDelay > kshort - 1) {
            adjDelay = kshort - 1; // don't break on silly samplerates
        }
        int halfTrig = fmin(0.5 + pow(B, 3.0), 0.999) * adjDelay;
        int halfRaw = 0.5 * adjDelay;
        int halfBez = 0.5 * adjDelay;
        double threshold = pow(C * 0.618, 2.0) - 0.1;
        double surface = (1.0 - pow(1.0 - D, 3.0)) * 0.9;
        double wet = E;
        int options = 0;
        if (wet < 0.001) {
            options = 1;
        }
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
            inputSampleL -= prevSampleL;
            prevSampleL = drySampleL;
            inputSampleR -= prevSampleR;
            prevSampleR = drySampleR;
            inputSampleL *= 16.0;
            inputSampleR *= 16.0;
            double bezL = drySampleL;
            for (int x = 0; x < 6; x++) {
                iirLAngle[x] = (iirLAngle[x] * (1.0 - filterOut)) + ((bezL - iirLSample[x]) * filterOut);
                bezL = ((iirLSample[x] + (iirLAngle[x] * filterOut)) * (1.0 - filterOut)) + (bezL * filterOut);
                iirLSample[x] = ((iirLSample[x] + (iirLAngle[x] * filterOut)) * (1.0 - filterOut)) + (bezL * filterOut);
            } // lowpass audio band
            double bezR = drySampleR;
            for (int x = 0; x < 6; x++) {
                iirRAngle[x] = (iirRAngle[x] * (1.0 - filterOut)) + ((bezR - iirRSample[x]) * filterOut);
                bezR = ((iirRSample[x] + (iirRAngle[x] * filterOut)) * (1.0 - filterOut)) + (bezR * filterOut);
                iirRSample[x] = ((iirRSample[x] + (iirRAngle[x] * filterOut)) * (1.0 - filterOut)) + (bezR * filterOut);
            } // lowpass audio band
            double rect = fabs(drySampleL * drySampleR * 64.0);
            for (int x = 0; x < 6; x++) {
                rect = fabs((iirCSample[x] * (1.0 - filterRef)) + (rect * filterRef));
                iirCSample[x] = (iirCSample[x] * (1.0 - filterRef)) + (rect * filterRef);
            } // lowpass rectified CV band
            aAL[count] = drySampleL;
            aAR[count] = drySampleR;
            aBL[count] = bezL;
            aBR[count] = bezR;
            aCL[count] = rect;
            count++;
            if (count < 0 || count > adjDelay) {
                count = 0;
            }
            double nearVal = rect;
            double farVal = aCL[count - ((count > adjDelay) ? adjDelay + 1 : 0)];
            // nearVal and farVal are the smoothed rectified version
            double prevL = aAL[(count + halfRaw + offset) - (((count + halfRaw + offset) > adjDelay) ? adjDelay + 1 : 0)];
            double prevR = aAR[(count + halfRaw + offset) - (((count + halfRaw + offset) > adjDelay) ? adjDelay + 1 : 0)];
            double outL = aAL[(count + halfRaw) - (((count + halfRaw) > adjDelay) ? adjDelay + 1 : 0)];
            double outR = aAR[(count + halfRaw) - (((count + halfRaw) > adjDelay) ? adjDelay + 1 : 0)];
            double outBezL = aBL[(count + halfBez) - (((count + halfBez) > adjDelay) ? adjDelay + 1 : 0)];
            double outBezR = aBR[(count + halfBez) - (((count + halfBez) > adjDelay) ? adjDelay + 1 : 0)];
            // these are the various outputs to fade between
            double trigL = aAL[(count + halfTrig) - (((count + halfTrig) > adjDelay) ? adjDelay + 1 : 0)];
            double trigR = aAR[(count + halfTrig) - (((count + halfTrig) > adjDelay) ? adjDelay + 1 : 0)];
            double deClickL = pow(fmax((fabs(trigL) - threshold) - fmax(nearVal, farVal), 0.0) * 16.0, 3.0);
            double deClickR = pow(fmax((fabs(trigR) - threshold) - fmax(nearVal, farVal), 0.0) * 16.0, 3.0);
            iirTargetL = fmax(iirTargetL - iirCut, 0.0);
            iirTargetR = fmax(iirTargetR - iirCut, 0.0); // taper down at iirCut speed
            if (deClickL > iirTargetL) {
                iirTargetL = fmin(deClickL, maxHeight);
            }
            if (deClickR > iirTargetR) {
                iirTargetR = fmin(deClickR, maxHeight); // beyond 1.0 to stretch window
            }
            if (deClickR * 0.618 > iirTargetL) {
                iirTargetL = fmin(deClickR * 0.618, maxHeight);
            }
            if (deClickL * 0.618 > iirTargetR) {
                iirTargetR = fmin(deClickL * 0.618, maxHeight); // opposite channel
            }
            iirClickL = fmin(iirClickL + iirCut, iirTargetL);
            iirClickR = fmin(iirClickR + iirCut, iirTargetR); // taper up on attack
            inputSampleL = (outBezL * fmin(iirClickL, 1.0)) + (outL * (1.0 - fmin(iirClickL, 1.0)));
            inputSampleR = (outBezR * fmin(iirClickR, 1.0)) + (outR * (1.0 - fmin(iirClickR, 1.0)));
            // declicking is applied: crossfade between darkened and normal
            if (wet < 1.0 && wet > 0.0) {
                inputSampleL = (inputSampleL * wet) + (outL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (outR * (1.0 - wet));
            } // neither full wet, nor the 'clicks only' special case below
            if (wet == 0.0) {
                inputSampleL = outL - inputSampleL;
                inputSampleR = outR - inputSampleR;
            } // clicks only at full dry
            double recordVolume = fmax(fmax(nearVal, farVal), fmax(prevL, prevR)) + 0.001; // engage only at quietest
            double surfaceL = sin(fmin((fabs(outL - prevL) / recordVolume) * surface, 3.14159265358979)) * 0.5;
            double surfaceR = sin(fmin((fabs(outR - prevR) / recordVolume) * surface, 3.14159265358979)) * 0.5;
            double gateOnAudio = fmax(surface - (recordVolume * surface * 4.0), 0.0);
            if (surface > 0.0 && wet > 0.0) {
                inputSampleL = (prevOutL * surfaceL) + (inputSampleL * (1.0 - surfaceL));
                inputSampleR = (prevOutR * surfaceR) + (inputSampleR * (1.0 - surfaceR));
                inputSampleL = (prevOutL * gateOnAudio) + (inputSampleL * (1.0 - gateOnAudio));
                inputSampleR = (prevOutR * gateOnAudio) + (inputSampleR * (1.0 - gateOnAudio));
                prevOutL = (prevOutL * gateOnAudio) + (inputSampleL * (1.0 - gateOnAudio));
                prevOutR = (prevOutR * gateOnAudio) + (inputSampleR * (1.0 - gateOnAudio));
            } // if we're also adding surface noise reduction that goes here
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
} // namespace airwindohhs::decrackle
