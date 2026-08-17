#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::retrobass {

constexpr std::string_view k_name{ "RetroBass" };
constexpr std::string_view k_short_description{
    "RetroBass is a special kind of highpass."
};
constexpr std::string_view k_long_description{
    "It started with a second-and-fourth-harmonic generator I stumbled into (never fear, you'll get that too). Though it was a little more complex than the simplest second harmonic generators, it still had the unmistakable signs of them: you could mess with the settings until the output clipped like mad and still you'd not really hear what it was doing. The asymmetry has tone qualities, is warm and all, but at such low harmonics it's just so hard to hear.Causing me to wonder: can you use that to take away low frequencies like a filter?So, combining it with a simpler version of the highpass filter I use in Channel, I start experimenting (on a livestream) by modulating it with the same IIR stuff I'm subtracting to make the highpass. Not only that, this asymmetrical even-harmonics thing is using the aliasing-manipulation hack I came up with for normal distortions just recently (and that is buck wild because I invented that on purely hard clipping, but it generalizes) and the next thing you know I'm figuring out whether it's best to bring it in directly, or inverted.And I observe that it produces this funny double bump of a distortion, but one way around the initial lobe of what was once a sine wave normalized to 1.0, is bigger, and what follows is both smaller and a tighter shape. It's acting like it's taking energy in the oscillating wave, and running out mid-oscillation. This is what analog circuits do, when their coupling capacitors run out of steam. It's just being done in a peculiar way.And dialing it in using the test waves, and then trying it out on lots of different things… I discover that it's RetroBuss. It's better than anything else I've had, for specifically making stuff bass-starved in a retro-seeming way. It sounds like old gear not designed for modern subs.Not only that, if I push it farther, it starts to get into those hot harmonic-rich zones that make some old vocals sound so intense. It's like a treble zing that is done through weakening the fundamental. Very handy.RetroBass will continue to be useful in things like retro consoles, but right now it's a standalone highpass you can use however you like. There's more to follow (I did mention the aliasing thing designed around hard clipping) but RetroBass leads the way. :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class RetroBass final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'rtbs';
    float A;
    double rbSampleL, rbSampleR;
    enum
    {
        bip_drbLA,
        bip_drbLB,
        bip_drbLC,
        bip_drbLD,
        bip_prbLA,
        bip_prbLB,
        bip_prbLC,
        bip_prbLD,
        bip_drbRA,
        bip_drbRB,
        bip_drbRC,
        bip_drbRD,
        bip_prbRA,
        bip_prbRB,
        bip_prbRC,
        bip_prbRD,
        bip_total // each distortion section can have one of these, it stacks well
    }; // not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    RetroBass()
    {
        A = 0.5;
        rbSampleL = rbSampleR = 0.0;
        for (int x = 0; x < bip_total; x++) {
            bip[x] = 0.0;
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
            case kParamA: return "freq"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Freq"; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double rbAmount = pow(A * 0.5, 2.0) / overallscale;
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
            rbSampleL = (rbSampleL * (1.0 - rbAmount)) + (inputSampleL * rbAmount);
            if (fabs(rbSampleL) > 1.18e-37) {
                double bip_delta = inputSampleL; // delta can be just local and re-used
                inputSampleL *= rbSampleL * 0.96;
                inputSampleL = fma(((inputSampleL * inputSampleL) * -0.166666666666666), (inputSampleL * inputSampleL), inputSampleL);
                inputSampleL /= rbSampleL * 0.96;
                bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
                bip[bip_drbLB] = bip[bip_prbLA] - bip[bip_drbLA];
                bip[bip_prbLA] = bip[bip_drbLA]; // velocity
                bip[bip_drbLC] = bip[bip_prbLB] - bip[bip_drbLB];
                bip[bip_prbLB] = bip[bip_drbLB]; // acceleration
                bip[bip_drbLD] = bip[bip_prbLC] - bip[bip_drbLC];
                bip[bip_prbLC] = bip[bip_drbLC]; // jerk
                double bip_drbE = bip[bip_prbLD] - bip[bip_drbLD];
                bip[bip_prbLD] = bip[bip_drbLD]; // snap
                inputSampleL *= (1.0 + (fabs(bip[bip_drbLC]) * 0.0618) + (fabs(bip[bip_drbLD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
            }
            inputSampleL -= rbSampleL * 0.92;
            rbSampleR = (rbSampleR * (1.0 - rbAmount)) + (inputSampleR * rbAmount);
            if (fabs(rbSampleR) > 1.18e-37) {
                double bip_delta = inputSampleR; // delta can be just local and re-used
                inputSampleR *= rbSampleR * 0.96;
                inputSampleR = fma(((inputSampleR * inputSampleR) * -0.166666666666666), (inputSampleR * inputSampleR), inputSampleR);
                inputSampleR /= rbSampleR * 0.96;
                bip[bip_drbRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
                bip[bip_drbRB] = bip[bip_prbRA] - bip[bip_drbRA];
                bip[bip_prbRA] = bip[bip_drbRA]; // velocity
                bip[bip_drbRC] = bip[bip_prbRB] - bip[bip_drbRB];
                bip[bip_prbRB] = bip[bip_drbRB]; // acceleration
                bip[bip_drbRD] = bip[bip_prbRC] - bip[bip_drbRC];
                bip[bip_prbRC] = bip[bip_drbRC]; // jerk
                double bip_drbE = bip[bip_prbRD] - bip[bip_drbRD];
                bip[bip_prbRD] = bip[bip_drbRD]; // snap
                inputSampleR *= (1.0 + (fabs(bip[bip_drbRC]) * 0.0618) + (fabs(bip[bip_drbRD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
            }
            inputSampleR -= rbSampleR * 0.92;
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::retrobass
