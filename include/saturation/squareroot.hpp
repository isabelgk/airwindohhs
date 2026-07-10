#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::squareroot {

constexpr std::string_view k_name{ "SquareRoot" };
constexpr std::string_view k_short_description{
    "SquareRoot has new discoveries in soft saturation!"
};
constexpr std::string_view k_long_description{
    "What if saturation, but not flat-topping, because it was a kind that literally never reaches a 'top' but always keeps producing louder output the harder you drive it?Well, that would defeat the purpose of safety clipping, first of all, but then let's see what else we can do :)I don't think sqrt() commonly gets used for saturation effects, for several reasons. First, it's wildly inaccurate at tiny values. Second, it won't safety clip (obviously). Third, if you DO try to use it even though it's wildly inaccurate at tiny values, it turns and bites you, throwing in a crossover distortion that can be really distracting. You've heard it… if you heard LRConvolve, because that uses a square root to compensate for how you're multiplying one channel by the other.There's no solution to these things, but is there a workaround? YES. And it's called 'Soar'.Turns out you can adjust how the square root works to get a smooth contour that still gives you that infinitely-extended softclip. It's a useful effect, that feels kinda like how our ears respond to loudness. If you have something like a drum track that's all spikey and doesn't sound anything like being there, SquareRoot at some level of drive (you can add as much as 12 dB in the plugin, and nearly a hundred dB with BitShiftGain in front of it) will immediately give it that 'there in the room' feel while still distinguishing dynamics for you.And then there's 'Soar' sitting there: all it does is change this adjustment from full smoothness… to uncorrected, nonlinear square root quirkiness. And that throws in controllable amounts of crossover distortion, Class B or AB distortion (since it's not a sharp edge but increasingly weird curves) and that makes quiet details louder, unnaturally so.It lets you keep that effect subtle (like the eccentricities of a big push-pull tube amp, or a grungey Hammond B3) or make it really obnoxious: then if you overdrive the input harder, that Soar coloration will become more subtle again, because you'll have to turn the output down. If even the plain output with no 'Soar' is too intense, you get a dry/wet control that should be able to help you find just the right intensity for whatever SquareRoot can produce.Soar is originally from a story by Mixerman, who used a stray knob to appease a difficult record label exec sitting in on a session. In the story, it was not real. But this is! Right down to the dangers of adding TOO MUCH soar and ruining everything. So, be prudent and don't soar too much!This is bonkers for getting retro tones, for simulating acoustic loudness (you can put it in front of Discontinuity for even more loudness cues!) and for giving certain instruments their natural color, like big amps and wonky organs. The effect exists in guitar stompboxes as a notoriously aggro pedal: ZVex's Machine pedal. But this plugin is not that, it's a separate effect with its own purposes that happens to be one of the few effects to work with crossover distortion, normally not a popular effect, but now rendered accessible and useful.I know I'll be finding uses for all this, and I hope you do too :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class SquareRoot final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SquareRoot()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        D = 1.0;
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
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "in"; break;
            case kParamB: return "soar"; break;
            case kParamC: return "out"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "In"; break;
            case kParamB: return "Soar"; break;
            case kParamC: return "Out"; break;
            case kParamD: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;

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

        double in = A * 2.0;
        in *= in;
        double soar = 0.3 - (B * 0.3);
        double out = C * 2.0;
        out *= out;
        double wet = D;
        while (--sampleFrames >= 0) {
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
            if (in != 1.0) {
                inputSampleL *= in;
                inputSampleR *= in;
            }
            double temp = fabs(inputSampleL) + (soar * soar);
            if (inputSampleL > 0.0) {
                inputSampleL = fmax((sqrt(1.0 / temp) * temp) - soar, 0.0);
            }
            if (inputSampleL < 0.0) {
                inputSampleL = fmin((-sqrt(1.0 / temp) * temp) + soar, 0.0);
            }
            temp = fabs(inputSampleR) + (soar * soar);
            if (inputSampleR > 0.0) {
                inputSampleR = fmax((sqrt(1.0 / temp) * temp) - soar, 0.0);
            }
            if (inputSampleR < 0.0) {
                inputSampleR = fmin((-sqrt(1.0 / temp) * temp) + soar, 0.0);
            }
            if (out != 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
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
} // namespace airwindohhs::squareroot
