#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::xregion {

constexpr std::string_view k_name{ "XRegion" };
constexpr std::string_view k_short_description{
    "XRegion is distorted staggered bandpasses, for extreme soundmangling."
};
constexpr std::string_view k_long_description{
    "XNotch too normal for you? HERE YA GO :DXRegion is made out of bandpasses, like XBandpass, and if you keep the First and Last controls set the same it does act like an increasingly manic bandpass.But, if they’re set differently and the Nuke control is up (engaging more poles of filtering) here’s what happens: first we go into the First bandpass, distorting before we filter (turns out if we make these filters glitch out the result is just too predictable) and then we go through each successive bandpass (up to five) each of which goes a step farther towards the frequency setting that’s in Last. We’re covering a region, we’re spreading the response out.But we’re also distorting, each time.So, if First is a higher frequency than Last, we get progressively lower pitched bandpasses and a sort of thick, roaring, dense tone. But if Last is a higher pitch than First… we’re starting with a bassy distort, and then filtering out the SOUND and keeping only the DISTORTIONS. Oh, and the farther apart you spread the controls the more gain it uses.So basically this is raw industrial mayhem. It’s so bonkers you can use it inside uLaw and the result won’t even be crazier. In theory you can use this to get a really intense bandpass effect, for instance distorting a snare or something, and carefully control the gain and ‘Nuke’ (less of that means less bandpasses, and it won’t go all the way towards ‘Last’ anymore). But you can also just go nuts with it for some filter-sweepy, very distorted effects that won’t be like anything you’ve heard. I suspect the ‘nice’ uses of this will be much more limited, though in theory it should be as good at those as XBandpass is (to get a nice smooth distort, don’t spread First and Last too wide, or set them too high or too low, and balance the result with Dry/Wet)There are a lot of people who won’t need this… at ALL. For those who do… hope you like it :)As an aside, XRegion can do a really killer overdriven bass amp kind of a tone."
};
constexpr std::string_view k_tags{
    "xyz-filters"
};

