#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapedither {

constexpr std::string_view k_name{ "TapeDither" };
constexpr std::string_view k_short_description{
    "TapeDither is TPDF dither with noise like reel-to-reel tape."
};
constexpr std::string_view k_long_description{
    "I thought I had it all worked out. Put out a proper, well-coded TPDF dither, the highpassed variation on it I’ve called PaulDither, and move on to the fancy boutique stuff.But there was this one experiment I had in mind. If you could do the highpass by delaying and then subtracting the random noise (and it gained you a bit of efficiency in the bargain), why not delay it more? It’d create comb filtering, a cancellation node. Why not keep delaying until the cancellation node dropped right down to around 1.5K?Surely this would give me a nice cancellation notch right where the ear is most sensitive, and heightened clarity. What could go wrong?Nope! I made a prototype, called it ‘NodeDither’, began experimenting, and immediately found that I’d made… a flanger! The long delay settings were useless. It made obvious overtones just out of the dither noise, a blatant tone color cast that wouldn’t produce the desired effect. It didn’t even produce an obvious notch in the response where I wanted it. The only thing it did do, was continue to function as a working TPDF dither no matter what the setting was (more on this later).But, something else turned up in the experiments, and that’s what brought you TapeDither.If you use one sample of delay and inverting the noise, you get PaulDither: simple one-pole highpass. If you use two samples of delay, you get another sort of texture: kind of silky, but still digitally bright and intrusive. Using lots of delay, such as ten samples, starts to sound like the flanger, undesirable.But, there’s something interesting about powers-of-two delay times. One, two, four, eight and so on, these delay times are slightly less ‘colored’ in tone than the others. I think it has to do with interactions with the sample rate: they seem to line up more neatly, making it slightly more easy not to hear the pitch of the ‘flangey’ quality.And four samples of delay (and then inverting the noise) produces something rather special: a noise profile that closely resembles what you hear off reel-to-reel tape.I can’t specify particular brands because (a) I hate when people do that to brand names not their own and (b) it’s a technical discovery, not some complicated way of forcing digital audio to mimic a particular brand. It’s no specific tape stock or tape machine. But what it is, is a voicing for TPDF dither that rolls off in an obvious way, around where tape noise rolls off. There’s another little bump past that, which many people won’t be able to hear, and then it begins to roll off again as it reaches the Nyquist frequency beyond where digital audio can’t go. Compare that to any normal flat, TPDF, or highpassed dither. Those keep putting out noise energy right up to the frequency limit.TapeDither is every bit a TPDF dither, technically correct and flawless as far as dither goes. But it also is a highpassed dither with a softer tonal voicing that resembles good tape machines, and that doesn’t affect the dither performance at all. It doesn’t attenuate the audio content at all. Only the background dither noise is turned into what you’d get off a tape deck, all while the audio is protected from truncation and digital artifacts.I would use this anywhere I was sending processed stems or tracks outboard to a multichannel DAC, for mixing in the analog domain or summing with an outboard summing box. Without hesitation. I’d use fancier boutique dithers on the 2-buss, though this will work there too… but if I was doing an analog mix from a multichannel converter, there is nothing I’d rather use than TapeDither. (maybe Naturalize. Maybe.)That’s because it’s one thing to apply an interesting dither (like Spatialize, for instance, or some noise shaper) on the 2-buss and get a unique texture or presentation on the audio. But if you’re stacking up lots of tracks and all of them get the same treatment, it’s vital to use something that won’t develop an exaggerated tone signature when multiplied that way. For dither, TPDF gives you that behavior, and TapeDither is the TPDF that most closely mimics the reality of analog mixing back in the days of big reel-to-reel machines. The tone of its noise floor is already more pillowy, mellow and relaxing than most TPDF or highpassed TPDF, and then when you stack up lots of tracks of it, the texture’s better still.I’m still excited to bring out Naturalize, but I’m just as excited about what TapeDither means. It makes me want to build analog consoles and mix into them, just so I can put TapeDither on any output that’s not playing raw 24 bit data exactly as captured. Anything with processing or gain on it would get a dithered output, and it would be a matter of—faders up—hello, Seventies :)Well, I like audio from the Seventies. I suppose I’d better work out how to do some of the sonic quirks of Doubly while I’m at it! Not that a rock band would use Doubly…"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class TapeDither final : public Effect<T>
{
    double previousDither1L;
    double previousDither2L;
    double previousDither3L;
    double previousDither4L;
    double previousDither1R;
    double previousDither2R;
    double previousDither3R;
    double previousDither4R;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    TapeDither()
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
        previousDither1L = 0.0;
        previousDither2L = 0.0;
        previousDither3L = 0.0;
        previousDither4L = 0.0;
        previousDither1R = 0.0;
        previousDither2R = 0.0;
        previousDither3R = 0.0;
        previousDither4R = 0.0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "quant"; break;
            case kParamB: return "derez"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Quant"; break;
            case kParamB: return "DeRez"; break;

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
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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

        double currentDitherL;
        double currentDitherR;
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
            currentDitherL = (double(fpdL) / UINT32_MAX);
            currentDitherR = (double(fpdR) / UINT32_MAX);
            inputSampleL += currentDitherL;
            inputSampleR += currentDitherR;
            inputSampleL -= previousDither4L;
            inputSampleR -= previousDither4R;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            previousDither4L = previousDither3L;
            previousDither3L = previousDither2L;
            previousDither2L = previousDither1L;
            previousDither1L = currentDitherL;
            previousDither4R = previousDither3R;
            previousDither3R = previousDither2R;
            previousDither2R = previousDither1R;
            previousDither1R = currentDitherR;
            inputSampleL /= outScale;
            inputSampleR /= outScale;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::tapedither
