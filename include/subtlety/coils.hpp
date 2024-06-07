#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::coils {

constexpr std::string_view k_name{ "Coils" };
constexpr std::string_view k_short_description{
    "Coils models the types of distortion you'll find in transformers."
};
constexpr std::string_view k_long_description{
    "Coils is like the inverse of Focus. Instead of boosting/distorting UP the mids, it saturates DOWN the extreme highs and lows leaving the mids untouched. Because it’s a fixed mid shape mimicking the Neve transformer picture I was shown, the biquad doesn’t have to run inside Console as it’s not a high-Q filter at all (you’ll notice way more of an effect on steep filters) so it trades off that effect for less processing. The saturation is the Density algorithm, but without clipping, so if you trash it you get wrap-around on the transfer function curve, which works out to be kinda transformery.The top control increasingly distorts (still kinda subtle), the middle one DC biases the core (second harmonics!) which does more when you’re less saturated, and then there’s a dry/wet. Enjoy :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Coils final : public Effect<T>
{
    double figureL[9];
    double figureR[9];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Coils()
    {
        A = 0.0;
        B = 0.5;
        C = 1.0;
        for (int x = 0; x < 9; x++) {
            figureL[x] = 0.0;
            figureR[x] = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "saturat"; break;
            case kParamB: return "core dc"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Saturat"; break;
            case kParamB: return "Core DC"; break;
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

        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        double boost = 1.0 - pow(A, 2);
        if (boost < 0.001) {
            boost = 0.001; // there's a divide, we can't have this be zero
        }
        figureL[0] = figureR[0] = 600.0 / Effect<T>::getSampleRate(); // fixed frequency, 600hz
        figureL[1] = figureR[1] = 0.023; // resonance
        double offset = (B * 2.0) - 1.0;
        double sinOffset = sin(offset); // we can cache this, it's expensive
        double wet = C;
        double K = tan(M_PI * figureR[0]);
        double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
        figureL[2] = figureR[2] = K / figureR[1] * norm;
        figureL[4] = figureR[4] = -figureR[2];
        figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
        figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
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
            // double tempSample = (inputSample * figure[2]) + figure[7];
            // figure[7] = -(tempSample * figure[5]) + figure[8];
            // figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
            // inputSample = tempSample + sin(drySample-tempSample);
            // or
            // inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
            //
            // given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
            // the full frequencies but distorts like a real transformer. Purest case, and since
            // we are not using a high Q we can remove the extra sin/asin on the biquad.
            double tempSample = (inputSampleL * figureL[2]) + figureL[7];
            figureL[7] = -(tempSample * figureL[5]) + figureL[8];
            figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
            inputSampleL = tempSample + ((sin(((drySampleL - tempSample) / boost) + offset) - sinOffset) * boost);
            // given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
            // the full frequencies but distorts like a real transformer. Since
            // we are not using a high Q we can remove the extra sin/asin on the biquad.
            tempSample = (inputSampleR * figureR[2]) + figureR[7];
            figureR[7] = -(tempSample * figureR[5]) + figureR[8];
            figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
            inputSampleR = tempSample + ((sin(((drySampleR - tempSample) / boost) + offset) - sinOffset) * boost);
            // given a bandlimited inputSample, freq 600hz and Q of 0.023, this restores a lot of
            // the full frequencies but distorts like a real transformer. Since
            // we are not using a high Q we can remove the extra sin/asin on the biquad.
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
} // namespace airwindohhs::coils
