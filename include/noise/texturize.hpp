#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::texturize {

constexpr std::string_view k_name{ "Texturize" };
constexpr std::string_view k_short_description{
    "TexturizeMS is a hidden-noise plugin for adding mid-side sonic texture to things."
};
constexpr std::string_view k_long_description{
    "It all started with snake oil… or should I say secret snake oil?There’s talk lately of a plugin which has gotten a rather critical reception: a plugin said to put subliminal noise into the sound, to produce near-magical enhancements of tone and all good things. But you can’t hear it directly… you gotta vibe it, listen and embrace the magic, and then you’ll believe…People were, shall we say, critical of this approach :)Since I’m free to code what I like, thanks to Patreon and all (and thank you, all who’re pitching in there) I took an interest, and now you have Texturize. It is NOT literally this other plugin, or their patents, or the specific method by which they make the magic concealed noise that makes everything better.But what it IS, is a riff on several previous plugins I’ve had for years and years, to produce a very similar function… but THIS one, you can tweak and you can also crank it up and listen to only the noise to hear what it’s like. Ruin the magic… but learn how the trick is worked. And it turns out it is really not snake oil at all… it’s just another thing to do with audio, and it does seem to work, and everyone making a plugin of this nature will have their own ‘take’ on it: if you like mine maybe you’ll try out the other folks’ plugin with more of that open mind, having proved that the concept is sound. Or just use mine, which is open source and free :)You get three controls. Bright makes the effect key off high frequencies more aggressively, to the point of hyping up energy, and Punchy varies between a softer, fuller sound and a real spiky choppy effect. And then there’s Dry/Wet, the heart of the plugin, where settings of 0.5 or less are probably going to keep the noise entirely inaudible, as it’s meant to be. Not heard… but felt. And the sonic transformation’s really interesting, all the more if it’s not obvious. It reminds me of how tape flutter can bring texture to pure tones, chorusing against nearby reflections for a fatter sound, but here it’s just noise… but noise doctored to act like the music and hide behind it.And then of course you can crank it up until you plainly hear hints of the noise… or slam it until you only hear the noise. But to actually use the plugin properly, keep things at 0.5 and don’t push Bright or Punchy or especially Dry/Wet to where it’s gone too far: tweak it on that subliminal level, turning Wet down if you need to, and see what you get.I might well start using it. It really does seem to work. Go ahead and fool with it and strip all its secrets… and see whether you believe, too :)This is further development on my earlier plugin, Texturize. It existed to blend sculpted noise with the direct signal in a way that can enhance the qualities of your audio… depending on what that was, and whether you liked the results. It 'does something', that's for sure, but what? My code for doing it already existed, but there was a lot of buzz over another plugin which did a variation of this effect with a great deal of mystery and hype. I thought it was useful to do an open source version where you could clearly hear what the thing did, and that's Texturize.But what if a simple stereo effect isn't enough?TexturizeMS breaks the effect into mid/side processing, and then lets you set them independently. That means you can add a bit of extra zip in the highs to just the side, while keeping the center cleaner. If you have synthetic elements that show obvious noise using Texturize, and they're centered, you can tune just the middle. I think it's a major upgrade in terms of actually using the effect usefully.It's still Texturize. It's still a sort of funky snake oil (or maybe truffle oil?) It's a flavor and is absolutely doing a thing, but you don't have to consider it useful. But if you liked the flavor and atmosphere of this one, I think it's your lucky day. Hope you like it!"
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class Texturize final : public Effect<T>
{
    bool polarityL;
    double lastSampleL;
    double iirSampleL;
    double noiseAL;
    double noiseBL;
    double noiseCL;
    bool flipL;
    bool polarityR;
    double lastSampleR;
    double iirSampleR;
    double noiseAR;
    double noiseBR;
    double noiseCR;
    bool flipR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Texturize()
    {
        polarityL = false;
        lastSampleL = 0.0;
        iirSampleL = 0.0;
        noiseAL = 0.0;
        noiseBL = 0.0;
        noiseCL = 0.0;
        flipL = true;
        polarityR = false;
        lastSampleR = 0.0;
        iirSampleR = 0.0;
        noiseAR = 0.0;
        noiseBR = 0.0;
        noiseCR = 0.0;
        flipR = true;
        A = 0.5;
        B = 0.5;
        C = 0.5;
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
        kParampolarityL = 0,
        kParamlastSampleL = 1,
        kParamiirSampleL = 2,
        kNumParameters = 3

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: polarityL = value; break;
            case kParamlastSampleL: lastSampleL = value; break;
            case kParamiirSampleL: iirSampleL = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return polarityL;
            case kParamlastSampleL: return lastSampleL;
            case kParamiirSampleL: return iirSampleL;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return false;
            case kParamlastSampleL: return 0.0;
            case kParamiirSampleL: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return "bright";
            case kParamlastSampleL: return "punchy";
            case kParamiirSampleL: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return "Bright";
            case kParamlastSampleL: return "Punchy";
            case kParamiirSampleL: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return std::to_string(A);
            case kParamlastSampleL: return std::to_string(B);
            case kParamiirSampleL: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return "";
            case kParamlastSampleL: return "";
            case kParamiirSampleL: return "";
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
        double slewAmount = ((pow(A, 2.0) * 4.0) + 0.71) / overallscale;
        double dynAmount = pow(B, 2.0);
        double wet = pow(C, 5);
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
            if (inputSampleL < 0) {
                if (polarityL == true) {
                    if (noiseAL < 0) {
                        flipL = true;
                    }
                    else {
                        flipL = false;
                    }
                }
                polarityL = false;
            }
            else {
                polarityL = true;
            }
            if (inputSampleR < 0) {
                if (polarityR == true) {
                    if (noiseAR < 0) {
                        flipR = true;
                    }
                    else {
                        flipR = false;
                    }
                }
                polarityR = false;
            }
            else {
                polarityR = true;
            }
            if (flipL) {
                noiseAL += (double(fpdL) / UINT32_MAX);
            }
            else {
                noiseAL -= (double(fpdL) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipL = !flipL;
            if (flipR) {
                noiseAR += (double(fpdR) / UINT32_MAX);
            }
            else {
                noiseAR -= (double(fpdR) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipR = !flipR;
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
            if (dynAmount < 0.4999) {
                inputSampleL = (inputSampleL * dynAmount * 2.0) + (sin(inputSampleL) * (1.0 - (dynAmount * 2.0)));
                inputSampleR = (inputSampleR * dynAmount * 2.0) + (sin(inputSampleR) * (1.0 - (dynAmount * 2.0)));
            }
            if (dynAmount > 0.5001) {
                inputSampleL = (asin(inputSampleL) * ((dynAmount * 2.0) - 1.0)) + (inputSampleL * (1.0 - ((dynAmount * 2.0) - 1.0)));
                inputSampleR = (asin(inputSampleR) * ((dynAmount * 2.0) - 1.0)) + (inputSampleR * (1.0 - ((dynAmount * 2.0) - 1.0)));
            }
            // doing this in two steps means I get to not run an extra sin/asin function per sample
            noiseBL = sin(noiseAL * (0.2 - (dynAmount * 0.125)) * fabs(inputSampleL));
            noiseBR = sin(noiseAR * (0.2 - (dynAmount * 0.125)) * fabs(inputSampleR));
            double slewL = fabs(inputSampleL - lastSampleL) * slewAmount;
            lastSampleL = inputSampleL * (0.86 - (dynAmount * 0.125));
            if (slewL > 1.0) {
                slewL = 1.0;
            }
            double iirIntensityL = slewL;
            iirIntensityL *= 2.472;
            iirIntensityL *= iirIntensityL;
            if (iirIntensityL > 1.0) {
                iirIntensityL = 1.0;
            }
            double slewR = fabs(inputSampleR - lastSampleR) * slewAmount;
            lastSampleR = inputSampleR * (0.86 - (dynAmount * 0.125));
            if (slewR > 1.0) {
                slewR = 1.0;
            }
            double iirIntensityR = slewR;
            iirIntensityR *= 2.472;
            iirIntensityR *= iirIntensityR;
            if (iirIntensityR > 1.0) {
                iirIntensityR = 1.0;
            }
            iirSampleL = (iirSampleL * (1.0 - iirIntensityL)) + (noiseBL * iirIntensityL);
            noiseBL = iirSampleL;
            noiseBL = (noiseBL * slewL) + (noiseCL * (1.0 - slewL));
            noiseCL = noiseBL;
            iirSampleR = (iirSampleR * (1.0 - iirIntensityR)) + (noiseBR * iirIntensityR);
            noiseBR = iirSampleR;
            noiseBR = (noiseBR * slewR) + (noiseCR * (1.0 - slewR));
            noiseCR = noiseBR;
            inputSampleL = (noiseCL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (noiseCR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::texturize
