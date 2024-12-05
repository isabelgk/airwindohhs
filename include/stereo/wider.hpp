#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::wider {

constexpr std::string_view k_name{ "Wider" };
constexpr std::string_view k_short_description{
    "Wider is Airwindows stereo space shaping."
};
constexpr std::string_view k_long_description{
    "Here’s a nice little building block. It’s stereo-only for obvious reasons (in AU, you won’t see it available on mono tracks): it’s a stereo space adjuster.It works like this: you’ve got mid and side channels, but taken up several notches. Instead of being adjusted by level controls, the sliders use the Density algorithm. That means if you’re boosting, they get fattened up, and if you cut, they retain some of the edge and definition. This technique from Density has a way of moving audio’s position in space: boost comes forward, and cut moves backward. It turns out that’s perfect for manipulating the shape of a stereo space.But that’s not all: as a final space-manipulating technique, Wider applies an itty-bitty time delay and interpolation (it can be as small as sub-sample) to whichever is the least forward, mid or side. That causes a delicate roll-off and sits the relevant part just a tiny bit back, spatially… and then the audio’s recombined into stereo again. It’s a bold thing to try, but it’s done very subtly so you wouldn’t know it was happening except I’ve just told you, so my secret is out :)The result is this: you can kinda-sorta use Wider to produce extreme stereo effects (look for StereoFX, coming soon, as a better way to get aggressive with space) but it really comes into its own when used to redesign stereo soundfields. With tiny, small adjustments, you can get hugely effective results that sound totally natural. This is the mastering-grade one, where you can transform the source in a convincing and musical way, and not lose anything in the process. The effectiveness and transparency, especially when used for tiny corrections, will make this a go-to plugin for real stereo work."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class Wider final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double p[4099];
    int count;
    float A;
    float B;
    float C;

  public:
    Wider()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        for (int fcount = 0; fcount < 4098; fcount++) {
            p[fcount] = 0.0;
        }
        count = 0;
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
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "width"; break;
            case kParamB: return "center"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Width"; break;
            case kParamB: return "Center"; break;
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
            case kParamB: return ""; break;
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
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
        double mid;
        double side;
        double out;
        double densityside = (A * 2.0) - 1.0;
        double densitymid = (B * 2.0) - 1.0;
        double wet = C;
        // removed extra dry variable
        wet *= 0.5; // we make mid-side by adding/subtracting both channels into each channel
        // and that's why we gotta divide it by 2: otherwise everything's doubled. So, premultiply it to save an extra 'math'
        double offset = (densityside - densitymid) / 2;
        if (offset > 0) {
            offset = sin(offset);
        }
        if (offset < 0) {
            offset = -sin(-offset);
        }
        offset = -(pow(offset, 4) * 20 * overallscale);
        int nearVal = (int)floor(fabs(offset));
        double farLevel = fabs(offset) - nearVal;
        int farVal = nearVal + 1;
        double nearLevel = 1.0 - farLevel;
        double bridgerectifier;
        // interpolating the sample
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            // assign working variables
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            // assign mid and side. Now, High Impact code
            if (densityside != 0.0)
            {
                out = fabs(densityside);
                bridgerectifier = fabs(side) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densityside > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (side > 0) {
                    side = (side * (1 - out)) + (bridgerectifier * out);
                }
                else {
                    side = (side * (1 - out)) - (bridgerectifier * out);
                }
                // blend according to density control
            }
            if (densitymid != 0.0)
            {
                out = fabs(densitymid);
                bridgerectifier = fabs(mid) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (densitymid > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (mid > 0) {
                    mid = (mid * (1 - out)) + (bridgerectifier * out);
                }
                else {
                    mid = (mid * (1 - out)) - (bridgerectifier * out);
                }
                // blend according to density control
            }
            if (count < 1 || count > 2048) {
                count = 2048;
            }
            if (offset > 0)
            {
                p[count + 2048] = p[count] = mid;
                mid = p[count + nearVal] * nearLevel;
                mid += p[count + farVal] * farLevel;
            }
            if (offset < 0)
            {
                p[count + 2048] = p[count] = side;
                side = p[count + nearVal] * nearLevel;
                side += p[count + farVal] * farLevel;
            }
            count -= 1;
            inputSampleL = (drySampleL * (1.0 - wet)) + ((mid + side) * wet);
            inputSampleR = (drySampleR * (1.0 - wet)) + ((mid - side) * wet);
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
} // namespace airwindohhs::wider
