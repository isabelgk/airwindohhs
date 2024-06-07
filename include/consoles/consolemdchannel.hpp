#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::consolemdchannel {

constexpr std::string_view k_name{ "ConsoleMDChannel" };
constexpr std::string_view k_short_description{
    "ConsoleMDChannel goes for the tone shaping of the classic MCI console!"
};
constexpr std::string_view k_long_description{
    "This has been a real journey, to get to this point. (see 'ConsoleMC' for more on that subject)ConsoleMD is replacing my fully analog mixing system, which I'm now looking to sell off. Technically, just the mixer, as I can re-use the Lavry for tracking, where I used to have it dedicated to re-capturing the mixdown from the Heritage Audio mixer. This is what it took, to retire that glorious beast.It's a channel strip that functions like a normal Console Channel, except it's got treble, a sweepable mid that only boosts (like the classic MCI mid boost), bass, a fader that runs before the EQ section like the classic MCI, and a special pan switching network that's like an evil cousin of LCR panning. And it's a summing buss that brings the summing character of the big MCI desks with a gnarly analog dirtiness that's not simply 'add a distortion box' but is actually a modification of other recent and unique work I've put out.ConsoleMD draws on the following recent plugins: Creature. NOT SubTight. Sinew. ResEQ2. Pear. BitShiftPan. There is not a single normal DSP algorithm in this thing unless you count sin() and asin() functions. It is ALL built out of current, 2023 Airwindows plugins that have all been pretty well received, sometimes with a fair bit of excitement. I told you all this was working up to something. This is it.Specifically, this is the first 'it' to come together: I've got at least five other big console concepts that deserve this treatment, but ConsoleMD is right here for you to jump into, right now. There's a couple reverbs in the demo, including kCathedral which is a call-out to the corresponding Bricasti patch. Those are for later.ConsoleMD is designed around running at 96k (or possibly 192k if you're so inclined) but ought to work at 44.1k. If you're at low sample rate and seek to oversample it 2x or 4x it shouldn't do it any harm.All of the EQ options are designed to be pristine when flat (mids, being boost only, are flat at 0.0 boost, not 0.5) and bring in multiple stages as you crank them harder and harder, whether it's the highs and mids getting increasingly saturated or the mids getting increasingly high-Q, allowing for everything from clarity to tonal extremeness thoroughly beyond what any real MCI console would do. This is not a clone, it's a tone in its own right, that tries to do the same job as some of the greatest big analog consoles ever to exist, but does it by the unforgiving and touchy rules of digital.I hope it brings you joy, and helps you get your sound as it helps me get mine."
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class ConsoleMDChannel final : public Effect<T>
{
    double pearA[18];
    double pearB[22];
    double mpkL[2005];
    double mpkR[2005];
    double f[66];
    double prevfreqMPeak;
    double prevamountMPeak;
    int mpc;
    double bassA;
    double bassB;
    double gainA;
    double gainB; // smoothed master fader for channel, from Z2 series filter code
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
    ConsoleMDChannel()
    {
        A = 0.5;
        B = 0.25;
        C = 0.0;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        for (int x = 0; x < 17; x++) {
            pearA[x] = 0.0;
        }
        for (int x = 0; x < 21; x++) {
            pearB[x] = 0.0;
        }
        for (int count = 0; count < 2004; count++) {
            mpkL[count] = 0.0;
            mpkR[count] = 0.0;
        }
        for (int count = 0; count < 65; count++) {
            f[count] = 0.0;
        }
        prevfreqMPeak = -1;
        prevamountMPeak = -1;
        mpc = 1;
        bassA = bassB = 0.0;
        gainA = gainB = 1.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.25; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "treble"; break;
            case kParamB: return "midfreq"; break;
            case kParamC: return "midpeak"; break;
            case kParamD: return "bass"; break;
            case kParamE: return "pan"; break;
            case kParamF: return "fader"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Treble"; break;
            case kParamB: return "MidFreq"; break;
            case kParamC: return "MidPeak"; break;
            case kParamD: return "Bass"; break;
            case kParamE: return "Pan"; break;
            case kParamF: return "Fader"; break;

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

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate(); // will be over 1.0848 when over 48k
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 3) {
            cycleEnd = 3;
        }
        double fatTreble = (A * 6.0) - 3.0;
        bassA = bassB;
        bassB = (D * 6.0) - 3.0;
        // these should stack to go up to -3.0 to 3.0
        if (fatTreble < 0.0) {
            fatTreble /= 3.0;
        }
        if (bassB < 0.0) {
            bassB /= 3.0;
        }
        // and then become -1.0 to 3.0;
        // there will be successive sin/cos stages w. dry/wet in these
        double freqTreble = 0.853;
        double freqMid = 0.026912;
        switch (cycleEnd)
        {
            case 1: // base sample rate, no change
                break;
            case 2: // 96k tier
                freqTreble = 0.4265;
                freqMid = 0.013456;
                break;
            case 3: // 192k tier
                freqTreble = 0.21325;
                freqMid = 0.006728;
                break;
        }
        // begin ResEQ2 Mid Boost
        double freqMPeak = pow(B + 0.16, 3);
        double amountMPeak = pow(C, 2);
        int maxMPeak = (amountMPeak * 63.0) + 1;
        if ((freqMPeak != prevfreqMPeak) || (amountMPeak != prevamountMPeak)) {
            for (int x = 0; x < maxMPeak; x++) {
                if (((double)x * freqMPeak) < M_PI_4) {
                    f[x] = sin(((double)x * freqMPeak) * 4.0) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
                else {
                    f[x] = cos((double)x * freqMPeak) * freqMPeak * sin(((double)(maxMPeak - x) / (double)maxMPeak) * M_PI_2);
                }
            }
            prevfreqMPeak = freqMPeak;
            prevamountMPeak = amountMPeak;
        } // end ResEQ2 Mid Boost
        int bitshiftL = 0;
        int bitshiftR = 0;
        double panControl = (E * 2.0) - 1.0; //-1.0 to 1.0
        double panAttenuation = (1.0 - fabs(panControl));
        int panBits = 20; // start centered
        if (panAttenuation > 0.0) {
            panBits = floor(1.0 / panAttenuation);
        }
        if (panControl > 0.25) {
            bitshiftL += panBits;
        }
        if (panControl < -0.25) {
            bitshiftR += panBits;
        }
        if (bitshiftL < 0) {
            bitshiftL = 0;
        }
        if (bitshiftL > 17) {
            bitshiftL = 17;
        }
        if (bitshiftR < 0) {
            bitshiftR = 0;
        }
        if (bitshiftR > 17) {
            bitshiftR = 17;
        }
        double gainL = 1.0;
        double gainR = 1.0;
        switch (bitshiftL)
        {
            case 17: gainL = 0.0; break;
            case 16: gainL = 0.0000152587890625; break;
            case 15: gainL = 0.000030517578125; break;
            case 14: gainL = 0.00006103515625; break;
            case 13: gainL = 0.0001220703125; break;
            case 12: gainL = 0.000244140625; break;
            case 11: gainL = 0.00048828125; break;
            case 10: gainL = 0.0009765625; break;
            case 9: gainL = 0.001953125; break;
            case 8: gainL = 0.00390625; break;
            case 7: gainL = 0.0078125; break;
            case 6: gainL = 0.015625; break;
            case 5: gainL = 0.03125; break;
            case 4: gainL = 0.0625; break;
            case 3: gainL = 0.125; break;
            case 2: gainL = 0.25; break;
            case 1: gainL = 0.5; break;
            case 0: break;
        }
        switch (bitshiftR)
        {
            case 17: gainR = 0.0; break;
            case 16: gainR = 0.0000152587890625; break;
            case 15: gainR = 0.000030517578125; break;
            case 14: gainR = 0.00006103515625; break;
            case 13: gainR = 0.0001220703125; break;
            case 12: gainR = 0.000244140625; break;
            case 11: gainR = 0.00048828125; break;
            case 10: gainR = 0.0009765625; break;
            case 9: gainR = 0.001953125; break;
            case 8: gainR = 0.00390625; break;
            case 7: gainR = 0.0078125; break;
            case 6: gainR = 0.015625; break;
            case 5: gainR = 0.03125; break;
            case 4: gainR = 0.0625; break;
            case 3: gainR = 0.125; break;
            case 2: gainR = 0.25; break;
            case 1: gainR = 0.5; break;
            case 0: break;
        }
        gainA = gainB;
        gainB = F * 2.0; // smoothed master fader from Z2 filters
        // BitShiftGain pre gain trim goes here
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
            double temp = (double)sampleFrames / inFramesToProcess;
            double gain = (gainA * temp) + (gainB * (1.0 - temp));
            double bass = (bassA * temp) + (bassB * (1.0 - temp));
            inputSampleL *= gain;
            inputSampleR *= gain;
            // for MCI consoles, the fader is before the EQ, which overdrives easily.
            // so we put the main fader here.
            // begin Pear filter stages
            double bassL = inputSampleL;
            double bassR = inputSampleR;
            double slew = ((bassL - pearA[0]) + pearA[1]) * freqTreble * 0.5;
            pearA[0] = bassL = (freqTreble * bassL) + ((1.0 - freqTreble) * (pearA[0] + pearA[1]));
            pearA[1] = slew;
            slew = ((bassR - pearA[2]) + pearA[3]) * freqTreble * 0.5;
            pearA[2] = bassR = (freqTreble * bassR) + ((1.0 - freqTreble) * (pearA[2] + pearA[3]));
            pearA[3] = slew;
            slew = ((bassL - pearA[4]) + pearA[5]) * freqTreble * 0.5;
            pearA[4] = bassL = (freqTreble * bassL) + ((1.0 - freqTreble) * (pearA[4] + pearA[5]));
            pearA[5] = slew;
            slew = ((bassR - pearA[6]) + pearA[7]) * freqTreble * 0.5;
            pearA[6] = bassR = (freqTreble * bassR) + ((1.0 - freqTreble) * (pearA[6] + pearA[7]));
            pearA[7] = slew;
            slew = ((bassL - pearA[8]) + pearA[9]) * freqTreble * 0.5;
            pearA[8] = bassL = (freqTreble * bassL) + ((1.0 - freqTreble) * (pearA[8] + pearA[9]));
            pearA[9] = slew;
            slew = ((bassR - pearA[10]) + pearA[11]) * freqTreble * 0.5;
            pearA[10] = bassR = (freqTreble * bassR) + ((1.0 - freqTreble) * (pearA[10] + pearA[11]));
            pearA[11] = slew;
            slew = ((bassL - pearA[12]) + pearA[13]) * freqTreble * 0.5;
            pearA[12] = bassL = (freqTreble * bassL) + ((1.0 - freqTreble) * (pearA[12] + pearA[13]));
            pearA[13] = slew;
            slew = ((bassR - pearA[14]) + pearA[15]) * freqTreble * 0.5;
            pearA[14] = bassR = (freqTreble * bassR) + ((1.0 - freqTreble) * (pearA[14] + pearA[15]));
            pearA[15] = slew;
            // unrolled mid/treble crossover (called bass to use fewer variables)
            double trebleL = inputSampleL - bassL;
            inputSampleL = bassL;
            double trebleR = inputSampleR - bassR;
            inputSampleR = bassR;
            // at this point 'bass' is actually still mid and bass
            slew = ((bassL - pearB[0]) + pearB[1]) * freqMid * 0.5;
            pearB[0] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (pearB[0] + pearB[1]));
            pearB[1] = slew;
            slew = ((bassR - pearB[2]) + pearB[3]) * freqMid * 0.5;
            pearB[2] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (pearB[2] + pearB[3]));
            pearB[3] = slew;
            slew = ((bassL - pearB[4]) + pearB[5]) * freqMid * 0.5;
            pearB[4] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (pearB[4] + pearB[5]));
            pearB[5] = slew;
            slew = ((bassR - pearB[6]) + pearB[7]) * freqMid * 0.5;
            pearB[6] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (pearB[6] + pearB[7]));
            pearB[7] = slew;
            slew = ((bassL - pearB[8]) + pearB[9]) * freqMid * 0.5;
            pearB[8] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (pearB[8] + pearB[9]));
            pearB[9] = slew;
            slew = ((bassR - pearB[10]) + pearB[11]) * freqMid * 0.5;
            pearB[10] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (pearB[10] + pearB[11]));
            pearB[11] = slew;
            slew = ((bassL - pearB[12]) + pearB[13]) * freqMid * 0.5;
            pearB[12] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (pearB[12] + pearB[13]));
            pearB[13] = slew;
            slew = ((bassR - pearB[14]) + pearB[15]) * freqMid * 0.5;
            pearB[14] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (pearB[14] + pearB[15]));
            pearB[15] = slew;
            slew = ((bassL - pearB[16]) + pearB[17]) * freqMid * 0.5;
            pearB[16] = bassL = (freqMid * bassL) + ((1.0 - freqMid) * (pearB[16] + pearB[17]));
            pearB[17] = slew;
            slew = ((bassR - pearB[18]) + pearB[19]) * freqMid * 0.5;
            pearB[18] = bassR = (freqMid * bassR) + ((1.0 - freqMid) * (pearB[18] + pearB[19]));
            pearB[19] = slew;
            double midL = inputSampleL - bassL;
            double midR = inputSampleR - bassR;
            // we now have three bands out of two pear filters
            double w = 0.0; // filter into bands, apply the sin/cos to each band
            if (fatTreble > 0.0) {
                w = fatTreble;
                if (w > 1.0) {
                    w = 1.0;
                }
                trebleL = (trebleL * (1.0 - w)) + (sin(trebleL * M_PI_2) * w);
                trebleR = (trebleR * (1.0 - w)) + (sin(trebleR * M_PI_2) * w);
                if (fatTreble > 1.0) {
                    w = fatTreble - 1.0;
                    if (w > 1.0) {
                        w = 1.0;
                    }
                    trebleL = (trebleL * (1.0 - w)) + (sin(trebleL * M_PI_2) * w);
                    trebleR = (trebleR * (1.0 - w)) + (sin(trebleR * M_PI_2) * w);
                    if (fatTreble > 2.0) {
                        w = fatTreble - 2.0;
                        trebleL = (trebleL * (1.0 - w)) + (sin(trebleL * M_PI_2) * w);
                        trebleR = (trebleR * (1.0 - w)) + (sin(trebleR * M_PI_2) * w);
                    } // sine stages for EQ or compression
                }
            }
            if (fatTreble < 0.0) {
                if (trebleL > 1.0) {
                    trebleL = 1.0;
                }
                if (trebleL < -1.0) {
                    trebleL = -1.0;
                }
                if (trebleR > 1.0) {
                    trebleR = 1.0;
                }
                if (trebleR < -1.0) {
                    trebleR = -1.0;
                }
                w = -fatTreble;
                if (w > 1.0) {
                    w = 1.0;
                }
                if (trebleL > 0) {
                    trebleL = (trebleL * (1.0 - w)) + ((1.0 - cos(trebleL)) * sin(w));
                }
                else {
                    trebleL = (trebleL * (1.0 - w)) + ((-1.0 + cos(-trebleL)) * sin(w));
                }
                if (trebleR > 0) {
                    trebleR = (trebleR * (1.0 - w)) + ((1.0 - cos(trebleR)) * sin(w));
                }
                else {
                    trebleR = (trebleR * (1.0 - w)) + ((-1.0 + cos(-trebleR)) * sin(w));
                }
            } // cosine stages for EQ or expansion
            // begin ResEQ2 Mid Boost
            mpc++;
            if (mpc < 1 || mpc > 2001) {
                mpc = 1;
            }
            mpkL[mpc] = midL;
            mpkR[mpc] = midR;
            double midMPeakL = 0.0;
            double midMPeakR = 0.0;
            for (int x = 0; x < maxMPeak; x++) {
                int y = x * cycleEnd;
                switch (cycleEnd)
                {
                    case 1:
                        midMPeakL += (mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        midMPeakR += (mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]);
                        break;
                    case 2:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.5);
                        break;
                    case 3:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.333);
                        break;
                    case 4:
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        y--;
                        midMPeakL += ((mpkL[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25);
                        midMPeakR += ((mpkR[(mpc - y) + ((mpc - y < 1) ? 2001 : 0)] * f[x]) * 0.25); // break
                }
            }
            midL = (midMPeakL * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? midL : midL * (1.5 - amountMPeak));
            midR = (midMPeakR * amountMPeak) + ((1.5 - amountMPeak > 1.0) ? midR : midR * (1.5 - amountMPeak));
            // end ResEQ2 Mid Boost
            if (bassL > 1.0) {
                bassL = 1.0;
            }
            if (bassL < -1.0) {
                bassL = -1.0;
            }
            if (bassR > 1.0) {
                bassR = 1.0;
            }
            if (bassR < -1.0) {
                bassR = -1.0;
            }
            if (bass > 0.0) {
                w = bass;
                if (w > 1.0) {
                    w = 1.0;
                }
                bassL = (bassL * (1.0 - w)) + (sin(bassL * M_PI_2) * w * 1.6);
                bassR = (bassR * (1.0 - w)) + (sin(bassR * M_PI_2) * w * 1.6);
                if (bass > 1.0) {
                    w = bass - 1.0;
                    if (w > 1.0) {
                        w = 1.0;
                    }
                    bassL = (bassL * (1.0 - w)) + (sin(bassL * M_PI_2) * w * 1.4);
                    bassR = (bassR * (1.0 - w)) + (sin(bassR * M_PI_2) * w * 1.4);
                    if (bass > 2.0) {
                        w = bass - 2.0;
                        bassL = (bassL * (1.0 - w)) + (sin(bassL * M_PI_2) * w * 1.2);
                        bassR = (bassR * (1.0 - w)) + (sin(bassR * M_PI_2) * w * 1.2);
                    } // sine stages for EQ or compression
                }
            }
            if (bass < 0.0) {
                w = -bass;
                if (w > 1.0) {
                    w = 1.0;
                }
                if (bassL > 0) {
                    bassL = (bassL * (1.0 - w)) + ((1.0 - cos(bassL)) * sin(w));
                }
                else {
                    bassL = (bassL * (1.0 - w)) + ((-1.0 + cos(-bassL)) * sin(w));
                }
                if (bassR > 0) {
                    bassR = (bassR * (1.0 - w)) + ((1.0 - cos(bassR)) * sin(w));
                }
                else {
                    bassR = (bassR * (1.0 - w)) + ((-1.0 + cos(-bassR)) * sin(w));
                }
            } // cosine stages for EQ or expansion
            // the sin() is further restricting output when fully attenuated
            inputSampleL = (bassL + midL + trebleL) * gainL;
            inputSampleR = (bassR + midR + trebleR) * gainR;
            // applies BitShiftPan pan section
            // begin sin() style Channel processing
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
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
} // namespace airwindohhs::consolemdchannel
