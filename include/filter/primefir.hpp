#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
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
    static constexpr int prime[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751, 4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791, 5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723, 7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919 };
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'prif';
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
