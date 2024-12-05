#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::channel4 {

constexpr std::string_view k_name{ "Channel4" };
constexpr std::string_view k_short_description{
    "Channel4 is a tone coloring plugin that gives more of an analog feel."
};
constexpr std::string_view k_long_description{
    "Channel4 uses a very good-sounding saturation function, a calibrated slew clipper and a highpass to approximate the tone coloration of a recording console. (the plugin Console4 approximates the space and depth on the mix buss, which is different.) Because the bandwidth-limiters are set up based on measurements of real gear (through impulse responses), Channel4’s console type is labeled ‘Neve/API/SSL’. It doesn’t actually contain any of those things or any convolution kernels, but the presentation of the sound ought to suggest those voicings.There’s also an argument for doing it that way, because in many cases you can’t hear those limitations unless they’re being way overloaded. Channel’s a very clean, pure ‘coloration’ plugin, especially with the drive kept at 50% or under. It’ll voice extreme sounds the way it’s labeled, but you would struggle to consistently set plugins like Slew to those targets because mostly you can’t hear what it’s doing, especially on normal/musical sounds."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel4 final : public Effect<T>
{
    double fpNShapeLA;
    double fpNShapeLB;
    double fpNShapeRA;
    double fpNShapeRB;
    bool fpFlip;
    // default stuff
    double iirSampleLA;
    double iirSampleRA;
    double iirSampleLB;
    double iirSampleRB;
    double lastSampleL;
    double lastSampleR;
    double iirAmount;
    double threshold;
    uint32_t fpdL;
    uint32_t fpdR;
    float consoletype;
    float drive; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Channel4()
    {
        consoletype = 0.0;
        drive = 0.0;
        fpNShapeLA = 0.0;
        fpNShapeLB = 0.0;
        fpNShapeRA = 0.0;
        fpNShapeRB = 0.0;
        fpFlip = true;
        iirSampleLA = 0.0;
        iirSampleRA = 0.0;
        iirSampleLB = 0.0;
        iirSampleRB = 0.0;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        iirAmount = 0.005832;
        threshold = 0.33362176; // instantiating with Neve values
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
            case kParamA: consoletype = value; break;
            case kParamB: drive = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return consoletype; break;
            case kParamB: return drive; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "console type"; break;
            case kParamB: return "drive"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Console Type"; break;
            case kParamB: return "Drive"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(consoletype); break;
            case kParamB: return std::to_string(drive); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return "%"; break;

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
        double fpTemp; // this is different from singlereplacing
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        const double localiirAmount = iirAmount / overallscale;
        const double localthreshold = threshold / overallscale;
        const double density = pow(drive, 2); // this doesn't relate to the plugins Density and Drive much
        double clamp;
        double bridgerectifier;
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
            if (fpFlip)
            {
                iirSampleLA = (iirSampleLA * (1 - localiirAmount)) + (inputSampleL * localiirAmount);
                inputSampleL = inputSampleL - iirSampleLA;
                iirSampleRA = (iirSampleRA * (1 - localiirAmount)) + (inputSampleR * localiirAmount);
                inputSampleR = inputSampleR - iirSampleRA;
            }
            else
            {
                iirSampleLB = (iirSampleLB * (1 - localiirAmount)) + (inputSampleL * localiirAmount);
                inputSampleL = inputSampleL - iirSampleLB;
                iirSampleRB = (iirSampleRB * (1 - localiirAmount)) + (inputSampleR * localiirAmount);
                inputSampleR = inputSampleR - iirSampleRB;
            }
            // highpass section
            bridgerectifier = fabs(inputSampleL) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleL = (inputSampleL * (1 - density)) - (bridgerectifier * density);
            }
            bridgerectifier = fabs(inputSampleR) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - density)) + (bridgerectifier * density);
            }
            else {
                inputSampleR = (inputSampleR * (1 - density)) - (bridgerectifier * density);
            }
            // drive section
            clamp = inputSampleL - lastSampleL;
            if (clamp > localthreshold) {
                inputSampleL = lastSampleL + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleL = lastSampleL - localthreshold;
            }
            lastSampleL = inputSampleL;
            clamp = inputSampleR - lastSampleR;
            if (clamp > localthreshold) {
                inputSampleR = lastSampleR + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleR = lastSampleR - localthreshold;
            }
            lastSampleR = inputSampleR;
            // slew section
            // noise shaping to 64-bit floating point
            if (fpFlip) {
                fpTemp = inputSampleL;
                fpNShapeLA = (fpNShapeLA * fpOld) + ((inputSampleL - fpTemp) * fpNew);
                inputSampleL += fpNShapeLA;
                fpTemp = inputSampleR;
                fpNShapeRA = (fpNShapeRA * fpOld) + ((inputSampleR - fpTemp) * fpNew);
                inputSampleR += fpNShapeRA;
            }
            else {
                fpTemp = inputSampleL;
                fpNShapeLB = (fpNShapeLB * fpOld) + ((inputSampleL - fpTemp) * fpNew);
                inputSampleL += fpNShapeLB;
                fpTemp = inputSampleR;
                fpNShapeRB = (fpNShapeRB * fpOld) + ((inputSampleR - fpTemp) * fpNew);
                inputSampleR += fpNShapeRB;
            }
            fpFlip = !fpFlip;
            // end noise shaping on 64 bit output
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::channel4
