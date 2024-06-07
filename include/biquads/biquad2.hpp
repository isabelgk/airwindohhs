#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquad2 {

constexpr std::string_view k_name{ "Biquad2" };
constexpr std::string_view k_short_description{
    "Biquad2 is the Airwindows biquad filter that's more sweepable and synthy."
};
constexpr std::string_view k_long_description{
    "This time, it’s time for the impossible!As in, there was a reason nobody was doing sweepable, ‘synthy’ biquad filters…Turns out it simply doesn’t work. One uses a different type, like state variable filters, for the synthy stuff. The reason is that, while biquads can sound pretty great (especially implemented like I do ’em), they fundamentally can’t cope with changing the filter coefficients mid-calculation. They flip out: you’ll hear some of that, especially at the frequency extremes. Low Q makes low frequency motions flip out, and high Q (and boy do I have a high Q for you this time) makes ultra-high frequency motions flip out in a really wild. glitchy way.So obviously I gave up.:Dnope! Instead, I just kind of forced the filter into zones where it mostly is controllable. This is partly through REALLY smoothing the filter cutoff, especially at low Q and low frequencies. If you try and update biquad coefficients every sample (and I already changed the form from the more CPU-efficient to the more stable form: didn’t help much at all) the algorithm gets super twitchy, so part of what I’ve done is just stabilizing things. I tried for ages to come up with some bizarre hack to force the filter back into stability: no dice. So, the range has been limited a little, the Q doesn’t adjust below a Q of 1, and it reacts really slowly, because that was the only way I could get it to behave at all. (the original Biquad can sometimes be better behaved, because it’s only recalculating coefficients every new buffer. So, zipper noise. When you get rid of that your troubles get infinitely worse, with biquads)I’ve also got the resonance (on low and highpass) working differently. I’m scaling back loudness, but I’m also applying a distortion, then averaging after that, to try and get an ‘overdriving filter effect’ not present in the purer original Biquad. You can go quieter into Biquad2 and avoid this, or not use as much resonance: I feel it’s kind of like the Roland Alpha Juno filter resonance distortion, not an everyday thing but when it pops up it has its own distinct quality that’s interesting.Anyway here’s Biquad2 ;)"
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class Biquad2 final : public Effect<T>
{
    double biquad[15]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8-9-10 and 11-12-13-14, and freq/res/coefficients serve both.
    double bL[11];
    double bR[11];
    double f[11];
    double frequencychase;
    double resonancechase;
    double outputchase;
    double wetchase;
    double frequencysetting;
    double resonancesetting;
    double outputsetting;
    double wetsetting;
    double chasespeed;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;

  public:
    Biquad2()
    {
        for (int x = 0; x < 15; x++) {
            biquad[x] = 0.0;
        }
        for (int x = 0; x < 11; x++) {
            bL[x] = 0.0;
            bR[x] = 0.0;
            f[x] = 0.0;
        }
        frequencychase = 0.0015;
        resonancechase = 0.001;
        outputchase = 1.0;
        wetchase = 1.0;
        frequencysetting = -1.0;
        resonancesetting = -1.0;
        outputsetting = -1.0;
        wetsetting = -2.0; //-1.0 is a possible setting here and this forces an update on chasespeed
        chasespeed = 500.0;
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        E = 1.0;
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
        kParamE = 4,
        kNumParameters = 5
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

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
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 1.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "type"; break;
            case kParamB: return "freq"; break;
            case kParamC: return "q"; break;
            case kParamD: return "output"; break;
            case kParamE: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Type"; break;
            case kParamB: return "Freq"; break;
            case kParamC: return "Q"; break;
            case kParamD: return "Output"; break;
            case kParamE: return "Inv/Wet"; break;

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
            case kParamE: return std::to_string(E); break;

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
            case kParamE: return ""; break;

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
        int type = ceil((A * 3.999) + 0.00001);
        double average = B * B;
        double frequencytarget = average * 0.39; // biquad[0], goes to 1.0
        frequencytarget /= overallscale;
        if (frequencytarget < 0.0015 / overallscale) {
            frequencytarget = 0.0015 / overallscale;
        }
        double resonancetarget = (C * C * 49.99) + 0.01; // biquad[1], goes to 50.0
        if (resonancetarget < 1.0) {
            resonancetarget = 1.0;
        }
        double outputtarget = D; // scaled to res
        if (type < 3) {
            outputtarget /= sqrt(resonancetarget);
        }
        double wettarget = (E * 2.0) - 1.0; // wet, goes -1.0 to 1.0
        // biquad contains these values:
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        //[2] is a0 but you need distinct ones for additional biquad instances so it's here
        //[3] is a1 but you need distinct ones for additional biquad instances so it's here
        //[4] is a2 but you need distinct ones for additional biquad instances so it's here
        //[5] is b1 but you need distinct ones for additional biquad instances so it's here
        //[6] is b2 but you need distinct ones for additional biquad instances so it's here
        //[7] is a stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is a stored delayed sample (you have to include the coefficient making code if you do that)
        //[9] is a stored delayed sample (you have to include the coefficient making code if you do that)
        //[10] is a stored delayed sample (you have to include the coefficient making code if you do that)
        double K = tan(M_PI * biquad[0]);
        double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
        // finished setting up biquad
        average = (1.0 - average) * 10.0; // max taps is 10, and low settings use more
        if (type == 1 || type == 3) {
            average = 1.0;
        }
        double gain = average;
        if (gain > 1.0) {
            f[0] = 1.0;
            gain -= 1.0;
        }
        else {
            f[0] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[1] = 1.0;
            gain -= 1.0;
        }
        else {
            f[1] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[2] = 1.0;
            gain -= 1.0;
        }
        else {
            f[2] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[3] = 1.0;
            gain -= 1.0;
        }
        else {
            f[3] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[4] = 1.0;
            gain -= 1.0;
        }
        else {
            f[4] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[5] = 1.0;
            gain -= 1.0;
        }
        else {
            f[5] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[6] = 1.0;
            gain -= 1.0;
        }
        else {
            f[6] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[7] = 1.0;
            gain -= 1.0;
        }
        else {
            f[7] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[8] = 1.0;
            gain -= 1.0;
        }
        else {
            f[8] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[9] = 1.0;
            gain -= 1.0;
        }
        else {
            f[9] = gain;
            gain = 0.0;
        }
        // there, now we have a neat little moving average with remainders
        if (average < 1.0) {
            average = 1.0;
        }
        f[0] /= average;
        f[1] /= average;
        f[2] /= average;
        f[3] /= average;
        f[4] /= average;
        f[5] /= average;
        f[6] /= average;
        f[7] /= average;
        f[8] /= average;
        f[9] /= average;
        // and now it's neatly scaled, too
        // finished setting up average
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
            double chasespeed = 50000;
            if (frequencychase < frequencytarget) {
                chasespeed = 500000;
            }
            chasespeed /= resonancechase;
            chasespeed *= overallscale;
            frequencychase = (((frequencychase * chasespeed) + frequencytarget) / (chasespeed + 1.0));
            double fasterchase = 1000 * overallscale;
            resonancechase = (((resonancechase * fasterchase) + resonancetarget) / (fasterchase + 1.0));
            outputchase = (((outputchase * fasterchase) + outputtarget) / (fasterchase + 1.0));
            wetchase = (((wetchase * fasterchase) + wettarget) / (fasterchase + 1.0));
            if (biquad[0] != frequencychase) {
                biquad[0] = frequencychase;
                K = tan(M_PI * biquad[0]);
            }
            if (biquad[1] != resonancechase) {
                biquad[1] = resonancechase;
                norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            }
            if (type == 1) { // lowpass
                biquad[2] = K * K * norm;
                biquad[3] = 2.0 * biquad[2];
                biquad[4] = biquad[2];
                biquad[5] = 2.0 * (K * K - 1.0) * norm;
            }
            if (type == 2) { // highpass
                biquad[2] = norm;
                biquad[3] = -2.0 * biquad[2];
                biquad[4] = biquad[2];
                biquad[5] = 2.0 * (K * K - 1.0) * norm;
            }
            if (type == 3) { // bandpass
                biquad[2] = K / biquad[1] * norm;
                biquad[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
                biquad[4] = -biquad[2];
                biquad[5] = 2.0 * (K * K - 1.0) * norm;
            }
            if (type == 4) { // notch
                biquad[2] = (1.0 + K * K) * norm;
                biquad[3] = 2.0 * (K * K - 1) * norm;
                biquad[4] = biquad[2];
                biquad[5] = biquad[3];
            }
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double outSampleL = biquad[2] * inputSampleL + biquad[3] * biquad[7] + biquad[4] * biquad[8] - biquad[5] * biquad[9] - biquad[6] * biquad[10];
            biquad[8] = biquad[7];
            biquad[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquad[10] = biquad[9];
            biquad[9] = inputSampleL; // DF1 left
            double outSampleR = biquad[2] * inputSampleR + biquad[3] * biquad[11] + biquad[4] * biquad[12] - biquad[5] * biquad[13] - biquad[6] * biquad[14];
            biquad[12] = biquad[11];
            biquad[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquad[14] = biquad[13];
            biquad[13] = inputSampleR; // DF1 right
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            bL[9] = bL[8];
            bL[8] = bL[7];
            bL[7] = bL[6];
            bL[6] = bL[5];
            bL[5] = bL[4];
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = inputSampleL;
            bR[9] = bR[8];
            bR[8] = bR[7];
            bR[7] = bR[6];
            bR[6] = bR[5];
            bR[5] = bR[4];
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = inputSampleR;
            inputSampleL *= f[0];
            inputSampleL += (bL[1] * f[1]);
            inputSampleL += (bL[2] * f[2]);
            inputSampleL += (bL[3] * f[3]);
            inputSampleL += (bL[4] * f[4]);
            inputSampleL += (bL[5] * f[5]);
            inputSampleL += (bL[6] * f[6]);
            inputSampleL += (bL[7] * f[7]);
            inputSampleL += (bL[8] * f[8]);
            inputSampleL += (bL[9] * f[9]); // intense averaging on deeper cutoffs
            inputSampleR *= f[0];
            inputSampleR += (bR[1] * f[1]);
            inputSampleR += (bR[2] * f[2]);
            inputSampleR += (bR[3] * f[3]);
            inputSampleR += (bR[4] * f[4]);
            inputSampleR += (bR[5] * f[5]);
            inputSampleR += (bR[6] * f[6]);
            inputSampleR += (bR[7] * f[7]);
            inputSampleR += (bR[8] * f[8]);
            inputSampleR += (bR[9] * f[9]); // intense averaging on deeper cutoffs
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // and then Console5 will spit out overs if you let it
            if (outputchase < 1.0) {
                inputSampleL *= outputchase;
                inputSampleR *= outputchase;
            }
            if (wetchase < 1.0) {
                inputSampleL = (inputSampleL * wetchase) + (drySampleL * (1.0 - fabs(wetchase)));
                inputSampleR = (inputSampleR * wetchase) + (drySampleR * (1.0 - fabs(wetchase)));
                // inv/dry/wet lets us turn LP into HP and band into notch
            }
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
} // namespace airwindohhs::biquad2
