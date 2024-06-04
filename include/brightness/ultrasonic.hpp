#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ultrasonic {

constexpr std::string_view k_name{ "Ultrasonic" };
constexpr std::string_view k_short_description{
    "UltrasonicMed is two stages of supersonic filter, for using inside digital mixes."
};
constexpr std::string_view k_long_description{
    "Now here’s a handy little utility plugin… sort of the ultimate Airwindows plugin, not only does it not have an interface, it doesn’t have a sound! :DHere’s why you should care… especially if you work at high sample rates.Aliasing in digital mixes is annoying. You typically don’t hear it directly, but it coarsens and flattens the mix, throwing all sorts of funny harmonics in there at random frequencies, like subtly ring modulating everything (the aliased harmonics go off on frequencies not related to the original notes, and clash). It’s often very subtle, but it turns up everywhere you have nonlinearities. If you distort stuff in digital mixes, you run straight into aliasing problems. Same if you compress, or do anything nonlinear… and even if you make a mix as pristine and minimal as possible, if you’re mixing in the Airwindows Console system that uses nonlinearities too.You can run at higher sample rates and that gives you more room, you can use soft saturations (like what’s in Console) and that makes the harmonics appear in order so only the highest frequencies will fold back and alias, but it’s in a computer: you’ll always run into the limits of juggling numbers and calling it music, and you’ll experience aliasing through nearly anything you do.But what if you just took those problem frequencies away?Ultrasonic is very simple. It has no controls. It’s a really steep lowpass filter at 20K (five poles, and it works out to 10th order Butterworth filtering). Unlike some ‘audible’ Airwindows filters that are supposed to sound interesting, Ultrasonic doesn’t use internal Console processing: that would be a nonlinearity, and defeat the purpose. Instead, it’s a super high resolution very boring and plain supersonic filter, calculating stuff at long double resolution, dithering its result to whatever floating point buss your DAW uses, and otherwise having no sound of its own.Drop it into your (preferably 96k or 192k) digital mix and it will clean up anything nonlinear that goes after it. This includes Console! Just because Console ‘decodes’ doesn’t mean it can’t be hurt by aliasing of its nonlinearities: it just distorts so gently that it’s not automatically worse, but any aliasing that turns up in ConsoleChannel doesn’t get taken away by ConsoleBuss. Digital only gets worse, not better, and the trick is to make it get worse as slowly as possible while you work with it.When you use Ultrasonic, for instance on every channel in a Console mix, you trade a degree of rawness and immediacy for an ease and smoothness that is immediately apparent if your stuff is running into nonlinearities anywhere. In many ways it makes the digital mix sound more analog. The tradeoff is, it’s still five poles of biquad filtering, and it will make stuff sound a bit slick, a bit more ‘processed’. You can kinda hear that you’re doing the extra processing, but the texture change is really appealing: stuff sits back (less super-treble will always sound more like the audio ‘sits back’ and is more polite) and bright shiny stuff sounds purer and sweeter. This is all the more true if you’re processing heavily.It’s very easy to use: just put the plugin before anything that might alias. By itself it should have no sound (though if you have true 96k or 192k audio, it of course is obliterating your real supersonic content). There are no controls and nothing to do, it isn’t itself nonlinear so it shouldn’t interact with anything, you don’t have to gain stage it or pay any attention to it at all. Very plain, simple, hopefully pretty low-CPU for all that it’s five poles of filtering at stinkin-high calculation accuracy.Put it on everything that you want to smooth out and un-digitalize. Sometimes there’s nothing quite like beating the problem into the ground with a sledgehammer. For frequencies over 20K and the aliasing that loves them, this is that sledgehammer. (It is also biquad filtering, so it runs with zero latency and you can track through it)Hi! This is just what it says on the tin. Airwindows Ultrasonic is the stacked-up, five-biquad filter that rolls off everything above 20k, so in theory it has no sound of its own. It’s there to work in high sample rate mixes, between plugins that have nonlinearities and don’t have their own filtering (some of mine do, like Console7) and it will clean up the top-end of a digital mix.But, the original Ultrasonic has SO many stages of filtering that it starts to become audible, softening the highs, and if you used lots of them you’d eat your CPU and would be over-processing.So, enter Ultrasonic Lite (and Ultrasonic Medium).These are the same sort of thing, except Ultrasonic Lite has only one stage of filtering, and Ultrasonic Medium has two. They also start a teeny bit higher, on the assumption that if you’re reaching for a Lite version of the filter, you’re looking to not hammer your highs too much. Ultrasonic Medium also subtly staggers the placement of its filters so it has a two-stage roll-off that is hopefully more natural sounding than just doubling up Ultrasonic Lite on its own.Use these just like you would use Ultrasonic, if there are places in your digital mix where you think you’d benefit from suppressing ultrasonic frequencies. These are not brickwalls: the idea here is that you can sprinkle these throughout your mix, anywhere you like, both before and after things that are nonlinear and distorty. For the strongest possible effect, use the original Ultrasonic… but in places where you don’t need that much help with the ultra-highs, try Medium or Lite and apply a cleaner, subtler filter that lets more of the air through.If you’ve got something that’s causing an aliasing that will give problems further down the mix chain, and you put Ultrasonic Lite in front of it and the aliasing that would’ve bounced back down to 40k is turned down before it even aliases, making that unwanted 40k quieter… and then there’s another Ultrasonic Lite afterwards and that directly turns down the unwanted, aliasing 40k… then you’ve got a gentle, distributed aliasing suppression across your whole mix, that will really control the tendency of aliasing to just build up and go critical on ya :)(followup: turns out using many instances of this WILL tend to roll off your highs, where using the full version won't. This is because the full version's doing more processing, but creating a steeper filter. This insight led to UltrasonX, and later versions of Console)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Ultrasonic final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double biquadD[15];
    double biquadE[15]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8-9-10 and 11-12-13-14, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Ultrasonic()
    {
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
            biquadE[x] = 0.0;
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

        biquadE[0] = biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = 20000.0 / getSampleRate();
        biquadA[1] = 0.50623256;
        biquadB[1] = 0.56116312;
        biquadC[1] = 0.70710678;
        biquadD[1] = 1.10134463;
        biquadE[1] = 3.19622661; // tenth order Butterworth out of five biquads
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        K = tan(M_PI * biquadD[0]);
        norm = 1.0 / (1.0 + K / biquadD[1] + K * K);
        biquadD[2] = K * K * norm;
        biquadD[3] = 2.0 * biquadD[2];
        biquadD[4] = biquadD[2];
        biquadD[5] = 2.0 * (K * K - 1.0) * norm;
        biquadD[6] = (1.0 - K / biquadD[1] + K * K) * norm;
        K = tan(M_PI * biquadE[0]);
        norm = 1.0 / (1.0 + K / biquadE[1] + K * K);
        biquadE[2] = K * K * norm;
        biquadE[3] = 2.0 * biquadE[2];
        biquadE[4] = biquadE[2];
        biquadE[5] = 2.0 * (K * K - 1.0) * norm;
        biquadE[6] = (1.0 - K / biquadE[1] + K * K) * norm;
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
            outSampleL = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
            biquadC[8] = biquadC[7];
            biquadC[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadC[10] = biquadC[9];
            biquadC[9] = inputSampleL; // DF1 left
            outSampleL = biquadD[2] * inputSampleL + biquadD[3] * biquadD[7] + biquadD[4] * biquadD[8] - biquadD[5] * biquadD[9] - biquadD[6] * biquadD[10];
            biquadD[8] = biquadD[7];
            biquadD[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadD[10] = biquadD[9];
            biquadD[9] = inputSampleL; // DF1 left
            outSampleL = biquadE[2] * inputSampleL + biquadE[3] * biquadE[7] + biquadE[4] * biquadE[8] - biquadE[5] * biquadE[9] - biquadE[6] * biquadE[10];
            biquadE[8] = biquadE[7];
            biquadE[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadE[10] = biquadE[9];
            biquadE[9] = inputSampleL; // DF1 left
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
            outSampleR = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
            biquadC[12] = biquadC[11];
            biquadC[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadC[14] = biquadC[13];
            biquadC[13] = inputSampleR; // DF1 right
            outSampleR = biquadD[2] * inputSampleR + biquadD[3] * biquadD[11] + biquadD[4] * biquadD[12] - biquadD[5] * biquadD[13] - biquadD[6] * biquadD[14];
            biquadD[12] = biquadD[11];
            biquadD[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadD[14] = biquadD[13];
            biquadD[13] = inputSampleR; // DF1 right
            outSampleR = biquadE[2] * inputSampleR + biquadE[3] * biquadE[11] + biquadE[4] * biquadE[12] - biquadE[5] * biquadE[13] - biquadE[6] * biquadE[14];
            biquadE[12] = biquadE[11];
            biquadE[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadE[14] = biquadE[13];
            biquadE[13] = inputSampleR; // DF1 right
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
} // namespace airwindohhs::ultrasonic
