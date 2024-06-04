#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ironoxideclassic2 {

constexpr std::string_view k_name{ "IronOxideClassic2" };
constexpr std::string_view k_short_description{
    "IronOxideClassic2 is my bandpassy tape sim, updated for high sample rate and aliasing control."
};
constexpr std::string_view k_long_description{
    "Iron Oxide was one of my first successful plugins. It’s an old school tape emulation, meaning that it’s kind of bandpassy and is about saturating and slamming and aggressive tone coloring. It was made to compete with a pricey (well, back then it was) commercial plugin where the company had treated some of my friends poorly: I charged off and made a ‘commercial tape emulation killer’ plugin, with very unorthodox techniques. This is before my ToTape plugins, before head bumps: just a fat saturated tape-slam plugin.This grew to have all sorts of things: separate ips controls for low and high cutoffs, flutter, just lots of stuff. But the original one was an input, a tape speed control, and an output… and Iron Oxide Classic is me returning to those roots. It’s also handy, because as I bring in stuff like undersampling of delay buffer plugins, I can get the fundamentals right on a simpler build. That’s what this is. It’s Iron Oxide Classic, the simplest form, but brought up to the latest technical specs.That means it’s using the undersampling to deliver the same tone whether you’re at 44.1k or 96k or 192k. Though it uses delay buffers and samples along a time window, in the new version that’s consistent among sample rates. That also means it’s substantially more CPU-friendly at high rates, so you might be able to run twice or four times as many of ’em. It’s also using anti-aliasing filtering (that kicks in at high rates and isn’t ‘in circuit’ at low rates) to clean up its behavior still further. The end result is that the new Iron Oxide Classic has a way more organic, natural tone than the previous one did. These things (and running projects at 2X or 4X) really help get the analog vibe out of ITB digital gear. Since Iron Oxide Classic 2 is able to adapt to this stuff gracefully, that also means it’ll handle rendering at 2x or 4X sample rate should your workflow require working at 1X and then rendering out the final audio at a higher rate. Mind you, I design stuff so you’ll be able to work directly at the higher rate, but this should have you covered whatever your workflow."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class IronOxideClassic2 final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double dL[264];
    double fastIIRAL;
    double fastIIRBL;
    double slowIIRAL;
    double slowIIRBL;
    double lastRefL[7];
    double iirSampleAR;
    double iirSampleBR;
    double dR[264];
    double fastIIRAR;
    double fastIIRBR;
    double slowIIRAR;
    double slowIIRBR;
    double lastRefR[7];
    double biquadA[15];
    double biquadB[15];
    int cycle;
    int gcount;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    IronOxideClassic2()
    {
        A = 0.5;
        B = 0.562341325190349;
        C = 0.5;
        for (int x = 0; x < 15; x++) {
            biquadA[x] = 0.0;
            biquadB[x] = 0.0;
        }
        for (int temp = 0; temp < 263; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        for (int count = 0; count < 6; count++) {
            lastRefL[count] = 0.0;
            lastRefR[count] = 0.0;
        }
        cycle = 0;
        gcount = 0;
        fastIIRAL = fastIIRBL = slowIIRAL = slowIIRBL = 0.0;
        iirSampleAL = iirSampleBL = 0.0;
        fastIIRAR = fastIIRBR = slowIIRAR = slowIIRBR = 0.0;
        iirSampleAR = iirSampleBR = 0.0;
        flip = true;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.562341325190349;
            case kParamC: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input trim";
            case kParamB: return "tape speed";
            case kParamC: return "output trim";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Trim";
            case kParamB: return "Tape Speed";
            case kParamC: return "Output Trim";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A*36.0)-18.0));
            case kParamB: return std::to_string((B*B)*148.5)+1.5);
            case kParamC: return std::to_string((C*36.0)-18.0));

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return "ips";
            case kParamC: return "dB";
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
        double inputgain = pow(10.0, ((A * 36.0) - 18.0) / 20.0);
        double outputgain = pow(10.0, ((C * 36.0) - 18.0) / 20.0);
        double ips = (((B * B) * (B * B) * 148.5) + 1.5) * 1.1;
        // slight correction to dial in convincing ips settings
        if (ips < 1 || ips > 200) {
            ips = 33.0;
        }
        // sanity checks are always key
        double iirAmount = ips / 430.0; // for low leaning
        double fastTaper = ips / 15.0;
        double slowTaper = 2.0 / (ips * ips);
        iirAmount /= overallscale;
        fastTaper /= overallscale;
        slowTaper /= overallscale;
        // now that we have this, we must multiply it back up
        fastTaper *= cycleEnd;
        slowTaper *= cycleEnd;
        // because we're only running that part one sample in two, or three, or four
        fastTaper += 1.0;
        slowTaper += 1.0;
        biquadA[0] = 24000.0 / getSampleRate();
        biquadA[1] = 1.618033988749894848204586;
        biquadB[0] = 24000.0 / getSampleRate();
        biquadB[1] = 0.618033988749894848204586;
        double K = tan(M_PI * biquadA[0]); // lowpass
        double norm = 1.0 / (1.0 + K / biquadA[1] + K * K);
        biquadA[2] = K * K * norm;
        biquadA[3] = 2.0 * biquadA[2];
        biquadA[4] = biquadA[2];
        biquadA[5] = 2.0 * (K * K - 1.0) * norm;
        biquadA[6] = (1.0 - K / biquadA[1] + K * K) * norm;
        K = tan(M_PI * biquadB[0]); // lowpass
        norm = 1.0 / (1.0 + K / biquadB[1] + K * K);
        biquadB[2] = K * K * norm;
        biquadB[3] = 2.0 * biquadB[2];
        biquadB[4] = biquadB[2];
        biquadB[5] = 2.0 * (K * K - 1.0) * norm;
        biquadB[6] = (1.0 - K / biquadB[1] + K * K) * norm;
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
            double tempSample;
            if (flip)
            {
                if (fabs(iirSampleAL) < 1.18e-37) {
                    iirSampleAL = 0.0;
                }
                if (fabs(iirSampleAR) < 1.18e-37) {
                    iirSampleAR = 0.0;
                }
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                if (fabs(iirSampleBL) < 1.18e-37) {
                    iirSampleBL = 0.0;
                }
                if (fabs(iirSampleBR) < 1.18e-37) {
                    iirSampleBR = 0.0;
                }
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // do IIR highpass for leaning out
            if (biquadA[0] < 0.49999) {
                tempSample = biquadA[2] * inputSampleL + biquadA[3] * biquadA[7] + biquadA[4] * biquadA[8] - biquadA[5] * biquadA[9] - biquadA[6] * biquadA[10];
                biquadA[8] = biquadA[7];
                biquadA[7] = inputSampleL;
                inputSampleL = tempSample;
                biquadA[10] = biquadA[9];
                biquadA[9] = inputSampleL; // DF1 left
                tempSample = biquadA[2] * inputSampleR + biquadA[3] * biquadA[11] + biquadA[4] * biquadA[12] - biquadA[5] * biquadA[13] - biquadA[6] * biquadA[14];
                biquadA[12] = biquadA[11];
                biquadA[11] = inputSampleR;
                inputSampleR = tempSample;
                biquadA[14] = biquadA[13];
                biquadA[13] = inputSampleR; // DF1 right
            }
            if (inputgain != 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            }
            double bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // preliminary gain stage using antialiasing
            double bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // preliminary gain stage using antialiasing
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do a tape sample
                // over to the Iron Oxide shaping code using inputsample
                if (gcount < 0 || gcount > 131) {
                    gcount = 131;
                }
                int count = gcount;
                // increment the counter
                double temp;
                dL[count + 131] = dL[count] = inputSampleL;
                dR[count + 131] = dR[count] = inputSampleR;
                if (flip)
                {
                    if (fabs(fastIIRAL) < 1.18e-37) {
                        fastIIRAL = 0.0;
                    }
                    if (fabs(slowIIRAL) < 1.18e-37) {
                        slowIIRAL = 0.0;
                    }
                    fastIIRAL = fastIIRAL / fastTaper;
                    slowIIRAL = slowIIRAL / slowTaper;
                    fastIIRAL += dL[count];
                    // scale stuff down
                    if (fabs(fastIIRAR) < 1.18e-37) {
                        fastIIRAR = 0.0;
                    }
                    if (fabs(slowIIRAR) < 1.18e-37) {
                        slowIIRAR = 0.0;
                    }
                    fastIIRAR = fastIIRAR / fastTaper;
                    slowIIRAR = slowIIRAR / slowTaper;
                    fastIIRAR += dR[count];
                    // scale stuff down
                    count += 3;
                    temp = dL[count + 127];
                    temp += dL[count + 113];
                    temp += dL[count + 109];
                    temp += dL[count + 107];
                    temp += dL[count + 103];
                    temp += dL[count + 101];
                    temp += dL[count + 97];
                    temp += dL[count + 89];
                    temp += dL[count + 83];
                    temp /= 2;
                    temp += dL[count + 79];
                    temp += dL[count + 73];
                    temp += dL[count + 71];
                    temp += dL[count + 67];
                    temp += dL[count + 61];
                    temp += dL[count + 59];
                    temp += dL[count + 53];
                    temp += dL[count + 47];
                    temp += dL[count + 43];
                    temp += dL[count + 41];
                    temp += dL[count + 37];
                    temp += dL[count + 31];
                    temp += dL[count + 29];
                    temp /= 2;
                    temp += dL[count + 23];
                    temp += dL[count + 19];
                    temp += dL[count + 17];
                    temp += dL[count + 13];
                    temp += dL[count + 11];
                    temp /= 2;
                    temp += dL[count + 7];
                    temp += dL[count + 5];
                    temp += dL[count + 3];
                    temp /= 2;
                    temp += dL[count + 2];
                    temp += dL[count + 1]; // end L
                    slowIIRAL += (temp / 128);
                    temp = dR[count + 127];
                    temp += dR[count + 113];
                    temp += dR[count + 109];
                    temp += dR[count + 107];
                    temp += dR[count + 103];
                    temp += dR[count + 101];
                    temp += dR[count + 97];
                    temp += dR[count + 89];
                    temp += dR[count + 83];
                    temp /= 2;
                    temp += dR[count + 79];
                    temp += dR[count + 73];
                    temp += dR[count + 71];
                    temp += dR[count + 67];
                    temp += dR[count + 61];
                    temp += dR[count + 59];
                    temp += dR[count + 53];
                    temp += dR[count + 47];
                    temp += dR[count + 43];
                    temp += dR[count + 41];
                    temp += dR[count + 37];
                    temp += dR[count + 31];
                    temp += dR[count + 29];
                    temp /= 2;
                    temp += dR[count + 23];
                    temp += dR[count + 19];
                    temp += dR[count + 17];
                    temp += dR[count + 13];
                    temp += dR[count + 11];
                    temp /= 2;
                    temp += dR[count + 7];
                    temp += dR[count + 5];
                    temp += dR[count + 3];
                    temp /= 2;
                    temp += dR[count + 2];
                    temp += dR[count + 1]; // end R
                    slowIIRAR += (temp / 128);
                    inputSampleL = fastIIRAL - (slowIIRAL / slowTaper);
                    inputSampleR = fastIIRAR - (slowIIRAR / slowTaper);
                }
                else
                {
                    if (fabs(fastIIRBL) < 1.18e-37) {
                        fastIIRBL = 0.0;
                    }
                    if (fabs(slowIIRBL) < 1.18e-37) {
                        slowIIRBL = 0.0;
                    }
                    fastIIRBL = fastIIRBL / fastTaper;
                    slowIIRBL = slowIIRBL / slowTaper;
                    fastIIRBL += dL[count];
                    // scale stuff down
                    if (fabs(fastIIRBR) < 1.18e-37) {
                        fastIIRBR = 0.0;
                    }
                    if (fabs(slowIIRBR) < 1.18e-37) {
                        slowIIRBR = 0.0;
                    }
                    fastIIRBR = fastIIRBR / fastTaper;
                    slowIIRBR = slowIIRBR / slowTaper;
                    fastIIRBR += dR[count];
                    // scale stuff down
                    count += 3;
                    temp = dL[count + 127];
                    temp += dL[count + 113];
                    temp += dL[count + 109];
                    temp += dL[count + 107];
                    temp += dL[count + 103];
                    temp += dL[count + 101];
                    temp += dL[count + 97];
                    temp += dL[count + 89];
                    temp += dL[count + 83];
                    temp /= 2;
                    temp += dL[count + 79];
                    temp += dL[count + 73];
                    temp += dL[count + 71];
                    temp += dL[count + 67];
                    temp += dL[count + 61];
                    temp += dL[count + 59];
                    temp += dL[count + 53];
                    temp += dL[count + 47];
                    temp += dL[count + 43];
                    temp += dL[count + 41];
                    temp += dL[count + 37];
                    temp += dL[count + 31];
                    temp += dL[count + 29];
                    temp /= 2;
                    temp += dL[count + 23];
                    temp += dL[count + 19];
                    temp += dL[count + 17];
                    temp += dL[count + 13];
                    temp += dL[count + 11];
                    temp /= 2;
                    temp += dL[count + 7];
                    temp += dL[count + 5];
                    temp += dL[count + 3];
                    temp /= 2;
                    temp += dL[count + 2];
                    temp += dL[count + 1];
                    slowIIRBL += (temp / 128);
                    temp = dR[count + 127];
                    temp += dR[count + 113];
                    temp += dR[count + 109];
                    temp += dR[count + 107];
                    temp += dR[count + 103];
                    temp += dR[count + 101];
                    temp += dR[count + 97];
                    temp += dR[count + 89];
                    temp += dR[count + 83];
                    temp /= 2;
                    temp += dR[count + 79];
                    temp += dR[count + 73];
                    temp += dR[count + 71];
                    temp += dR[count + 67];
                    temp += dR[count + 61];
                    temp += dR[count + 59];
                    temp += dR[count + 53];
                    temp += dR[count + 47];
                    temp += dR[count + 43];
                    temp += dR[count + 41];
                    temp += dR[count + 37];
                    temp += dR[count + 31];
                    temp += dR[count + 29];
                    temp /= 2;
                    temp += dR[count + 23];
                    temp += dR[count + 19];
                    temp += dR[count + 17];
                    temp += dR[count + 13];
                    temp += dR[count + 11];
                    temp /= 2;
                    temp += dR[count + 7];
                    temp += dR[count + 5];
                    temp += dR[count + 3];
                    temp /= 2;
                    temp += dR[count + 2];
                    temp += dR[count + 1];
                    slowIIRBR += (temp / 128);
                    inputSampleL = fastIIRBL - (slowIIRBL / slowTaper);
                    inputSampleR = fastIIRBR - (slowIIRBR / slowTaper);
                }
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
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            // can use as an output limiter
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            // can use as an output limiter
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            if (biquadB[0] < 0.49999) {
                tempSample = biquadB[2] * inputSampleL + biquadB[3] * biquadB[7] + biquadB[4] * biquadB[8] - biquadB[5] * biquadB[9] - biquadB[6] * biquadB[10];
                biquadB[8] = biquadB[7];
                biquadB[7] = inputSampleL;
                inputSampleL = tempSample;
                biquadB[10] = biquadB[9];
                biquadB[9] = inputSampleL; // DF1 left
                tempSample = biquadB[2] * inputSampleR + biquadB[3] * biquadB[11] + biquadB[4] * biquadB[12] - biquadB[5] * biquadB[13] - biquadB[6] * biquadB[14];
                biquadB[12] = biquadB[11];
                biquadB[11] = inputSampleR;
                inputSampleR = tempSample;
                biquadB[14] = biquadB[13];
                biquadB[13] = inputSampleR; // DF1 right
            }
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
            flip = !flip;
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
} // namespace airwindohhs::ironoxideclassic2
