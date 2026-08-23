#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::channelx {

constexpr std::string_view k_name{ "ChannelX" };
constexpr std::string_view k_short_description{
    "ChannelX translates Channel9 into a profusion of wild experiments."
};
constexpr std::string_view k_long_description{
    "Everything's the same, and nothing is the same.It's quite hard to tell what will happen with this plugin update. To surface appearances it's exactly the same as Console9, right down to the slightly odd choices like a (lower slope) interleaved IIR filter for highpassing. The controls and what they do are exactly the same. It still has named 'modes' it operates in, still has a drive control that produces the Spiral saturator at 100% and Density at 200% and crossfades from dry to Spiral to Density, linearly. This is what Channel9 has had since 2021.It's not 2021 anymore… and Chris from Airwindows has been responding to the pressure of vibecode guys flooding the plugin market by trying new stuff more often… based on the existing Airwindows codebase, and pretty much nothing else. And Channel's number came up. (technically, letter, since X can be used for 10)Longhand was an experiment in using a Taylor series to make a sine function 'by hand' in case building it out of a single, continuous algorithm sounded better than the stock, optimized form. It seemed to sound nice. Both the sine part, and the Spiral part of ChannelX are now made that way, longhand, at very high precision.RetroBass came out just last week! It involves modulating even harmonics into the sound. ChannelX now includes this in its highpasses, but still uses the dielectric modulation from Capacitor, and also still uses interleaved filters, which RetroBass does NOT use.A stack of other plugins are being updated with an experiment I did inspired by elliptic BLEP antialiasing! But where that is established as a way to flawlessly synthesize a filtered result to any degree of high-order filter ripple, my experiments veered off to produce a tweak that operates over ONE sample, not to delete aliasing but to doctor it in such a way that high frequencies remain locked in place and do not 'come forward' in the way aliased highs seem to do. Totally other thing, seems to work, zero ripple or blur or SRC-like qualities. The directness is incredible. (people don't seem to go for this quality very often in 2026, which might be as good a reason as any to pursue it)So ChannelX is a wild mix of very new experiments under a familiar surface. There have been times I freaked out over how good it sounded compared to Channel9. With a bit of luck this bodes well for new plugins coming up. We shall see."
};
constexpr std::string_view k_tags{
    "tone-color"
};

