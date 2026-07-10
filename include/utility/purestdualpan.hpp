#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestdualpan {

constexpr std::string_view k_name{ "PurestDualPan" };
constexpr std::string_view k_short_description{
    "PurestDualPan is an updated PurestGain but as a dual pan."
};
constexpr std::string_view k_long_description{
    "This one's by patron request: as in, one specific patron REALLY wanted a PurestDualPan, so now everybody has one :)It is using dezippering, but instead of the much slower one from PurestGain, it's the quicker and more direct one from out of the Z filter plugins. It's using the pan law I used in Console9 and later, that uses sine functions to come up with a pan law. And it's using a quirky little audio taper in its left and right gain controls, so that 0.5 is unity gain, but full crank is actually 3.069492192001773 because it's 2 to the power of the golden ratio, same for the taper of attenuating anything. This produces a funny sort of taper that I like, and also makes it impossible to reference the actual control's number in any useful way. So, behave as if you can't see numbers, they certainly won't be what you think they are and there's no predicting what they will be in dB terms.I have a Bit Shift one also, but it doesn't strictly count as that because it requires an add as part of the algorithm. This one counts as a PurestGain, though! If you're a PurestGain fan, this is the same only as a dual pan."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class PurestDualPan final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double gainLA, gainLB;
    double gainRA, gainRB;
    double panLA, panLB;
    double panRA, panRB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestDualPan()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        D = 1.0;
        gainLA = gainLB = 0.5;
        panLA = panLB = 0.0;
        gainRA = gainRB = 0.5;
        panRA = panRB = 1.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "l vol"; break;
            case kParamB: return "l pan"; break;
            case kParamC: return "r vol"; break;
            case kParamD: return "r pan"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "L Vol"; break;
            case kParamB: return "L Pan"; break;
            case kParamC: return "R Vol"; break;
            case kParamD: return "R Pan"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        gainLA = gainLB;
        gainLB = A;
        panLA = panLB;
        panLB = B;
        gainRA = gainRB;
        gainRB = C;
        panRA = panRB;
        panRB = D;
        while (--sampleFrames >= 0) {
            long double inputSampleL = *in1;
            long double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double temp = (double)sampleFrames / inFramesToProcess;
            long double LpanR = ((panLA * temp) + (panLB * (1.0 - temp))) * 1.57079633;
            long double LpanL = 1.57079633 - LpanR;
            LpanR = sin(LpanR);
            LpanL = sin(LpanL);
            long double gainL = ((gainLA * temp) + (gainLB * (1.0 - temp))) * 2.0;
            gainL = pow(gainL, gainL + 0.618033988749894848204586);
            long double RpanR = ((panRA * temp) + (panRB * (1.0 - temp))) * 1.57079633;
            long double RpanL = 1.57079633 - RpanR;
            RpanR = sin(RpanR);
            RpanL = sin(RpanL);
            long double gainR = ((gainRA * temp) + (gainRB * (1.0 - temp))) * 2.0;
            gainR = pow(gainR, gainR + 0.618033988749894848204586);
            long double LoutL = LpanL * gainL * inputSampleL;
            long double LoutR = LpanR * gainL * inputSampleL;
            long double RoutL = RpanL * gainR * inputSampleR;
            long double RoutR = RpanR * gainR * inputSampleR;
            inputSampleL = LoutL + RoutL;
            inputSampleR = LoutR + RoutR;
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
} // namespace airwindohhs::purestdualpan
