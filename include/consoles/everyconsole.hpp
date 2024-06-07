#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::everyconsole {

constexpr std::string_view k_name{ "EveryConsole" };
constexpr std::string_view k_short_description{
    "EveryConsole is mix-and-match Airwindows Console."
};
constexpr std::string_view k_long_description{
    "Sometimes I give people tools AS I am working on them…EveryConsole contains the original Console algorithm, Console6, Console7, the sin()/asin() routine that's the guts of Console8 and PurestConsoles 1 and 2, BShifty which is the near-sin() approximation that's in PurestConsole3 from last week, and Console Zero.All of these algorithms are stripped of all the tone shaping parts and ultrasonic filterings to be the bare-minimum functions for their purposes. That doesn't mean they are every Airwindows saturation routine: Distortion has more. But these are all the ones designed to encode and then decode on the buss.And, EveryConsole includes both the Channel functions, and the Buss functions, under one hood. So you basically select the version you want, and whether it's channel or buss.Because there's no filtering or tone shaping these lend themselves to oversampling, for instance Reaper's new oversampling. That doesn't mean it'll be better: I think it kinda won't, but if you have oversampling capacity this is now yours to fool with. EveryConsole gives you access to the raw encode/decode functions without making you use them in an Airwindows way.That's not why I made it, though. I'm working on modeling the sound of big classic consoles, and I've got a lot of audio reference, and I needed to do a lot of study of how different Console versions combined. I'm already working on doing tone shaping to get closer to the target, and I wondered whether mismatching Console versions got you types of presentations that reminded the listener of specific big consoles.That would be a YES.So I'm busily at work using this plugin to monitor a lot of variations on Console and compare them to classic records, and if I get to have a tool then so do you! So, here is EveryConsole. If you'd like to combine it with the distributed ultrasonic filtering I like to use, then load up UltrasonX or HypersonX and arrange the instances of those so they're in the right places around the EveryConsole instances. If you'd like more like Console7's ability to bring elements forward and back in mix, or Console8's tone, you have to use those as there's extra stuff built into the plugins. But if you want more basic building blocks to assemble your own Airwindows big console… this is for you as much as it is for me.Hope you like it!"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class EveryConsole final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    EveryConsole()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "contype"; break;
            case kParamB: return "intrim"; break;
            case kParamC: return "outtrim"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "ConType"; break;
            case kParamB: return "InTrim"; break;
            case kParamC: return "OutTrim"; break;

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

        double half = 0.0;
        double falf = 0.0;
        int console = (int)A * 11.999;
        double inTrim = B * 2.0; // 0-2
        double outTrim = C * 2.0;
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
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
            switch (console)
            {
                case 0:
                    half = inputSampleL * 0.83;
                    falf = fabs(half);
                    half *= falf;
                    half *= falf;
                    inputSampleL -= half;
                    half = inputSampleR * 0.83;
                    falf = fabs(half);
                    half *= falf;
                    half *= falf;
                    inputSampleR -= half;
                    break; // original ConsoleChannel, before sin/asin
                case 1:
                    half = inputSampleL * 0.885;
                    falf = fabs(half);
                    half *= falf;
                    half *= falf;
                    inputSampleL += half;
                    half = inputSampleR * 0.885;
                    falf = fabs(half);
                    half *= falf;
                    half *= falf;
                    inputSampleR += half;
                    break; // original ConsoleBuss, before sin/asin
                case 2:
                    if (inputSampleL > M_PI_2) {
                        inputSampleL = M_PI_2;
                    }
                    if (inputSampleL < -M_PI_2) {
                        inputSampleL = -M_PI_2;
                    }
                    // clip to max sine without any wavefolding
                    inputSampleL = sin(inputSampleL);
                    if (inputSampleR > M_PI_2) {
                        inputSampleR = M_PI_2;
                    }
                    if (inputSampleR < -M_PI_2) {
                        inputSampleR = -M_PI_2;
                    }
                    // clip to max sine without any wavefolding
                    inputSampleR = sin(inputSampleR);
                    break; // sin() function ConsoleChannel
                case 3:
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    // without this, you can get a NaN condition where it spits out DC offset at full blast!
                    inputSampleL = asin(inputSampleL);
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    // without this, you can get a NaN condition where it spits out DC offset at full blast!
                    inputSampleR = asin(inputSampleR);
                    break; // sin() function ConsoleBuss
                case 4:
                    // encode/decode courtesy of torridgristle under the MIT license
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    else if (inputSampleL > 0.0) {
                        inputSampleL = 1.0 - pow(1.0 - inputSampleL, 2.0);
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    else if (inputSampleL < 0.0) {
                        inputSampleL = -1.0 + pow(1.0 + inputSampleL, 2.0);
                    }
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    else if (inputSampleR > 0.0) {
                        inputSampleR = 1.0 - pow(1.0 - inputSampleR, 2.0);
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    else if (inputSampleR < 0.0) {
                        inputSampleR = -1.0 + pow(1.0 + inputSampleR, 2.0);
                    }
                    // Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5 for Console6Channel
                    break; // crude sine. Note that because modern processors love math more than extra variables, this is optimized
                case 5:
                    // encode/decode courtesy of torridgristle under the MIT license
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    else if (inputSampleL > 0.0) {
                        inputSampleL = 1.0 - pow(1.0 - inputSampleL, 0.5);
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    else if (inputSampleL < 0.0) {
                        inputSampleL = -1.0 + pow(1.0 + inputSampleL, 0.5);
                    }
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    else if (inputSampleR > 0.0) {
                        inputSampleR = 1.0 - pow(1.0 - inputSampleR, 0.5);
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    else if (inputSampleR < 0.0) {
                        inputSampleR = -1.0 + pow(1.0 + inputSampleR, 0.5);
                    }
                    // Inverse Square 1-(1-x)^2 and 1-(1-x)^0.5 for Console6Buss
                    break; // crude arcsine. Note that because modern processors love math more than extra variables, this is optimized
                case 6:
                    if (inputSampleL > 1.097) {
                        inputSampleL = 1.097;
                    }
                    if (inputSampleL < -1.097) {
                        inputSampleL = -1.097;
                    }
                    inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
                    if (inputSampleR > 1.097) {
                        inputSampleR = 1.097;
                    }
                    if (inputSampleR < -1.097) {
                        inputSampleR = -1.097;
                    }
                    inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
                    // this is a version of Spiral blended 80/20 with regular Density.
                    // It's blending between two different harmonics in the overtones of the algorithm
                    break; // Console7Channel
                case 7:
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    inputSampleL = ((asin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.618033988749894848204586) + (asin(inputSampleL) * 0.381966011250105);
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    inputSampleR = ((asin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.618033988749894848204586) + (asin(inputSampleR) * 0.381966011250105);
                    // this is an asin version of Spiral blended with regular asin ConsoleBuss.
                    // It's blending between two different harmonics in the overtones of the algorithm.
                    break; // Console7Buss
                case 8:
                    inputSampleL += ((pow(inputSampleL, 5) / 128.0) + (pow(inputSampleL, 9) / 262144.0)) - ((pow(inputSampleL, 3) / 8.0) + (pow(inputSampleL, 7) / 4096.0));
                    inputSampleR += ((pow(inputSampleR, 5) / 128.0) + (pow(inputSampleR, 9) / 262144.0)) - ((pow(inputSampleR, 3) / 8.0) + (pow(inputSampleR, 7) / 4096.0));
                    break; // crude sine. Note that because modern processors love math more than extra variables, this is optimized
                case 9:
                    inputSampleL += (pow(inputSampleL, 3) / 4.0) + (pow(inputSampleL, 5) / 8.0) + (pow(inputSampleL, 7) / 16.0) + (pow(inputSampleL, 9) / 32.0);
                    inputSampleR += (pow(inputSampleR, 3) / 4.0) + (pow(inputSampleR, 5) / 8.0) + (pow(inputSampleR, 7) / 16.0) + (pow(inputSampleR, 9) / 32.0);
                    break; // crude arcsine. Note that because modern processors love math more than extra variables, this is optimized
                case 10:
                    if (inputSampleL > 1.4137166941154) {
                        inputSampleL = 1.4137166941154;
                    }
                    if (inputSampleL < -1.4137166941154) {
                        inputSampleL = -1.4137166941154;
                    }
                    if (inputSampleL > 0.0) {
                        inputSampleL = (inputSampleL / 2.0) * (2.8274333882308 - inputSampleL);
                    }
                    else {
                        inputSampleL = -(inputSampleL / -2.0) * (2.8274333882308 + inputSampleL);
                    }
                    if (inputSampleR > 1.4137166941154) {
                        inputSampleR = 1.4137166941154;
                    }
                    if (inputSampleR < -1.4137166941154) {
                        inputSampleR = -1.4137166941154;
                    }
                    if (inputSampleR > 0.0) {
                        inputSampleR = (inputSampleR / 2.0) * (2.8274333882308 - inputSampleR);
                    }
                    else {
                        inputSampleR = -(inputSampleR / -2.0) * (2.8274333882308 + inputSampleR);
                    }
                    break; // ConsoleZeroChannel
                case 11:
                    if (inputSampleL > 2.8) {
                        inputSampleL = 2.8;
                    }
                    if (inputSampleL < -2.8) {
                        inputSampleL = -2.8;
                    }
                    if (inputSampleL > 0.0) {
                        inputSampleL = (inputSampleL * 2.0) / (3.0 - inputSampleL);
                    }
                    else {
                        inputSampleL = -(inputSampleL * -2.0) / (3.0 + inputSampleL);
                    }
                    if (inputSampleR > 2.8) {
                        inputSampleR = 2.8;
                    }
                    if (inputSampleR < -2.8) {
                        inputSampleR = -2.8;
                    }
                    if (inputSampleR > 0.0) {
                        inputSampleR = (inputSampleR * 2.0) / (3.0 - inputSampleR);
                    }
                    else {
                        inputSampleR = -(inputSampleR * -2.0) / (3.0 + inputSampleR);
                    }
                    break; // ConsoleZeroBuss
            }
            if (outTrim != 1.0) {
                inputSampleL *= outTrim;
                inputSampleR *= outTrim;
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
} // namespace airwindohhs::everyconsole
