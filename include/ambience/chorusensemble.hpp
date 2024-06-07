#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::chorusensemble {

constexpr std::string_view k_name{ "ChorusEnsemble" };
constexpr std::string_view k_short_description{
    "ChorusEnsemble is a more complex, multi-tap mono chorus."
};
constexpr std::string_view k_long_description{
    "Here we can fill out the Airwindows palette of modulation plugins a bit… like Chorus, this is using my special slightly dark interpolation with a little pre-sparkle to get an adaptable, rich chorusing effect. But ChorusEnsemble uses a bank of chorus taps to get a more complex, textured sound that’s farther from the original. You can set it wrongly, so don’t assume all the settings are appropriate: that said, a little care should give you nice lush chorusing that’ll work great on pads and backgrounds. The reason I allow for the ‘ugly’ settings is, who’s to say you might not have a use for them, and if you find that use you’ll have a tonal element that other people don’t have on tap (generally, it’s so hard to sell plugins that can sound wrong and broken that people will tend to shun that and limit you to ‘nice’)."
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class ChorusEnsemble final : public Effect<T>
{
    const static int totalsamples = 16386;
    float dL[totalsamples];
    float dR[totalsamples];
    double sweep;
    int gcount;
    double airPrevL;
    double airEvenL;
    double airOddL;
    double airFactorL;
    double airPrevR;
    double airEvenR;
    double airOddR;
    double airFactorR;
    bool fpFlip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    ChorusEnsemble()
    {
        A = 0.5;
        B = 0.5;
        C = 0.8;
        for (int count = 0; count < totalsamples - 1; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        sweep = 3.141592653589793238 / 2.0;
        gcount = 0;
        airPrevL = 0.0;
        airEvenL = 0.0;
        airOddL = 0.0;
        airFactorL = 0.0;
        airPrevR = 0.0;
        airEvenR = 0.0;
        airOddR = 0.0;
        airFactorR = 0.0;
        fpFlip = true;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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
            case kParamC: return 0.8; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "speed"; break;
            case kParamB: return "range"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Speed"; break;
            case kParamB: return "Range"; break;
            case kParamC: return "Dry/Wet"; break;

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
            case kParamC: return ""; break;

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
        double speed = pow(A, 3) * 0.001;
        speed *= overallscale;
        int loopLimit = (int)(totalsamples * 0.499);
        int count;
        double range = pow(B, 3) * loopLimit * 0.12;
        double wet = C;
        double modulation = range * wet;
        // removed extra dry variable
        double tupi = 3.141592653589793238 * 2.0;
        double offset;
        double start[4];
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        // now we'll precalculate some stuff that needn't be in every sample
        start[0] = range;
        start[1] = range * 2;
        start[2] = range * 3;
        start[3] = range * 4;
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
            airFactorL = airPrevL - inputSampleL;
            if (fpFlip) {
                airEvenL += airFactorL;
                airOddL -= airFactorL;
                airFactorL = airEvenL;
            }
            else {
                airOddL += airFactorL;
                airEvenL -= airFactorL;
                airFactorL = airOddL;
            }
            airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
            airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
            airPrevL = inputSampleL;
            inputSampleL += (airFactorL * wet);
            // air, compensates for loss of highs in flanger's interpolation
            airFactorR = airPrevR - inputSampleR;
            if (fpFlip) {
                airEvenR += airFactorR;
                airOddR -= airFactorR;
                airFactorR = airEvenR;
            }
            else {
                airOddR += airFactorR;
                airEvenR -= airFactorR;
                airFactorR = airOddR;
            }
            airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
            airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
            airPrevR = inputSampleR;
            inputSampleR += (airFactorR * wet);
            // air, compensates for loss of highs in flanger's interpolation
            if (gcount < 1 || gcount > loopLimit) {
                gcount = loopLimit;
            }
            count = gcount;
            dL[count + loopLimit] = dL[count] = inputSampleL;
            dR[count + loopLimit] = dR[count] = inputSampleR;
            gcount--;
            // double buffer
            offset = start[0] + (modulation * sin(sweep));
            count = gcount + (int)floor(offset);
            inputSampleL = dL[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += dL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += (dL[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleL -= (((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50); // interpolation hacks 'r us
            inputSampleR = dR[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += dR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += (dR[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleR -= (((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50); // interpolation hacks 'r us
            offset = start[1] + (modulation * sin(sweep + 1.0));
            count = gcount + (int)floor(offset);
            inputSampleL += dL[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += dL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += (dL[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleL -= (((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50); // interpolation hacks 'r us
            inputSampleR += dR[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += dR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += (dR[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleR -= (((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50); // interpolation hacks 'r us
            offset = start[2] + (modulation * sin(sweep + 2.0));
            count = gcount + (int)floor(offset);
            inputSampleL += dL[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += dL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += (dL[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleL -= (((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50); // interpolation hacks 'r us
            inputSampleR += dR[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += dR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += (dR[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleR -= (((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50); // interpolation hacks 'r us
            offset = start[3] + (modulation * sin(sweep + 3.0));
            count = gcount + (int)floor(offset);
            inputSampleL += dL[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += dL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += (dL[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleL -= (((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50); // interpolation hacks 'r us
            inputSampleR += dR[count] * (1 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += dR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += (dR[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
            inputSampleR -= (((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50); // interpolation hacks 'r us
            inputSampleL *= 0.125; // to get a comparable level
            inputSampleR *= 0.125; // to get a comparable level
            sweep += speed;
            if (sweep > tupi) {
                sweep -= tupi;
            }
            // still scrolling through the samples, remember
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            fpFlip = !fpFlip;
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

} // namespace airwindohhs::chorusensemble
