#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestair {

constexpr std::string_view k_name{ "PurestAir" };
constexpr std::string_view k_short_description{
    "PurestAir is a brightness experiment."
};
constexpr std::string_view k_long_description{
    "A while back, I got reminded of this plugin. Someone said, ‘you haven’t put this out, when are you going to?’ and I said ‘Oh, that’s right, I haven’t’.So here you go. It’s a bit weird. Normally ‘Purest’ plugins are ultra-simple. This isn’t quite like that and I don’t remember why. It’s sort of like Air or Energy, an air-band boost or cut, but it’s also got a limiting factor where it’ll refuse to boost past a certain intensity of treble. In that way, it’s like Acceleration. Except it’s not exactly Acceleration, not exactly a slew clipper… heck, it’s not even like Air or Energy. It’s a little like a de-esser in Bizarro World? Working oppositely?Anyway, it’s yours if you want it. There are many odd ways to do EQ code. This is clearly one of them, and it seems to be not exactly like anything else I’ve done, and it sure does crank up the extreme air band if you want that. And it’s got that clamping factor to cap it (though similarly to Acceleration, it’ll give you trouble trying to hear what’s being done). I guess check it out, and if your ears are amazing WAY up in the ultra-treble, and you also like the sound of it, you’ll probably be able to hear the action of the limiter just fine.Me, I’m more interested in dark reverbs and deeper spaces and bass, so this was never my pet plugin: maybe it can be yours :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class PurestAir final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double last1SampleL;
    double halfwaySampleL;
    double halfDrySampleL;
    double halfDiffSampleL;
    double diffSampleL;
    double lastSampleL;
    double s1L;
    double s2L;
    double s3L;
    double applyL;
    double last1SampleR;
    double halfwaySampleR;
    double halfDrySampleR;
    double halfDiffSampleR;
    double diffSampleR;
    double lastSampleR;
    double s1R;
    double s2R;
    double s3R;
    double applyR;
    float A;
    float B;
    float C;

  public:
    PurestAir()
    {
        A = 0.5;
        B = 0.0;
        C = 1.0;
        last1SampleL = 0.0;
        halfwaySampleL = halfDrySampleL = halfDiffSampleL = diffSampleL = 0.0;
        lastSampleL = 0.0;
        s1L = s2L = s3L = 0.0;
        applyL = 0.0;
        last1SampleR = 0.0;
        halfwaySampleR = halfDrySampleR = halfDiffSampleR = diffSampleR = 0.0;
        lastSampleR = 0.0;
        s1R = s2R = s3R = 0.0;
        applyR = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "boost"; break;
            case kParamB: return "limit"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Boost"; break;
            case kParamB: return "Limit"; break;
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
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;

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

        double applyTarget = (A * 2.0) - 1.0;
        double threshold = pow((1 - fabs(applyTarget)), 3);
        if (applyTarget > 0) {
            applyTarget *= 3;
        }
        double intensity = pow(B, 2) * 5.0;
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
            halfDrySampleL = halfwaySampleL = (inputSampleL + last1SampleL) / 2.0;
            last1SampleL = inputSampleL;
            s3L = s2L;
            s2L = s1L;
            s1L = inputSampleL;
            double m1 = (s1L - s2L) * ((s1L - s2L) / 1.3);
            double m2 = (s2L - s3L) * ((s1L - s2L) / 1.3);
            double sense = fabs((m1 - m2) * ((m1 - m2) / 1.3)) * intensity;
            // this will be 0 for smooth, high for SSS
            applyL += applyTarget - sense;
            applyL *= 0.5;
            if (applyL < -1.0) {
                applyL = -1.0;
            }
            double clamp = halfwaySampleL - halfDrySampleL;
            if (clamp > threshold) {
                halfwaySampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                halfwaySampleL = lastSampleL - threshold;
            }
            lastSampleL = halfwaySampleL;
            clamp = inputSampleL - lastSampleL;
            if (clamp > threshold) {
                inputSampleL = lastSampleL + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleL - threshold;
            }
            lastSampleL = inputSampleL;
            diffSampleL = *in1 - inputSampleL;
            halfDiffSampleL = halfDrySampleL - halfwaySampleL;
            inputSampleL = *in1 + ((diffSampleL + halfDiffSampleL) * applyL);
            // done with left channel
            halfDrySampleR = halfwaySampleR = (inputSampleR + last1SampleR) / 2.0;
            last1SampleR = inputSampleR;
            s3R = s2R;
            s2R = s1R;
            s1R = inputSampleR;
            m1 = (s1R - s2R) * ((s1R - s2R) / 1.3);
            m2 = (s2R - s3R) * ((s1R - s2R) / 1.3);
            sense = fabs((m1 - m2) * ((m1 - m2) / 1.3)) * intensity;
            // this will be 0 for smooth, high for SSS
            applyR += applyTarget - sense;
            applyR *= 0.5;
            if (applyR < -1.0) {
                applyR = -1.0;
            }
            clamp = halfwaySampleR - halfDrySampleR;
            if (clamp > threshold) {
                halfwaySampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                halfwaySampleR = lastSampleR - threshold;
            }
            lastSampleR = halfwaySampleR;
            clamp = inputSampleR - lastSampleR;
            if (clamp > threshold) {
                inputSampleR = lastSampleR + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleR - threshold;
            }
            lastSampleR = inputSampleR;
            diffSampleR = *in2 - inputSampleR;
            halfDiffSampleR = halfDrySampleR - halfwaySampleR;
            inputSampleR = *in2 + ((diffSampleR + halfDiffSampleR) * applyR);
            // done with right channel
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
} // namespace airwindohhs::purestair
