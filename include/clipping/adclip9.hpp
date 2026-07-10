#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::adclip9 {

constexpr std::string_view k_name{ "ADClip9" };
constexpr std::string_view k_short_description{
    "ADClip9 is the ClipOnly3 version of a loudenator/biggenator."
};
constexpr std::string_view k_long_description{
    "Update time! As promised, I'm putting out ClipOnly3 as an ADClip (the fundamental concepts are still ADClip, so that makes this ADClip9.)Multiple things have changed since ADClip8, more than two years ago. The way this one works there's no benefit to stacking up instances, so the colorful mode names like 'Apocalypse' go by the wayside… but more useful things in ADClip are retained or even expanded.The three modes are Boost, Match, and ClipOnly. That means you've got a 'pure loudenate' setting, a 'level matched' setting with its own dB-measured control (which kept me up late at night rebuilding everything for the Nth time because it applied the correct amount, but wasn't SHOWING the relevant dB in the interface) and a setting that's silent until you start clipping and then shows the delta: this one can be used if you're looking to not grind against musical content but only transient spikes.The remaining controls are Noise and Ceiling, and technically they're both thresholds. They are NOT measured in dB, they're made to give subtler and subtler control as you go nearer to clipping directly at 0dB. The default settings of 0.7 and 0.75 closely match what ClipOnly3 does, which gets intersample peaks as close to 0 as I dared. If you lower Noise, what you're doing is hitting the 'transition to noise' threshold sooner, without affecting the hard clip limit. If you lower Ceiling, you're lowering the hard clip limit. You'll probably end up moving both, but if you do it's up to you to find your own balance point. Don't expect it to become a 'replace the audio with noise and clip the rest' plugin: Silhouette might be your best bet for that, ADClip9 is still the Airwindows take on 'softening the edges of clips with noise' as found in ClipOnly3.It's been a couple years since I advanced the state of the art in 'unreasonable loudenation' and let people get their hands dirtier than with ClipOnly (which is always designed to be a transparent safety clipper with no controls that happens to behave perfectly.) So here you go, with a bit of luck this'll hold people while I work on the opposite extremes of dynamics :)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ADClip9 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    double lastSampleL;
    double lastDryL;
    double intermediateL[18];
    double slewL[34];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double lastDryR;
    double intermediateR[18];
    double slewR[34];
    bool wasPosClipR;
    bool wasNegClipR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ADClip9()
    {
        A = 0.0;
        B = 0.0;
        C = 0.7;
        D = 0.75;
        E = 0.0;
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 17; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
        }
        for (int x = 0; x < 33; x++) {
            slewL[x] = 0.0;
            slewR[x] = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.7; break;
            case kParamD: return 0.75; break;
            case kParamE: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "boost"; break;
            case kParamB: return "match"; break;
            case kParamC: return "noise"; break;
            case kParamD: return "ceiling"; break;
            case kParamE: return "mode"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Boost"; break;
            case kParamB: return "Match"; break;
            case kParamC: return "Noise"; break;
            case kParamD: return "Ceiling"; break;
            case kParamE: return "Mode"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return "dB"; break;
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
        double inputGain = pow(10.0, (A * 18.0) / 20.0);
        double match = pow(10.0, (B * 18.0) / 20.0);
        double noiseLevel = 1.0 - pow(1.0 - C, 2.0);
        double ceiling = 1.0 - pow(1.0 - D, 2.0);
        int mode = (int)(E * 2.999) + 1;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            switch (mode) {
                case 1:
                    inputSampleL *= inputGain;
                    inputSampleR *= inputGain;
                    break; // Boost
                case 2:
                    inputSampleL *= match;
                    inputSampleR *= match;
                    break; // Match
                case 3:
                    inputSampleL *= inputGain;
                    inputSampleR *= inputGain;
                    break; // ClipOnly
            }
            // this is our output mode switch, showing the effects
            double overshootL = lastDryL;
            lastDryL = inputSampleL;
            double overshootR = lastDryR;
            lastDryR = inputSampleR;
            // begin ClipOnly3 as a little, compressed chunk that can be dropped into code
            double noise = 1.0 - ((double(fpdL) / UINT32_MAX) * (1.0 - noiseLevel)); // 0.076
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = (noiseLevel * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = ceiling;
                }
            }
            wasPosClipL = false;
            if (inputSampleL > noiseLevel) {
                wasPosClipL = true;
                inputSampleL = (noiseLevel * noise) + (lastSampleL * (1.0 - noise));
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = (-noiseLevel * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = -ceiling;
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -noiseLevel) {
                wasNegClipL = true;
                inputSampleL = (-noiseLevel * noise) + (lastSampleL * (1.0 - noise));
            }
            slewL[spacing * 2] = fabs(lastSampleL - inputSampleL);
            for (int x = spacing * 2; x > 0; x--) {
                slewL[x - 1] = slewL[x];
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL;
            // latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0];
            if (wasPosClipL || wasNegClipL) {
                for (int x = spacing; x > 0; x--) {
                    lastSampleL += intermediateL[x];
                }
                lastSampleL /= spacing;
            }
            double finalSlew = 0.0;
            for (int x = spacing * 2; x >= 0; x--) {
                if (finalSlew < slewL[x]) {
                    finalSlew = slewL[x];
                }
            }
            double postclip = ceiling / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleL > postclip) {
                inputSampleL = postclip;
            }
            if (inputSampleL < -postclip) {
                inputSampleL = -postclip;
            }
            noise = 1.0 - ((double(fpdR) / UINT32_MAX) * (1.0 - noiseLevel)); // 0.076
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = (noiseLevel * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = ceiling;
                }
            }
            wasPosClipR = false;
            if (inputSampleR > noiseLevel) {
                wasPosClipR = true;
                inputSampleR = (noiseLevel * noise) + (lastSampleR * (1.0 - noise));
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = (-noiseLevel * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = -ceiling;
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -noiseLevel) {
                wasNegClipR = true;
                inputSampleR = (-noiseLevel * noise) + (lastSampleR * (1.0 - noise));
            }
            slewR[spacing * 2] = fabs(lastSampleR - inputSampleR);
            for (int x = spacing * 2; x > 0; x--) {
                slewR[x - 1] = slewR[x];
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR;
            // latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0];
            if (wasPosClipR || wasNegClipR) {
                for (int x = spacing; x > 0; x--) {
                    lastSampleR += intermediateR[x];
                }
                lastSampleR /= spacing;
            }
            finalSlew = 0.0;
            for (int x = spacing * 2; x >= 0; x--) {
                if (finalSlew < slewR[x]) {
                    finalSlew = slewR[x];
                }
            }
            postclip = ceiling / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleR > postclip) {
                inputSampleR = postclip;
            }
            if (inputSampleR < -postclip) {
                inputSampleR = -postclip;
            }
            // end ClipOnly3 as a little, compressed chunk that can be dropped into code
            switch (mode) {
                case 1: break; // Boost
                case 2:
                    inputSampleL /= match;
                    inputSampleR /= match;
                    break; // Match
                case 3:
                    inputSampleL = (inputSampleL - overshootL) / inputGain;
                    inputSampleR = (inputSampleR - overshootR) / inputGain;
                    break; // Clip Only
            }
            // this is our output mode switch, showing the effects
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::adclip9
