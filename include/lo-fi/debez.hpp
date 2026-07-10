#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::debez {

constexpr std::string_view k_name{ "DeBez" };
constexpr std::string_view k_short_description{
    "DeBez gives you retro sampley textures!"
};
constexpr std::string_view k_long_description{
    "Here's a way-point along the path of me figuring out how to do things! DeBez is a little bit like previous DeRez plugins I've made, and a little bit like my reverbs, and a little bit like HipCrush. I got there from here, and you can add these sounds to your palette!It combines three things. First, bitcrush like in HipCrush where moving the control to the right gives you 'compressey' bitcrushes that will bring up noise floors and make them roar, and moving the control to the left gives you 'gated' textures where the bitcrush is offset so it'll cut out. This is more obvious on extreme bitcrushes, but it'll be the case even on subtler, 'texture changing' bitcrushes.The DeBez control is simply Bezier undersampling, just like in my reverbs. It's smoothing the edges of the bitcrushing, happening after the crush. To the right, you're getting a 'continuous' version which generates a weird digital-hell overtone. To the left, you get a 'stepped' version of the same thing, one that snaps to integer numbers to suppress (mostly) the overtone. That one might be what you want if you're going for an 'old sampler' effect, as it'll be less edgy, more solid.Then, instead of just a dry/wet you're getting an inv/dry/wet control. Turn that up all the way and you get the full DeBez effect, at the middle you have dry again, but to the left you are subtracting the crushed version from dry. This is gonna interact in curious ways as you do things like bitcrush in gatey mode, or roll off highs with DeBez, or play with that overtone.And what does it do when you do that? It exactly clones the vintage and unobtainable sampler of… haha no. It absolutely does not! Instead it makes sounds that have NOT been heard before. It's on you to see if they're useful, and I can't tell you what it's useful for because I don't often go for bitcrush effects in the first place so I'm not the one with a vision here. The point is, now you have a tiny plugin that can produce really unusual sounds along these lines, and when you include subtracting the crush version (which can also be a treble-cut, or overtone-added version) it's a tiny mad science lab just waiting to happen.Have fun! I have one more weekend before 2026 and plenty of stuff to do so I'll get back to work :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeBez final : public Effect<T>
{
    float A;
    float B;
    float C;
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_InR,
        bez_UnInR,
        bez_SampR,
        bez_AvgInSampL,
        bez_AvgInSampR,
        bez_AvgOutSampL,
        bez_AvgOutSampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezF[bez_total];
    double rezA;
    double rezB;
    double bitA;
    double bitB;
    double wetA;
    double wetB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    DeBez()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        for (int x = 0; x < bez_total; x++) {
            bezF[x] = 0.0;
        }
        bezF[bez_cycle] = 1.0;
        rezA = 0.5;
        rezB = 0.5;
        bitA = 0.5;
        bitB = 0.5;
        wetA = 1.0;
        wetB = 1.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "debez"; break;
            case kParamB: return "derez"; break;
            case kParamC: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "DeBez"; break;
            case kParamB: return "DeRez"; break;
            case kParamC: return "Inv/Wet"; break;

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
        rezA = rezB;
        rezB = A * 2.0;
        bitA = bitB;
        bitB = B * 2.0;
        wetA = wetB;
        wetB = C * 2.0;
        bool steppedFreq = true; // Revised Bezier Undersampling
        if (rezB > 1.0) { // has full rez at center, stepped
            steppedFreq = false; // to left, continuous to right
            rezB = 1.0 - (rezB - 1.0);
        } // if it's set up like that it's the revised algorithm
        rezB = fmin(fmax(pow(rezB, 3.0), 0.0005), 1.0); // note: no overallscale, already inside undersampling
        int bezFreqFraction = (int)(1.0 / rezB);
        double bezFreqTrim = (double)bezFreqFraction / (bezFreqFraction + 1.0);
        if (steppedFreq) { // this hard-locks derez to exact subdivisions of 1.0
            rezB = 1.0 / bezFreqFraction;
            bezFreqTrim = 1.0 - (rezB * bezFreqTrim);
        }
        else { // this makes it match the 1.0 case using stepped
            bezFreqTrim = 1.0 - pow(rezB * 0.5, 1.0 / (rezB * 0.5));
        } // the revision more accurately connects the bezier curves
        while (--sampleFrames >= 0) {
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
            double rez = (rezA * temp) + (rezB * (1.0 - temp));
            double bit = ((bitA * temp) + (bitB * (1.0 - temp))) - 1.0;
            double wet = ((wetA * temp) + (wetB * (1.0 - temp))) - 1.0;
            double dry = 1.0 - wet;
            if (wet > 1.0) {
                wet = 1.0;
            }
            if (wet < -1.0) {
                wet = -1.0;
            }
            if (dry > 1.0) {
                dry = 1.0;
            }
            if (dry < 0.0) {
                dry = 0.0;
            }
            // this bitcrush can be subtracted.
            bezF[bez_cycle] += rez;
            bezF[bez_SampL] += (inputSampleL * rez);
            bezF[bez_SampR] += (inputSampleR * rez);
            if (bezF[bez_cycle] > 1.0) {
                if (steppedFreq) {
                    bezF[bez_cycle] = 0.0;
                }
                else {
                    bezF[bez_cycle] -= 1.0;
                }
                inputSampleL = (bezF[bez_SampL] + bezF[bez_AvgInSampL]) * 0.5;
                bezF[bez_AvgInSampL] = bezF[bez_SampL];
                inputSampleR = (bezF[bez_SampR] + bezF[bez_AvgInSampR]) * 0.5;
                bezF[bez_AvgInSampR] = bezF[bez_SampR];
                bool crushGate = (bit < 0.0);
                bit = 1.0 - fabs(bit);
                bit = fmin(fmax(bit * 16.0, 0.5), 16.0);
                double bitFactor = pow(2.0, bit);
                inputSampleL *= bitFactor;
                inputSampleL = floor(inputSampleL + (crushGate ? 0.5 / bitFactor : 0.0));
                inputSampleL /= bitFactor;
                inputSampleR *= bitFactor;
                inputSampleR = floor(inputSampleR + (crushGate ? 0.5 / bitFactor : 0.0));
                inputSampleR /= bitFactor;
                // derez inside debez
                bezF[bez_CL] = bezF[bez_BL];
                bezF[bez_BL] = bezF[bez_AL];
                bezF[bez_AL] = inputSampleL;
                bezF[bez_SampL] = 0.0;
                bezF[bez_CR] = bezF[bez_BR];
                bezF[bez_BR] = bezF[bez_AR];
                bezF[bez_AR] = inputSampleR;
                bezF[bez_SampR] = 0.0;
            }
            double X = bezF[bez_cycle] * bezFreqTrim;
            double CBLfreq = (bezF[bez_CL] * (1.0 - X)) + (bezF[bez_BL] * X);
            double BALfreq = (bezF[bez_BL] * (1.0 - X)) + (bezF[bez_AL] * X);
            double CBALfreq = (bezF[bez_BL] + (CBLfreq * (1.0 - X)) + (BALfreq * X)) * 0.125;
            inputSampleL = CBALfreq + bezF[bez_AvgOutSampL];
            bezF[bez_AvgOutSampL] = CBALfreq;
            double CBRfreq = (bezF[bez_CR] * (1.0 - X)) + (bezF[bez_BR] * X);
            double BARfreq = (bezF[bez_BR] * (1.0 - X)) + (bezF[bez_AR] * X);
            double CBARfreq = (bezF[bez_BR] + (CBRfreq * (1.0 - X)) + (BARfreq * X)) * 0.125;
            inputSampleR = CBARfreq + bezF[bez_AvgOutSampR];
            bezF[bez_AvgOutSampR] = CBARfreq;
            inputSampleL = (wet * inputSampleL) + (dry * drySampleL);
            inputSampleR = (wet * inputSampleR) + (dry * drySampleR);
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
} // namespace airwindohhs::debez
