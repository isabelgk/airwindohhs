#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::buildatpdf {

constexpr std::string_view k_name{ "BuildATPDF" };
constexpr std::string_view k_short_description{
    "BuildATPDF is a dither-making toolkit."
};
constexpr std::string_view k_long_description{
    "I've explained how DoublePaul was made: taking a TPDF highpass dither, and including additional nearby samples to cause the tone of the noise to be more high-pitched.Wouldn’t be fair if I didn’t give you a chance to play with it yourself :)To use BuildATPDF the way I used it, set the middle sliders to -1 and +1. Then adjust the other sliders until you’ve tailored the sound of the noise in a way you like. With the other sliders at 0, you have PaulDither. If the surrounding sliders ‘oscillate’ (going minus, plus, minus, plus) you have more of what DoublePaul is. Tailor the highpass in ways gentle or obvious, or even darken the highpass while leaving its dither functionality intact! It’s the combination of a -1 and +1 tap that gives you highpassed TPDF. (This won’t work with +1 and +1, because it’s the same random noise passing through the plugin: it needs to play a positive node against a negative node)Or, you can set up the -1 and +1 taps, and then just play with the sliders to see what it sounds like!"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class BuildATPDF final : public Effect<T>
{
    double bL[11];
    double bR[11];
    double f[11];
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;
    float J;
    // parameters. Always 0-1, and we scale/alter them elsewhere.
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    BuildATPDF()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 0.5;
        for (int count = 0; count < 11; count++) {
            bL[count] = 0.0;
            bR[count] = 0.0;
            f[count] = 0.0;
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
        kParamF = 5,
        kParamG = 6,
        kParamH = 7,
        kParamI = 8,
        kParamJ = 9,
        kNumParameters = 10
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
        kParamF:
            F = value;
            break;
        kParamG:
            G = value;
            break;
        kParamH:
            H = value;
            break;
        kParamI:
            I = value;
            break;
        kParamJ:
            J = value;
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
        kParamF:
            return F;
            break;
        kParamG:
            return G;
            break;
        kParamH:
            return H;
            break;
        kParamI:
            return I;
            break;
        kParamJ:
            return J;
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
            return 0.5;
            break;
        kParamE:
            return 0.5;
            break;
        kParamF:
            return 0.5;
            break;
        kParamG:
            return 0.5;
            break;
        kParamH:
            return 0.5;
            break;
        kParamI:
            return 0.5;
            break;
        kParamJ:
            return 0.5;
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
            return "first";
            break;
        kParamB:
            return "second";
            break;
        kParamC:
            return "third";
            break;
        kParamD:
            return "fourth";
            break;
        kParamE:
            return "fifth";
            break;
        kParamF:
            return "sixth";
            break;
        kParamG:
            return "seventh";
            break;
        kParamH:
            return "eighth";
            break;
        kParamI:
            return "ninth";
            break;
        kParamJ:
            return "tenth";
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
            return "First";
            break;
        kParamB:
            return "Second";
            break;
        kParamC:
            return "Third";
            break;
        kParamD:
            return "Fourth";
            break;
        kParamE:
            return "Fifth";
            break;
        kParamF:
            return "Sixth";
            break;
        kParamG:
            return "Seventh";
            break;
        kParamH:
            return "Eighth";
            break;
        kParamI:
            return "Ninth";
            break;
        kParamJ:
            return "Tenth";
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
        kParamF:
            return std::to_string(F);
            break;
        kParamG:
            return std::to_string(G);
            break;
        kParamH:
            return std::to_string(H);
            break;
        kParamI:
            return std::to_string(I);
            break;
        kParamJ:
            return std::to_string(J);
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
            return " ";
            break;
        kParamB:
            return " ";
            break;
        kParamC:
            return " ";
            break;
        kParamD:
            return " ";
            break;
        kParamE:
            return " ";
            break;
        kParamF:
            return " ";
            break;
        kParamG:
            return " ";
            break;
        kParamH:
            return " ";
            break;
        kParamI:
            return " ";
            break;
        kParamJ:
            return " ";
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

        f[0] = (A * 2) - 1;
        f[1] = (B * 2) - 1;
        f[2] = (C * 2) - 1;
        f[3] = (D * 2) - 1;
        f[4] = (E * 2) - 1;
        f[5] = (F * 2) - 1;
        f[6] = (G * 2) - 1;
        f[7] = (H * 2) - 1;
        f[8] = (I * 2) - 1;
        f[9] = (J * 2) - 1;
        double currentDither;
        double inputSampleL;
        double inputSampleR;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = (double(fpdL) / UINT32_MAX);
            currentDither = (bL[0] * f[0]);
            currentDither += (bL[1] * f[1]);
            currentDither += (bL[2] * f[2]);
            currentDither += (bL[3] * f[3]);
            currentDither += (bL[4] * f[4]);
            currentDither += (bL[5] * f[5]);
            currentDither += (bL[6] * f[6]);
            currentDither += (bL[7] * f[7]);
            currentDither += (bL[8] * f[8]);
            currentDither += (bL[9] * f[9]);
            inputSampleL += currentDither;
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = (double(fpdR) / UINT32_MAX);
            currentDither = (bR[0] * f[0]);
            currentDither += (bR[1] * f[1]);
            currentDither += (bR[2] * f[2]);
            currentDither += (bR[3] * f[3]);
            currentDither += (bR[4] * f[4]);
            currentDither += (bR[5] * f[5]);
            currentDither += (bR[6] * f[6]);
            currentDither += (bR[7] * f[7]);
            currentDither += (bR[8] * f[8]);
            currentDither += (bR[9] * f[9]);
            inputSampleR += currentDither;
            inputSampleL = floor(inputSampleL);
            inputSampleR = floor(inputSampleR);
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
}

};
} // namespace airwindohhs::buildatpdf
