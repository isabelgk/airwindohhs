#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquadnonlin {

constexpr std::string_view k_name{ "BiquadNonLin" };
constexpr std::string_view k_short_description{
    "BiquadNonLin is Capacitor2, but for biquad filtering."
};
constexpr std::string_view k_long_description{
    "A biquad filter is a very basic piece of typical DAW EQs. If you were going to think of the most ordinary, normal, non-special digital EQ that would sound like every other digital EQ, you're probably going to be stacking up biquad filters. It is the most boring of digital filters, at least ones that are kind of flexible.Let's break it!BiquadNonLin takes the traditional biquad filter, complete with cutoff and Q factor, and applies the same technique used in Capacitor2. That was the one where I read about Murata ceramic capacitors showing striking variations in value, based upon how much voltage pressure was against them. Which is not to say all capacitors do that… but knowing this has happened in the real world suggests that it's an avenue to pursue. And so, Capacitor2 brought in this interesting nonlinearity based on real-world stuff, like a (in fact literally a) frequency modulation… and then it sat for a while. Because how would you even apply that to a carefully worked out algorithm like a biquad?The answer is 'you just do'. Does it produce a clean result? Nope. You can break things pretty thoroughly, especially up near the Nyquist frequency. But you can apply less… I find 0.5 tends to work quite well… and you can get the same kind of nonlinearity.It's just that this time, you can do it on a steep, resonant filter, and get that more analog texture on something that's otherwise very controllable. I'm looking to use this on many things: I think it'll make for a better-sounding distributed ultrasonic filter, and I'd also like to make a multistage version to use as the guts of a future ConsoleSL… because it should work more like a steeper but less vibey filter, except with the analogification.Anyway, here's BiquadNonLin: one biquad filter, lightly broken for your sonic pleasure. Type at 0 is a lowpass, Type at 1 is a bandpass. The biquad tended to explode when I tried to make it do highpasses or notches, but it turned out I could just make the dry/wet an Inv/Wet control, so that means you just use Inv instead, and Type at 0 becomes the highpass (it's subtracted) and Type at 1 is the notch. If you have a hard time hearing the nonlinearity, setting it to highpass and sweeping it up at high NonLin should make it real obvious.Oh, one more thing…Today, the first VST3 is out… kind of. I have it up on GitHub at https://github.com/airwindows/Meter and you see it in the video for BiquadDouble. The thing is, I can't build it anywhere else except a single VST3 on my own machine, for Mac Apple Silicon only. The project on GitHub is all the code, up to date, and it's supposed to be able to build all sorts of different versions.You can fork it, you can download it, you can try to get it to compile just like I did. There's lots of people smarter than me about computers and they didn't make this meter, so I had to. So, if you can make a Pamplejuce project compile, Meter is yours today, no matter what sort of computer you're on or what platform you're targeting. VST3? AU for iPad? AAX? Knock yourself out.If you get anything to work, tell me how you did it, and everyone can have the result. Otherwise, I will keep plugging away, and eventually I'll get this stuff sorted out. But if you're impatient… the GPLed code is there, it compiled for me, let me know how you do :)"
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class BiquadNonLin final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        biq_freq,
        biq_reso,
        biq_a0,
        biq_a1,
        biq_a2,
        biq_b1,
        biq_b2,
        biq_aA0,
        biq_aA1,
        biq_aA2,
        biq_bA1,
        biq_bA2,
        biq_aB0,
        biq_aB1,
        biq_aB2,
        biq_bB1,
        biq_bB2,
        biq_sL1,
        biq_sL2,
        biq_sR1,
        biq_sR2,
        biq_total
    };
    double biquad[biq_total];
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    BiquadNonLin()
    {
        A = 0.0;
        B = 0.5;
        C = 0.7071;
        D = 0.0;
        E = 1.0;
        for (int x = 0; x < biq_total; x++) {
            biquad[x] = 0.0;
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
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;
            case kParamB: return 0.5;
            case kParamC: return 0.7071;
            case kParamD: return 0.0;
            case kParamE: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "type";
            case kParamB: return "freq";
            case kParamC: return "q";
            case kParamD: return "nonlin";
            case kParamE: return "invwet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Type";
            case kParamB: return "Freq";
            case kParamC: return "Q";
            case kParamD: return "NonLin";
            case kParamE: return "Inv/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string((B * B * B * 0.9999) + 0.0001);
            case kParamC: return std::to_string((C * C * C * 29.99) + 0.01);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string((E * 2.0) - 1.0);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
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
        overallscale *= getSampleRate();
        int type = 0;
        if (A > 0.5) {
            type = 1;
        }
        biquad[biq_freq] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquad[biq_freq] < 0.0001) {
            biquad[biq_freq] = 0.0001;
        }
        biquad[biq_reso] = (C * C * C * 29.99) + 0.01;
        if (biquad[biq_reso] < 0.0001) {
            biquad[biq_reso] = 0.0001;
        }
        double nonLin = pow(D, 2);
        // if you're using a 0.5 for a lowpass fixed frequency, value is 0.25
        double wet = (E * 2.0) - 1.0;
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
        biquad[biq_aA0] = biquad[biq_aB0];
        biquad[biq_aA1] = biquad[biq_aB1];
        biquad[biq_aA2] = biquad[biq_aB2];
        biquad[biq_bA1] = biquad[biq_bB1];
        biquad[biq_bA2] = biquad[biq_bB2];
        // previous run through the buffer is still in the filter, so we move it
        // to the A section and now it's the new starting point.
        if (type == 0) { // lowpass
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = K * K * norm;
            biquad[biq_aB1] = 2.0 * biquad[biq_aB0];
            biquad[biq_aB2] = biquad[biq_aB0];
            biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (type == 1) { // bandpass
            double K = tan(M_PI * biquad[biq_freq]);
            double norm = 1.0 / (1.0 + K / biquad[biq_reso] + K * K);
            biquad[biq_aB0] = K / biquad[biq_reso] * norm;
            biquad[biq_aB1] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquad[biq_aB2] = -biquad[biq_aB0];
            biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
            biquad[biq_bB2] = (1.0 - K / biquad[biq_reso] + K * K) * norm;
        }
        if (biquad[biq_aA0] == 0.0) { // if we have just started, start directly with raw info
            biquad[biq_aA0] = biquad[biq_aB0];
            biquad[biq_aA1] = biquad[biq_aB1];
            biquad[biq_aA2] = biquad[biq_aB2];
            biquad[biq_bA1] = biquad[biq_bB1];
            biquad[biq_bA2] = biquad[biq_bB2];
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
            double buf = (double)sampleFrames / inFramesToProcess;
            biquad[biq_a0] = (biquad[biq_aA0] * buf) + (biquad[biq_aB0] * (1.0 - buf));
            biquad[biq_a1] = (biquad[biq_aA1] * buf) + (biquad[biq_aB1] * (1.0 - buf));
            biquad[biq_a2] = (biquad[biq_aA2] * buf) + (biquad[biq_aB2] * (1.0 - buf));
            biquad[biq_b1] = (biquad[biq_bA1] * buf) + (biquad[biq_bB1] * (1.0 - buf));
            biquad[biq_b2] = (biquad[biq_bA2] * buf) + (biquad[biq_bB2] * (1.0 - buf));
            double dia0 = fabs(biquad[biq_a0] * (1.0 + (inputSampleL * nonLin)));
            if (dia0 > 1.0) {
                dia0 = 1.0;
            }
            double dia2 = dia0; // if lowpass, use this in both places
            if (type == 1) {
                dia2 = -dia2;
            }
            // if bandpass, you must reverse polarity
            double tempSample = (inputSampleL * dia0) + biquad[biq_sL1];
            biquad[biq_sL1] = (inputSampleL * biquad[biq_a1]) - (tempSample * biquad[biq_b1]) + biquad[biq_sL2];
            biquad[biq_sL2] = (inputSampleL * dia2) - (tempSample * biquad[biq_b2]);
            inputSampleL = tempSample;
            inputSampleL *= wet;
            if (wet > 0.0) {
                inputSampleL += (drySampleL * (1.0 - wet));
            }
            else {
                inputSampleL += drySampleL;
            } // inv/dry/wet lets us turn LP into HP and band into notch
            dia0 = fabs(biquad[biq_a0] * (1.0 + (inputSampleR * nonLin)));
            if (dia0 > 1.0) {
                dia0 = 1.0;
            }
            dia2 = dia0; // if lowpass, use this in both places
            if (type == 1) {
                dia2 = -dia2;
            }
            // if bandpass, you must reverse polarity
            tempSample = (inputSampleR * dia0) + biquad[biq_sR1];
            biquad[biq_sR1] = (inputSampleR * biquad[biq_a1]) - (tempSample * biquad[biq_b1]) + biquad[biq_sR2];
            biquad[biq_sR2] = (inputSampleR * dia2) - (tempSample * biquad[biq_b2]);
            inputSampleR = tempSample;
            inputSampleR *= wet;
            if (wet > 0.0) {
                inputSampleR += (drySampleR * (1.0 - wet));
            }
            else {
                inputSampleR += drySampleR;
            } // inv/dry/wet lets us turn RP into HP and band into notch
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
} // namespace airwindohhs::biquadnonlin
