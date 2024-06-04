#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pockey {

constexpr std::string_view k_name{ "Pockey" };
constexpr std::string_view k_short_description{
    "Pockey2 is more efficient, more intense lo-fi hiphop in a plugin."
};
constexpr std::string_view k_long_description{
    "This plugin is designed to give you the most vibe possible out of a particular target: 12 bit uLaw-sampled lo-fi hiphop. It's based on earlier plugins like DeRez and BitGlitter, but is tailored to deliver just the right kind of warm fuzzy texture like an old sampler… or the Pocket Operator sampler that's a lot more accessible than retro 12-bit is.Pockey uses uLaw encoding to stretch 12 bit sampling to where it sounds pretty nearly CD quality, except there's a whole vibe and texture you get through having the soft, delicate sounds slightly low-bit and the louder sounds even more crunchy than that. The lowest setting on the resolution slider is exactly 12 bit, and everything higher than that gives you progressively more lo-fi in an 'analog bitcrush' mode, meaning that it will gradually build as you turn it up, rather than jump from step to step. That way you can fade smoothly from 4-bit to 12, if you like.The frequency crush effect is also special: it uses an edge-softening algorithm a little bit like PurestDrive, for the purpose of turning the harsh and grating frequency crush you'd normally get from a plugin, into something a little more resonant, more sonorous. It's still a sort of digital nasty, but steps way over in the direction of 'classic sampler digital nasty', again in a way that suits the more chill lo-fi genres. If you're looking to have it be as clean as possible, take your audio and explore in the very lowest settings of the frequency control for a spot that's minimally intrusive, or a digital-sampler overtone that works best with the sounds you're using. All the stuff near 0 ought to be well behaved. Then as you crank it up, you'll get rowdier digital artifacts, but always with that softening that helps lo-fi hiphop provide a soothing backdrop to life: which is partly due to the absence of super-extended frequencies grabbing your attention.It's both an art and a science, and Pockey is there to help you find the art through me digging into the science of how these things are done :)So this is pretty rapid iteration. I'd been struggling with getting Pockey to willingly do 12 bit: the algorithm didn't seem to like it, even crashed when I tried to go for 16 bit. Turns out it was counting bit by bit in order to do the analog bitcrushing thing. And there was another way to do it… but then other things had to change… and then other ideas started to happen…Suffice to say, it earned its new version number.Pockey2 takes the basic concept of Pockey and brings in an entirely fresh character, while running way more efficiently. Instead of analog-style floating point bitcrushing, it's integer… but you'll find the difference between 5037 'stairsteps' and 5038 'stairsteps' will amount to about the same thing, just way more easy on the CPU. Still uLaw, still a big sampler-esque texture, now letting you go from 4 bit all the way to 16 bit at no CPU penalty. There's not a lot of point running 16 bit here as it's still uLaw, but in case you're looking for just the lightest possible 'sampler' touch on your 24 bit DAW track, you've got it.Instead of analog-style floating point frequency crushing, that's integer too… which means the adjustability of the super-bright frequency crushing will be 'steppy', not allowing fine adjustments especially if the DAW's at 44.1k. But if you're running 96k, you get a much broader range of adjustment! And you get to frequency crush down way lower than before… and the tone! Another reason Pockey2 is a new version number is that, while the tone of the frequency crush on Pockey was interesting and vibey, the stuff you can do with the different-sounding Pockey2 is beyond belief. Experiment with the DeFreq control and see what you get. There's a little bit of the 'sweep the digital sampler cutoff point' still there, especially at deeper frequency crushes, but Pockey2 shines when finding just the right spot to entirely transform a sound into pure lo-fi hip hop classic sampler madness. Even more than the original Pockey, this one can completely transform a sound into retro digital.You can use both. If you like Pockey and find its CPU use too heavy, DeRez it more, it struggles to get to high bit depths. If you like Pockey2, it doesn't struggle with anything except sometimes with slowly sweeping a frequency crush way up high. I hope you like both Pockeys and find your best use for them :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Pockey final : public Effect<T>
{
    double lastSampleL;
    double heldSampleL;
    double lastSoftenL;
    double positionL;
    double lastSampleR;
    double heldSampleR;
    double lastSoftenR;
    double positionR;
    double freq;
    double freqA;
    double freqB;
    double rez;
    double rezA;
    double rezB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Pockey()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        lastSampleL = 0.0;
        heldSampleL = 0.0;
        lastSoftenL = 0.0;
        positionL = 0.0;
        lastSampleR = 0.0;
        heldSampleR = 0.0;
        lastSoftenR = 0.0;
        positionR = 0.0;
        freq = 0.5;
        freqA = 0.5;
        freqB = 0.5;
        rez = 0.5;
        rezA = 0.5;
        rezB = 0.5;
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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;
            case kParamB: return 0.0;
            case kParamC: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "defreq";
            case kParamB: return "derez";
            case kParamC: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "DeFreq";
            case kParamB: return "DeRez";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double freqMin = 0.08 / overallscale;
        freqA = freqB;
        freqB = (pow(1.0 - A, 3) * (0.618033988749894848204586 - freqMin)) + freqMin;
        // freq is always engaged at least a little
        rezA = rezB;
        rezB = pow(B * 0.618033988749894848204586, 3) + 0.000244140625;
        // rez is always at least 12 bit truncation
        double wet = C;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double freq = (freqA * temp) + (freqB * (1.0 - temp));
            double rez = (rezA * temp) + (rezB * (1.0 - temp));
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = log(1.0 + (255 * fabs(inputSampleL))) / log(255);
            }
            if (inputSampleL < 0) {
                inputSampleL = -log(1.0 + (255 * fabs(inputSampleL))) / log(255);
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleR > 0) {
                inputSampleR = log(1.0 + (255 * fabs(inputSampleR))) / log(255);
            }
            if (inputSampleR < 0) {
                inputSampleR = -log(1.0 + (255 * fabs(inputSampleR))) / log(255);
            }
            // end uLaw encode
            double offset = inputSampleL;
            if (inputSampleL > 0)
            {
                while (offset > 0) {
                    offset -= rez;
                }
                inputSampleL -= offset;
                // it's below 0 so subtracting adds the remainder
            }
            if (inputSampleL < 0)
            {
                while (offset < 0) {
                    offset += rez;
                }
                inputSampleL -= offset;
                // it's above 0 so subtracting subtracts the remainder
            }
            inputSampleL *= (1.0 - rez);
            offset = inputSampleR;
            if (inputSampleR > 0)
            {
                while (offset > 0) {
                    offset -= rez;
                }
                inputSampleR -= offset;
                // it's below 0 so subtracting adds the remainder
            }
            if (inputSampleR < 0)
            {
                while (offset < 0) {
                    offset += rez;
                }
                inputSampleR -= offset;
                // it's above 0 so subtracting subtracts the remainder
            }
            inputSampleR *= (1.0 - rez);
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = (pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleL < 0) {
                inputSampleL = -(pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleR > 0) {
                inputSampleR = (pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            if (inputSampleR < 0) {
                inputSampleR = -(pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            // end uLaw decode
            positionL += freq;
            double outputSample = heldSampleL;
            if (positionL > 1.0)
            {
                positionL -= 1.0;
                heldSampleL = (lastSampleL * positionL) + (inputSampleL * (1 - positionL));
                outputSample = (heldSampleL * (1 - positionL)) + (outputSample * positionL);
            }
            inputSampleL = outputSample;
            double slew = fabs(inputSampleL - lastSoftenL) * freq;
            if (slew > 0.5) {
                slew = 0.5;
            }
            inputSampleL = (inputSampleL * slew) + (lastSoftenL * (1.0 - slew));
            // conditional average: only if we actually have brightness
            lastSampleL = drySampleL;
            lastSoftenL = outputSample;
            positionR += freq;
            outputSample = heldSampleR;
            if (positionR > 1.0)
            {
                positionR -= 1.0;
                heldSampleR = (lastSampleR * positionR) + (inputSampleR * (1 - positionR));
                outputSample = (heldSampleR * (1 - positionR)) + (outputSample * positionR);
            }
            inputSampleR = outputSample;
            slew = fabs(inputSampleR - lastSoftenR) * freq;
            if (slew > 0.5) {
                slew = 0.5;
            }
            inputSampleR = (inputSampleR * slew) + (lastSoftenR * (1.0 - slew));
            // conditional average: only if we actually have brightness
            lastSampleR = drySampleR;
            lastSoftenR = outputSample;
            // end Frequency Derez
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
} // namespace airwindohhs::pockey
