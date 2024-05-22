#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Tube final : public Effect<T>
{
    const std::string m_name{ "Tube" };

    double previousSampleA;
    double previousSampleB;
    double previousSampleC;
    double previousSampleD;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    Tube()
    {
        A = 0.0;
        previousSampleA = 0.0;
        previousSampleB = 0.0;
        previousSampleC = 0.0;
        previousSampleD = 0.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "TUBE";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
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
        double gain = 1.0 + (A * 0.2246161992650486);
        // this maxes out at +1.76dB, which is the exact difference between what a triangle/saw wave
        // would be, and a sine (the fullest possible wave at the same peak amplitude). Why do this?
        // Because the nature of this plugin is the 'more FAT TUUUUBE fatness!' knob, and because
        // sticking to this amount maximizes that effect on a 'normal' sound that is itself unclipped
        // while confining the resulting 'clipped' area to what is already 'fattened' into a flat
        // and distorted region. You can always put a gain trim in front of it for more distortion,
        // or cascade them in the DAW for more distortion.
        double iterations = 1.0 - A;
        int powerfactor = (5.0 * iterations) + 1;
        double gainscaling = 1.0 / (double)(powerfactor + 1);
        double outputscaling = 1.0 + (1.0 / (double)(powerfactor));
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
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleA;
                previousSampleA = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleB;
                previousSampleB = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
            inputSampleL *= gain;
            inputSampleR *= gain;
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
            double factor = inputSampleL; // Left channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleL;
            }
            // this applies more and more of a 'curve' to the transfer function
            if ((powerfactor % 2 == 1) && (inputSampleL != 0.0)) {
                factor = (factor / inputSampleL) * fabs(inputSampleL);
            }
            // if we would've got an asymmetrical effect this undoes the last step, and then
            // redoes it using an absolute value to make the effect symmetrical again
            factor *= gainscaling;
            inputSampleL -= factor;
            inputSampleL *= outputscaling;
            factor = inputSampleR; // Right channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleR;
            }
            // this applies more and more of a 'curve' to the transfer function
            if ((powerfactor % 2 == 1) && (inputSampleR != 0.0)) {
                factor = (factor / inputSampleR) * fabs(inputSampleR);
            }
            // if we would've got an asymmetrical effect this undoes the last step, and then
            // redoes it using an absolute value to make the effect symmetrical again
            factor *= gainscaling;
            inputSampleR -= factor;
            inputSampleR *= outputscaling;
            /*  This is the simplest raw form of the 'fattest' TUBE boost between -1.0 and 1.0
             if (inputSample > 1.0) inputSample = 1.0; if (inputSample < -1.0) inputSample = -1.0;
             inputSample = (inputSample-(inputSample*fabs(inputSample)*0.5))*2.0;
             */
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleC;
                previousSampleC = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleD;
                previousSampleD = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
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
