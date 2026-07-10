#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::console4buss {

constexpr std::string_view k_name{ "Console4Buss" };
constexpr std::string_view k_short_description{
    "Console4Buss is the original Airwindows ITB mixing system."
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class Console4Buss final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    double fpNShapeL;
    double fpNShapeR;
    // default stuff
    double lastSampleL;
    double lastSampleR;
    double gainchase;
    double settingchase;
    double chasespeed;
    float gain;

  public:
    Console4Buss()
    {
        gain = 1.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
        // TODO: uncomment canDo entries according to your plugin's capabilities
    }

    enum params
    {
        kParamA = 0,
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: gain = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return gain; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "trim"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Trim"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(gain); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return " "; break;

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
        double inputSampleL;
        double inputSampleR;
        double half;
        double falf;
        double slewcompensation;
        if (settingchase != gain) {
            chasespeed *= 2.0;
            settingchase = gain;
        }
        if (chasespeed > 2500.0) {
            chasespeed = 2500.0;
        }
        if (gainchase < 0.0) {
            gainchase = gain;
        }
        while (--sampleFrames >= 0) {
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
            gainchase = (((gainchase * chasespeed) + gain) / (chasespeed + 1.0));
            // gainchase is chasing the target, as a simple multiply gain factor
            if (1.0 != gainchase) {
                inputSampleL *= gainchase;
                inputSampleR *= gainchase;
            }
            // done with trim control
            half = inputSampleL;
            falf = fabs(half);
            half *= falf;
            half *= falf;
            slewcompensation = fabs(inputSampleL - lastSampleL) * overallscale;
            // magnify effect at high sample rate so it will still register when inter-sample changes
            // are very small at high rates.
            if (slewcompensation > 1.0) {
                slewcompensation = 1.0;
            }
            // let's not invert the effect: maximum application is to cancel out half entirely
            half *= (1.0 - slewcompensation);
            // apply it
            lastSampleL = inputSampleL;
            inputSampleL += half;
            // this is the inverse processing for Console: boosts but not so much if there's slew.
            // is this too subtle an effect?
            half = inputSampleR;
            falf = fabs(half);
            half *= falf;
            half *= falf;
            slewcompensation = fabs(inputSampleR - lastSampleR) * overallscale;
            // magnify effect at high sample rate so it will still register when inter-sample changes
            // are very small at high rates.
            if (slewcompensation > 1.0) {
                slewcompensation = 1.0;
            }
            // let's not invert the effect: maximum application is to cancel out half entirely
            half *= (1.0 - slewcompensation);
            // apply it
            lastSampleR = inputSampleR;
            inputSampleR += half;
            // this is the inverse processing for Console: boosts but not so much if there's slew.
            // is this too subtle an effect?
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
} // namespace airwindohhs::console4buss
