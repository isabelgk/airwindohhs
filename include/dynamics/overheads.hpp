#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::overheads {

constexpr std::string_view k_name{ "Overheads" };
constexpr std::string_view k_short_description{
    "Overheads is for compressing only part of the sound, strangely!"
};
constexpr std::string_view k_long_description{
    "I could say this was a compressor for putting on drum overheads to take out the drums and leave only the cymbals, but that would only be scratching the surface…Overheads is one of those old Airwindows plugins built on really strange ideas. Let's assume we want to compress drums but leave cymbals. How might we do that? We could filter, but why would we do that when we could do something more perverse? Instead, let's take a really short delay, like a flange. Next, invert it: compression gets driven by the source audio minus the delayed. Then expand the delay. Then what?Then, high frequencies tend to slip through the cracks between the delay gap. They don't get affected as strongly. But deeper frequencies will produce one part louder than the other, and subtracting produces an output that can kick in the compression. Except it might not, because the sounds might not line up. So add a control called 'Node' to move the delay gap. But how do you know what to do with 'node'? Best change it to something else: 'Sharp', for instance (which is what happened). Then what?So, put 'Sharp' in the middle somewhere. Start cranking up Compr to compress it, and you'll hear the sound squish, then negate: an area in the sound will dynamically invert, as if you're deleting the snare or kick or whatever, but it will be weird. Move 'Sharp' around to adjust it: larger 'Sharp' should let it grab slightly deeper sounds, smaller 'Sharp' shifts the cancellation up a little. Push 'Compr' further to hear what it does. To actually use it for its intended purpose, back it off so you're only slightly clamping down the drums and leaving the cymbals, making space for spot mics. It'll mess with the cymbals: see if you like how that works.Or: do whatever go nuts, do crimes, ruin everything. Put it on every drum and set them all differently. Get a really bizarre sound that's not like anything else, live long, prosper. (if you do, join my Patreon!)No promises.Oh, also, as seen in the video, if you use this on a sine wave at just the right level, it will turn the sine wave into a triangle. I totally didn't mean for it to do that, and am not really sure how it manages it. Beware. Have fun :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Overheads final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    double ovhGain;
    double ovhL[130];
    double ovhR[130];
    int ovhCount;

  public:
    Overheads()
    {
        A = 0.0;
        B = 0.5;
        C = 1.0;
        ovhGain = 1.0;
        for (int count = 0; count < 129; count++) {
            ovhL[count] = 0.0;
            ovhR[count] = 0.0;
        }
        ovhCount = 0;
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
            return 0.5;
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
            return "compr";
            break;
        kParamB:
            return "sharp";
            break;
        kParamC:
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
            return "Compr";
            break;
        kParamB:
            return "Sharp";
            break;
        kParamC:
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
        double ovhTrim = pow(A, 3);
        ovhTrim += 1.0;
        ovhTrim *= ovhTrim;
        int offset = (pow(B, 7) * 16.0 * overallscale) + 1;
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
            // begin Overheads compressor
            inputSampleL *= ovhTrim;
            inputSampleR *= ovhTrim;
            ovhCount--;
            if (ovhCount < 0 || ovhCount > 128) {
                ovhCount = 128;
            }
            ovhL[ovhCount] = inputSampleL;
            ovhR[ovhCount] = inputSampleR;
            double ovhClamp = sin(fabs(inputSampleL - ovhL[(ovhCount + offset) - (((ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
            ovhGain *= (1.0 - ovhClamp);
            ovhGain += ((1.0 - ovhClamp) * ovhClamp);
            ovhClamp = sin(fabs(inputSampleR - ovhR[(ovhCount + offset) - (((ovhCount + offset) > 128) ? 129 : 0)]) * (ovhTrim - 1.0) * 64.0);
            ovhGain *= (1.0 - ovhClamp);
            ovhGain += ((1.0 - ovhClamp) * ovhClamp);
            if (ovhGain > 1.0) {
                ovhGain = 1.0;
            }
            if (ovhGain < 0.0) {
                ovhGain = 0.0;
            }
            inputSampleL *= ovhGain;
            inputSampleR *= ovhGain;
            // end Overheads compressor
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
} // namespace airwindohhs::overheads
