#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::air {

constexpr std::string_view k_name{ "Air" };
constexpr std::string_view k_short_description{
    "Air is a different bright EQ from any other. Requires 44.1K."
};
constexpr std::string_view k_long_description{
    "See Air2 for more on this. Air is the same, but simpler, and restricted to one sample rate so that if you're at a higher sample rate, the effects will shift up in frequency, possibly making them inaudible."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Air final : public Effect<T>
{
    double airPrevAL;
    double airEvenAL;
    double airOddAL;
    double airFactorAL;
    double airPrevBL;
    double airEvenBL;
    double airOddBL;
    double airFactorBL;
    double airPrevCL;
    double airEvenCL;
    double airOddCL;
    double airFactorCL;
    double tripletPrevL;
    double tripletMidL;
    double tripletAL;
    double tripletBL;
    double tripletCL;
    double tripletFactorL;
    double airPrevAR;
    double airEvenAR;
    double airOddAR;
    double airFactorAR;
    double airPrevBR;
    double airEvenBR;
    double airOddBR;
    double airFactorBR;
    double airPrevCR;
    double airEvenCR;
    double airOddCR;
    double airFactorCR;
    double tripletPrevR;
    double tripletMidR;
    double tripletAR;
    double tripletBR;
    double tripletCR;
    double tripletFactorR;
    bool flipA;
    bool flipB;
    bool flop;
    int count;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D; // parameters. Always 0-1, and we scale/alter them elsewhere.
    float E;
    float F;

  public:
    Air()
    {
        airPrevAL = 0.0;
        airEvenAL = 0.0;
        airOddAL = 0.0;
        airFactorAL = 0.0;
        airPrevBL = 0.0;
        airEvenBL = 0.0;
        airOddBL = 0.0;
        airFactorBL = 0.0;
        airPrevCL = 0.0;
        airEvenCL = 0.0;
        airOddCL = 0.0;
        airFactorCL = 0.0;
        tripletPrevL = 0.0;
        tripletMidL = 0.0;
        tripletAL = 0.0;
        tripletBL = 0.0;
        tripletCL = 0.0;
        tripletFactorL = 0.0;
        airPrevAR = 0.0;
        airEvenAR = 0.0;
        airOddAR = 0.0;
        airFactorAR = 0.0;
        airPrevBR = 0.0;
        airEvenBR = 0.0;
        airOddBR = 0.0;
        airFactorBR = 0.0;
        airPrevCR = 0.0;
        airEvenCR = 0.0;
        airOddCR = 0.0;
        airFactorCR = 0.0;
        tripletPrevR = 0.0;
        tripletMidR = 0.0;
        tripletAR = 0.0;
        tripletBR = 0.0;
        tripletCR = 0.0;
        tripletFactorR = 0.0;
        flipA = false;
        flipB = false;
        flop = false;
        count = 1;
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        E = 1.0;
        F = 1.0;
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
        kParamE = 4,
        kParamF = 5,
        kNumParameters = 6
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

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
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "22k tap"; break;
            case kParamB: return "15k tap"; break;
            case kParamC: return "11k tap"; break;
            case kParamD: return "filters q"; break;
            case kParamE: return "output level"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "22K tap"; break;
            case kParamB: return "15K tap"; break;
            case kParamC: return "11K tap"; break;
            case kParamD: return "filters Q"; break;
            case kParamE: return "Output Level"; break;
            case kParamF: return "Dry/Wet"; break;

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
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return " "; break;
            case kParamC: return " "; break;
            case kParamD: return " "; break;
            case kParamE: return " "; break;
            case kParamF: return " "; break;

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

        double hiIntensity = -pow(((A * 2.0) - 1.0), 3) * 2;
        double tripletintensity = -pow(((B * 2.0) - 1.0), 3);
        double airIntensity = -pow(((C * 2.0) - 1.0), 3) / 2;
        double filterQ = 2.1 - D;
        double output = E;
        double wet = F;
        double dry = 1.0 - wet;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double correctionL;
        double correctionR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            correctionL = 0.0;
            correctionR = 0.0; // from here on down, please add L and R to the code
            if (count < 1 || count > 3) {
                count = 1;
            }
            tripletFactorL = tripletPrevL - inputSampleL;
            tripletFactorR = tripletPrevR - inputSampleR;
            switch (count)
            {
                case 1:
                    tripletAL += tripletFactorL;
                    tripletCL -= tripletFactorL;
                    tripletFactorL = tripletAL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletAR += tripletFactorR;
                    tripletCR -= tripletFactorR;
                    tripletFactorR = tripletAR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
                case 2:
                    tripletBL += tripletFactorL;
                    tripletAL -= tripletFactorL;
                    tripletFactorL = tripletBL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletBR += tripletFactorR;
                    tripletAR -= tripletFactorR;
                    tripletFactorR = tripletBR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
                case 3:
                    tripletCL += tripletFactorL;
                    tripletBL -= tripletFactorL;
                    tripletFactorL = tripletCL * tripletintensity;
                    tripletPrevL = tripletMidL;
                    tripletMidL = inputSampleL;
                    tripletCR += tripletFactorR;
                    tripletBR -= tripletFactorR;
                    tripletFactorR = tripletCR * tripletintensity;
                    tripletPrevR = tripletMidR;
                    tripletMidR = inputSampleR;
                    break;
            }
            tripletAL /= filterQ;
            tripletBL /= filterQ;
            tripletCL /= filterQ;
            correctionL = correctionL + tripletFactorL;
            tripletAR /= filterQ;
            tripletBR /= filterQ;
            tripletCR /= filterQ;
            correctionR = correctionR + tripletFactorR;
            count++;
            // finished Triplet section- 15K
            if (flop)
            {
                airFactorAL = airPrevAL - inputSampleL;
                airFactorAR = airPrevAR - inputSampleR;
                if (flipA)
                {
                    airEvenAL += airFactorAL;
                    airOddAL -= airFactorAL;
                    airFactorAL = airEvenAL * airIntensity;
                    airEvenAR += airFactorAR;
                    airOddAR -= airFactorAR;
                    airFactorAR = airEvenAR * airIntensity;
                }
                else
                {
                    airOddAL += airFactorAL;
                    airEvenAL -= airFactorAL;
                    airFactorAL = airOddAL * airIntensity;
                    airOddAR += airFactorAR;
                    airEvenAR -= airFactorAR;
                    airFactorAR = airOddAR * airIntensity;
                }
                airOddAL = (airOddAL - ((airOddAL - airEvenAL) / 256.0)) / filterQ;
                airEvenAL = (airEvenAL - ((airEvenAL - airOddAL) / 256.0)) / filterQ;
                airPrevAL = inputSampleL;
                correctionL = correctionL + airFactorAL;
                airOddAR = (airOddAR - ((airOddAR - airEvenAR) / 256.0)) / filterQ;
                airEvenAR = (airEvenAR - ((airEvenAR - airOddAR) / 256.0)) / filterQ;
                airPrevAR = inputSampleR;
                correctionR = correctionR + airFactorAR;
                flipA = !flipA;
            }
            else
            {
                airFactorBL = airPrevBL - inputSampleL;
                airFactorBR = airPrevBR - inputSampleR;
                if (flipB)
                {
                    airEvenBL += airFactorBL;
                    airOddBL -= airFactorBL;
                    airFactorBL = airEvenBL * airIntensity;
                    airEvenBR += airFactorBR;
                    airOddBR -= airFactorBR;
                    airFactorBR = airEvenBR * airIntensity;
                }
                else
                {
                    airOddBL += airFactorBL;
                    airEvenBL -= airFactorBL;
                    airFactorBL = airOddBL * airIntensity;
                    airOddBR += airFactorBR;
                    airEvenBR -= airFactorBR;
                    airFactorBR = airOddBR * airIntensity;
                }
                airOddBL = (airOddBL - ((airOddBL - airEvenBL) / 256.0)) / filterQ;
                airEvenBL = (airEvenBL - ((airEvenBL - airOddBL) / 256.0)) / filterQ;
                airPrevBL = inputSampleL;
                correctionL = correctionL + airFactorBL;
                airOddBR = (airOddBR - ((airOddBR - airEvenBR) / 256.0)) / filterQ;
                airEvenBR = (airEvenBR - ((airEvenBR - airOddBR) / 256.0)) / filterQ;
                airPrevBR = inputSampleR;
                correctionR = correctionR + airFactorBR;
                flipB = !flipB;
            }
            // 11K one
            airFactorCL = airPrevCL - inputSampleL;
            airFactorCR = airPrevCR - inputSampleR;
            if (flop)
            {
                airEvenCL += airFactorCL;
                airOddCL -= airFactorCL;
                airFactorCL = airEvenCL * hiIntensity;
                airEvenCR += airFactorCR;
                airOddCR -= airFactorCR;
                airFactorCR = airEvenCR * hiIntensity;
            }
            else
            {
                airOddCL += airFactorCL;
                airEvenCL -= airFactorCL;
                airFactorCL = airOddCL * hiIntensity;
                airOddCR += airFactorCR;
                airEvenCR -= airFactorCR;
                airFactorCR = airOddCR * hiIntensity;
            }
            airOddCL = (airOddCL - ((airOddCL - airEvenCL) / 256.0)) / filterQ;
            airEvenCL = (airEvenCL - ((airEvenCL - airOddCL) / 256.0)) / filterQ;
            airPrevCL = inputSampleL;
            correctionL = correctionL + airFactorCL;
            airOddCR = (airOddCR - ((airOddCR - airEvenCR) / 256.0)) / filterQ;
            airEvenCR = (airEvenCR - ((airEvenCR - airOddCR) / 256.0)) / filterQ;
            airPrevCR = inputSampleR;
            correctionR = correctionR + airFactorCR;
            flop = !flop;
            inputSampleL += correctionL;
            inputSampleR += correctionR;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
            }
            // nice little output stage template: if we have another scale of floating point
            // number, we really don't want to meaninglessly multiply that by 1.0.
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
} // namespace airwindohhs::air