template <typename T>
class ChannelX final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'chaX';
    float A;
    float B;
    float C;
    double rbSampleLA;
    double rbSampleRA;
    double rbSampleLB;
    double rbSampleRB;
    double lastSampleLA, lastSampleLB, lastSampleLC;
    double lastSampleRA, lastSampleRB, lastSampleRC;
    bool flip;
    enum
    {
        bip_dvLA,
        bip_dvLB,
        bip_dvLC,
        bip_dvLD,
        bip_pvLA,
        bip_pvLB,
        bip_pvLC,
        bip_pvLD,
        bip_dvRA,
        bip_dvRB,
        bip_dvRC,
        bip_dvRD,
        bip_pvRA,
        bip_pvRB,
        bip_pvRC,
        bip_pvRD,
        bip_drbLA,
        bip_drbLB,
        bip_drbLC,
        bip_drbLD,
        bip_praLA,
        bip_praLB,
        bip_praLC,
        bip_praLD,
        bip_prbLA,
        bip_prbLB,
        bip_prbLC,
        bip_prbLD,
        bip_drbRA,
        bip_drbRB,
        bip_drbRC,
        bip_drbRD,
        bip_praRA,
        bip_praRB,
        bip_praRC,
        bip_praRD,
        bip_prbRA,
        bip_prbRB,
        bip_prbRC,
        bip_prbRD,
        bip_total // each distortion section can have one of these, it stacks well
    }; // not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ChannelX()
    {
        A = 0.0;
        B = 0.0;
        C = 1.0;
        rbSampleLA = 0.0;
        rbSampleRA = 0.0;
        rbSampleLB = 0.0;
        rbSampleRB = 0.0;
        lastSampleLA = lastSampleLB = lastSampleLC = 0.0;
        lastSampleRA = lastSampleRB = lastSampleRC = 0.0;
        flip = false;
        for (int x = 0; x < bip_total; x++) {
            bip[x] = 0.0;
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
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "console type"; break;
            case kParamB: return "drive"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Console Type"; break;
            case kParamB: return "Drive"; break;
            case kParamC: return "Output"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return "%"; break;
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
        double density = B * 2.0; // 0-2
        double phattity = density - 1.0;
        if (density > 1.0) {
            density = 1.0; // max out at full wet for Spiral aspect
        }
        if (phattity < 0.0) {
            phattity = 0.0; //
        }
        double nonLin = 5.0 - density; // number is smaller for more intense, larger for more subtle
        double output = C;
        double rbAmount = 0.005832;
        double threshold = 0.33362176; // instantiating with Neve values
        switch ((int)(A * 4.999))
        {
            case 0:
                rbAmount = 0.005832;
                threshold = 0.33362176;
                break; // Neve
            case 1:
                rbAmount = 0.004096;
                threshold = 0.59969536;
                break; // API
            case 2:
                rbAmount = 0.004913;
                threshold = 0.84934656;
                break; // SSL
            case 3:
                rbAmount = 0.009216;
                threshold = 0.149;
                break; // Teac
            case 4:
                rbAmount = 0.011449;
                threshold = 0.092;
                break; // Mackie
        }
        rbAmount /= overallscale; // we've learned not to try and adjust threshold for sample rate
        // also can't set and forget this as we're updating it in place
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
            double dielectricScale = fabs(2.0 - ((inputSampleL + nonLin) / nonLin));
            if (flip) {
                rbSampleLA = (rbSampleLA * (1.0 - (rbAmount * dielectricScale))) + (inputSampleL * rbAmount * dielectricScale);
                if (fabs(rbSampleLA) > 1.18e-37) {
                    double bip_delta = inputSampleL; // delta can be just local and re-used
                    inputSampleL *= rbSampleLA * 0.96;
                    inputSampleL = fma(((inputSampleL * inputSampleL) * -0.166666666666666), (inputSampleL * inputSampleL), inputSampleL);
                    inputSampleL /= rbSampleLA * 0.96;
                    bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
                    bip[bip_drbLB] = bip[bip_praLA] - bip[bip_drbLA];
                    bip[bip_praLA] = bip[bip_drbLA]; // velocity
                    bip[bip_drbLC] = bip[bip_praLB] - bip[bip_drbLB];
                    bip[bip_praLB] = bip[bip_drbLB]; // acceleration
                    bip[bip_drbLD] = bip[bip_praLC] - bip[bip_drbLC];
                    bip[bip_praLC] = bip[bip_drbLC]; // jerk
                    double bip_drbE = bip[bip_praLD] - bip[bip_drbLD];
                    bip[bip_praLD] = bip[bip_drbLD]; // snap
                    inputSampleL *= (1.0 + (fabs(bip[bip_drbLC]) * 0.0618) + (fabs(bip[bip_drbLD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
                }
                inputSampleL -= rbSampleLA * 0.92;
            }
            else {
                rbSampleLB = (rbSampleLB * (1.0 - (rbAmount * dielectricScale))) + (inputSampleL * rbAmount * dielectricScale);
                if (fabs(rbSampleLB) > 1.18e-37) {
                    double bip_delta = inputSampleL; // delta can be just local and re-used
                    inputSampleL *= rbSampleLB * 0.96;
                    inputSampleL = fma(((inputSampleL * inputSampleL) * -0.166666666666666), (inputSampleL * inputSampleL), inputSampleL);
                    inputSampleL /= rbSampleLB * 0.96;
                    bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
                    bip[bip_drbLB] = bip[bip_prbLA] - bip[bip_drbLA];
                    bip[bip_prbLA] = bip[bip_drbLA]; // velocity
                    bip[bip_drbLC] = bip[bip_prbLB] - bip[bip_drbLB];
                    bip[bip_prbLB] = bip[bip_drbLB]; // acceleration
                    bip[bip_drbLD] = bip[bip_prbLC] - bip[bip_drbLC];
                    bip[bip_prbLC] = bip[bip_drbLC]; // jerk
                    double bip_drbE = bip[bip_prbLD] - bip[bip_drbLD];
                    bip[bip_prbLD] = bip[bip_drbLD]; // snap
                    inputSampleL *= (1.0 + (fabs(bip[bip_drbLC]) * 0.0618) + (fabs(bip[bip_drbLD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
                }
                inputSampleL -= rbSampleLB * 0.92;
            }
            // highpass section L including capacitor modeling nonlinearity
            dielectricScale = fabs(2.0 - ((inputSampleR + nonLin) / nonLin));
            if (flip) {
                rbSampleRA = (rbSampleRA * (1.0 - (rbAmount * dielectricScale))) + (inputSampleR * rbAmount * dielectricScale);
                if (fabs(rbSampleRA) > 1.18e-37) {
                    double bip_delta = inputSampleR; // delta can be just local and re-used
                    inputSampleR *= rbSampleRA * 0.96;
                    inputSampleR = fma(((inputSampleR * inputSampleR) * -0.166666666666666), (inputSampleR * inputSampleR), inputSampleR);
                    inputSampleR /= rbSampleRA * 0.96;
                    bip[bip_drbRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
                    bip[bip_drbRB] = bip[bip_praRA] - bip[bip_drbRA];
                    bip[bip_praRA] = bip[bip_drbRA]; // velocity
                    bip[bip_drbRC] = bip[bip_praRB] - bip[bip_drbRB];
                    bip[bip_praRB] = bip[bip_drbRB]; // acceleration
                    bip[bip_drbRD] = bip[bip_praRC] - bip[bip_drbRC];
                    bip[bip_praRC] = bip[bip_drbRC]; // jerk
                    double bip_drbE = bip[bip_praRD] - bip[bip_drbRD];
                    bip[bip_praRD] = bip[bip_drbRD]; // snap
                    inputSampleR *= (1.0 + (fabs(bip[bip_drbRC]) * 0.0618) + (fabs(bip[bip_drbRD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
                }
                inputSampleR -= rbSampleRA * 0.92;
            }
            else {
                rbSampleRB = (rbSampleRB * (1.0 - (rbAmount * dielectricScale))) + (inputSampleR * rbAmount * dielectricScale);
                if (fabs(rbSampleRB) > 1.18e-37) {
                    double bip_delta = inputSampleR; // delta can be just local and re-used
                    inputSampleR *= rbSampleRB * 0.96;
                    inputSampleR = fma(((inputSampleR * inputSampleR) * -0.166666666666666), (inputSampleR * inputSampleR), inputSampleR);
                    inputSampleR /= rbSampleRB * 0.96;
                    bip[bip_drbRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
                    bip[bip_drbRB] = bip[bip_prbRA] - bip[bip_drbRA];
                    bip[bip_prbRA] = bip[bip_drbRA]; // velocity
                    bip[bip_drbRC] = bip[bip_prbRB] - bip[bip_drbRB];
                    bip[bip_prbRB] = bip[bip_drbRB]; // acceleration
                    bip[bip_drbRD] = bip[bip_prbRC] - bip[bip_drbRC];
                    bip[bip_prbRC] = bip[bip_drbRC]; // jerk
                    double bip_drbE = bip[bip_prbRD] - bip[bip_drbRD];
                    bip[bip_prbRD] = bip[bip_drbRD]; // snap
                    inputSampleR *= (1.0 + (fabs(bip[bip_drbRC]) * 0.0618) + (fabs(bip[bip_drbRD]) * -0.05982) + (fabs(bip_drbE) * 0.0206));
                }
                inputSampleR -= rbSampleRB * 0.92;
            }
            // highpass section R including capacitor modeling nonlinearity
            double bip_delta = inputSampleL; // delta can be just local and re-used
            double drySample = inputSampleL;
            inputSampleL = fmax(fmin(inputSampleL, 1.0), -1.0);
            long double phatSample = inputSampleL * 1.57079633; // phatSample is sin()
            long double X = phatSample;
            X *= X; // long double for even
            long double temp = phatSample * X; // the initial multiplies
            phatSample -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            phatSample += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            phatSample -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            phatSample += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            phatSample -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            phatSample += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            phatSample -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            phatSample += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            phatSample -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            phatSample += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            inputSampleL *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            long double distSample = inputSampleL * fabs(inputSampleL); // distSample is Spiral
            X = distSample;
            X *= X; // long double for even
            temp = distSample * X; // the initial multiplies
            distSample -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            distSample += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            distSample -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            distSample += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            distSample -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            distSample += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            distSample -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            distSample += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            distSample -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            distSample += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            distSample /= ((fabs(inputSampleL) == 0.0) ? 1.0 : fabs(inputSampleL));
            inputSampleL = distSample; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleL = (drySample * (1 - density)) + (distSample * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleL = (inputSampleL * (1 - phattity)) + (phatSample * phattity); // apply original Density on top
            }
            bip[bip_dvLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA] - bip[bip_dvLA];
            bip[bip_pvLA] = bip[bip_dvLA]; // velocity
            bip[bip_dvLC] = bip[bip_pvLB] - bip[bip_dvLB];
            bip[bip_pvLB] = bip[bip_dvLB]; // acceleration
            bip[bip_dvLD] = bip[bip_pvLC] - bip[bip_dvLC];
            bip[bip_pvLC] = bip[bip_dvLC]; // jerk
            double bip_dvE = bip[bip_pvLD] - bip[bip_dvLD];
            bip[bip_pvLD] = bip[bip_dvLD]; // snap
            inputSampleL *= (1.0 + (fabs(bip[bip_dvLC]) * 0.0618) + (fabs(bip[bip_dvLD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
            bip_delta = inputSampleR; // delta can be just local and re-used
            drySample = inputSampleR;
            inputSampleR = fmax(fmin(inputSampleR, 1.0), -1.0);
            phatSample = inputSampleR * 1.57079633; // phatSample is sin()
            X = phatSample;
            X *= X; // long double for even
            temp = phatSample * X; // the initial multiplies
            phatSample -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            phatSample += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            phatSample -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            phatSample += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            phatSample -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            phatSample += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            phatSample -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            phatSample += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            phatSample -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            phatSample += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            inputSampleR *= 1.2533141373155;
            // clip to 1.2533141373155 to reach maximum output, or 1.57079633 for pure sine 'phat' version
            distSample = inputSampleR * fabs(inputSampleR); // distSample is Spiral
            X = distSample;
            X *= X; // long double for even
            temp = distSample * X; // the initial multiplies
            distSample -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            distSample += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            distSample -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            distSample += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            distSample -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            distSample += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            distSample -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            distSample += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            distSample -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            distSample += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            distSample /= ((fabs(inputSampleR) == 0.0) ? 1.0 : fabs(inputSampleR));
            inputSampleR = distSample; // purest form is full Spiral
            if (density < 1.0) {
                inputSampleR = (drySample * (1 - density)) + (distSample * density); // fade Spiral aspect
            }
            if (phattity > 0.0) {
                inputSampleR = (inputSampleR * (1 - phattity)) + (phatSample * phattity); // apply original Density on top
            }
            bip[bip_dvRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA] - bip[bip_dvRA];
            bip[bip_pvRA] = bip[bip_dvRA]; // velocity
            bip[bip_dvRC] = bip[bip_pvRB] - bip[bip_dvRB];
            bip[bip_pvRB] = bip[bip_dvRB]; // acceleration
            bip[bip_dvRD] = bip[bip_pvRC] - bip[bip_dvRC];
            bip[bip_pvRC] = bip[bip_dvRC]; // jerk
            bip_dvE = bip[bip_pvRD] - bip[bip_dvRD];
            bip[bip_pvRD] = bip[bip_dvRD]; // snap
            inputSampleR *= (1.0 + (fabs(bip[bip_dvRC]) * 0.0618) + (fabs(bip[bip_dvRD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
            double clamp = (lastSampleLB - lastSampleLC) * 0.381966011250105;
            clamp -= (lastSampleLA - lastSampleLB) * 0.6180339887498948482045;
            clamp += inputSampleL - lastSampleLA; // regular slew clamping added
            lastSampleLC = lastSampleLB;
            lastSampleLB = lastSampleLA;
            lastSampleLA = inputSampleL; // now our output relates off lastSampleB
            if (clamp > threshold) {
                inputSampleL = lastSampleLB + threshold;
            }
            if (-clamp > threshold) {
                inputSampleL = lastSampleLB - threshold;
            }
            // slew section
            lastSampleLA = (lastSampleLA * 0.381966011250105) + (inputSampleL * 0.6180339887498948482045);
            // split the difference between raw and smoothed for buffer
            clamp = (lastSampleRB - lastSampleRC) * 0.381966011250105;
            clamp -= (lastSampleRA - lastSampleRB) * 0.6180339887498948482045;
            clamp += inputSampleR - lastSampleRA; // regular slew clamping added
            lastSampleRC = lastSampleRB;
            lastSampleRB = lastSampleRA;
            lastSampleRA = inputSampleR; // now our output relates off lastSampleB
            if (clamp > threshold) {
                inputSampleR = lastSampleRB + threshold;
            }
            if (-clamp > threshold) {
                inputSampleR = lastSampleRB - threshold;
            }
            // slew section
            lastSampleRA = (lastSampleRA * 0.381966011250105) + (inputSampleR * 0.6180339887498948482045);
            // split the difference between raw and smoothed for buffer
            flip = !flip;
            if (output < 1.0)
            {
                inputSampleL *= output;
                inputSampleR *= output;
            }
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::channelx
