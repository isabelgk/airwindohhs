#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ulawencode {

constexpr std::string_view k_name{ "uLawEncode" };
constexpr std::string_view k_short_description{
    "uLawEncode is a Console-like encode/decode pair, but much more extreme."
};
constexpr std::string_view k_long_description{
    "Sometimes you do a plugin that’s not all that sensible.Those can be special, though, because those are the ones people can’t get anywhere else.There’s a process called uLaw compansion. It’s not really compression and expansion, though: it’s like a kind of distortion that can be reversed. It’s used for telephone transmission, and showed up in some of the earliest digital audio processors: back in the day, you had so few bits that you had to make them count.uLaw comes in two parts. The encoding applies a hideous distortion, making all the quietest sounds unreasonably loud and squishing the dynamic range up into the extreme near-clipping zone. The decoding neatly inverts this process and puts it all back. uLaw (the Airwindows plugin) does the high-resolution calculation of this process, so you can get exactly the compansion to a high degree of accuracy.What you do then, is you put a bitcrusher like DeRez in between the two uLaw plugins, and it suddenly sounds a lot cleaner with less roaring bit noise, because you’ve remapped where the quantization points are. It’s in the video, and easy to do with DeRez.And then… you can do all manner of other strange things, because you can put any plugin between uLawEncode and uLawDecode. Not just a bitcrusher. But, anything you put there is going to turn into a monster in a rather pronounced way. For instance, if you put an EQ in there, it will go crazy and any changes you make become loud, distorted parodies of what you meant. If you put a delay or ambience, it’ll get warped very harshly. If you put pitch shifts, flanges, who-knows-what in there, you get shockingly horrible versions back out.So, you can stick to the bitcrushers (mine or any other one you like: it’s a discrete pair for sandwiching any other plugin or plugins!) or you can treat it as a terrifying new building block that wasn’t directly available (far as I know) before. The plugins have gain and wet/dry controls which normally ought to be all set at 1.0, where they default. I can’t tell you how to use these controls to tame the behavior because it’ll vary with whichever plugin you try, and the controls don’t really belong and won’t necessarily give sensible results. You’re on your own.Also, you can’t frame a mix using uLaw the way you could using Console: the effect is far too ugly and intense. But there’s one common point: like PurestConsole, if you have just one track playing, it ought to come through pristine and perfect, no matter which single track it is. Then if any other track so much as whispers its presence, you’ll have heinous distortion of an unusual kind. I would say ‘Autechre’ but they’ve probably already done this before in Max/MSP. :)I make these things because I have a Patreon that frees me from starving, and also frees me from having to release popular and sensible plugins that are nice and approachable. Mind you, some of my plugins can be approachable, and I like making those too… I have progress on the mono-bass version of DubSub, and I’m happy with BassKit, the streamlined and polished mono-bass-enhancer that lets you beef up tapey or bassbin-y fatness in mono or add literal subsonic thunderousness with the octave divider… but thanks to the freedom of the Patreon I can make things that are truly themselves, with no nod to popularity whatsoever.uLaw is like that. You probably don’t have it because it’s ugly and strange and needs to be designed into a more sensible configuration, typically with a bitcrusher and nothing else, because nearly anything else you do with it produces horrible noises.I’m genuinely happy to bring you this audio chainsaw. You never can tell what will be handy, either as the pair, or individually: logarithmic processing may well find creative uses. Have fun!"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class uLawEncode final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    uLawEncode()
    {
        A = 1.0;
        B = 1.0;
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
            return 1.0;
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
            return "gain";
            break;
        kParamB:
            return "dry/wet";
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
            return "Gain";
            break;
        kParamB:
            return "Dry/Wet";
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
        overallscale *= getSampleRate();
        double gain = A;
        double wet = B;
        // removed extra dry variable
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            noisesourceL = noisesourceL % 1700021;
            noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
                inputSampleL -= applyresidue;
            }
            noisesourceR = noisesourceR % 1700021;
            noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
                inputSampleR -= applyresidue;
            }
            // for live air, we always apply the dither noise. Then, if our result is
            // effectively digital black, we'll subtract it auLawEncode. We want a 'air' hiss
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleL < 0) {
                inputSampleL = -log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleR > 0) {
                inputSampleR = log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
            if (inputSampleR < 0) {
                inputSampleR = -log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs::ulawencode
