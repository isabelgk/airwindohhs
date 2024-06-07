#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ensemble {

constexpr std::string_view k_name{ "Ensemble" };
constexpr std::string_view k_short_description{
    "Ensemble is a weird flangey little modulation effect."
};
constexpr std::string_view k_long_description{
    "Here’s a further experiment along the lines of Chorus and ChorusEnsemble! This one is more in the ‘unique because it’s kind of lame’ category. I like being able to do this sort of thing, because in this 2017 plugin business, everything you do has to be the hippest trendiest most popular thing or you’re basically doomed to get squished like a bug.But hey! I’m doing a Patreon, not a ‘business’ of selling ‘hit plugins’ that are ‘the best plugins’. And therefore, nothing’s stopping me from putting out something that’s not an emulation of some famous hardware manufacturer’s property (and putting them out of business, eventually). My stuff doesn’t have to be the target market for what people have learned to want the most over the years (often for good reason). Heck, my plugins don’t have to have a reason! And most importantly, my plugins don’t have to succeed. They can exist (and be updated, etc) even if only a few people out there like them… or even if unpopular people like them.Kind of punk, or something (stay tuned for some major DIY Airwindows stuff coming down the pike along those lines).So, here’s Ensemble. It’s a weird, unique little sound. It was meant to be a big pad thickener with great richness and depth. Well, you can throw on a bunch of bass, but it’s more like ‘cheesey string ensemble synthesizer from the 70s’, and that by accident, so it’s not even a specific (branded!) string ensemble synthesizer from the 70s. It’s kind of an annoying sound, I think.It’s free, so if you think you might have use for that, have fun with it! I’m off to make something else :)"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class Ensemble final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    const static int totalsamples = 65540;
    float dL[totalsamples];
    float dR[totalsamples];
    double sweep[49];
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
    float A;
    float B;
    float C;
    float D;

  public:
    Ensemble()
    {
        A = 0.5;
        B = 0.0;
        C = 1.0;
        D = 1.0;
        for (int count = 0; count < totalsamples - 1; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        for (int count = 0; count < 49; count++) {
            sweep[count] = 3.141592653589793238 / 2.0;
        }
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
        kParamD = 3,
        kNumParameters = 4
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 1.0;
            break;
        kParamD:
            return 1.0;
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
            return "ensemble";
            break;
        kParamB:
            return "fullness";
            break;
        kParamC:
            return "brighten";
            break;
        kParamD:
            return "dry/wet";
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
            return "Ensemble";
            break;
        kParamB:
            return "Fullness";
            break;
        kParamC:
            return "Brighten";
            break;
        kParamD:
            return "Dry/Wet";
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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "vox";
            break;
        kParamB:
            return " ";
            break;
        kParamC:
            return " ";
            break;
        kParamD:
            return " ";
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
        overallscale *= Effect<T>::getSampleRate();
        double spd = pow(0.4 + (B / 12), 10);
        spd *= overallscale;
        double depth = 0.002 / spd;
        double tupi = 3.141592653589793238 * 2.0;
        double taps = floor((A * 46.0) + 2.9);
        double brighten = C;
        double wet = D;
        // removed extra dry variable
        double hapi = 3.141592653589793238 / taps;
        double offset;
        double floffset;
        double start[49];
        double sinoffset[49];
        double speed[49];
        int count;
        int ensemble;
        double tempL;
        double tempR;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        // now we'll precalculate some stuff that needn't be in every sample
        for (count = 1; count <= taps; count++)
        {
            start[count] = depth * count;
            sinoffset[count] = hapi * (count - 1);
            speed[count] = spd / (1 + (count / taps));
        }
        // that's for speeding up things in the sample-processing area
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
            inputSampleL += (airFactorL * brighten);
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
            inputSampleR += (airFactorR * brighten);
            // air, compensates for loss of highs in flanger's interpolation
            fpFlip = !fpFlip;
            if (gcount < 1 || gcount > 32767) {
                gcount = 32767;
            }
            count = gcount;
            dL[count + 32767] = dL[count] = tempL = inputSampleL;
            dR[count + 32767] = dR[count] = tempR = inputSampleR;
            // double buffer
            for (ensemble = 1; ensemble <= taps; ensemble++)
            {
                offset = start[ensemble] + (depth * sin(sweep[ensemble] + sinoffset[ensemble]));
                floffset = offset - floor(offset);
                count = gcount + (int)floor(offset);
                tempL += dL[count] * (1 - floffset); // less as value moves away from .0
                tempL += dL[count + 1]; // we can assume always using this in one way or another?
                tempL += dL[count + 2] * floffset; // greater as value moves away from .0
                tempL -= ((dL[count] - dL[count + 1]) - (dL[count + 1] - dL[count + 2])) / 50; // interpolation hacks 'r us
                tempR += dR[count] * (1 - floffset); // less as value moves away from .0
                tempR += dR[count + 1]; // we can assume always using this in one way or another?
                tempR += dR[count + 2] * floffset; // greater as value moves away from .0
                tempR -= ((dR[count] - dR[count + 1]) - (dR[count + 1] - dR[count + 2])) / 50; // interpolation hacks 'r us
                sweep[ensemble] += speed[ensemble];
                if (sweep[ensemble] > tupi) {
                    sweep[ensemble] -= tupi;
                }
            }
            gcount--;
            // still scrolling through the samples, remember
            inputSampleL = tempL / (4.0 * sqrt(taps));
            inputSampleR = tempR / (4.0 * sqrt(taps));
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
}

};
} // namespace airwindohhs::ensemble
