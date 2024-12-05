#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::zoom {

constexpr std::string_view k_name{ "Zoom" };
constexpr std::string_view k_short_description{
    "Zoom is a bank of Density-like waveshapers for subtly modulating peak energy."
};
constexpr std::string_view k_long_description{
    "This is a little piece of what could become a new Mastering plugin, but I've got a lot of testing to do to work out whether that should happen. Zoom is waveshapers: as many as five stacked up for high gain levels, and most settings will use at least a couple. They're applied sort of in parallel: the basic idea is a lot like Density, but handled very differently.The purpose is to try and alter, very strongly, the peak energy and only the peak energy of the sound, while leaving the loudness more or less alone. That's why the 'negative' settings, below 0.5, don't get as much quieter as those in Density. It's taking the hottest peaks and further boosting them while cutting back quieter stuff. Boosts still work as in Density, except the character will be a little different.So, you might find the sounds of this interesting, or find that it's an amazing overdrive or 'weird expander', or whatever, but those are all obvious changes that you can hear because they're modifying the RMS loudness of the signal. And that's fine too. I'm just saying, for the last month I've been on a deep dive into Meter and the concepts around it, and I'm working towards something specific. I would like to see whether you can take an already good track and make it significantly better by heavily modifying just the PEAK energy and getting that exactly the way I want it. A new version of Meter is coming out and I'll be doing more 'vinyl hit record videos' to demonstrate what I'm talking about, and to do that I have to write new plugins that specifically master audio to work with Meter and work on the stuff I 'can't hear'… at least, not directly. If I'm right, perfecting this stuff will cause tracks to sound inexplicably amazing (and they will have to start out with the potential for amazing: some don't need help, and for others it's very obvious what must happen.)For instance, I might have various 80s hit records where, due to the taste in mixing and mastering, there's constantly stray peaks 6 to 12 dB hotter than everything else, and I'd need to remap them to be a more continuous range of peak energy. Or, the area is too dense and could use being opened up subtly, allowing the hottest peaks to 'un-saturate' slightly without it being really audible, again to get me an area of peak energy that's as evenly distributed as possible.And that's what Zoom was coded to do, and why it's hard to hear what it's doing until you crank it up to the point of obviousness. It is also a simple waveshaper so you can do that while oversampling it in the DAW, which of course I won't be doing because used normally it'll barely create any harmonics at all. It's a 'zoom lens' for audio: everything is still in focus and looks the same, but the presentation has shifted in the desired direction, probably only discernable by Meter.I might still be working on Meter and far from ready to put out a Mastering that's entirely based on my untested theories, but the new Meter should be coming up just next week, and if you wanted to experiment as I am experimenting (or just slam stuff, I'm not your Mom) then here is Zoom. Have fun!"
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class Zoom final : public Effect<T>
{
    float A;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Zoom()
    {
        A = 0.5;
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
            case kParamA: return "zoom"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Zoom"; break;

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

        double zoom = (A * 2.0) - 1.0;
        double zoomStages = (fabs(zoom) * 4.0) + 1.0;
        for (int count = 0; count < sqrt(zoomStages); count++) {
            zoom *= fabs(zoom);
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
            for (int count = 0; count < zoomStages; count++) {
                if (zoom > 0.0) {
                    double closer = inputSampleL * 1.57079633;
                    if (closer > 1.57079633) {
                        closer = 1.57079633;
                    }
                    if (closer < -1.57079633) {
                        closer = -1.57079633;
                    }
                    inputSampleL = (inputSampleL * (1.0 - zoom)) + (sin(closer) * zoom);
                    closer = inputSampleR * 1.57079633;
                    if (closer > 1.57079633) {
                        closer = 1.57079633;
                    }
                    if (closer < -1.57079633) {
                        closer = -1.57079633;
                    }
                    inputSampleR = (inputSampleR * (1.0 - zoom)) + (sin(closer) * zoom);
                } // zooming in will make the body of the sound louder: it's just Density
                if (zoom < 0.0) {
                    double farther = fabs(inputSampleL) * 1.57079633;
                    if (farther > 1.57079633) {
                        farther = 1.0;
                    }
                    else {
                        farther = 1.0 - cos(farther);
                    }
                    if (inputSampleL > 0.0) {
                        inputSampleL = (inputSampleL * (1.0 + zoom)) - (farther * zoom * 1.57079633);
                    }
                    if (inputSampleL < 0.0) {
                        inputSampleL = (inputSampleL * (1.0 + zoom)) + (farther * zoom * 1.57079633);
                    }
                    farther = fabs(inputSampleR) * 1.57079633;
                    if (farther > 1.57079633) {
                        farther = 1.0;
                    }
                    else {
                        farther = 1.0 - cos(farther);
                    }
                    if (inputSampleR > 0.0) {
                        inputSampleR = (inputSampleR * (1.0 + zoom)) - (farther * zoom * 1.57079633);
                    }
                    if (inputSampleR < 0.0) {
                        inputSampleR = (inputSampleR * (1.0 + zoom)) + (farther * zoom * 1.57079633);
                    }
                } // zooming out boosts the hottest peaks but cuts back softer stuff
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
} // namespace airwindohhs::zoom
