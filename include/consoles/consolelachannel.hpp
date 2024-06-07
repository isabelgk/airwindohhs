#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::consolelachannel {

constexpr std::string_view k_name{ "ConsoleLAChannel" };
constexpr std::string_view k_short_description{
    "ConsoleLAChannel is the Airwindows take on the Quad Eight console."
};
constexpr std::string_view k_long_description{
    "APIs and Neves and MCI, oh my… well, ConsoleLA emulates the most incredible console you've never heard of. This thing was the sound of LA in the seventies. It's on the back cover of Steely Dan records, it's been seen in Tom Scholz's basement, and it's heard even on classic 70s LA and Hollywood and San Francisco records it didn't make… because it's the production model of a whole lineage of West Coast custom recording consoles with similar designs and circuitry. And you can still get them, apparently, the company lives… but I don't know where, or for how much, or how, as there's no sign of prices or any way to get them. There's none on Reverb, Vintage King doesn't have any, good luck even finding channel strips…Meet the Quad Eight.This sound echoes through the Seventies. Tons of Steely Dan, tons of Zappa, Grateful Dead, Neil Young, Joni Mitchell… that's not even counting the wilder sightings, like when people figured this console mixed The Wall, or hearing it on Yes's Relayer album. The thing is, this was THE big production console before the days of Neve and API and SSL. Quad Eight were the first to go into production and come up with a recording console that you could just buy, set up, and use. Before them, you had to build your own.Quad Eight were (are, since the website says they can still make you gear) supplying the film industry, and it shows. These desks are amazing at making movies for your ears. (in the Zappa phrase, on the Hot Rats album, which was done at the Whitney studios in Glendale the year before they got a Neve, so that album was likely also Quad Eight or a kindred West Coast console)ConsoleLA, like ConsoleMC before it, is a bit of a different approach to emulating these great old dinosaurs, some still things you could conceivably find and have, some forever lost. The thing is, this is the nearest thing to custom point-to-point wiring of discrete transistors. On top of that, the Quad Eight ran a higher supply voltage than anybody: negative 28 to positive 28 volts, for enormous energy and headroom. The way to get these sounds is not layer upon layer of 'digital emulation' but trying to get the behaviors right with minimal, atypical algorithms. Only then can you get the energy and sonority of the real Quad Eight.The filtering is a whole other thing from the MCI. No mid peak here. The slopes are more gradual, but you can cut the hell out of the mids, and the deeper into the Seventies we got, the deeper the midrange cuts became. What I've done with ConsoleLA is try to make the simplest, most approachable Console system that can get the widest range of sounds as heard on these records, and reward you when you pursue music-recording in the old school, human way.This doubtless won't be the last, but it might just end up being my favorite. I hope you like ConsoleLA :)"
};
constexpr std::string_view k_tags{
    "consoles"
};

