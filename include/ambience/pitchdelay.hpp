#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pitchdelay {

constexpr std::string_view k_name{ "PitchDelay" };
constexpr std::string_view k_short_description{
    "PitchDelay is TapeDelay2 but with pitch shift instead of flutter."
};
constexpr std::string_view k_long_description{
    "So I was at a a synthesizer meet-up talking with some folks and someone asked me if I could do a particular thing. I’d mentioned how my TapeDelay2, which I was about to post that very night, would let you wiggle around the speed control and you’d get crazy wobble JUST like if you were doing it on a tape deck, because of the way I ran the delay buffers. And the request was to make a plugin where the time would wobble but the pitch would not, perhaps using some pitch shifter algorithm.And… I did not make that thing! :D but I made this instead, and here you go!All this is, is TapeDelay2 but instead of the flutter control, it’s got a fixed pitch shift. It’s being done in a more normal way than Glitch Shifter, but for all that it still enables some silly and extreme noises. Everything not flutter/pitch knob related, is exactly the same: all my development time went towards making the pitch shift interesting.If you shift up, you can go towards very shrill crazy up-shifts that are right to the edge of blowing up the plugin. If you shift down, you can drop pitch down to literally nothing… and then keep going until you’re doing reverse buffer looping, which ends up (at a setting of 0) being the same pitch you started with, but backwards. Except it’s not playing the actual audio backwards, it’s cycling the algorithm backwards while the sound still plays ‘forwards’, so you get a ‘voice disguise’ effect. Sneak the setting just off the zero point, and it’s backwards low-speed, good for alien monster voices. If you include the regeneration while doing this you get a glorious mess. Also, the regeneration can be set to WAY more than just feedback, but it subtly restrains itself a bit so that you can hover around total feedback in a usable way. This combined with pitch shifting settings and the filter that comes with TapeDelay2 can give you a whole pile of strange, sorta-analogy noises without even putting more sounds in (it does require some sort of noise beyond digital black to start with, but once it’s going you’ll be able to play it like a weird instrument)Add this to your Tape Delay arsenal. It’s not part of TapeDelay because it’s weird enough to be its own purpose (dedicated plugins for purposes is more or less my thing). Hope you like it :)"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class PitchDelay final : public Effect<T>
{
    double dL[88211];
    double prevSampleL;
    double regenSampleL;
    double delayL;
    double sweepL;
    double regenFilterL[9];
    double outFilterL[9];
    double lastRefL[10];
    double dR[88211];
    double prevSampleR;
    double regenSampleR;
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
    PitchDelay()
    {
        A = 1.0;
        B = 0.0;
        C = 0.5;
        D = 0.0;
        E = 0.5;
        F = 1.0;
        for (int x = 0; x < 88210; x++) {
            dL[x] = 0.0;
            dR[x] = 0.0;
        }
        prevSampleL = 0.0;
        regenSampleL = 0.0;
        delayL = 0.0;
        sweepL = 0.0;
        prevSampleR = 0.0;
        regenSampleR = 0.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
            break;
        kParamF:
            F = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;
        kParamE:
            return E;
            break;
        kParamF:
            return F;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 0.0;
            break;
        kParamE:
            return 0.5;
            break;
        kParamF:
            return 1.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "time";
            break;
        kParamB:
            return "regen";
            break;
        kParamC:
            return "freq";
            break;
        kParamD:
            return "reso";
            break;
        kParamE:
            return "pitch";
            break;
        kParamF:
            return "dry/wet";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Time";
            break;
        kParamB:
            return "Regen";
            break;
        kParamC:
            return "Freq";
            break;
        kParamD:
            return "Reso";
            break;
        kParamE:
            return "Pitch";
            break;
        kParamF:
            return "Dry/Wet";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
            break;
        kParamF:
            return std::to_string(F);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;
        kParamE:
            return "";
            break;
        kParamF:
            return "";
            break;

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
        double baseSpeed = (pow(A, 4) * 20.0) + 1.0;
        double feedbackL = B * (3.0 - fabs(regenSampleL * 2.0));
        double feedbackR = B * (3.0 - fabs(regenSampleR * 2.0));
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
        double vibSpeed = (E - 0.5) * 61.8;
        double vibDepth = (fabs(vibSpeed) * 20.0 * baseSpeed) + 1.0;
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
                double speedL = baseSpeed;
                double speedR = baseSpeed;
                // begin left channel
                int pos = floor(delayL);
                double newSample = inputSampleL + (regenSampleL * feedbackL);
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
                sweepL += (0.0001 * vibSpeed);
                if (sweepL < 0.0) {
                    sweepL += 6.283185307179586;
                }
                if (sweepL > 6.283185307179586) {
                    sweepL -= 6.283185307179586;
                }
                double sweepOffsetL = sweepL + M_PI;
                if (sweepOffsetL > 6.283185307179586) {
                    sweepOffsetL -= 6.283185307179586;
                }
                double newTapA = delayL - (sweepL * vibDepth);
                if (newTapA < 0) {
                    newTapA += 88200.0;
                }
                double newTapB = delayL - (sweepOffsetL * vibDepth);
                if (newTapB < 0) {
                    newTapB += 88200.0;
                }
                double tapAmplitudeA = (sin(sweepL + (M_PI * 1.5)) + 1.0) * 0.25;
                double tapAmplitudeB = (sin(sweepOffsetL + (M_PI * 1.5)) + 1.0) * 0.25;
                pos = floor(newTapA);
                inputSampleL = dL[pos] * tapAmplitudeA;
                pos = floor(newTapB);
                inputSampleL += dL[pos] * tapAmplitudeB;
                regenSampleL = sin(inputSampleL);
                tempSample = (inputSampleL * outFilterL[2]) + outFilterL[7];
                outFilterL[7] = -(tempSample * outFilterL[5]) + outFilterL[8];
                outFilterL[8] = (inputSampleL * outFilterL[4]) - (tempSample * outFilterL[6]);
                inputSampleL = tempSample;
                // end left channel
                // begin right channel
                pos = floor(delayR);
                newSample = inputSampleR + (regenSampleR * feedbackR);
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
                sweepR += (0.0001 * vibSpeed);
                if (sweepR < 0.0) {
                    sweepR += 6.283185307179586;
                }
                if (sweepR > 6.283185307179586) {
                    sweepR -= 6.283185307179586;
                }
                double sweepOffsetR = sweepR + M_PI;
                if (sweepOffsetR > 6.283185307179586) {
                    sweepOffsetR -= 6.283185307179586;
                }
                newTapA = delayR - (sweepR * vibDepth);
                if (newTapA < 0) {
                    newTapA += 88200.0;
                }
                newTapB = delayR - (sweepOffsetR * vibDepth);
                if (newTapB < 0) {
                    newTapB += 88200.0;
                }
                tapAmplitudeA = (sin(sweepR + (M_PI * 1.5)) + 1.0) * 0.25;
                tapAmplitudeB = (sin(sweepOffsetR + (M_PI * 1.5)) + 1.0) * 0.25;
                pos = floor(newTapA);
                inputSampleR = dR[pos] * tapAmplitudeA;
                pos = floor(newTapB);
                inputSampleR += dR[pos] * tapAmplitudeB;
                regenSampleR = sin(inputSampleR);
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
} // namespace airwindohhs::pitchdelay
