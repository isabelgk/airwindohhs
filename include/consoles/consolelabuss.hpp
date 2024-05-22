#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ConsoleLABuss final : public Effect<T>
{
    std::string m_name{ "ConsoleLABuss" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSinewL;
    double lastSinewR;
    double subAL;
    double subAR;
    double subBL;
    double subBR;
    double subCL;
    double subCR;
    double gainA;
    double gainB; // smoothed master fader for channel, from Z2 series filter code
    float A;

    enum params
    {
        kParamA = 0,
        kNumParameters = 1

    };

  public:
    ConsoleLABuss()
    {
        A = 1.0;
        lastSinewL = lastSinewR = 0.0;
        subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
        gainA = gainB = 1.0;
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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Master";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
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
        overallscale *= getSampleRate();
        gainA = gainB;
        gainB = sqrt(A); // smoothed master fader from Z2 filters
        // this will be applied three times: this is to make the various tone alterations
        // hit differently at different master fader drive levels.
        // in particular, backing off the master fader tightens the super lows
        // but opens up the modified Sinew, because more of the attentuation happens before
        // you even get to slew clipping :) and if the fader is not active, it bypasses completely.
        double threshSinew = 0.718 / overallscale;
        double subTrim = 0.0011 / overallscale;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double gain = (gainA * temp) + (gainB * (1.0 - temp));
            // setting up smoothed master fader
            // begin SubTight section
            double subSampleL = inputSampleL * subTrim;
            double subSampleR = inputSampleR * subTrim;
            double scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subAL + (sin(subAL - subSampleL) * scale));
            subAL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subAR + (sin(subAR - subSampleR) * scale));
            subAR = subSampleR * scale;
            scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subBL + (sin(subBL - subSampleL) * scale));
            subBL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subBR + (sin(subBR - subSampleR) * scale));
            subBR = subSampleR * scale;
            scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subCL + (sin(subCL - subSampleL) * scale));
            subCL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subCR + (sin(subCR - subSampleR) * scale));
            subCR = subSampleR * scale;
            if (subSampleL > 0.25) {
                subSampleL = 0.25;
            }
            if (subSampleL < -0.25) {
                subSampleL = -0.25;
            }
            if (subSampleR > 0.25) {
                subSampleR = 0.25;
            }
            if (subSampleR < -0.25) {
                subSampleR = -0.25;
            }
            inputSampleL -= (subSampleL * 16.0);
            inputSampleR -= (subSampleR * 16.0);
            // end SubTight section
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            } // if using the master fader, we are going to attenuate three places.
            // subtight is always fully engaged: tighten response when restraining full console
            // begin ConsoleZeroBuss which is the one we choose for ConsoleLA
            if (inputSampleL > 2.8) {
                inputSampleL = 2.8;
            }
            if (inputSampleL < -2.8) {
                inputSampleL = -2.8;
            }
            if (inputSampleL > 0.0) {
                inputSampleL = (inputSampleL * 2.0) / (3.0 - inputSampleL);
            }
            else {
                inputSampleL = -(inputSampleL * -2.0) / (3.0 + inputSampleL);
            }
            if (inputSampleR > 2.8) {
                inputSampleR = 2.8;
            }
            if (inputSampleR < -2.8) {
                inputSampleR = -2.8;
            }
            if (inputSampleR > 0.0) {
                inputSampleR = (inputSampleR * 2.0) / (3.0 - inputSampleR);
            }
            else {
                inputSampleR = -(inputSampleR * -2.0) / (3.0 + inputSampleR);
            }
            // ConsoleZero Buss
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            } // if using the master fader, we are going to attenuate three places.
            // after C0Buss but before EverySlew: allow highs to come out a bit more
            // when pulling back master fader. Less drive equals more open
            temp = inputSampleL;
            double clamp = inputSampleL - lastSinewL;
            if (lastSinewL > 1.0) {
                lastSinewL = 1.0;
            }
            if (lastSinewL < -1.0) {
                lastSinewL = -1.0;
            }
            double sinew = threshSinew * cos(lastSinewL);
            if (clamp > sinew) {
                temp = lastSinewL + sinew;
            }
            if (-clamp > sinew) {
                temp = lastSinewL - sinew;
            }
            inputSampleL = lastSinewL = temp;
            temp = inputSampleR;
            clamp = inputSampleR - lastSinewR;
            if (lastSinewR > 1.0) {
                lastSinewR = 1.0;
            }
            if (lastSinewR < -1.0) {
                lastSinewR = -1.0;
            }
            sinew = threshSinew * cos(lastSinewR);
            if (clamp > sinew) {
                temp = lastSinewR + sinew;
            }
            if (-clamp > sinew) {
                temp = lastSinewR - sinew;
            }
            inputSampleR = lastSinewR = temp;
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            } // if using the master fader, we are going to attenuate three places.
            // after EverySlew fades the total output sound: least change in tone here.
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
