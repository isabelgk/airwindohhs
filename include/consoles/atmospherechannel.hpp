#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::atmospherechannel {

constexpr std::string_view k_name{ "AtmosphereChannel" };
constexpr std::string_view k_short_description{
    "AtmosphereChannel is Console5 processing with powerful new acoustic distance effects."
};
constexpr std::string_view k_long_description{
    "I’m pleased with this one: it’s in the Console5 family (so its parts can be interchanged with any other Console5 plugins) but it represents a few different breakthroughs, and if they work out, I’ll be keeping them.First, this is a multi-stage slew clipper. The effect’s a bit like PDConsole, a gluing and taming of bright digital highs: but, where PDConsole uses the algorithm of PurestDrive to get a more analog mixing-desk kind of sound, Atmosphere’s many stages of slew clipping (across fourteen samples, each with a different maximum slew) enforces the behavior of free air and acoustic distance. This has been a goal for quite a while, but I’ve never got results like these before (might be worth fitting a more exaggerated version into a ‘Distance 2’, if people like). It’s not calibrated to overwhelm, but the scale of your mix should be huge, and since it’s an extended-window slew clipping effect your quieter, subtler sounds don’t get muddied.This type of processing steps on the annoying digital harshness with a heavy foot, but doesn’t directly EQ: the results you get will depend on what it would be like to have your sound blasting away through a perfect playback system at a distance of ten to thirty feet. If you need up-close, bright and loud sounds, you’ll need to use another system (such as PurestConsole), this one is for space and scale. It’ll probably work well for some electronic mixes if they’re meant to sound loud, it’ll give a real hugeness to rock or metal mixes (so long as the desired effect is more ‘live gig’ in scale) and it will really come into its own summing orchestral stuff from virtual instruments (or anything with similar scale/power needs)."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class AtmosphereChannel final : public Effect<T>
{
    double gainchase;
    double settingchase;
    double chasespeed;
    uint32_t fpdL;
    uint32_t fpdR;
    double fpNShapeL;
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleDL;
    double lastSampleEL;
    double lastSampleFL;
    double lastSampleGL;
    double lastSampleHL;
    double lastSampleIL;
    double lastSampleJL;
    double lastSampleKL;
    double lastSampleLL;
    double lastSampleML;
    double fpNShapeR;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;
    double lastSampleDR;
    double lastSampleER;
    double lastSampleFR;
    double lastSampleGR;
    double lastSampleHR;
    double lastSampleIR;
    double lastSampleJR;
    double lastSampleKR;
    double lastSampleLR;
    double lastSampleMR;
    double thresholdA;
    double thresholdB;
    double thresholdC;
    double thresholdD;
    double thresholdE;
    double thresholdF;
    double thresholdG;
    double thresholdH;
    double thresholdI;
    double thresholdJ;
    double thresholdK;
    double thresholdL;
    double thresholdM;
    float A;

  public:
    AtmosphereChannel()
    {
        A = 1.0;
        gainchase = -90.0;
        settingchase = -90.0;
        chasespeed = 350.0;
        fpNShapeL = 0.0;
        lastSampleAL = 0.0;
        lastSampleBL = 0.0;
        lastSampleCL = 0.0;
        lastSampleDL = 0.0;
        lastSampleEL = 0.0;
        lastSampleFL = 0.0;
        lastSampleGL = 0.0;
        lastSampleHL = 0.0;
        lastSampleIL = 0.0;
        lastSampleJL = 0.0;
        lastSampleKL = 0.0;
        lastSampleLL = 0.0;
        lastSampleML = 0.0;
        fpNShapeR = 0.0;
        lastSampleAR = 0.0;
        lastSampleBR = 0.0;
        lastSampleCR = 0.0;
        lastSampleDR = 0.0;
        lastSampleER = 0.0;
        lastSampleFR = 0.0;
        lastSampleGR = 0.0;
        lastSampleHR = 0.0;
        lastSampleIR = 0.0;
        lastSampleJR = 0.0;
        lastSampleKR = 0.0;
        lastSampleLR = 0.0;
        lastSampleMR = 0.0;
        thresholdA = 0.618033988749894;
        thresholdB = 0.679837387624884;
        thresholdC = 0.747821126387373;
        thresholdD = 0.82260323902611;
        thresholdE = 0.904863562928721;
        thresholdF = 0.995349919221593;
        thresholdG = 1.094884911143752;
        thresholdH = 1.204373402258128;
        thresholdI = 1.32481074248394;
        thresholdJ = 1.457291816732335;
        thresholdK = 1.603020998405568;
        thresholdL = 1.763323098246125;
        thresholdM = 1.939655408070737;
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
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
        double clamp;
        double inputgain = A;
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
        thresholdA = 0.618033988749894 / overallscale;
        thresholdB = 0.679837387624884 / overallscale;
        thresholdC = 0.747821126387373 / overallscale;
        thresholdD = 0.82260323902611 / overallscale;
        thresholdE = 0.904863562928721 / overallscale;
        thresholdF = 0.995349919221593 / overallscale;
        thresholdG = 1.094884911143752 / overallscale;
        thresholdH = 1.204373402258128 / overallscale;
        thresholdI = 1.32481074248394 / overallscale;
        thresholdJ = 1.457291816732335 / overallscale;
        thresholdK = 1.603020998405568 / overallscale;
        thresholdL = 1.763323098246125 / overallscale;
        thresholdM = 1.939655408070737 / overallscale;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
            clamp = inputSampleL - lastSampleAL;
            if (clamp > thresholdA) {
                inputSampleL = lastSampleAL + thresholdA;
            }
            if (-clamp > thresholdA) {
                inputSampleL = lastSampleAL - thresholdA;
            }
            clamp = inputSampleL - lastSampleBL;
            if (clamp > thresholdB) {
                inputSampleL = lastSampleBL + thresholdB;
            }
            if (-clamp > thresholdB) {
                inputSampleL = lastSampleBL - thresholdB;
            }
            clamp = inputSampleL - lastSampleCL;
            if (clamp > thresholdC) {
                inputSampleL = lastSampleCL + thresholdC;
            }
            if (-clamp > thresholdC) {
                inputSampleL = lastSampleCL - thresholdC;
            }
            clamp = inputSampleL - lastSampleDL;
            if (clamp > thresholdD) {
                inputSampleL = lastSampleDL + thresholdD;
            }
            if (-clamp > thresholdD) {
                inputSampleL = lastSampleDL - thresholdD;
            }
            clamp = inputSampleL - lastSampleEL;
            if (clamp > thresholdE) {
                inputSampleL = lastSampleEL + thresholdE;
            }
            if (-clamp > thresholdE) {
                inputSampleL = lastSampleEL - thresholdE;
            }
            clamp = inputSampleL - lastSampleFL;
            if (clamp > thresholdF) {
                inputSampleL = lastSampleFL + thresholdF;
            }
            if (-clamp > thresholdF) {
                inputSampleL = lastSampleFL - thresholdF;
            }
            clamp = inputSampleL - lastSampleGL;
            if (clamp > thresholdG) {
                inputSampleL = lastSampleGL + thresholdG;
            }
            if (-clamp > thresholdG) {
                inputSampleL = lastSampleGL - thresholdG;
            }
            clamp = inputSampleL - lastSampleHL;
            if (clamp > thresholdH) {
                inputSampleL = lastSampleHL + thresholdH;
            }
            if (-clamp > thresholdH) {
                inputSampleL = lastSampleHL - thresholdH;
            }
            clamp = inputSampleL - lastSampleIL;
            if (clamp > thresholdI) {
                inputSampleL = lastSampleIL + thresholdI;
            }
            if (-clamp > thresholdI) {
                inputSampleL = lastSampleIL - thresholdI;
            }
            clamp = inputSampleL - lastSampleJL;
            if (clamp > thresholdJ) {
                inputSampleL = lastSampleJL + thresholdJ;
            }
            if (-clamp > thresholdJ) {
                inputSampleL = lastSampleJL - thresholdJ;
            }
            clamp = inputSampleL - lastSampleKL;
            if (clamp > thresholdK) {
                inputSampleL = lastSampleKL + thresholdK;
            }
            if (-clamp > thresholdK) {
                inputSampleL = lastSampleKL - thresholdK;
            }
            clamp = inputSampleL - lastSampleLL;
            if (clamp > thresholdL) {
                inputSampleL = lastSampleLL + thresholdL;
            }
            if (-clamp > thresholdL) {
                inputSampleL = lastSampleLL - thresholdL;
            }
            clamp = inputSampleL - lastSampleML;
            if (clamp > thresholdM) {
                inputSampleL = lastSampleML + thresholdM;
            }
            if (-clamp > thresholdM) {
                inputSampleL = lastSampleML - thresholdM;
            }
            lastSampleML = lastSampleLL;
            lastSampleLL = lastSampleKL;
            lastSampleKL = lastSampleJL;
            lastSampleJL = lastSampleIL;
            lastSampleIL = lastSampleHL;
            lastSampleHL = lastSampleGL;
            lastSampleGL = lastSampleFL;
            lastSampleFL = lastSampleEL;
            lastSampleEL = lastSampleDL;
            lastSampleDL = lastSampleCL;
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = drySampleL;
            // store the raw L input sample again for use next time
            clamp = inputSampleR - lastSampleAR;
            if (clamp > thresholdA) {
                inputSampleR = lastSampleAR + thresholdA;
            }
            if (-clamp > thresholdA) {
                inputSampleR = lastSampleAR - thresholdA;
            }
            clamp = inputSampleR - lastSampleBR;
            if (clamp > thresholdB) {
                inputSampleR = lastSampleBR + thresholdB;
            }
            if (-clamp > thresholdB) {
                inputSampleR = lastSampleBR - thresholdB;
            }
            clamp = inputSampleR - lastSampleCR;
            if (clamp > thresholdC) {
                inputSampleR = lastSampleCR + thresholdC;
            }
            if (-clamp > thresholdC) {
                inputSampleR = lastSampleCR - thresholdC;
            }
            clamp = inputSampleR - lastSampleDR;
            if (clamp > thresholdD) {
                inputSampleR = lastSampleDR + thresholdD;
            }
            if (-clamp > thresholdD) {
                inputSampleR = lastSampleDR - thresholdD;
            }
            clamp = inputSampleR - lastSampleER;
            if (clamp > thresholdE) {
                inputSampleR = lastSampleER + thresholdE;
            }
            if (-clamp > thresholdE) {
                inputSampleR = lastSampleER - thresholdE;
            }
            clamp = inputSampleR - lastSampleFR;
            if (clamp > thresholdF) {
                inputSampleR = lastSampleFR + thresholdF;
            }
            if (-clamp > thresholdF) {
                inputSampleR = lastSampleFR - thresholdF;
            }
            clamp = inputSampleR - lastSampleGR;
            if (clamp > thresholdG) {
                inputSampleR = lastSampleGR + thresholdG;
            }
            if (-clamp > thresholdG) {
                inputSampleR = lastSampleGR - thresholdG;
            }
            clamp = inputSampleR - lastSampleHR;
            if (clamp > thresholdH) {
                inputSampleR = lastSampleHR + thresholdH;
            }
            if (-clamp > thresholdH) {
                inputSampleR = lastSampleHR - thresholdH;
            }
            clamp = inputSampleR - lastSampleIR;
            if (clamp > thresholdI) {
                inputSampleR = lastSampleIR + thresholdI;
            }
            if (-clamp > thresholdI) {
                inputSampleR = lastSampleIR - thresholdI;
            }
            clamp = inputSampleR - lastSampleJR;
            if (clamp > thresholdJ) {
                inputSampleR = lastSampleJR + thresholdJ;
            }
            if (-clamp > thresholdJ) {
                inputSampleR = lastSampleJR - thresholdJ;
            }
            clamp = inputSampleR - lastSampleKR;
            if (clamp > thresholdK) {
                inputSampleR = lastSampleKR + thresholdK;
            }
            if (-clamp > thresholdK) {
                inputSampleR = lastSampleKR - thresholdK;
            }
            clamp = inputSampleR - lastSampleLR;
            if (clamp > thresholdL) {
                inputSampleR = lastSampleLR + thresholdL;
            }
            if (-clamp > thresholdL) {
                inputSampleR = lastSampleLR - thresholdL;
            }
            clamp = inputSampleR - lastSampleMR;
            if (clamp > thresholdM) {
                inputSampleR = lastSampleMR + thresholdM;
            }
            if (-clamp > thresholdM) {
                inputSampleR = lastSampleMR - thresholdM;
            }
            lastSampleMR = lastSampleLR;
            lastSampleLR = lastSampleKR;
            lastSampleKR = lastSampleJR;
            lastSampleJR = lastSampleIR;
            lastSampleIR = lastSampleHR;
            lastSampleHR = lastSampleGR;
            lastSampleGR = lastSampleFR;
            lastSampleFR = lastSampleER;
            lastSampleER = lastSampleDR;
            lastSampleDR = lastSampleCR;
            lastSampleCR = lastSampleBR;
            lastSampleBR = lastSampleAR;
            lastSampleAR = drySampleR;
            // store the raw R input sample again for use next time
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
} // namespace airwindohhs::atmospherechannel
