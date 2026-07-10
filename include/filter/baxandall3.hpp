#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::baxandall3 {

constexpr std::string_view k_name{ "Baxandall3" };
constexpr std::string_view k_short_description{
    "Baxandall3 is for new tone colors."
};
constexpr std::string_view k_long_description{
    "The new Baxandall combines elements of Baxandall2, with things I'd left behind in Baxandall the original, to produce something that has tone colors which haven't been heard before.The catch is, it's because people don't go for these tone colors, or know how to get them, or what to do with them if they had them. But when has that ever stopped us? This could be your lucky day (or, a big waste of time :) )The new Baxandall adopts the technique from its predecessor, letting you apply larger boosts and cuts, and sweeping the frequencies so you're accentuating the extremes when you get extra aggressive. It's based on Bessel filter slopes, but it brings back interleaving (meaning that it'll take on odd and interesting flavors when you get aggressive with the highs, and it can only tilt so far).But most of all, it brings back the Console processing (tweaked for maximum sonic density) and it does it in a strange, backwards way.If you apply slight boosts, the loudness goes up… INSIDE the processing. This is the same as if you were playing with the DAW faders in a Console mix. It's 'Doing It Wrong', but wrong on purpose. What happens when you do this basic, fundamental thing wrong?Firstly, the filter becomes hypersensitive to near-flat settings. If you boost, you're not just increasing that frequency's level, you're also pushing it harder into the 'anti-saturation' and getting that much hotter a result. The calibration's off. It'll be expanded, peakier, more dynamic.If you cut, you're pulling it back before it reaches 'anti-saturation', so it's not only softer, it's also more distorted. The dynamic punch is flattened, because it's saturation that hasn't been counteracted. You step really hard on the presence and punchiness of whatever's being turned down.There's an input gain control so you can gain stage this, but it's all working off Treble and Bass being either exactly 0.5, or Bad Things happen. The fact that they're interleaved Bessel filters just means the things that happen are spread across a wider range. The tone shapings that can happen out of this are really interesting and bizarre.Thing is, there's a twist to the catch. Stuff being expanded and dynamic-ified when it gets louder? That behavior also makes things sound farther away. Suppressing and distorting stuff more when you turn it down? That tends to make things sound more close-up. So the most basic, fundamental operations of Baxandall3 simultaneously apply huge EQ curves, while also hiding them and making them shift spatially the opposite to what you'd expect. Normally when we push levels up we expect saturation to rise. In Console7, the first time I experimented with this mechanic, that's what you get: more is also closer, and it's very natural and easy to hear. (FatEQ is the same.) This? This is backwards.I can't even imagine what people will make of this. If you're not being super-aggressive with it, you'll find that it responds to the tiniest adjustments. If you are getting super-aggressive with it, let me know what works and what doesn't because I'm still wrapping my head around how it even works. I assume there's going to be some sort of sound that Baxandall3 fits perfectly, and I'm not entirely sure what it'd be. But in its backward spatial tomfoolery, I'm sure it's the missing link for getting SOME kind of tone. Enjoy exploring!"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Baxandall3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        bax_freq,
        bax_reso,
        bax_a0,
        bax_a1,
        bax_a2,
        bax_b1,
        bax_b2,
        bax_sLA1,
        bax_sLA2,
        bax_sRA1,
        bax_sRA2,
        bax_sLB1,
        bax_sLB2,
        bax_sRB1,
        bax_sRB2,
        bax_total
    }; // baxed frequency biquad filter for ultrasonics, stereo
    double baxH[bax_total];
    double baxL[bax_total];
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Baxandall3()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < bax_total; x++) {
            baxH[x] = 0.0;
            baxL[x] = 0.0;
        }
        flip = false;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "treble"; break;
            case kParamC: return "bass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Treble"; break;
            case kParamC: return "Bass"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double inputGain = pow(A * 2.0, 2.0);
        double trebleGain = pow(B * 2.0, 2.0);
        double trebleFreq = ((2000.0 * trebleGain) + 200.0) / Effect<T>::getSampleRate();
        if (trebleFreq > 0.45) {
            trebleFreq = 0.45;
        }
        baxH[bax_freq] = trebleFreq;
        baxH[bax_reso] = 0.57735026919; // bessel second order
        double K = tan(M_PI * baxH[bax_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / baxH[bax_reso] + K * K);
        baxH[bax_a0] = K * K * norm;
        baxH[bax_a1] = 2.0 * baxH[bax_a0];
        baxH[bax_a2] = baxH[bax_a0];
        baxH[bax_b1] = 2.0 * (K * K - 1.0) * norm;
        baxH[bax_b2] = (1.0 - K / baxH[bax_reso] + K * K) * norm;
        // end bax highpass
        double bassGain = pow(C * 2.0, 2.0);
        double bassFreq = pow((1.0 - C) * 2.0, 2.0);
        bassFreq = ((2000.0 * bassFreq) + 200.0) / Effect<T>::getSampleRate();
        if (bassFreq > 0.45) {
            bassFreq = 0.45;
        }
        baxL[bax_freq] = bassFreq;
        baxL[bax_reso] = 0.57735026919; // bessel second order
        K = tan(M_PI * baxL[bax_freq]); // lowpass
        norm = 1.0 / (1.0 + K / baxL[bax_reso] + K * K);
        baxL[bax_a0] = K * K * norm;
        baxL[bax_a1] = 2.0 * baxL[bax_a0];
        baxL[bax_a2] = baxL[bax_a0];
        baxL[bax_b1] = 2.0 * (K * K - 1.0) * norm;
        baxL[bax_b2] = (1.0 - K / baxL[bax_reso] + K * K) * norm;
        // end bax lowpass
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL = sin(fmax(fmin(inputSampleL * inputGain, M_PI_2), -M_PI_2));
            inputSampleR = sin(fmax(fmin(inputSampleR * inputGain, M_PI_2), -M_PI_2));
            // encode Console5: good cleanness
            double trebleSampleL;
            double trebleSampleR;
            double bassSampleL;
            double bassSampleR;
            if (flip) {
                trebleSampleL = (inputSampleL * baxH[bax_a0]) + baxH[bax_sLA1];
                baxH[bax_sLA1] = (inputSampleL * baxH[bax_a1]) - (trebleSampleL * baxH[bax_b1]) + baxH[bax_sLA2];
                baxH[bax_sLA2] = (inputSampleL * baxH[bax_a2]) - (trebleSampleL * baxH[bax_b2]);
                trebleSampleL = inputSampleL - trebleSampleL;
                trebleSampleR = (inputSampleR * baxH[bax_a0]) + baxH[bax_sRA1];
                baxH[bax_sRA1] = (inputSampleR * baxH[bax_a1]) - (trebleSampleR * baxH[bax_b1]) + baxH[bax_sRA2];
                baxH[bax_sRA2] = (inputSampleR * baxH[bax_a2]) - (trebleSampleR * baxH[bax_b2]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleL = (inputSampleL * baxL[bax_a0]) + baxL[bax_sLA1];
                baxL[bax_sLA1] = (inputSampleL * baxL[bax_a1]) - (bassSampleL * baxL[bax_b1]) + baxL[bax_sLA2];
                baxL[bax_sLA2] = (inputSampleL * baxL[bax_a2]) - (bassSampleL * baxL[bax_b2]);
                bassSampleR = (inputSampleR * baxL[bax_a0]) + baxL[bax_sRA1];
                baxL[bax_sRA1] = (inputSampleR * baxL[bax_a1]) - (bassSampleR * baxL[bax_b1]) + baxL[bax_sRA2];
                baxL[bax_sRA2] = (inputSampleR * baxL[bax_a2]) - (bassSampleR * baxL[bax_b2]);
            }
            else {
                trebleSampleL = (inputSampleL * baxH[bax_a0]) + baxH[bax_sLB1];
                baxH[bax_sLB1] = (inputSampleL * baxH[bax_a1]) - (trebleSampleL * baxH[bax_b1]) + baxH[bax_sLB2];
                baxH[bax_sLB2] = (inputSampleL * baxH[bax_a2]) - (trebleSampleL * baxH[bax_b2]);
                trebleSampleL = inputSampleL - trebleSampleL;
                trebleSampleR = (inputSampleR * baxH[bax_a0]) + baxH[bax_sRB1];
                baxH[bax_sRB1] = (inputSampleR * baxH[bax_a1]) - (trebleSampleR * baxH[bax_b1]) + baxH[bax_sRB2];
                baxH[bax_sRB2] = (inputSampleR * baxH[bax_a2]) - (trebleSampleR * baxH[bax_b2]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleL = (inputSampleL * baxL[bax_a0]) + baxL[bax_sLB1];
                baxL[bax_sLB1] = (inputSampleL * baxL[bax_a1]) - (bassSampleL * baxL[bax_b1]) + baxL[bax_sLB2];
                baxL[bax_sLB2] = (inputSampleL * baxL[bax_a2]) - (bassSampleL * baxL[bax_b2]);
                bassSampleR = (inputSampleR * baxL[bax_a0]) + baxL[bax_sRB1];
                baxL[bax_sRB1] = (inputSampleR * baxL[bax_a1]) - (bassSampleR * baxL[bax_b1]) + baxL[bax_sRB2];
                baxL[bax_sRB2] = (inputSampleR * baxL[bax_a2]) - (bassSampleR * baxL[bax_b2]);
            }
            flip = !flip;
            trebleSampleL *= trebleGain;
            trebleSampleR *= trebleGain;
            bassSampleL *= bassGain;
            bassSampleR *= bassGain;
            inputSampleL = bassSampleL + trebleSampleL; // interleaved biquad
            inputSampleR = bassSampleR + trebleSampleR; // interleaved biquad
            inputSampleL = asin(fmax(fmin(inputSampleL, 0.99999), -0.99999));
            inputSampleR = asin(fmax(fmin(inputSampleR, 0.99999), -0.99999));
            // amplitude aspect
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
} // namespace airwindohhs::baxandall3
