#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Texturize final : public Effect<T>
{
    const std::string m_name{ "Texturize" };

    bool polarityL;
    double lastSampleL;
    double iirSampleL;
    double noiseAL;
    double noiseBL;
    double noiseCL;
    bool flipL;
    bool polarityR;
    double lastSampleR;
    double iirSampleR;
    double noiseAR;
    double noiseBR;
    double noiseCR;
    bool flipR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParampolarityL = 0,
        kParamlastSampleL = 1,
        kParamiirSampleL = 2,
        kNumParameters = 3

    };

  public:
    Texturize()
    {
        polarityL = false;
        lastSampleL = 0.0;
        iirSampleL = 0.0;
        noiseAL = 0.0;
        noiseBL = 0.0;
        noiseCL = 0.0;
        flipL = true;
        polarityR = false;
        lastSampleR = 0.0;
        iirSampleR = 0.0;
        noiseAR = 0.0;
        noiseBR = 0.0;
        noiseCR = 0.0;
        flipR = true;
        A = 0.5;
        B = 0.5;
        C = 0.5;
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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: polarityL = value; break;
            case kParamlastSampleL: lastSampleL = value; break;
            case kParamiirSampleL: iirSampleL = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return polarityL;
            case kParamlastSampleL: return lastSampleL;
            case kParamiirSampleL: return iirSampleL;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return "Bright";
            case kParamlastSampleL: return "Punchy";
            case kParamiirSampleL: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return std::to_string(A);
            case kParamlastSampleL: return std::to_string(B);
            case kParamiirSampleL: return std::to_string(C);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParampolarityL: return "";
            case kParamlastSampleL: return "";
            case kParamiirSampleL: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double slewAmount = ((pow(A, 2.0) * 4.0) + 0.71) / overallscale;
        double dynAmount = pow(B, 2.0);
        double wet = pow(C, 5);
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
            if (inputSampleL < 0) {
                if (polarityL == true) {
                    if (noiseAL < 0) {
                        flipL = true;
                    }
                    else {
                        flipL = false;
                    }
                }
                polarityL = false;
            }
            else {
                polarityL = true;
            }
            if (inputSampleR < 0) {
                if (polarityR == true) {
                    if (noiseAR < 0) {
                        flipR = true;
                    }
                    else {
                        flipR = false;
                    }
                }
                polarityR = false;
            }
            else {
                polarityR = true;
            }
            if (flipL) {
                noiseAL += (double(fpdL) / UINT32_MAX);
            }
            else {
                noiseAL -= (double(fpdL) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipL = !flipL;
            if (flipR) {
                noiseAR += (double(fpdR) / UINT32_MAX);
            }
            else {
                noiseAR -= (double(fpdR) / UINT32_MAX);
            }
            // here's the guts of the random walk
            flipR = !flipR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (dynAmount < 0.4999) {
                inputSampleL = (inputSampleL * dynAmount * 2.0) + (sin(inputSampleL) * (1.0 - (dynAmount * 2.0)));
                inputSampleR = (inputSampleR * dynAmount * 2.0) + (sin(inputSampleR) * (1.0 - (dynAmount * 2.0)));
            }
            if (dynAmount > 0.5001) {
                inputSampleL = (asin(inputSampleL) * ((dynAmount * 2.0) - 1.0)) + (inputSampleL * (1.0 - ((dynAmount * 2.0) - 1.0)));
                inputSampleR = (asin(inputSampleR) * ((dynAmount * 2.0) - 1.0)) + (inputSampleR * (1.0 - ((dynAmount * 2.0) - 1.0)));
            }
            // doing this in two steps means I get to not run an extra sin/asin function per sample
            noiseBL = sin(noiseAL * (0.2 - (dynAmount * 0.125)) * fabs(inputSampleL));
            noiseBR = sin(noiseAR * (0.2 - (dynAmount * 0.125)) * fabs(inputSampleR));
            double slewL = fabs(inputSampleL - lastSampleL) * slewAmount;
            lastSampleL = inputSampleL * (0.86 - (dynAmount * 0.125));
            if (slewL > 1.0) {
                slewL = 1.0;
            }
            double iirIntensityL = slewL;
            iirIntensityL *= 2.472;
            iirIntensityL *= iirIntensityL;
            if (iirIntensityL > 1.0) {
                iirIntensityL = 1.0;
            }
            double slewR = fabs(inputSampleR - lastSampleR) * slewAmount;
            lastSampleR = inputSampleR * (0.86 - (dynAmount * 0.125));
            if (slewR > 1.0) {
                slewR = 1.0;
            }
            double iirIntensityR = slewR;
            iirIntensityR *= 2.472;
            iirIntensityR *= iirIntensityR;
            if (iirIntensityR > 1.0) {
                iirIntensityR = 1.0;
            }
            iirSampleL = (iirSampleL * (1.0 - iirIntensityL)) + (noiseBL * iirIntensityL);
            noiseBL = iirSampleL;
            noiseBL = (noiseBL * slewL) + (noiseCL * (1.0 - slewL));
            noiseCL = noiseBL;
            iirSampleR = (iirSampleR * (1.0 - iirIntensityR)) + (noiseBR * iirIntensityR);
            noiseBR = iirSampleR;
            noiseBR = (noiseBR * slewR) + (noiseCR * (1.0 - slewR));
            noiseCR = noiseBR;
            inputSampleL = (noiseCL * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (noiseCR * wet) + (drySampleR * (1.0 - wet));
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
