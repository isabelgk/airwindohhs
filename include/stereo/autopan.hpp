#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::autopan {

constexpr std::string_view k_name{ "AutoPan" };
constexpr std::string_view k_short_description{
    "AutoPan is for getting some organic stereo movement into your mix."
};
constexpr std::string_view k_long_description{
    "This is pretty simple: I got asked for an auto-pan. So I Airwindowsified it a bit, and here you go.It’s set up to get some nice organic movement into your mix in various ways. Rate is how fast it goes (from nearly static, to a rapid flutter).Phase means each channel relative to each other: on either side, you get full L-R and back again. At the center, it’s a mono tremolo (though, for stereo channels). Off-center, it’ll offset the sines in such a way that the sound sets up a swirling, circular stereo motion: swooping forward from the side, going across, and then back to near-silence again. Basically, centered is ‘not side motion’ and to the sides is ‘more side motion’.Wide just cuts the mid channel. What this does for the auto-panning is, it’ll make stuff stretch out a bit beyond the edges of the speakers. If you’ve got something full to the side then you’ll get a bit of the opposite on the other speaker. Full wide is ‘only side channel’ and you’ll hear it in both because that’s what you get when you have only side channel: centered ‘mono’ sound, just out of phase completely. You’ll probably want to use smaller amounts of this, unless…Dry/Wet is the final useful thing here. You can set up extreme stuff, like full Side or weird motions with Phase, and then just dial it back with Dry/Wet and it’ll give you a nice subtle activity of whatever you set up: don’t sleep on Dry/Wet here, you can get nice effects through using aggressive settings elsewhere and just sneaking in a little Dry/Wet. Especially if you’re doing extreme things with Wide, Dry/Wet is how you can integrate that into a sound so it’s just a nice little subtle motion that’s interesting. Or of course you can just crank it out, I’m not your mom :)"
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class AutoPan final : public Effect<T>
{
    double rate, oldfpd;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    AutoPan()
    {
        A = 0.1;
        B = 0.5;
        C = 0.0;
        D = 1.0;
        rate = 3.0;
        oldfpd = 429496.7295;
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
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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
            case kParamD: return D; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.1; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.0; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "rate"; break;
            case kParamB: return "phase"; break;
            case kParamC: return "wide"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Rate"; break;
            case kParamB: return "Phase"; break;
            case kParamC: return "Wide"; break;
            case kParamD: return "Dry/Wet"; break;

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
            case kParamD: return std::to_string(D); break;

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
            case kParamC: return ""; break;
            case kParamD: return ""; break;

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
        double drift = (pow(A, 4) * 0.01) / overallscale;
        double offset = 3.141592653589793 + (B * 6.283185307179586);
        double panlaw = 1.0 - pow(C, 2);
        double wet = D;
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
            rate += (oldfpd * drift);
            if (rate > 6.283185307179586) {
                rate = 0.0;
                oldfpd = 0.4294967295 + (fpdL * 0.0000000000618);
            }
            else if (rate < 0.0) {
                rate = 6.283185307179586;
                oldfpd = 0.4294967295 + (fpdL * 0.0000000000618);
            }
            inputSampleL *= (sin(rate) + 1.0);
            inputSampleR *= (sin(rate + offset) + 1.0);
            double mid = (inputSampleL + inputSampleR) * panlaw;
            double side = inputSampleL - inputSampleR;
            // assign mid and side.Between these sections, you can do mid/side processing
            inputSampleL = (mid + side) / 4.0;
            inputSampleR = (mid - side) / 4.0;
            // unassign mid and side, and compensate for the sin()+1 volume boost
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::autopan
