#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::softclock3 {

constexpr std::string_view k_name{ "SoftClock3" };
constexpr std::string_view k_short_description{
    "SoftClock3 is a groove-oriented time reference."
};
constexpr std::string_view k_long_description{
    "You might or might not have heard of the first SoftClock. It came out shortly after I'd developed it because people desperately wanted to play with it, and before I was able to use it much. (still can't, too busy with plugins.) It's the metronome made of waving, wobbling tones where the beat is always a swoop of a (mostly) sine wave that goes up to a high point. Inspired by a joke metronome of dog woofs that proved strangely inspiring to play along to, SoftClock is the machine tempo with perfect regularity but no actual beat, where you have to place the 'click' of your notes wherever it SEEMS to be.Version 3 is largely about taking SoftClock in a strange direction where it builds in Airwindows theories about tempo from 2020, and designs its wobbly guide sound around them, producing distinct wobbles based on what sort of tempo node you're using.But what on earth is a tempo node? Well, in practical terms it's the thing that took away the whole 'big beat' and 'snare pocket' and 'speed' controls, replacing them all with ONE control that is simply 'Flavor'. Set 'flavor' to zero, and you get a simple repeating pitch-waver to guide your tempo. It is not a 'click' but you'll find it easy to sync up to it because the pitch-waver sounds like moving your body feels, and you can set a tempo by its regularity. The beat is where you feel the beat is. The waver is perfectly regular, so it doesn't drift.Set 'flavor' to 0.5, or even more, and things start getting a lot more interesting.That's because this 'node' concept of tempo covers a lot of bases. You can have 'relaxed' nodes where the groove mustn't be too rigid or strenuous. You can have 'tense' nodes where it's positively brittle, loaded with energy. There's nodes that amp up the relaxed energy until it's an infectuous groove… or nodes that take the relaxed energy and hang back, sneaking in attitude until the groove swaggers. It's a continuum of groove, cycling between serene ease and jittery edginess, and it wraps around and around rising tempo like a barber-pole from the slowest to the fastest tempos. One of the Airwindows fans, Bo Danerius, worked out the algorithm and shared it, after I made a post outlining the theory: math webpages can do things like that given scatter-plots of data, and I'd sorted a whole bunch of hit records into these categories.And now, SoftClock3 does it automatically: just set the tempo and it AUTOMATICALLY applies the speed, swing and wobble settings to deliver the correct pitch-wobble for whatever that tempo is. No guesswork. It even helps you tune in exact tempos for a song or riff: if you're near the ideal setting, it's likely that one direction will be ramping up speed and tension, and the other direction will slow the wavering and calm the energy. Either of these directions could be either faster tempo or slower: depends what node you're already at. It ought to be a very natural way to find the correct tempo for whatever you like, by roughing in the general region and then exploring until SoftClock moves the way you need the song to move. Use the Flavor control to govern the amount of character that should be applied to the tempo: full relaxed or full tense don't have swing, but groove and swagger do, and they're adjusted by the amount of Flavor.If all this sounds like complete madness, that is perfectly fine. It's experimentation. We can't all go 'tell me what plugin you want and I'll have AI make you one'. Some of us gotta deal with getting these big ideas, and pursuing where they lead, sometimes for years. It's been six years since I posted the first thoughts on this stuff. SoftClock already existed. Why not run with the combination of all these things, and see what it does?SoftClock3 is a groove-oriented time reference. It's pursuing various theories nobody else has, in hopes of making a kind of 'click' that so perfectly represents entrainment (the way musicians feel time) that it can not only lock people in effortlessly, but also lead them towards the perfect tempo, through exaggerating the flavor of that tempo until it's impossible to miss.If it works, we can all discover a new thing together… and lay down some momentous grooves of all flavors and varieties. :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class SoftClock3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    double sinePos;
    double barPos;
    double inc;
    int beatPos;
    double beatAccent[35];
    double beatSwing[35];
    int beatTable[35];
    int currentBPM;
    double step;
    double swagger;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SoftClock3()
    {
        A = 0.2;
        B = 0.2;
        C = 0.2;
        D = 0.0;
        E = 0.5;
        sinePos = 0.0;
        barPos = 0.0;
        beatPos = 0;
        for (int x = 0; x < 34; x++) {
            beatAccent[x] = 0.0;
            beatSwing[x] = 0.0;
        }
        inc = 0.0;
        beatTable[0] = 0;
        beatTable[1] = 1;
        beatTable[2] = 2;
        beatTable[3] = 3;
        beatTable[4] = 4;
        beatTable[5] = 5;
        beatTable[6] = 6;
        beatTable[7] = 7;
        beatTable[8] = 8;
        beatTable[9] = 9;
        beatTable[10] = 10;
        beatTable[11] = 11;
        beatTable[12] = 11;
        beatTable[13] = 11;
        beatTable[14] = 11;
        beatTable[15] = 13;
        beatTable[16] = 16;
        beatTable[17] = 13;
        beatTable[18] = 13;
        beatTable[19] = 17;
        beatTable[20] = 17;
        beatTable[21] = 17;
        beatTable[22] = 17;
        beatTable[23] = 19;
        beatTable[24] = 24;
        beatTable[25] = 19;
        beatTable[26] = 19;
        beatTable[27] = 19;
        beatTable[28] = 23;
        beatTable[29] = 23;
        beatTable[30] = 23;
        beatTable[31] = 23;
        beatTable[32] = 32;
        beatTable[33] = 32;
        beatTable[34] = 32;
        currentBPM = 0;
        step = 0.0;
        swagger = 0.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.2; break;
            case kParamB: return 0.2; break;
            case kParamC: return 0.2; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "tempo"; break;
            case kParamB: return "count"; break;
            case kParamC: return "tuple"; break;
            case kParamD: return "triplet"; break;
            case kParamE: return "flavor"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Tempo"; break;
            case kParamB: return "Count"; break;
            case kParamC: return "Tuple"; break;
            case kParamD: return "Triplet"; break;
            case kParamE: return "Flavor"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "bpm"; break;
            case kParamB: return "0"; break;
            case kParamC: return "th note"; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;

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
        int bpm = (int)(A * 200.99) + 40;
        int beatCode = (int)(B * 32.99);
        double notes = (double)fmax(((int)(C * 16.99)) / 4.0, 0.125);
        double bpmTarget = (Effect<T>::getSampleRate() * 60.0) / ((double)bpm * notes);
        double triplet = 0.0;
        if (D * 1.99 > 0.5) {
            triplet = 1.0;
        }
        double accent = E;
        if (bpm != currentBPM) {
            currentBPM = bpm;
            step = 20.0;
            double trial = 250.0;
            while (trial > bpm) {
                step -= 1.0;
                trial = (0.016666666666 * pow(step, M_PI)) + (step * M_PI) + 20;
            }
            // we now have the nearest 'flow' energy state UNDER the real bpm, and will step up more slowly
            while (trial < bpm) {
                step += 0.01;
                trial = (0.016666666666 * pow(step, M_PI)) + (step * M_PI) + 20;
            }
            // we now have the nearest tenth of the energy state and have overshot
            while (trial > bpm) {
                step -= 0.001;
                trial = (0.016666666666 * pow(step, M_PI)) + (step * M_PI) + 20;
            }
            // we now have the nearest hundredth to the correct energy state, and can throw away 'trial'
            step = step - ((int)step); // and calculate speed based on the fractional remainder of 'step'
            step = ((0.5 - fabs(step - 0.5)) * 2.0) * 0.75;
            swagger = pow((0.5 - fabs(step - 0.5)) * 2.0, 1.618033988749894) * 0.5 * accent;
            // and turn it into the 'speed' control
            // with step 0.0 being 'flow', 0.25 being 'groove', 0.5 being 'edge' and 0.75 being 'tude'
            // turned into a linear speed control where edge is top step and flow being lowest step.
        }
        double swing = (triplet + swagger) * bpmTarget * 0.33333;
        // swing makes beats hit LATER, so the One is 0.0
        int beatMax = beatTable[beatCode];
        // only some counts are literal, others are ways to do prime grooves with different subrhythms
        for (int x = 0; x < (beatMax + 1); x++) {
            beatAccent[x] = ((double)fabs((double)beatMax - ((double)x * 2.0))) / (double)(beatMax * 1.618033988749894);
            if (x % 2 > 0) {
                beatSwing[x] = (swagger * (1.0 - beatAccent[x]));
            }
            else {
                beatSwing[x] = swing;
            }
        } // this makes the non-accented beats drop down to quiet and back up to half volume
        // we're also decoupling swing from triplet feel unless actually playing triplets
        // otherwise, it's part of how you hear accents and tempos
        if (beatCode > 0) {
            beatAccent[1] = 0.9;
        }
        beatSwing[1] = 0.0; // first note is an accent at full crank
        switch (beatCode) {
            case 0: break; // not used
            case 1: break; // 1
            case 2: break; // 2
            case 3: break; // 3
            case 4:
                beatAccent[3] = 0.9;
                beatSwing[3] = 0.0;
                break; // 4-22
            case 5:
                beatAccent[4] = 0.9;
                beatSwing[4] = 0.0;
                break; // 5-32
            case 6:
                beatAccent[4] = 0.9;
                beatSwing[4] = 0.0;
                break; // 6-33
            case 7:
                beatAccent[5] = 0.9;
                beatSwing[5] = 0.0;
                break; // 7-43
            case 8:
                beatAccent[5] = 0.9;
                beatSwing[5] = 0.0;
                break; // 8-44
            case 9:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatSwing[4] = 0.0;
                beatSwing[7] = 0.0;
                break; // 9-333
            case 10:
                beatAccent[6] = 0.9;
                beatSwing[6] = 0.0;
                break; // 10-55
            case 11:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = 0.0;
                beatSwing[7] = 0.0;
                beatSwing[10] = 0.0;
                break; // 11-3332
            case 12:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = 0.0;
                beatSwing[9] = 0.0;
                break; // 11-443
            case 13:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = 0.0;
                beatSwing[11] = 0.0;
                break; // 11-551
            case 14:
                beatAccent[7] = 0.9;
                beatSwing[7] = 0.0;
                break; // 11-65
            case 15:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = 0.0;
                beatSwing[7] = 0.0;
                beatSwing[10] = 0.0;
                break; // 13-3334
            case 16:
                beatAccent[9] = 0.9;
                beatSwing[9] = 0.0;
                break; // 16-88
            case 17:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = 0.0;
                beatSwing[9] = 0.0;
                break; // 13-445
            case 18:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = 0.0;
                beatSwing[11] = 0.0;
                break; // 13-553
            case 19:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = 0.0;
                beatSwing[9] = 0.0;
                beatSwing[13] = 0.0;
                beatSwing[17] = 0.0;
                break; // 17-44441
            case 20:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = 0.0;
                beatSwing[11] = 0.0;
                beatSwing[16] = 0.0;
                break; // 17-5552
            case 21:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = 0.0;
                beatSwing[15] = 0.0;
                break; // 17-773
            case 22:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = 0.0;
                beatSwing[17] = 0.0;
                break; // 17-881
            case 23:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = 0.0;
                beatSwing[9] = 0.0;
                beatSwing[13] = 0.0;
                beatSwing[17] = 0.0;
                break; // 19-44443
            case 24:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = 0.0;
                beatSwing[17] = 0.0;
                break; // 24-888
            case 25:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = 0.0;
                beatSwing[11] = 0.0;
                beatSwing[16] = 0.0;
                break; // 19-5554
            case 26:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = 0.0;
                beatSwing[15] = 0.0;
                break; // 19-775
            case 27:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = 0.0;
                beatSwing[17] = 0.0;
                break; // 19-883
            case 28:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatAccent[21] = 0.7;
                beatSwing[5] = 0.0;
                beatSwing[9] = 0.0;
                beatSwing[13] = 0.0;
                beatSwing[17] = 0.0;
                beatSwing[21] = 0.0;
                break; // 23-444443
            case 29:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = 0.0;
                beatSwing[11] = 0.0;
                beatSwing[16] = 0.0;
                break; // 23-5558
            case 30:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatAccent[22] = 0.7;
                beatSwing[8] = 0.0;
                beatSwing[15] = 0.0;
                beatSwing[22] = 0.0;
                break; // 23-7772
            case 31:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = 0.0;
                beatSwing[17] = 0.0;
                break; // 23-887
            case 32:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatAccent[25] = 0.7;
                beatSwing[9] = 0.0;
                beatSwing[17] = 0.0;
                beatSwing[25] = 0.0;
                break; // 32-8888
            default: break;
        }
        double chaseSpeed = ((step * 0.000125) + 0.0000125) / overallscale;
        double rootSpeed = 0.9999 - (chaseSpeed * ((2.0 * step) + 2.0));
        double pulseWidth = (0.085 + (accent * 0.04) - ((1.0 - step) * 0.06)) / chaseSpeed;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            barPos += 1.0;
            if (barPos > bpmTarget) {
                barPos = 0.0;
                beatPos++;
                if (beatPos > beatMax) {
                    beatPos = 1;
                }
            }
            if ((barPos < (pulseWidth + beatSwing[beatPos])) && (barPos > beatSwing[beatPos])) {
                inc = (((beatAccent[beatPos] * accent) + (1.0 - accent)) * chaseSpeed) + (inc * (1.0 - chaseSpeed));
            }
            else {
                inc *= rootSpeed;
            }
            sinePos += inc;
            if (sinePos > 6.283185307179586) {
                sinePos -= 6.283185307179586;
            }
            inputSampleL = inputSampleR = sin(sin(sinePos) * inc * 5.0);
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
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::softclock3
