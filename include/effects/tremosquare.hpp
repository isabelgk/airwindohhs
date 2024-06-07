#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tremosquare {

constexpr std::string_view k_name{ "TremoSquare" };
constexpr std::string_view k_short_description{
    "TremoSquare is a squarewave tremolo effect that only switches on zero crossings."
};
constexpr std::string_view k_long_description{
    "Surprise! More sound design, texture-making, mix-blendering fun from Airwindows.I meant to put out BassAmp today, but it’s not ready: needs more work. But I had this weird little critter handy, and so it’s skipping ahead whether you like it or not!And that depends on what you get up to, signal processing wise. TremoSquare comes out of one of my livestreams, where I coded a plugin from scratch one Monday so people could watch the process. (I do that now.) It’s a squarewave tremolo, but it only transitions from silent to full volume on the zero crossings.What that does, is firstly give the aggressive tremolo a nice warm coloration that doesn’t click or crackle, even on bassy sounds. But secondly, if you ramp up the frequency super high, it stops registering as a frequency because the crossings take precedence and interfere with the frequency of the transitions. So, you get a distinct sort of ‘de-rezzing’ effect that’s literally nothing but a tremolo, except it’s sculpted to be smoother and more graceful. It doesn’t tempo sync: think of it more like that effect on Bowie’s ‘Diamond Dogs’ album of singing through a fan that’s on. It’s got a dry/wet control so it can be faded in for effect, and the frequency range of the tremolo is extremely huge.Hope you like it! I meant to do an entirely different plugin, but I just had this lying around…"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class TremoSquare final : public Effect<T>
{
    double osc;
    bool polarityL;
    bool muteL;
    bool polarityR;
    bool muteR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    TremoSquare()
    {
        A = 0.5;
        B = 1.0;
        osc = 0.0;
        polarityL = false;
        muteL = false;
        polarityR = false;
        muteR = false;
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
            return 0.5;
            break;
        kParamB:
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
            return "freq";
            break;
        kParamB:
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
            return "Freq";
            break;
        kParamB:
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
        overallscale *= Effect<T>::getSampleRate();
        double increment = pow(A, 4) / (50.0 * overallscale);
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
            osc += increment;
            if (osc > 1.0) {
                osc = 0.0;
            }
            // this is our little oscillator code
            if (inputSampleL < 0) {
                if (polarityL == true) {
                    muteL = (osc < 0.5);
                }
                polarityL = false;
            }
            else {
                if (polarityL == false) {
                    muteL = (osc < 0.5);
                }
                polarityL = true;
            }
            if (inputSampleR < 0) {
                if (polarityR == true) {
                    muteR = (osc < 0.5);
                }
                polarityR = false;
            }
            else {
                if (polarityR == false) {
                    muteR = (osc < 0.5);
                }
                polarityR = true;
            }
            if (muteL) {
                inputSampleL = 0.0;
            }
            if (muteR) {
                inputSampleR = 0.0;
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::tremosquare
