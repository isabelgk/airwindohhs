#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::cojones {

constexpr std::string_view k_name{ "Cojones" };
constexpr std::string_view k_short_description{
    "Cojones is a new kind of distorty."
};
constexpr std::string_view k_long_description{
    "Cojones is one of the promised releases from back when I started all this. What’s interesting is, Cojones is also the seeds of Dither Me Timbers and StudioTan. That’s because it does a similar thing: it tracks the trajectory of the waveform (over five instead of three samples) and either heightens or minimizes any disparities it finds. It’s called Cojones, because I thought it highlighted that sort of quality in voices and guitars, though it’s easy to just make it be distorty and strange.You’ll find that boosting Cojones can give a peculiar sort of midrangey sonority. I’m not going to say it’s GOOD sounding, but it is at least distinctive. There’s also a ‘breathy’ which is more three-sample stuff like Dither Me Timbers and StudioTan, and a ‘body’ control that can beef up or cut bass and low mids.Pretty much play with it and if you hate it, throw it away and curse its name and mine. It’s all the rage! :D seriously, if you’re the sort to like this, you know who you are. If you’ve been putting Dither Me Timbers or StudioTan in places that aren’t the output dither, you need to try this instead as you’ll get a lot more out of it. And if its seasoning seems way too spicy and always produces trebly grit, try very slight amounts of its mojo, as this is one that’s set up so you can apply too much.After all, what good is an ugly new distorty if you can’t overuse it and make unpleasant yet unrecognizable noises? :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Cojones final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double storedL[2];
    double diffL[6];
    double storedR[2];
    double diffR[6];
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Cojones()
    {
        A = 0.5; // 0-2
        B = 0.5; // 0-2
        C = 0.5; // 0-2
        D = 1.0;
        E = 1.0;
        storedL[0] = storedL[1] = 0.0;
        diffL[0] = diffL[1] = diffL[2] = diffL[3] = diffL[4] = diffL[5] = 0.0;
        storedR[0] = storedR[1] = 0.0;
        diffR[0] = diffR[1] = diffR[2] = diffR[3] = diffR[4] = diffR[5] = 0.0;
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
        kNumParameters = 5
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
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
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
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;
        kParamE:
            return E;
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
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
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
            return "breathy";
            break;
        kParamB:
            return "cojones";
            break;
        kParamC:
            return "body";
            break;
        kParamD:
            return "output";
            break;
        kParamE:
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
            return "Breathy";
            break;
        kParamB:
            return "Cojones";
            break;
        kParamC:
            return "Body";
            break;
        kParamD:
            return "Output";
            break;
        kParamE:
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
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
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
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;
        kParamE:
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

        double breathy = A * 2.0;
        double cojones = B * 2.0;
        double body = C * 2.0;
        double output = D;
        double wet = E;
        double averageL[5];
        double averageR[5];
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
            averageL[0] = diffL[0] + diffL[1];
            averageL[1] = averageL[0] + diffL[2];
            averageL[2] = averageL[1] + diffL[3];
            averageL[3] = averageL[2] + diffL[4];
            averageL[4] = averageL[3] + diffL[5];
            averageL[0] /= 2.0;
            averageL[1] /= 3.0;
            averageL[2] /= 4.0;
            averageL[3] /= 5.0;
            averageL[4] /= 6.0;
            double meanA = diffL[0];
            double meanB = diffL[0];
            if (fabs(averageL[4]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[4];
            }
            if (fabs(averageL[3]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[3];
            }
            if (fabs(averageL[2]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[2];
            }
            if (fabs(averageL[1]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[1];
            }
            if (fabs(averageL[0]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageL[0];
            }
            double meanOut = ((meanA + meanB) / 2.0);
            storedL[0] = (storedL[1] + meanOut);
            double outputSample = storedL[0] * body;
            // presubtract cojones
            outputSample += (((inputSampleL - storedL[0]) - averageL[1]) * cojones);
            outputSample += (averageL[1] * breathy);
            inputSampleL = outputSample;
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
            averageR[0] = diffR[0] + diffR[1];
            averageR[1] = averageR[0] + diffR[2];
            averageR[2] = averageR[1] + diffR[3];
            averageR[3] = averageR[2] + diffR[4];
            averageR[4] = averageR[3] + diffR[5];
            averageR[0] /= 2.0;
            averageR[1] /= 3.0;
            averageR[2] /= 4.0;
            averageR[3] /= 5.0;
            averageR[4] /= 6.0;
            meanA = diffR[0];
            meanB = diffR[0];
            if (fabs(averageR[4]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[4];
            }
            if (fabs(averageR[3]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[3];
            }
            if (fabs(averageR[2]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[2];
            }
            if (fabs(averageR[1]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[1];
            }
            if (fabs(averageR[0]) < fabs(meanB)) {
                meanA = meanB;
                meanB = averageR[0];
            }
            meanOut = ((meanA + meanB) / 2.0);
            storedR[0] = (storedR[1] + meanOut);
            outputSample = storedR[0] * body;
            // presubtract cojones
            outputSample += (((inputSampleR - storedR[0]) - averageR[1]) * cojones);
            outputSample += (averageR[1] * breathy);
            inputSampleR = outputSample;
            // end R
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::cojones