template <typename T>
class ConsoleLAChannel final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double subAL;
    double subAR;
    double subBL;
    double subBR;
    double subCL;
    double subCR;
    double hullL[225];
    double hullR[225];
    int hullp;
    double pearB[22];
    double midA;
    double midB;
    double bassA;
    double bassB;
    double gainA;
    double gainB; // smoothed master fader for channel, from Z2 series filter code
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    ConsoleLAChannel()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        for (int count = 0; count < 222; count++) {
            hullL[count] = 0.0;
            hullR[count] = 0.0;
        }
        hullp = 1;
        for (int x = 0; x < 21; x++) {
            pearB[x] = 0.0;
        }
        subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
        midA = midB = 0.0;
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
        kNumParameters = 5
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.5;
            break;
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 0.5;
            break;
        kParamE:
            return 0.5;
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
            return "treble";
            break;
        kParamB:
            return "mid";
            break;
        kParamC:
            return "bass";
            break;
        kParamD:
            return "pan";
            break;
        kParamE:
            return "fader";
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
            return "Treble";
            break;
        kParamB:
            return "Mid";
            break;
        kParamC:
            return "Bass";
            break;
        kParamD:
            return "Pan";
            break;
        kParamE:
            return "Fader";
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
        overallscale *= Effect<T>::getSampleRate();
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        int limit = 4 * cycleEnd;
        double divisor = 2.0 / limit;
        double treble = (A * 6.0) - 3.0;
        midA = midB;
        midB = (B * 6.0) - 3.0;
        bassA = bassB;
        bassB = (C * 6.0) - 3.0;
        // these should stack to go up to -3.0 to 3.0
        if (treble < 0.0) {
            treble /= 3.0;
        }
        if (midB < 0.0) {
            midB /= 3.0;
        }
        if (bassB < 0.0) {
            bassB /= 3.0;
        }
        // and then become -1.0 to 3.0;
        // there will be successive sin/cos stages w. dry/wet in these
        double freqMid = 0.024;
        switch (cycleEnd)
        {
            case 1: // base sample rate, no change
                break;
            case 2: // 96k tier
                freqMid = 0.012;
                break;
            case 3: // 192k tier
                freqMid = 0.006;
                break;
        }
        int bitshiftL = 0;
        int bitshiftR = 0;
        double panControl = (D * 2.0) - 1.0; //-1.0 to 1.0
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
        gainB = E * 2.0; // smoothed master fader from Z2 filters
        // BitShiftGain pre gain trim goes here
        double subTrim = 0.0011 / overallscale;
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
            double mid = (midA * temp) + (midB * (1.0 - temp));
            double bass = (bassA * temp) + (bassB * (1.0 - temp));
            // begin Hull2 Treble
            hullp--;
            if (hullp < 0) {
                hullp += 60;
            }
            hullL[hullp] = hullL[hullp + 60] = inputSampleL;
            hullR[hullp] = hullR[hullp + 60] = inputSampleR;
            int x = hullp;
            double bassL = 0.0;
            double bassR = 0.0;
            while (x < hullp + (limit / 2)) {
                bassL += hullL[x] * divisor;
                bassR += hullR[x] * divisor;
                x++;
            }
            bassL += bassL * 0.125;
            bassR += bassR * 0.125;
            while (x < hullp + limit) {
                bassL -= hullL[x] * 0.125 * divisor;
                bassR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 20] = hullL[hullp + 80] = bassL;
            hullR[hullp + 20] = hullR[hullp + 80] = bassR;
            x = hullp + 20;
            bassL = bassR = 0.0;
            while (x < hullp + 20 + (limit / 2)) {
                bassL += hullL[x] * divisor;
                bassR += hullR[x] * divisor;
                x++;
            }
            bassL += bassL * 0.125;
            bassR += bassR * 0.125;
            while (x < hullp + 20 + limit) {
                bassL -= hullL[x] * 0.125 * divisor;
                bassR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 40] = hullL[hullp + 100] = bassL;
            hullR[hullp + 40] = hullR[hullp + 100] = bassR;
            x = hullp + 40;
            bassL = bassR = 0.0;
            while (x < hullp + 40 + (limit / 2)) {
                bassL += hullL[x] * divisor;
                bassR += hullR[x] * divisor;
                x++;
            }
            bassL += bassL * 0.125;
            bassR += bassR * 0.125;
            while (x < hullp + 40 + limit) {
                bassL -= hullL[x] * 0.125 * divisor;
                bassR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            double trebleL = inputSampleL - bassL;
            inputSampleL = bassL;
            double trebleR = inputSampleR - bassR;
            inputSampleR = bassR;
            // end Hull2 treble
            // begin Pear filter stages
            // at this point 'bass' is actually still mid and bass
            double slew = ((bassL - pearB[0]) + pearB[1]) * freqMid * 0.5;
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
            // we now have three bands out of hull and pear filters
            double w = 0.0; // filter into bands, apply the sin/cos to each band
            if (treble > 0.0) {
                w = treble;
                if (w > 1.0) {
                    w = 1.0;
                }
                trebleL = (trebleL * (1.0 - w)) + (sin(trebleL * M_PI_2) * treble);
                trebleR = (trebleR * (1.0 - w)) + (sin(trebleR * M_PI_2) * treble);
            }
            if (treble < 0.0) {
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
                w = -treble;
                if (w > 1.0) {
                    w = 1.0;
                }
                if (trebleL > 0) {
                    trebleL = (trebleL * (1.0 - w)) + ((1.0 - cos(trebleL * w)) * (1.0 - w));
                }
                else {
                    trebleL = (trebleL * (1.0 - w)) + ((-1.0 + cos(-trebleL * w)) * (1.0 - w));
                }
                if (trebleR > 0) {
                    trebleR = (trebleR * (1.0 - w)) + ((1.0 - cos(trebleR * w)) * (1.0 - w));
                }
                else {
                    trebleR = (trebleR * (1.0 - w)) + ((-1.0 + cos(-trebleR * w)) * (1.0 - w));
                }
            } // cosine stages for EQ or expansion
            if (midL > 1.0) {
                midL = 1.0;
            }
            if (midL < -1.0) {
                midL = -1.0;
            }
            if (midR > 1.0) {
                midR = 1.0;
            }
            if (midR < -1.0) {
                midR = -1.0;
            }
            if (mid > 0.0) {
                w = mid;
                if (w > 1.0) {
                    w = 1.0;
                }
                midL = (midL * (1.0 - w)) + (sin(midL * M_PI_2) * mid);
                midR = (midR * (1.0 - w)) + (sin(midR * M_PI_2) * mid);
            }
            if (mid < 0.0) {
                w = -mid;
                if (w > 1.0) {
                    w = 1.0;
                }
                if (midL > 0) {
                    midL = (midL * (1.0 - w)) + ((1.0 - cos(midL * w)) * (1.0 - w));
                }
                else {
                    midL = (midL * (1.0 - w)) + ((-1.0 + cos(-midL * w)) * (1.0 - w));
                }
                if (midR > 0) {
                    midR = (midR * (1.0 - w)) + ((1.0 - cos(midR * w)) * (1.0 - w));
                }
                else {
                    midR = (midR * (1.0 - w)) + ((-1.0 + cos(-midR * w)) * (1.0 - w));
                }
            } // cosine stages for EQ or expansion
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
                bassL = (bassL * (1.0 - w)) + (sin(bassL * M_PI_2) * bass);
                bassR = (bassR * (1.0 - w)) + (sin(bassR * M_PI_2) * bass);
            }
            if (bass < 0.0) {
                w = -bass;
                if (w > 1.0) {
                    w = 1.0;
                }
                if (bassL > 0) {
                    bassL = (bassL * (1.0 - w)) + ((1.0 - cos(bassL * w)) * (1.0 - w));
                }
                else {
                    bassL = (bassL * (1.0 - w)) + ((-1.0 + cos(-bassL * w)) * (1.0 - w));
                }
                if (bassR > 0) {
                    bassR = (bassR * (1.0 - w)) + ((1.0 - cos(bassR * w)) * (1.0 - w));
                }
                else {
                    bassR = (bassR * (1.0 - w)) + ((-1.0 + cos(-bassR * w)) * (1.0 - w));
                }
            } // cosine stages for EQ or expansion
            inputSampleL = (bassL + midL + trebleL) * gainL * gain;
            inputSampleR = (bassR + midR + trebleR) * gainR * gain;
            // applies BitShiftPan pan section, and smoothed fader gain
            // begin SubTight section
            double subSampleL = inputSampleL * subTrim;
            double subSampleR = inputSampleR * subTrim;
            double scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subAL + (sin(subAL - subSampleL) * scale));
            subAL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subAR + (sin(subAR - subSampleR) * scale));
            subAR = subSampleR * scale;
            scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subBL + (sin(subBL - subSampleL) * scale));
            subBL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subBR + (sin(subBR - subSampleR) * scale));
            subBR = subSampleR * scale;
            scale = 0.5 + fabs(subSampleL * 0.5);
            subSampleL = (subCL + (sin(subCL - subSampleL) * scale));
            subCL = subSampleL * scale;
            scale = 0.5 + fabs(subSampleR * 0.5);
            subSampleR = (subCR + (sin(subCR - subSampleR) * scale));
            subCR = subSampleR * scale;
            if (subSampleL > 0.25) {
                subSampleL = 0.25;
            }
            if (subSampleL < -0.25) {
                subSampleL = -0.25;
            }
            if (subSampleR > 0.25) {
                subSampleR = 0.25;
            }
            if (subSampleR < -0.25) {
                subSampleR = -0.25;
            }
            inputSampleL += (subSampleL * 16.0);
            inputSampleR += (subSampleR * 16.0);
            // end SubTight section
            // begin Console7 Channel processing
            if (inputSampleL > 1.097) {
                inputSampleL = 1.097;
            }
            if (inputSampleL < -1.097) {
                inputSampleL = -1.097;
            }
            if (inputSampleR > 1.097) {
                inputSampleR = 1.097;
            }
            if (inputSampleR < -1.097) {
                inputSampleR = -1.097;
            }
            inputSampleL = ((sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL))) * 0.8) + (sin(inputSampleL) * 0.2);
            inputSampleR = ((sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR))) * 0.8) + (sin(inputSampleR) * 0.2);
            // this is a version of Spiral blended 80/20 with regular Density.
            // It's blending between two different harmonics in the overtones of the algorithm
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
} // namespace airwindohhs::consolelachannel