template <typename T>
class XRegion final : public Effect<T>
{
    double biquad[15];
    double biquadA[15];
    double biquadB[15];
    double biquadC[15];
    double biquadD[15];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    XRegion()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        E = 1.0;
        for (int x = 0; x < 15; x++) {
            biquad[x] = 0.0;
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "gain"; break;
            case kParamB: return "first"; break;
            case kParamC: return "last"; break;
            case kParamD: return "nuke"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Gain"; break;
            case kParamB: return "First"; break;
            case kParamC: return "Last"; break;
            case kParamD: return "Nuke"; break;
            case kParamE: return "Dry/Wet"; break;

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

        double gain = pow(A + 0.5, 4);
        double high = B;
        double low = C;
        double mid = (high + low) * 0.5;
        double spread = 1.001 - fabs(high - low);
        biquad[0] = (pow(high, 3) * 20000.0) / Effect<T>::getSampleRate();
        if (biquad[0] < 0.00009) {
            biquad[0] = 0.00009;
        }
        double compensation = sqrt(biquad[0]) * 6.4 * spread;
        double clipFactor = 0.75 + (biquad[0] * D * 37.0);
        biquadA[0] = (pow((high + mid) * 0.5, 3) * 20000.0) / Effect<T>::getSampleRate();
        if (biquadA[0] < 0.00009) {
            biquadA[0] = 0.00009;
        }
        double compensationA = sqrt(biquadA[0]) * 6.4 * spread;
        double clipFactorA = 0.75 + (biquadA[0] * D * 37.0);
        biquadB[0] = (pow(mid, 3) * 20000.0) / Effect<T>::getSampleRate();
        if (biquadB[0] < 0.00009) {
            biquadB[0] = 0.00009;
        }
        double compensationB = sqrt(biquadB[0]) * 6.4 * spread;
        double clipFactorB = 0.75 + (biquadB[0] * D * 37.0);
        biquadC[0] = (pow((mid + low) * 0.5, 3) * 20000.0) / Effect<T>::getSampleRate();
        if (biquadC[0] < 0.00009) {
            biquadC[0] = 0.00009;
        }
        double compensationC = sqrt(biquadC[0]) * 6.4 * spread;
        double clipFactorC = 0.75 + (biquadC[0] * D * 37.0);
        biquadD[0] = (pow(low, 3) * 20000.0) / Effect<T>::getSampleRate();
        if (biquadD[0] < 0.00009) {
            biquadD[0] = 0.00009;
        }
        double compensationD = sqrt(biquadD[0]) * 6.4 * spread;
        double clipFactorD = 0.75 + (biquadD[0] * D * 37.0);
        double K = tan(M_PI * biquad[0]);
        double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquad[2] = K / 0.7071 * norm;
        biquad[4] = -biquad[2];
        biquad[5] = 2.0 * (K * K - 1.0) * norm;
        biquad[6] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadA[0]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadA[2] = K / 0.7071 * norm;
        biquadA[4] = -biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadB[0]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadB[2] = K / 0.7071 * norm;
        biquadB[4] = -biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadC[0]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadC[2] = K / 0.7071 * norm;
        biquadC[4] = -biquadC[2];
        biquadC[5] = 2.0 * (K * K - 1.0) * norm;
        biquadC[6] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadD[0]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadD[2] = K / 0.7071 * norm;
        biquadD[4] = -biquadD[2];
        biquadD[5] = 2.0 * (K * K - 1.0) * norm;
        biquadD[6] = (1.0 - K / 0.7071 + K * K) * norm;
        double aWet = 1.0;
        double bWet = 1.0;
        double cWet = 1.0;
        double dWet = D * 4.0;
        double wet = E;
        // four-stage wet/dry control using progressive stages that bypass when not engaged
        if (dWet < 1.0) {
            aWet = dWet;
            bWet = 0.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 2.0) {
            bWet = dWet - 1.0;
            cWet = 0.0;
            dWet = 0.0;
        }
        else if (dWet < 3.0) {
            cWet = dWet - 2.0;
            dWet = 0.0;
        }
        else {
            dWet -= 3.0;
        }
        // this is one way to make a little set of dry/wet stages that are successively added to the
        // output as the control is turned up. Each one independently goes from 0-1 and stays at 1
        // beyond that point: this is a way to progressively add a 'black box' sound processing
        // which lets you fall through to simpler processing at lower settings.
        double outSample = 0.0;
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
            if (gain != 1.0) {
                inputSampleL *= gain;
                inputSampleR *= gain;
            }
            double nukeLevelL = inputSampleL;
            double nukeLevelR = inputSampleR;
            inputSampleL *= clipFactor;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            outSample = biquad[2] * inputSampleL + biquad[4] * biquad[8] - biquad[5] * biquad[9] - biquad[6] * biquad[10];
            biquad[8] = biquad[7];
            biquad[7] = inputSampleL;
            biquad[10] = biquad[9];
            biquad[9] = outSample; // DF1 left
            inputSampleL = outSample / compensation;
            nukeLevelL = inputSampleL;
            inputSampleR *= clipFactor;
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            outSample = biquad[2] * inputSampleR + biquad[4] * biquad[12] - biquad[5] * biquad[13] - biquad[6] * biquad[14];
            biquad[12] = biquad[11];
            biquad[11] = inputSampleR;
            biquad[14] = biquad[13];
            biquad[13] = outSample; // DF1 right
            inputSampleR = outSample / compensation;
            nukeLevelR = inputSampleR;
            if (aWet > 0.0) {
                inputSampleL *= clipFactorA;
                if (inputSampleL > 1.57079633) {
                    inputSampleL = 1.57079633;
                }
                if (inputSampleL < -1.57079633) {
                    inputSampleL = -1.57079633;
                }
                inputSampleL = sin(inputSampleL);
                outSample = biquadA[2] * inputSampleL + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
                biquadA[8] = biquadA[7];
                biquadA[7] = inputSampleL;
                biquadA[10] = biquadA[9];
                biquadA[9] = outSample; // DF1 left
                inputSampleL = outSample / compensationA;
                inputSampleL = (inputSampleL * aWet) + (nukeLevelL * (1.0 - aWet));
                nukeLevelL = inputSampleL;
                inputSampleR *= clipFactorA;
                if (inputSampleR > 1.57079633) {
                    inputSampleR = 1.57079633;
                }
                if (inputSampleR < -1.57079633) {
                    inputSampleR = -1.57079633;
                }
                inputSampleR = sin(inputSampleR);
                outSample = biquadA[2] * inputSampleR + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
                biquadA[12] = biquadA[11];
                biquadA[11] = inputSampleR;
                biquadA[14] = biquadA[13];
                biquadA[13] = outSample; // DF1 right
                inputSampleR = outSample / compensationA;
                inputSampleR = (inputSampleR * aWet) + (nukeLevelR * (1.0 - aWet));
                nukeLevelR = inputSampleR;
            }
            if (bWet > 0.0) {
                inputSampleL *= clipFactorB;
                if (inputSampleL > 1.57079633) {
                    inputSampleL = 1.57079633;
                }
                if (inputSampleL < -1.57079633) {
                    inputSampleL = -1.57079633;
                }
                inputSampleL = sin(inputSampleL);
                outSample = biquadB[2] * inputSampleL + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
                biquadB[8] = biquadB[7];
                biquadB[7] = inputSampleL;
                biquadB[10] = biquadB[9];
                biquadB[9] = outSample; // DF1 left
                inputSampleL = outSample / compensationB;
                inputSampleL = (inputSampleL * bWet) + (nukeLevelL * (1.0 - bWet));
                nukeLevelL = inputSampleL;
                inputSampleR *= clipFactorB;
                if (inputSampleR > 1.57079633) {
                    inputSampleR = 1.57079633;
                }
                if (inputSampleR < -1.57079633) {
                    inputSampleR = -1.57079633;
                }
                inputSampleR = sin(inputSampleR);
                outSample = biquadB[2] * inputSampleR + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
                biquadB[12] = biquadB[11];
                biquadB[11] = inputSampleR;
                biquadB[14] = biquadB[13];
                biquadB[13] = outSample; // DF1 right
                inputSampleR = outSample / compensationB;
                inputSampleR = (inputSampleR * bWet) + (nukeLevelR * (1.0 - bWet));
                nukeLevelR = inputSampleR;
            }
            if (cWet > 0.0) {
                inputSampleL *= clipFactorC;
                if (inputSampleL > 1.57079633) {
                    inputSampleL = 1.57079633;
                }
                if (inputSampleL < -1.57079633) {
                    inputSampleL = -1.57079633;
                }
                inputSampleL = sin(inputSampleL);
                outSample = biquadC[2] * inputSampleL + biquadC[4] * biquadC[8] - biquadC[5] * biquadC[9] - biquadC[6] * biquadC[10];
                biquadC[8] = biquadC[7];
                biquadC[7] = inputSampleL;
                biquadC[10] = biquadC[9];
                biquadC[9] = outSample; // DF1 left
                inputSampleL = outSample / compensationC;
                inputSampleL = (inputSampleL * cWet) + (nukeLevelL * (1.0 - cWet));
                nukeLevelL = inputSampleL;
                inputSampleR *= clipFactorC;
                if (inputSampleR > 1.57079633) {
                    inputSampleR = 1.57079633;
                }
                if (inputSampleR < -1.57079633) {
                    inputSampleR = -1.57079633;
                }
                inputSampleR = sin(inputSampleR);
                outSample = biquadC[2] * inputSampleR + biquadC[4] * biquadC[12] - biquadC[5] * biquadC[13] - biquadC[6] * biquadC[14];
                biquadC[12] = biquadC[11];
                biquadC[11] = inputSampleR;
                biquadC[14] = biquadC[13];
                biquadC[13] = outSample; // DF1 right
                inputSampleR = outSample / compensationC;
                inputSampleR = (inputSampleR * cWet) + (nukeLevelR * (1.0 - cWet));
                nukeLevelR = inputSampleR;
            }
            if (dWet > 0.0) {
                inputSampleL *= clipFactorD;
                if (inputSampleL > 1.57079633) {
                    inputSampleL = 1.57079633;
                }
                if (inputSampleL < -1.57079633) {
                    inputSampleL = -1.57079633;
                }
                inputSampleL = sin(inputSampleL);
                outSample = biquadD[2] * inputSampleL + biquadD[4] * biquadD[8] - biquadD[5] * biquadD[9] - biquadD[6] * biquadD[10];
                biquadD[8] = biquadD[7];
                biquadD[7] = inputSampleL;
                biquadD[10] = biquadD[9];
                biquadD[9] = outSample; // DF1 left
                inputSampleL = outSample / compensationD;
                inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));
                nukeLevelL = inputSampleL;
                inputSampleR *= clipFactorD;
                if (inputSampleR > 1.57079633) {
                    inputSampleR = 1.57079633;
                }
                if (inputSampleR < -1.57079633) {
                    inputSampleR = -1.57079633;
                }
                inputSampleR = sin(inputSampleR);
                outSample = biquadD[2] * inputSampleR + biquadD[4] * biquadD[12] - biquadD[5] * biquadD[13] - biquadD[6] * biquadD[14];
                biquadD[12] = biquadD[11];
                biquadD[11] = inputSampleR;
                biquadD[14] = biquadD[13];
                biquadD[13] = outSample; // DF1 right
                inputSampleR = outSample / compensationD;
                inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0 - dWet));
                nukeLevelR = inputSampleR;
            }
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
} // namespace airwindohhs::xregion
