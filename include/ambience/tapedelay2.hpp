#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class TapeDelay2 final : public Effect<T>
{
    const std::string m_name{ "TapeDelay2" };

    double dL[88211];
    double prevSampleL;
    double delayL;
    double sweepL;
    double regenFilterL[9];
    double outFilterL[9];
    double lastRefL[10];
    double dR[88211];
    double prevSampleR;
    double delayR;
    double sweepR;
    double regenFilterR[9];
    double outFilterR[9];
    double lastRefR[10];
    int cycle;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kParamD = 3,
        kParamE = 4,
        kParamF = 5,
        kNumParameters = 6

    };

  public:
    TapeDelay2()
    {
        A = 1.0;
        B = 0.0;
        C = 0.5;
        D = 0.0;
        E = 0.0;
        F = 1.0;
        for (int x = 0; x < 88210; x++) {
            dL[x] = 0.0;
            dR[x] = 0.0;
        }
        prevSampleL = 0.0;
        delayL = 0.0;
        sweepL = 0.0;
        prevSampleR = 0.0;
        delayR = 0.0;
        sweepR = 0.0;
        for (int x = 0; x < 9; x++) {
            regenFilterL[x] = 0.0;
            outFilterL[x] = 0.0;
            lastRefL[x] = 0.0;
            regenFilterR[x] = 0.0;
            outFilterR[x] = 0.0;
            lastRefR[x] = 0.0;
        }
        cycle = 0;
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
            case kParamF: F = value; break;

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
            case kParamF: return F;

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Time";
            case kParamB: return "Regen";
            case kParamC: return "Freq";
            case kParamD: return "Reso";
            case kParamE: return "Flutter";
            case kParamF: return "Dry/Wet";

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
            case kParamF: return std::to_string(F);

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
            case kParamF: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        if (cycle > cycleEnd - 1) {
            cycle = cycleEnd - 1; // sanity check
        }
        double baseSpeed = (pow(A, 4) * 25.0) + 1.0;
        double feedback = pow(B, 2);
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        regenFilterL[0] = regenFilterR[0] = ((pow(C, 3) * 0.4) + 0.0001);
        regenFilterL[1] = regenFilterR[1] = pow(D, 2) + 0.01; // resonance
        double K = tan(M_PI * regenFilterR[0]);
        double norm = 1.0 / (1.0 + K / regenFilterR[1] + K * K);
        regenFilterL[2] = regenFilterR[2] = K / regenFilterR[1] * norm;
        regenFilterL[4] = regenFilterR[4] = -regenFilterR[2];
        regenFilterL[5] = regenFilterR[5] = 2.0 * (K * K - 1.0) * norm;
        regenFilterL[6] = regenFilterR[6] = (1.0 - K / regenFilterR[1] + K * K) * norm;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        outFilterL[0] = outFilterR[0] = regenFilterR[0];
        outFilterL[1] = outFilterR[1] = regenFilterR[1] * 1.618033988749894848204586; // resonance
        K = tan(M_PI * outFilterR[0]);
        norm = 1.0 / (1.0 + K / outFilterR[1] + K * K);
        outFilterL[2] = outFilterR[2] = K / outFilterR[1] * norm;
        outFilterL[4] = outFilterR[4] = -outFilterR[2];
        outFilterL[5] = outFilterR[5] = 2.0 * (K * K - 1.0) * norm;
        outFilterL[6] = outFilterR[6] = (1.0 - K / outFilterR[1] + K * K) * norm;
        double vibSpeed = pow(E, 5) * baseSpeed * ((regenFilterR[0] * 0.09) + 0.025); // 0.05
        double wet = F * 2.0;
        double dry = 2.0 - wet;
        if (wet > 1.0) {
            wet = 1.0;
        }
        if (wet < 0.0) {
            wet = 0.0;
        }
        if (dry > 1.0) {
            dry = 1.0;
        }
        if (dry < 0.0) {
            dry = 0.0;
        }
        // this echo makes 50% full dry AND full wet, not crossfaded.
        // that's so it can be on submixes without cutting back dry channel when adjusted:
        // unless you go super heavy, you are only adjusting the added echo loudness.
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
            cycle++;
            if (cycle == cycleEnd) {
                double speedL = baseSpeed + (vibSpeed * (sin(sweepL) + 1.0));
                double speedR = baseSpeed + (vibSpeed * (sin(sweepR) + 1.0));
                sweepL += (0.05 * inputSampleL * inputSampleL);
                if (sweepL > 6.283185307179586) {
                    sweepL -= 6.283185307179586;
                }
                sweepR += (0.05 * inputSampleR * inputSampleR);
                if (sweepR > 6.283185307179586) {
                    sweepR -= 6.283185307179586;
                }
                // begin left channel
                int pos = floor(delayL);
                double newSample = inputSampleL + dL[pos] * feedback;
                double tempSample = (newSample * regenFilterL[2]) + regenFilterL[7];
                regenFilterL[7] = -(tempSample * regenFilterL[5]) + regenFilterL[8];
                regenFilterL[8] = (newSample * regenFilterL[4]) - (tempSample * regenFilterL[6]);
                newSample = tempSample;
                delayL -= speedL;
                if (delayL < 0) {
                    delayL += 88200.0;
                }
                double increment = (newSample - prevSampleL) / speedL;
                dL[pos] = prevSampleL;
                while (pos != floor(delayL)) {
                    dL[pos] = prevSampleL;
                    prevSampleL += increment;
                    pos--;
                    if (pos < 0) {
                        pos += 88200;
                    }
                }
                prevSampleL = newSample;
                pos = floor(delayL);
                inputSampleL = dL[pos];
                tempSample = (inputSampleL * outFilterL[2]) + outFilterL[7];
                outFilterL[7] = -(tempSample * outFilterL[5]) + outFilterL[8];
                outFilterL[8] = (inputSampleL * outFilterL[4]) - (tempSample * outFilterL[6]);
                inputSampleL = tempSample;
                // end left channel
                // begin right channel
                pos = floor(delayR);
                newSample = inputSampleR + dR[pos] * feedback;
                tempSample = (newSample * regenFilterR[2]) + regenFilterR[7];
                regenFilterR[7] = -(tempSample * regenFilterR[5]) + regenFilterR[8];
                regenFilterR[8] = (newSample * regenFilterR[4]) - (tempSample * regenFilterR[6]);
                newSample = tempSample;
                delayR -= speedR;
                if (delayR < 0) {
                    delayR += 88200.0;
                }
                increment = (newSample - prevSampleR) / speedR;
                dR[pos] = prevSampleR;
                while (pos != floor(delayR)) {
                    dR[pos] = prevSampleR;
                    prevSampleR += increment;
                    pos--;
                    if (pos < 0) {
                        pos += 88200;
                    }
                }
                prevSampleR = newSample;
                pos = floor(delayR);
                inputSampleR = dR[pos];
                tempSample = (inputSampleR * outFilterR[2]) + outFilterR[7];
                outFilterR[7] = -(tempSample * outFilterR[5]) + outFilterR[8];
                outFilterR[8] = (inputSampleR * outFilterR[4]) - (tempSample * outFilterR[6]);
                inputSampleR = tempSample;
                // end right channel
                if (cycleEnd == 4) {
                    lastRefL[0] = lastRefL[4]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[1] = (lastRefL[0] + lastRefL[2]) / 2; // one quarter
                    lastRefL[3] = (lastRefL[2] + inputSampleL) / 2; // three quarters
                    lastRefL[4] = inputSampleL; // full
                    lastRefR[0] = lastRefR[4]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[1] = (lastRefR[0] + lastRefR[2]) / 2; // one quarter
                    lastRefR[3] = (lastRefR[2] + inputSampleR) / 2; // three quarters
                    lastRefR[4] = inputSampleR; // full
                }
                if (cycleEnd == 3) {
                    lastRefL[0] = lastRefL[3]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + lastRefL[0] + inputSampleL) / 3; // third
                    lastRefL[1] = (lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
                    lastRefL[3] = inputSampleL; // full
                    lastRefR[0] = lastRefR[3]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + lastRefR[0] + inputSampleR) / 3; // third
                    lastRefR[1] = (lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
                    lastRefR[3] = inputSampleR; // full
                }
                if (cycleEnd == 2) {
                    lastRefL[0] = lastRefL[2]; // start from previous last
                    lastRefL[1] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[2] = inputSampleL; // full
                    lastRefR[0] = lastRefR[2]; // start from previous last
                    lastRefR[1] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[2] = inputSampleR; // full
                }
                if (cycleEnd == 1) {
                    lastRefL[0] = inputSampleL;
                    lastRefR[0] = inputSampleR;
                }
                cycle = 0; // reset
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
            }
            else {
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
                // we are going through our references now
            }
            switch (cycleEnd) // multi-pole average using lastRef[] variables
            {
                case 4:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[7]) * 0.5;
                    lastRefL[7] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[7]) * 0.5;
                    lastRefR[7] = lastRefR[8]; // continue, do not break
                case 3:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[6]) * 0.5;
                    lastRefL[6] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[6]) * 0.5;
                    lastRefR[6] = lastRefR[8]; // continue, do not break
                case 2:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[5]) * 0.5;
                    lastRefL[5] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[5]) * 0.5;
                    lastRefR[5] = lastRefR[8]; // continue, do not break
                case 1:
                    break; // no further averaging
            }
            if (wet < 1.0) {
                inputSampleL *= wet;
                inputSampleR *= wet;
            }
            if (dry < 1.0) {
                drySampleL *= dry;
                drySampleR *= dry;
            }
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
            // this is our submix echo dry/wet: 0.5 is BOTH at FULL VOLUME
            // purpose is that, if you're adding echo, you're not altering other balances
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
