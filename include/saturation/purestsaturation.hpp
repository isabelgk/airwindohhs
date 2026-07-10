#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestsaturation {

constexpr std::string_view k_name{ "PurestSaturation" };
constexpr std::string_view k_short_description{
    "PurestSaturation is an experiment in softclipping."
};
constexpr std::string_view k_long_description{
    "Not so long ago, I pushed and pulled the parameters of a sin() function (thing called a Taylor series expansion) to produce TapeHack, which is a softclip that uses the wrong values for a sin() softclip, to produce another softclip that feels more retro and vintage.But what if we did something weirder? Specifically, doing this requires that you divide by increasingly huge numbers. For a real sin() these are factorials (very huge indeed). For TapeHack, I doctored these until the shape of the softclip did exactly what I wanted.Remember how my BitShiftGain works? The ultra-clean gain change I can do (either internal to a plugin, or as the plugin of that name) because if you multiply or divide by exactly powers of 2, in any floating point format, it'll change the game without ever recalculating the 'mantissa' of the number. It only changes the 'exponent'. And if you don't change the mantissa… you never requantize. So you have literal perfect gain change that is totally and perfectly lossless within the entire range of the exponent… as long as you only change by 6.08 dB increments.A bit like the Monty Python skit where a man inherits miles of string, except due to bad planning it is in six inch lengths…But hey, if you're doing a Taylor series, the first stage's subtracted at 15.56 dB down. Then, 41.58 dB down. Then, 74.04 dB and so on.What if instead, -18 and -42 and -72 and -108 and -150 dB? Rather than the correct math, instead using whatever division will make sure the mantissa is exactly the same?And here we are. 'BitShiftGain' but five times over, inside a saturation algorithm, set up to be intentionally wrong but as if we're passing through the parts of the algorithm, losslessly. Except our notion of 'losslessly' is very much Airwindowsized and requires that we use the wrong dividers… except for that's how we have TapeHack.What happens? Try it and find out. I've never heard a sophisticated sine approximation algorithm that completely breaks math in order to be able to get as many mantissas unquantized, before. Remember, it's also a distortion of what a sin() would be, so it's guaranteed to be different and have a distinct sound, but this is somewhat arbitrary. I find it to be more dynamic than ordinary softclip, and I'm still working out the variations and what I might get out of it. Today it's this! Hope you like it :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class PurestSaturation final : public Effect<T>
{
    float A;
    float B;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestSaturation()
    {
        A = 0.1;
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
            case kParamA: return 0.1; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Output"; break;

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

        long double inputGain = A * 10.0;
        long double outputGain = B;
        while (--sampleFrames >= 0) {
            long double inputSampleL = *in1;
            long double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            inputSampleL = fmin(fmax(inputSampleL, -2.032610446872596), 2.032610446872596);
            long double X = inputSampleL * inputSampleL;
            long double temp = inputSampleL * X;
            inputSampleL -= (temp * 0.125);
            temp *= X;
            inputSampleL += (temp * 0.0078125);
            temp *= X;
            inputSampleL -= (temp * 0.000244140625);
            temp *= X;
            inputSampleL += (temp * 0.000003814697265625);
            temp *= X;
            inputSampleL -= (temp * 0.0000000298023223876953125);
            temp *= X;
            // purestsaturation: sine, except all the corrections
            // retain mantissa of a long double increasing power function
            inputSampleR = fmin(fmax(inputSampleR, -2.032610446872596), 2.032610446872596);
            X = inputSampleR * inputSampleR;
            temp = inputSampleR * X;
            inputSampleR -= (temp * 0.125);
            temp *= X;
            inputSampleR += (temp * 0.0078125);
            temp *= X;
            inputSampleR -= (temp * 0.000244140625);
            temp *= X;
            inputSampleR += (temp * 0.000003814697265625);
            temp *= X;
            inputSampleR -= (temp * 0.0000000298023223876953125);
            temp *= X;
            // purestsaturation: sine, except all the corrections
            // retain mantissa of a long double increasing power function
            if (outputGain < 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
            }
            // begin 64 bit stereo floating point dither
            int expon;
            frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            inputSampleL += ((double(fpdL) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
            frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleR += ((double(fpdR) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
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
} // namespace airwindohhs::purestsaturation
