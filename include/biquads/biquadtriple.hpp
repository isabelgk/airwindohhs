#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquadtriple {

constexpr std::string_view k_name{ "BiquadTriple" };
constexpr std::string_view k_short_description{
    "BiquadTriple is a handy Airwindows cascaded biquad filter: steeper roll-off before resonance."
};
constexpr std::string_view k_long_description{
    "Here’s another utility plugin! This is the same as BiquadDouble, except it’s three stacked biquads. Otherwise the same: you get to set the frequency, Q etc. for all the filters in parallel. This saves time over doing three instances of Biquad, plus they’re run in series inside Console inside the plugin, so there will be a deeper tone out of this one.It’s for if you wanted to get steeper filter slopes without the cutoff going resonant. This, like BiquadDouble, is very much ‘tools to make tools’, for if you’re mocking up some kind of effect and know you’re going to be using filters. You can test out what you’ll use in your effect, this way. Note: seems like the inv/dry/wet is really just wet/dry/wet, and I have too many new and more worthwhile plugins to put out to fuss over it so I’d advise just taking BiquadTriple as it is. I’d be using it as full-wet anyway though you can bleed in dry for more of a shelf effect if you’re doing a steep rolloff of some kind.I expect to be doing some more variations on this as there are known ways to stack filters where the Q factors are very specific and staggered: however, those will be more aimed at the mixer/end user, and probably will be called isolators (that being a common term for this type of steep crossover-like filter). And again, I’ve got other things to put out that are more interesting, so more will be revealed as I do those experiments. I think an Airwindows isolator-style effect would be good. Bear in mind the biquads can be fussy about extreme settings and moving the sliders abruptly."
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class BiquadTriple final : public Effect<T>
{
    double biquadA[11];
    double biquadB[11];
    double biquadC[11]; // note that this stereo form doesn't require L and R forms!
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
    BiquadTriple()
    {
        for (int x = 0; x < 11; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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
            case kParamD: return "inv/wet"; break;

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
            case kParamD: return "Inv/Wet"; break;

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
        biquadA[0] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquadA[0] < 0.0001) {
            biquadA[0] = 0.0001;
        }
        biquadA[1] = (C * C * C * 29.99) + 0.01;
        if (biquadA[1] < 0.0001) {
            biquadA[1] = 0.0001;
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
            double K = tan(M_PI * biquadA[0]);
            double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
            biquadA[2] = K * K * norm;
            biquadA[3] = 2.0 * biquadA[2];
            biquadA[4] = biquadA[2];
            biquadA[5] = 2.0 * (K * K - 1.0) * norm;
            biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquadA[0]);
            double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
            biquadA[2] = norm;
            biquadA[3] = -2.0 * biquadA[2];
            biquadA[4] = biquadA[2];
            biquadA[5] = 2.0 * (K * K - 1.0) * norm;
            biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquadA[0]);
            double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
            biquadA[2] = K / biquadA[1] * norm;
            biquadA[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquadA[4] = -biquadA[2];
            biquadA[5] = 2.0 * (K * K - 1.0) * norm;
            biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquadA[0]);
            double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
            biquadA[2] = (1.0 + K * K) * norm;
            biquadA[3] = 2.0 * (K * K - 1) * norm;
            biquadA[4] = biquadA[2];
            biquadA[5] = biquadA[3];
            biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        }
        for (int x = 0; x < 7; x++) {
            biquadC[x] = biquadB[x] = biquadA[x];
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
            double tempSampleL = (inputSampleL * biquadA[2]) + biquadA[7];
            biquadA[7] = (inputSampleL * biquadA[3]) - (tempSampleL * biquadA[5]) + biquadA[8];
            biquadA[8] = (inputSampleL * biquadA[4]) - (tempSampleL * biquadA[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquadA[2]) + biquadA[9];
            biquadA[9] = (inputSampleR * biquadA[3]) - (tempSampleR * biquadA[5]) + biquadA[10];
            biquadA[10] = (inputSampleR * biquadA[4]) - (tempSampleR * biquadA[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
            tempSampleL = (inputSampleL * biquadB[2]) + biquadB[7];
            biquadB[7] = (inputSampleL * biquadB[3]) - (tempSampleL * biquadB[5]) + biquadB[8];
            biquadB[8] = (inputSampleL * biquadB[4]) - (tempSampleL * biquadB[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadB[2]) + biquadB[9];
            biquadB[9] = (inputSampleR * biquadB[3]) - (tempSampleR * biquadB[5]) + biquadB[10];
            biquadB[10] = (inputSampleR * biquadB[4]) - (tempSampleR * biquadB[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
            tempSampleL = (inputSampleL * biquadC[2]) + biquadC[7];
            biquadC[7] = (inputSampleL * biquadC[3]) - (tempSampleL * biquadC[5]) + biquadC[8];
            biquadC[8] = (inputSampleL * biquadC[4]) - (tempSampleL * biquadC[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            tempSampleR = (inputSampleR * biquadC[2]) + biquadC[9];
            biquadC[9] = (inputSampleR * biquadC[3]) - (tempSampleR * biquadC[5]) + biquadC[10];
            biquadC[10] = (inputSampleR * biquadC[4]) - (tempSampleR * biquadC[6]);
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
} // namespace airwindohhs::biquadtriple
