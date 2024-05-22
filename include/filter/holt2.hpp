#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Holt2 final : public Effect<T>
{
    std::string m_name{ "Holt2" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double previousSampleAL;
    double previousTrendAL;
    double previousSampleBL;
    double previousTrendBL;
    double previousSampleCL;
    double previousTrendCL;
    double previousSampleDL;
    double previousTrendDL;
    double previousSampleEL;
    double previousTrendEL;
    double previousSampleFL;
    double previousTrendFL;
    double previousSampleGL;
    double previousTrendGL;
    double previousSampleHL;
    double previousTrendHL;
    double previousSampleAR;
    double previousTrendAR;
    double previousSampleBR;
    double previousTrendBR;
    double previousSampleCR;
    double previousTrendCR;
    double previousSampleDR;
    double previousTrendDR;
    double previousSampleER;
    double previousTrendER;
    double previousSampleFR;
    double previousTrendFR;
    double previousSampleGR;
    double previousTrendGR;
    double previousSampleHR;
    double previousTrendHR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5

    };

  public:
    Holt2()
    {
        A = 1.0;
        B = 0.5;
        C = 1.0;
        D = 1.0;
        E = 1.0;
        previousSampleAL = 0.0;
        previousTrendAL = 0.0;
        previousSampleBL = 0.0;
        previousTrendBL = 0.0;
        previousSampleCL = 0.0;
        previousTrendCL = 0.0;
        previousSampleDL = 0.0;
        previousTrendDL = 0.0;
        previousSampleEL = 0.0;
        previousTrendEL = 0.0;
        previousSampleFL = 0.0;
        previousTrendFL = 0.0;
        previousSampleGL = 0.0;
        previousTrendGL = 0.0;
        previousSampleHL = 0.0;
        previousTrendHL = 0.0;
        previousSampleAR = 0.0;
        previousTrendAR = 0.0;
        previousSampleBR = 0.0;
        previousTrendBR = 0.0;
        previousSampleCR = 0.0;
        previousTrendCR = 0.0;
        previousSampleDR = 0.0;
        previousTrendDR = 0.0;
        previousSampleER = 0.0;
        previousTrendER = 0.0;
        previousSampleFR = 0.0;
        previousTrendFR = 0.0;
        previousSampleGR = 0.0;
        previousTrendGR = 0.0;
        previousSampleHR = 0.0;
        previousTrendHR = 0.0;
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Freq";
            case kParamB: return "Reso";
            case kParamC: return "Poles";
            case kParamD: return "Output";
            case kParamE: return "Dry/Wet";

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
            case kParamC: return std::to_string(C * 8.0);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

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
        double alpha = pow(A, 4) + 0.00001;
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        double resControl = (B * 0.15) + 0.12;
        double beta = (alpha * pow(resControl, 2));
        // 0.27 max resonance for full stages on white noise keeping below 0dB
        // 0.12 min resonance for not losing all the level as we go down
        // as we remove the 'avoid zero' +0.00001 on beta, our subsonic stability improves
        alpha += ((1.0 - beta) * pow(A, 3)); // correct for droop in frequency
        if (alpha > 1.0) {
            alpha = 1.0;
        }
        double trend;
        double forecast; // defining these here because we're copying the routine eight times
        double aWet = 0.0;
        double bWet = 0.0;
        double cWet = 0.0;
        double dWet = 0.0;
        double eWet = 0.0;
        double fWet = 0.0;
        double gWet = 0.0;
        double hWet = C * 8.0;
        // eight-stage wet/dry control using progressive stages that bypass when not engaged
        if (hWet < 1.0) {
            aWet = hWet;
            hWet = 0.0;
        }
        else if (hWet < 2.0) {
            bWet = hWet - 1.0;
            aWet = 1.0;
            hWet = 0.0;
        }
        else if (hWet < 3.0) {
            cWet = hWet - 2.0;
            bWet = aWet = 1.0;
            hWet = 0.0;
        }
        else if (hWet < 4.0) {
            dWet = hWet - 3.0;
            cWet = bWet = aWet = 1.0;
            hWet = 0.0;
        }
        else if (hWet < 5.0) {
            eWet = hWet - 4.0;
            dWet = cWet = bWet = aWet = 1.0;
            hWet = 0.0;
        }
        else if (hWet < 6.0) {
            fWet = hWet - 5.0;
            eWet = dWet = cWet = bWet = aWet = 1.0;
            hWet = 0.0;
        }
        else if (hWet < 7.0) {
            gWet = hWet - 6.0;
            fWet = eWet = dWet = cWet = bWet = aWet = 1.0;
            hWet = 0.0;
        }
        else {
            hWet -= 7.0;
            gWet = fWet = eWet = dWet = cWet = bWet = aWet = 1.0;
        }
        // this is one way to make a little set of dry/wet stages that are successively added to the
        // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        // beyond that point: this is a way to progressively add a 'black box' sound processing
        // which lets you fall through to simpler processing at lower settings.
        double gain = D;
        double wet = E;
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
            if (aWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleAL) + ((0.999 - beta) * previousTrendAL));
                forecast = previousSampleAL + previousTrendAL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleAL = inputSampleL;
                previousTrendAL = trend;
                inputSampleL = (inputSampleL * aWet) + (drySampleL * (1.0 - aWet));
                trend = (beta * (inputSampleR - previousSampleAR) + ((0.999 - beta) * previousTrendAR));
                forecast = previousSampleAR + previousTrendAR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleAR = inputSampleR;
                previousTrendAR = trend;
                inputSampleR = (inputSampleR * aWet) + (drySampleR * (1.0 - aWet));
            }
            if (bWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleBL) + ((0.999 - beta) * previousTrendBL));
                forecast = previousSampleBL + previousTrendBL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleBL = inputSampleL;
                previousTrendBL = trend;
                inputSampleL = (inputSampleL * bWet) + (previousSampleAL * (1.0 - bWet));
                trend = (beta * (inputSampleR - previousSampleBR) + ((0.999 - beta) * previousTrendBR));
                forecast = previousSampleBR + previousTrendBR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleBR = inputSampleR;
                previousTrendBR = trend;
                inputSampleR = (inputSampleR * bWet) + (previousSampleAR * (1.0 - bWet));
            }
            if (cWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleCL) + ((0.999 - beta) * previousTrendCL));
                forecast = previousSampleCL + previousTrendCL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleCL = inputSampleL;
                previousTrendCL = trend;
                inputSampleL = (inputSampleL * cWet) + (previousSampleBL * (1.0 - cWet));
                trend = (beta * (inputSampleR - previousSampleCR) + ((0.999 - beta) * previousTrendCR));
                forecast = previousSampleCR + previousTrendCR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleCR = inputSampleR;
                previousTrendCR = trend;
                inputSampleR = (inputSampleR * cWet) + (previousSampleBR * (1.0 - cWet));
            }
            if (dWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleDL) + ((0.999 - beta) * previousTrendDL));
                forecast = previousSampleDL + previousTrendDL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleDL = inputSampleL;
                previousTrendDL = trend;
                inputSampleL = (inputSampleL * dWet) + (previousSampleCL * (1.0 - dWet));
                trend = (beta * (inputSampleR - previousSampleDR) + ((0.999 - beta) * previousTrendDR));
                forecast = previousSampleDR + previousTrendDR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleDR = inputSampleR;
                previousTrendDR = trend;
                inputSampleR = (inputSampleR * dWet) + (previousSampleCR * (1.0 - dWet));
            }
            if (eWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleEL) + ((0.999 - beta) * previousTrendEL));
                forecast = previousSampleEL + previousTrendEL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleEL = inputSampleL;
                previousTrendEL = trend;
                inputSampleL = (inputSampleL * eWet) + (previousSampleDL * (1.0 - eWet));
                trend = (beta * (inputSampleR - previousSampleER) + ((0.999 - beta) * previousTrendER));
                forecast = previousSampleER + previousTrendER;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleER = inputSampleR;
                previousTrendER = trend;
                inputSampleR = (inputSampleR * eWet) + (previousSampleDR * (1.0 - eWet));
            }
            if (fWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleFL) + ((0.999 - beta) * previousTrendFL));
                forecast = previousSampleFL + previousTrendFL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleFL = inputSampleL;
                previousTrendFL = trend;
                inputSampleL = (inputSampleL * fWet) + (previousSampleEL * (1.0 - fWet));
                trend = (beta * (inputSampleR - previousSampleFR) + ((0.999 - beta) * previousTrendFR));
                forecast = previousSampleFR + previousTrendFR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleFR = inputSampleR;
                previousTrendFR = trend;
                inputSampleR = (inputSampleR * fWet) + (previousSampleER * (1.0 - fWet));
            }
            if (gWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleGL) + ((0.999 - beta) * previousTrendGL));
                forecast = previousSampleGL + previousTrendGL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleGL = inputSampleL;
                previousTrendGL = trend;
                inputSampleL = (inputSampleL * gWet) + (previousSampleFL * (1.0 - gWet));
                trend = (beta * (inputSampleR - previousSampleGR) + ((0.999 - beta) * previousTrendGR));
                forecast = previousSampleGR + previousTrendGR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleGR = inputSampleR;
                previousTrendGR = trend;
                inputSampleR = (inputSampleR * gWet) + (previousSampleFR * (1.0 - gWet));
            }
            if (hWet > 0.0) {
                trend = (beta * (inputSampleL - previousSampleHL) + ((0.999 - beta) * previousTrendHL));
                forecast = previousSampleHL + previousTrendHL;
                inputSampleL = (alpha * inputSampleL) + ((0.999 - alpha) * forecast);
                previousSampleHL = inputSampleL;
                previousTrendHL = trend;
                inputSampleL = (inputSampleL * hWet) + (previousSampleGL * (1.0 - hWet));
                trend = (beta * (inputSampleR - previousSampleHR) + ((0.999 - beta) * previousTrendHR));
                forecast = previousSampleHR + previousTrendHR;
                inputSampleR = (alpha * inputSampleR) + ((0.999 - alpha) * forecast);
                previousSampleHR = inputSampleR;
                previousTrendHR = trend;
                inputSampleR = (inputSampleR * hWet) + (previousSampleGR * (1.0 - hWet));
            }
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            if (wet < 1.0) {
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
} // namespace airwindohhs
