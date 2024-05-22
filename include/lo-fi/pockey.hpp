#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Pockey final : public Effect<T>
{
    const std::string m_name{ "Pockey" };

    double lastSampleL;
    double heldSampleL;
    double lastSoftenL;
    double positionL;
    double lastSampleR;
    double heldSampleR;
    double lastSoftenR;
    double positionR;
    double freq;
    double freqA;
    double freqB;
    double rez;
    double rezA;
    double rezB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    Pockey()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        lastSampleL = 0.0;
        heldSampleL = 0.0;
        lastSoftenL = 0.0;
        positionL = 0.0;
        lastSampleR = 0.0;
        heldSampleR = 0.0;
        lastSoftenR = 0.0;
        positionR = 0.0;
        freq = 0.5;
        freqA = 0.5;
        freqB = 0.5;
        rez = 0.5;
        rezA = 0.5;
        rezB = 0.5;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "DeFreq";
            case kParamB: return "DeRez";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double freqMin = 0.08 / overallscale;
        freqA = freqB;
        freqB = (pow(1.0 - A, 3) * (0.618033988749894848204586 - freqMin)) + freqMin;
        // freq is always engaged at least a little
        rezA = rezB;
        rezB = pow(B * 0.618033988749894848204586, 3) + 0.000244140625;
        // rez is always at least 12 bit truncation
        double wet = C;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double freq = (freqA * temp) + (freqB * (1.0 - temp));
            double rez = (rezA * temp) + (rezB * (1.0 - temp));
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = log(1.0 + (255 * fabs(inputSampleL))) / log(255);
            }
            if (inputSampleL < 0) {
                inputSampleL = -log(1.0 + (255 * fabs(inputSampleL))) / log(255);
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleR > 0) {
                inputSampleR = log(1.0 + (255 * fabs(inputSampleR))) / log(255);
            }
            if (inputSampleR < 0) {
                inputSampleR = -log(1.0 + (255 * fabs(inputSampleR))) / log(255);
            }
            // end uLaw encode
            double offset = inputSampleL;
            if (inputSampleL > 0)
            {
                while (offset > 0) {
                    offset -= rez;
                }
                inputSampleL -= offset;
                // it's below 0 so subtracting adds the remainder
            }
            if (inputSampleL < 0)
            {
                while (offset < 0) {
                    offset += rez;
                }
                inputSampleL -= offset;
                // it's above 0 so subtracting subtracts the remainder
            }
            inputSampleL *= (1.0 - rez);
            offset = inputSampleR;
            if (inputSampleR > 0)
            {
                while (offset > 0) {
                    offset -= rez;
                }
                inputSampleR -= offset;
                // it's below 0 so subtracting adds the remainder
            }
            if (inputSampleR < 0)
            {
                while (offset < 0) {
                    offset += rez;
                }
                inputSampleR -= offset;
                // it's above 0 so subtracting subtracts the remainder
            }
            inputSampleR *= (1.0 - rez);
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = (pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleL < 0) {
                inputSampleL = -(pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleR > 0) {
                inputSampleR = (pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            if (inputSampleR < 0) {
                inputSampleR = -(pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            // end uLaw decode
            positionL += freq;
            double outputSample = heldSampleL;
            if (positionL > 1.0)
            {
                positionL -= 1.0;
                heldSampleL = (lastSampleL * positionL) + (inputSampleL * (1 - positionL));
                outputSample = (heldSampleL * (1 - positionL)) + (outputSample * positionL);
            }
            inputSampleL = outputSample;
            double slew = fabs(inputSampleL - lastSoftenL) * freq;
            if (slew > 0.5) {
                slew = 0.5;
            }
            inputSampleL = (inputSampleL * slew) + (lastSoftenL * (1.0 - slew));
            // conditional average: only if we actually have brightness
            lastSampleL = drySampleL;
            lastSoftenL = outputSample;
            positionR += freq;
            outputSample = heldSampleR;
            if (positionR > 1.0)
            {
                positionR -= 1.0;
                heldSampleR = (lastSampleR * positionR) + (inputSampleR * (1 - positionR));
                outputSample = (heldSampleR * (1 - positionR)) + (outputSample * positionR);
            }
            inputSampleR = outputSample;
            slew = fabs(inputSampleR - lastSoftenR) * freq;
            if (slew > 0.5) {
                slew = 0.5;
            }
            inputSampleR = (inputSampleR * slew) + (lastSoftenR * (1.0 - slew));
            // conditional average: only if we actually have brightness
            lastSampleR = drySampleR;
            lastSoftenR = outputSample;
            // end Frequency Derez
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
} // namespace airwindohhs
