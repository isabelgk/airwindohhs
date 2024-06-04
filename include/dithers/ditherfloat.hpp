#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ditherfloat {

constexpr std::string_view k_name{ "DitherFloat" };
constexpr std::string_view k_short_description{
    "DitherFloat is a demo of floating point truncation: can also dither to 32 bit float output."
};
constexpr std::string_view k_long_description{
    "This AirwindowsPedia entry contains the most current public domain source code for adding floating point dithering to your own work, either to a float or double precision buss (which would mean you'd have to be using something longer like long double for your internal processing).DitherFloat is NOT for putting in mixes to 'dither the plugins'. It is a demo, and the only use you could possibly do with it is to put it on a double precision buss and dither to save as 32 bit floating point instead of double. I don't think that's productive. Again, DitherFloat is NOT a plugin to use on things to make them more dithered. It's a proof of concept, like putting 'DeRez' onto dither plugins to hear how that works. You can literally hear the truncation in floating-point mantissas using this plugin, and what it sounds like when those are dithered. On to the original post when this came out…This began as a challenge. A forum poster said you couldn’t dither floating point outputs, and posted a link to a study explaining why.Floating point (including the kind that fits between every plugin in a mix on MacOS or 32-bit bussed VST) has fixed point math of the kind one needs to dither, in a part of the number called the mantissa. (64-bit VST has it too, just more finely grained.) It’s not all that hard to work out how to apply dither to this part. You scale it up or down according to the part of the number called the exponent.The trouble is (a) it’s hard enough getting people to dither to 16-bit CDs, and (b) the argument is that the amplitude of the dither would fluctuate madly, making it unhelpful and incorrect. This is kind of like how flat dither isn’t correct: with only one noise source what happens is, the noise floor fluctuates according to the waveform causing a kind of distortion. If you have a low sine wave you’ll hear the ripple effect of flat dither, and the argument is that dithering floating point is like that only more so (and so, nobody ever tried).DitherFloat demonstrates this, and it’s not true. You can TPDF dither (even PaulDither, like I’m using here) floating point. The noise doesn’t fluctuate according to the waveform represented in the mantissa. It fluctuates according to the value in the EXPONENT, because it has to, because the quantization noise also fluctuates wildly in volume. And if you get it right, you end up with no truncation distortion at all, just like using TPDF to fixed point.The video explains more and shows it working. It’s practically impossible to hear ONE stage of 32 bit truncation (may be literally impossible, I think) but you can cheat and hear it as obviously as you like. You just add a huge offset to the number, convert it to float, and then subtract the offset again: and that’s what DitherFloat does. It’s a demo. It shows you there’s still truncation in floats, and it shows you the way TPDF dither completely removes the truncation distortion. It linearizes the signal so that no trace of the truncation is present (that’s how correct dither works).You can’t add DitherFloat after existing plugins to fix them: even though you can use it (with zero offset) to export a 64 bit buss to 32 bit float and dither it, by itself DitherFloat can’t fix existing plugins. You would have to put the code for the dithering, into every single plugin that outputs 32 bit floats. Every MacOS plugin, every singlereplacing VST (every older VST implementation before they implemented 64 bit buss). For the Airwindows library that would involve personally revising every plugin I’ve released under the VST/Patreon years, many hundreds of plugins.So I did. :D(as of 2022, all the 32 bit buss plugins I make still dither to the 32 bit floats. In 2021, I experimented and decided to make my internal busses double precision instead of long double, as I wasn't able to find a difference and thought it would save some CPU; at that point, Airwindows plugins running on the VST double processing buss began outputting directly without a dither stage, for a slight efficiency boost. They still have the DitherFloat code in case the random number needs to get used, but the application of the dither is meaningless when the buss is already double, so that bit is commented out.)Below is the actual code, licensed as public domain, for both the 32 bit and the original 64 bit floating point dithers.//This is free and unencumbered software released into the public domain.//this is in the header file, defining a continuing variableuint32_t fpd; //this is an unsigned int used for the random generator//this is in reset, where we start off the variable as a default starting numberfpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;//the random generator is xorshift32 which can't start off with zero.//if the reset is randomized, all the dithers are different randoms//then in the actual processing code:int expon; frexpf((float)inputSample, &expon);fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));//Anyone is free to copy, modify, publish, use, compile, sell, or//distribute this software, either in source code form or as a compiled//binary, for any purpose, commercial or non-commercial, and by any//means.//In jurisdictions that recognize copyright laws, the author or authors//of this software dedicate any and all copyright interest in the//software to the public domain. We make this dedication for the benefit//of the public at large and to the detriment of our heirs and//successors. We intend this dedication to be an overt act of//relinquishment in perpetuity of all present and future rights to this//software under copyright law.//THE SOFTWARE IS PROVIDED " AS IS ", WITHOUT WARRANTY OF ANY KIND,//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR//OTHER DEALINGS IN THE SOFTWARE.64 bit version for hilarious overkill is almost identical but with small changes in frexp() and the constants. This is now a sort of mad community effort, so that becomes public domain too://This is free and unencumbered software released into the public domain.//this is in the header file, defining a continuing variableuint32_t fpd; //this is an unsigned int used for the random generator//this is in reset, where we start off the variable as a default starting numberfpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;//the random generator is xorshift32 which can't start off with zero.//if the reset is randomized, all the dithers are different randoms//then in the actual processing code:int expon; frexpf((double)inputSample, &expon);fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));//Anyone is free to copy, modify, publish, use, compile, sell, or//distribute this software, either in source code form or as a compiled//binary, for any purpose, commercial or non-commercial, and by any//means.//In jurisdictions that recognize copyright laws, the author or authors//of this software dedicate any and all copyright interest in the//software to the public domain. We make this dedication for the benefit//of the public at large and to the detriment of our heirs and//successors. We intend this dedication to be an overt act of//relinquishment in perpetuity of all present and future rights to this//software under copyright law.//THE SOFTWARE IS PROVIDED " AS IS ", WITHOUT WARRANTY OF ANY KIND,//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR//OTHER DEALINGS IN THE SOFTWARE.It’s not complicated but it’s my own code and it’ll work: it’s even pretty easy to drop into other plugins. If anyone’s game to do that and shout out that they did, I’d love to hear about it.If they don’t, they will just continue adding a little truncation with every single plugin, every single calculation, and now none of MY plugins do that :)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class DitherFloat final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    DitherFloat()
    {
        A = 0.0;
        B = 1.0;
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
        kNumParameters = 2

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;
            case kParamB: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "offset";
            case kParamB: return "dither";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Offset";
            case kParamB: return "Dither";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A * 32));
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
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
        int floatOffset = (A * 32);
        double blend = B;
        double gain = 0;
        switch (floatOffset)
        {
            case 0: gain = 1.0; break;
            case 1: gain = 2.0; break;
            case 2: gain = 4.0; break;
            case 3: gain = 8.0; break;
            case 4: gain = 16.0; break;
            case 5: gain = 32.0; break;
            case 6: gain = 64.0; break;
            case 7: gain = 128.0; break;
            case 8: gain = 256.0; break;
            case 9: gain = 512.0; break;
            case 10: gain = 1024.0; break;
            case 11: gain = 2048.0; break;
            case 12: gain = 4096.0; break;
            case 13: gain = 8192.0; break;
            case 14: gain = 16384.0; break;
            case 15: gain = 32768.0; break;
            case 16: gain = 65536.0; break;
            case 17: gain = 131072.0; break;
            case 18: gain = 262144.0; break;
            case 19: gain = 524288.0; break;
            case 20: gain = 1048576.0; break;
            case 21: gain = 2097152.0; break;
            case 22: gain = 4194304.0; break;
            case 23: gain = 8388608.0; break;
            case 24: gain = 16777216.0; break;
            case 25: gain = 33554432.0; break;
            case 26: gain = 67108864.0; break;
            case 27: gain = 134217728.0; break;
            case 28: gain = 268435456.0; break;
            case 29: gain = 536870912.0; break;
            case 30: gain = 1073741824.0; break;
            case 31: gain = 2147483648.0; break;
            case 32: gain = 4294967296.0; break;
        }
        // we are directly punching in the gain values rather than calculating them
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1 + (gain - 1);
            double inputSampleR = *in2 + (gain - 1);
            // begin stereo 32 bit floating point dither
            int expon;
            frexpf((float)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            inputSampleL += (fpdL * 3.4e-36l * pow(2, expon + 62) * blend); // remove 'blend' for real use, it's for the demo;
            frexpf((float)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleR += (fpdR * 3.4e-36l * pow(2, expon + 62) * blend); // remove 'blend' for real use, it's for the demo;
            // end stereo 32 bit floating point dither
            inputSampleL = (float)inputSampleL; // equivalent of 'floor' for 32 bit floating point
            inputSampleR = (float)inputSampleR; // equivalent of 'floor' for 32 bit floating point
            // We do that separately, we're truncating to floating point WHILE heavily offset.
            // note for 64 bit version: this is not for actually dithering 64 bit floats!
            // This is specifically for demonstrating the sound of 32 bit floating point dither
            // even over a 64 bit buss. Therefore it should be using float, above!
            *out1 = inputSampleL - (gain - 1);
            *out2 = inputSampleR - (gain - 1);
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::ditherfloat
