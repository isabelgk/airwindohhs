#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubsub2 {

constexpr std::string_view k_name{ "DubSub2" };
constexpr std::string_view k_short_description{
    "DubSub2 is the essence of the Airwindows head bump."
};
constexpr std::string_view k_long_description{
    "The final element for reinventing ToTape is in place, with DubSub2!I've always used this one particular algorithm, both for ToTape and for various bass-emphasis plugins. It acts like a combination between an IIR filter and a saturation. And it's got fantastic sound, but with a catch: it has to be controlled in the higher frequencies or it'll sound growly and distorted if you push it, and it's unstable.That's literal. I have to do stuff to control it or it will throw bass so hard that it sits around pushing DC. The algorithm has to be filtered because it's unstable, it'll constantly bring up deep bass no matter what. The sound of it is intrinsically tied to this behavior.So, what if I tried the technique I use in Parametric, and set up some biquad filters as bandpasses, and then stack them (slightly staggered, for tone purposes) to get better rejection of unwanted DC energy? What could go wrong? Well… it's tricky. If you do that, phase shifts will cause there to be a cancellation around the bandpass frequency. You'll get notches, the placement depending on what your filter bandwidth is.But hang on. Jack Endino's got a webpage where he's measured lots of real analog tape machines, showing the head bumps. And the thing is, on his measurements there's consistently a notch there, too. It's exactly an octave above the head bump, and that's part of the sound.So what's the bandpass resonance, when you're using two of them stacked, and then you want the notch produced by the phase shifting (already an unusual choice) to line up exactly an octave over the head bump resonance, so that your DubSub2 head bump will consistently behave like the real thing? You can set the head bump frequency to whatever you like (Jack finds that doubling the track width halves the head bump frequency, and of course going from 15 ips to 30 ips doubles the head bump frequency, and the notch stays exactly an octave higher). But the Q is what positions that notch. Since there is clearly no correct value for such a bizarre experiment, since super-shallow Q won't work, since using Butterworth (0.7071) is slightly too tight… what's the resonance number for the two stacked bandpasses?The golden ratio.…hope you enjoy DubSub2, and this is what will become the head bump for ToTape7 :)"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class DubSub2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    double headBumpL;
    double headBumpR;
    enum
    {
        hdb_freq,
        hdb_reso,
        hdb_a0,
        hdb_a1,
        hdb_a2,
        hdb_b1,
        hdb_b2,
        hdb_sL1,
        hdb_sL2,
        hdb_sR1,
        hdb_sR2,
        hdb_total
    }; // fixed frequency biquad filter for ultrasonics, stereo
    double hdbA[hdb_total];
    double hdbB[hdb_total];

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DubSub2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        headBumpL = 0.0;
        headBumpR = 0.0;
        for (int x = 0; x < hdb_total; x++) {
            hdbA[x] = 0.0;
            hdbB[x] = 0.0;
        }
        // from ZBandpass, so I can use enums with it
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "headbmp"; break;
            case kParamB: return "headfrq"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "HeadBmp"; break;
            case kParamB: return "HeadFrq"; break;
            case kParamC: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return "hz"; break;
            case kParamC: return ""; break;

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
        double headBumpDrive = (A * 0.1) / overallscale;
        hdbA[hdb_freq] = (((B * B) * 175.0) + 25.0) / Effect<T>::getSampleRate();
        hdbB[hdb_freq] = hdbA[hdb_freq] * 0.9375;
        hdbB[hdb_reso] = hdbA[hdb_reso] = 0.618033988749894848204586;
        hdbB[hdb_a1] = hdbA[hdb_a1] = 0.0;
        double K = tan(M_PI * hdbA[hdb_freq]);
        double norm = 1.0 / (1.0 + K / hdbA[hdb_reso] + K * K);
        hdbA[hdb_a0] = K / hdbA[hdb_reso] * norm;
        hdbA[hdb_a2] = -hdbA[hdb_a0];
        hdbA[hdb_b1] = 2.0 * (K * K - 1.0) * norm;
        hdbA[hdb_b2] = (1.0 - K / hdbA[hdb_reso] + K * K) * norm;
        K = tan(M_PI * hdbB[hdb_freq]);
        norm = 1.0 / (1.0 + K / hdbB[hdb_reso] + K * K);
        hdbB[hdb_a0] = K / hdbB[hdb_reso] * norm;
        hdbB[hdb_a2] = -hdbB[hdb_a0];
        hdbB[hdb_b1] = 2.0 * (K * K - 1.0) * norm;
        hdbB[hdb_b2] = (1.0 - K / hdbB[hdb_reso] + K * K) * norm;
        double headWet = C;
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
            // begin HeadBump
            headBumpL += (inputSampleL * headBumpDrive);
            headBumpL -= (headBumpL * headBumpL * headBumpL * (0.0618 / sqrt(overallscale)));
            headBumpR += (inputSampleR * headBumpDrive);
            headBumpR -= (headBumpR * headBumpR * headBumpR * (0.0618 / sqrt(overallscale)));
            double headBiqSampleL = (headBumpL * hdbA[hdb_a0]) + hdbA[hdb_sL1];
            hdbA[hdb_sL1] = (headBumpL * hdbA[hdb_a1]) - (headBiqSampleL * hdbA[hdb_b1]) + hdbA[hdb_sL2];
            hdbA[hdb_sL2] = (headBumpL * hdbA[hdb_a2]) - (headBiqSampleL * hdbA[hdb_b2]);
            double headBumpSampleL = (headBiqSampleL * hdbB[hdb_a0]) + hdbB[hdb_sL1];
            hdbB[hdb_sL1] = (headBiqSampleL * hdbB[hdb_a1]) - (headBumpSampleL * hdbB[hdb_b1]) + hdbB[hdb_sL2];
            hdbB[hdb_sL2] = (headBiqSampleL * hdbB[hdb_a2]) - (headBumpSampleL * hdbB[hdb_b2]);
            double headBiqSampleR = (headBumpR * hdbA[hdb_a0]) + hdbA[hdb_sR1];
            hdbA[hdb_sR1] = (headBumpR * hdbA[hdb_a1]) - (headBiqSampleR * hdbA[hdb_b1]) + hdbA[hdb_sR2];
            hdbA[hdb_sR2] = (headBumpR * hdbA[hdb_a2]) - (headBiqSampleR * hdbA[hdb_b2]);
            double headBumpSampleR = (headBiqSampleR * hdbB[hdb_a0]) + hdbB[hdb_sR1];
            hdbB[hdb_sR1] = (headBiqSampleR * hdbB[hdb_a1]) - (headBumpSampleR * hdbB[hdb_b1]) + hdbB[hdb_sR2];
            hdbB[hdb_sR2] = (headBiqSampleR * hdbB[hdb_a2]) - (headBumpSampleR * hdbB[hdb_b2]);
            // end HeadBump
            inputSampleL = (headBumpSampleL * headWet) + (drySampleL * (1.0 - headWet));
            inputSampleR = (headBumpSampleR * headWet) + (drySampleR * (1.0 - headWet));
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
} // namespace airwindohhs::dubsub2
