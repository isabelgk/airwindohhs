#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::deckwrecka {

constexpr std::string_view k_name{ "Deckwrecka" };
constexpr std::string_view k_short_description{
    "Deckwrecka fattens and dirties up beats."
};
constexpr std::string_view k_long_description{
    "Back in the day, I was asked by composer Alan Gold to create a special plugin. How special? To give you some idea, you might recognize the name quicker as Agzilla… or the DECKWRECKA. And so, that became the name of the plugin, appearing on the Deckwrecka blog, then lost to time.Until now! Hope this sits well with the eponymous Deckwrecka. It was always free and now it’s doubly free because it’s open source too. Now it’s brought up to date with the most recent Airwindows technologies, and it’s available in VST form for the first time ever. :DSo what exactly is this thing? It’s like a thunderousness overdrive. It’s huge, slamming, dirty bass, like spinning records on a turntable run through 1000 watts and a pile of monster bassbins. Technically it’s like extra bass plus overdrive plus certain types of dirt and grunge all rolled up together into a pile of funk. Or at least that’s the endeavour.You can use it how you please, but you can throw it on kick drums for EDM and hip-hop, or whatever elements need to be more beefy and sub-rattling."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Deckwrecka final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    int bflip;
    double iirHeadBumpAL;
    double iirHeadBumpBL;
    double iirHeadBumpCL;
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleGL;
    double iirSampleHL;
    double iirSampleIL;
    double iirSampleJL;
    double iirSampleKL;
    double iirSampleLL;
    double iirSampleML;
    double iirSampleNL;
    double iirSampleOL;
    double iirSamplePL;
    double iirSampleQL;
    double iirSampleRL;
    double iirSampleSL;
    double iirSampleTL;
    double iirSampleUL;
    double iirSampleVL;
    double iirSampleWL;
    double iirSampleXL;
    double iirSampleYL;
    double iirSampleZL;
    double iirHeadBumpAR;
    double iirHeadBumpBR;
    double iirHeadBumpCR;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double iirSampleGR;
    double iirSampleHR;
    double iirSampleIR;
    double iirSampleJR;
    double iirSampleKR;
    double iirSampleLR;
    double iirSampleMR;
    double iirSampleNR;
    double iirSampleOR;
    double iirSamplePR;
    double iirSampleQR;
    double iirSampleRR;
    double iirSampleSR;
    double iirSampleTR;
    double iirSampleUR;
    double iirSampleVR;
    double iirSampleWR;
    double iirSampleXR;
    double iirSampleYR;
    double iirSampleZR;
    float A;

  public:
    Deckwrecka()
    {
        A = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        bflip = 1;
        iirHeadBumpAL = 0.0;
        iirHeadBumpBL = 0.0;
        iirHeadBumpCL = 0.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleGL = 0.0;
        iirSampleHL = 0.0;
        iirSampleIL = 0.0;
        iirSampleJL = 0.0;
        iirSampleKL = 0.0;
        iirSampleLL = 0.0;
        iirSampleML = 0.0;
        iirSampleNL = 0.0;
        iirSampleOL = 0.0;
        iirSamplePL = 0.0;
        iirSampleQL = 0.0;
        iirSampleRL = 0.0;
        iirSampleSL = 0.0;
        iirSampleTL = 0.0;
        iirSampleUL = 0.0;
        iirSampleVL = 0.0;
        iirSampleWL = 0.0;
        iirSampleXL = 0.0;
        iirSampleYL = 0.0;
        iirSampleZL = 0.0;
        iirHeadBumpAR = 0.0;
        iirHeadBumpBR = 0.0;
        iirHeadBumpCR = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        iirSampleGR = 0.0;
        iirSampleHR = 0.0;
        iirSampleIR = 0.0;
        iirSampleJR = 0.0;
        iirSampleKR = 0.0;
        iirSampleLR = 0.0;
        iirSampleMR = 0.0;
        iirSampleNR = 0.0;
        iirSampleOR = 0.0;
        iirSamplePR = 0.0;
        iirSampleQR = 0.0;
        iirSampleRR = 0.0;
        iirSampleSR = 0.0;
        iirSampleTR = 0.0;
        iirSampleUR = 0.0;
        iirSampleVR = 0.0;
        iirSampleWR = 0.0;
        iirSampleXR = 0.0;
        iirSampleYR = 0.0;
        iirSampleZR = 0.0;
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
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
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
            return "wreck";
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
            return "Wreck";
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
        double wreck = pow(A, 2);
        double randyL;
        double randyR;
        double HeadBumpL = 0.0;
        double HeadBumpR = 0.0;
        double HeadBumpFreq = 0.01 / overallscale;
        double iirHPAmount = 0.0000014 / overallscale;
        double altHPAmount = 1.0 - iirHPAmount;
        double tempSample;
        double bridgerectifier;
        double correction;
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
            bflip++;
            if (bflip < 1 || bflip > 3) {
                bflip = 1;
            }
            randyL = (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            randyL += (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            randyL += (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            randyL += (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            randyL += (double(fpdL) / UINT32_MAX);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            randyL += (double(fpdL) / UINT32_MAX);
            randyL /= 6.0;
            randyL *= wreck; // 0 to 1 the noise, may not be needed
            // set up the noise
            randyR = (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            randyR += (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            randyR += (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            randyR += (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            randyR += (double(fpdR) / UINT32_MAX);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            randyR += (double(fpdR) / UINT32_MAX);
            randyR /= 6.0;
            randyR *= wreck; // 0 to 1 the noise, may not be needed
            // set up the noise
            switch (bflip)
            {
                case 1:
                    iirHeadBumpAL += (inputSampleL * randyL);
                    iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
                    iirHeadBumpAR += (inputSampleR * randyR);
                    iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
                    break;
                case 2:
                    iirHeadBumpBL += (inputSampleL * randyL);
                    iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
                    iirHeadBumpBR += (inputSampleR * randyR);
                    iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
                    break;
                case 3:
                    iirHeadBumpCL += (inputSampleL * randyL);
                    iirHeadBumpCL -= (iirHeadBumpCL * iirHeadBumpCL * iirHeadBumpCL * HeadBumpFreq);
                    iirHeadBumpCR += (inputSampleR * randyR);
                    iirHeadBumpCR -= (iirHeadBumpCR * iirHeadBumpCR * iirHeadBumpCR * HeadBumpFreq);
                    break;
            }
            HeadBumpL = iirHeadBumpAL + iirHeadBumpBL + iirHeadBumpCL;
            HeadBumpR = iirHeadBumpAR + iirHeadBumpBR + iirHeadBumpCR;
            tempSample = HeadBumpL;
            iirSampleAL = (iirSampleAL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleAL;
            correction = iirSampleAL;
            iirSampleBL = (iirSampleBL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleBL;
            correction += iirSampleBL;
            iirSampleCL = (iirSampleCL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleCL;
            correction += iirSampleCL;
            iirSampleDL = (iirSampleDL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleDL;
            correction += iirSampleDL;
            iirSampleEL = (iirSampleEL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleEL;
            correction += iirSampleEL;
            iirSampleFL = (iirSampleFL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleFL;
            correction += iirSampleFL;
            iirSampleGL = (iirSampleGL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleGL;
            correction += iirSampleGL;
            iirSampleHL = (iirSampleHL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleHL;
            correction += iirSampleHL;
            iirSampleIL = (iirSampleIL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleIL;
            correction += iirSampleIL;
            iirSampleJL = (iirSampleJL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleJL;
            correction += iirSampleJL;
            iirSampleKL = (iirSampleKL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleKL;
            correction += iirSampleKL;
            iirSampleLL = (iirSampleLL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleLL;
            correction += iirSampleLL;
            iirSampleML = (iirSampleML * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleML;
            correction += iirSampleML;
            iirSampleNL = (iirSampleNL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleNL;
            correction += iirSampleNL;
            iirSampleOL = (iirSampleOL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleOL;
            correction += iirSampleOL;
            iirSamplePL = (iirSamplePL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSamplePL;
            correction += iirSamplePL;
            iirSampleQL = (iirSampleQL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleQL;
            correction += iirSampleQL;
            iirSampleRL = (iirSampleRL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleRL;
            correction += iirSampleRL;
            iirSampleSL = (iirSampleSL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleSL;
            correction += iirSampleSL;
            iirSampleTL = (iirSampleTL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleTL;
            correction += iirSampleTL;
            iirSampleUL = (iirSampleUL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleUL;
            correction += iirSampleUL;
            iirSampleVL = (iirSampleVL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleVL;
            correction += iirSampleVL;
            iirSampleWL = (iirSampleWL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleWL;
            correction += iirSampleWL;
            iirSampleXL = (iirSampleXL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleXL;
            correction += iirSampleXL;
            iirSampleYL = (iirSampleYL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleYL;
            correction += iirSampleYL;
            iirSampleZL = (iirSampleZL * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleZL;
            correction += iirSampleZL;
            // do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
            // numbers being used. Don't keep doing it against the possibly high level signal number.
            HeadBumpL -= correction;
            // apply stored up tiny corrections
            HeadBumpL *= (1.0 - (wreck / 2));
            tempSample = HeadBumpR;
            iirSampleAR = (iirSampleAR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleAR;
            correction = iirSampleAR;
            iirSampleBR = (iirSampleBR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleBR;
            correction += iirSampleBR;
            iirSampleCR = (iirSampleCR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleCR;
            correction += iirSampleCR;
            iirSampleDR = (iirSampleDR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleDR;
            correction += iirSampleDR;
            iirSampleER = (iirSampleER * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleER;
            correction += iirSampleER;
            iirSampleFR = (iirSampleFR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleFR;
            correction += iirSampleFR;
            iirSampleGR = (iirSampleGR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleGR;
            correction += iirSampleGR;
            iirSampleHR = (iirSampleHR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleHR;
            correction += iirSampleHR;
            iirSampleIR = (iirSampleIR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleIR;
            correction += iirSampleIR;
            iirSampleJR = (iirSampleJR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleJR;
            correction += iirSampleJR;
            iirSampleKR = (iirSampleKR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleKR;
            correction += iirSampleKR;
            iirSampleLR = (iirSampleLR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleLR;
            correction += iirSampleLR;
            iirSampleMR = (iirSampleMR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleMR;
            correction += iirSampleMR;
            iirSampleNR = (iirSampleNR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleNR;
            correction += iirSampleNR;
            iirSampleOR = (iirSampleOR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleOR;
            correction += iirSampleOR;
            iirSamplePR = (iirSamplePR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSamplePR;
            correction += iirSamplePR;
            iirSampleQR = (iirSampleQR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleQR;
            correction += iirSampleQR;
            iirSampleRR = (iirSampleRR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleRR;
            correction += iirSampleRR;
            iirSampleSR = (iirSampleSR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleSR;
            correction += iirSampleSR;
            iirSampleTR = (iirSampleTR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleTR;
            correction += iirSampleTR;
            iirSampleUR = (iirSampleUR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleUR;
            correction += iirSampleUR;
            iirSampleVR = (iirSampleVR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleVR;
            correction += iirSampleVR;
            iirSampleWR = (iirSampleWR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleWR;
            correction += iirSampleWR;
            iirSampleXR = (iirSampleXR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleXR;
            correction += iirSampleXR;
            iirSampleYR = (iirSampleYR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleYR;
            correction += iirSampleYR;
            iirSampleZR = (iirSampleZR * altHPAmount) + (tempSample * iirHPAmount);
            tempSample -= iirSampleZR;
            correction += iirSampleZR;
            // do the IIR on a dummy sample, and store up the correction in a variable at the same scale as the very low level
            // numbers being used. Don't keep doing it against the possibly high level signal number.
            HeadBumpR -= correction;
            // apply stored up tiny corrections
            HeadBumpR *= (1.0 - (wreck / 2));
            bridgerectifier = fabs(HeadBumpL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (HeadBumpL > 0.0) {
                HeadBumpL = bridgerectifier;
            }
            else {
                HeadBumpL = -bridgerectifier;
            }
            // clamp HeadBump to -1 to 1 with distortion
            bridgerectifier = fabs(HeadBumpR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (HeadBumpR > 0.0) {
                HeadBumpR = bridgerectifier;
            }
            else {
                HeadBumpR = -bridgerectifier;
            }
            // clamp HeadBump to -1 to 1 with distortion
            inputSampleL *= (1.0 - wreck);
            inputSampleL += (HeadBumpL * wreck);
            inputSampleR *= (1.0 - wreck);
            inputSampleR += (HeadBumpR * wreck);
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
} // namespace airwindohhs::deckwrecka
