#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::finalclip {

constexpr std::string_view k_name{ "FinalClip" };
constexpr std::string_view k_short_description{
    "FinalClip is one stage of ADClip8 set up to clip +6dB for Final Cut Pro."
};
constexpr std::string_view k_long_description{
    "If you like ADClip8, this came first and is basically the seed of ADClip8.FinalClip is a few variations on ClipOnly2, with a bit of slew clipping thrown in, and adjusted to work specifically for Final Cut Pro, the video software I use. This is because if I put any normal clipper such as you'd use in a DAW into Final Cut, it applies the clip to the cut at 6dB down. I can only assume this has to do with how it gain stages stuff for video. There's a way to gather clips into some sort of large composite clip and apply a 'master buss' plugin to the sum, and if you thought Airwindows Console in DAWs was awkward to set up, yikes! I'm not even trying to do that. I just run camera audio directly to the output of Final Cut and use that.Because the clip point is 6 dB down, it seemed like I could build a special clipper just for Final Cut, that would always apply what I needed. I could control loud voice outbursts, soften the entry to/exit from clipped parts in the way ClipOnly2 does, and I could even throw in a very subtle slew-clip to just faintly soften treble information, so my sibilance off my Roswell Colares would never be annoying (it's pretty good already, the mic tries to imitate an ELA M 251)Not only did this work, it turned out that allowing multiple stages of exactly this gave me ADClip8! So everybody wins.And of course I'm immediately letting people have the tool I made for me. If you experience normal clippers such as ClipOnly taking effect 6dB down, this is the one to use. Or, if for instance you're hammering a peak limiter on your mix buss at way over +6dB to the buss, then you can try this to precondition what the limiter sees, and you might get a pleasing result.But really, this just exists as a tool for me, or anyone who's using Final Cut and would like to drag a simple Airwindows clipper onto a (video) clip in the timeline, and have it make peaks more elegant, with no red clip lights happening and no abrasive highs when things get loud. Using this, I don't have to be that careful of my dynamic levels recording. If I suddenly yelp or talk loudly, clipping the camera's recording, this plugin will straighten it out and smooth off the corners just a bit so the loud bit isn't too objectionable.Since there's next to nothing to say about this plugin, nothing to do (no controls) and nothing to show (in a DAW, the interface is a blank rectangle or an empty line), the video's full of everything else I do to get the audio for my plugin-posting videos. If you'd like to see my setup, have at it. Otherwise, totally ignore it and just use the plugin :)"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class FinalClip final : public Effect<T>
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
    FinalClip()
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
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (inputSampleL - lastSampleL > 0.618033988749894) {
                inputSampleL = lastSampleL + 0.618033988749894;
            }
            if (inputSampleL - lastSampleL < -0.618033988749894) {
                inputSampleL = lastSampleL - 0.618033988749894;
            }
            // same as slew clippage
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 1.0 + (inputSampleL * 0.381966011250105);
                }
                else {
                    lastSampleL = 0.618033988749894 + (lastSampleL * 0.618033988749894);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 1.618033988749894) {
                wasPosClipL = true;
                inputSampleL = 1.0 + (lastSampleL * 0.381966011250105);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -1.0 + (inputSampleL * 0.381966011250105);
                }
                else {
                    lastSampleL = -0.618033988749894 + (lastSampleL * 0.618033988749894);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -1.618033988749894) {
                wasNegClipL = true;
                inputSampleL = -1.0 + (lastSampleL * 0.381966011250105);
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
            if (inputSampleR - lastSampleR > 0.618033988749894) {
                inputSampleR = lastSampleR + 0.618033988749894;
            }
            if (inputSampleR - lastSampleR < -0.618033988749894) {
                inputSampleR = lastSampleR - 0.618033988749894;
            }
            // same as slew clippage
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 1.0 + (inputSampleR * 0.381966011250105);
                }
                else {
                    lastSampleR = 0.618033988749894 + (lastSampleR * 0.618033988749894);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 1.618033988749894) {
                wasPosClipR = true;
                inputSampleR = 1.0 + (lastSampleR * 0.381966011250105);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -1.0 + (inputSampleR * 0.381966011250105);
                }
                else {
                    lastSampleR = -0.618033988749894 + (lastSampleR * 0.618033988749894);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -1.618033988749894) {
                wasNegClipR = true;
                inputSampleR = -1.0 + (lastSampleR * 0.381966011250105);
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
} // namespace airwindohhs::finalclip
