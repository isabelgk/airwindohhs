#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::trianglizer {

constexpr std::string_view k_name{ "Trianglizer" };
constexpr std::string_view k_short_description{
    "Trianglizer is a special waveshaper that can transform sines into triangles."
};
constexpr std::string_view k_long_description{
    "This should be interesting. It's a waveshaper with a couple unusual tricks. Meet Trianglizer.The first thing it can do is turn full-scale sines into triangle waves… sort of. The slope on the triangle wave is slightly inconsistent (because there isn't a magical math function to make the sine into a perfect triangle, it's just a side-effect of something else). It's all a product of some work I did refining the algorithms of Console 6 to improve their accuracy as they do a zero crossing, which is constantly when music is playing (whether or not the music is loud, it all centers on zero and issues around zero crossing produce class B distortion in tiny amounts)The thing to note here is that, while it wants full scale sines for a sharp point on the triangle, if your sines aren't that loud, what it will do is instead make a triangle wave with a round point. The point gets sharper or rounder depending on how loud the wave is. That means it's a dynamic triangle, like there's a filter, except it's getting synthesized without one.Plus, it's a waveshaper, so it doesn't care if you have a sine or anything else. It'll just reshape the wave for you. Then if you're not done, you can adjust the 'tri/fat' control and it'll do the same thing except it widens the top of the triangle into a rounded-top trapezoid wave, adding a different kind of saturation beyond simply clipping, while keeping the same slightly inconsistent 'triangle wave' angles, through zero crossing.But what does that DO?Turns out we are applying two different shapes to get our result. The loud stuff mostly gets turned down (though with Fat engaged, it's turned up to make peaks thicker and hotter). There's a place in the waveform where the level remains exactly the same, and all the transitions between these points are super smooth. But as the sound gets quieter still… that, too, gets turned down. The slope of the waveform remains totally smooth, but it's a shallower angle than the sine that feeds it, and even shallower than what a true triangle wave would give you (subtly including some dry signal might fix this).And so, we have an effect that dries up fat siney basses and makes them more articulate, cutting the level a bit, when it's on Tri. On regular music, drums etc. it does likewise. Stuff drops back and becomes more dynamic, with the hottest peaks poking way out in spikey form. It'll make things more crisp and defined.But the really interesting part is when you set it to Fat, and begin using that for added beef and slam. Because when you're doing that, you're doing the loudness saturation… but your quietness is simultaneously dropping back. So you get a super-dynamic retro effect that to me sounds like old Kinks albums or something: it's distorted, it's loud, but it's all punch and no wallow. Unlike just using sine functions to make everything super fat, this purely adds muscle and some variation of it is probably going to find its way into a retro Console emulation from me. It's that good at conveying Fifties and early Sixties sound, where everything was intensely dynamic yet weirdly opaque and dense and the sound was felt as much as heard.Or, you could just play with it and see what you get :) I am only trying to interpret a new plugin I didn't have before, which is kind of like my old High Impact plugin, but using a far more elegant algorithm that could have all kinds of uses.Just remember: set to Tri to lean out and point things, set to Fat for guts and punch while still leaning out things, and use the dry-wet if the effect is too strong. It handles the loud and soft parts separately while handling them at the same time with one very smooth and accurate algorithm. It's a mystery to me too, but I think it will find many uses as a sonic muscle-adder. Enjoy!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Trianglizer final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Trianglizer()
    {
        A = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "tri/fat"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Tri/Fat"; break;
            case kParamB: return "Dry/Wet"; break;

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

        double offset = (A + 1.0) * 0.5;
        double inverter = (offset * 2.0) - 1.0;
        double wet = B;
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
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            else if (inputSampleL > 0.0) {
                inputSampleL = -expm1((log1p(-inputSampleL) * (offset + (inputSampleL * inverter))));
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            else if (inputSampleL < 0.0) {
                inputSampleL = expm1((log1p(inputSampleL) * (offset - (inputSampleL * inverter))));
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            else if (inputSampleR > 0.0) {
                inputSampleR = -expm1((log1p(-inputSampleR) * (offset + (inputSampleR * inverter))));
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            else if (inputSampleR < 0.0) {
                inputSampleR = expm1((log1p(inputSampleR) * (offset - (inputSampleR * inverter))));
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::trianglizer
