#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::sweeten {

constexpr std::string_view k_name{ "Sweeten" };
constexpr std::string_view k_short_description{
    "Sweeten is where you can find super-clean second harmonic."
};
constexpr std::string_view k_long_description{
    "So this serves a few purposes. Sweeten is one of the super-minimal, one knob plugins. It makes second harmonic: much like SingleEndedTriode, or Inflamer. The first purpose is to exist, so if you're thinking 'I gotta sweeten this sound, now which Airwindows plugin of the three billion and twelve does that? Conflagration? No, that can't be it' then you can go 'is there one literally called Sweeten', and now there is! And it does what you'd expect, even harmonics (second harmonic, specifically). That's all it does.Another purpose is because I use stuff like this in my designs. So it's useful to have a chunk of code that I can take and put in the midrange section of an EQ, or something, if I think it's lacking that subtle nonlinearity which comes out of some circuits. Sweeten is specifically designed to let me do that quickly and easily, and if I make a tool for myself which is actually neat and efficient, I give it to people.The only thing Sweeten can do that you can't already do with Inflamer is be simple, and maybe one or two fewer math operations, and that's IT. This isn't new (second harmonic isn't new, either). My hope is that it's such an elegant, easily identifiable little device that it'll come in handy. It's also using the stepped-control, bit-shift-gain thing I do lately, so one thing about it is that it'll prevent you overprocessing. If you turn it up until you can hear it a little too well, drop back a step and then you can't tell it's doing anything, but it very much still is. For many types of processing (second harmonic emphatically included!) the optimal setting is where it's doing its thing but at no point is it ever distracting or sticking out as a mixing mistake. Sweeten is predisposed to quickly get you to that point with no fuss.I'll be using it: if anybody else finds it useful, that's even better :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Sweeten final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double savg[20];
    float A;

  public:
    Sweeten()
    {
        A = 0.0;
        for (int x = 0; x < 19; x++) {
            savg[x] = 0.0;
        }
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
        kNumParameters = 1

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "sweeten";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Sweeten";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        int sweetBits = 10 - floor(A * 10.0);
        double sweet = 1.0;
        switch (sweetBits)
        {
            case 11: sweet = 0.00048828125; break;
            case 10: sweet = 0.0009765625; break;
            case 9: sweet = 0.001953125; break;
            case 8: sweet = 0.00390625; break;
            case 7: sweet = 0.0078125; break;
            case 6: sweet = 0.015625; break;
            case 5: sweet = 0.03125; break;
            case 4: sweet = 0.0625; break;
            case 3: sweet = 0.125; break;
            case 2: sweet = 0.25; break;
            case 1: sweet = 0.5; break;
            case 0: sweet = 1.0; break;
            case -1: sweet = 2.0; break;
        } // now we have our input trim
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
            double sweetSample = inputSampleL;
            double sv = sweetSample;
            sweetSample = (sweetSample + savg[0]) * 0.5;
            savg[0] = sv;
            if (cycleEnd > 1) {
                sv = sweetSample;
                sweetSample = (sweetSample + savg[1]) * 0.5;
                savg[1] = sv;
                if (cycleEnd > 2) {
                    sv = sweetSample;
                    sweetSample = (sweetSample + savg[2]) * 0.5;
                    savg[2] = sv;
                    if (cycleEnd > 3) {
                        sv = sweetSample;
                        sweetSample = (sweetSample + savg[3]) * 0.5;
                        savg[3] = sv;
                    }
                } // if undersampling code is present, this gives a simple averaging that stacks up
            } // when high sample rates are present, for a more intense aliasing reduction. PRE nonlinearity
            sweetSample = (sweetSample * sweetSample * sweet); // second harmonic (nonlinearity)
            sv = sweetSample;
            sweetSample = (sweetSample + savg[4]) * 0.5;
            savg[4] = sv;
            if (cycleEnd > 1) {
                sv = sweetSample;
                sweetSample = (sweetSample + savg[5]) * 0.5;
                savg[5] = sv;
                if (cycleEnd > 2) {
                    sv = sweetSample;
                    sweetSample = (sweetSample + savg[6]) * 0.5;
                    savg[6] = sv;
                    if (cycleEnd > 3) {
                        sv = sweetSample;
                        sweetSample = (sweetSample + savg[7]) * 0.5;
                        savg[7] = sv;
                    }
                } // if undersampling code is present, this gives a simple averaging that stacks up
            } // when high sample rates are present, for a more intense aliasing reduction. POST nonlinearity
            inputSampleL -= sweetSample; // apply the filtered second harmonic correction
            sweetSample = inputSampleR;
            sv = sweetSample;
            sweetSample = (sweetSample + savg[8]) * 0.5;
            savg[8] = sv;
            if (cycleEnd > 1) {
                sv = sweetSample;
                sweetSample = (sweetSample + savg[9]) * 0.5;
                savg[9] = sv;
                if (cycleEnd > 2) {
                    sv = sweetSample;
                    sweetSample = (sweetSample + savg[10]) * 0.5;
                    savg[10] = sv;
                    if (cycleEnd > 3) {
                        sv = sweetSample;
                        sweetSample = (sweetSample + savg[11]) * 0.5;
                        savg[11] = sv;
                    }
                } // if undersampling code is present, this gives a simple averaging that stacks up
            } // when high sample rates are present, for a more intense aliasing reduction. PRE nonlinearity
            sweetSample = (sweetSample * sweetSample * sweet); // second harmonic (nonlinearity)
            sv = sweetSample;
            sweetSample = (sweetSample + savg[12]) * 0.5;
            savg[12] = sv;
            if (cycleEnd > 1) {
                sv = sweetSample;
                sweetSample = (sweetSample + savg[13]) * 0.5;
                savg[13] = sv;
                if (cycleEnd > 2) {
                    sv = sweetSample;
                    sweetSample = (sweetSample + savg[14]) * 0.5;
                    savg[14] = sv;
                    if (cycleEnd > 3) {
                        sv = sweetSample;
                        sweetSample = (sweetSample + savg[15]) * 0.5;
                        savg[15] = sv;
                    }
                } // if undersampling code is present, this gives a simple averaging that stacks up
            } // when high sample rates are present, for a more intense aliasing reduction. POST nonlinearity
            inputSampleR -= sweetSample; // apply the filtered second harmonic correction
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
} // namespace airwindohhs::sweeten
