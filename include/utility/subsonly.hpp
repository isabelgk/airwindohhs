#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::subsonly {

constexpr std::string_view k_name{ "SubsOnly" };
constexpr std::string_view k_short_description{
    "SubsOnly is a mix check plugin that shows you only the extreme lows."
};
constexpr std::string_view k_long_description{
    "By request, here’s my mix monitoring tools, for VST and Audio Unit, Mac and PC! Please remember, these like Guitar Conditioner are control-less plugins so you don’t operate them by opening their windows: they’re either on or off, which probably is shown in your DAW mixer window already.SlewOnly gives you what it says on the tin: only the difference between samples, expressed as a sample. This produces a super-bright sound with zero latency and absolutely no pre-ring or post-ring. You can do two poles of this filtering: just add another SlewOnly. But, as is, this will give you an incredibly clear and transparent window on the ultra-highs, so you can dial in subtleties in the treble. Beware: don’t switch right back to the normal sound or it’ll sound incredibly dull. Rest your ears for a moment before resuming work.SubsOnly does the same thing for subsonic bass. It sounds a little like a house party from an adjacent house. You can tweak sub-bass elements and really hear how they’re balancing against each other, and if you’re packing too much into the subs you’ll hear that as well. Get things thumping properly through SubsOnly and it should translate well to all manner of bassbins and subwoofers.Lastly, these are calibrated (in SlewOnly quite literally: pink noise will be about the same amplitude with or without it, though the tone will be way different) so that you can switch them in and out, and expect roughly the same loudnesses. It’s okay if SubsOnly is louder for fullbodied bass: with that, it’s not so much about level-matching, it’s about getting your sound (at whatever desired loudness) and then having the bass still make sense through SubsOnly. If you’re madly overloading it, there won’t be groove, just a lot of thunder and noise, at elevated levels. These go on at the end of your master buss, so you can hear what your 2-buss chain did to the sound. Especially if you try and go for ‘commercially loud’ volumes, sanity checking with these tools can be a real silver bullet."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class SubsOnly final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleGL;
    double iirSampleHL;
    double iirSampleIL;
    double iirSampleJL;
    double iirSampleKL;
    double iirSampleLL;
    double iirSampleML;
    double iirSampleNL;
    double iirSampleOL;
    double iirSamplePL;
    double iirSampleQL;
    double iirSampleRL;
    double iirSampleSL;
    double iirSampleTL;
    double iirSampleUL;
    double iirSampleVL;
    double iirSampleWL;
    double iirSampleXL;
    double iirSampleYL;
    double iirSampleZL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double iirSampleGR;
    double iirSampleHR;
    double iirSampleIR;
    double iirSampleJR;
    double iirSampleKR;
    double iirSampleLR;
    double iirSampleMR;
    double iirSampleNR;
    double iirSampleOR;
    double iirSamplePR;
    double iirSampleQR;
    double iirSampleRR;
    double iirSampleSR;
    double iirSampleTR;
    double iirSampleUR;
    double iirSampleVR;
    double iirSampleWR;
    double iirSampleXR;
    double iirSampleYR;
    double iirSampleZR;
    uint32_t fpdL;
    uint32_t fpdR;

  public:
    SubsOnly()
    {
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleGL = 0.0;
        iirSampleHL = 0.0;
        iirSampleIL = 0.0;
        iirSampleJL = 0.0;
        iirSampleKL = 0.0;
        iirSampleLL = 0.0;
        iirSampleML = 0.0;
        iirSampleNL = 0.0;
        iirSampleOL = 0.0;
        iirSamplePL = 0.0;
        iirSampleQL = 0.0;
        iirSampleRL = 0.0;
        iirSampleSL = 0.0;
        iirSampleTL = 0.0;
        iirSampleUL = 0.0;
        iirSampleVL = 0.0;
        iirSampleWL = 0.0;
        iirSampleXL = 0.0;
        iirSampleYL = 0.0;
        iirSampleZL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        iirSampleGR = 0.0;
        iirSampleHR = 0.0;
        iirSampleIR = 0.0;
        iirSampleJR = 0.0;
        iirSampleKR = 0.0;
        iirSampleLR = 0.0;
        iirSampleMR = 0.0;
        iirSampleNR = 0.0;
        iirSampleOR = 0.0;
        iirSamplePR = 0.0;
        iirSampleQR = 0.0;
        iirSampleRR = 0.0;
        iirSampleSR = 0.0;
        iirSampleTR = 0.0;
        iirSampleUR = 0.0;
        iirSampleVR = 0.0;
        iirSampleWR = 0.0;
        iirSampleXR = 0.0;
        iirSampleYR = 0.0;
        iirSampleZR = 0.0;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double iirAmount = 2250 / 44100.0;
        double gaintarget = 1.42;
        double gain;
        iirAmount /= overallscale;
        double altAmount = 1.0 - iirAmount;
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
            gain = gaintarget;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            iirSampleAL = (iirSampleAL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleAL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleBL = (iirSampleBL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleBL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleCL = (iirSampleCL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleCL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleDL = (iirSampleDL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleDL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleEL = (iirSampleEL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleEL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleFL = (iirSampleFL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleFL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleGL = (iirSampleGL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleGL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleHL = (iirSampleHL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleHL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleIL = (iirSampleIL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleIL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleJL = (iirSampleJL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleJL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleKL = (iirSampleKL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleKL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleLL = (iirSampleLL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleLL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleML = (iirSampleML * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleML;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleNL = (iirSampleNL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleNL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleOL = (iirSampleOL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleOL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSamplePL = (iirSamplePL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSamplePL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleQL = (iirSampleQL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleQL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleRL = (iirSampleRL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleRL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleSL = (iirSampleSL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleSL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleTL = (iirSampleTL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleTL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleUL = (iirSampleUL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleUL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleVL = (iirSampleVL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleVL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleWL = (iirSampleWL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleWL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleXL = (iirSampleXL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleXL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleYL = (iirSampleYL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleYL;
            inputSampleL *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            iirSampleZL = (iirSampleZL * altAmount) + (inputSampleL * iirAmount);
            inputSampleL = iirSampleZL;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            gain = gaintarget;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            iirSampleAR = (iirSampleAR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleAR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleBR = (iirSampleBR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleBR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleCR = (iirSampleCR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleCR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleDR = (iirSampleDR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleDR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleER = (iirSampleER * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleER;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleFR = (iirSampleFR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleFR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleGR = (iirSampleGR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleGR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleHR = (iirSampleHR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleHR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleIR = (iirSampleIR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleIR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleJR = (iirSampleJR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleJR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleKR = (iirSampleKR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleKR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleLR = (iirSampleLR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleLR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleMR = (iirSampleMR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleMR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleNR = (iirSampleNR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleNR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleOR = (iirSampleOR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleOR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSamplePR = (iirSamplePR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSamplePR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleQR = (iirSampleQR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleQR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleRR = (iirSampleRR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleRR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleSR = (iirSampleSR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleSR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleTR = (iirSampleTR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleTR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleUR = (iirSampleUR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleUR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleVR = (iirSampleVR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleVR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleWR = (iirSampleWR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleWR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleXR = (iirSampleXR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleXR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleYR = (iirSampleYR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleYR;
            inputSampleR *= gain;
            gain = ((gain - 1) * 0.75) + 1;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            iirSampleZR = (iirSampleZR * altAmount) + (inputSampleR * iirAmount);
            inputSampleR = iirSampleZR;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::subsonly
