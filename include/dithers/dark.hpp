#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dark {

constexpr std::string_view k_name{ "Dark" };
constexpr std::string_view k_short_description{
    "Dark is a wordlength reducer that gives your music a blacker backdrop."
};
constexpr std::string_view k_long_description{
    "Some weeks are MY kind of fun…This won’t make big changes in your audio. In fact if you think you can reliably hear this on its HD setting, I think you’re mistaken. And yet, this might be the funnest thing I’ve done all year.Why? Because I’m back to the dithers again. I figured out a way (or two) to go EVEN FARTHER in the direction I’d chosen. And it worked: it worked super well, and you can have it. Introducing Airwindows Dark.How does it work? It’s very simple, really. Much like Not Just Another Dither (NJAD, my previous best) it analyzes the results of the audio, depending on whether the dither rounds up or down. With correct TPDF dither, it’s a factor of randomness, a noise that breaks up patterns in the output. With NJAD, it runs a Benford Realness calculation and uses that (for a more natural-sounding audio output). But Dark?It simply works out the average trajectory of where the audio’s going. It’s following the lower frequencies, suppressing the highs. And then it makes its choice based solely on whatever is going to further this trajectory… and keep the output as smooth as possible. It is ‘dithering’ with intent, doing whatever it has to in order to get a darker, softer output. All done by truncation alone.The result is delightful, if you are into analog sonics and the absence of bright digital artifacts and hisses. It is NOT obvious, unless 16 bit artifacts are already obvious to you, and at HD (24 bit) it is purely a matter of thoroughness and making correct choices and you shouldn’t be hearing a difference. You damn sure won’t be able to blind test a difference (that requires much more obvious stuff happening).But, but, but! If your experience with audio (and probably loud listening levels, and REALLY good monitoring, and amazing source files) involves sinking into a lush bed of analog-rich sonics, where you quickly notice subtle shifts in sonic attitude and then take much longer to get used to them and form your judgements… there’s nothing at all like this.It can only wordlength reduce, so especially at 24 bit it shouldn’t be able to ever hurt bright sounds that are supposed to be there. It’s only dithering (in a novel way). But it’s doing its thing in a way that’s completely outside of anything you can do with filtering or normal processing. If you need depth and space, if you need rich black silence behind your mix, this beats NJAD… soundly.I hope you like it. The demonstration at 8 bit wordlength in the video ought to show you what to expect. Dark is yours to use (in fact, you can have the source code under the MIT license!). For now, if you are using Monitoring you’ll need to switch it off to use Dark, as Monitoring defaults to a 24 bit wordlength reduction using NJAD and I’m not prepared to simply update it and have it default to Dark for all things.Though I’m tempted. ;) (in fact, this is what Monitoring2 became)And in turn, within two weeks, Dark became Dark Redux: same Dark, exactly as before, BUT now it has one added control. DeRez! Just like in the DeRez plugin, it zooms seamlessly down to 1 (or 0) bits. It’s continuous, not discrete: you can do one and a half bits or whatever, play it by ear. Except that unlike the DeRez plugin, it’s still Dark… so you can hear more clearly what the new wordlength reducer is doing.It defaults to 0, which is exactly as it was before. Both the 24 and 16 bit settings derez, which means you can take it to 0 bit (silence) with 16 bit, switch to 24 bit, and the result is you’re listening to 8 bit. (if you need really specific behavior, you can use BitShiftGain before and after the plugin to get exact bit values).This makes it possible to do old school sampling effects, dark bitcrushed sounds. It also demonstrates that the Dark algorithm is a bit special, because you have to really trash the wordlength to deteriorate the tone much. It’s more like a slightly noisier, slightly funkier, slightly gated grunge tone, even though the output is literally just bitcrushing. There isn’t even any smoothing applied to get the ‘dark’ tonality, it’s literally all a wordlength reducer, and with DeRez at 0 it’s exactly the original Dark plugin.If there’s a problem or if you need to not see a control marked ‘DeRez’, my updates-in-place always leave the original file there, in this case renamed to DarkOriginal. They have the same ID and DAWs ought to be able to handle swapping the new one in, even on existing projects that use Dark, it’s just to make sure people can get back the original release if they ever need to."
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class Dark final : public Effect<T>
{
    float lastSampleL[100];
    float lastSampleR[100];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Dark()
    {
        A = 1.0;
        B = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        for (int count = 0; count < 99; count++) {
            lastSampleL[count] = 0;
            lastSampleR[count] = 0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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
        int depth = (int)(17.0 * overallscale);
        if (depth < 3) {
            depth = 3;
        }
        if (depth > 98) {
            depth = 98;
        }
        int processing = (VstInt32)(A * 1.999);
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
        float derez = B;
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
            float expectedSlew = 0;
            for (int x = 0; x < depth; x++) {
                expectedSlew += (lastSampleL[x + 1] - lastSampleL[x]);
            }
            expectedSlew /= depth; // we have an average of all recent slews
            // we are doing that to voice the thing down into the upper mids a bit
            // it mustn't just soften the brightest treble, it must smooth high mids too
            float testA = fabs((lastSampleL[0] - quantA) - expectedSlew);
            float testB = fabs((lastSampleL[0] - quantB) - expectedSlew);
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
            expectedSlew = 0;
            for (int x = 0; x < depth; x++) {
                expectedSlew += (lastSampleR[x + 1] - lastSampleR[x]);
            }
            expectedSlew /= depth; // we have an average of all recent slews
            // we are doing that to voice the thing down into the upper mids a bit
            // it mustn't just soften the brightest treble, it must smooth high mids too
            testA = fabs((lastSampleR[0] - quantA) - expectedSlew);
            testB = fabs((lastSampleR[0] - quantB) - expectedSlew);
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
} // namespace airwindohhs::dark
