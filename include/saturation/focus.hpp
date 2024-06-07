#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::focus {

constexpr std::string_view k_name{ "Focus" };
constexpr std::string_view k_short_description{
    "Focus brings out clarity by distorting. Aggressive, subtle, flexible."
};
constexpr std::string_view k_long_description{
    "Focus creates two audio paths, one of them a bandpass and the other, everything BUT the bandpass. Then, you distort what's in the bandpass, which will help the result avoid aliasing even if you're not at high sample rates. The reference to UnBox is because I wrote that one first, and then extended it into Focus. Here's the original post.What if you took UnBox, with its aliasing-resistant distortion, and revised it around a band pass? Not so much an EQ thing, more a ‘Fletcher Munson Loudness Curve’ thing, where you could zero in on just that most sensitive area, and add distortion to just that? And use the UnBox tech so you’d still have undistorted clear super-lows and highs, as much as you wanted? And set an output level so that if you DID end up driving the mids real hard, you could dial it back so that you got your focussing effect but without apparent mids boost? Or mids boost if you like, whichever. And a dry/wet on the end for added subtlety and flexibility. And then, how about if you could pick between Density, Drive, Spiral, Mojo and Dyno distortion algorithms?Here’s Focus. If you keep the Focus control real low, it’ll act like a broad-based overdrive, five different ways. Kill the Output level control, and you’ll hear what’s left over from the UnBox tech. Start to bring up Focus to about halfway, and you will rapidly start narrowing the distortion band and letting through lots more lows and highs, while also (if you turn Output back up) tightening and purifying what you distort in the mids. Focus even more, and you’ll get to where it’s an ultra-focussed mid laser that you can distort, that’ll cut through anything. if you want to start tuning it, dial Focus back: this is not about that, it’s exactly targetting the Fletcher-Munson loudness curves, think of it in terms of focussing the midrange and making it more intense. At low Focus settings, tuning it would be almost meaningless.This is ‘dial an ultimate distortion, ITB’. Even more than the ‘swiss army knife’ one I’ll likely still make later. This becomes the recommended one. It replaces Density, Drive, all versions of Spiral, etc… because its principle of operation gives you essential Airwindows distortions, using UnBox tech to resist aliasing and allow for clean bass, and because the way it’s set up you can immediately go to whatever you’re trying to achieve. Big roaring fuzzy? Density with very low Focus, or possibly Mojo. Gritty? Drive. Cleaner overdrive? Spiral. Need to clean things up but still have that pungent, fierce energy from an amazing distortion? Start increasing Focus, carefully. Trying to do an insanely focussed searing-hot distortion with high Focus but things are too saturated? Reach for Dyno and its distinctive overtones. Pretty much anything you want, in one plugin, but with very approachable controls plus you can do things like kill Output Level to quickly check how much clean audio is getting through in the highs and lows.Don’t think of it like an EQ, think of it like an extension of your ears. Whether it’s on tracks, submixes, or the 2-buss, Focus can locate anything accurately in your listeners’ attention, because we all have the same sensitivities and loudness curves to our hearing (most likely). Use with caution (unless you don’t want to!) and enjoy a real power tool for mix clarity, with Focus.(followup: the reference to 'swiss army knife' distortion is to the plugin Distortion, which is also available now)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Focus final : public Effect<T>
{
    double figureL[9];
    double figureR[9];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Focus()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        E = 1.0;
        for (int x = 0; x < 9; x++) {
            figureL[x] = 0.0;
            figureR[x] = 0.0;
        }
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
        kParamE = 4,
        kNumParameters = 5
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
        kParamE:
            E = value;
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
        kParamE:
            return E;
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
            return 0.0;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 1.0;
            break;
        kParamE:
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
            return "boost";
            break;
        kParamB:
            return "focus";
            break;
        kParamC:
            return "mode";
            break;
        kParamD:
            return "output";
            break;
        kParamE:
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
            return "Boost";
            break;
        kParamB:
            return "Focus";
            break;
        kParamC:
            return "Mode";
            break;
        kParamD:
            return "Output";
            break;
        kParamE:
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
        kParamE:
            return std::to_string(E);
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
            return "dB";
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
        kParamE:
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

        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        double boost = pow(10.0, (A * 12.0) / 20.0);
        figureL[0] = figureR[0] = 3515.775 / getSampleRate(); // fixed frequency, 3.515775k
        figureL[1] = figureR[1] = pow(pow(B, 3) * 2, 2) + 0.0001; // resonance
        int mode = (int)(C * 4.999);
        double output = D;
        double wet = E;
        double K = tan(M_PI * figureR[0]);
        double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
        figureL[2] = figureR[2] = K / figureR[1] * norm;
        figureL[4] = figureR[4] = -figureR[2];
        figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
        figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double tempSample = (inputSampleL * figureL[2]) + figureL[7];
            figureL[7] = -(tempSample * figureL[5]) + figureL[8];
            figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
            inputSampleL = tempSample;
            tempSample = (inputSampleR * figureR[2]) + figureR[7];
            figureR[7] = -(tempSample * figureR[5]) + figureR[8];
            figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
            inputSampleR = tempSample;
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // decode Console5
            double groundSampleL = drySampleL - inputSampleL; // set up UnBox
            double groundSampleR = drySampleR - inputSampleR; // set up UnBox
            inputSampleL *= boost; // now, focussed area gets cranked before distort
            inputSampleR *= boost; // now, focussed area gets cranked before distort
            switch (mode)
            {
                case 0: // Density
                    if (inputSampleL > 1.570796326794897) {
                        inputSampleL = 1.570796326794897;
                    }
                    if (inputSampleL < -1.570796326794897) {
                        inputSampleL = -1.570796326794897;
                    }
                    if (inputSampleR > 1.570796326794897) {
                        inputSampleR = 1.570796326794897;
                    }
                    if (inputSampleR < -1.570796326794897) {
                        inputSampleR = -1.570796326794897;
                    }
                    // clip to 1.570796326794897 to reach maximum output
                    inputSampleL = sin(inputSampleL);
                    inputSampleR = sin(inputSampleR);
                    break;
                case 1: // Drive
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
                    inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.6) * (fabs(inputSampleL) * 0.6));
                    inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.6) * (fabs(inputSampleR) * 0.6));
                    inputSampleL *= 1.6;
                    inputSampleR *= 1.6;
                    break;
                case 2: // Spiral
                    if (inputSampleL > 1.2533141373155) {
                        inputSampleL = 1.2533141373155;
                    }
                    if (inputSampleL < -1.2533141373155) {
                        inputSampleL = -1.2533141373155;
                    }
                    if (inputSampleR > 1.2533141373155) {
                        inputSampleR = 1.2533141373155;
                    }
                    if (inputSampleR < -1.2533141373155) {
                        inputSampleR = -1.2533141373155;
                    }
                    // clip to 1.2533141373155 to reach maximum output
                    inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
                    inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
                    break;
                case 3: // Mojo
                    double mojo;
                    mojo = pow(fabs(inputSampleL), 0.25);
                    if (mojo > 0.0) {
                        inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo) * 0.987654321;
                    }
                    mojo = pow(fabs(inputSampleR), 0.25);
                    if (mojo > 0.0) {
                        inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo) * 0.987654321;
                    }
                    // mojo is the one that flattens WAAAAY out very softly before wavefolding
                    break;
                case 4: // Dyno
                    double dyno;
                    dyno = pow(fabs(inputSampleL), 4);
                    if (dyno > 0.0) {
                        inputSampleL = (sin(inputSampleL * dyno) / dyno) * 1.1654321;
                    }
                    dyno = pow(fabs(inputSampleR), 4);
                    if (dyno > 0.0) {
                        inputSampleR = (sin(inputSampleR * dyno) / dyno) * 1.1654321;
                    }
                    // dyno is the one that tries to raise peak energy
                    break;
            }
            if (output != 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            inputSampleL += groundSampleL; // effectively UnBox
            inputSampleR += groundSampleR; // effectively UnBox
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
};
} // namespace airwindohhs::focus
