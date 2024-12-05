#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubly2 {

constexpr std::string_view k_name{ "Dubly2" };
constexpr std::string_view k_short_description{
    "Dubly2 is a key part of seventies sonics!"
};
constexpr std::string_view k_long_description{
    "As we race towards ToTape7, more of the component parts emerge as their own plugins! As long as I'm reinventing all the things that go into ToTape7 (I've seen people get excited about the previous plugin, ToTape6, and felt I could do a heck of a lot better now) I like to let people have the new stuff immediately, and I like to let people use the component parts of things. So, first Flutter2 gets lifted to a new level, and now, Dubly2 is yours!And where Flutter2 took the basic algorithm to a new level, Dubly2 hasn't changed the basic sound of the plugin very much (it DOES change, especially if you're slamming it with volume). Instead… it's the control you get… which goes from a 2022-Chris 'one knob simplification' design, to six knobs and a dry/wet. Normally, nearly all of those would be preset, or perhaps would be trimpots inside the noise reduction box. Dubly's sound dates back to the late (or even mid) 1960s, and of course the 1970s, and the noise reduction machines were sold to big studios.So, unlike modern noise reductions where the system's very specific and detailed and is a black box you can't mess with… Dubly is kicking it old school! You can leave the encode and decode controls set to 0.5 for a 'normal' sound, or you can mess with them. The algorithms are very simple for better tone quality, but the end result is very Dubly!Encode has an encode level, and a frequency control. This specifies the 'brightness band' you'll be using and compressing 2:1 to get the brightness boost that gets added to the audio. It's pretty straightforward.Decode has a decode level, and a frequency control. This (and it's the same on the old units, believe it or not) does exactly the same encode, the same way… except you add it phase-inverted. That's it: that's Dubly. You can look up on old schematics, exactly where the output of the elaborate filtering and compression stuff is routed to one spot on the circuit board for 'encode' and another spot for 'decode'.This does have some implications. The highs get squished twice. In real units, care is taken to not have the compression time constants 'breathe' weirdly: for Dubly, we just use uLaw encoding on a brightness-filtered sound and it works great. It produces a similar effect when you add small amounts to the source audio.If you're going to mess with the trimpots (now ordinary sliders for anybody to play with), you can get controllable amounts of the 'two noise reduction units in encode, in series' simply by turning up the Encode amount. The existing processing becomes a special 'gloss and sparkle' control. You can tweak the Decode Amount similarly. The frequencies can be nudged up or down together, or you can make them be slightly different frequencies for unusual midrange effects that aren't normal: letting more mids through on Encode will cause them to be more compressed and intense, or you could go the other way and slim them down a little for an 'expanded' feel. It's all rather subtle but it's also tonal characteristics that really can't be had any other way… and if I'm not mistaken, these little adjustments can 'dial in' on the sound of many classic, beloved records.I look forward to playing with all this in the full ToTape7, with flutter and tape bias modeling, but for now you can fully play with and get used to these qualities in advance! Have fun with Dubly2 :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Dubly2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    double iirEncL;
    double iirDecL;
    double iirEncR;
    double iirDecR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Dubly2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        D = 0.5;
        E = 0.5;
        F = 0.0;
        G = 1.0;
        iirEncL = 0.0;
        iirDecL = 0.0;
        iirEncR = 0.0;
        iirDecR = 0.0;
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
        kParamF = 5,
        kParamG = 6,
        kNumParameters = 7
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.0; break;
            case kParamG: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "encamnt"; break;
            case kParamB: return "encfreq"; break;
            case kParamC: return "tapedrv"; break;
            case kParamD: return "decamnt"; break;
            case kParamE: return "decfreq"; break;
            case kParamF: return "out pad"; break;
            case kParamG: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "EncAmnt"; break;
            case kParamB: return "EncFreq"; break;
            case kParamC: return "TapeDrv"; break;
            case kParamD: return "DecAmnt"; break;
            case kParamE: return "DecFreq"; break;
            case kParamF: return "Out Pad"; break;
            case kParamG: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;

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
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;

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
        double dublyAmount = pow(A, 3) * 0.25;
        double iirEncFreq = B / overallscale;
        double tapeDrv = (C * 2.0) + 1.0;
        double outlyAmount = pow(D, 3) * 0.25;
        double iirDecFreq = E / overallscale;
        double outPad = (F * 2.0) + 1.0;
        if (tapeDrv > 1.0) {
            outPad += 0.005;
        }
        double wet = G;
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
            iirEncL = (iirEncL * (1.0 - iirEncFreq)) + (inputSampleL * iirEncFreq);
            iirEncR = (iirEncR * (1.0 - iirEncFreq)) + (inputSampleR * iirEncFreq);
            double doubly = inputSampleL - iirEncL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL += doubly * dublyAmount;
            doubly = inputSampleR - iirEncR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR += doubly * dublyAmount;
            // end Dubly encode
            if (tapeDrv > 1.0) {
                inputSampleL *= tapeDrv;
                inputSampleR *= tapeDrv;
            }
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            if (outPad > 1.0) {
                inputSampleL /= outPad;
                inputSampleR /= outPad;
            }
            iirDecL = (iirDecL * (1.0 - iirDecFreq)) + (inputSampleL * iirDecFreq);
            iirDecR = (iirDecR * (1.0 - iirDecFreq)) + (inputSampleR * iirDecFreq);
            doubly = inputSampleL - iirDecL;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleL -= doubly * outlyAmount;
            doubly = inputSampleR - iirDecR;
            if (doubly > 1.0) {
                doubly = 1.0;
            }
            if (doubly < -1.0) {
                doubly = -1.0;
            }
            if (doubly > 0) {
                doubly = log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            if (doubly < 0) {
                doubly = -log(1.0 + (255 * fabs(doubly))) / 2.40823996531;
            }
            inputSampleR -= doubly * outlyAmount;
            // end Dubly decode
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::dubly2
