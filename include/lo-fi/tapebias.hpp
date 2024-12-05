#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapebias {

constexpr std::string_view k_name{ "TapeBias" };
constexpr std::string_view k_short_description{
    "TapeBias is a new piece of ToTape allowing you to underbias or overbias."
};
constexpr std::string_view k_long_description{
    "Closing in on the big update to ToTape, and here's a helpful detail, that's not existed in ToTape before. I've been doing recalls and rebuilds of the big ToTape7 for weeks, but this little aspect more or less worked right away.Biasing a tape means adding a really high frequency to the sound, a supersonic frequency like 40k. You record to the tape, and this frequency being there means the iron oxide (or whatever metals are in use) gets jostled around, enough that slow gradual changes can be represented properly. Without it, signals kinda get stuck, It's like dither for tiny metal chunks stuck to plastic!So there are two ways to go about figuring this one out.One is to painstakingly model the whole system, in mathematical perfection, while overprocessing the heck out of everything, and then stick a virtual faceplate on it. And presumably charge a bunch of money. Not my jam.The other is to HACK AWAY like mad until you can make noises that SOUND LIKE what's happening, ideally with nice simple algorithms that will retain the digital tone better. But what even is happening?TapeBias is bypassed at 0.5. Perfect bias here means it applies NONE of the processing. In fact in the final ToTape7 (next week if all goes well!) you can literally bypass the processing at 0.5 to save CPU: it won't be doing anything anyway.If you overbias, which is commonly suggested as a good practice within reason, you're applying this high frequency tone louder and louder. That will record onto the tape too, in fact you can de-flutter using it, but it's also eating up headroom: maybe, lots of headroom. If all your tape headroom is used up trying to record a supersonic tone, and bear in mind there's a boost/cut dynamic for treble already to try and minimize noise (not even counting Dubly!) then you'll be clipping the real highs more easily if the bias is too intense.In comparison with test files recorded on real tape machines, I found that GoldenSlew did the nicest job of acting like the sound of overbiasing. TapeBias uses a very slightly changed version of GoldenSlew on overbiasing. If it's inside ToTape7 it's an even better effect because it combines with everything else, but in this case you have basically GoldenSlew for when you overbias.By contrast, there's no such existing effect that acts like underbiasing. I could refer to a real tape recording and see the odd flat bits that pop up when the biasing isn't quite enough to handle the audio. The sound is known to go a little brighter, sort of dry things up… a possible sonic effect, especially if it's a plugin and not a giant pain to recalibrate tape machines just to do.Turns out it's possible to do an algorithm that acts quite a bit like what happens with underbiasing, except that rather than put in flat bits of audio, it puts in slanted bits that do about the same thing… and it's a simple algorithm, and you can bring it in subtly or make it obvious. And it works very nicely in ToTape7, inside a full tape emulation with all its parts… but here you get it a week early, all by itself. Because, why not? Why not be able to use that part of the effect, isolated, perhaps for some sound design purpose, or to do weird things with LFOs in VCV Rack or who knows what else?So, here's TapeBias. Turn it up to overbias, turn it down to underbias. Have fun :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class TapeBias final : public Effect<T>
{
    float A;
    enum
    {
        prevSampL1,
        prevSampR1,
        threshold1,
        prevSampL2,
        prevSampR2,
        threshold2,
        prevSampL3,
        prevSampR3,
        threshold3,
        prevSampL4,
        prevSampR4,
        threshold4,
        prevSampL5,
        prevSampR5,
        threshold5,
        prevSampL6,
        prevSampR6,
        threshold6,
        prevSampL7,
        prevSampR7,
        threshold7,
        prevSampL8,
        prevSampR8,
        threshold8,
        prevSampL9,
        prevSampR9,
        threshold9,
        gslew_total
    }; // fixed frequency pear filter for ultrasonics, stereo
    double gslew[gslew_total]; // probably worth just using a number here

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    TapeBias()
    {
        A = 0.5;
        for (int x = 0; x < gslew_total; x++) {
            gslew[x] = 0.0;
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
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bias"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Bias"; break;

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
        double bias = (A * 2.0) - 1.0;
        double underBias = (pow(bias, 4) * 0.25) / overallscale;
        double overBias = pow(1.0 - bias, 3) / overallscale;
        if (bias > 0.0) {
            underBias = 0.0;
        }
        if (bias < 0.0) {
            overBias = 1.0 / overallscale;
        }
        gslew[threshold9] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold8] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold7] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold6] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold5] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold4] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold3] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold2] = overBias;
        overBias *= 1.618033988749894848204586;
        gslew[threshold1] = overBias;
        overBias *= 1.618033988749894848204586;
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
            // start bias routine
            for (int x = 0; x < gslew_total; x += 3) {
                if (underBias > 0.0) {
                    double stuck = fabs(inputSampleL - (gslew[x] / 0.975)) / underBias;
                    if (stuck < 1.0) {
                        inputSampleL = (inputSampleL * stuck) + ((gslew[x] / 0.975) * (1.0 - stuck));
                    }
                    stuck = fabs(inputSampleR - (gslew[x + 1] / 0.975)) / underBias;
                    if (stuck < 1.0) {
                        inputSampleR = (inputSampleR * stuck) + ((gslew[x + 1] / 0.975) * (1.0 - stuck));
                    }
                }
                if ((inputSampleL - gslew[x]) > gslew[x + 2]) {
                    inputSampleL = gslew[x] + gslew[x + 2];
                }
                if (-(inputSampleL - gslew[x]) > gslew[x + 2]) {
                    inputSampleL = gslew[x] - gslew[x + 2];
                }
                gslew[x] = inputSampleL * 0.975;
                if ((inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                    inputSampleR = gslew[x + 1] + gslew[x + 2];
                }
                if (-(inputSampleR - gslew[x + 1]) > gslew[x + 2]) {
                    inputSampleR = gslew[x + 1] - gslew[x + 2];
                }
                gslew[x + 1] = inputSampleR * 0.975;
            }
            // end bias routine
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
} // namespace airwindohhs::tapebias
