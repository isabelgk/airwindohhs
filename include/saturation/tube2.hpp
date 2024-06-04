#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tube2 {

constexpr std::string_view k_name{ "Tube2" };
constexpr std::string_view k_short_description{
    "Tube2 extends Tube, with more sophisticated processing and input trim."
};
constexpr std::string_view k_long_description{
    "Hi again! :DI took such a beating over TUUUBBEEEE!!!11!! that I’m following up immediately with another version. Not as a replacement, but just pursuing a LOT of the things people were talking about… while retaining some of the bloody-mindedness that makes people so mad at me.Folks who actually know what they’re talking about had me digging up copies of the RCA Receiving Tube Manual, to study how the electron field’s impedance can fluctuate changing the behavior of preceding stages, and how this is affected by unavoidable time delays as the electrons transit from cathode to plate through grid. (If I have that backwards, please turn your DAW upside down to compensate). There’s good reason to expect second harmonics both on the low end and as higher harmonics: all that’s in there. I did my best to find algorithms that’d sharpen corners going one direction and loosen them going the other. It’s been quite a ride. It did motivate me to code an update to Monitoring that you’ll be seeing pretty soon: adding the ‘Tube used as a safety clipper) tiny pad for appropriate output levels, and switching the new Monitoring to use Dark as its wordlength reducer. You’re hearing all that in the video too.Most of all: this is the version of Tube that’s pretty close to level matched if you have the input trim at 0.5. You can pad it more if you like. Tube2 still lets you make the audio REALLY BIG, because that’s what it’s for: I needed a safety clipper stage before I needed anything else, and it’s still designed to accurately top out at 0dB exactly, and anything you hit it with from well below to quite a bit above should all sound right.If you need more gain than you can get from cranking the input pad until it’s wide open, THAT is when you should break out Tube (1) and use it as a feeder for Tube (2). I don’t think there’s much to be gained from running Tube2 into Tube2, though I’m not your Mom and you can do as you like. I’m just saying the whole thing’s designed around finishing up in Tube2, with whatever degree of ‘Tube’ you see fit, and similar behaviors of the control: regardless of what your levels are doing, more TUBE means softer and more saturated distortion, plus all the new behaviors making stuff interesting. When you back off the TUBE control, you’re going for more linearity in every sense, and you should be able to dial in the right vibe without trouble.I have no idea whether this is gonna get me a fresh new wave of, uh, criticism :) I’m not even sure whether that would be good or whether it’s more likely to be SO good for people that they rush off and use it as intended, don’t find time to get mad, and then the whole thing dies a media death. It is said that scandal drives page views… but my own drives are rather simpler.I just want to make stuff that sounds good, along the lines I’ve been developing over more than ten years working on getting digital audio to not sound like DAW hell.This is an upgrade. Hope you like it :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Tube2 final : public Effect<T>
{
    double previousSampleA;
    double previousSampleB;
    double previousSampleC;
    double previousSampleD;
    double previousSampleE;
    double previousSampleF;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Tube2()
    {
        A = 0.5;
        B = 0.5;
        previousSampleA = 0.0;
        previousSampleB = 0.0;
        previousSampleC = 0.0;
        previousSampleD = 0.0;
        previousSampleE = 0.0;
        previousSampleF = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input";
            case kParamB: return "tube";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";
            case kParamB: return "Tube";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
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
        double inputPad = A;
        double iterations = 1.0 - B;
        int powerfactor = (9.0 * iterations) + 1;
        double asymPad = (double)powerfactor;
        double gainscaling = 1.0 / (double)(powerfactor + 1);
        double outputscaling = 1.0 + (1.0 / (double)(powerfactor));
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
            if (inputPad < 1.0) {
                inputSampleL *= inputPad;
                inputSampleR *= inputPad;
            }
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleA;
                previousSampleA = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleB;
                previousSampleB = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // flatten bottom, point top of sine waveshaper L
            inputSampleL /= asymPad;
            double sharpen = -inputSampleL;
            if (sharpen > 0.0) {
                sharpen = 1.0 + sqrt(sharpen);
            }
            else {
                sharpen = 1.0 - sqrt(-sharpen);
            }
            inputSampleL -= inputSampleL * fabs(inputSampleL) * sharpen * 0.25;
            // this will take input from exactly -1.0 to 1.0 max
            inputSampleL *= asymPad;
            // flatten bottom, point top of sine waveshaper R
            inputSampleR /= asymPad;
            sharpen = -inputSampleR;
            if (sharpen > 0.0) {
                sharpen = 1.0 + sqrt(sharpen);
            }
            else {
                sharpen = 1.0 - sqrt(-sharpen);
            }
            inputSampleR -= inputSampleR * fabs(inputSampleR) * sharpen * 0.25;
            // this will take input from exactly -1.0 to 1.0 max
            inputSampleR *= asymPad;
            // end first asym section: later boosting can mitigate the extreme
            // softclipping of one side of the wave
            // and we are asym clipping more when Tube is cranked, to compensate
            // original Tube algorithm: powerfactor widens the more linear region of the wave
            double factor = inputSampleL; // Left channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleL;
            }
            if ((powerfactor % 2 == 1) && (inputSampleL != 0.0)) {
                factor = (factor / inputSampleL) * fabs(inputSampleL);
            }
            factor *= gainscaling;
            inputSampleL -= factor;
            inputSampleL *= outputscaling;
            factor = inputSampleR; // Right channel
            for (int x = 0; x < powerfactor; x++) {
                factor *= inputSampleR;
            }
            if ((powerfactor % 2 == 1) && (inputSampleR != 0.0)) {
                factor = (factor / inputSampleR) * fabs(inputSampleR);
            }
            factor *= gainscaling;
            inputSampleR -= factor;
            inputSampleR *= outputscaling;
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleC;
                previousSampleC = stored;
                inputSampleL *= 0.5;
                stored = inputSampleR;
                inputSampleR += previousSampleD;
                previousSampleD = stored;
                inputSampleR *= 0.5;
            } // for high sample rates on this plugin we are going to do a simple average
            // end original Tube. Now we have a boosted fat sound peaking at 0dB exactly
            // hysteresis and spiky fuzz L
            double slew = previousSampleE - inputSampleL;
            if (overallscale > 1.9) {
                double stored = inputSampleL;
                inputSampleL += previousSampleE;
                previousSampleE = stored;
                inputSampleL *= 0.5;
            }
            else {
                previousSampleE = inputSampleL; // for this, need previousSampleC always
            }
            if (slew > 0.0) {
                slew = 1.0 + (sqrt(slew) * 0.5);
            }
            else {
                slew = 1.0 - (sqrt(-slew) * 0.5);
            }
            inputSampleL -= inputSampleL * fabs(inputSampleL) * slew * gainscaling;
            // reusing gainscaling that's part of another algorithm
            if (inputSampleL > 0.52) {
                inputSampleL = 0.52;
            }
            if (inputSampleL < -0.52) {
                inputSampleL = -0.52;
            }
            inputSampleL *= 1.923076923076923;
            // hysteresis and spiky fuzz R
            slew = previousSampleF - inputSampleR;
            if (overallscale > 1.9) {
                double stored = inputSampleR;
                inputSampleR += previousSampleF;
                previousSampleF = stored;
                inputSampleR *= 0.5;
            }
            else {
                previousSampleF = inputSampleR; // for this, need previousSampleC always
            }
            if (slew > 0.0) {
                slew = 1.0 + (sqrt(slew) * 0.5);
            }
            else {
                slew = 1.0 - (sqrt(-slew) * 0.5);
            }
            inputSampleR -= inputSampleR * fabs(inputSampleR) * slew * gainscaling;
            // reusing gainscaling that's part of another algorithm
            if (inputSampleR > 0.52) {
                inputSampleR = 0.52;
            }
            if (inputSampleR < -0.52) {
                inputSampleR = -0.52;
            }
            inputSampleR *= 1.923076923076923;
            // end hysteresis and spiky fuzz section
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
} // namespace airwindohhs::tube2
