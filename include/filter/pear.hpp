#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pear {

constexpr std::string_view k_name{ "Pear" };
constexpr std::string_view k_short_description{
    "Pear is the testbed for a new filter, implemented as a highpass/lowpass shelf."
};
constexpr std::string_view k_long_description{
    "Welcome to the Airwindows skunkworks, where you can get your hands on really unusual experiments!Pear is a new sort of filter I'm looking to use for my famous-console versions of Console. I've taken the concept of Holt and altered it along the lines I explored in Console Zero, and then I ran with the things I experimented in BitShiftPan a little more.So, now you have a fixed frequency shelving EQ that can be either a lowpass or a highpass, and it uses an algorithm that is literally different than anything used before, with a behavior that's likewise not like anything else (the code certainly isn't going to be found elsewhere, and the slope steepens as it nears the edge of the passband: cascading it, which this plugin lets you do, makes that end point ever-steeper rather than adding a bump to a steepening overall slope like in any normal filter)What's it sound like? Hear for yourself, it's yours. My take on it is that it's exactly what I need for the future of Console: this is not a synth filter, it's a desk filter. I can make it be really clear, even when doing extreme things at really steep slopes. It's not a scientific filter for doing really accurate curves, it's for sounding musically good.I'm not sure what the frequencies are, partly because the transition point is an increasing slope even when it ends up quite steep, and partly because it's not calculated on the basis of frequencies. Those filter points come out of the use of bit-shifting in the algorithm: it will still work in designs that don't use that, and I'll be using more carefully placed crossover points in future Console versions, but for this one, treat it as a switchable EQ. One way to get an EQ point to shift slightly is to increase or decrease the number of nodes (also stepped): consider it the digital equivalent of an analog switched circuit. Use the inv/wet control to dial in how much highpassing or lowpassing you want: that becomes your shelving control on the filter.Another part of the big-console sound is saturation, and this filter does not include that: expect what I do with this to be more intense as far as sounding like real big consoles. I'll need to configure that to suit the target console, as some are famously dirty: so often, people restoring these desks are told how to replace the dead capacitors and the original inductors, on the grounds that the original ones had no headroom (HMMMM…) but for now, Pear is very pure in tone, and it doesn't distort on its own.It's going to be a lot of work doing what I'm going to do with this tech, and that's not even counting the changes to my reverbs that will come about as I learn from the Bricasti: sure enough, I've identified stuff that I can probably do, and the result should be worth it. I'll try not to bog down and keep plugins coming out as I dig into all this! Stay tuned :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Pear final : public Effect<T>
{
    enum
    {
        prevSampL1,
        prevSlewL1,
        prevSampR1,
        prevSlewR1,
        prevSampL2,
        prevSlewL2,
        prevSampR2,
        prevSlewR2,
        prevSampL3,
        prevSlewL3,
        prevSampR3,
        prevSlewR3,
        prevSampL4,
        prevSlewL4,
        prevSampR4,
        prevSlewR4,
        prevSampL5,
        prevSlewL5,
        prevSampR5,
        prevSlewR5,
        prevSampL6,
        prevSlewL6,
        prevSampR6,
        prevSlewR6,
        prevSampL7,
        prevSlewL7,
        prevSampR7,
        prevSlewR7,
        prevSampL8,
        prevSlewL8,
        prevSampR8,
        prevSlewR8,
        prevSampL9,
        prevSlewL9,
        prevSampR9,
        prevSlewR9,
        prevSampL10,
        prevSlewL10,
        prevSampR10,
        prevSlewR10,
        pear_total

        public :
            Pear(){
                A = 1.0;
                B = 0.5;
                C = 1.0;
                for (int x = 0; x < pear_total; x++) pear[x] = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
            case kParamA: return 1.0; break;
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
            case kParamA: return "bitfreq"; break;
            case kParamB: return "poles"; break;
            case kParamC: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "bitFreq"; break;
            case kParamB: return "Poles"; break;
            case kParamC: return "Inv/Wet"; break;

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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 3) {
            cycleEnd = 3;
        }
        cycleEnd--; // this is going to be 0 for 44.1 or 48k, 1 for 88.2 or 96k, 2 for 176 or 192k.
        // DIFFERENT! Offsetting the bt shift
        int bitshiftFreq = (10 - (A * 10.0)) + cycleEnd;
        double freq = 1.0;
        switch (bitshiftFreq)
        {
            case 16: freq = 0.0000152587890625; break;
            case 15: freq = 0.000030517578125; break;
            case 14: freq = 0.00006103515625; break;
            case 13: freq = 0.0001220703125; break;
            case 12: freq = 0.000244140625; break;
            case 11: freq = 0.00048828125; break;
            case 10: freq = 0.0009765625; break;
            case 9: freq = 0.001953125; break;
            case 8: freq = 0.00390625; break;
            case 7: freq = 0.0078125; break;
            case 6: freq = 0.015625; break;
            case 5: freq = 0.03125; break;
            case 4: freq = 0.0625; break;
            case 3: freq = 0.125; break;
            case 2: freq = 0.25; break;
            case 1: freq = 0.5; break;
            case 0: freq = 1.0; break;
        }
        double maxPoles = B * pear_total;
        double wet = (C * 2.0) - 1.0; // inv-dry-wet for highpass
        double dry = 2.0 - (C * 2.0);
        if (dry > 1.0) {
            dry = 1.0; // full dry for use with inv, to 0.0 at full wet
        }
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
            // Pear is a variation on a Holt filter, made to act like my biquad filters:
            // its variables (L and R prevSample and prevSlew) contained in arrays for easier handling.
            // On top of that, the array is used to facilitate use of the filter in a stack which will
            // sharpen its very weak frequency response performance (3dB/oct per pole) and allow it to run
            // in a loop. Many earlier Airwindows filters did this 'unrolled', often with variables given
            // lettered suffixes like iirA through iirZ. The use of an array will make this a lot tidier.
            for (int x = 0; x < maxPoles; x += 4) {
                double slew = ((inputSampleL - pear[x]) + pear[x + 1]) * freq * 0.5;
                pear[x] = inputSampleL = (freq * inputSampleL) + ((1.0 - freq) * (pear[x] + pear[x + 1]));
                pear[x + 1] = slew;
                slew = ((inputSampleR - pear[x + 2]) + pear[x + 3]) * freq * 0.5;
                pear[x + 2] = inputSampleR = (freq * inputSampleR) + ((1.0 - freq) * (pear[x + 2] + pear[x + 3]));
                pear[x + 3] = slew;
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
} // namespace airwindohhs::pear
