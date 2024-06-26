#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::cliponly2 {

constexpr std::string_view k_name{ "ClipOnly2" };
constexpr std::string_view k_short_description{
    "ClipOnly2 suppresses the brightness of digital clipping without affecting unclipped samples, at any sample rate."
};
constexpr std::string_view k_long_description{
    "ClipOnly2 is the heart of my mastering-grade clipping algorithm. Instead of trying to define the cleanest possible nasty sharp edge, or doing a soft-clip thing, ClipOnly passes through ALL nonclipped samples totally untouched… but when you get a clipped sample, what ClipOnly does is it takes the sample entering clipping, and the sample exiting clipping, and it interpolates between the last unclipped sample and the clipped stuff. So, it is synthesizing a soft entry and exit from what is otherwise total hard clipping, and if only the one sample clipped? That very bright clip simply goes away, turned right down.This produces a hard-clip suitable for safety clipper purposes, which is purely ‘bypass’ (plus a one sample delay to allow for the processing), with softer highs than you’d get from any pure hard-clip, no matter how oversampled. It’s an alternate technique, and is also pretty CPU-efficient.ClipOnly2 takes this principle and changes the ‘one sample’ to ‘the space of one sample at 44.1k’. Same tone, same ear-friendly approach to clipping extreme highs, except that now it’s effective at high sample rates. I’m demonstrating it and its predecessor at 96k, but ClipOnly2 is designed to work up to 700k or so, in case people get giddy with their newfound power :)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ClipOnly2 final : public Effect<T>
{
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    // uint32_t fpdL;
    // uint32_t fpdR; //leave off
    // default stuff

  public:
    ClipOnly2()
    {
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
        // fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
        // fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX; //leave off for ClipOnly
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
        // double hardness = 0.7390851332151606; // x == cos(x)
        // double softness = 0.260914866784839; // 1.0 - hardness
        // double refclip = 0.9549925859; // -0.2dB we're making all this pure raw code
        // refclip*hardness = 0.705820822569392  to use ClipOnly as a prefab code-chunk.
        // refclip*softness = 0.249171763330607	Seven decimal places is plenty as it's
        // not related to the original sound much: it's an arbitrary position in softening.
        // This is where the numbers come from, referencing the code of the original ClipOnly
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            //		if (fabs(inputSampleL)<1.18e-43) inputSampleL = fpdL * 1.18e-43;
            //		if (fabs(inputSampleR)<1.18e-43) inputSampleR = fpdR * 1.18e-43;
            // do NOT use on ClipOnly, it is almost always a pure bypass
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
            //		int expon; frexp((double)inputSampleL, &expon);
            //		fpdL ^= fpdL << 13; fpdL ^= fpdL >> 17; fpdL ^= fpdL << 5;
            //		inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            //		frexp((double)inputSampleR, &expon);
            //		fpdR ^= fpdR << 13; fpdR ^= fpdR >> 17; fpdR ^= fpdR << 5;
            //		inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            // end 64 bit stereo floating point dither
            // do NOT use on ClipOnly, it is almost always a pure bypass
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::cliponly2
