#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ultrasonicmed {

constexpr std::string_view k_name{ "UltrasonicMed" };
constexpr std::string_view k_short_description{
    "UltrasonicMed is two stages of supersonic filter, for using inside digital mixes."
};
constexpr std::string_view k_long_description{
    "Hi! This is just what it says on the tin. Airwindows Ultrasonic is the stacked-up, five-biquad filter that rolls off everything above 20k, so in theory it has no sound of its own. It’s there to work in high sample rate mixes, between plugins that have nonlinearities and don’t have their own filtering (some of mine do, like Console7) and it will clean up the top-end of a digital mix.But, the original Ultrasonic has SO many stages of filtering that it starts to become audible, softening the highs, and if you used lots of them you’d eat your CPU and would be over-processing.So, enter Ultrasonic Lite (and Ultrasonic Medium).These are the same sort of thing, except Ultrasonic Lite has only one stage of filtering, and Ultrasonic Medium has two. They also start a teeny bit higher, on the assumption that if you’re reaching for a Lite version of the filter, you’re looking to not hammer your highs too much. Ultrasonic Medium also subtly staggers the placement of its filters so it has a two-stage roll-off that is hopefully more natural sounding than just doubling up Ultrasonic Lite on its own.Use these just like you would use Ultrasonic, if there are places in your digital mix where you think you’d benefit from suppressing ultrasonic frequencies. These are not brickwalls: the idea here is that you can sprinkle these throughout your mix, anywhere you like, both before and after things that are nonlinear and distorty. For the strongest possible effect, use the original Ultrasonic… but in places where you don’t need that much help with the ultra-highs, try Medium or Lite and apply a cleaner, subtler filter that lets more of the air through.If you’ve got something that’s causing an aliasing that will give problems further down the mix chain, and you put Ultrasonic Lite in front of it and the aliasing that would’ve bounced back down to 40k is turned down before it even aliases, making that unwanted 40k quieter… and then there’s another Ultrasonic Lite afterwards and that directly turns down the unwanted, aliasing 40k… then you’ve got a gentle, distributed aliasing suppression across your whole mix, that will really control the tendency of aliasing to just build up and go critical on ya :)(followup: turns out using many instances of this WILL tend to roll off your highs, where using the full version won't. This is because the full version's doing more processing, but creating a steeper filter. This insight led to UltrasonX, and later versions of Console)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class UltrasonicMed final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    UltrasonicMed()
    {
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        biquadA[0] = 24000.0 / getSampleRate();
        if (getSampleRate() < 88000.0) {
            biquadA[0] = 21000.0 / getSampleRate();
        }
        biquadB[0] = 22000.0 / getSampleRate();
        if (getSampleRate() < 88000.0) {
            biquadB[0] = 20000.0 / getSampleRate();
        }
        biquadA[1] = 0.70710678;
        biquadB[1] = 0.70710678;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
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
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            outSampleL = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
            biquadB[8] = biquadB[7];
            biquadB[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadB[10] = biquadB[9];
            biquadB[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            outSampleR = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
            biquadB[12] = biquadB[11];
            biquadB[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadB[14] = biquadB[13];
            biquadB[13] = inputSampleR; // DF1 right
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
} // namespace airwindohhs::ultrasonicmed
