#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dehiss {

constexpr std::string_view k_name{ "DeHiss" };
constexpr std::string_view k_short_description{
    "DeHiss tries to suppress background hiss, like a hiss gate."
};
constexpr std::string_view k_long_description{
    "This acts like a filtery gate. It tries to apply a lowpass filter to suppress quiet background noise whenever loud bright treble isn’t happening. You can use it in creative ways, but it really was designed to suppress hiss from a cheap USB condenser mic. Sort of an experiment. It’s got a dry/wet now in case that comes in handy. I’m not sure how much else is out there specifically like this, but then I’m not sure how much call there is for it in the first place :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class DeHiss final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedL[2];
    double diffL[6];
    double gateL;
    double rawL;
    double storedR[2];
    double diffR[6];
    double gateR;
    double rawR;
    float A;
    float B;

  public:
    DeHiss()
    {
        A = 0.0;
        B = 1.0;
        storedL[0] = storedL[1] = 0.0;
        diffL[0] = diffL[1] = diffL[2] = diffL[3] = diffL[4] = diffL[5] = 0.0;
        gateL = 1.0;
        rawL = 2.0;
        storedR[0] = storedR[1] = 0.0;
        diffR[0] = diffR[1] = diffR[2] = diffR[3] = diffR[4] = diffR[5] = 0.0;
        gateR = 1.0;
        rawR = 2.0;
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
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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
        kParamB:
            return B;
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
        kParamB:
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
            return "thresh";
            break;
        kParamB:
            return "dry/wet";
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
            return "Thresh";
            break;
        kParamB:
            return "Dry/Wet";
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
        kParamB:
            return std::to_string(B);
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
        kParamB:
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
        overallscale *= getSampleRate();
        double meanAL;
        double meanBL;
        double meanOutL = 0;
        double meanLastL;
        double averageL[5];
        double meanAR;
        double meanBR;
        double meanOutR = 0;
        double meanLastR;
        double averageR[5];
        double threshold = pow(A, 9);
        double wet = B;
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
            // begin L
            storedL[1] = storedL[0];
            storedL[0] = inputSampleL;
            diffL[5] = diffL[4];
            diffL[4] = diffL[3];
            diffL[3] = diffL[2];
            diffL[2] = diffL[1];
            diffL[1] = diffL[0];
            diffL[0] = storedL[0] - storedL[1];
            averageL[4] = (diffL[0] + diffL[1] + diffL[2] + diffL[3] + diffL[4] + diffL[5]) / 6.0;
            averageL[3] = (diffL[0] + diffL[1] + diffL[2] + diffL[3] + diffL[4]) / 5.0;
            averageL[2] = (diffL[0] + diffL[1] + diffL[2] + diffL[3]) / 4.0;
            averageL[1] = (diffL[0] + diffL[1] + diffL[2]) / 3.0;
            averageL[0] = (diffL[0] + diffL[1]) / 2.0;
            meanAL = diffL[0];
            meanBL = diffL[0];
            if (fabs(averageL[4]) < fabs(meanBL)) {
                meanAL = meanBL;
                meanBL = averageL[4];
            }
            if (fabs(averageL[3]) < fabs(meanBL)) {
                meanAL = meanBL;
                meanBL = averageL[3];
            }
            if (fabs(averageL[2]) < fabs(meanBL)) {
                meanAL = meanBL;
                meanBL = averageL[2];
            }
            if (fabs(averageL[1]) < fabs(meanBL)) {
                meanAL = meanBL;
                meanBL = averageL[1];
            }
            if (fabs(averageL[0]) < fabs(meanBL)) {
                meanAL = meanBL;
                meanBL = averageL[0];
            }
            meanLastL = meanOutL;
            meanOutL = ((meanAL + meanBL) / 2.0);
            if (rawL > 0) {
                rawL -= 0.001;
            }
            if (fabs(inputSampleL) > threshold) {
                gateL = 1.0;
                rawL = 2.0;
            }
            else {
                gateL = (gateL * 0.999);
                if (threshold > 0) {
                    gateL += ((fabs(inputSampleL) / threshold) * 0.001);
                }
            }
            if ((fabs(meanOutL) > threshold) || (fabs(meanLastL) > threshold)) {
            }
            else {
                storedL[0] = storedL[1] + (meanOutL * gateL);
            }
            if (rawL < 1) {
                inputSampleL = (inputSampleL * rawL) + (storedL[0] * (1 - rawL));
            }
            // end L
            // begin R
            storedR[1] = storedR[0];
            storedR[0] = inputSampleR;
            diffR[5] = diffR[4];
            diffR[4] = diffR[3];
            diffR[3] = diffR[2];
            diffR[2] = diffR[1];
            diffR[1] = diffR[0];
            diffR[0] = storedR[0] - storedR[1];
            averageR[4] = (diffR[0] + diffR[1] + diffR[2] + diffR[3] + diffR[4] + diffR[5]) / 6.0;
            averageR[3] = (diffR[0] + diffR[1] + diffR[2] + diffR[3] + diffR[4]) / 5.0;
            averageR[2] = (diffR[0] + diffR[1] + diffR[2] + diffR[3]) / 4.0;
            averageR[1] = (diffR[0] + diffR[1] + diffR[2]) / 3.0;
            averageR[0] = (diffR[0] + diffR[1]) / 2.0;
            meanAR = diffR[0];
            meanBR = diffR[0];
            if (fabs(averageR[4]) < fabs(meanBR)) {
                meanAR = meanBR;
                meanBR = averageR[4];
            }
            if (fabs(averageR[3]) < fabs(meanBR)) {
                meanAR = meanBR;
                meanBR = averageR[3];
            }
            if (fabs(averageR[2]) < fabs(meanBR)) {
                meanAR = meanBR;
                meanBR = averageR[2];
            }
            if (fabs(averageR[1]) < fabs(meanBR)) {
                meanAR = meanBR;
                meanBR = averageR[1];
            }
            if (fabs(averageR[0]) < fabs(meanBR)) {
                meanAR = meanBR;
                meanBR = averageR[0];
            }
            meanLastR = meanOutR;
            meanOutR = ((meanAR + meanBR) / 2.0);
            if (rawR > 0) {
                rawR -= 0.001;
            }
            if (fabs(inputSampleR) > threshold) {
                gateR = 1.0;
                rawR = 2.0;
            }
            else {
                gateR = (gateR * 0.999);
                if (threshold > 0) {
                    gateR += ((fabs(inputSampleR) / threshold) * 0.001);
                }
            }
            if ((fabs(meanOutR) > threshold) || (fabs(meanLastR) > threshold)) {
            }
            else {
                storedR[0] = storedR[1] + (meanOutR * gateR);
            }
            if (rawR < 1) {
                inputSampleR = (inputSampleR * rawR) + (storedR[0] * (1 - rawR));
            }
            // end R
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::dehiss
