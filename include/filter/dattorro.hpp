#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::dattorro {

constexpr std::string_view k_name{ "Dattorro" };
constexpr std::string_view k_short_description{
    "Dattorro is the resonant lowpass filter out of Donut."
};
constexpr std::string_view k_long_description{
    "Here's the filter out of Donut! This started as an experiment to see if I could code an SVF. That's a State Variable filter, and now that the other plugin makers have stopped laughing I'll tell you what it is :)Turns out a State Variable filter is about the simplest way you can do biquad-filter-like things (read: controllable and flexible) and plugin devs really like this one. That includes heavyweight genius devs like Andy from Cytomic who made The Glue and The Drop (Andy is a 'model every component' guy doing stuff I choose not to do, a rigorous circuit modeler). Dattorro is not that: Dattorro is me taking the concept of the state variable filter and doing Airwindowsy things with it.For instance, the frequency control's designed to go right up to the high frequency limit of the filter no matter what sample rate it's running under. It does this not by restricting the max frequency, but by altering the logarithm taper of the control so midrange frequencies will always be roughly in the middle of the control's travel. That means you never see exactly what frequency you're using, but neither do you see it on a 303 or x0xb0x and those are great, and this is very much a synth style filter.There's code for every kind of SVF inside Dattorro's source code, commented out, but the lowpass you get (and the resonant behavior, it has a great reso flavor) is not stock SVF, but airwindowsized. That's because I'm using a sin() function in there (for code simplicity. PurestSaturation would work just as well for this, or TapeHack) but not in the way you'd think. The obvious thing to do is put it on as a post clipper for simply adding distortion.But you know I'm always interested in anti-saturation, and so the softclip is going on the bandpass output… which is then SUBTRACTED from the lowpass (well, 0.5 of bandpass into sin() and then subtracted, I was working out what sounded best) and that's the filter. It's nearly as simple as the stock SVF but it sounds way, way slicker and that's what's in Donut and now here it is as a standalone filter for you.The sound is like the filtered-out part gets deeper and more vivid and interesting. Applying inverted saturation is like expansion: it's part of Dubly, and is like inverse Density, and is the buss section of every Console plugin in one way or another. It makes stuff sound farther away. Applying it to a synth filter in this way gives sonic depth and also a curious side-effect: the rolloff actually got shallower, even when it's resonant.This might be a clue towards something that could make a better Baxandall-style filter. I'll keep experimenting. Enjoy Dattorro :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Dattorro final : public Effect<T>
{
    float A;
    float B;
    float C;
    double lowL, lowR;
    double bandL, bandR;
    double freqA, freqB;
    double resoA, resoB;
    double outA, outB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Dattorro()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        lowL = lowR = bandL = bandR = 0.0;
        freqA = freqB = 0.5;
        resoA = resoB = 0.5;
        outA = outB = 1.0;
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
            case kParamA: return "freq"; break;
            case kParamB: return "reso"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Freq"; break;
            case kParamB: return "Reso"; break;
            case kParamC: return "Output"; break;

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
        freqA = freqB;
        resoA = resoB;
        outA = outB;
        freqB = pow(A, overallscale + 1.0) * 1.225;
        resoB = pow(1.0 - B, 2.0);
        if (resoB < 0.001) {
            resoB = 0.001; // q of 0.0 is just a tone
        }
        outB = C / sqrt(resoB);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            const double temp = (double)sampleFrames / inFramesToProcess;
            const double freq = (freqA * temp) + (freqB * (1.0 - temp));
            const double reso = (resoA * temp) + (resoB * (1.0 - temp));
            const double out = (outA * temp) + (outB * (1.0 - temp)); // dezippering
            lowL += freq * bandL;
            bandL += freq * ((reso * inputSampleL) - lowL - (reso * bandL));
            inputSampleL = (lowL - sin(bandL * 0.5)) * out;
            lowR += freq * bandR;
            bandR += freq * ((reso * inputSampleR) - lowR - (reso * bandR));
            inputSampleR = (lowR - sin(bandR * 0.5)) * out; // airwindattorro
            // since this is called Dattorro, I'm including a variation on
            // the textbook code for this, so you can have the normal SVF
            // on tap if you want its lowpass, bandpass or highpass.
            // You can steepen it by cascading additional layers of SVF.
            // the Dattorro source does not produce correct frequencies.
            // it uses cutoff = GetParameter( kParam_A )*20000.0;
            // and then f = 2.0*sin(M_PI * (cutoff / GetSampleRate()));
            // this causes a crash when f is higher than 0.25 Nyquist
            // and also doesn't return the right frequency
            //-------- here is the controls code, for outside the buffer
            // double f = pow(GetParameter( kParam_A ),overallscale+1.0)*1.225;
            // double q = pow(1.0-GetParameter( kParam_B ),2.0); //reso
            // if (q < 0.001) q = 0.001; // q of 0.0 is just a tone
            // double outL = GetParameter( kParam_C ); //lowpass output
            // double outB = 0.0; //bandpass output
            // double outH = 0.0; //highpass output
            // notch output is simply highpass+lowpass
            //-------- here is the audio code, inside the buffer
            // low += f*band;
            // band += f*((q*inputSample)-low-(q*band));
            // const double high = (q*inputSample) - low - (q*band);
            // band += f*high;
            // inputSample = 0.0; //now let's build from the outputs
            // inputSample += low*outL;
            // inputSample += band*outB;
            // inputSample += high*outH;
            //-------- and we're done, that's a Dattorro SVF
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
} // namespace airwindohhs::dattorro
