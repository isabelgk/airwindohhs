#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::spiral2 {

constexpr std::string_view k_name{ "Spiral2" };
constexpr std::string_view k_short_description{
    "Spiral2 is Spiral with controls including Presence. Also, a historical post presented in full as it is just after my Dad's death. Post follows:"
};
constexpr std::string_view k_long_description{
    "I will need to take a week to sort of meditate and settle my mind, but it was important to me to still be giving stuff to my fans and patrons etc. so I was looking around for something fairly easy to do. For instance, take Spiral and give it the ‘Density/Drive’ treatment, including things like pre and post gain and a highpass and a dry/wet. Busywork but nothing particularly innovative.I think Dad would have been proud that I stumbled across something that was kind of innovative anyhow. Now I have Spiral2. It has those controls (that people really wanted: and if you set them to unity/neutral, they bypass so you can have EXACTLY the same as Spiral itself, if you like) but something else happened…It occurred to me, if I was blending between dry signal and the sin() function saturated signal using the signal itself as the blend factor, I was just using a sample. Well, I also knew how to store a sample, and then the next time it’d be ‘lastSample’. And what would happen if I blended between dry and the saturated one… using the PREVIOUS sample?With low frequency stuff, pretty much nothing. But what if there was high frequency stuff? What’d happen then?Turns out, it’s a little like ‘Pyewacket’, my compressor that compresses into a ‘negative Density’ effect that lets peaks through. With that, the front of the wave is unusually pure and punchy, and there’s better articulation of sounds. With Spiral2, it lets onset transients through, especially if they’re happening suddenly out of existing silence (and bear in mind it can be only one sample of silence for it to work). It’s not a super obvious effect… but if you use Spiral to chop off peaks, and add this new effect (which I could only call Presence) then you can get quite the opposite effect: at full Presence, it sounds like everything’s being distorted but the meters show how onset transients are still getting through.If you set it halfway, it becomes very close to peaking at exactly 0 dB. If you set it to 0 you get the original Spiral (note that it still has the ‘continuing around the sin() curve’ effect so if you over-slam it, it’ll choke and go quieter).So, I wanted to do something nice for my peeps since everybody has been so kind and supportive. Turns out my muse thought that was the sweetest intention ever, and really came through for me. Enjoy Spiral2: you’ll find that Presence is quite a striking effect. I think in extreme cases it’s TOO much air, but that’s why it’s on a slider, which is really just a crossfader between the two ‘circuits’, normal and with the one sample delay on the ‘sense’ circuit.See ya soon, and I hope you like Spiral2. If you would have dropped $50 on this without a moment’s hesitation once you hear what it can do by trying it, please do that using the Patreon. I’m looking to keep expanding and be more ambitious, if that’s OK. It’s more fun being ambitious with a budget, and food and shelter and stuff :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Spiral2 final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double prevSampleL;
    uint32_t fpdL;
    uint32_t fpdR;
    double iirSampleAR;
    double iirSampleBR;
    double prevSampleR;
    bool flip;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Spiral2()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        prevSampleL = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        prevSampleR = 0.0;
        flip = true;
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
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
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
        kParamD:
            return D;
            break;
        kParamE:
            return E;
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
            return 0.5;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
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
            return "input";
            break;
        kParamB:
            return "highpass";
            break;
        kParamC:
            return "presence";
            break;
        kParamD:
            return "output";
            break;
        kParamE:
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
            return "Input";
            break;
        kParamB:
            return "Highpass";
            break;
        kParamC:
            return "Presence";
            break;
        kParamD:
            return "Output";
            break;
        kParamE:
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
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
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
        kParamD:
            return "";
            break;
        kParamE:
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
        double gain = pow(A * 2.0, 2.0);
        double iirAmount = pow(B, 3.0) / overallscale;
        double presence = C;
        double output = D;
        double wet = E;
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
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
                prevSampleL *= gain;
                prevSampleR *= gain;
            }
            if (flip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // highpass section
            double presenceSampleL = sin(inputSampleL * fabs(prevSampleL)) / ((prevSampleL == 0.0) ? 1 : fabs(prevSampleL));
            double presenceSampleR = sin(inputSampleR * fabs(prevSampleR)) / ((prevSampleR == 0.0) ? 1 : fabs(prevSampleR));
            // change from first Spiral: delay of one sample on the scaling factor.
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
                presenceSampleL *= output;
                presenceSampleR *= output;
            }
            if (presence > 0.0) {
                inputSampleL = (inputSampleL * (1.0 - presence)) + (presenceSampleL * presence);
                inputSampleR = (inputSampleR * (1.0 - presence)) + (presenceSampleR * presence);
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
            prevSampleL = drySampleL;
            prevSampleR = drySampleR;
            flip = !flip;
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
} // namespace airwindohhs::spiral2
