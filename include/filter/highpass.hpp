#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::highpass {

constexpr std::string_view k_name{ "Highpass" };
constexpr std::string_view k_short_description{
    "Highpass is a time warp, for retro midrangey sounds."
};
constexpr std::string_view k_long_description{
    "These (Lowpass and Highpass) are mixing EQs, not mastering ones (though I’m not the boss of you). They’re complementary: the one is the inverse of the other. However, because of their peculiarities that makes them behave quite differently. What they have in common is they’re interleaved IIR filters, something people don’t normally do. The experiment here has to do with my discovery that digital audio only exists in sets of samples (never just as the isolated sample: the waveform isn’t there, the sample value is only a signpost that the audio is to weave its way around)They’ve also got a very unusual parameter, soft/hard or loose/tight, which controls how the IIR filters are fed audio. When you offset it, you get a situation where the cutoff is higher at louder volumes, or at quieter volumes. This is on a sample-by-sample basis so it’s a tone-character modification, subtle but interesting. Loose/tight is just the best way I could describe what’s happening there.Lowpass gives you a treble rolloff (some have joked that I make dozens of treble rolloffs! Yes, but they all sound different) and what’s immediately obvious is, the stuff right up by Nyquist on the threshold of hearing is not rolled off with the rest of the treble. Also, if you only want to cut extreme treble, you can do it with just adjusting the soft/hard control away from the center position. At deeper cut settings, the soft/hard control gives you two different textures (both of which keep a hint of ‘air’ right up top). The dry/wet control allows you to blend your result. Lowpass gives you big sounds with various colorations and a sparkly gloss that comes from your underlying sound: it’s a big-ifying filter that might suit huge synth pads or orchestral tracks.Highpass, the inverse of this, gives totally different impressions. The same filter-offset behavior turns into ‘loose/tight’ and the extreme treble gets stepped on, rather than retained. This makes Highpass take on ‘classic’ tonalities, particularly with the offset on ‘loose’, which gives a tubey and softened texture. If you run it full-wet, you’ll get a radical ‘analogification’, wiping out all extreme lows and the highest highs, and sounding like some small vintage radio at high filter settings. It’s a small-ifying filter that’s also a time warp (with offset on ‘tight’, you have a transistor radio instead, still retro-sounding!) and all you have to do is dial in your boost area and then balance it with dry/wet to get intense texture shaping that normal EQs can’t come close to delivering.Again, these are not mastering EQs unless you face really unusual mastering requirements. They’re mixing tools, and they really do act like different animals so they’re each contained in their own plug. They’ve been around for ages but the revision to VST form has brought them a new level of tonal sophistication plus the very useful dry/wet controls that take them out of ‘experiment-land’ (they have always been building blocks for plugins such as Guitar Conditioner) and makes them stand alone as useful mix tools."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Highpass final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    bool fpFlip;

  public:
    Highpass()
    {
        A = 0.0;
        B = 0.5;
        C = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        fpFlip = true;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "highpass"; break;
            case kParamB: return "loose/tight"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Highpass"; break;
            case kParamB: return "Loose/Tight"; break;
            case kParamC: return "Dry/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return " "; break;
            case kParamC: return " "; break;

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
        double iirAmount = pow(A, 3) / overallscale;
        double tight = (B * 2.0) - 1.0;
        double wet = C;
        // removed extra dry variable
        double offset;
        double inputSampleL;
        double inputSampleR;
        double outputSampleL;
        double outputSampleR;
        iirAmount += (iirAmount * tight * tight);
        if (tight > 0) {
            tight /= 1.5;
        }
        else {
            tight /= 3.0;
        }
        // we are setting it up so that to either extreme we can get an audible sound,
        // but sort of scaled so small adjustments don't shift the cutoff frequency yet.
        if (iirAmount <= 0.0) {
            iirAmount = 0.0;
        }
        if (iirAmount > 1.0) {
            iirAmount = 1.0;
        }
        // handle the change in cutoff frequency
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            outputSampleL = inputSampleL;
            outputSampleR = inputSampleR;
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleL) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleL)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip)
            {
                iirSampleAL = (iirSampleAL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                outputSampleL = outputSampleL - iirSampleAL;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                outputSampleL = outputSampleL - iirSampleBL;
            }
            if (tight > 0) {
                offset = (1 - tight) + (fabs(inputSampleR) * tight);
            }
            else {
                offset = (1 + tight) + ((1 - fabs(inputSampleR)) * tight);
            }
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (fpFlip)
            {
                iirSampleAR = (iirSampleAR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                outputSampleR = outputSampleR - iirSampleAR;
            }
            else
            {
                iirSampleBR = (iirSampleBR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                outputSampleR = outputSampleR - iirSampleBR;
            }
            fpFlip = !fpFlip;
            if (wet < 1.0) {
                outputSampleL = (outputSampleL * wet) + (inputSampleL * (1.0 - wet));
            }
            if (wet < 1.0) {
                outputSampleR = (outputSampleR * wet) + (inputSampleR * (1.0 - wet));
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
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::highpass
