#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Stonefire final : public Effect<T>
{
    const std::string m_name{ "Stonefire" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    enum
    {
        pvAL1,
        pvSL1,
        accSL1,
        acc2SL1,
        pvAL2,
        pvSL2,
        accSL2,
        acc2SL2,
        pvAL3,
        pvSL3,
        accSL3,
        pvAL4,
        pvSL4,
        gndavgL,
        outAL,
        gainAL,
        pvAR1,
        pvSR1,
        accSR1,
        acc2SR1,
        pvAR2,
        pvSR2,
        accSR2,
        acc2SR2,
        pvAR3,
        pvSR3,
        accSR3,
        pvAR4,
        pvSR4,
        gndavgR,
        outAR,
        gainAR,
        air_total
    };
    double air[air_total];
    enum
    {
        prevSampL1,
        prevSlewL1,
        accSlewL1,
        prevSampL2,
        prevSlewL2,
        accSlewL2,
        prevSampL3,
        prevSlewL3,
        accSlewL3,
        kalGainL,
        kalOutL,
        prevSampR1,
        prevSlewR1,
        accSlewR1,
        prevSampR2,
        prevSlewR2,
        accSlewR2,
        prevSampR3,
        prevSlewR3,
        accSlewR3,
        kalGainR,
        kalOutR,
        kal_total
    };
    double kal[kal_total];
    double trebleGainA;
    double trebleGainB;
    double midGainA;
    double midGainB;
    double bassGainA;
    double bassGainB;
    float A;
    float B;
    float C;
    float D;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kNumParameters = 4

    };

  public:
    Stonefire()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        for (int x = 0; x < air_total; x++) {
            air[x] = 0.0;
        }
        for (int x = 0; x < kal_total; x++) {
            kal[x] = 0.0;
        }
        trebleGainA = 1.0;
        trebleGainB = 1.0;
        midGainA = 1.0;
        midGainB = 1.0;
        bassGainA = 1.0;
        bassGainB = 1.0;
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

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Air";
            case kParamB: return "Fire";
            case kParamC: return "Stone";
            case kParamD: return "Range";

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
        trebleGainA = trebleGainB;
        trebleGainB = A * 2.0;
        midGainA = midGainB;
        midGainB = B * 2.0;
        bassGainA = bassGainB;
        bassGainB = C * 2.0;
        // simple three band to adjust
        double kalman = 1.0 - pow(D, 2);
        // crossover frequency between mid/bass
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double trebleGain = (trebleGainA * temp) + (trebleGainB * (1.0 - temp));
            if (trebleGain > 1.0) {
                trebleGain = pow(trebleGain, 3.0 + sqrt(overallscale));
            }
            if (trebleGain < 1.0) {
                trebleGain = 1.0 - pow(1.0 - trebleGain, 2);
            }
            double midGain = (midGainA * temp) + (midGainB * (1.0 - temp));
            if (midGain > 1.0) {
                midGain *= midGain;
            }
            if (midGain < 1.0) {
                midGain = 1.0 - pow(1.0 - midGain, 2);
            }
            double bassGain = (bassGainA * temp) + (bassGainB * (1.0 - temp));
            if (bassGain > 1.0) {
                bassGain *= bassGain;
            }
            if (bassGain < 1.0) {
                bassGain = 1.0 - pow(1.0 - bassGain, 2);
            }
            // begin Air3L
            air[pvSL4] = air[pvAL4] - air[pvAL3];
            air[pvSL3] = air[pvAL3] - air[pvAL2];
            air[pvSL2] = air[pvAL2] - air[pvAL1];
            air[pvSL1] = air[pvAL1] - inputSampleL;
            air[accSL3] = air[pvSL4] - air[pvSL3];
            air[accSL2] = air[pvSL3] - air[pvSL2];
            air[accSL1] = air[pvSL2] - air[pvSL1];
            air[acc2SL2] = air[accSL3] - air[accSL2];
            air[acc2SL1] = air[accSL2] - air[accSL1];
            air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1]) * 0.5));
            air[gainAL] *= 0.5;
            air[gainAL] += fabs(drySampleL - air[outAL]) * 0.5;
            if (air[gainAL] > 0.3 * sqrt(overallscale)) {
                air[gainAL] = 0.3 * sqrt(overallscale);
            }
            air[pvAL4] = air[pvAL3];
            air[pvAL3] = air[pvAL2];
            air[pvAL2] = air[pvAL1];
            air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
            double midL = drySampleL - ((air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
            temp = (midL + air[gndavgL]) * 0.5;
            air[gndavgL] = midL;
            midL = temp;
            double trebleL = drySampleL - midL;
            inputSampleL = midL;
            // end Air3L
            // begin Air3R
            air[pvSR4] = air[pvAR4] - air[pvAR3];
            air[pvSR3] = air[pvAR3] - air[pvAR2];
            air[pvSR2] = air[pvAR2] - air[pvAR1];
            air[pvSR1] = air[pvAR1] - inputSampleR;
            air[accSR3] = air[pvSR4] - air[pvSR3];
            air[accSR2] = air[pvSR3] - air[pvSR2];
            air[accSR1] = air[pvSR2] - air[pvSR1];
            air[acc2SR2] = air[accSR3] - air[accSR2];
            air[acc2SR1] = air[accSR2] - air[accSR1];
            air[outAR] = -(air[pvAR1] + air[pvSR3] + air[acc2SR2] - ((air[acc2SR2] + air[acc2SR1]) * 0.5));
            air[gainAR] *= 0.5;
            air[gainAR] += fabs(drySampleR - air[outAR]) * 0.5;
            if (air[gainAR] > 0.3 * sqrt(overallscale)) {
                air[gainAR] = 0.3 * sqrt(overallscale);
            }
            air[pvAR4] = air[pvAR3];
            air[pvAR3] = air[pvAR2];
            air[pvAR2] = air[pvAR1];
            air[pvAR1] = (air[gainAR] * air[outAR]) + drySampleR;
            double midR = drySampleR - ((air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
            temp = (midR + air[gndavgR]) * 0.5;
            air[gndavgR] = midR;
            midR = temp;
            double trebleR = drySampleR - midR;
            inputSampleR = midR;
            // end Air3R
            // begin KalmanL
            temp = inputSampleL = inputSampleL * (1.0 - kalman) * 0.777;
            inputSampleL *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2];
            kal[prevSlewL3] *= 0.5;
            kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1];
            kal[prevSlewL2] *= 0.5;
            kal[prevSlewL1] += kal[prevSampL1] - inputSampleL;
            kal[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2];
            kal[accSlewL2] *= 0.5;
            kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1];
            kal[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]);
            kal[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3];
            kal[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainL] += fabs(temp - kal[kalOutL]) * kalman * 8.0;
            kal[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainL] > kalman * 0.5) {
                kal[kalGainL] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutL] += (temp * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampL3] = kal[prevSampL2];
            kal[prevSampL2] = kal[prevSampL1];
            kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0 - kal[kalGainL]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampL1] > 1.0) {
                kal[prevSampL1] = 1.0;
            }
            if (kal[prevSampL1] < -1.0) {
                kal[prevSampL1] = -1.0;
            }
            double bassL = kal[kalOutL] * 0.777;
            midL -= bassL;
            // end KalmanL
            // begin KalmanR
            temp = inputSampleR = inputSampleR * (1.0 - kalman) * 0.777;
            inputSampleR *= (1.0 - kalman);
            // set up gain levels to control the beast
            kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2];
            kal[prevSlewR3] *= 0.5;
            kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1];
            kal[prevSlewR2] *= 0.5;
            kal[prevSlewR1] += kal[prevSampR1] - inputSampleR;
            kal[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2];
            kal[accSlewR2] *= 0.5;
            kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1];
            kal[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]);
            kal[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3];
            kal[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainR] += fabs(temp - kal[kalOutR]) * kalman * 8.0;
            kal[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainR] > kalman * 0.5) {
                kal[kalGainR] = kalman * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutR] += (temp * (1.0 - (0.68 + (kalman * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampR3] = kal[prevSampR2];
            kal[prevSampR2] = kal[prevSampR1];
            kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0 - kal[kalGainR]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampR1] > 1.0) {
                kal[prevSampR1] = 1.0;
            }
            if (kal[prevSampR1] < -1.0) {
                kal[prevSampR1] = -1.0;
            }
            double bassR = kal[kalOutR] * 0.777;
            midR -= bassR;
            // end KalmanR
            inputSampleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            inputSampleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
            // applies pan section, and smoothed fader gain
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
