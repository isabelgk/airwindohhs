#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console5buss {

constexpr std::string_view k_name{ "Console5Buss" };
constexpr std::string_view k_short_description{
    "Console5Buss is a richer, warmer Console system."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console5Buss final : public Effect<T>
{
    double lastSampleBussL;
    double lastFXBussL;
    double lastSampleBussR;
    double lastFXBussR;
    double iirCorrectL;
    double iirCorrectR;
    double gainchase;
    double settingchase;
    double chasespeed;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Console5Buss()
    {
        A = 1.0;
        lastSampleBussL = 0.0;
        lastSampleBussR = 0.0;
        lastFXBussL = 0.0;
        lastFXBussR = 0.0;
        iirCorrectL = 0.0;
        iirCorrectR = 0.0;
        gainchase = -90.0;
        settingchase = -90.0;
        chasespeed = 350.0;
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
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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
        overallscale *= Effect<T>::getSampleRate();
        double inputgain = A;
        double differenceL;
        double differenceR;
        double nearZeroL;
        double nearZeroR;
        double servoTrim = 0.0000001 / overallscale;
        double bassTrim = 0.005 / overallscale;
        double inputSampleL;
        double inputSampleR;
        if (settingchase != inputgain) {
            chasespeed *= 2.0;
            settingchase = inputgain;
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        if (gainchase < 0.0) {
            gainchase = inputgain;
        }
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
            chasespeed *= 0.9999;
            chasespeed -= 0.01;
            if (chasespeed < 350.0) {
                chasespeed = 350.0;
            }
            // we have our chase speed compensated for recent fader activity
            gainchase = (((gainchase * chasespeed) + inputgain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= gainchase;
                inputSampleR *= gainchase;
            }
            // done with trim control
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL = asin(inputSampleL);
            // amplitude aspect
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            differenceL = lastSampleBussL - inputSampleL;
            differenceR = lastSampleBussR - inputSampleR;
            lastSampleBussL = inputSampleL;
            lastSampleBussR = inputSampleR;
            // derive slew part off direct sample measurement + from last time
            if (differenceL > 1.57079633) {
                differenceL = 1.57079633;
            }
            if (differenceL < -1.57079633) {
                differenceL = -1.57079633;
            }
            if (differenceR > 1.57079633) {
                differenceR = 1.57079633;
            }
            if (differenceR < -1.57079633) {
                differenceR = -1.57079633;
            }
            differenceL = lastFXBussL + sin(differenceL);
            differenceR = lastFXBussR + sin(differenceR);
            // we're about to use this twice and then not use difference again, so we'll reuse it
            // enhance slew is arcsin(): cutting it back is sin()
            iirCorrectL += inputSampleL - differenceL;
            iirCorrectR += inputSampleR - differenceR;
            inputSampleL = differenceL;
            inputSampleR = differenceR;
            // apply the slew to stored value: can develop DC offsets.
            // store the change we made so we can dial it back
            lastFXBussL = inputSampleL;
            lastFXBussR = inputSampleR;
            if (lastFXBussL > 1.0) {
                lastFXBussL = 1.0;
            }
            if (lastFXBussL < -1.0) {
                lastFXBussL = -1.0;
            }
            if (lastFXBussR > 1.0) {
                lastFXBussR = 1.0;
            }
            if (lastFXBussR < -1.0) {
                lastFXBussR = -1.0;
            }
            // build new signal off what was present in output last time
            nearZeroL = pow(fabs(fabs(lastFXBussL) - 1.0), 2);
            nearZeroR = pow(fabs(fabs(lastFXBussR) - 1.0), 2);
            // if the sample is very near zero this number is higher.
            if (iirCorrectL > 0) {
                iirCorrectL -= servoTrim;
            }
            if (iirCorrectL < 0) {
                iirCorrectL += servoTrim;
            }
            if (iirCorrectR > 0) {
                iirCorrectR -= servoTrim;
            }
            if (iirCorrectR < 0) {
                iirCorrectR += servoTrim;
            }
            // cut back the servo by which we're pulling back the DC
            lastFXBussL += (iirCorrectL * 0.0000005);
            lastFXBussR += (iirCorrectR * 0.0000005);
            // apply the servo to the stored value, pulling back the DC
            lastFXBussL *= (1.0 - (nearZeroL * bassTrim));
            lastFXBussR *= (1.0 - (nearZeroR * bassTrim));
            // this cuts back the DC offset directly, relative to how near zero we are
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
} // namespace airwindohhs::console5buss
