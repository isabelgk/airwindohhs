#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::compresaturator {

constexpr std::string_view k_name{ "Compresaturator" };
constexpr std::string_view k_short_description{
    "Compresaturator fades between compressing and soft clipping."
};
constexpr std::string_view k_long_description{
    "One of the final plugins from the Kagi era, this stopped being available for a while when Kagi went out of business. It’s like a soft-saturator in which you can dial in even more cleanness, but couple it with a weird springy quality that’s very dynamic. It occupies more of the role of a stem or buss compressor than a peak limiter, but makes for a very powerful loudenator. The idea is that you can get a loud punchy sound but with more attitude than usual.Since it’s one of those strange Airwindows algorithms, be aware you can push it too far and get it to ‘flutter’ or oscillate like a tremolo or do other odd things: in normal use that shouldn’t ever be a concern, I’m just saying that this plugin isn’t normal and doesn’t sound or act quite like other plugins.In other words, classic Airwindows :) hope you like it!"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Compresaturator final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    int dCount;
    float dL[11000];
    int lastWidthL;
    double padFactorL;
    float dR[11000];
    int lastWidthR;
    double padFactorR;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Compresaturator()
    {
        A = 0.5; //-12 to +12 dB
        B = 0.5; // 0 to 100%
        C = 0.5; // 50 to 5000 samples, default 500
        D = 1.0;
        E = 1.0;
        for (int count = 0; count < 10990; count++) {
            dL[count] = 0.0;
            dR[count] = 0.0;
        }
        dCount = 0;
        lastWidthL = 500;
        padFactorL = 0;
        lastWidthR = 500;
        padFactorR = 0;
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
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive"; break;
            case kParamB: return "clamp"; break;
            case kParamC: return "expand"; break;
            case kParamD: return "output"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;
            case kParamB: return "Clamp"; break;
            case kParamC: return "Expand"; break;
            case kParamD: return "Output"; break;
            case kParamE: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB"; break;
            case kParamB: return "%"; break;
            case kParamC: return "sa"; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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

        double inputgain = pow(10.0, ((A * 24.0) - 12.0) / 20.0);
        double satComp = B * 2.0;
        int widestRange = C * C * 5000;
        if (widestRange < 50) {
            widestRange = 50;
        }
        satComp += (((double)widestRange / 3000.0) * satComp);
        // set the max wideness of comp zone, minimum range boosted (too much?)
        double output = D;
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
            if (dCount < 1 || dCount > 5000) {
                dCount = 5000;
            }
            // begin L
            double temp = inputSampleL;
            double variSpeed = 1.0 + ((padFactorL / lastWidthL) * satComp);
            if (variSpeed < 1.0) {
                variSpeed = 1.0;
            }
            double totalgain = inputgain / variSpeed;
            if (totalgain != 1.0) {
                inputSampleL *= totalgain;
                if (totalgain < 1.0) {
                    temp *= totalgain;
                    // no boosting beyond unity please
                }
            }
            double bridgerectifier = fabs(inputSampleL);
            double overspill = 0;
            int targetWidth = widestRange;
            // we now have defaults and an absolute input value to work with
            if (bridgerectifier < 0.01) {
                padFactorL *= 0.9999;
            }
            // in silences we bring back padFactor if it got out of hand
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
                targetWidth = 8;
            }
            // if our output's gone beyond saturating to distorting, we begin chasing the
            // buffer size smaller. Anytime we don't have that, we expand (smoothest sound, only adding to an increasingly subdivided buffer)
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
                overspill = temp - bridgerectifier;
            }
            if (inputSampleL < 0) {
                inputSampleL = -bridgerectifier;
                overspill = (-temp) - bridgerectifier;
            }
            // drive section
            dL[dCount + 5000] = dL[dCount] = overspill * satComp;
            // we now have a big buffer to draw from, which is always positive amount of overspill
            padFactorL += dL[dCount];
            double randy = (double(fpdL) / UINT32_MAX);
            if ((targetWidth * randy) > lastWidthL) {
                // we are expanding the buffer so we don't remove this trailing sample
                lastWidthL += 1;
            }
            else {
                padFactorL -= dL[dCount + lastWidthL];
                // zero change, or target is smaller and we are shrinking
                if (targetWidth < lastWidthL) {
                    lastWidthL -= 1;
                    if (lastWidthL < 2) {
                        lastWidthL = 2;
                    }
                    // sanity check as randy can give us target zero
                    padFactorL -= dL[dCount + lastWidthL];
                }
            }
            // variable attack/release speed more rapid as comp intensity increases
            // implemented in a way where we're repeatedly not altering the buffer as it expands, which makes the comp artifacts smoother
            if (padFactorL < 0) {
                padFactorL = 0;
            }
            // end L
            // begin R
            temp = inputSampleR;
            variSpeed = 1.0 + ((padFactorR / lastWidthR) * satComp);
            if (variSpeed < 1.0) {
                variSpeed = 1.0;
            }
            totalgain = inputgain / variSpeed;
            if (totalgain != 1.0) {
                inputSampleR *= totalgain;
                if (totalgain < 1.0) {
                    temp *= totalgain;
                    // no boosting beyond unity please
                }
            }
            bridgerectifier = fabs(inputSampleR);
            overspill = 0;
            targetWidth = widestRange;
            // we now have defaults and an absolute input value to work with
            if (bridgerectifier < 0.01) {
                padFactorR *= 0.9999;
            }
            // in silences we bring back padFactor if it got out of hand
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
                targetWidth = 8;
            }
            // if our output's gone beyond saturating to distorting, we begin chasing the
            // buffer size smaller. Anytime we don't have that, we expand (smoothest sound, only adding to an increasingly subdivided buffer)
            bridgerectifier = sin(bridgerectifier);
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
                overspill = temp - bridgerectifier;
            }
            if (inputSampleR < 0) {
                inputSampleR = -bridgerectifier;
                overspill = (-temp) - bridgerectifier;
            }
            // drive section
            dR[dCount + 5000] = dR[dCount] = overspill * satComp;
            // we now have a big buffer to draw from, which is always positive amount of overspill
            padFactorR += dR[dCount];
            randy = (double(fpdR) / UINT32_MAX);
            if ((targetWidth * randy) > lastWidthR) {
                // we are expanding the buffer so we don't remove this trailing sample
                lastWidthR += 1;
            }
            else {
                padFactorR -= dR[dCount + lastWidthR];
                // zero change, or target is smaller and we are shrinking
                if (targetWidth < lastWidthR) {
                    lastWidthR -= 1;
                    if (lastWidthR < 2) {
                        lastWidthR = 2;
                    }
                    // sanity check as randy can give us target zero
                    padFactorR -= dR[dCount + lastWidthR];
                }
            }
            // variable attack/release speed more rapid as comp intensity increases
            // implemented in a way where we're repeatedly not altering the buffer as it expands, which makes the comp artifacts smoother
            if (padFactorR < 0) {
                padFactorR = 0;
            }
            // end R
            dCount--;
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
            // inputSampleL += static_cast<int32_t>(fpd) * 1.110223024625156e-44L * pow(2,expon+62);
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // inputSampleR += static_cast<int32_t>(fpd) * 1.110223024625156e-44L * pow(2,expon+62);
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
} // namespace airwindohhs::compresaturator
