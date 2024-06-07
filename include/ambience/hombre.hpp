#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hombre {

constexpr std::string_view k_name{ "Hombre" };
constexpr std::string_view k_short_description{
    "Hombre is atmosphere and texture (through very short delays)."
};
constexpr std::string_view k_long_description{
    "Once upon a time there was the blues.No, let’s be more specific. Once upon a time (and even now!) there was ZZ Top. Brainchild of the Reverend Billy Gibbons, swathed in funk and mystery, serving up juicy grooves from the heart of Texas.Thing is, Rev. Billy tells some tall tales and their engineer of the day, Terry Manning, he don’t talk ATALL.So what is a person to do when they hear these albums and the guitars slide off that vinyl like grease off a hot griddle, and you know you can’t just put up a mic on anything amp-shaped and get near that magic? You know those are dirt guitars, but the whole texture’s different. Skulduggery is afoot. And the boys ain’t tellin’.Well, here’s what I did. It seemed to me that some of the mojo sounded like echoes and delays, but not just any old ones. You can take something as small as a dentist’s mirror, put it near the mic, and aim it until you’re reflecting another copy of your sound into the mic again: the delay is tiny but real, and the tone? Well, that’s based on how big the panel (or dentist’s mirror) is. If it’s tiny, you get only highs. If it’s a big ol’ panel, or a floor or wall, you get down into maybe the lowest bass. Any panel will do this. Billy and Terry might have been constructing lil’ forts around the amps, making a purely acoustic home for the blues. You can literally pick what range of sound you reflect, how long a delay it is (still so tiny it’s not heard as one!) and you don’t have to make it full-range: a softer reflector ignores highs, smaller panels ignore lows. If you want to juice up what your mic hears, this is one way to do it.If you’re playing with super-short echoes, you’re reinforcing the lows. Unless it’s out of phase, flipped upside down in the DAW, in which case you’re cancelling them! And then, supposing you have one delay that’s in phase and one that’s out, and you calibrate them just right, and then you’re neither reinforcing or cancelling the lows, instead you’re just thickening the texture of whatever you’ve got… all the little detail doubled, tripled, dripping down the mix, but the body of the thing basically the same and no sustain, just a couple of delay taps in real close…I’m not Billy and Terry. Since I’m Chris, I’ll fess up: that’s exactly what I did, and you can have it in Hombre. It’s two calibrated delay taps, which you can tweak a little, and if you bring them in you’ll thicken and diffuse your tones without altering where the lows sit, or adding much in the way of extra sustain. It’ll be punchy and get out of the way like reverb won’t, but it’ll be fatter and juicier than the dry signal. This is my interpretation of the ZZ Top secret sauce, or at least one of ’em, implemented in software rather than acoustics.I’ll never know how close I came, because them Texas boys don’t tell tales out of school. But Hombre is my humble offering for a simple plugin that brings a little mojo to what would otherwise be a dry voice or guitar… and it won’t muddy things up, just grease ’em a little."
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class Hombre final : public Effect<T>
{
    double pL[4001];
    double pR[4001];
    double slide;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Hombre()
    {
        A = 0.421;
        B = 0.5;
        for (int count = 0; count < 4000; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        gcount = 0;
        slide = 0.421;
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
            return 0.421;
            break;
        kParamB:
            return 0.5;
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
            return "voicing";
            break;
        kParamB:
            return "intensity";
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
            return "Voicing";
            break;
        kParamB:
            return "Intensity";
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
        double target = A;
        double offsetA;
        double offsetB;
        int widthA = (int)(1.0 * overallscale);
        int widthB = (int)(7.0 * overallscale); // max 364 at 44.1, 792 at 96K
        double wet = B;
        // removed extra dry variable
        double totalL;
        double totalR;
        int count;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            slide = (slide * 0.9997) + (target * 0.0003);
            offsetA = ((pow(slide, 2)) * 77) + 3.2;
            offsetB = (3.85 * offsetA) + 41;
            offsetA *= overallscale;
            offsetB *= overallscale;
            // adjust for sample rate
            if (gcount < 1 || gcount > 2000) {
                gcount = 2000;
            }
            count = gcount;
            pL[count + 2000] = pL[count] = inputSampleL;
            pR[count + 2000] = pR[count] = inputSampleR;
            // double buffer
            count = (int)(gcount + floor(offsetA));
            totalL = pL[count] * 0.391; // less as value moves away from .0
            totalL += pL[count + widthA]; // we can assume always using this in one way or another?
            totalL += pL[count + widthA + widthA] * 0.391; // greater as value moves away from .0
            totalR = pR[count] * 0.391; // less as value moves away from .0
            totalR += pR[count + widthA]; // we can assume always using this in one way or another?
            totalR += pR[count + widthA + widthA] * 0.391; // greater as value moves away from .0
            inputSampleL += ((totalL * 0.274));
            inputSampleR += ((totalR * 0.274));
            count = (int)(gcount + floor(offsetB));
            totalL = pL[count] * 0.918; // less as value moves away from .0
            totalL += pL[count + widthB]; // we can assume always using this in one way or another?
            totalL += pL[count + widthB + widthB] * 0.918; // greater as value moves away from .0
            totalR = pR[count] * 0.918; // less as value moves away from .0
            totalR += pR[count + widthB]; // we can assume always using this in one way or another?
            totalR += pR[count + widthB + widthB] * 0.918; // greater as value moves away from .0
            inputSampleL -= ((totalL * 0.629));
            inputSampleR -= ((totalR * 0.629));
            inputSampleL /= 4;
            inputSampleR /= 4;
            gcount--;
            // still scrolling through the samples, remember
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
}

};
} // namespace airwindohhs::hombre
