#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquad {

constexpr std::string_view k_name{ "Biquad" };
constexpr std::string_view k_short_description{
    "Biquad is the Airwindows implementation of a biquad filter."
};
constexpr std::string_view k_long_description{
    "This little puppy is really flexible!That’s true of any biquad filter, but I find most of them are really flexible and slightly plastic. Traditionally biquads require special handling, because you can’t push them too far before they break, and also they sound ‘DAW-y’ and synthetic. Not in the same way as linear-phase EQs, mind you, but still with something about them that feels digital.So I fixed that :) with a couple caveats. Firstly, part of it is brute force. I figured that if biquads run into trouble with 32-bit coefficients on some filter curves, and it’s widely known that you should do things like run ’em in series to get better behavior, we could always run long doubles and be totally sure they work as intended. Overkill? Meh. Sure, but it’s known that these break due to losing control of themselves due to not long enough wordlengths. Why wouldn’t they work and sound better with long enough wordlengths? And so they do. Also, you get to run simpler biquads if you don’t have to run lots of them, so it averages out. I think it’s just a matter of audio DSP coders stubbornly insisting on not using long data words on the grounds it doesn’t matter to the sound. This, when the filters go obviously wrong under some conditions for just that reason? Anyway, here’s long double biquad, because nobody else was doing it far as I know.The second part is trickier, because if you use it you remove some functionality. Biquads make great DC blockers. But, biquads also work through the summing of many delay taps, all very close together. Some are on input, others on output. They’re a mixing of multiple copies of the signal.That means Console applies to the biquad filter.We already knew that, of course: I’ve long observed that you can put Console (or BussColors4/ConsoleBuss4) around a bog-standard DAW EQ and get an enhanced sound out of it. Mind you, I wasn’t controlling that EQ, but it still worked. But now, I have Console5 (not 6, but I COULD do 6 in the right context) built right into Biquad. So, it does expect to have a signal between -1 and 1 (or lower than 0dBFS in the DAW), and that means Biquad itself can’t remove huge DC offsets ten times the size of the audio content. (It can still remove smaller offsets just fine). And it runs into basically PurestConsoleChannel, does the biquad, then uses PurestConsoleBuss and goes out to an inverse/dry/wet control (which itself has multiple uses).What’s a biquad, you ask? The Airwindows biquad uses four of the six common biquad options (I prefer to do shelves with subtractive/additive use of filtered audio). The top control has settings one through four. They go lowpass, highpass, bandpass and notch. That’s what I consider the platonic set of biquads, and it lets the filter design be simple and clear. You can set a frequency (in ‘amount of the audio range’) from ‘zero’ to ‘one’. There’ll be EQs with more specific frequencies, that’s easy, but this is proof of concept and to be used by ear, plus it will always cover DC to Nyquist at any sample rate. Zero is not really DC, because that kills the biquad: also, One is not really the Nyquist frequency (half the sample rate) for the same reason. But, they act a lot like they range that far, because the high resolution lets Biquad calculate things accurately even that far out of the normal range.They don’t update/recalculate every sample, but the way I’ve defined the data structures means they could if you wanted them to (at a cost in higher CPU-eating). The code’s MIT licensed open source, so GPL people can just take it if they credit me, and all the projects that are using the Airwindows library are advised to get up to date and include this one. Set right, it is THE ultimate sample-instrument tone shaper to sit ‘under the hood’ and voice somebody’s musical product after the sample-playback stuff is taken care of. You’ll be seeing a lot of stuff come out that uses this code, as there are many plugins that require this type of filter to work, often ones that will benefit from the sonic improvements that are part of Biquad.More will be revealed. Suffice to say this is a very useful building block that’ll allow for some very special plugins."
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class Biquad final : public Effect<T>
{
    double biquad[11]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8 and 9-10, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    Biquad()
    {
        for (int x = 0; x < 11; x++) {
            biquad[x] = 0.0;
        }
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 1.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 1.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "type";
            break;
        kParamB:
            return "freq";
            break;
        kParamC:
            return "q";
            break;
        kParamD:
            return "inv/wet";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Type";
            break;
        kParamB:
            return "Freq";
            break;
        kParamC:
            return "Q";
            break;
        kParamD:
            return "Inv/Wet";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;

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
        overallscale *= getSampleRate();
        int type = ceil((A * 3.999) + 0.00001);
        biquad[0] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquad[0] < 0.0001) {
            biquad[0] = 0.0001;
        }
        biquad[1] = (C * C * C * 29.99) + 0.01;
        if (biquad[1] < 0.0001) {
            biquad[1] = 0.0001;
        }
        double wet = (D * 2.0) - 1.0;
        // biquad contains these values:
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        //[2] is a0 but you need distinct ones for additional biquad instances so it's here
        //[3] is a1 but you need distinct ones for additional biquad instances so it's here
        //[4] is a2 but you need distinct ones for additional biquad instances so it's here
        //[5] is b1 but you need distinct ones for additional biquad instances so it's here
        //[6] is b2 but you need distinct ones for additional biquad instances so it's here
        //[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
        //[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
        // to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
        // put this code either within the sample buffer (for smoothly modulating freq or res)
        // or in this 'read the controls' area (for letting you change freq and res with controls)
        // or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
        if (type == 1) { // lowpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K * K * norm;
            biquad[3] = 2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = norm;
            biquad[3] = -2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K / biquad[1] * norm;
            biquad[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquad[4] = -biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = (1.0 + K * K) * norm;
            biquad[3] = 2.0 * (K * K - 1) * norm;
            biquad[4] = biquad[2];
            biquad[5] = biquad[3];
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            /*
             double mid = inputSampleL + inputSampleR;
             double side = inputSampleL - inputSampleR;
             //assign mid and side.Between these sections, you can do mid/side processing
             double tempSampleM = (mid * biquad[2]) + biquad[7];
             biquad[7] = (mid * biquad[3]) - (tempSampleM * biquad[5]) + biquad[8];
             biquad[8] = (mid * biquad[4]) - (tempSampleM * biquad[6]);
             mid = tempSampleM; //like mono AU, 7 and 8 store mid channel
             double tempSampleS = (side * biquad[2]) + biquad[9];
             biquad[9] = (side * biquad[3]) - (tempSampleS * biquad[5]) + biquad[10];
             biquad[10] = (side * biquad[4]) - (tempSampleS * biquad[6]);
             inputSampleR = tempSampleS; //note: 9 and 10 store the side channel
             inputSampleL = (mid+side)/2.0;
             inputSampleR = (mid-side)/2.0;
             //unassign mid and side
             */
            double tempSampleL = (inputSampleL * biquad[2]) + biquad[7];
            biquad[7] = (inputSampleL * biquad[3]) - (tempSampleL * biquad[5]) + biquad[8];
            biquad[8] = (inputSampleL * biquad[4]) - (tempSampleL * biquad[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquad[2]) + biquad[9];
            biquad[9] = (inputSampleR * biquad[3]) - (tempSampleR * biquad[5]) + biquad[10];
            biquad[10] = (inputSampleR * biquad[4]) - (tempSampleR * biquad[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
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
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
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
} // namespace airwindohhs::biquad
