#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::slewonly {

constexpr std::string_view k_name{ "SlewOnly" };
constexpr std::string_view k_short_description{
    "SlewOnly is a mix check plugin that shows you only the extreme highs."
};
constexpr std::string_view k_long_description{
    "By request, here’s my mix monitoring tools, for VST and Audio Unit, Mac and PC! Please remember, these like Guitar Conditioner are control-less plugins so you don’t operate them by opening their windows: they’re either on or off, which probably is shown in your DAW mixer window already.SlewOnly gives you what it says on the tin: only the difference between samples, expressed as a sample. This produces a super-bright sound with zero latency and absolutely no pre-ring or post-ring. You can do two poles of this filtering: just add another SlewOnly. But, as is, this will give you an incredibly clear and transparent window on the ultra-highs, so you can dial in subtleties in the treble. Beware: don’t switch right back to the normal sound or it’ll sound incredibly dull. Rest your ears for a moment before resuming work.SubsOnly does the same thing for subsonic bass. It sounds a little like a house party from an adjacent house. You can tweak sub-bass elements and really hear how they’re balancing against each other, and if you’re packing too much into the subs you’ll hear that as well. Get things thumping properly through SubsOnly and it should translate well to all manner of bassbins and subwoofers.Lastly, these are calibrated (in SlewOnly quite literally: pink noise will be about the same amplitude with or without it, though the tone will be way different) so that you can switch them in and out, and expect roughly the same loudnesses. It’s okay if SubsOnly is louder for fullbodied bass: with that, it’s not so much about level-matching, it’s about getting your sound (at whatever desired loudness) and then having the bass still make sense through SubsOnly. If you’re madly overloading it, there won’t be groove, just a lot of thunder and noise, at elevated levels. These go on at the end of your master buss, so you can hear what your 2-buss chain did to the sound. Especially if you try and go for ‘commercially loud’ volumes, sanity checking with these tools can be a real silver bullet."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class SlewOnly final : public Effect<T>
{
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    SlewOnly()
    {
        lastSampleL = 0.0;
        lastSampleR = 0.0;
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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

        double inputSampleL;
        double inputSampleR;
        double outputSampleL;
        double outputSampleR;
        double trim = 2.302585092994045684017991; // natural logarithm of 10
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
            outputSampleL = (inputSampleL - lastSampleL) * trim;
            outputSampleR = (inputSampleR - lastSampleR) * trim;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
            if (outputSampleL > 1.0) {
                outputSampleL = 1.0;
            }
            if (outputSampleR > 1.0) {
                outputSampleR = 1.0;
            }
            if (outputSampleL < -1.0) {
                outputSampleL = -1.0;
            }
            if (outputSampleR < -1.0) {
                outputSampleR = -1.0;
            }
            *out1 = outputSampleL;
            *out2 = outputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::slewonly
