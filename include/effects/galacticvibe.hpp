#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::galacticvibe {

constexpr std::string_view k_name{ "GalacticVibe" };
constexpr std::string_view k_short_description{
    "GalacticVibe is the stereo vibrato from the original Galactic reverb."
};
constexpr std::string_view k_long_description{
    "GalacticVibe is about taking the code I used to make the Galactic reverb super wide, and breaking it out into a chorus-like effect… at which it turns out to be great!Used in Galactic, it's always full wet. It's a dual vibrato that's 'quadrature', meaning the channels are two different offsets on the same LFO. Except it's not exactly an LFO in the sense of low frequency oscillator, because it's slightly irregular. You don't hear it as being super off, but it's a somewhat organic quality whatever speed you choose.This produces two stereo outputs (even if you're just giving it mono) and the way they modulate is a bit like a miked-up Leslie: the pitch wavers across both channels. But then, if you add dry (which isn't part of Galactic's modulation) that blends against both stereo channels… and you end up with a nice little vibey chorus, with definite Leslie-like qualities, but without any of the speaker emulation and without an accurate pitch modulation on what would be the treble horns (instead, it's something else, a little smoother).So it turns out the modulation inside Galactic (Galactic2 is different) is quite nice just by itself! In particular, it seems very nice giving slow swirly effects, and ramping up to a quicker speed has a really striking 'leslie, but not leslie' quality. A happy accident that is now yours, because this is an open source, free plugin, so enjoy adding this tool to your stereo toolkit.All this is paid for by my Patreon, and the better that does the more I can do with it: as promised, the Bricasti reverb is here. Like I said I am not going to make a clone, but I am certain I can both get some useful k-series reverbs inspired by classic Bricasti patches, and design future reverb algorithms that incorporate more Bricast-isms, just from being able to hear it properly and make use of it in the studio. There's also the new Console versions coming along, currently in study mode as I explore a world of classic vinyl records heard in the fullest fidelity, and develop channel EQs that will go along with the new Console versions.Thanks, and hope you like GalacticVibe!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class GalacticVibe final : public Effect<T>
{
    double aML[3111];
    double aMR[3111];
    double vibML, vibMR, depthM, oldfpd;
    double vibM;
    int countM;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    GalacticVibe()
    {
        A = 0.0;
        B = 1.0;
        for (int count = 0; count < 3110; count++) {
            aML[count] = aMR[count] = 0.0;
        }
        vibM = 3.0;
        vibML = vibMR = depthM = 0.0;
        oldfpd = 429496.7295;
        countM = 1;
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
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drift"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drift"; break;
            case kParamB: return "Dry/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double drift = pow(A, 3) * 0.001;
        double wet = B;
        int delayM = 256;
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
            vibM += (oldfpd * drift);
            if (vibM > (3.141592653589793238 * 2.0)) {
                vibM = 0.0;
                oldfpd = 0.4294967295 + (fpdL * 0.0000000000618);
            }
            aML[countM] = inputSampleL;
            aMR[countM] = inputSampleR;
            countM++;
            if (countM < 0 || countM > delayM) {
                countM = 0;
            }
            double offsetML = (sin(vibM) + 1.0) * 127;
            double offsetMR = (sin(vibM + (3.141592653589793238 / 2.0)) + 1.0) * 127;
            int workingML = countM + offsetML;
            int workingMR = countM + offsetMR;
            double interpolML = (aML[workingML - ((workingML > delayM) ? delayM + 1 : 0)] * (1 - (offsetML - floor(offsetML))));
            interpolML += (aML[workingML + 1 - ((workingML + 1 > delayM) ? delayM + 1 : 0)] * ((offsetML - floor(offsetML))));
            double interpolMR = (aMR[workingMR - ((workingMR > delayM) ? delayM + 1 : 0)] * (1 - (offsetMR - floor(offsetMR))));
            interpolMR += (aMR[workingMR + 1 - ((workingMR + 1 > delayM) ? delayM + 1 : 0)] * ((offsetMR - floor(offsetMR))));
            inputSampleL = interpolML;
            inputSampleR = interpolMR;
            // predelay that applies vibrato
            // want vibrato speed AND depth like in MatrixVerb
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::galacticvibe
