#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::everyslew {

constexpr std::string_view k_name{ "EverySlew" };
constexpr std::string_view k_short_description{
    "EverySlew is a wide variety of unique sound processing tools."
};
constexpr std::string_view k_long_description{
    "Where to even begin? This is the Swiss Army Chainsaw of treble processing plugins. It'll do Slew, GoldenSlew, PlatinumSlew, or any combination of them, forwards, backwards and inside out. I hesitate even to recommend it as there is no real introduction to it, or any other plugin or processor that will really teach you how to use it. If you'd like to just tame unruly high frequencies, you can still use GoldenSlew or PlatinumSlew, and as long as you can still hear audio coming out you're fine (don't crank up Slew or GoldenSlew all the way to 1.0 unless you want to sample and hold, because they will do what you asked).But if you're still reading, here's what you get extra in EverySlew, which is mainly for me to use developing versions of Console that sound like real analog desks.Firstly, the Depth control gives you number of stages, with full crank being the equivalent of GoldenSlew or PlatinumSlew. For many sounds there will be no difference between the full ten stages those have, and much fewer. It'll be hard to hear. Fewer stages means lighter CPU load, so I'll be finding optimal settings here, but it also means less of the 'golden' effect when given extreme inputs. When used to take off only the brightest highs (more on that later) you can use fewer stages with no penalty. Two or three should suffice: 0.2 or 0.3 or so.Secondly, the Halo control gives you a terrifying, industrial meltdown of a sound that also introduces attenuation into the deep bass. I called it Halo because it brings in an odd sparkliness and energy that could come in handy. It uses another level of previous-sample to try and identify actual corners, changes in the angle of the wave, like my DeEss and Acceleration plugins do. I've never got this algorithm to work until now: having made it work, it's a strange and unruly thing. Realistic settings will all be rather low, especially if Slew is anything like obvious. HIgh Halo settings are a kind of terrible distortion I've never quite heard before: I expect this to be used in anger to make some very aggressive noises not quite like any other (which is always handy, right?)Lastly, you've now got a dry/wet. But not JUST a dry/wet, an Inv/Dry/Wet. Dry is 0.5 on the control. Crank it up and you have full wet… but set it to Inv, and you can make it cancel out the dry signal. When you do that, it shows you only the bits that are slew clipping, and silences everything else. So, for the first time, it's a slew clipper where you can monitor the clipping only, and directly hear exactly what the thing is doing. And this is handy, because the effect is very hard to hear at the levels it should be used! It will tend to make things sound exactly like they already did, except with glue and space in the mix, combing over the stray edges and points. Setting it to Inv means you can hear the points being put into place. Also, if you're using Halo, you can hear when that begins to reshape the effect, bringing in lows and so on.EverySlew isn't for everybody. I made it because I'll need it. I share it because that's what I do. If you're the sort of person to like this sort of thing, I hope you like it :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class EverySlew final : public Effect<T>
{
    enum
    {
        prevSampL1,
        prevSampR1,
        invSampL1,
        invSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        invSampL2,
        invSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        invSampL3,
        invSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        invSampL4,
        invSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        invSampL5,
        invSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        invSampL6,
        invSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        invSampL7,
        invSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        invSampL8,
        invSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        invSampL9,
        invSampR9,
        threshold9,
        prevSampL10,
        prevSampR10,
        invSampL10,
        invSampR10,
        threshold10,
        gslew_total

        public :
            EverySlew(){
                A = 0.0;
                B = 1.0;
                C = 0.16;
                D = 1.0;
                for (int x = 0; x < gslew_total; x++) gslew[x] = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
            return 0.0;
            break;
        kParamB:
            return 1.0;
            break;
        kParamC:
            return 0.16;
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
            return "slew";
            break;
        kParamB:
            return "depth";
            break;
        kParamC:
            return "halo";
            break;
        kParamD:
            return "inv/wet";
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
            return "Slew";
            break;
        kParamB:
            return "Depth";
            break;
        kParamC:
            return "Halo";
            break;
        kParamD:
            return "Inv/Wet";
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double source = pow(1 - A, 4) / overallscale;
        int stages = (1.0 - B) * 9.99;
        stages *= 5;
        double halo = C;
        double wet = (D * 2.0) - 1.0; // inv-dry-wet for highpass
        double dry = 2.0 - (D * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
        gslew[threshold10] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold9] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold8] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold7] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold6] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold5] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold4] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold3] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold2] = source;
        source *= 1.618033988749894848204586;
        gslew[threshold1] = source;
        source *= 1.618033988749894848204586;
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
            for (int x = stages; x < gslew_total; x += 5) {
                if (((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * halo)) + (gslew[x + 4] * (1.0 - halo));
                }
                if (-((inputSampleL - gslew[x]) - ((gslew[x] - gslew[x + 2]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleL = (gslew[x] - ((gslew[x] - gslew[x + 2]) * halo * 0.78)) - (gslew[x + 4] * (1.0 - (halo * 0.78)));
                }
                gslew[x + 2] = gslew[x] * (1.0 - halo);
                gslew[x] = inputSampleL;
                if (((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * halo)) + (gslew[x + 4] * (1.0 - halo));
                }
                if (-((inputSampleR - gslew[x + 1]) - ((gslew[x + 1] - gslew[x + 3]) * 0.618033988749894848204586)) > gslew[x + 4]) {
                    inputSampleR = (gslew[x + 1] - ((gslew[x + 1] - gslew[x + 3]) * halo * 0.78)) - (gslew[x + 4] * (1.0 - (halo * 0.78)));
                }
                gslew[x + 3] = gslew[x + 1] * (1.0 - halo);
                gslew[x + 1] = inputSampleR;
            }
            inputSampleL *= wet;
            inputSampleR *= wet;
            drySampleL *= dry;
            drySampleR *= dry;
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
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
} // namespace airwindohhs::everyslew
