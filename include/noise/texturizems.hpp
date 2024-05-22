#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class TexturizeMS final : public Effect<T>
{
    std::string m_name{ "TexturizeMS" };

    bool polarityM;
    double lastSampleM;
    double iirSampleM;
    double noiseAM;
    double noiseBM;
    double noiseCM;
    bool flipM;
    bool polarityS;
    double lastSampleS;
    double iirSampleS;
    double noiseAS;
    double noiseBS;
    double noiseCS;
    bool flipS;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.

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

  public:
    TexturizeMS()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        polarityM = false;
        lastSampleM = 0.0;
        iirSampleM = 0.0;
        noiseAM = 0.0;
        noiseBM = 0.0;
        noiseCM = 0.0;
        flipM = true;
        polarityS = false;
        lastSampleS = 0.0;
        iirSampleS = 0.0;
        noiseAS = 0.0;
        noiseBS = 0.0;
        noiseCS = 0.0;
        flipS = true;
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

    constexpr std::string_view name()
    {
        return m_name;
    }

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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;
            case kParamF: return F;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "BrightM";
            case kParamB: return "PunchyM";
            case kParamC: return "DryWetM";
            case kParamD: return "BrightS";
            case kParamE: return "PunchyS";
            case kParamF: return "DryWetS";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);
            case kParamF: return std::to_string(F);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
            case kParamF: return "";
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
        double slewAmountM = ((pow(A, 2.0) * 4.0) + 0.71) / overallscale;
        double dynAmountM = pow(B, 2.0);
        double wetM = pow(C, 5);
        double slewAmountS = ((pow(D, 2.0) * 4.0) + 0.71) / overallscale;
        double dynAmountS = pow(E, 2.0);
        double wetS = pow(F, 5);
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
            double mid = inputSampleL + inputSampleR;
            double side = inputSampleL - inputSampleR;
            // assign mid and side.Between these sections, you can do mid/side processing
            double drySampleM = mid;
            double drySampleS = side;
            // begin mid
            if (mid < 0) {
                if (polarityM == true) {
                    if (noiseAM < 0) {
                        flipM = true;
                    }
                    else {
                        flipM = false;
                    }
                }
                polarityM = false;
            }
            else {
                polarityM = true;
            }
            if (flipM) {
                noiseAM += (double(fpdL) / UINT32_MAX);
            }
            else {
                noiseAM -= (double(fpdL) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipM = !flipM;
            if (mid > 1.0) {
                mid = 1.0;
            }
            if (mid < -1.0) {
                mid = -1.0;
            }
            if (dynAmountM < 0.4999) {
                mid = (mid * dynAmountM * 2.0) + (sin(mid) * (1.0 - (dynAmountM * 2.0)));
            }
            if (dynAmountM > 0.5001) {
                mid = (asin(mid) * ((dynAmountM * 2.0) - 1.0)) + (mid * (1.0 - ((dynAmountM * 2.0) - 1.0)));
            }
            // doing this in two steps means I get to not run an extra sin/asin function per sample
            noiseBM = sin(noiseAM * (0.2 - (dynAmountM * 0.125)) * fabs(mid));
            double slew = fabs(mid - lastSampleM) * slewAmountM;
            lastSampleM = mid * (0.86 - (dynAmountM * 0.125));
            if (slew > 1.0) {
                slew = 1.0;
            }
            double iirIntensity = slew;
            iirIntensity *= 2.472;
            iirIntensity *= iirIntensity;
            if (iirIntensity > 1.0) {
                iirIntensity = 1.0;
            }
            iirSampleM = (iirSampleM * (1.0 - iirIntensity)) + (noiseBM * iirIntensity);
            noiseBM = iirSampleM;
            noiseBM = (noiseBM * slew) + (noiseCM * (1.0 - slew));
            noiseCM = noiseBM;
            mid = (noiseCM * wetM) + (drySampleM * (1.0 - wetM));
            // end mid
            // begin side
            if (side < 0) {
                if (polarityS == true) {
                    if (noiseAS < 0) {
                        flipS = true;
                    }
                    else {
                        flipS = false;
                    }
                }
                polarityS = false;
            }
            else {
                polarityS = true;
            }
            if (flipS) {
                noiseAS += (double(fpdR) / UINT32_MAX);
            }
            else {
                noiseAS -= (double(fpdR) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipS = !flipS;
            if (side > 1.0) {
                side = 1.0;
            }
            if (side < -1.0) {
                side = -1.0;
            }
            if (dynAmountS < 0.4999) {
                side = (side * dynAmountS * 2.0) + (sin(side) * (1.0 - (dynAmountS * 2.0)));
            }
            if (dynAmountS > 0.5001) {
                side = (asin(side) * ((dynAmountS * 2.0) - 1.0)) + (side * (1.0 - ((dynAmountS * 2.0) - 1.0)));
            }
            // doing this in two steps means I get to not run an extra sin/asin function per sample
            noiseBS = sin(noiseAS * (0.2 - (dynAmountS * 0.125)) * fabs(side));
            slew = fabs(side - lastSampleS) * slewAmountS;
            lastSampleS = side * (0.86 - (dynAmountS * 0.125));
            if (slew > 1.0) {
                slew = 1.0;
            }
            iirIntensity = slew;
            iirIntensity *= 2.472;
            iirIntensity *= iirIntensity;
            if (iirIntensity > 1.0) {
                iirIntensity = 1.0;
            }
            iirSampleS = (iirSampleS * (1.0 - iirIntensity)) + (noiseBS * iirIntensity);
            noiseBS = iirSampleS;
            noiseBS = (noiseBS * slew) + (noiseCS * (1.0 - slew));
            noiseCS = noiseBS;
            side = (noiseCS * wetS) + (drySampleS * (1.0 - wetS));
            // end side
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            // unassign mid and side
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
} // namespace airwindohhs
