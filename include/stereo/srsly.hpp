#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::srsly {

constexpr std::string_view k_name{ "Srsly" };
constexpr std::string_view k_short_description{
    "Srsly is a psychoacoustic stereo processor."
};
constexpr std::string_view k_long_description{
    "People get really excited sometimes, so here’s a failure :)Srsly is meant to do what a Hughes SRS processor does. That’s an obscure stereo widener actually made by Howard “Spruce Goose” Hughes’ company. I tackled it because a musician I love, Chad Clark, needs one ‘cos his is broken and not in proper working order. Hence, the need for somebody to try and do a plugin version. So I tried, with some pictures out of Popular Mechanics and a single cruddy youtube video to be my guide (this thing is OBSCURE) and I have not succeeded. Whatever the real one sounds like, this ain’t it.But it does a thing, and I’ve added a Q control to help get it to do useful things since I can’t tell what’s right or wrong for this stuff, and this is as far as I can carry it right now. The source is MIT licensed for anyone else who’d like to pick up where I left off, and quite a bit of basic information is shown in the curves of that Popular Mechanics article about the thing, and it is kinda neat: accentuate comb-filtery effects you get from the shapes of your own ears. It does do a thing, that’s not like any other stereo effect I know of. And I ought to be able to use some of the tech from this to do subtle stereo-field enhancements in future versions of Console, stuff like that. You can refine it down to basic concepts and apply those.(followup: this remains useful for more subtle stereo spatiality things)"
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class Srsly final : public Effect<T>
{
    double biquadM2[11];
    double biquadM7[11];
    double biquadM10[11];
    double biquadL3[11];
    double biquadL7[11];
    double biquadR3[11];
    double biquadR7[11];
    double biquadS3[11];
    double biquadS5[11];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;

  public:
    Srsly()
    {
        for (int x = 0; x < 11; x++) {
            biquadM2[x] = 0.0;
            biquadM7[x] = 0.0;
            biquadM10[x] = 0.0;
            biquadL3[x] = 0.0;
            biquadL7[x] = 0.0;
            biquadR3[x] = 0.0;
            biquadR7[x] = 0.0;
            biquadS3[x] = 0.0;
            biquadS5[x] = 0.0;
        }
        A = 0.0;
        B = 0.0;
        C = 1.0;
        D = 0.5;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 1.0; break;
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "center"; break;
            case kParamB: return "space"; break;
            case kParamC: return "level"; break;
            case kParamD: return "q"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Center"; break;
            case kParamB: return "Space"; break;
            case kParamC: return "Level"; break;
            case kParamD: return "Q"; break;
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

        double sampleRate = Effect<T>::getSampleRate();
        if (sampleRate < 22000) {
            sampleRate = 22000; // keep biquads in range
        }
        double tempSample;
        biquadM2[0] = 2000 / sampleRate; // up
        biquadM7[0] = 7000 / sampleRate; // down
        biquadM10[0] = 10000 / sampleRate; // down
        biquadL3[0] = 3000 / sampleRate; // up
        biquadL7[0] = 7000 / sampleRate; // way up
        biquadR3[0] = 3000 / sampleRate; // up
        biquadR7[0] = 7000 / sampleRate; // way up
        biquadS3[0] = 3000 / sampleRate; // up
        biquadS5[0] = 5000 / sampleRate; // way down
        double focusM = 15.0 - (A * 10.0);
        double focusS = 21.0 - (B * 15.0);
        double Q = D + 0.25; // add Q control: from half to double intensity
        biquadM2[1] = focusM * 0.25 * Q; // Q, mid 2K boost is much broader
        biquadM7[1] = focusM * Q; // Q
        biquadM10[1] = focusM * Q; // Q
        biquadS3[1] = focusM * Q; // Q
        biquadS5[1] = focusM * Q; // Q
        biquadL3[1] = focusS * Q; // Q
        biquadL7[1] = focusS * Q; // Q
        biquadR3[1] = focusS * Q; // Q
        biquadR7[1] = focusS * Q; // Q
        double K = tan(M_PI * biquadM2[0]);
        double norm = 1.0 / (1.0 + K / biquadM2[1] + K * K);
        biquadM2[2] = K / biquadM2[1] * norm;
        biquadM2[4] = -biquadM2[2];
        biquadM2[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM2[6] = (1.0 - K / biquadM2[1] + K * K) * norm;
        K = tan(M_PI * biquadM7[0]);
        norm = 1.0 / (1.0 + K / biquadM7[1] + K * K);
        biquadM7[2] = K / biquadM7[1] * norm;
        biquadM7[4] = -biquadM7[2];
        biquadM7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM7[6] = (1.0 - K / biquadM7[1] + K * K) * norm;
        K = tan(M_PI * biquadM10[0]);
        norm = 1.0 / (1.0 + K / biquadM10[1] + K * K);
        biquadM10[2] = K / biquadM10[1] * norm;
        biquadM10[4] = -biquadM10[2];
        biquadM10[5] = 2.0 * (K * K - 1.0) * norm;
        biquadM10[6] = (1.0 - K / biquadM10[1] + K * K) * norm;
        K = tan(M_PI * biquadL3[0]);
        norm = 1.0 / (1.0 + K / biquadL3[1] + K * K);
        biquadL3[2] = K / biquadL3[1] * norm;
        biquadL3[4] = -biquadL3[2];
        biquadL3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadL3[6] = (1.0 - K / biquadL3[1] + K * K) * norm;
        K = tan(M_PI * biquadL7[0]);
        norm = 1.0 / (1.0 + K / biquadL7[1] + K * K);
        biquadL7[2] = K / biquadL7[1] * norm;
        biquadL7[4] = -biquadL7[2];
        biquadL7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadL7[6] = (1.0 - K / biquadL7[1] + K * K) * norm;
        K = tan(M_PI * biquadR3[0]);
        norm = 1.0 / (1.0 + K / biquadR3[1] + K * K);
        biquadR3[2] = K / biquadR3[1] * norm;
        biquadR3[4] = -biquadR3[2];
        biquadR3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadR3[6] = (1.0 - K / biquadR3[1] + K * K) * norm;
        K = tan(M_PI * biquadR7[0]);
        norm = 1.0 / (1.0 + K / biquadR7[1] + K * K);
        biquadR7[2] = K / biquadR7[1] * norm;
        biquadR7[4] = -biquadR7[2];
        biquadR7[5] = 2.0 * (K * K - 1.0) * norm;
        biquadR7[6] = (1.0 - K / biquadR7[1] + K * K) * norm;
        K = tan(M_PI * biquadS3[0]);
        norm = 1.0 / (1.0 + K / biquadS3[1] + K * K);
        biquadS3[2] = K / biquadS3[1] * norm;
        biquadS3[4] = -biquadS3[2];
        biquadS3[5] = 2.0 * (K * K - 1.0) * norm;
        biquadS3[6] = (1.0 - K / biquadS3[1] + K * K) * norm;
        K = tan(M_PI * biquadS5[0]);
        norm = 1.0 / (1.0 + K / biquadS5[1] + K * K);
        biquadS5[2] = K / biquadS5[1] * norm;
        biquadS5[4] = -biquadS5[2];
        biquadS5[5] = 2.0 * (K * K - 1.0) * norm;
        biquadS5[6] = (1.0 - K / biquadS5[1] + K * K) * norm;
        double depthM = pow(A, 2) * 2.0;
        ; // proportion to mix in the filtered stuff
        double depthS = pow(B, 2) * 2.0;
        ; // proportion to mix in the filtered stuff
        double level = C; // output pad
        double wet = E; // dry/wet
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
            double mid = inputSampleL + inputSampleR;
            double rawmid = mid * 0.5; // we'll use this to isolate L&R a little
            double side = inputSampleL - inputSampleR;
            double boostside = side * depthS;
            // assign mid and side.Between these sections, you can do mid/side processing
            tempSample = (mid * biquadM2[2]) + biquadM2[7];
            biquadM2[7] = (-tempSample * biquadM2[5]) + biquadM2[8];
            biquadM2[8] = (mid * biquadM2[4]) - (tempSample * biquadM2[6]);
            double M2Sample = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (mid * biquadM7[2]) + biquadM7[7];
            biquadM7[7] = (-tempSample * biquadM7[5]) + biquadM7[8];
            biquadM7[8] = (mid * biquadM7[4]) - (tempSample * biquadM7[6]);
            double M7Sample = -tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            tempSample = (mid * biquadM10[2]) + biquadM10[7];
            biquadM10[7] = (-tempSample * biquadM10[5]) + biquadM10[8];
            biquadM10[8] = (mid * biquadM10[4]) - (tempSample * biquadM10[6]);
            double M10Sample = -tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            // mid
            tempSample = (side * biquadS3[2]) + biquadS3[7];
            biquadS3[7] = (-tempSample * biquadS3[5]) + biquadS3[8];
            biquadS3[8] = (side * biquadS3[4]) - (tempSample * biquadS3[6]);
            double S3Sample = tempSample * 2.0; // like mono AU, 7 and 8 store L channel
            tempSample = (side * biquadS5[2]) + biquadS5[7];
            biquadS5[7] = (-tempSample * biquadS5[5]) + biquadS5[8];
            biquadS5[8] = (side * biquadS5[4]) - (tempSample * biquadS5[6]);
            double S5Sample = -tempSample * 5.0; // like mono AU, 7 and 8 store L channel
            mid = (M2Sample + M7Sample + M10Sample) * depthM;
            side = (S3Sample + S5Sample + boostside) * depthS;
            double msOutSampleL = (mid + side) / 2.0;
            double msOutSampleR = (mid - side) / 2.0;
            // unassign mid and side
            double isoSampleL = inputSampleL - rawmid;
            double isoSampleR = inputSampleR - rawmid; // trying to isolate L and R a little
            tempSample = (isoSampleL * biquadL3[2]) + biquadL3[7];
            biquadL3[7] = (-tempSample * biquadL3[5]) + biquadL3[8];
            biquadL3[8] = (isoSampleL * biquadL3[4]) - (tempSample * biquadL3[6]);
            double L3Sample = tempSample; // like mono AU, 7 and 8 store L channel
            tempSample = (isoSampleR * biquadR3[2]) + biquadR3[9];
            biquadR3[9] = (-tempSample * biquadR3[5]) + biquadR3[10];
            biquadR3[10] = (isoSampleR * biquadR3[4]) - (tempSample * biquadR3[6]);
            double R3Sample = tempSample; // note: 9 and 10 store the R channel
            tempSample = (isoSampleL * biquadL7[2]) + biquadL7[7];
            biquadL7[7] = (-tempSample * biquadL7[5]) + biquadL7[8];
            biquadL7[8] = (isoSampleL * biquadL7[4]) - (tempSample * biquadL7[6]);
            double L7Sample = tempSample * 3.0; // like mono AU, 7 and 8 store L channel
            tempSample = (isoSampleR * biquadR7[2]) + biquadR7[9];
            biquadR7[9] = (-tempSample * biquadR7[5]) + biquadR7[10];
            biquadR7[10] = (isoSampleR * biquadR7[4]) - (tempSample * biquadR7[6]);
            double R7Sample = tempSample * 3.0; // note: 9 and 10 store the R channel
            double processingL = msOutSampleL + ((L3Sample + L7Sample) * depthS);
            double processingR = msOutSampleR + ((R3Sample + R7Sample) * depthS);
            // done with making filters, now we apply them
            inputSampleL += processingL;
            inputSampleR += processingR;
            if (level < 1.0) {
                inputSampleL *= level;
                inputSampleR *= level;
            }
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
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::srsly
