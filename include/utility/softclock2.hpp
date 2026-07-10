#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::softclock2 {

constexpr std::string_view k_name{ "SoftClock2" };
constexpr std::string_view k_short_description{
    "SoftClock2 is a groove-oriented time reference."
};
constexpr std::string_view k_long_description{
    "SoftClock2, entrainment boogaloo!You might or might not have heard of the first SoftClock. It came out shortly after I'd developed it because people desperately wanted to play with it, and before I was able to use it much. (still can't, too busy with plugins.) It's the metronome made of waving, wobbling tones where the beat is always a swoop of a (mostly) sine wave that goes up to a high point. Inspired by a joke metronome of dog woofs that proved strangely inspiring to play along to, SoftClock is the machine tempo with perfect regularity but no actual beat, where you have to place the 'click' of your notes wherever it SEEMS to be.Turns out there are some refinements to be made with SoftClock2. None of them involve ever specifying exactly where a click really is, or letting it sync with a DAW grid. The click is ONLY where you think it to be, and at its best the click is a shape in time defined by your riffs, a terrific way to swing along with really grooving time or dramatic flourishes. It's the anti-machine rhythm.This is because SoftClock2 swings along with the 'entrainment' of your limbs and musical gestures. Generally you don't play music in sudden microsecond-long bursts of your arms or fingers. You have to swing your arm or lean into a kick or, if you're Les Claypool, stomp your foot along to the beat to get yourself moving to the music. The weight of your motion helps you to keep things steady.SoftClock2 gives an audio picture of that motion, not of any specific point where the note hits. That makes it a lot more friendly and encouraging to play along to than a traditional click.And this, you'll need, if you take advantage of SoftClock2's capacity for strange grooves and polyrhythms. Here's where one of the updates is found: on at least the generic VST version of the plugin, the 'Count' control tells you not only which beat it's doing, but after that, the time signature. Many times that's predictable, like a 5 or a 7 or a 4/4. But then you have many variations on 11, 13, 17, 19, 23… just a wide gamut of freaky proggy things to count. The count always works like this: the nonaccented notes start high with the One, swing down to halfway, and then ramp up again to the One. The accents follow patterns: if you want to interpret the count differently (or just have a steady pulse) that's fine, but it's meant to give you subdivisions, typically repeated long subdivisions ending with a short one. For instance, all the 19s are counted that way, like going 5-5-5-4 because the idea is a good weird groove sets up patterns you can track, and a departure at the end. So, SoftClock2 was always designed to do that.There's a change, though. The original had a 'BigBeat' control and a Swing control. Swing hesitated alternate beats up to and beyond doing a triplet shuffle feel, and BigBeat always took the 'valley' and treated it as the snare hit, which you could hesitate to make a weightier backbeat or a pocket backbeat. Doesn't take much.The trouble is, people kept hearing the peak as the snare backbeat.So, SoftClock2 simply lets you decide. This is most relevant to simpler beats, and the weird crazy time signatures may still sound best with the beats ramping up to the One. But if you're just doing a 4 or an 8, or need to reverse the feel of something, or you're doing a reggae where the One is the valley but you need it to hit well behind the beat to groove properly… increase the Valley control instead, and now that accent is slowed. Or, if you're using a simple beat but you hear it ramping up to the snare hit, increase the Peak control and it gets more weight and swing behind it. You can sync it with settings of Swing too, or even construct perfectly steady but wonky grooves this way.If you're using an AU version, or a DAW where it's not constantly redrawing the plugin labelling, or if you're using Consolidated and it doesn't update what's next to Count quickly enough, I found that closing and opening the plugin interface does seem to update what's shown to the current setting. It's just that some DAWs live-draw this label constantly, and others cache it and stop recalculating it. I can't control that part, but you can get a reference to what you did with a little extra trouble. Count it, it should be doing spaced accents with a departure right at the end to give you the funny time. You, too, can do 17s and 19s that are actually catchy, if you make 'em out of sub-riffs in this way.And then if you want to be really annoying you can lay down SoftClock with whatever you're doing, and what if you'd like to overdub a complete departure? Like you're in 4/4 but you'd like to put down part of the riff where it's 7 notes in the space of 4?Cheat. Start with Tuple in 4 to lay down the main riff, and then elsewhere in the arrange window, do a separate pass with Tuple set to 7. It'll be quite a bit faster. Groove whatever you want to that, get a good snippet, and then fly that over to the middle of your 4 groove… and it will line up perfectly with the notes you did in 4, because Tuple is just as accurate in 7 as it is in 4. It'll do the math for you, so you can construct all sorts of peculiar things but make them groove like anything.Play with it, have fun. There's no wrong way to use this, and you don't have to make it over-fancy. It's just able to scale up to the most unreasonable things and make them seem like you have amazing powers of odd-time-having. The best part is, you do it all playing live to the wobbling tone in SoftClock, so it won't seem artificial. Just kind of alien :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class SoftClock2 final : public Effect<T>
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
    double sinePos;
    double barPos;
    double inc;
    int beatPos;
    double beatAccent[35];
    double beatSwing[35];
    int beatTable[35];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    SoftClock2()
    {
        A = 0.2;
        B = 0.2;
        C = 0.2;
        D = 0.0;
        E = 0.0;
        F = 0.0;
        G = 0.5;
        H = 0.5;
        I = 0.5;
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
        kNumParameters = 9
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            case kParamI: I = value; break;

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
            case kParamF: return F; break;
            case kParamG: return G; break;
            case kParamH: return H; break;
            case kParamI: return I; break;

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
            case kParamE: return 0.0; break;
            case kParamF: return 0.0; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.5; break;

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
            case kParamD: return "swing"; break;
            case kParamE: return "peak"; break;
            case kParamF: return "valley"; break;
            case kParamG: return "accents"; break;
            case kParamH: return "boost"; break;
            case kParamI: return "speed"; break;

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
            case kParamD: return "Swing"; break;
            case kParamE: return "Peak"; break;
            case kParamF: return "Valley"; break;
            case kParamG: return "Accents"; break;
            case kParamH: return "Boost"; break;
            case kParamI: return "Speed"; break;

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
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;
            case kParamI: return std::to_string(I); break;

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
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return ""; break;
            case kParamI: return ""; break;

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
        double swing = D * bpmTarget * 0.66666;
        double peak = E * bpmTarget * 0.33333;
        double valley = F * bpmTarget * 0.33333;
        // swing makes beats hit LATER, so the One is 0.0
        // peak means go UP to a late beat
        // valley means go DOWN to a late beat
        int beatMax = beatTable[beatCode];
        // only some counts are literal, others are ways to do prime grooves with different subrhythms
        for (int x = 0; x < (beatMax + 1); x++) {
            beatAccent[x] = ((double)fabs((double)beatMax - ((double)x * 2.0))) / (double)(beatMax * 1.618033988749894);
            if (x % 2 > 0) {
                beatSwing[x] = 0.0;
            }
            else {
                beatSwing[x] = swing;
            }
        } // this makes the non-accented beats drop down to quiet and back up to half volume
        if (beatCode > 0) {
            beatAccent[1] = 0.9;
        }
        beatSwing[1] = peak; // first note is an accent at full crank
        switch (beatCode) {
            case 0: break; // not used
            case 1: break; // 1
            case 2: break; // 2
            case 3: break; // 3
            case 4:
                beatAccent[3] = 0.9;
                beatSwing[3] = valley;
                break; // 4-22
            case 5:
                beatAccent[4] = 0.9;
                beatSwing[4] = valley;
                break; // 5-32
            case 6:
                beatAccent[4] = 0.9;
                beatSwing[4] = valley;
                break; // 6-33
            case 7:
                beatAccent[5] = 0.9;
                beatSwing[5] = valley;
                break; // 7-43
            case 8:
                beatAccent[5] = 0.9;
                beatSwing[5] = valley;
                break; // 8-44
            case 9:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatSwing[4] = valley;
                beatSwing[7] = valley;
                break; // 9-333
            case 10:
                beatAccent[6] = 0.9;
                beatSwing[6] = valley;
                break; // 10-55
            case 11:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = valley;
                beatSwing[7] = valley;
                beatSwing[10] = valley;
                break; // 11-3332
            case 12:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = valley;
                beatSwing[9] = valley;
                break; // 11-443
            case 13:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = valley;
                beatSwing[11] = valley;
                break; // 11-551
            case 14:
                beatAccent[7] = 0.9;
                beatSwing[7] = valley;
                break; // 11-65
            case 15:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = valley;
                beatSwing[7] = valley;
                beatSwing[10] = valley;
                break; // 13-3334
            case 16:
                beatAccent[9] = 0.9;
                beatSwing[9] = valley;
                break; // 16-88
            case 17:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = valley;
                beatSwing[9] = valley;
                break; // 13-445
            case 18:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = valley;
                beatSwing[11] = valley;
                break; // 13-553
            case 19:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = valley;
                beatSwing[9] = valley;
                beatSwing[13] = valley;
                beatSwing[17] = valley;
                break; // 17-44441
            case 20:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = valley;
                beatSwing[11] = valley;
                beatSwing[16] = valley;
                break; // 17-5552
            case 21:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = valley;
                beatSwing[15] = valley;
                break; // 17-773
            case 22:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = valley;
                beatSwing[17] = valley;
                break; // 17-881
            case 23:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = valley;
                beatSwing[9] = valley;
                beatSwing[13] = valley;
                beatSwing[17] = valley;
                break; // 19-44443
            case 24:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = valley;
                beatSwing[17] = valley;
                break; // 24-888
            case 25:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = valley;
                beatSwing[11] = valley;
                beatSwing[16] = valley;
                break; // 19-5554
            case 26:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = valley;
                beatSwing[15] = valley;
                break; // 19-775
            case 27:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = valley;
                beatSwing[17] = valley;
                break; // 19-883
            case 28:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatAccent[21] = 0.7;
                beatSwing[5] = valley;
                beatSwing[9] = valley;
                beatSwing[13] = valley;
                beatSwing[17] = valley;
                beatSwing[21] = valley;
                break; // 23-444443
            case 29:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = valley;
                beatSwing[11] = valley;
                beatSwing[16] = valley;
                break; // 23-5558
            case 30:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatAccent[22] = 0.7;
                beatSwing[8] = valley;
                beatSwing[15] = valley;
                beatSwing[22] = valley;
                break; // 23-7772
            case 31:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = valley;
                beatSwing[17] = valley;
                break; // 23-887
            case 32:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatAccent[25] = 0.7;
                beatSwing[9] = valley;
                beatSwing[17] = valley;
                beatSwing[25] = valley;
                break; // 32-8888
            default: break;
        }
        double accent = 1.0 - pow(1.0 - G, 2);
        double chaseSpeed = ((I * 0.00016) + 0.000016) / overallscale;
        double rootSpeed = 1.0 - (chaseSpeed * ((1.0 - I) + 0.5) * 4.0);
        double pulseWidth = ((H * 0.2) - ((1.0 - I) * 0.03)) / chaseSpeed;
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
            inputSampleL = inputSampleR = sin(sin(sinePos) * inc * 8.0);
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
} // namespace airwindohhs::softclock2
