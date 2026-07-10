#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::cliponly3 {

constexpr std::string_view k_name{ "ClipOnly3" };
constexpr std::string_view k_short_description{
    "ClipOnly3 is just plain stinkin' loud."
};
constexpr std::string_view k_long_description{
    "Turns out you can go harder than ClipOnly2, through the power of noise!This plugin takes an idea currently rocking its way through bass music loudenation, the idea of covering up the sound of hardclipping through using noise, and runs in a completely different direction with it, to substantially upgrade ClipOnly while still not working quite the same as anything else.There's still the ClipOnly behavior of softening the onsets and exits from clipping. Still the total not-changing of any sample below the threshold. Still an ability to not add edgy digital glare to clipped highs like cymbals.But now, delicately clipped stuff gets a delicate tinge of noise. Hitting the clip harder begins to produce more noise… and then as you blow past that limit in turn, the noise gets replaced with purely hard clipping to a greater and greater extent, so that hyper-slamming restricts the noise part to only the onset and departure edges of the clips. And then there's an intersample peaking stage that was put in to deal with super-high-frequency test tones, which lets the maximum clip amplitude go insanely high without triggering analog peaking.So, like all ClipOnly, it's a tiny featureless plugin that will pass most audio untouched. It's just that when you slam it, there's practically no limit to what you can get away with. It's a clipper, so if you need a limiter this is probably not for you. But if you already knew you were going to be getting some clipping, or if your sounds mask it, or if you were just looking for total meltdown… have fun :)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ClipOnly3 final : public Effect<T>
{
    double lastSampleL;
    double intermediateL[18];
    double slewL[34];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[18];
    double slewR[34];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly3
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ClipOnly3()
    {
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
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {

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
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            // begin ClipOnly3 as a little, compressed chunk that can be dropped into code
            double noise = 1.0 - ((double(fpdL) / UINT32_MAX) * 0.076);
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = (0.9085097 * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = 0.94; //~-0.2dB to nearly match ClipOnly and ClipOnly2
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9085097) {
                wasPosClipL = true;
                inputSampleL = (0.9085097 * noise) + (lastSampleL * (1.0 - noise));
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = (-0.9085097 * noise) + (inputSampleL * (1.0 - noise));
                }
                else {
                    lastSampleL = -0.94;
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9085097) {
                wasNegClipL = true;
                inputSampleL = (-0.9085097 * noise) + (lastSampleL * (1.0 - noise));
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
            double postclip = 0.94 / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleL > postclip) {
                inputSampleL = postclip;
            }
            if (inputSampleL < -postclip) {
                inputSampleL = -postclip;
            }
            noise = 1.0 - ((double(fpdR) / UINT32_MAX) * 0.076);
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = (0.9085097 * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = 0.94; //~-0.2dB to nearly match ClipOnly and ClipOnly2
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9085097) {
                wasPosClipR = true;
                inputSampleR = (0.9085097 * noise) + (lastSampleR * (1.0 - noise));
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = (-0.9085097 * noise) + (inputSampleR * (1.0 - noise));
                }
                else {
                    lastSampleR = -0.94;
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9085097) {
                wasNegClipR = true;
                inputSampleR = (-0.9085097 * noise) + (lastSampleR * (1.0 - noise));
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
            postclip = 0.94 / (1.0 + (finalSlew * 1.3986013));
            if (inputSampleR > postclip) {
                inputSampleR = postclip;
            }
            if (inputSampleR < -postclip) {
                inputSampleR = -postclip;
            }
            // end ClipOnly3 as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::cliponly3
