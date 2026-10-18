#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::purestconsole2channel {

constexpr std::string_view k_name{ "PurestConsole2Channel" };
constexpr std::string_view k_short_description{
    "PurestConsole2Channel is the distributed filter version of PurestConsole."
};
constexpr std::string_view k_long_description{
    "Meet PurestConsole2. This serves a special purpose in modern Airwindows Console mixes. I’ve got Console7, which gives you all the glue you could want, creates solidity and the sense of an analog mixing desk (all the more if you use Console7Cascade, which I’m avoiding for this track and this video). But Console7 does its aliasing prevention by rolling off right at 20k, which helps the sense of glue but steps on some of the super-sparkly treble you sometimes get in modern mixes. It’s set up to do it really gracefully, but some have noticed a diminishing of super-glittery highs. PurestConsole was always the ‘colorless, transparent’ take on any Console system, but with PurestConsole2 we’re stepping just a bit away from that, to serve as a complement to Console7. PurestConsole2 does the same ‘filter the super-highs to prevent aliasing’ thing that Console7 does. BUT, not quite the same. It starts higher (run it at 44 or 48k and it won’t even attempt to filter) to extend to 30k before filtering. But then it filters SHARPER… to roll off quicker. And in doing that, it gives you a presence peak beyond hearing. Not a huge one, but it’s there: it’s also on the end of the system, not (like Console7) going into the system. So PurestConsole2 gives you a sprinkling of treble glitter even while it rolls off the aliasing-prone frequencies more effectively. The end result is the same kind of analog warmth… except it’s a cool, airy clarity that resolves absolutely EVERYTHING. If you’re shooting for super-clear this is the one you want. And since Console mixes can drive submixes which then use another Console system to sum the stems to the 2-buss… you can sneak it in on your harmony vocal beds, or orchestral stems, or you could use Console7 for everything and then sum only the stems to the 2-buss using PurestConsole2. Instead of mixing and matching within the summing busses, design your mix structure by figuring out where you want analog fatness and slam, and where you want clarity and resolution. The specific way I go about doing this grandiose thing has to do with filters, which is an odd thing to include in PurestConsole but it's got its place. This is one of those 'distributed filters' but it's only two stages of filtering: it's running a 0.618 resonance filter on the channels, and then a 1.618 resonance filter on the buss. These combine to become a flat filter… but the buss aspect is running inside Console, so the resonant peak it has is being applied to the summing before decoding. Hence, the 'airy clarity' stuff: we're literally applying expansion to just the highs of the track in summing without altering the tonal balance of the whole chain."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class PurestConsole2Channel final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'pcoe';
    double biquadA[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestConsole2Channel()
    {
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
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
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        biquadA[0] = 30000.0 / Effect<T>::getSampleRate();
        biquadA[1] = 0.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (biquadA[0] < 0.49999) {
                double tempSample = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
                biquadA[8] = biquadA[7];
                biquadA[7] = inputSampleL;
                inputSampleL = tempSample;
                biquadA[10] = biquadA[9];
                biquadA[9] = inputSampleL; // DF1 left
                tempSample = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
                biquadA[12] = biquadA[11];
                biquadA[11] = inputSampleR;
                inputSampleR = tempSample;
                biquadA[14] = biquadA[13];
                biquadA[13] = inputSampleR; // DF1 right
            }
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // amplitude aspect
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
} // namespace airwindohhs::purestconsole2channel
