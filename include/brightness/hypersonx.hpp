#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hypersonx {

constexpr std::string_view k_name{ "HypersonX" };
constexpr std::string_view k_short_description{
    "HypersonX is like UltrasonX but with seven stages instead of five."
};
constexpr std::string_view k_long_description{
    "This one’s pretty straightforward. If you know UltrasonX, this is the same thing but for Hypersonic. These are ultrasonic filters for use at high sample rates such as 96k: for less processing than it would take to oversample a bunch of stuff you can run the whole mix at an elevated sample rate and put filtering between nonlinear stages: it works better to do a little filtering in many places, and it helps the mix from seeming overprocessed.HypersonX is steeper, because it’s seven stages instead of only five. It cuts off a little higher, so it should go right past 20k without hinting at rolling off or softening anything. But since it (like Hypersonic itself) is seven stages, that means you’ve got to find seven spots in your mix to stick the respective stages of the filter.Like UltrasonX (and like future Console versions I do that will use this technology), you have to have one each of every stage of HypersonX, from input to final 2-buss. That means you don’t have to repeat ’em across all channels: for instance, if you’re running stage A and B on the channels, and C, D and E on submixes, and F and G on the 2-buss, that’s way less filters than you’d have if you placed just one Hypersonic on the start of every single channel, AND it will work better because it’ll keep every little nonlinear stage in check. Otherwise, they can seize on small bits of aliasing and alias them further until they combine and become obvious. Single filtering at the input won’t protect you from this. Distributed filtering will.This might not be for everybody. The thing is, if you’re running at high sample rates and you want to resist aliasing and have a warm, analog tone that still retains clarity, this might be for you… and similarly to other Airwindows inventions, the cumbersome nature of HypersonX could become your secret weapon. Who wants to carefully arrange to have one each of seven types of inaudible filtering across your signal path, from channel start to end of 2-buss? Possibly you, if the resulting tone speaks to your musical tastes. The barrier to entry is a setup that’s a pain in the butt and fussy rules for how it works (you can combine it with the original PurestConsole if you’d like to also have the cleanest possible Airwindows Console mix, that’s a whole other set of fussy rules!) but you might find the results a little spectacular.I hope so! The next thing for me is building this into the next version(s) of Console, and I believe it will help things a whole lot. But with HypersonX, you don’t have to wait. Use it on its own or combine it with any earlier version of Console (not Console7 or PurestConsole2, which have their own filtering, but Console 5 or 6 or PurestConsole or Atmosphere or PDConsole which didn’t have ultrasonic filtering built in) and begin constructing your own in-house mixing board, and get your personal sound.(followup: this became the ultrasonic filter tech for Console8)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class HypersonX final : public Effect<T>
{
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total

        public :
            HypersonX(){
                A = 0.5;
                for (int x = 0; x < fix_total; x++) fixA[x] = 0.0;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
            case kParamA: return "q"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Q"; break;

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

        double cutoff = 25000.0 / Effect<T>::getSampleRate();
        if (cutoff > 0.49) {
            cutoff = 0.49; // don't crash if run at 44.1k
        }
        fixA[fix_freq] = cutoff;
        switch ((VstInt32)(A * 6.999))
        {
            case kA:
                fixA[fix_reso] = 4.46570214;
                break;
            case kB:
                fixA[fix_reso] = 1.51387132;
                break;
            case kC:
                fixA[fix_reso] = 0.93979296;
                break;
            case kD:
                fixA[fix_reso] = 0.70710678; // butterworth Q
                break;
            case kE:
                fixA[fix_reso] = 0.59051105;
                break;
            case kF:
                fixA[fix_reso] = 0.52972649;
                break;
            case kG:
                fixA[fix_reso] = 0.50316379;
                break;
        }
        double K = tan(M_PI * fixA[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
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
            double outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            // begin 64 bit stereo floating point dither
            // only relevant if buss is double
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
} // namespace airwindohhs::hypersonx
