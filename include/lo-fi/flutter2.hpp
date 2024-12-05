#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::flutter2 {

constexpr std::string_view k_name{ "Flutter2" };
constexpr std::string_view k_short_description{
    "Flutter2 is the flutter from ToTape7, standalone."
};
constexpr std::string_view k_long_description{
    "It's time for a new ToTape (actively being worked on, and yes you're still getting more reverbs and ConsoleX and all that) and it turns out that when I went back in to revise the tape flutter algorithm, live on stream, the work went so well that I COULD NOT WAIT. I was basically looking to address how, in the original Flutter, it slows as well as becoming gentler when you turn it down, and I thought that for 2024 ToTape7 it might be worth bringing in extra controls just to give people more power over their flutter effect.And then when I improved the algorithm and discovered that, cranked up, it does everything from shortwave radio impressions to Cookie Monsterification, that was exciting.But not as exciting as when I learned that if you crank it way up and make the flutterspeed REALLY slow, you get a heavy guitar doubler. Suddenly, I had a really decent fake hard-panned, still weirdly tight, doubled guitar that sounded fantastic apart from occasionally (and understandably) going mono on me.Enjoy Flutter2, it's going to be in the new ToTape7 when that's done. It means you have everything from reel-to-reel (less flutter than 0.5 for that), to cassette, to Roland Space Echo, to VHS, to cassette that's been yanked out of the shell and jumped up and down on for a bit and put back and attempted to be played.Also, here's my tip: rather than track one guitar and try to make it two with Flutter2, have you considered tracking three guitars and trying to make it five? If you do two real doubletracks, and then one 'thickener' to put in the center, and THEN add Flutter2 to that center track and use the dry/wet control, you can very definitely mimic FIVE guitar tracks for the price of three, and if center/flutter2 is slightly quieter it won't stick out that much when Flutter starts to hint at mono occasionally, because there's meant to be a mono track. You can lean on the Flutter2 a bit harder if you want to go superwide.Oh, which reminds me, there's also a new Srsly coming… but more on that later. Enjoy Flutter2 :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Flutter2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    double dL[1002];
    double dR[1002];
    double sweepL;
    double sweepR;
    double nextmaxL;
    double nextmaxR;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Flutter2()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        for (int temp = 0; temp < 1001; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        sweepL = M_PI;
        sweepR = M_PI;
        nextmaxL = 0.5;
        nextmaxR = 0.5;
        gcount = 0;
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
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "flutter"; break;
            case kParamB: return "speed"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Flutter"; break;
            case kParamB: return "Speed"; break;
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
        double flutDepth = pow(A, 4) * overallscale * 90;
        if (flutDepth > 498.0) {
            flutDepth = 498.0;
        }
        double flutFrequency = (0.02 * pow(B, 3)) / overallscale;
        double wet = C;
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
            // begin Flutter
            if (gcount < 0 || gcount > 999) {
                gcount = 999;
            }
            dL[gcount] = inputSampleL;
            int count = gcount;
            double offset = flutDepth + (flutDepth * sin(sweepL));
            sweepL += nextmaxL * flutFrequency;
            if (sweepL > (M_PI * 2.0)) {
                sweepL -= M_PI * 2.0;
                nextmaxL = 0.24 + (fpdL / (double)UINT32_MAX * 0.74);
            }
            count += (int)floor(offset);
            inputSampleL = (dL[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
            inputSampleL += (dL[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));
            dR[gcount] = inputSampleR;
            count = gcount;
            offset = flutDepth + (flutDepth * sin(sweepR));
            sweepR += nextmaxR * flutFrequency;
            if (sweepR > (M_PI * 2.0)) {
                sweepR -= M_PI * 2.0;
                nextmaxR = 0.24 + (fpdR / (double)UINT32_MAX * 0.74);
            }
            count += (int)floor(offset);
            inputSampleR = (dR[count - ((count > 999) ? 1000 : 0)] * (1 - (offset - floor(offset))));
            inputSampleR += (dR[count + 1 - ((count + 1 > 999) ? 1000 : 0)] * (offset - floor(offset)));
            gcount--;
            // end Flutter
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::flutter2
