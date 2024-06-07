#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::remap {

constexpr std::string_view k_name{ "Remap" };
constexpr std::string_view k_short_description{
    "Remap puts the guts back into overloudenated audio!"
};
constexpr std::string_view k_long_description{
    "Is your audio too flat?Here’s the thing. We have an endless series of saturations, console models, tape emulations, iron oxide slams, smooth compressors, naughty compressors, magneto-dynamic infundibulators… as music-mongers, it seems we spend all our time distorting, squishing and flattening.And this is normal, because if you don’t do some of that it’s easy to come up with a very empty, stark, vanilla recording. Most of the genres we know and love feature some form of distortion or dynamics compression, or more likely both.But what about when we get carried away, and the result is about as impactful as Muzak? (which is fine in its place, but we crave a lot more)Until recently it didn’t matter because louder was always ‘better’… but now, Replay Gain and a million automatic gain functions have rendered the loudness our enemy. If you squish just a little too much you can end up flat, boring AND turned down by the gain control. So what do we do to get more impact and mojo WITHOUT splatting our mixes against a digital wall?Remap is finally out to answer that. You might not need it: if you’ve got great self-control or always squish too little, it might not help you. But for an awful lot of people, Remap can be the ‘hail mary’ mix de-squisher, after the fact. And since it works the way it does, it can find uses of other sorts, for it’s a pretty simple algorithm.Remap does a fairly decent job of taking a full scale sine wave and transmogrifying it into a softened triangle wave, if you set it just right. It heightens the pointiness, the peak energy, the aura of things. If you don’t exaggerate it, it stays nice and clean. If you do exaggerate it, you get a fierce crunchy punchiness but that’s what the dry/wet control is for. It produces peaks above 0dB on fullscale content, so be warned: it’s basically putting the dynamics back. Especially with soft-clipped stuff, Remap can reshape your original wave back again… or provide expansion and power where none existed.Pretty much anywhere your mix feels flat and congested, Remap can help (so long as your gain staging is toward the loud side). Turn it up until it’s too much then back it off. Below 0.5 will always be very subtle: above 0.5, things might get funky in a hurry. You might find a huge fierce bass drum manifesting itself, or guitars growing fangs and attitude, or vocals enunciating more clearly and passionately, belting harder. It depends on what’s already in your mix: used correctly, Remap can bring more of it out. There will be most likely ONE focus point for the Remap slider, for any given mix or sound within its range. Find that and then use output level and dry/wet to balance that super-real signal with however much of the source you want. This one REALLY likes dry/wet to give you natural results, the focal point might be a real gritty tonality. No gloss, just guts and kick and attitude."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Remap final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Remap()
    {
        A = 0.5;
        B = 1.0;
        C = 1.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
            return 1.0;
            break;
        kParamC:
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
            return "remap";
            break;
        kParamB:
            return "output";
            break;
        kParamC:
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
            return "Remap";
            break;
        kParamB:
            return "Output";
            break;
        kParamC:
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

        double drive = A * 0.5;
        double gain = (drive + 0.2) * 8;
        double out = B;
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
            double gaintrimL = fabs(inputSampleL);
            double gaintrimR = fabs(inputSampleR);
            double bridgerectifierL = gaintrimL * gain;
            double bridgerectifierR = gaintrimR * gain;
            if (gaintrimL > 1.0) {
                gaintrimL = 1.0;
            }
            else {
                gaintrimL *= gaintrimL;
            }
            if (gaintrimR > 1.0) {
                gaintrimR = 1.0;
            }
            else {
                gaintrimR *= gaintrimR;
            }
            if (inputSampleL > 1.57079633) {
                bridgerectifierL = 1.0 - (1.0 - sin(bridgerectifierL));
            }
            else {
                bridgerectifierL = sin(bridgerectifierL);
            }
            if (inputSampleR > 1.57079633) {
                bridgerectifierR = 1.0 - (1.0 - sin(bridgerectifierR));
            }
            else {
                bridgerectifierR = sin(bridgerectifierR);
            }
            if (inputSampleL > 0) {
                inputSampleL -= (bridgerectifierL * gaintrimL * drive);
            }
            else {
                inputSampleL += (bridgerectifierL * gaintrimL * drive);
            }
            if (inputSampleR > 0) {
                inputSampleR -= (bridgerectifierR * gaintrimR * drive);
            }
            else {
                inputSampleR += (bridgerectifierR * gaintrimR * drive);
            }
            if (out < 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
            if (wet < 1.0) {
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
} // namespace airwindohhs::remap
