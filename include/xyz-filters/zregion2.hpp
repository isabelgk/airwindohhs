#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class ZRegion2 final : public Effect<T>
{
    const std::string m_name{ "ZRegion2" };

    double iirSampleAL;
    double iirSampleAR;
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
    }; // coefficient interpolating biquad filter, stereo
    double biquad[biq_total];
    double biquadA[biq_total];
    double biquadB[biq_total];
    double biquadC[biq_total];
    double biquadD[biq_total];
    double inTrimA;
    double inTrimB;
    double wetA;
    double wetB;
    double overallWetA;
    double overallWetB;
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total
    }; // fixed frequency biquad filter for ultrasonics, stereo
    double fixA[fix_total];
    double fixB[fix_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kNumParameters = 5

    };

  public:
    ZRegion2()
    {
        A = 0.1;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        iirSampleAL = 0.0;
        iirSampleAR = 0.0;
        for (int x = 0; x < biq_total; x++) {
            biquad[x] = 0.0;
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
            biquadC[x] = 0.0;
            biquadD[x] = 0.0;
        }
        inTrimA = 0.1;
        inTrimB = 0.1;
        wetA = 0.5;
        wetB = 0.5;
        overallWetA = 1.0;
        overallWetB = 1.0;
        for (int x = 0; x < fix_total; x++) {
            fixA[x] = 0.0;
            fixB[x] = 0.0;
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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
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

    float get_parameter_value(int index) override
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

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input";
            case kParamB: return "First";
            case kParamC: return "Last";
            case kParamD: return "Poles";
            case kParamE: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
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

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        // begin from XRegion
        double high = B;
        double low = C;
        double mid = (high + low) * 0.5;
        double spread = 1.001 - fabs(high - low);
        biquad[biq_freq] = (pow(high, 3) * 20000.0) / getSampleRate();
        if (biquad[biq_freq] < 0.00009) {
            biquad[biq_freq] = 0.00009;
        }
        double compensation = sqrt(biquad[biq_freq]) * 6.4 * spread;
        double clipFactor = 0.75 + (biquad[biq_freq] * D * 37.0);
        biquadA[biq_freq] = (pow((high + mid) * 0.5, 3) * 20000.0) / getSampleRate();
        if (biquadA[biq_freq] < 0.00009) {
            biquadA[biq_freq] = 0.00009;
        }
        double compensationA = sqrt(biquadA[biq_freq]) * 6.4 * spread;
        double clipFactorA = 0.75 + (biquadA[biq_freq] * D * 37.0);
        biquadB[biq_freq] = (pow(mid, 3) * 20000.0) / getSampleRate();
        if (biquadB[biq_freq] < 0.00009) {
            biquadB[biq_freq] = 0.00009;
        }
        double compensationB = sqrt(biquadB[biq_freq]) * 6.4 * spread;
        double clipFactorB = 0.75 + (biquadB[biq_freq] * D * 37.0);
        biquadC[biq_freq] = (pow((mid + low) * 0.5, 3) * 20000.0) / getSampleRate();
        if (biquadC[biq_freq] < 0.00009) {
            biquadC[biq_freq] = 0.00009;
        }
        double compensationC = sqrt(biquadC[biq_freq]) * 6.4 * spread;
        double clipFactorC = 0.75 + (biquadC[biq_freq] * D * 37.0);
        biquadD[biq_freq] = (pow(low, 3) * 20000.0) / getSampleRate();
        if (biquadD[biq_freq] < 0.00009) {
            biquadD[biq_freq] = 0.00009;
        }
        double compensationD = sqrt(biquadD[biq_freq]) * 6.4 * spread;
        double clipFactorD = 0.75 + (biquadD[biq_freq] * D * 37.0);
        // set up all the interpolations
        biquad[biq_aA0] = biquad[biq_aB0];
        biquad[biq_aA1] = biquad[biq_aB1];
        biquad[biq_aA2] = biquad[biq_aB2];
        biquad[biq_bA1] = biquad[biq_bB1];
        biquad[biq_bA2] = biquad[biq_bB2];
        biquadA[biq_aA0] = biquadA[biq_aB0];
        biquadA[biq_aA1] = biquadA[biq_aB1];
        biquadA[biq_aA2] = biquadA[biq_aB2];
        biquadA[biq_bA1] = biquadA[biq_bB1];
        biquadA[biq_bA2] = biquadA[biq_bB2];
        biquadB[biq_aA0] = biquadB[biq_aB0];
        biquadB[biq_aA1] = biquadB[biq_aB1];
        biquadB[biq_aA2] = biquadB[biq_aB2];
        biquadB[biq_bA1] = biquadB[biq_bB1];
        biquadB[biq_bA2] = biquadB[biq_bB2];
        biquadC[biq_aA0] = biquadC[biq_aB0];
        biquadC[biq_aA1] = biquadC[biq_aB1];
        biquadC[biq_aA2] = biquadC[biq_aB2];
        biquadC[biq_bA1] = biquadC[biq_bB1];
        biquadC[biq_bA2] = biquadC[biq_bB2];
        biquadD[biq_aA0] = biquadD[biq_aB0];
        biquadD[biq_aA1] = biquadD[biq_aB1];
        biquadD[biq_aA2] = biquadD[biq_aB2];
        biquadD[biq_bA1] = biquadD[biq_bB1];
        biquadD[biq_bA2] = biquadD[biq_bB2];
        // since this is Region, they are all different
        double K = tan(M_PI * biquad[biq_freq]);
        double norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquad[biq_aB0] = K / 0.7071 * norm;
        biquad[biq_aB2] = -biquad[biq_aB0];
        biquad[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquad[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadA[biq_freq]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadA[biq_aB0] = K / 0.7071 * norm;
        biquadA[biq_aB2] = -biquadA[biq_aB0];
        biquadA[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadA[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadB[biq_freq]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadB[biq_aB0] = K / 0.7071 * norm;
        biquadB[biq_aB2] = -biquadB[biq_aB0];
        biquadB[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadB[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadC[biq_freq]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadC[biq_aB0] = K / 0.7071 * norm;
        biquadC[biq_aB2] = -biquadC[biq_aB0];
        biquadC[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadC[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
        K = tan(M_PI * biquadD[biq_freq]);
        norm = 1.0 / (1.0 + K / 0.7071 + K * K);
        biquadD[biq_aB0] = K / 0.7071 * norm;
        biquadD[biq_aB2] = -biquadD[biq_aB0];
        biquadD[biq_bB1] = 2.0 * (K * K - 1.0) * norm;
        biquadD[biq_bB2] = (1.0 - K / 0.7071 + K * K) * norm;
        // opamp stuff
        inTrimA = inTrimB;
        inTrimB = A * 10.0;
        inTrimB *= inTrimB;
        inTrimB *= inTrimB;
        wetA = wetB;
        wetB = D;
        overallWetA = overallWetB;
        overallWetB = E;
        double iirAmountA = 0.00069 / overallscale;
        fixB[fix_freq] = fixA[fix_freq] = 15500.0 / getSampleRate();
        fixB[fix_reso] = fixA[fix_reso] = 0.935;
        K = tan(M_PI * fixA[fix_freq]); // lowpass
        norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        for (int x = 0; x < 7; x++) {
            fixB[x] = fixA[x];
        }
        // end opamp stuff
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
            double overallDrySampleL = inputSampleL;
            double overallDrySampleR = inputSampleR;
            double nukeLevelL = inputSampleL;
            double nukeLevelR = inputSampleR;
            double temp = (double)sampleFrames / inFramesToProcess;
            biquad[biq_a0] = (biquad[biq_aA0] * temp) + (biquad[biq_aB0] * (1.0 - temp));
            biquad[biq_a1] = (biquad[biq_aA1] * temp) + (biquad[biq_aB1] * (1.0 - temp));
            biquad[biq_a2] = (biquad[biq_aA2] * temp) + (biquad[biq_aB2] * (1.0 - temp));
            biquad[biq_b1] = (biquad[biq_bA1] * temp) + (biquad[biq_bB1] * (1.0 - temp));
            biquad[biq_b2] = (biquad[biq_bA2] * temp) + (biquad[biq_bB2] * (1.0 - temp));
            biquadA[biq_a0] = (biquadA[biq_aA0] * temp) + (biquadA[biq_aB0] * (1.0 - temp));
            biquadA[biq_a1] = (biquadA[biq_aA1] * temp) + (biquadA[biq_aB1] * (1.0 - temp));
            biquadA[biq_a2] = (biquadA[biq_aA2] * temp) + (biquadA[biq_aB2] * (1.0 - temp));
            biquadA[biq_b1] = (biquadA[biq_bA1] * temp) + (biquadA[biq_bB1] * (1.0 - temp));
            biquadA[biq_b2] = (biquadA[biq_bA2] * temp) + (biquadA[biq_bB2] * (1.0 - temp));
            biquadB[biq_a0] = (biquadB[biq_aA0] * temp) + (biquadB[biq_aB0] * (1.0 - temp));
            biquadB[biq_a1] = (biquadB[biq_aA1] * temp) + (biquadB[biq_aB1] * (1.0 - temp));
            biquadB[biq_a2] = (biquadB[biq_aA2] * temp) + (biquadB[biq_aB2] * (1.0 - temp));
            biquadB[biq_b1] = (biquadB[biq_bA1] * temp) + (biquadB[biq_bB1] * (1.0 - temp));
            biquadB[biq_b2] = (biquadB[biq_bA2] * temp) + (biquadB[biq_bB2] * (1.0 - temp));
            biquadC[biq_a0] = (biquadC[biq_aA0] * temp) + (biquadC[biq_aB0] * (1.0 - temp));
            biquadC[biq_a1] = (biquadC[biq_aA1] * temp) + (biquadC[biq_aB1] * (1.0 - temp));
            biquadC[biq_a2] = (biquadC[biq_aA2] * temp) + (biquadC[biq_aB2] * (1.0 - temp));
            biquadC[biq_b1] = (biquadC[biq_bA1] * temp) + (biquadC[biq_bB1] * (1.0 - temp));
            biquadC[biq_b2] = (biquadC[biq_bA2] * temp) + (biquadC[biq_bB2] * (1.0 - temp));
            biquadD[biq_a0] = (biquadD[biq_aA0] * temp) + (biquadD[biq_aB0] * (1.0 - temp));
            biquadD[biq_a1] = (biquadD[biq_aA1] * temp) + (biquadD[biq_aB1] * (1.0 - temp));
            biquadD[biq_a2] = (biquadD[biq_aA2] * temp) + (biquadD[biq_aB2] * (1.0 - temp));
            biquadD[biq_b1] = (biquadD[biq_bA1] * temp) + (biquadD[biq_bB1] * (1.0 - temp));
            biquadD[biq_b2] = (biquadD[biq_bA2] * temp) + (biquadD[biq_bB2] * (1.0 - temp));
            // this is the interpolation code for all the biquads
            double inTrim = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            double wet = (wetA * temp) + (wetB * (1.0 - temp));
            double aWet = 1.0;
            double bWet = 1.0;
            double cWet = 1.0;
            double dWet = wet * 4.0;
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
            double overallWet = (overallWetA * temp) + (overallWetB * (1.0 - temp));
            if (inTrim != 1.0) {
                inputSampleL *= inTrim;
                inputSampleR *= inTrim;
            }
            // begin XRegion
            inputSampleL *= clipFactor;
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            outSample = (inputSampleL * biquad[biq_a0]) + biquad[biq_sL1];
            biquad[biq_sL1] = (inputSampleL * biquad[biq_a1]) - (outSample * biquad[biq_b1]) + biquad[biq_sL2];
            biquad[biq_sL2] = (inputSampleL * biquad[biq_a2]) - (outSample * biquad[biq_b2]);
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
            outSample = (inputSampleR * biquad[biq_a0]) + biquad[biq_sR1];
            biquad[biq_sR1] = (inputSampleR * biquad[biq_a1]) - (outSample * biquad[biq_b1]) + biquad[biq_sR2];
            biquad[biq_sR2] = (inputSampleR * biquad[biq_a2]) - (outSample * biquad[biq_b2]);
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
                outSample = (inputSampleL * biquadA[biq_a0]) + biquadA[biq_sL1];
                biquadA[biq_sL1] = (inputSampleL * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sL2];
                biquadA[biq_sL2] = (inputSampleL * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
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
                outSample = (inputSampleR * biquadA[biq_a0]) + biquadA[biq_sR1];
                biquadA[biq_sR1] = (inputSampleR * biquadA[biq_a1]) - (outSample * biquadA[biq_b1]) + biquadA[biq_sR2];
                biquadA[biq_sR2] = (inputSampleR * biquadA[biq_a2]) - (outSample * biquadA[biq_b2]);
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
                outSample = (inputSampleL * biquadB[biq_a0]) + biquadB[biq_sL1];
                biquadB[biq_sL1] = (inputSampleL * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sL2];
                biquadB[biq_sL2] = (inputSampleL * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
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
                outSample = (inputSampleR * biquadB[biq_a0]) + biquadB[biq_sR1];
                biquadB[biq_sR1] = (inputSampleR * biquadB[biq_a1]) - (outSample * biquadB[biq_b1]) + biquadB[biq_sR2];
                biquadB[biq_sR2] = (inputSampleR * biquadB[biq_a2]) - (outSample * biquadB[biq_b2]);
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
                outSample = (inputSampleL * biquadC[biq_a0]) + biquadC[biq_sL1];
                biquadC[biq_sL1] = (inputSampleL * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sL2];
                biquadC[biq_sL2] = (inputSampleL * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
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
                outSample = (inputSampleR * biquadC[biq_a0]) + biquadC[biq_sR1];
                biquadC[biq_sR1] = (inputSampleR * biquadC[biq_a1]) - (outSample * biquadC[biq_b1]) + biquadC[biq_sR2];
                biquadC[biq_sR2] = (inputSampleR * biquadC[biq_a2]) - (outSample * biquadC[biq_b2]);
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
                outSample = (inputSampleL * biquadD[biq_a0]) + biquadD[biq_sL1];
                biquadD[biq_sL1] = (inputSampleL * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sL2];
                biquadD[biq_sL2] = (inputSampleL * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
                inputSampleL = outSample / compensationD;
                inputSampleL = (inputSampleL * dWet) + (nukeLevelL * (1.0 - dWet));
                inputSampleR *= clipFactorD;
                if (inputSampleR > 1.57079633) {
                    inputSampleR = 1.57079633;
                }
                if (inputSampleR < -1.57079633) {
                    inputSampleR = -1.57079633;
                }
                inputSampleR = sin(inputSampleR);
                outSample = (inputSampleR * biquadD[biq_a0]) + biquadD[biq_sR1];
                biquadD[biq_sR1] = (inputSampleR * biquadD[biq_a1]) - (outSample * biquadD[biq_b1]) + biquadD[biq_sR2];
                biquadD[biq_sR2] = (inputSampleR * biquadD[biq_a2]) - (outSample * biquadD[biq_b2]);
                inputSampleR = outSample / compensationD;
                inputSampleR = (inputSampleR * dWet) + (nukeLevelR * (1.0 - dWet));
            }
            // end XRegion
            // opamp stage
            if (fabs(iirSampleAL) < 1.18e-37) {
                iirSampleAL = 0.0;
            }
            iirSampleAL = (iirSampleAL * (1.0 - iirAmountA)) + (inputSampleL * iirAmountA);
            inputSampleL -= iirSampleAL;
            if (fabs(iirSampleAR) < 1.18e-37) {
                iirSampleAR = 0.0;
            }
            iirSampleAR = (iirSampleAR * (1.0 - iirAmountA)) + (inputSampleR * iirAmountA);
            inputSampleR -= iirSampleAR;
            outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            inputSampleL -= (inputSampleL * inputSampleL * inputSampleL * inputSampleL * inputSampleL * 0.1768);
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            inputSampleR -= (inputSampleR * inputSampleR * inputSampleR * inputSampleR * inputSampleR * 0.1768);
            outSample = (inputSampleL * fixB[fix_a0]) + fixB[fix_sL1];
            fixB[fix_sL1] = (inputSampleL * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sL2];
            fixB[fix_sL2] = (inputSampleL * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            outSample = (inputSampleR * fixB[fix_a0]) + fixB[fix_sR1];
            fixB[fix_sR1] = (inputSampleR * fixB[fix_a1]) - (outSample * fixB[fix_b1]) + fixB[fix_sR2];
            fixB[fix_sR2] = (inputSampleR * fixB[fix_a2]) - (outSample * fixB[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            // end opamp stage
            if (overallWet != 1.0) {
                inputSampleL = (inputSampleL * overallWet) + (overallDrySampleL * (1.0 - overallWet));
                inputSampleR = (inputSampleR * overallWet) + (overallDrySampleR * (1.0 - overallWet));
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
} // namespace airwindohhs
