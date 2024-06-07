#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::consolemdbuss {

constexpr std::string_view k_name{ "ConsoleMDBuss" };
constexpr std::string_view k_short_description{
    "ConsoleMDBuss goes for the tone shaping of the classic MCI console!"
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class ConsoleMDBuss final : public Effect<T>
{
    double lastSinewL;
    double lastSinewR;
    double gainA;
    double gainB; // smoothed master fader for channel, from Z2 series filter code
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    ConsoleMDBuss()
    {
        A = 1.0;
        lastSinewL = lastSinewR = 0.0;
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
            return 1.0;
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
            return "master";
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
            return "Master";
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        gainA = gainB;
        gainB = sqrt(A); // smoothed master fader from Z2 filters
        // this will be applied three times: this is to make the various tone alterations
        // hit differently at different master fader drive levels.
        // in particular, backing off the master fader tightens the super lows
        // but opens up the modified Sinew, because more of the attentuation happens before
        // you even get to slew clipping :) and if the fader is not active, it bypasses completely.
        double threshSinew = 0.5171104 / overallscale;
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
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            } // if using the master fader, we are going to attenuate three places.
            // subtight is always fully engaged: tighten response when restraining full console
            // begin Console7Buss which is the one we choose for ConsoleMC
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL = ((asin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.618033988749894848204586) + (asin(inputSampleL) * 0.381966011250105);
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR = ((asin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.618033988749894848204586) + (asin(inputSampleR) * 0.381966011250105);
            // this is an asin version of Spiral blended with regular asin ConsoleBuss.
            // It's blending between two different harmonics in the overtones of the algorithm
            if (gain < 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            } // if using the master fader, we are going to attenuate three places.
            // after C7Buss but before EverySlew: allow highs to come out a bit more
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
} // namespace airwindohhs::consolemdbuss
