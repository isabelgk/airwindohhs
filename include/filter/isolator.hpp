#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::isolator {

constexpr std::string_view k_name{ "Isolator" };
constexpr std::string_view k_short_description{
    "Isolator is a steep highpass or lowpass filter, like you might find in a speaker crossover."
};
constexpr std::string_view k_long_description{
    "Isolator is like some of my Biquad Filter plugins, except it’s tuned to a particular purpose. In DJing, one technique is to use a filter on the full mix for effect, and Isolator is geared towards two specific tricks in that vein: sweeping the filter down until the music is just a throb and then bringing it back, or sweeping the filter up to leave only highs and then dropping the bass back in.This is done using a speaker-crossover style filter: Isolator is a fifth order Butterworth filter made out of three series biquad filters run inside an instance of Airwindows Console. It rolls off at 30 dB/octave (I think I had this as 36db/octave in the video, but that would be a sixth-order filter) and probably slightly steeper than that, because I’m up to my usual tricks and chose the fifth order Butterworth because it’s made out of Q factors that are a close approximation of the golden ratio. So, I’ve used that, making the filter JUST a bit steeper than normal :D also, the way the frequency control is mapped, means it gives you accurate control over the full Nyquist range of the filter AND still puts the midrange in the middle of the control, through changing the logarithm of the filter frequency. That means it always is able to go full-range no matter what sample rate you’re at: maxed out, is always ‘max sample rate’, it just sweeps farther up when run at higher sample rates once you’re at the far right of the control.That’s the tech rundown. In practice, this is a lowpass/highpass filter. You can sweep it around, though it’s possible to freak it out if you throw a lot of filter-sweep activity at it in a zone where it’s handling a lot of energy. It wouldn’t be good to try and FM this control: biquads sound wonderfully smooth and rich but they won’t handle that treatment, especially run inside their own version of Console. You can use the treble and bass controls as a form of shelf, or output gain: note that I’ve set them up for instant response so you CAN FM them or do really aggressive, choppy things with them without upsetting the filter. That said, if you have a heavily bassy sound coming through them, you can get clicks since they will respond instantly: it’s that or have them chase the value, and you can’t do aggressive glitchy stuff if you chase the value, so I’ve got them responding directly. That does mean you can do stuff like have the lows coming through and then ‘flicker’ in the highs setting for a glitchy effect, at whatever speed you can automate the slider.I expect mostly this will be useful for the classic ‘now it’s all bass… and now the full mix gradually swells up’ effect, but there are many possible uses. You can use it to neatly lop off highs or lows with the steepest cutoff possible that isn’t QUITE resonant. You can cut the bass on tracks with great accuracy, and you can use it as a shelf of great accuracy: the bands are made by subtracting from the lowpassed version, so having both treble and bass full up means you have literally unaltered digital audio, apart from some ‘zero dB’ multiplications. That means very subtle cuts in highs OR lows might be using a sharp near-resonant filter sound, but the less cut you have the cleaner the output will be. It’s a mastering-grade surgical high/low shelf, much like professional DJ isolators must be constructed to carry the full mix with ideal sound quality.Except I can beat those in one very Airwindows detail: if you are using it as a lowpass, and you’ve got high on zero and low on 1 and you open up the filter completely sweeping it up… no matter what sample rate you’re at, once you reach 1 (full crank) on the frequency control, it seamlessly bypasses itself while still calculating the filtered signal. So, unlike analog isolators, when you run THIS isolator on the full mix, at times you’re not using it, it is literally not in the signal path anymore. :D"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Isolator final : public Effect<T>
{
    double biquadA[15];
    double biquadB[15];
    double biquadC[15]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8-9-10 and 11-12-13-14, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Isolator()
    {
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
        }
        A = 1.0;
        B = 0.0;
        C = 1.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
            return 0.0;
            break;
        kParamC:
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
            return "freq";
            break;
        kParamB:
            return "high";
            break;
        kParamC:
            return "low";
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
            return "Freq";
            break;
        kParamB:
            return "High";
            break;
        kParamC:
            return "Low";
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
        bool bypass = (A == 1.0);
        double high = B;
        double low = C; // this gives us shelving, and
        // the ability to use the isolator as a highpass
        if (high > 0.0) {
            bypass = false;
        }
        if (low < 1.0) {
            bypass = false;
        }
        biquadA[0] = pow(A, (2.0 * sqrt(overallscale))) * 0.4999;
        if (biquadA[0] < 0.001) {
            biquadA[0] = 0.001;
        }
        biquadC[0] = biquadB[0] = biquadA[0];
        biquadA[1] = 0.5;
        biquadB[1] = 0.618033988749894848204586;
        biquadC[1] = 1.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / biquadC[1] + K * K);
        biquadC[2] = K * K * norm;
        biquadC[3] = 2.0 * biquadC[2];
        biquadC[4] = biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / biquadC[1] + K * K) * norm;
        // there is a form for highpass
        // but I would suggest subtracting the lowpass from dry
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
            double drySampleL = *in1;
            double drySampleR = *in2;
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            double outSampleL = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
            biquadA[8] = biquadA[7];
            biquadA[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadA[10] = biquadA[9];
            biquadA[9] = inputSampleL; // DF1 left
            outSampleL = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
            biquadB[8] = biquadB[7];
            biquadB[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadB[10] = biquadB[9];
            biquadB[9] = inputSampleL; // DF1 left
            outSampleL = biquadC[2] * inputSampleL + biquadC[3] * biquadC[7] + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
            biquadC[8] = biquadC[7];
            biquadC[7] = inputSampleL;
            inputSampleL = outSampleL;
            biquadC[10] = biquadC[9];
            biquadC[9] = inputSampleL; // DF1 left
            double outSampleR = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
            biquadA[12] = biquadA[11];
            biquadA[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadA[14] = biquadA[13];
            biquadA[13] = inputSampleR; // DF1 right
            outSampleR = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
            biquadB[12] = biquadB[11];
            biquadB[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadB[14] = biquadB[13];
            biquadB[13] = inputSampleR; // DF1 right
            outSampleR = biquadC[2] * inputSampleR + biquadC[3] * biquadC[11] + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
            biquadC[12] = biquadC[11];
            biquadC[11] = inputSampleR;
            inputSampleR = outSampleR;
            biquadC[14] = biquadC[13];
            biquadC[13] = inputSampleR; // DF1 right
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
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            if (bypass) {
                inputSampleL = drySampleL;
                inputSampleR = drySampleR;
            }
            else {
                inputSampleL = (inputSampleL * low) + ((drySampleL - inputSampleL) * high);
                inputSampleR = (inputSampleR * low) + ((drySampleR - inputSampleR) * high);
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
} // namespace airwindohhs::isolator
