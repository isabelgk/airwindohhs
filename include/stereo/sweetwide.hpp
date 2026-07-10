#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sweetwide {

constexpr std::string_view k_name{ "SweetWide" };
constexpr std::string_view k_short_description{
    "SweetWide is a strange grungy stereo widener."
};
constexpr std::string_view k_long_description{
    "It's experiment time! This is a really good example of the sort of thing I do for Airwindows, and for the folks out there.You may well find this horrible, lame and useless. That is OK, it's not going to be for everybody.But if you like the sort of weird grunge this produces, we're talking signature sound, secret weapon town, bigtime… and NOBODY else will be bringing it, and no AI will arrive at this unless literally told to copy it (in which case they should credit and say it's SweetWide they're using).This plugin comes not from the world of Srsly or EdIsDim or Wider (plenty of other things I've made to do stereo widening nicely). It comes from my experiments with ring modulation. It's really a very simple algorithm though it does use square root functions: that's next to nothing, as far as CPU is concerned. It's doing terrible things to the stereo signal coming in, for the purpose of exaggerating side-channel information, but in a way where it's tending to produce asymmetric modulations. That's more along the lines of 'second harmonic, fourth harmonic etc' rather than 'third harmonic and normal distortion', and that's why Sweet is part of the name: we tend to hear even harmonics as more 'sweet' than odd harmonics.Even then it's not acting normal because it's using that Soar control to govern how the square-root reacts to subtle sounds. It serves the purpose of bringing up a gnarly compressey grunginess, except for stereo widening. If you crank it out all the way it's very obvious, but you can dial the Soar setting in that way, and then pull it back if you'd like it to still be unique but not so obnoxious.It'll also do a stereo narrowing effect that's maybe even more obnoxious, by turning Un/Wide all the way to Un. It's basically the same thing as an Inv/Dry/Wet control. The whole thing runs without any stored variables, and the setup of controls is hilariously trivial: without the square roots it'd be about the most CPU-efficient plugin you have.The reason you don't have this already is, it sounds terrible. But it's terrible in kind of a wonderful way. I demonstrate it on a full mix, and you should never use it on a full mix. Instead, I think it'd be a great widener on a heavy guitar buss, or certain synth tones, or maybe it's a really interesting way to narrow a drum room sound and bring a lot of grungey color to things. Any sound you can get from it, you can tune with the Soar control. I've managed to bring the totally weird Soar control from my ring modulators to a stereo widener/narrower, in case that's handy.If this is the plugin for you, you've probably already started throwing it on things and enjoying its unique gnarly space-distortions, and if this isn't the plugin for you, I don't know how much more clearly I can warn you. You're welcome :)"
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class SweetWide final : public Effect<T>
{
    float A;
    float B;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SweetWide()
    {
        A = 0.0;
        B = 0.5;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "soar"; break;
            case kParamB: return "un/wide"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Soar"; break;
            case kParamB: return "Un/Wide"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
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

        double soar = 0.3 - (A * 0.3);
        double blend = -(B - 0.5);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double outL = 0.0;
            double outR = 0.0;
            double inL = fabs(inputSampleL) + (soar * soar);
            double inR = fabs(inputSampleR) + (soar * soar);
            if (inputSampleL > 0.0) {
                outL = fmax((sqrt(inR / inL) * inL) - soar, 0.0);
            }
            if (inputSampleL < 0.0) {
                outL = fmin((-sqrt(inR / inL) * inL) + soar, 0.0);
            }
            if (inputSampleR > 0.0) {
                outR = fmax((sqrt(inL / inR) * inR) - soar, 0.0);
            }
            if (inputSampleR < 0.0) {
                outR = fmin((-sqrt(inL / inR) * inR) + soar, 0.0);
            }
            inputSampleL = (outL * blend) + (inputSampleL * (1.0 - blend));
            inputSampleR = (outR * blend) + (inputSampleR * (1.0 - blend));
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
} // namespace airwindohhs::sweetwide
