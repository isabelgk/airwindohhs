#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::silken {

constexpr std::string_view k_name{ "Silken" };
constexpr std::string_view k_short_description{
    "Silken is a high frequency boost that gives ambience and texture."
};
constexpr std::string_view k_long_description{
    "Here's one where the plugin is probably better than my ability to demo it. Silken is a kind of high frequency boost, based on PrimeFIR used in 'prime mode' but backwards, so rather than being a lowpass with lots of ambient leakage, it's a method of subtracting such a brickwall mostly linear phase lowpass with different leakage.So that's a lot. You can simply listen to it and see whether it is able to do 'silky high frequency boosts such as you might use for lead vocals' or you can bear with me as I try to explain HOW it does that thing. Because, even though stuff's kinda hectic around here and my video wasn't good, the plugin I made really brings a useful texture. If I could sing better I'd be all over this demonstrating how great it is, and even so it might help out. I feel it might click with people so it behooves me to explain how it's real.So, you can run a brickwall filter, using a 'window' (one of the controls) to determine just how steep the filter's gonna be. It's an algorithm called a sinc filter, and the wider the window, the steeper your brickwall can be. It's a phase-linear filter, so it has pre-echo and it has latency. Silken does not compensate for this latency, which depends on how wide the window is: it's a slightly unusual arrangement because it's not completely symmetrical in an effort to cut down on the latency.So far so good. It's like a shelf for boosting highs and cutting lows. But then, bring in what PrimeFIR does. That lets you make the filter only from prime numbered samples, and not every sample. What happens when you make a sparse filter like that? I've made multiple plugins that use this TYPE of effect: BrightAmbience, and in fact my classic plugin Iron Oxide. There's plenty of experience in using this type of effect… at least with me :)PrimeFIR can make a 'lowpass' out of only prime-numbered samples (part of the filter, not just counting every sample in your audio) and it lets through a sort of 'haze' around the filtered sound, as audio across the whole window bleeds through. What Silken does is different in two ways. First, it's subtracting the filtered part, to make it a highpass. Second, it's constructing the filter out of only NON-prime samples, this time. So what's happening is, it's more effective at being a highpass than PrimeFIR is at being a lowpass, but the stuff that leaks through is still out of the prime numbers because those are now the ones NOT being subtracted.You get a highpass where, the harder you push it, the more of an 'aura of silky ambience' you get around the highs. It will sort of diffuse super high frequency transient information, like a diffusion filter does for visual information. The result is flattering in exactly the same way a camera's diffusion filter is. It should work fine on even the most high quality sources, but it should be an absolute lifesaver on the kind of nasty mic (like certain lavaliers!) that puts out distressing hyperfocussed bright transients. Now, you can diffuse that and change the texture of it, not just turn the brightness you've got up and down.Hope you like it :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Silken final : public Effect<T>
{
    float A;
    float B;
    float C;
    double firBufferL[32768];
    double firBufferR[32768];
    double fir[1000];
    int firPosition;
    double firlastSampleL;
    double infirmediateL[16];
    double firlastSampleR;
    double infirmediateR[16];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Silken()
    {
        A = 0.0;
        B = 0.5;
        C = 0.5;
        for (int count = 0; count < 32767; count++) {
            firBufferL[count] = 0.0;
            firBufferR[count] = 0.0;
        }
        for (int count = 0; count < 999; count++) {
            fir[count] = 0.0;
        }
        firPosition = 0;
        firlastSampleL = 0.0;
        firlastSampleR = 0.0;
        for (int x = 0; x < 16; x++) {
            infirmediateL[x] = 0.0;
            infirmediateR[x] = 0.0;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "silken"; break;
            case kParamB: return "freq"; break;
            case kParamC: return "window"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Silken"; break;
            case kParamB: return "Freq"; break;
            case kParamC: return "Window"; break;

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
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        double wet = A;
        double freq = pow(B, 2) * M_PI_2;
        if (freq < 0.0001) {
            freq = 0.0001;
        }
        double positionMiddle = sin(freq) * 0.5; // shift relative to frequency, not sample-rate
        freq /= overallscale; // generating the FIR relative to sample rate
        const int window = (int)fmin((C * C * 256.0 * overallscale) + 2.0, 998.0); // so's the window size
        const int middle = (int)((double)window * positionMiddle);
        for (int fip = 0; fip < middle; fip++) {
            fir[fip] = (unprime[middle - fip]) * freq;
            fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
            fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
        }
        fir[middle] = 1.0;
        for (int fip = middle + 1; fip < window; fip++) {
            fir[fip] = (unprime[fip - middle]) * freq;
            fir[fip] = sin(fir[fip]) / fir[fip]; // sinc function
            fir[fip] *= sin(((double)fip / (double)window) * M_PI); // windowed with sin()
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
            if (firp + unprime[window] < 32767) {
                for (int fip = 1; fip < window; fip++) {
                    inputSampleL += firBufferL[firp + unprime[fip]] * fir[fip];
                    inputSampleR += firBufferR[firp + unprime[fip]] * fir[fip];
                }
            }
            else {
                for (int fip = 1; fip < window; fip++) {
                    inputSampleL += firBufferL[firp + unprime[fip] - ((firp + unprime[fip] > 32767) ? 32768 : 0)] * fir[fip];
                    inputSampleR += firBufferR[firp + unprime[fip] - ((firp + unprime[fip] > 32767) ? 32768 : 0)] * fir[fip];
                }
            }
            inputSampleL *= sqrt(freq * 0.618033988749894848204586); // compensate for gain
            inputSampleR *= sqrt(freq * 0.618033988749894848204586); // compensate for gain
            firPosition--;
            double softSpeed = fabs(inputSampleL);
            if (softSpeed < 1.0) {
                softSpeed = 1.0;
            }
            else {
                softSpeed = 1.0 / softSpeed;
            }
            inputSampleL = sin(inputSampleL) * 0.9549925859; // scale to what cliponly uses
            inputSampleL = (inputSampleL * softSpeed) + (firlastSampleL * (1.0 - softSpeed));
            softSpeed = fabs(inputSampleR);
            if (softSpeed < 1.0) {
                softSpeed = 1.0;
            }
            else {
                softSpeed = 1.0 / softSpeed;
            }
            inputSampleR = sin(inputSampleR) * 0.9549925859; // scale to what cliponly uses
            inputSampleR = (inputSampleR * softSpeed) + (firlastSampleR * (1.0 - softSpeed));
            infirmediateL[spacing] = inputSampleL;
            inputSampleL = firlastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                infirmediateL[x - 1] = infirmediateL[x];
            }
            firlastSampleL = infirmediateL[0]; // run a little buffer to handle this
            infirmediateR[spacing] = inputSampleR;
            inputSampleR = firlastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                infirmediateR[x - 1] = infirmediateR[x];
            }
            firlastSampleR = infirmediateR[0]; // run a little buffer to handle this
            if (firp + unprime[middle] < 32768) {
                inputSampleL = (firBufferL[firp + unprime[middle]] * (wet + 1.0)) - (inputSampleL * wet);
                inputSampleR = (firBufferR[firp + unprime[middle]] * (wet + 1.0)) - (inputSampleR * wet);
            }
            else {
                inputSampleL = (firBufferL[firp + unprime[middle] - 32768] * (wet + 1.0)) - (inputSampleL * wet);
                inputSampleR = (firBufferR[firp + unprime[middle] - 32768] * (wet + 1.0)) - (inputSampleR * wet);
            } // dry/wet must use a sample from the middle of firBuffer for dry,
            // because it's an FIR filter that is phase linear by nature
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
} // namespace airwindohhs::silken
