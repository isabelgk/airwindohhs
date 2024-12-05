#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::golem {

constexpr std::string_view k_name{ "Golem" };
constexpr std::string_view k_short_description{
    "Golem lets you blend a stereo track of two mics on an amp."
};
constexpr std::string_view k_long_description{
    "Golem takes in a stereo track, typically two mics on a guitar cab (a popular technique among metalheads), and mixes them to mono in the middle of the track.But wait, there’s (very slightly) more!Golem lets you balance the respective inputs so you hear more of L or R, and most importantly, Golem lets you apply a delay to only the side that you want to delay. The other side is always no-latency, straight through, but the one you’re delaying gets its little sample delay. Either one, it automatically switches.Almost done…The way this control is implemented, means that small adjustments give you a tiny, tiny linear-interpolated delay. I think linear interpolating is best for guitars because it’ll scrub off the tiniest amount of ‘hiss and rattiness’ while giving the least processed sound. That said, interpolating between samples in the way this does, allows you to dial in the subtlest imaginable phase relationships between your mics, and THAT has profound effects on the subtleties of your miked guitar tone. You are tuning where the cancellations between mics are, on a very fine level, in order to do specific things in the sound, and you need easily controllable subsample delays at your fingertips. Enter Golem, your simple tool for exactly that.There are some options for allowing a larger range of delay, or inverting one of the channels, but it’s basically just mixing two tracks and being able to micro-delay one of them. Some folks will be deeply unimpressed because you have to be a bit of a fanatic to get that worked up about delaying one of the guitar cab mics five-eighths of a sample in order to place a cancellation node JUST EXACTLY where you need it to be.But you’re talking to someone who noise shapes a long double to the floating-point buss, so yay fanatics! You know who you are. This plugin is for you."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class Golem final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    // default stuff
    double p[4099];
    int count;
    float A;
    float B;
    float C;

  public:
    Golem()
    {
        A = 0.5;
        B = 0.5;
        C = 0.0;
        for (count = 0; count < 4098; count++) {
            p[count] = 0.0;
        }
        count = 0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = true;
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
            case kParamC: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "balance"; break;
            case kParamB: return "offset"; break;
            case kParamC: return "phase"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Balance"; break;
            case kParamB: return "Offset"; break;
            case kParamC: return "Phase"; break;

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

        int phase = (int)((C * 5.999) + 1);
        double balance = ((A * 2.0) - 1.0) / 2.0;
        double gainL = 0.5 - balance;
        double gainR = 0.5 + balance;
        double range = 30.0;
        if (phase == 3) {
            range = 700.0;
        }
        if (phase == 4) {
            range = 700.0;
        }
        double offset = pow((B * 2.0) - 1.0, 5) * range;
        if (phase > 4) {
            offset = 0.0;
        }
        if (phase > 5)
        {
            gainL = 0.5;
            gainR = 0.5;
        }
        int nearVal = (int)floor(fabs(offset));
        double farLevel = fabs(offset) - nearVal;
        int farVal = nearVal + 1;
        double nearLevel = 1.0 - farLevel;
        double inputSampleL;
        double inputSampleR;
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
            // assign working variables
            if (phase == 2) {
                inputSampleL = -inputSampleL;
            }
            if (phase == 4) {
                inputSampleL = -inputSampleL;
            }
            inputSampleL *= gainL;
            inputSampleR *= gainR;
            if (count < 1 || count > 2048) {
                count = 2048;
            }
            if (offset > 0)
            {
                p[count + 2048] = p[count] = inputSampleL;
                inputSampleL = p[count + nearVal] * nearLevel;
                inputSampleL += p[count + farVal] * farLevel;
                // consider adding third sample just to bring out superhighs subtly, like old interpolation hacks
                // or third and fifth samples, ditto
            }
            if (offset < 0)
            {
                p[count + 2048] = p[count] = inputSampleR;
                inputSampleR = p[count + nearVal] * nearLevel;
                inputSampleR += p[count + farVal] * farLevel;
            }
            count -= 1;
            inputSampleL = inputSampleL + inputSampleR;
            inputSampleR = inputSampleL;
            // the output is totally mono
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
} // namespace airwindohhs::golem
