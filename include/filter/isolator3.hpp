#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::isolator3 {

constexpr std::string_view k_name{ "Isolator3" };
constexpr std::string_view k_short_description{
    "Isolator3 is Isolator2, but on one slider, with a band-narrower control."
};
constexpr std::string_view k_long_description{
    "Isolator3 is basically a request. I got asked for an isolator filter 'on one knob', which I've seen around but I've not used myself. Basically it means middle is full-range, to the left you're lowpassing and to the right you're highpassing.Turns out you need to run it as two filters because it's impossible to 'switch' the biquads inside it, from one role to the other. So you constantly run two filters one of which you can't hear the effect of.Except then there's the Q control, which does not actually control the Q of the filters. They're still steep, stacked-up filters without special resonances of their own. But when you turn up the Q slider, the cutoffs approach each other, turn into something like a bandpass on the fly. Full up, it should act a little like a resonance. To go back to full range, turn Q back down to zero and Iso to 0.5 (the middle).Lastly, this is lots and lots of filter stages all of which are smoothed for better modulation… but it's made of biquad filters, which don't like being modulated, and then it's two types of filter run into each other. I've put in safety clipping because it turns out that if you yank the control around real crazy, it's easy to get Isolator to glitch out. The glitches aren't always useful sounding but now and then it emits really weird synthetic burps and frills and so I've just made sure it can't blow up the output too bad, and then left it to its own devices. So, use Isolator3 with smooth intentional motions for traditional effects, yank it around madly if you want it to spit digits and sparks at you, have fun is the most important instruction.Hope it works as I got COVID returning from my vacation and it is heavily still with me so I've lost a bunch of steps. Not ideas, it's just that this is a good time for me to work through the backlog a bit. May or may not livestream in the upcoming week and if I do I won't be talking for eight hours at a stretch: probably won't need to go to the hospital but will check with a doctor. I'm in my space, Chris's 'hunting for ideas mountaintop' and it's no biggie for me to hole up for a few weeks. See ya again next week with something else from the backlog! :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Isolator3 final : public Effect<T>
{
    float A;
    float B;
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_aA0,
        biq_aA1,
        biq_aA2,
        biq_bA1,
        biq_bA2,
        biq_aB0,
        biq_aB1,
        biq_aB2,
        biq_bB1,
        biq_bB2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    }; // coefficient interpolating biquad filter, stereo
    double biquadA[biq_total];
    double biquadB[biq_total];
    double biquadC[biq_total];
    double biquadD[biq_total];
    double biquadE[biq_total];
    double biquadF[biq_total];
    double biquadG[biq_total];
    double hiquadA[biq_total];
    double hiquadB[biq_total];
    double hiquadC[biq_total];
    double hiquadD[biq_total];
    double hiquadE[biq_total];
    double hiquadF[biq_total];
    double hiquadG[biq_total];

    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR;

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Isolator3()
    {
        A = 0.5;
        B = 0.0;
        for (int x = 0; x < biq_total; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
            biquadE[x] = 0.0;
            biquadF[x] = 0.0;
            biquadG[x] = 0.0;
            hiquadA[x] = 0.0;
            hiquadB[x] = 0.0;
            hiquadC[x] = 0.0;
            hiquadD[x] = 0.0;
            hiquadE[x] = 0.0;
            hiquadF[x] = 0.0;
            hiquadG[x] = 0.0;
        }
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
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
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "iso"; break;
            case kParamB: return "q"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Iso"; break;
            case kParamB: return "Q"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double f = A;
        double q = B;
        double isoFreq = ((f * 2.0) * (1.0 - q)) + (f * q);
        double hisoFreq = (((f * 2.0) - 1.0) * (1.0 - q)) + (f * q);
        if (isoFreq > 1.0) {
            isoFreq = 1.0;
        }
        if (hisoFreq < 0.0) {
            hisoFreq = 0.0;
        }
        biquadA[biq_freq] = pow(isoFreq, (2.0 * sqrt(overallscale))) * 0.4999;
        if (biquadA[biq_freq] < 0.00025) {
            biquadA[biq_freq] = 0.00025;
        }
        biquadG[biq_freq] = biquadF[biq_freq] = biquadE[biq_freq] = biquadD[biq_freq] = biquadC[biq_freq] = biquadB[biq_freq] = biquadA[biq_freq];
        hiquadA[biq_freq] = pow(hisoFreq, (2.0 * sqrt(overallscale))) * 0.4999;
        if (hiquadA[biq_freq] < 0.00025) {
            hiquadA[biq_freq] = 0.00025;
        }
        hiquadG[biq_freq] = hiquadF[biq_freq] = hiquadE[biq_freq] = hiquadD[biq_freq] = hiquadC[biq_freq] = hiquadB[biq_freq] = hiquadA[biq_freq];
        hiquadA[biq_reso] = biquadA[biq_reso] = 4.46570214;
        hiquadB[biq_reso] = biquadB[biq_reso] = 1.51387132;
        hiquadC[biq_reso] = biquadC[biq_reso] = 0.93979296;
        hiquadD[biq_reso] = biquadD[biq_reso] = 0.70710678;
        hiquadE[biq_reso] = biquadE[biq_reso] = 0.59051105;
        hiquadF[biq_reso] = biquadF[biq_reso] = 0.52972649;
        hiquadG[biq_reso] = biquadG[biq_reso] = 0.50316379;
        biquadA[biq_aA0] = biquadA[biq_aB0];
        biquadA[biq_aA1] = biquadA[biq_aB1];
        biquadA[biq_aA2] = biquadA[biq_aB2];
        biquadA[biq_bA1] = biquadA[biq_bB1];
        biquadA[biq_bA2] = biquadA[biq_bB2];
        biquadB[biq_aA0] = biquadB[biq_aB0];
        biquadB[biq_aA1] = biquadB[biq_aB1];
        biquadB[biq_aA2] = biquadB[biq_aB2];
        biquadB[biq_bA1] = biquadB[biq_bB1];
        biquadB[biq_bA2] = biquadB[biq_bB2];
        biquadC[biq_aA0] = biquadC[biq_aB0];
        biquadC[biq_aA1] = biquadC[biq_aB1];
        biquadC[biq_aA2] = biquadC[biq_aB2];
        biquadC[biq_bA1] = biquadC[biq_bB1];
        biquadC[biq_bA2] = biquadC[biq_bB2];
        biquadD[biq_aA0] = biquadD[biq_aB0];
        biquadD[biq_aA1] = biquadD[biq_aB1];
        biquadD[biq_aA2] = biquadD[biq_aB2];
        biquadD[biq_bA1] = biquadD[biq_bB1];
        biquadD[biq_bA2] = biquadD[biq_bB2];
        biquadE[biq_aA0] = biquadE[biq_aB0];
        biquadE[biq_aA1] = biquadE[biq_aB1];
        biquadE[biq_aA2] = biquadE[biq_aB2];
        biquadE[biq_bA1] = biquadE[biq_bB1];
        biquadE[biq_bA2] = biquadE[biq_bB2];
        biquadF[biq_aA0] = biquadF[biq_aB0];
        biquadF[biq_aA1] = biquadF[biq_aB1];
        biquadF[biq_aA2] = biquadF[biq_aB2];
        biquadF[biq_bA1] = biquadF[biq_bB1];
        biquadF[biq_bA2] = biquadF[biq_bB2];
        biquadG[biq_aA0] = biquadG[biq_aB0];
        biquadG[biq_aA1] = biquadG[biq_aB1];
        biquadG[biq_aA2] = biquadG[biq_aB2];
        biquadG[biq_bA1] = biquadG[biq_bB1];
        biquadG[biq_bA2] = biquadG[biq_bB2];
        hiquadA[biq_aA0] = hiquadA[biq_aB0];
        hiquadA[biq_aA1] = hiquadA[biq_aB1];
        hiquadA[biq_aA2] = hiquadA[biq_aB2];
        hiquadA[biq_bA1] = hiquadA[biq_bB1];
        hiquadA[biq_bA2] = hiquadA[biq_bB2];
        hiquadB[biq_aA0] = hiquadB[biq_aB0];
        hiquadB[biq_aA1] = hiquadB[biq_aB1];
        hiquadB[biq_aA2] = hiquadB[biq_aB2];
        hiquadB[biq_bA1] = hiquadB[biq_bB1];
        hiquadB[biq_bA2] = hiquadB[biq_bB2];
        hiquadC[biq_aA0] = hiquadC[biq_aB0];
        hiquadC[biq_aA1] = hiquadC[biq_aB1];
        hiquadC[biq_aA2] = hiquadC[biq_aB2];
        hiquadC[biq_bA1] = hiquadC[biq_bB1];
        hiquadC[biq_bA2] = hiquadC[biq_bB2];
        hiquadD[biq_aA0] = hiquadD[biq_aB0];
        hiquadD[biq_aA1] = hiquadD[biq_aB1];
        hiquadD[biq_aA2] = hiquadD[biq_aB2];
        hiquadD[biq_bA1] = hiquadD[biq_bB1];
        hiquadD[biq_bA2] = hiquadD[biq_bB2];
        hiquadE[biq_aA0] = hiquadE[biq_aB0];
        hiquadE[biq_aA1] = hiquadE[biq_aB1];
        hiquadE[biq_aA2] = hiquadE[biq_aB2];
        hiquadE[biq_bA1] = hiquadE[biq_bB1];
        hiquadE[biq_bA2] = hiquadE[biq_bB2];
        hiquadF[biq_aA0] = hiquadF[biq_aB0];
        hiquadF[biq_aA1] = hiquadF[biq_aB1];
        hiquadF[biq_aA2] = hiquadF[biq_aB2];
        hiquadF[biq_bA1] = hiquadF[biq_bB1];
        hiquadF[biq_bA2] = hiquadF[biq_bB2];
        hiquadG[biq_aA0] = hiquadG[biq_aB0];
        hiquadG[biq_aA1] = hiquadG[biq_aB1];
        hiquadG[biq_aA2] = hiquadG[biq_aB2];
        hiquadG[biq_bA1] = hiquadG[biq_bB1];
        hiquadG[biq_bA2] = hiquadG[biq_bB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        double K = tan(M_PI * biquadA[biq_freq]);
        double norm = 1.0 / (1.0 + K / biquadA[biq_reso] + K * K);
        biquadA[biq_aB0] = K * K * norm;
        biquadA[biq_aB1] = 2.0 * biquadA[biq_aB0];
        biquadA[biq_aB2] = biquadA[biq_aB0];
        biquadA[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadA[biq_bB2] = (1.0 - K / biquadA[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadB[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadB[biq_reso] + K * K);
        biquadB[biq_aB0] = K * K * norm;
        biquadB[biq_aB1] = 2.0 * biquadB[biq_aB0];
        biquadB[biq_aB2] = biquadB[biq_aB0];
        biquadB[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadB[biq_bB2] = (1.0 - K / biquadB[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadC[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadC[biq_reso] + K * K);
        biquadC[biq_aB0] = K * K * norm;
        biquadC[biq_aB1] = 2.0 * biquadC[biq_aB0];
        biquadC[biq_aB2] = biquadC[biq_aB0];
        biquadC[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadC[biq_bB2] = (1.0 - K / biquadC[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadD[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadD[biq_reso] + K * K);
        biquadD[biq_aB0] = K * K * norm;
        biquadD[biq_aB1] = 2.0 * biquadD[biq_aB0];
        biquadD[biq_aB2] = biquadD[biq_aB0];
        biquadD[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadD[biq_bB2] = (1.0 - K / biquadD[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadE[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadE[biq_reso] + K * K);
        biquadE[biq_aB0] = K * K * norm;
        biquadE[biq_aB1] = 2.0 * biquadE[biq_aB0];
        biquadE[biq_aB2] = biquadE[biq_aB0];
        biquadE[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadE[biq_bB2] = (1.0 - K / biquadE[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadF[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadF[biq_reso] + K * K);
        biquadF[biq_aB0] = K * K * norm;
        biquadF[biq_aB1] = 2.0 * biquadF[biq_aB0];
        biquadF[biq_aB2] = biquadF[biq_aB0];
        biquadF[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadF[biq_bB2] = (1.0 - K / biquadF[biq_reso] + K * K) * norm;
        K = tan(M_PI * biquadG[biq_freq]);
        norm = 1.0 / (1.0 + K / biquadG[biq_reso] + K * K);
        biquadG[biq_aB0] = K * K * norm;
        biquadG[biq_aB1] = 2.0 * biquadG[biq_aB0];
        biquadG[biq_aB2] = biquadG[biq_aB0];
        biquadG[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadG[biq_bB2] = (1.0 - K / biquadG[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadA[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadA[biq_reso] + K * K);
        hiquadA[biq_aB0] = K * K * norm;
        hiquadA[biq_aB1] = 2.0 * hiquadA[biq_aB0];
        hiquadA[biq_aB2] = hiquadA[biq_aB0];
        hiquadA[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadA[biq_bB2] = (1.0 - K / hiquadA[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadB[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadB[biq_reso] + K * K);
        hiquadB[biq_aB0] = K * K * norm;
        hiquadB[biq_aB1] = 2.0 * hiquadB[biq_aB0];
        hiquadB[biq_aB2] = hiquadB[biq_aB0];
        hiquadB[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadB[biq_bB2] = (1.0 - K / hiquadB[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadC[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadC[biq_reso] + K * K);
        hiquadC[biq_aB0] = K * K * norm;
        hiquadC[biq_aB1] = 2.0 * hiquadC[biq_aB0];
        hiquadC[biq_aB2] = hiquadC[biq_aB0];
        hiquadC[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadC[biq_bB2] = (1.0 - K / hiquadC[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadD[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadD[biq_reso] + K * K);
        hiquadD[biq_aB0] = K * K * norm;
        hiquadD[biq_aB1] = 2.0 * hiquadD[biq_aB0];
        hiquadD[biq_aB2] = hiquadD[biq_aB0];
        hiquadD[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadD[biq_bB2] = (1.0 - K / hiquadD[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadE[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadE[biq_reso] + K * K);
        hiquadE[biq_aB0] = K * K * norm;
        hiquadE[biq_aB1] = 2.0 * hiquadE[biq_aB0];
        hiquadE[biq_aB2] = hiquadE[biq_aB0];
        hiquadE[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadE[biq_bB2] = (1.0 - K / hiquadE[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadF[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadF[biq_reso] + K * K);
        hiquadF[biq_aB0] = K * K * norm;
        hiquadF[biq_aB1] = 2.0 * hiquadF[biq_aB0];
        hiquadF[biq_aB2] = hiquadF[biq_aB0];
        hiquadF[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadF[biq_bB2] = (1.0 - K / hiquadF[biq_reso] + K * K) * norm;
        K = tan(M_PI * hiquadG[biq_freq]);
        norm = 1.0 / (1.0 + K / hiquadG[biq_reso] + K * K);
        hiquadG[biq_aB0] = K * K * norm;
        hiquadG[biq_aB1] = 2.0 * hiquadG[biq_aB0];
        hiquadG[biq_aB2] = hiquadG[biq_aB0];
        hiquadG[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        hiquadG[biq_bB2] = (1.0 - K / hiquadG[biq_reso] + K * K) * norm;
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
            double temp = (double)sampleFrames / inFramesToProcess;
            hiquadA[biq_a0] = (hiquadA[biq_aA0] * temp) + (hiquadA[biq_aB0] * (1.0 - temp));
            hiquadA[biq_a1] = (hiquadA[biq_aA1] * temp) + (hiquadA[biq_aB1] * (1.0 - temp));
            hiquadA[biq_a2] = (hiquadA[biq_aA2] * temp) + (hiquadA[biq_aB2] * (1.0 - temp));
            hiquadA[biq_b1] = (hiquadA[biq_bA1] * temp) + (hiquadA[biq_bB1] * (1.0 - temp));
            hiquadA[biq_b2] = (hiquadA[biq_bA2] * temp) + (hiquadA[biq_bB2] * (1.0 - temp));
            hiquadB[biq_a0] = (hiquadB[biq_aA0] * temp) + (hiquadB[biq_aB0] * (1.0 - temp));
            hiquadB[biq_a1] = (hiquadB[biq_aA1] * temp) + (hiquadB[biq_aB1] * (1.0 - temp));
            hiquadB[biq_a2] = (hiquadB[biq_aA2] * temp) + (hiquadB[biq_aB2] * (1.0 - temp));
            hiquadB[biq_b1] = (hiquadB[biq_bA1] * temp) + (hiquadB[biq_bB1] * (1.0 - temp));
            hiquadB[biq_b2] = (hiquadB[biq_bA2] * temp) + (hiquadB[biq_bB2] * (1.0 - temp));
            hiquadC[biq_a0] = (hiquadC[biq_aA0] * temp) + (hiquadC[biq_aB0] * (1.0 - temp));
            hiquadC[biq_a1] = (hiquadC[biq_aA1] * temp) + (hiquadC[biq_aB1] * (1.0 - temp));
            hiquadC[biq_a2] = (hiquadC[biq_aA2] * temp) + (hiquadC[biq_aB2] * (1.0 - temp));
            hiquadC[biq_b1] = (hiquadC[biq_bA1] * temp) + (hiquadC[biq_bB1] * (1.0 - temp));
            hiquadC[biq_b2] = (hiquadC[biq_bA2] * temp) + (hiquadC[biq_bB2] * (1.0 - temp));
            hiquadD[biq_a0] = (hiquadD[biq_aA0] * temp) + (hiquadD[biq_aB0] * (1.0 - temp));
            hiquadD[biq_a1] = (hiquadD[biq_aA1] * temp) + (hiquadD[biq_aB1] * (1.0 - temp));
            hiquadD[biq_a2] = (hiquadD[biq_aA2] * temp) + (hiquadD[biq_aB2] * (1.0 - temp));
            hiquadD[biq_b1] = (hiquadD[biq_bA1] * temp) + (hiquadD[biq_bB1] * (1.0 - temp));
            hiquadD[biq_b2] = (hiquadD[biq_bA2] * temp) + (hiquadD[biq_bB2] * (1.0 - temp));
            hiquadE[biq_a0] = (hiquadE[biq_aA0] * temp) + (hiquadE[biq_aB0] * (1.0 - temp));
            hiquadE[biq_a1] = (hiquadE[biq_aA1] * temp) + (hiquadE[biq_aB1] * (1.0 - temp));
            hiquadE[biq_a2] = (hiquadE[biq_aA2] * temp) + (hiquadE[biq_aB2] * (1.0 - temp));
            hiquadE[biq_b1] = (hiquadE[biq_bA1] * temp) + (hiquadE[biq_bB1] * (1.0 - temp));
            hiquadE[biq_b2] = (hiquadE[biq_bA2] * temp) + (hiquadE[biq_bB2] * (1.0 - temp));
            hiquadF[biq_a0] = (hiquadF[biq_aA0] * temp) + (hiquadF[biq_aB0] * (1.0 - temp));
            hiquadF[biq_a1] = (hiquadF[biq_aA1] * temp) + (hiquadF[biq_aB1] * (1.0 - temp));
            hiquadF[biq_a2] = (hiquadF[biq_aA2] * temp) + (hiquadF[biq_aB2] * (1.0 - temp));
            hiquadF[biq_b1] = (hiquadF[biq_bA1] * temp) + (hiquadF[biq_bB1] * (1.0 - temp));
            hiquadF[biq_b2] = (hiquadF[biq_bA2] * temp) + (hiquadF[biq_bB2] * (1.0 - temp));
            hiquadG[biq_a0] = (hiquadG[biq_aA0] * temp) + (hiquadG[biq_aB0] * (1.0 - temp));
            hiquadG[biq_a1] = (hiquadG[biq_aA1] * temp) + (hiquadG[biq_aB1] * (1.0 - temp));
            hiquadG[biq_a2] = (hiquadG[biq_aA2] * temp) + (hiquadG[biq_aB2] * (1.0 - temp));
            hiquadG[biq_b1] = (hiquadG[biq_bA1] * temp) + (hiquadG[biq_bB1] * (1.0 - temp));
            hiquadG[biq_b2] = (hiquadG[biq_bA2] * temp) + (hiquadG[biq_bB2] * (1.0 - temp));
            // this is the interpolation code for the hiquad
            double outSample = (inputSampleL * hiquadA[biq_a0]) + hiquadA[biq_sL1];
            hiquadA[biq_sL1] = (inputSampleL * hiquadA[biq_a1]) - (outSample * hiquadA[biq_b1]) + hiquadA[biq_sL2];
            hiquadA[biq_sL2] = (inputSampleL * hiquadA[biq_a2]) - (outSample * hiquadA[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadA[biq_a0]) + hiquadA[biq_sR1];
            hiquadA[biq_sR1] = (inputSampleR * hiquadA[biq_a1]) - (outSample * hiquadA[biq_b1]) + hiquadA[biq_sR2];
            hiquadA[biq_sR2] = (inputSampleR * hiquadA[biq_a2]) - (outSample * hiquadA[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadB[biq_a0]) + hiquadB[biq_sL1];
            hiquadB[biq_sL1] = (inputSampleL * hiquadB[biq_a1]) - (outSample * hiquadB[biq_b1]) + hiquadB[biq_sL2];
            hiquadB[biq_sL2] = (inputSampleL * hiquadB[biq_a2]) - (outSample * hiquadB[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadB[biq_a0]) + hiquadB[biq_sR1];
            hiquadB[biq_sR1] = (inputSampleR * hiquadB[biq_a1]) - (outSample * hiquadB[biq_b1]) + hiquadB[biq_sR2];
            hiquadB[biq_sR2] = (inputSampleR * hiquadB[biq_a2]) - (outSample * hiquadB[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadC[biq_a0]) + hiquadC[biq_sL1];
            hiquadC[biq_sL1] = (inputSampleL * hiquadC[biq_a1]) - (outSample * hiquadC[biq_b1]) + hiquadC[biq_sL2];
            hiquadC[biq_sL2] = (inputSampleL * hiquadC[biq_a2]) - (outSample * hiquadC[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadC[biq_a0]) + hiquadC[biq_sR1];
            hiquadC[biq_sR1] = (inputSampleR * hiquadC[biq_a1]) - (outSample * hiquadC[biq_b1]) + hiquadC[biq_sR2];
            hiquadC[biq_sR2] = (inputSampleR * hiquadC[biq_a2]) - (outSample * hiquadC[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadD[biq_a0]) + hiquadD[biq_sL1];
            hiquadD[biq_sL1] = (inputSampleL * hiquadD[biq_a1]) - (outSample * hiquadD[biq_b1]) + hiquadD[biq_sL2];
            hiquadD[biq_sL2] = (inputSampleL * hiquadD[biq_a2]) - (outSample * hiquadD[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadD[biq_a0]) + hiquadD[biq_sR1];
            hiquadD[biq_sR1] = (inputSampleR * hiquadD[biq_a1]) - (outSample * hiquadD[biq_b1]) + hiquadD[biq_sR2];
            hiquadD[biq_sR2] = (inputSampleR * hiquadD[biq_a2]) - (outSample * hiquadD[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadE[biq_a0]) + hiquadE[biq_sL1];
            hiquadE[biq_sL1] = (inputSampleL * hiquadE[biq_a1]) - (outSample * hiquadE[biq_b1]) + hiquadE[biq_sL2];
            hiquadE[biq_sL2] = (inputSampleL * hiquadE[biq_a2]) - (outSample * hiquadE[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadE[biq_a0]) + hiquadE[biq_sR1];
            hiquadE[biq_sR1] = (inputSampleR * hiquadE[biq_a1]) - (outSample * hiquadE[biq_b1]) + hiquadE[biq_sR2];
            hiquadE[biq_sR2] = (inputSampleR * hiquadE[biq_a2]) - (outSample * hiquadE[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadF[biq_a0]) + hiquadF[biq_sL1];
            hiquadF[biq_sL1] = (inputSampleL * hiquadF[biq_a1]) - (outSample * hiquadF[biq_b1]) + hiquadF[biq_sL2];
            hiquadF[biq_sL2] = (inputSampleL * hiquadF[biq_a2]) - (outSample * hiquadF[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadF[biq_a0]) + hiquadF[biq_sR1];
            hiquadF[biq_sR1] = (inputSampleR * hiquadF[biq_a1]) - (outSample * hiquadF[biq_b1]) + hiquadF[biq_sR2];
            hiquadF[biq_sR2] = (inputSampleR * hiquadF[biq_a2]) - (outSample * hiquadF[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * hiquadG[biq_a0]) + hiquadG[biq_sL1];
            hiquadG[biq_sL1] = (inputSampleL * hiquadG[biq_a1]) - (outSample * hiquadG[biq_b1]) + hiquadG[biq_sL2];
            hiquadG[biq_sL2] = (inputSampleL * hiquadG[biq_a2]) - (outSample * hiquadG[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * hiquadG[biq_a0]) + hiquadG[biq_sR1];
            hiquadG[biq_sR1] = (inputSampleR * hiquadG[biq_a1]) - (outSample * hiquadG[biq_b1]) + hiquadG[biq_sR2];
            hiquadG[biq_sR2] = (inputSampleR * hiquadG[biq_a2]) - (outSample * hiquadG[biq_b2]);
            inputSampleR = outSample;
            inputSampleL = (drySampleL - inputSampleL);
            inputSampleR = (drySampleR - inputSampleR);
            biquadA[biq_a0] = (biquadA[biq_aA0] * temp) + (biquadA[biq_aB0] * (1.0 - temp));
            biquadA[biq_a1] = (biquadA[biq_aA1] * temp) + (biquadA[biq_aB1] * (1.0 - temp));
            biquadA[biq_a2] = (biquadA[biq_aA2] * temp) + (biquadA[biq_aB2] * (1.0 - temp));
            biquadA[biq_b1] = (biquadA[biq_bA1] * temp) + (biquadA[biq_bB1] * (1.0 - temp));
            biquadA[biq_b2] = (biquadA[biq_bA2] * temp) + (biquadA[biq_bB2] * (1.0 - temp));
            biquadB[biq_a0] = (biquadB[biq_aA0] * temp) + (biquadB[biq_aB0] * (1.0 - temp));
            biquadB[biq_a1] = (biquadB[biq_aA1] * temp) + (biquadB[biq_aB1] * (1.0 - temp));
            biquadB[biq_a2] = (biquadB[biq_aA2] * temp) + (biquadB[biq_aB2] * (1.0 - temp));
            biquadB[biq_b1] = (biquadB[biq_bA1] * temp) + (biquadB[biq_bB1] * (1.0 - temp));
            biquadB[biq_b2] = (biquadB[biq_bA2] * temp) + (biquadB[biq_bB2] * (1.0 - temp));
            biquadC[biq_a0] = (biquadC[biq_aA0] * temp) + (biquadC[biq_aB0] * (1.0 - temp));
            biquadC[biq_a1] = (biquadC[biq_aA1] * temp) + (biquadC[biq_aB1] * (1.0 - temp));
            biquadC[biq_a2] = (biquadC[biq_aA2] * temp) + (biquadC[biq_aB2] * (1.0 - temp));
            biquadC[biq_b1] = (biquadC[biq_bA1] * temp) + (biquadC[biq_bB1] * (1.0 - temp));
            biquadC[biq_b2] = (biquadC[biq_bA2] * temp) + (biquadC[biq_bB2] * (1.0 - temp));
            biquadD[biq_a0] = (biquadD[biq_aA0] * temp) + (biquadD[biq_aB0] * (1.0 - temp));
            biquadD[biq_a1] = (biquadD[biq_aA1] * temp) + (biquadD[biq_aB1] * (1.0 - temp));
            biquadD[biq_a2] = (biquadD[biq_aA2] * temp) + (biquadD[biq_aB2] * (1.0 - temp));
            biquadD[biq_b1] = (biquadD[biq_bA1] * temp) + (biquadD[biq_bB1] * (1.0 - temp));
            biquadD[biq_b2] = (biquadD[biq_bA2] * temp) + (biquadD[biq_bB2] * (1.0 - temp));
            biquadE[biq_a0] = (biquadE[biq_aA0] * temp) + (biquadE[biq_aB0] * (1.0 - temp));
            biquadE[biq_a1] = (biquadE[biq_aA1] * temp) + (biquadE[biq_aB1] * (1.0 - temp));
            biquadE[biq_a2] = (biquadE[biq_aA2] * temp) + (biquadE[biq_aB2] * (1.0 - temp));
            biquadE[biq_b1] = (biquadE[biq_bA1] * temp) + (biquadE[biq_bB1] * (1.0 - temp));
            biquadE[biq_b2] = (biquadE[biq_bA2] * temp) + (biquadE[biq_bB2] * (1.0 - temp));
            biquadF[biq_a0] = (biquadF[biq_aA0] * temp) + (biquadF[biq_aB0] * (1.0 - temp));
            biquadF[biq_a1] = (biquadF[biq_aA1] * temp) + (biquadF[biq_aB1] * (1.0 - temp));
            biquadF[biq_a2] = (biquadF[biq_aA2] * temp) + (biquadF[biq_aB2] * (1.0 - temp));
            biquadF[biq_b1] = (biquadF[biq_bA1] * temp) + (biquadF[biq_bB1] * (1.0 - temp));
            biquadF[biq_b2] = (biquadF[biq_bA2] * temp) + (biquadF[biq_bB2] * (1.0 - temp));
            biquadG[biq_a0] = (biquadG[biq_aA0] * temp) + (biquadG[biq_aB0] * (1.0 - temp));
            biquadG[biq_a1] = (biquadG[biq_aA1] * temp) + (biquadG[biq_aB1] * (1.0 - temp));
            biquadG[biq_a2] = (biquadG[biq_aA2] * temp) + (biquadG[biq_aB2] * (1.0 - temp));
            biquadG[biq_b1] = (biquadG[biq_bA1] * temp) + (biquadG[biq_bB1] * (1.0 - temp));
            biquadG[biq_b2] = (biquadG[biq_bA2] * temp) + (biquadG[biq_bB2] * (1.0 - temp));
            // this is the interpolation code for the biquad
            outSample = (inputSampleL * biquadA[biq_a0]) + biquadA[biq_sL1];
            biquadA[biq_sL1] = (inputSampleL * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sL2];
            biquadA[biq_sL2] = (inputSampleL * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadA[biq_a0]) + biquadA[biq_sR1];
            biquadA[biq_sR1] = (inputSampleR * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sR2];
            biquadA[biq_sR2] = (inputSampleR * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadB[biq_a0]) + biquadB[biq_sL1];
            biquadB[biq_sL1] = (inputSampleL * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sL2];
            biquadB[biq_sL2] = (inputSampleL * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadB[biq_a0]) + biquadB[biq_sR1];
            biquadB[biq_sR1] = (inputSampleR * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sR2];
            biquadB[biq_sR2] = (inputSampleR * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadC[biq_a0]) + biquadC[biq_sL1];
            biquadC[biq_sL1] = (inputSampleL * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sL2];
            biquadC[biq_sL2] = (inputSampleL * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadC[biq_a0]) + biquadC[biq_sR1];
            biquadC[biq_sR1] = (inputSampleR * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sR2];
            biquadC[biq_sR2] = (inputSampleR * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadD[biq_a0]) + biquadD[biq_sL1];
            biquadD[biq_sL1] = (inputSampleL * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sL2];
            biquadD[biq_sL2] = (inputSampleL * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadD[biq_a0]) + biquadD[biq_sR1];
            biquadD[biq_sR1] = (inputSampleR * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sR2];
            biquadD[biq_sR2] = (inputSampleR * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadE[biq_a0]) + biquadE[biq_sL1];
            biquadE[biq_sL1] = (inputSampleL * biquadE[biq_a1]) - (outSample * biquadE[biq_b1]) + biquadE[biq_sL2];
            biquadE[biq_sL2] = (inputSampleL * biquadE[biq_a2]) - (outSample * biquadE[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadE[biq_a0]) + biquadE[biq_sR1];
            biquadE[biq_sR1] = (inputSampleR * biquadE[biq_a1]) - (outSample * biquadE[biq_b1]) + biquadE[biq_sR2];
            biquadE[biq_sR2] = (inputSampleR * biquadE[biq_a2]) - (outSample * biquadE[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadF[biq_a0]) + biquadF[biq_sL1];
            biquadF[biq_sL1] = (inputSampleL * biquadF[biq_a1]) - (outSample * biquadF[biq_b1]) + biquadF[biq_sL2];
            biquadF[biq_sL2] = (inputSampleL * biquadF[biq_a2]) - (outSample * biquadF[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadF[biq_a0]) + biquadF[biq_sR1];
            biquadF[biq_sR1] = (inputSampleR * biquadF[biq_a1]) - (outSample * biquadF[biq_b1]) + biquadF[biq_sR2];
            biquadF[biq_sR2] = (inputSampleR * biquadF[biq_a2]) - (outSample * biquadF[biq_b2]);
            inputSampleR = outSample;
            outSample = (inputSampleL * biquadG[biq_a0]) + biquadG[biq_sL1];
            biquadG[biq_sL1] = (inputSampleL * biquadG[biq_a1]) - (outSample * biquadG[biq_b1]) + biquadG[biq_sL2];
            biquadG[biq_sL2] = (inputSampleL * biquadG[biq_a2]) - (outSample * biquadG[biq_b2]);
            inputSampleL = outSample;
            outSample = (inputSampleR * biquadG[biq_a0]) + biquadG[biq_sR1];
            biquadG[biq_sR1] = (inputSampleR * biquadG[biq_a1]) - (outSample * biquadG[biq_b1]) + biquadG[biq_sR2];
            biquadG[biq_sR2] = (inputSampleR * biquadG[biq_a2]) - (outSample * biquadG[biq_b2]);
            inputSampleR = outSample;
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::isolator3
