#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::debess {

constexpr std::string_view k_name{ "DeBess" };
constexpr std::string_view k_short_description{
    "DeBess is an improved DeEss, with perfect rejection of non-ess audio."
};
constexpr std::string_view k_long_description{
    "Meet DeBess.Named because, for some of you at least, it is at last The Best De Ess. Period. Ever.…assuming certain conditions.DeBess is an extension of my former DeEss, which itself was the high point of several earlier attempts at a special de-esser with an unusual algorithm for finding specifically ess content and rejecting anything else in that frequency range, no matter how many overtones it had. DeEss used a set of sample comparisons to try and find esses, and was very successful at this… except some folks had trouble getting it to engage, and others needed it to be more perfect at rejecting even the faintest softening of other content.DeBess does this by extending the sample comparison window a LOT. In fact, it’s now a slider! You can set it to be even blurrier than DeEss if you like… or barely crack it open to replicate the original DeEss… or crank it up for high isolation de-essing. If you are recording on prosumer equipment or using moving-coil microphones, you might not get enough change between samples to engage DeBess. Same if you’re using high sample rates and your mics do NOT extend right up as far as the sampling lets you: DeBess is not for taking stage mics and making them lisp. It is very distinctly for taking the most high-end of vocal tracks and de-essing only the ess sounds out of them, with zero cost to anything else. Whatever you’re using, if your esses are blowing out the highest treble (which is exactly what you need a de-esser for) then it ought to work for you.If you’re using high sample rate and struggling to get DeBess action and you’re going to be treble boosting for that ultra-bright voice sound, try brightening BEFORE DeBess and you’ll probably be able to get what you need. It wants very bright esses to work with, so it can duck and darken them. Use the filter control to shape a better EQ on your esses, rather than just trying to duck ’em."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class DeBess final : public Effect<T>
{
    double sL[41], mL[41], cL[41];
    double ratioAL;
    double ratioBL;
    double iirSampleAL;
    double iirSampleBL;
    double sR[41], mR[41], cR[41];
    double ratioAR;
    double ratioBR;
    double iirSampleAR;
    double iirSampleBR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    DeBess()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.0;
        for (int x = 0; x < 41; x++) {
            sL[x] = 0.0;
            mL[x] = 0.0;
            cL[x] = 0.0;
            sR[x] = 0.0;
            mR[x] = 0.0;
            cR[x] = 0.0;
        }
        ratioAL = ratioBL = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        ratioAR = ratioBR = 1.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        flip = false;
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
            return 0.0;
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
            return "intense";
            break;
        kParamB:
            return "sharp";
            break;
        kParamC:
            return "depth";
            break;
        kParamD:
            return "filter";
            break;
        kParamE:
            return "sense";
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
            return "Intense";
            break;
        kParamB:
            return "Sharp";
            break;
        kParamC:
            return "Depth";
            break;
        kParamD:
            return "Filter";
            break;
        kParamE:
            return "Sense";
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double intensity = pow(A, 5) * (8192 / overallscale);
        double sharpness = B * 40.0;
        if (sharpness < 2) {
            sharpness = 2;
        }
        double speed = 0.1 / sharpness;
        double depth = 1.0 / (C + 0.0001);
        double iirAmount = D;
        float monitoring = E;
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
            sL[0] = inputSampleL; // set up so both [0] and [1] will be input sample
            sR[0] = inputSampleR; // set up so both [0] and [1] will be input sample
            // we only use the [1] so this is just where samples come in
            for (int x = sharpness; x > 0; x--) {
                sL[x] = sL[x - 1];
                sR[x] = sR[x - 1];
            } // building up a set of slews
            mL[1] = (sL[1] - sL[2]) * ((sL[1] - sL[2]) / 1.3);
            mR[1] = (sR[1] - sR[2]) * ((sR[1] - sR[2]) / 1.3);
            for (int x = sharpness - 1; x > 1; x--) {
                mL[x] = (sL[x] - sL[x + 1]) * ((sL[x - 1] - sL[x]) / 1.3);
                mR[x] = (sR[x] - sR[x + 1]) * ((sR[x - 1] - sR[x]) / 1.3);
            } // building up a set of slews of slews
            double senseL = fabs(mL[1] - mL[2]) * sharpness * sharpness;
            double senseR = fabs(mR[1] - mR[2]) * sharpness * sharpness;
            for (int x = sharpness - 1; x > 0; x--) {
                double multL = fabs(mL[x] - mL[x + 1]) * sharpness * sharpness;
                if (multL < 1.0) {
                    senseL *= multL;
                }
                double multR = fabs(mR[x] - mR[x + 1]) * sharpness * sharpness;
                if (multR < 1.0) {
                    senseR *= multR;
                }
            } // sense is slews of slews times each other
            senseL = 1.0 + (intensity * intensity * senseL);
            if (senseL > intensity) {
                senseL = intensity;
            }
            senseR = 1.0 + (intensity * intensity * senseR);
            if (senseR > intensity) {
                senseR = intensity;
            }
            if (flip) {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                ratioAL = (ratioAL * (1.0 - speed)) + (senseL * speed);
                ratioAR = (ratioAR * (1.0 - speed)) + (senseR * speed);
                if (ratioAL > depth) {
                    ratioAL = depth;
                }
                if (ratioAR > depth) {
                    ratioAR = depth;
                }
                if (ratioAL > 1.0) {
                    inputSampleL = iirSampleAL + ((inputSampleL - iirSampleAL) / ratioAL);
                }
                if (ratioAR > 1.0) {
                    inputSampleR = iirSampleAR + ((inputSampleR - iirSampleAR) / ratioAR);
                }
            }
            else {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                ratioBL = (ratioBL * (1.0 - speed)) + (senseL * speed);
                ratioBR = (ratioBR * (1.0 - speed)) + (senseR * speed);
                if (ratioBL > depth) {
                    ratioBL = depth;
                }
                if (ratioBR > depth) {
                    ratioBR = depth;
                }
                if (ratioAL > 1.0) {
                    inputSampleL = iirSampleBL + ((inputSampleL - iirSampleBL) / ratioBL);
                }
                if (ratioAR > 1.0) {
                    inputSampleR = iirSampleBR + ((inputSampleR - iirSampleBR) / ratioBR);
                }
            }
            flip = !flip;
            if (monitoring > 0.49999) {
                inputSampleL = *in1 - inputSampleL;
                inputSampleR = *in2 - inputSampleR;
            }
            // sense monitoring
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
} // namespace airwindohhs::debess
