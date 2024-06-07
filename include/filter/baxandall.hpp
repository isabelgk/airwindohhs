#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::baxandall {

constexpr std::string_view k_name{ "Baxandall" };
constexpr std::string_view k_short_description{
    "Baxandall is a killer general-purpose EQ."
};
constexpr std::string_view k_long_description{
    "Baxandall starts with a two-band filter that, zeroed out, subtracts an inverse lowpass from a lowpass and gives you bit-identical, perfectly transparent sound. That’s if you’re being subtle. If you boost or cut, lows or highs, it gives you the gentle broad boosts you expect, centered on the vital midrange. As you get more intense with the boosting, it gets more extreme, to where if you’re doing double boosts to get an intense exaggerated sound, a mids notch will naturally develop to accentuate the boosting. The whole voicing shifts to accomodate what you want to do with it, and you can play bass against treble or vice versa to get really wild voicings, such as for extreme EQ treatments (in terms of lows or highs)… but using the same natural Airwindows Baxandall tonality, so it won’t sound ‘filtery’, it’ll sound like it was meant to be that way.Airwindows Baxandall uses my interleaved biquad filters (original, purest form, not meant for rapid automation) that run inside a Console5 instance to deepen the sound of the filter."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Baxandall final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double trebleAL[9];
    double trebleBL[9];
    double bassAL[9];
    double bassBL[9];
    double trebleAR[9];
    double trebleBR[9];
    double bassAR[9];
    double bassBR[9];
    bool flip;
    float A;
    float B;
    float C;

  public:
    Baxandall()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int x = 0; x < 9; x++) {
            trebleAL[x] = 0.0;
            trebleBL[x] = 0.0;
            bassAL[x] = 0.0;
            bassBL[x] = 0.0;
            trebleAR[x] = 0.0;
            trebleBR[x] = 0.0;
            bassAR[x] = 0.0;
            bassBR[x] = 0.0;
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
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "treble"; break;
            case kParamB: return "bass"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Treble"; break;
            case kParamB: return "Bass"; break;
            case kParamC: return "Output"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return "dB"; break;
            case kParamC: return "dB"; break;

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

        double trebleGain = pow(10.0, ((A * 30.0) - 15.0) / 20.0);
        double trebleFreq = (4410.0 * trebleGain) / Effect<T>::getSampleRate();
        if (trebleFreq > 0.45) {
            trebleFreq = 0.45;
        }
        trebleAL[0] = trebleBL[0] = trebleAR[0] = trebleBR[0] = trebleFreq;
        double bassGain = pow(10.0, ((B * 30.0) - 15.0) / 20.0);
        double bassFreq = pow(10.0, -((B * 30.0) - 15.0) / 20.0);
        bassFreq = (8820.0 * bassFreq) / Effect<T>::getSampleRate();
        if (bassFreq > 0.45) {
            bassFreq = 0.45;
        }
        bassAL[0] = bassBL[0] = bassAR[0] = bassBR[0] = bassFreq;
        trebleAL[1] = trebleBL[1] = trebleAR[1] = trebleBR[1] = 0.4;
        bassAL[1] = bassBL[1] = bassAR[1] = bassBR[1] = 0.2;
        double output = pow(10.0, ((C * 30.0) - 15.0) / 20.0);
        double K = tan(M_PI * trebleAL[0]);
        double norm = 1.0 / (1.0 + K / trebleAL[1] + K * K);
        trebleBL[2] = trebleAL[2] = trebleBR[2] = trebleAR[2] = K * K * norm;
        trebleBL[3] = trebleAL[3] = trebleBR[3] = trebleAR[3] = 2.0 * trebleAL[2];
        trebleBL[4] = trebleAL[4] = trebleBR[4] = trebleAR[4] = trebleAL[2];
        trebleBL[5] = trebleAL[5] = trebleBR[5] = trebleAR[5] = 2.0 * (K * K - 1.0) * norm;
        trebleBL[6] = trebleAL[6] = trebleBR[6] = trebleAR[6] = (1.0 - K / trebleAL[1] + K * K) * norm;
        K = tan(M_PI * bassAL[0]);
        norm = 1.0 / (1.0 + K / bassAL[1] + K * K);
        bassBL[2] = bassAL[2] = bassBR[2] = bassAR[2] = K * K * norm;
        bassBL[3] = bassAL[3] = bassBR[3] = bassAR[3] = 2.0 * bassAL[2];
        bassBL[4] = bassAL[4] = bassBR[4] = bassAR[4] = bassAL[2];
        bassBL[5] = bassAL[5] = bassBR[5] = bassAR[5] = 2.0 * (K * K - 1.0) * norm;
        bassBL[6] = bassAL[6] = bassBR[6] = bassAR[6] = (1.0 - K / bassAL[1] + K * K) * norm;
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
            if (output != 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            } // gain trim in front of plugin, in case Console stage clips
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double trebleSampleL;
            double bassSampleL;
            double trebleSampleR;
            double bassSampleR;
            if (flip)
            {
                trebleSampleL = (inputSampleL * trebleAL[2]) + trebleAL[7];
                trebleAL[7] = (inputSampleL * trebleAL[3]) - (trebleSampleL * trebleAL[5]) + trebleAL[8];
                trebleAL[8] = (inputSampleL * trebleAL[4]) - (trebleSampleL * trebleAL[6]);
                trebleSampleL = inputSampleL - trebleSampleL;
                bassSampleL = (inputSampleL * bassAL[2]) + bassAL[7];
                bassAL[7] = (inputSampleL * bassAL[3]) - (bassSampleL * bassAL[5]) + bassAL[8];
                bassAL[8] = (inputSampleL * bassAL[4]) - (bassSampleL * bassAL[6]);
                trebleSampleR = (inputSampleR * trebleAR[2]) + trebleAR[7];
                trebleAR[7] = (inputSampleR * trebleAR[3]) - (trebleSampleR * trebleAR[5]) + trebleAR[8];
                trebleAR[8] = (inputSampleR * trebleAR[4]) - (trebleSampleR * trebleAR[6]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleR = (inputSampleR * bassAR[2]) + bassAR[7];
                bassAR[7] = (inputSampleR * bassAR[3]) - (bassSampleR * bassAR[5]) + bassAR[8];
                bassAR[8] = (inputSampleR * bassAR[4]) - (bassSampleR * bassAR[6]);
            }
            else
            {
                trebleSampleL = (inputSampleL * trebleBL[2]) + trebleBL[7];
                trebleBL[7] = (inputSampleL * trebleBL[3]) - (trebleSampleL * trebleBL[5]) + trebleBL[8];
                trebleBL[8] = (inputSampleL * trebleBL[4]) - (trebleSampleL * trebleBL[6]);
                trebleSampleL = inputSampleL - trebleSampleL;
                bassSampleL = (inputSampleL * bassBL[2]) + bassBL[7];
                bassBL[7] = (inputSampleL * bassBL[3]) - (bassSampleL * bassBL[5]) + bassBL[8];
                bassBL[8] = (inputSampleL * bassBL[4]) - (bassSampleL * bassBL[6]);
                trebleSampleR = (inputSampleR * trebleBR[2]) + trebleBR[7];
                trebleBR[7] = (inputSampleR * trebleBR[3]) - (trebleSampleR * trebleBR[5]) + trebleBR[8];
                trebleBR[8] = (inputSampleR * trebleBR[4]) - (trebleSampleR * trebleBR[6]);
                trebleSampleR = inputSampleR - trebleSampleR;
                bassSampleR = (inputSampleR * bassBR[2]) + bassBR[7];
                bassBR[7] = (inputSampleR * bassBR[3]) - (bassSampleR * bassBR[5]) + bassBR[8];
                bassBR[8] = (inputSampleR * bassBR[4]) - (bassSampleR * bassBR[6]);
            }
            flip = !flip;
            trebleSampleL *= trebleGain;
            bassSampleL *= bassGain;
            inputSampleL = bassSampleL + trebleSampleL; // interleaved biquad
            trebleSampleR *= trebleGain;
            bassSampleR *= bassGain;
            inputSampleR = bassSampleR + trebleSampleR; // interleaved biquad
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleR = asin(inputSampleR);
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::baxandall
