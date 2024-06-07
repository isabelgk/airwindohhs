#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::channel9 {

constexpr std::string_view k_name{ "Channel9" };
constexpr std::string_view k_short_description{
    "Channel9 is Channel8 with ultrasonic filtering, and new Teac and Mackie 'low end' settings."
};
constexpr std::string_view k_long_description{
    "Channel9 retains everything it had—the two-stage drive control, the newly improved highpass, the slew clipping—the same calibrated Airwindows algorithms to not clone, but give you the general sense of various fancy name brand consoles in a cleaner, less ’emulated’ way which lets the music through…But now, Channel9 has the same ultrasonic aliasing-filtering that’s come to Console and other recent Airwindows plugins. In a new way! Because Channel9 isn’t just sticking to ’20k’ as its definition of supersonic. Instead, Channel9 steepens its filter with a teeny resonant peak at this cutoff point… and then selects it in keeping with what the real console would be doing, if you had it! The Neve is the most extended, well beyond 20k. The SSL cuts off tighter, gives more audible sparkle (due to the additional gain stages in a really huge SSL desk, the cutoff will be steeper over the entire desk). API is between the two. The lift at the peak comes before the saturation stage, for better smoothness when the console is being driven.And then I went and added two MUCH more affordable ‘models’, with their own usefulness.The new settings are ‘Teac’ and ‘Mackie’. That’s right, old school house/techno classic basement mixers! The Teac, I was able to reference recordings of a real unit. I didn’t get it perfect as the real board had a noticeable high-mids peak that doesn’t belong in Channel, but it’s the correct kind of dark and vibey. And the Mackie’s my take on what you get out of the classic vintage 1202: A hair leaner than the Teac, but brighter. They both grind a bit harder than any of the big expensive desks, they both have slightly more exaggerated reshaping of the deep bass (using the Capacitor2 algorithm, like the others), they both control the brightest highs much like you’d get in a classic old low end mixer. It’s two new settings that follow entirely different rules than the big guns, for folks who know how to use an actual mixer much like metal guitarists use a Tube Screamer.You can still have your fancy desk models (only better: the ultrasonic filtering brings just the right additional distinction to the models) but now you can go cheap and get the tone and vibe of some house music warhorses. The sonic reshaping these low end models do is just the ticket for sculpting relentlessly synthetic sounds into an appealing result. And since it’s Channel… it won’t have the noise of the real ones, it won’t have weird extra colorations, it’ll honor more of your real mix as it reshapes it: sort of best of both worlds! You can always add funky colorations or noise to your digital mix, but you can’t remove that stuff from the real mixers. Channel9 will shape your sound in ‘classic’ ways but along Airwindows lines: getting out of the way so your sound is interfered with as little as possible.Channel9 can be used anywhere you like. If you’re doing a Console mix, I’d put it after ConsoleBuss. If you’re not doing a Console mix, you can literally do anything you want with it: it’s a subtle distortion/fattener combined with a set of careful tone shaping algorithms. Hope you like it!"
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class Channel9 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleLA;
    double iirSampleRA;
    double iirSampleLB;
    double iirSampleRB;
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;
    double biquadA[15];
    double biquadB[15];
    double iirAmount;
    double threshold;
    double cutoff;
    bool flip;
    float A;
    float B;
    float C;

  public:
    Channel9()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
        }
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        iirSampleLA = 0.0;
        iirSampleRA = 0.0;
        iirSampleLB = 0.0;
        iirSampleRB = 0.0;
        lastSampleAL = lastSampleBL = lastSampleCL = 0.0;
        lastSampleAR = lastSampleBR = lastSampleCR = 0.0;
        flip = false;
        iirAmount = 0.005832;
        threshold = 0.33362176; // instantiating with Neve values
        cutoff = 28811.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
            return 0.0;
            break;
        kParamC:
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
            return "console type";
            break;
        kParamB:
            return "drive";
            break;
        kParamC:
            return "output";
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
            return "Console Type";
            break;
        kParamB:
            return "Drive";
            break;
        kParamC:
            return "Output";
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
            return "%";
            break;
        kParamC:
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double localiirAmount = iirAmount / overallscale;
        double localthreshold = threshold; // we've learned not to try and adjust threshold for sample rate
        double density = B * 2.0; // 0-2
        double phattity = density - 1.0;
        if (density > 1.0) {
            density = 1.0; // max out at full wet for Spiral aspect
        }
        if (phattity < 0.0) {
            phattity = 0.0; //
        }
        double nonLin = 5.0 - density; // number is smaller for more intense, larger for more subtle
        biquadB[0] = biquadA[0] = cutoff / getSampleRate();
        biquadA[1] = 1.618033988749894848204586;
        biquadB[1] = 0.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
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
            double tempSample;
            if (biquadA[0] < 0.49999) {
                tempSample = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
                biquadA[8] = biquadA[7];
                biquadA[7] = inputSampleL;
                if (fabs(tempSample) < 1.18e-37) {
                    tempSample = 0.0;
                }
                inputSampleL = tempSample;
                biquadA[10] = biquadA[9];
                biquadA[9] = inputSampleL; // DF1 left
                tempSample = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
                biquadA[12] = biquadA[11];
                biquadA[11] = inputSampleR;
                if (fabs(tempSample) < 1.18e-37) {
                    tempSample = 0.0;
                }
                inputSampleR = tempSample;
                biquadA[14] = biquadA[13];
                biquadA[13] = inputSampleR; // DF1 right
            }
            double dielectricScaleL = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
            double dielectricScaleR = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));
            if (flip)
            {
                if (fabs(iirSampleLA) < 1.18e-37) {
                    iirSampleLA = 0.0;
                }
                iirSampleLA = (iirSampleLA * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
                inputSampleL = inputSampleL - iirSampleLA;
                if (fabs(iirSampleRA) < 1.18e-37) {
                    iirSampleRA = 0.0;
                }
                iirSampleRA = (iirSampleRA * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
                inputSampleR = inputSampleR - iirSampleRA;
            }
            else
            {
                if (fabs(iirSampleLB) < 1.18e-37) {
                    iirSampleLB = 0.0;
                }
                iirSampleLB = (iirSampleLB * (1.0 - (localiirAmount * dielectricScaleL))) + (inputSampleL * localiirAmount * dielectricScaleL);
                inputSampleL = inputSampleL - iirSampleLB;
                if (fabs(iirSampleRB) < 1.18e-37) {
                    iirSampleRB = 0.0;
                }
                iirSampleRB = (iirSampleRB * (1.0 - (localiirAmount * dielectricScaleR))) + (inputSampleR * localiirAmount * dielectricScaleR);
                inputSampleR = inputSampleR - iirSampleRB;
            }
            // highpass section
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            double phatSampleL = sin(inputSampleL * 1.57079633);
            inputSampleL *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            double distSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleL = distSampleL; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleL = (drySampleL * (1 - density)) + (distSampleL * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleL = (inputSampleL * (1 - phattity)) + (phatSampleL * phattity); // apply original Density on top
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            double phatSampleR = sin(inputSampleR * 1.57079633);
            inputSampleR *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            double distSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            inputSampleR = distSampleR; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleR = (drySampleR * (1 - density)) + (distSampleR * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleR = (inputSampleR * (1 - phattity)) + (phatSampleR * phattity); // apply original Density on top
            }
            // begin L
            double clamp = (lastSampleBL - lastSampleCL) * 0.381966011250105;
            clamp -= (lastSampleAL - lastSampleBL) * 0.6180339887498948482045;
            clamp += inputSampleL - lastSampleAL; // regular slew clamping added
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = inputSampleL; // now our output relates off lastSampleB
            if (clamp > localthreshold) {
                inputSampleL = lastSampleBL + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleL = lastSampleBL - localthreshold;
            }
            lastSampleAL = (lastSampleAL * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end L
            // begin R
            clamp = (lastSampleBR - lastSampleCR) * 0.381966011250105;
            clamp -= (lastSampleAR - lastSampleBR) * 0.6180339887498948482045;
            clamp += inputSampleR - lastSampleAR; // regular slew clamping added
            lastSampleCR = lastSampleBR;
            lastSampleBR = lastSampleAR;
            lastSampleAR = inputSampleR; // now our output relates off lastSampleB
            if (clamp > localthreshold) {
                inputSampleR = lastSampleBR + localthreshold;
            }
            if (-clamp > localthreshold) {
                inputSampleR = lastSampleBR - localthreshold;
            }
            lastSampleAR = (lastSampleAR * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045); // split the difference between raw and smoothed for buffer
            // end R
            flip = !flip;
            if (C < 1.0) {
                inputSampleL *= C;
                inputSampleR *= C;
            }
            if (biquadB[0] < 0.49999) {
                tempSample = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
                biquadB[8] = biquadB[7];
                biquadB[7] = inputSampleL;
                if (fabs(tempSample) < 1.18e-37) {
                    tempSample = 0.0;
                }
                inputSampleL = tempSample;
                biquadB[10] = biquadB[9];
                biquadB[9] = inputSampleL; // DF1 left
                tempSample = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
                biquadB[12] = biquadB[11];
                biquadB[11] = inputSampleR;
                if (fabs(tempSample) < 1.18e-37) {
                    tempSample = 0.0;
                }
                inputSampleR = tempSample;
                biquadB[14] = biquadB[13];
                biquadB[13] = inputSampleR; // DF1 right
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
} // namespace airwindohhs::channel9
