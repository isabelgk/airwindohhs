#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tubedesk {

constexpr std::string_view k_name{ "TubeDesk" };
constexpr std::string_view k_short_description{
    "TubeDesk is a tube recording console type tone coloring."
};
constexpr std::string_view k_long_description{
    "More classic Airwindows analog modeling with TubeDesk!Tubes aren’t necessarily ‘mellow’. They’re also known for clarity, realism. TubeDesk isn’t a mud-ifier, but it might bring you some effects reminiscent of vintage recordings.Like the other Desk plugins, it’s got a kind of saturation going on, a distinct flavor to how it distorts. However, unlike TransDesk, its power supply is very different. TubeDesk is so old school it acts like there’s a vintage tube rectifier, imparting an obvious rectifier sag. This conditions the sound, affecting how dynamics work through the plugin.You have a distinct ‘analog modeling’ tone then, which is no specific console, no arbitrary color: just sort of retro vibe, generalized. It’s not calibrated to work with Console (that’s the for-pay version of Desk that consolidated these and calibrated them all) and it doesn’t have special requirements for where it should go. Place it where you want that style of tone coloring: like Desk, you can use it on auxes and submixes (including ones ‘inside’ Console) to better emulate running through a lot of circuitry."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class TubeDesk final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    int gcount;
    double dL[5000];
    double controlL;
    double lastSampleL;
    double lastOutSampleL;
    double lastSlewL;
    double dR[5000];
    double controlR;
    double lastSampleR;
    double lastOutSampleR;
    double lastSlewR;

  public:
    TubeDesk()
    {
        for (int count = 0; count < 4999; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        gcount = 0;
        controlL = 0;
        lastSampleL = 0.0;
        lastOutSampleL = 0.0;
        lastSlewL = 0.0;
        controlR = 0;
        lastSampleR = 0.0;
        lastOutSampleR = 0.0;
        lastSlewR = 0.0;
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
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
        double intensity = 0.4384938;
        double depthA = 549.0;
        int offsetA = (int)(depthA * overallscale);
        if (offsetA < 1) {
            offsetA = 1;
        }
        if (offsetA > 2440) {
            offsetA = 2440;
        }
        double clamp;
        double thickness;
        double out;
        double gain = 0.5;
        double slewgain = 0.128;
        double prevslew = 0.105;
        double balanceB = 0.0001;
        slewgain *= overallscale;
        prevslew *= overallscale;
        balanceB /= overallscale;
        double balanceA = 1.0 - balanceB;
        double slew;
        double bridgerectifier;
        double combSample;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            if (gcount < 0 || gcount > 2450) {
                gcount = 2450;
            }
            // begin L
            dL[gcount + 2450] = dL[gcount] = fabs(inputSampleL) * intensity;
            controlL += (dL[gcount] / offsetA);
            controlL -= (dL[gcount + offsetA] / offsetA);
            controlL -= 0.000001;
            clamp = 1;
            if (controlL < 0) {
                controlL = 0;
            }
            if (controlL > 1) {
                clamp -= (controlL - 1);
                controlL = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            // control = 0 to 1
            thickness = ((1.0 - controlL) * 2.0) - 1.0;
            out = fabs(thickness);
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            inputSampleL *= clamp;
            slew = inputSampleL - lastSampleL;
            lastSampleL = inputSampleL;
            // Set up direct reference for slew
            bridgerectifier = fabs(slew * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slew > 0) {
                slew = bridgerectifier / slewgain;
            }
            else {
                slew = -(bridgerectifier / slewgain);
            }
            inputSampleL = (lastOutSampleL * balanceA) + (lastSampleL * balanceB) + slew;
            // go from last slewed, but include some raw values
            lastOutSampleL = inputSampleL;
            // Set up slewed reference
            combSample = fabs(drySampleL * lastSampleL);
            if (combSample > 1.0) {
                combSample = 1.0;
            }
            // bailout for very high input gains
            inputSampleL -= (lastSlewL * combSample * prevslew);
            lastSlewL = slew;
            // slew interaction with previous slew
            inputSampleL *= gain;
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // drive section
            inputSampleL /= gain;
            // end of Desk section
            // end L
            // begin R
            dR[gcount + 2450] = dR[gcount] = fabs(inputSampleR) * intensity;
            controlR += (dR[gcount] / offsetA);
            controlR -= (dR[gcount + offsetA] / offsetA);
            controlR -= 0.000001;
            clamp = 1;
            if (controlR < 0) {
                controlR = 0;
            }
            if (controlR > 1) {
                clamp -= (controlR - 1);
                controlR = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            // control = 0 to 1
            thickness = ((1.0 - controlR) * 2.0) - 1.0;
            out = fabs(thickness);
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            inputSampleR *= clamp;
            slew = inputSampleR - lastSampleR;
            lastSampleR = inputSampleR;
            // Set up direct reference for slew
            bridgerectifier = fabs(slew * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slew > 0) {
                slew = bridgerectifier / slewgain;
            }
            else {
                slew = -(bridgerectifier / slewgain);
            }
            inputSampleR = (lastOutSampleR * balanceA) + (lastSampleR * balanceB) + slew;
            // go from last slewed, but include some raw values
            lastOutSampleR = inputSampleR;
            // Set up slewed reference
            combSample = fabs(drySampleR * lastSampleR);
            if (combSample > 1.0) {
                combSample = 1.0;
            }
            // bailout for very high input gains
            inputSampleR -= (lastSlewR * combSample * prevslew);
            lastSlewR = slew;
            // slew interaction with previous slew
            inputSampleR *= gain;
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // drive section
            inputSampleR /= gain;
            // end of Desk section
            // end R
            gcount--;
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
} // namespace airwindohhs::tubedesk
