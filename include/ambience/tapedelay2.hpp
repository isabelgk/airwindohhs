#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapedelay2 {

constexpr std::string_view k_name{ "TapeDelay2" };
constexpr std::string_view k_short_description{
    "TapeDelay2 is a new implementation of a flexible tape echo."
};
constexpr std::string_view k_long_description{
    "TapeDelay2 gives you substantial changes over my original TapeDelay. It’s no longer trying to do the ‘Iron Oxide’ style tone shaping the original one did. In fact, it’s not even a delay in the same sense as its predecessor.Instead, it’s a brand new, undersampled, Airwindows-bandpassed monster ready to make a whole pile of very convincing sounds. As plugin delays go, this covers a big range of purposes.You’ve got a nice long delay time that’s still available at higher sample rates, thanks to the undersampling techniques. The delay line (and the regeneration) makes use of Airwindows bandpasses, but only on the undersampled content: meaning that if you’ve got it set to very nearly full range (resonance of zero) it’ll give the same subtle highpassing and lowpassing no matter what sample rate you’re at (a normal bandpass would have to roll off closer to the sample rate’s Nyquist frequency, in other words it would let through too many highs to do a proper tape emulation). You can tighten the bandwidth by increasing resonance. You can adjust the region you’re highlighting. And you can still adjust the frequency control even when set to full wide, which gives you more of a tilt EQ. It really turned out to have a lot of flexibility, and there’s two separate bandpasses so that you can shape the overall tone and also focus in on the regenerations if you like. Vintage sounds in the classic Airwindows way, meaning ‘no overprocessing, just high fidelity clean and simple processing’. Also, the dry/wet operates like my recent reverbs: 50% means full dry AND full wet, so you can bring in subtle echoes without altering the gain of your underlying track. Use it like a kind of reverb, with whatever tone and resonance works for you!And lastly, just to top it off, Tape Flutter. This is a new implementation that I’ve never tried before. Instead of a simple vibrato, in TapeDelay2 the flutter keys off the amplitude of the underlying track, making it a lot more wavery and irregular. Subtle effects are easily achieved in most settings, and crank it up for more of a warbly dirty-tape quality. It should be irregular enough to sound like real tape wobble. It’s done by modulating the tape speed… because unlike any previous Airwindows tape effect, TapeDelay2 works by taking a full-length tape loop and literally speeding it up, rather than trying to change the length of the delay in any way. So both the warble, and any manipulations you make to the delay time, act like messing with the pitch of a physical tape machine with a set record and playback head… which turns out to be the best way to do this :)This is one of the good ones. Hope ya like it!"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class TapeDelay2 final : public Effect<T>
{
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.0; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "time"; break;
            case kParamB: return "regen"; break;
            case kParamC: return "freq"; break;
            case kParamD: return "reso"; break;
            case kParamE: return "flutter"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Time"; break;
            case kParamB: return "Regen"; break;
            case kParamC: return "Freq"; break;
            case kParamD: return "Reso"; break;
            case kParamE: return "Flutter"; break;
            case kParamF: return "Dry/Wet"; break;

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
} // namespace airwindohhs::tapedelay2
