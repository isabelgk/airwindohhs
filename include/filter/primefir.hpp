#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::primefir {

constexpr std::string_view k_name{ "PrimeFIR" };
constexpr std::string_view k_short_description{
    "PrimeFIR is a mostly linear-phase brickwall with a taste for the bizarre!"
};
constexpr std::string_view k_long_description{
    "It's experiment time again! Inspired by the original filter in Baconpaul's Six Sines FM synth (which I recommend the heck out of, by the way) I got busy trying to make a similar type of filter. This is not how I usually filter. I have a great distaste for the smearyness of linear phase and the weird vibe the pre-ring gives off, but it was amazing in Six Sines, partly because of the incredibly tiny and CPU-efficient window, and so I had a whack at the problem.As usual I had a whack at it in pinata fashion, so rather than do it properly I came up with a way to implement the sinc function I needed for the brickwall, with a sin() based window, and for good measure I made the post-ring longer than the pre-ring just because I could, and made the window size variable so you could filter right down to moderately low frequencies, and that's the first part of the experiment.And then I thought it would be fun to calculate out the sinc function for what it would need to be if you took, not every sample, but every PRIME sample in that window, and used that. Only some of the samples, but across a much MUCH broader time window for the same number of filter positions. And also those positions wouldn't be reinforcing each other, because they're prime numbers. How hard could it be?So, when you turn the 'prime' control over 0.5, you get a completely different filter, if that's still an appropriate name for it. In theory it'll filter down lower, except it completely fails to be a brickwall. It's sort of like a shelf? And sort of phasey, and sort of diffuse, and it's yours to play with. You're hearing only a window of the prime-numbered samples peeking through and acting like it's a filter or something resembling one.And you've heard something like it before… because nearly twenty (!) years ago, my plugin Iron Oxide came out, and one of the things it does is build tone shaping out of prime-spaced successions of samples. No kidding: this technique hasn't really been played with since Iron Oxide and variations on BrightAmbience. Here it's a way to broaden the range of frequencies a brickwall filter can 'reach', except that since it's such a sparse set of filter coefficients, it stops working like any normal brickwall filter and turns into something else.If I wanted to make it be a similarly inefficient highpass, merely including an 'Inv/Wet' control wouldn't be enough, I'd need to doctor the filter so instead it was accepting the nonprimes and omitting the primes, run the brickwall, and then subtract that.This (not the prime version) is also what I've been trying for the last two reverbs, and some folks love that but others cried foul. I'm going to broaden my filter palette a bit for the reverbs, though now I know what this one can do (a sort of slick flashy effect) and will most likely turn to the Pear filter with nonlinearity for the next few reverbs. I've got about four different methods not counting simple biquads that will do different things. Even one (BezEQ!) that will produce gnarly artifacts for texture, for certain kinds of special effects.For now, enjoy playing with the experimental PrimeFIR. It might just lead to an interesting sort of highpass for livening up things as significant as lead vocals… more on that later. Have fun :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class PrimeFIR final : public Effect<T>
{
    float A;
    float B;
    float C;
    double firBufferL[32768];
    double firBufferR[32768];
    int firPosition;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PrimeFIR()
    {
        A = 1.0;
        B = 0.5;
        C = 0.0;
        for (int count = 0; count < 32767; count++) {
            firBufferL[count] = 0.0;
            firBufferR[count] = 0.0;
        }
        firPosition = 0;
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
            case kParamA: return 1.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "freq"; break;
            case kParamB: return "window"; break;
            case kParamC: return "prime"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Freq"; break;
            case kParamB: return "Window"; break;
            case kParamC: return "Prime"; break;

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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        double freq = pow(A, 2) * M_PI_2;
        if (freq < 0.0001) {
            freq = 0.0001;
        }
        double positionMiddle = sin(freq) * 0.5; // shift relative to frequency, not sample-rate
        freq /= overallscale; // generating the FIR relative to sample rate
        int window = (int)(B * 256.0 * overallscale); // so's the window size
        if (window < 2) {
            window = 2;
        }
        if (window > 998) {
            window = 998;
        }
        double fir[1000];
        int middle = (int)((double)window * positionMiddle);
        bool nonPrime = (C < 0.5);
        if (nonPrime) {
            for (int fip = 0; fip < middle; fip++) {
                fir[fip] = (fip - middle) * freq;
                fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
                fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
            }
            fir[middle] = 1.0;
            for (int fip = middle + 1; fip < window; fip++) {
                fir[fip] = (fip - middle) * freq;
                fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
                fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
            }
        }
        else {
            for (int fip = 0; fip < middle; fip++) {
                fir[fip] = (prime[middle - fip]) * freq;
                fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
                fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
            }
            fir[middle] = 1.0;
            for (int fip = middle + 1; fip < window; fip++) {
                fir[fip] = (prime[fip - middle]) * freq;
                fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
                fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
            }
        }
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (firPosition < 0 || firPosition > 32767) {
                firPosition = 32767;
            }
            int firp = firPosition;
            firBufferL[firp] = inputSampleL;
            inputSampleL = 0.0;
            firBufferR[firp] = inputSampleR;
            inputSampleR = 0.0;
            if (nonPrime) {
                if (firp + window < 32767) {
                    for (int fip = 1; fip < window; fip++) {
                        inputSampleL += firBufferL[firp + fip] * fir[fip];
                        inputSampleR += firBufferR[firp + fip] * fir[fip];
                    }
                }
                else {
                    for (int fip = 1; fip < window; fip++) {
                        inputSampleL += firBufferL[firp + fip - ((firp + fip > 32767) ? 32768 : 0)] * fir[fip];
                        inputSampleR += firBufferR[firp + fip - ((firp + fip > 32767) ? 32768 : 0)] * fir[fip];
                    }
                }
                inputSampleL *= 0.25;
                inputSampleR *= 0.25;
            }
            else {
                if (firp + prime[window] < 32767) {
                    for (int fip = 1; fip < window; fip++) {
                        inputSampleL += firBufferL[firp + prime[fip]] * fir[fip];
                        inputSampleR += firBufferR[firp + prime[fip]] * fir[fip];
                    }
                }
                else {
                    for (int fip = 1; fip < window; fip++) {
                        inputSampleL += firBufferL[firp + prime[fip] - ((firp + prime[fip] > 32767) ? 32768 : 0)] * fir[fip];
                        inputSampleR += firBufferR[firp + prime[fip] - ((firp + prime[fip] > 32767) ? 32768 : 0)] * fir[fip];
                    }
                }
                inputSampleL *= 0.5;
                inputSampleR *= 0.5;
            }
            inputSampleL *= sqrt(freq); // compensate for gain
            inputSampleR *= sqrt(freq); // compensate for gain
            firPosition--;
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
} // namespace airwindohhs::primefir
