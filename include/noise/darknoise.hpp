#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::darknoise {

constexpr std::string_view k_name{ "DarkNoise" };
constexpr std::string_view k_short_description{
    "DarkNoise is an alternative method for producing bassier noise directly. Sound design stuff."
};
constexpr std::string_view k_long_description{
    "DarkNoise is a technical experiment, that might be useful for sound design folks, or game coders, or people coding things like algorithmic clap effects. It’s basically a way to generate noise directly that’s more midrangey, or more bassy, without having to filter it (though there’s a filter included, too!)So how it works is: if you just generate rand() every sample, that’s white noise. (or if you use an algorithm like my dithering-to-the-floating-point, which is not crypto-grade noise but runs more CPU-efficiently). And if you take a value and add rand() to it every sample, that’s Brownian noise (something moves, but randomly) but it generates DC offset and needs to get filtered. I’ve also done clever forms of noise like VoiceOfTheStarship (there in your NewUpdates.zip download for free, try it and compare with DarkNoise) which do the brownian noise, but at regular intervals it forces the random noise always to move TOWARDS zero, suppressing the DC naturally.This is different. I’m not sure it’s better but it’s different, and what it does is: say you’re keeping a list of values that are all random. And you’re replacing them with new random values, and you get your output by adding ’em together. Now, imagine that for each random number you put in, that tells you the next position in the list to replace. That’s DarkNoise. It has a brighter top-end than VoiceOfTheStarship, and runs just as fast, but requires you keep a big pile of numbers around. However, you don’t have to actually add them all every sample. It’s in the code, how to work around that part.Enjoy the plugin if you like weird noise sources: again, might be sound design, maybe you’d like to gate it along with something? Gate it along with your snare and pick one of the midrangey settings and you might get a nice beefy reinforcement. Cranked way up, it gives a background noise ambience that’s like wind (heard from indoors, or being out in the wind) which can go from almost still, to thousands of miles an hour."
};
constexpr std::string_view k_tags{
    "noise"
};

template <typename T>
class DarkNoise final : public Effect<T>
{
    double rL[8193];
    double bL[11][11];
    double lastRandyL;
    double outputNoiseL;
    double rR[8193];
    double bR[11][11];
    double lastRandyR;
    double outputNoiseR;
    int freq;
    double f[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    DarkNoise()
    {
        for (int count = 0; count < 8192; count++) {
            rL[count] = 0.5;
            rR[count] = 0.5;
        }
        for (int x = 0; x < 11; x++) {
            f[x] = 0.0;
            for (int y = 0; y < 11; y++) {
                bL[x][y] = 0.0;
                bR[x][y] = 0.0;
            }
        }
        lastRandyL = 0.0;
        outputNoiseL = 0.5;
        lastRandyR = 0.0;
        outputNoiseR = 0.5;
        freq = 1;
        A = 0.5;
        B = 1.0;
        C = 1.0;
        D = 1.0;
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
            return 1.0;
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
            return "freq";
            break;
        kParamB:
            return "dark";
            break;
        kParamC:
            return "output";
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
            return "Freq";
            break;
        kParamB:
            return "Dark";
            break;
        kParamC:
            return "Output";
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
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
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

        int freqTarget = (int)(pow(A, 3) * 8192.0);
        if (freqTarget < 2) {
            freqTarget = 2;
        }
        double volumeScale = (1.0 / freqTarget) * sqrt(freqTarget);
        double overalltaps = (pow(A, 2) * 8.0) + 2.0;
        double taps = overalltaps;
        // this is our averaging, which is not integer but continuous
        double overallpoles = (A * 9.0) + 1.0;
        // this is the poles of the filter, also not integer but continuous
        int yLimit = floor(overallpoles) + 1;
        double yPartial = overallpoles - floor(overallpoles);
        // now we can do a for loop, and also apply the final pole continuously
        int xLimit = 1;
        for (int x = 0; x < 11; x++) {
            if (taps > 1.0) {
                f[x] = 1.0;
                taps -= 1.0;
                xLimit++;
            }
            else {
                f[x] = taps;
                taps = 0.0;
            }
        } // there, now we have a neat little moving average with remainders
        if (xLimit > 9) {
            xLimit = 9;
        }
        if (overalltaps < 1.0) {
            overalltaps = 1.0;
        }
        for (int x = 0; x < xLimit; x++) {
            f[x] /= overalltaps;
        } // and now it's neatly scaled, too
        double dark = B;
        double out = C * 0.5;
        double wet = D;
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
            if (freqTarget < freq) {
                outputNoiseL = ((outputNoiseL - 0.5) * (1.0 - (1.0 / freq))) + 0.5;
                outputNoiseR = ((outputNoiseR - 0.5) * (1.0 - (1.0 / freq))) + 0.5;
                rL[freq] = 0.5;
                rR[freq] = 0.5;
                freq--;
            }
            if (freqTarget > freq) {
                freq++;
            }
            // our tone control chases the input one bin at a time
            double randyL = (double)fpdL / UINT32_MAX;
            double randyR = (double)fpdR / UINT32_MAX;
            // 0 to 1 random value
            int replacementBinL = randyL * freq;
            outputNoiseL -= rL[replacementBinL];
            rL[replacementBinL] = lastRandyL;
            outputNoiseL += lastRandyL;
            lastRandyL = randyL;
            // we update only one of the slots we're using
            int replacementBinR = randyR * freq;
            outputNoiseR -= rR[replacementBinR];
            rR[replacementBinR] = lastRandyR;
            outputNoiseR += lastRandyR;
            lastRandyR = randyR;
            // we update only one of the slots we're using
            inputSampleL = (-0.5 + outputNoiseL) * volumeScale;
            inputSampleR = (-0.5 + outputNoiseR) * volumeScale;
            double nondarkSampleL = inputSampleL;
            double nondarkSampleR = inputSampleR;
            double previousPoleL = 0;
            double previousPoleR = 0;
            for (int y = 0; y < yLimit; y++) {
                for (int x = xLimit; x >= 0; x--) {
                    bL[x + 1][y] = bL[x][y];
                    bR[x + 1][y] = bR[x][y];
                }
                bL[0][y] = previousPoleL = inputSampleL;
                bR[0][y] = previousPoleR = inputSampleR;
                inputSampleL = 0.0;
                inputSampleR = 0.0;
                for (int x = 0; x < xLimit; x++) {
                    inputSampleL += (bL[x][y] * f[x]);
                    inputSampleR += (bR[x][y] * f[x]);
                }
            }
            inputSampleL = (previousPoleL * (1.0 - yPartial)) + (inputSampleL * yPartial);
            inputSampleR = (previousPoleR * (1.0 - yPartial)) + (inputSampleR * yPartial);
            // in this way we can blend in the final pole
            if (dark != 1.0) {
                inputSampleL = (inputSampleL * dark) + (nondarkSampleL * (1.0 - dark));
                inputSampleR = (inputSampleR * dark) + (nondarkSampleR * (1.0 - dark));
            }
            if (out != 1.0) {
                inputSampleL *= out;
                inputSampleR *= out;
            }
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::darknoise
