#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::beam {

constexpr std::string_view k_name{ "Beam" };
constexpr std::string_view k_short_description{
    "Beam is a wordlength reducer that tries to heighten sonority."
};
constexpr std::string_view k_long_description{
    "Beam’s a wordlength reducer like Dark, but instead of seeking to always give you the least departure from whatever trajectory the audio’s on (like Dark: and that suppresses highs, including noise in the highs), Beam seeks to make all waveforms converge on the same angle (either ascending, or descending). Imagine a world of triangle-waves, the deepest frequencies taking the most energy, and progressively quieter until high frequencies are in balance with the lows. That’s Beam. Beam has a ‘focus’ control that sort of optimizes the frequency range it’s providing a window into (as in, lower settings SEEM to encourage focus on deeper sounds, higher settings SEEMS to highlight the treble, and the 0.5 is set to zero in on where our hearing’s most sensitive.) And it uses those frequencies, across a broad range of possible sounds, to emphasize the energy and depth of the audio to shocking effect.Except, it’s not. It doesn’t know what a frequency is. It’s only ‘dithering’ in such a way as to try and accentuate certain waveform slope angles, and has no real power to do even that beyond statistical averaging of a bunch of samples.On top of that, its noise floor is WEIRD. Dark gates into silence, sort of well-behavedly. Normal dithers become noise seamlessly. Beam goes nuts and screams, and the only thing I can say for it is, raw truncation is worse. It’s super weird… and yet, even while it’s doing that, the background audio retains a startling depth and personality. Or at least so it seems… even more than Dark, with this one you turn up DeRez and nothing happens to the sound at all. You don’t even hear the noise until it’s silly loud. (there will be a series of dither reissues with DeRez, both for auditioning and for lo-fi duties.) It might even be a ‘sonic maximizer’, putting tone qualities into the audio that weren’t there to begin with.I admit I don’t know what to make of this at all. I know how I got it, and I know it’s doing what I made it to do. But nobody knows what you get if you sculpt audio, not by frequencies or loudness, but by reinforcing certain waveform slope angles, because to my knowledge nobody has ever wanted to do that or had a way to make it happen, even in subtle ways like this. And now that I’ve done it, I’m not certain it’s better than Dark. I understand what Dark’s doing, and why that’s useful. This is a wilder beast. Looking forward to getting people’s reactions… because one of these may replace NJAD in my flagship Airwindows plugin, Monitoring. (followup: turns out it is Dark in Monitoring2)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class Beam final : public Effect<T>
{
    float lastSampleL[100];
    float lastSampleR[100];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Beam()
    {
        A = 1.0;
        B = 0.5;
        C = 0.0;
        for (int count = 0; count < 99; count++) {
            lastSampleL[count] = 0;
            lastSampleR[count] = 0;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
        kParamC:
            return C;
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
            return 1.0;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.0;
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
            return "quant";
            break;
        kParamB:
            return "focus";
            break;
        kParamC:
            return "derez";
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
            return "Quant";
            break;
        kParamB:
            return "Focus";
            break;
        kParamC:
            return "DeRez";
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
        kParamC:
            return std::to_string(C);
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
        kParamC:
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

        int processing = (VstInt32)(A * 1.999);
        float sonority = B * 1.618033988749894848204586;
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int depth = (int)(17.0 * overallscale);
        if (depth < 3) {
            depth = 3;
        }
        if (depth > 98) {
            depth = 98;
        }
        bool highres = false;
        if (processing == 1) {
            highres = true;
        }
        float scaleFactor;
        if (highres) {
            scaleFactor = 8388608.0;
        }
        else {
            scaleFactor = 32768.0;
        }
        float derez = C;
        if (derez > 0.0) {
            scaleFactor *= pow(1.0 - derez, 6);
        }
        if (scaleFactor < 0.0001) {
            scaleFactor = 0.0001;
        }
        float outScale = scaleFactor;
        if (outScale < 8.0) {
            outScale = 8.0;
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleL *= scaleFactor;
            inputSampleR *= scaleFactor;
            // 0-1 is now one bit, now we dither
            // We are doing it first Left, then Right, because the loops may run faster if
            // they aren't too jammed full of variables. This means re-running code.
            // begin left
            int quantA = floor(inputSampleL);
            int quantB = floor(inputSampleL + 1.0);
            // to do this style of dither, we quantize in either direction and then
            // do a reconstruction of what the result will be for each choice.
            // We then evaluate which one we like, and keep a history of what we previously had
            float expectedSlewA = 0;
            for (int x = 0; x < depth; x++) {
                expectedSlewA += (lastSampleL[x + 1] - lastSampleL[x]);
            }
            float expectedSlewB = expectedSlewA;
            expectedSlewA += (lastSampleL[0] - quantA);
            expectedSlewB += (lastSampleL[0] - quantB);
            // now we have a collection of all slews, averaged and left at total scale
            float clamp = sonority;
            if (fabs(inputSampleL) < sonority) {
                clamp = fabs(inputSampleL);
            }
            float testA = fabs(fabs(expectedSlewA) - clamp);
            float testB = fabs(fabs(expectedSlewB) - clamp);
            // doing this means the result will be lowest when it's reaching the target slope across
            // the desired time range, either positively or negatively. Should produce the same target
            // at whatever sample rate, as high rate stuff produces smaller increments.
            if (testA < testB) {
                inputSampleL = quantA;
            }
            else {
                inputSampleL = quantB;
            }
            // select whichever one departs LEAST from the vector of averaged
            // reconstructed previous final samples. This will force a kind of dithering
            // as it'll make the output end up as smooth as possible
            for (int x = depth; x >= 0; x--) {
                lastSampleL[x + 1] = lastSampleL[x];
            }
            lastSampleL[0] = inputSampleL;
            // end left
            // begin right
            quantA = floor(inputSampleR);
            quantB = floor(inputSampleR + 1.0);
            // to do this style of dither, we quantize in either direction and then
            // do a reconstruction of what the result will be for each choice.
            // We then evaluate which one we like, and keep a history of what we previously had
            expectedSlewA = 0;
            for (int x = 0; x < depth; x++) {
                expectedSlewA += (lastSampleR[x + 1] - lastSampleR[x]);
            }
            expectedSlewB = expectedSlewA;
            expectedSlewA += (lastSampleR[0] - quantA);
            expectedSlewB += (lastSampleR[0] - quantB);
            // now we have a collection of all slews, averaged and left at total scale
            clamp = sonority;
            if (fabs(inputSampleR) < sonority) {
                clamp = fabs(inputSampleR);
            }
            testA = fabs(fabs(expectedSlewA) - clamp);
            testB = fabs(fabs(expectedSlewB) - clamp);
            // doing this means the result will be lowest when it's reaching the target slope across
            // the desired time range, either positively or negatively. Should produce the same target
            // at whatever sample rate, as high rate stuff produces smaller increments.
            if (testA < testB) {
                inputSampleR = quantA;
            }
            else {
                inputSampleR = quantB;
            }
            // select whichever one departs LEAST from the vector of averaged
            // reconstructed previous final samples. This will force a kind of dithering
            // as it'll make the output end up as smooth as possible
            for (int x = depth; x >= 0; x--) {
                lastSampleR[x + 1] = lastSampleR[x];
            }
            lastSampleR[0] = inputSampleR;
            // end right
            inputSampleL /= outScale;
            inputSampleR /= outScale;
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::beam
