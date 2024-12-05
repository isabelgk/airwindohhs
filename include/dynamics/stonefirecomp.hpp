#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::stonefirecomp {

constexpr std::string_view k_name{ "StoneFireComp" };
constexpr std::string_view k_short_description{
    "StoneFireComp is some of the dynamics for ConsoleX."
};
constexpr std::string_view k_long_description{
    "This is new.People have had compressors for a while (and you've just got a new one in Pop3). And people have had multiband compressors, and used them for both good and nefarious purposes. And technically, people have had Kalman filters, mostly because I made some and have had them out there for a while, both in the basic form and the 'air band' form I use for Air4.But KALMAN multiband compression?And yet, here we are. This is probably the trickiest aspect of ConsoleX, put out so you can come to terms with it (or, indeed, make a mess and a bunch of strange noises, while having no idea what's going on: that's cool too!)This is two Pop3 compressors, run as a multiband filter, except the bandsplitter is a Kalman filter so it doesn't really work in terms of 'frequencies'. It works in terms of 'isolating sound qualities' and splits that, and then when you assemble the split audio again, if it balances perfectly you get the original sound back.But if it doesn't balance, because for instance you've compressed both the parts in different ways? Well, that's when things get interesting.You can isolate each part using the Fire and Stone controls: like in ConsoleX, they're unity gain at 0.5 and can be used to apply makeup gain to the compression. You can set the range control quite low so that Stone is basically kick and rumble, or quite high so that Fire is basically about brightness, or you can take a middle setting and fully use the strangeness of the crossover and the way it digs into transients and produces midrange in non-harmonic ways. Again, if you reassemble the parts perfectly, the strangeness goes away. But where's the fun in that?I can only bring this much to Airwindows Consolidated, restricted to ten controls. ConsoleX is this plus gating and an air band (uncompressed) and separate ratios for all parts of both bands, plus sidechain EQ flavor boosts that are also uncompressed. This uses the same ratio control for both bands, and that's your secret for dialing back the strangeness and regaining control: you can crank Ratio to hear what StoneFireComp is doing, which could be nearly anything, and then turn it right down to apply appropriate amounts of your effect. In ConsoleX you do that and can also let through 'peeks' (peaks) of heavily filtered raw sound by way of equalization in the normal sense.In line with my desire to make stuff that serves new purposes, that can be used for new musics and new kinds of sound, I honestly don't have that much advice on how to 'make classic sounds' using it. Or even, what sounds are good from it. I do know that using it subtly, with low ratio, should work better on the 2-buss, and that different sounds will end up needing different treatment, perhaps very different. But I don't know what will sound great yet, just that I suspect this can really find some interesting uses. Remember, Stone and Fire are distinct textures that recombine into the original sound perfectly, so combining their qualities with the qualities of compression should get you interesting results. Keeping settings similar will help it act 'normal'. Taking wild departures… won't.Have fun, because ConsoleX is coming (though there is still a lot of work to do)."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class StoneFireComp final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;
    float J;
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
    double fireCompL;
    double fireCompR;
    double stoneCompL;
    double stoneCompR;

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    StoneFireComp()
    {
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 1.0;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 1.0;
        for (int x = 0; x < kal_total; x++) {
            kal[x] = 0.0;
        }
        fireCompL = 1.0;
        fireCompR = 1.0;
        stoneCompL = 1.0;
        stoneCompR = 1.0;
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
        kParamF = 5,
        kParamG = 6,
        kParamH = 7,
        kParamI = 8,
        kParamJ = 9,
        kNumParameters = 10
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
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            case kParamI: I = value; break;
            case kParamJ: J = value; break;

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
            case kParamF: return F; break;
            case kParamG: return G; break;
            case kParamH: return H; break;
            case kParamI: return I; break;
            case kParamJ: return J; break;

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
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "fire th"; break;
            case kParamB: return "attack"; break;
            case kParamC: return "release"; break;
            case kParamD: return "fire"; break;
            case kParamE: return "stoneth"; break;
            case kParamF: return "attack"; break;
            case kParamG: return "release"; break;
            case kParamH: return "stone"; break;
            case kParamI: return "range"; break;
            case kParamJ: return "ratio"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Fire Th"; break;
            case kParamB: return "Attack"; break;
            case kParamC: return "Release"; break;
            case kParamD: return "Fire"; break;
            case kParamE: return "StoneTh"; break;
            case kParamF: return "Attack"; break;
            case kParamG: return "Release"; break;
            case kParamH: return "Stone"; break;
            case kParamI: return "Range"; break;
            case kParamJ: return "Ratio"; break;

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
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;
            case kParamI: return std::to_string(I); break;
            case kParamJ: return std::to_string(J); break;

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
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return ""; break;
            case kParamI: return ""; break;
            case kParamJ: return ""; break;

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
        double compFThresh = pow(A, 4);
        double compFAttack = 1.0 / (((pow(B, 3) * 5000.0) + 500.0) * overallscale);
        double compFRelease = 1.0 / (((pow(C, 5) * 50000.0) + 500.0) * overallscale);
        double fireGain = D * 2.0;
        fireGain = pow(fireGain, 3);
        double firePad = fireGain;
        if (firePad > 1.0) {
            firePad = 1.0;
        }
        double compSThresh = pow(E, 4);
        double compSAttack = 1.0 / (((pow(F, 3) * 5000.0) + 500.0) * overallscale);
        double compSRelease = 1.0 / (((pow(G, 5) * 50000.0) + 500.0) * overallscale);
        double stoneGain = H * 2.0;
        stoneGain = pow(stoneGain, 3);
        double stonePad = stoneGain;
        if (stonePad > 1.0) {
            stonePad = 1.0;
        }
        double kalman = 1.0 - (pow(I, 2) / overallscale);
        // crossover frequency between mid/bass
        double compRatio = 1.0 - pow(1.0 - J, 2);
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
            // begin KalmanL
            double fireL = inputSampleL;
            double temp = inputSampleL = inputSampleL * (1.0 - kalman) * 0.777;
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
            double stoneL = kal[kalOutL] * 0.777;
            fireL -= stoneL;
            // end KalmanL
            // begin KalmanR
            double fireR = inputSampleR;
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
            double stoneR = kal[kalOutR] * 0.777;
            fireR -= stoneR;
            // end KalmanR
            // fire dynamics
            if (fabs(fireL) > compFThresh) { // compression L
                fireCompL -= (fireCompL * compFAttack);
                fireCompL += ((compFThresh / fabs(fireL)) * compFAttack);
            }
            else {
                fireCompL = (fireCompL * (1.0 - compFRelease)) + compFRelease;
            }
            if (fabs(fireR) > compFThresh) { // compression R
                fireCompR -= (fireCompR * compFAttack);
                fireCompR += ((compFThresh / fabs(fireR)) * compFAttack);
            }
            else {
                fireCompR = (fireCompR * (1.0 - compFRelease)) + compFRelease;
            }
            if (fireCompL > fireCompR) {
                fireCompL -= (fireCompL * compFAttack);
            }
            if (fireCompR > fireCompL) {
                fireCompR -= (fireCompR * compFAttack);
            }
            fireCompL = fmax(fmin(fireCompL, 1.0), 0.0);
            fireCompR = fmax(fmin(fireCompR, 1.0), 0.0);
            fireL *= (((1.0 - compRatio) * firePad) + (fireCompL * compRatio * fireGain));
            fireR *= (((1.0 - compRatio) * firePad) + (fireCompR * compRatio * fireGain));
            // stone dynamics
            if (fabs(stoneL) > compSThresh) { // compression L
                stoneCompL -= (stoneCompL * compSAttack);
                stoneCompL += ((compSThresh / fabs(stoneL)) * compSAttack);
            }
            else {
                stoneCompL = (stoneCompL * (1.0 - compSRelease)) + compSRelease;
            }
            if (fabs(stoneR) > compSThresh) { // compression R
                stoneCompR -= (stoneCompR * compSAttack);
                stoneCompR += ((compSThresh / fabs(stoneR)) * compSAttack);
            }
            else {
                stoneCompR = (stoneCompR * (1.0 - compSRelease)) + compSRelease;
            }
            if (stoneCompL > stoneCompR) {
                stoneCompL -= (stoneCompL * compSAttack);
            }
            if (stoneCompR > stoneCompL) {
                stoneCompR -= (stoneCompR * compSAttack);
            }
            stoneCompL = fmax(fmin(stoneCompL, 1.0), 0.0);
            stoneCompR = fmax(fmin(stoneCompR, 1.0), 0.0);
            stoneL *= (((1.0 - compRatio) * stonePad) + (stoneCompL * compRatio * stoneGain));
            stoneR *= (((1.0 - compRatio) * stonePad) + (stoneCompR * compRatio * stoneGain));
            inputSampleL = stoneL + fireL;
            inputSampleR = stoneR + fireR;
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
} // namespace airwindohhs::stonefirecomp
