#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::softclock {

constexpr std::string_view k_name{ "SoftClock" };
constexpr std::string_view k_short_description{
    "SoftClock is a groove-oriented time reference."
};
constexpr std::string_view k_long_description{
    "By special request, here's SoftClock before I can properly demo and use it. I'll have to play catch-up!SoftClock is a timing reference, like a click track… but far more powerful, because it does not synchronize machines so much as it synchronizes HUMANS. You put it on an unused track, and route it to another track AS AUDIO which you then record, in the arrange window as just another audio file. This can be done as you lay down an initial track while listening to the SoftClock output.What's the noise it makes? Frequency swoops, of a tone. It doesn't turn on and off, there's no attack or decay… a continuous tone which drops in volume as the pitch drops, but otherwise is just a drone… and this is your new click track.The next question is 'for the love of God, why would you do such a thing?' Because not only is this not a percussive sound, it doesn't even have a definable attack, and yet it proposes to replace click tracks, and in fact wants to become a timing reference for a world of 'human played' music where nothing is snapped to grid. There will be no grid, just human-played tracks riffing over SoftClock.The reason is, Entrainment. The 'swoop' of the tone mimicks the swing of a drummer's arm, or the strum of a guitar… or a dancer's body. It's digitally accurate, but directs the listener not toward an inhumanly accurate 'tick' but towards a motion… and when many players are moving in synchrony, that's when you have a powerful groove.But there's more. It's also a reference to where you are in the bar. The non-accented beats ebb down to a lower tone, and then ramp up again toward the One, which is the loudest and highest-pitched swoop of the tone. The amount of variation can be controlled, as can the basic pitch. Bear in mind the volume does modulate, but not in such a way as to produce an attack, even at the fastest speed settings. You dial it in to mimic the motion of your body playing, whether that's groovy or laid-back or hammering, and the non-accented beats always show you where you are in the bar, in an easy-to-hear way, even if the beats are themselves swung or the backbeat is slowed to add weight and bombast.What about those accented beats? Firstly, they're louder and higher-pitched, but they're also spaced out in an interesting way. You see, SoftClock can give you 4/4, or eighth notes, or four bars of 4/4… but it will also give you odd times beyond your humble sevens and elevens. It'll give variations on funny times when you've gone beyond 4/4. Here's the list, and how they're counted:1, 2, and 3 are counted as you'd expect. 4 is counted as 2,2. 5 is counted as 3,2 (in other words: one two three one two, with one being the accent). 6 is 3-3, and 7 is counted backwards from how Pink Floyd's 'Money' had it: 4-3, one two three four one two three. 8 is 4-4, 9 is 3-3-3, 10 is 5-5.Then things get more complicated. Note that Count numbers 8, 16, 24 and 32 are reserved for 4/4 with accents every eight beats, so you can have normal time but hear the unaccented beats guide you around one, two, three or four bars and highlight the first One.11 is counted four ways: 3332, 443, 551, 65.13 is counted three ways: 3334, 445, 553.17 is counted four ways: 44441, 5552, 773, 881. 19 is counted four ways: 44443, 5554, 775, 883.23 is counted four ways: 444443, 5558, 7772, 887.Count the accents and they'll come in these spacings. It's options for 'sub-phrases', repetitions or modulations up front and a variation for a turn-around, orienting you to the odd rhythm. The idea is for the repetitions to themselves be easily countable, and ideally seem to fit into a 'four sections but one is different' or 'three sections but one is different' model.This is in line with a music arrangement game I've been developing that's more likely to see life as a video-game now than as the card game I'd intended. Point being, SoftClock can do normal metronome things (except using entrainment rather than ticking at you) and orientation things as far as locating you in the bar, but it can also combine all that into a baffling proggy rhythm that nevertheless guides you into grooving and memorable riffs of many kinds. If the standard variations on freaky prime-numbered prog-meters aren't enough, you can play with Swing and the delay of BigBeat (accented beats) until you've gone full Beefheart: or, rather, full Magic Band and John French drum parts.Dive as deep as you like, or keep it more simple: SoftClock has you covered. You may be surprised at how much easier it is to hear, entrain, and orient yourself when using a click track that refuses to click. Remember, lay it down as a track, don't try to match it to a normal click, don't grid anything! SoftClock is for gridless grooves, and music that sounds human because it moves like humans. See how it feels."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class SoftClock final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
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
    SoftClock()
    {
        A = 0.2;
        B = 0.2;
        C = 0.2;
        D = 0.0;
        E = 0.0;
        F = 0.5;
        G = 0.5;
        H = 0.5;
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
        kNumParameters = 8
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
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;

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
            case kParamE: return "bigbeat"; break;
            case kParamF: return "accents"; break;
            case kParamG: return "boost"; break;
            case kParamH: return "speed"; break;

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
            case kParamE: return "BigBeat"; break;
            case kParamF: return "Accents"; break;
            case kParamG: return "Boost"; break;
            case kParamH: return "Speed"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return ""; break;

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
        double bigbeat = E * bpmTarget * 0.33333;
        // swing makes beats hit LATER, so the One is 0.0
        // bigbeat makes snares hit either on the beat or later
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
            beatAccent[1] = 1.0;
        }
        beatSwing[1] = 0.0; // first note is an accent at full crank
        switch (beatCode) {
            case 0: break; // not used
            case 1: break; // 1
            case 2: break; // 2
            case 3: break; // 3
            case 4:
                beatAccent[3] = 0.9;
                beatSwing[3] = bigbeat;
                break; // 4-22
            case 5:
                beatAccent[4] = 0.9;
                beatSwing[4] = bigbeat;
                break; // 5-32
            case 6:
                beatAccent[4] = 0.9;
                beatSwing[4] = bigbeat;
                break; // 6-33
            case 7:
                beatAccent[5] = 0.9;
                beatSwing[5] = bigbeat;
                break; // 7-43
            case 8:
                beatAccent[5] = 0.9;
                beatSwing[5] = bigbeat;
                break; // 8-44
            case 9:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatSwing[4] = bigbeat;
                beatSwing[7] = bigbeat;
                break; // 9-333
            case 10:
                beatAccent[6] = 0.9;
                beatSwing[6] = bigbeat;
                break; // 10-55
            case 11:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = bigbeat;
                beatSwing[7] = bigbeat;
                beatSwing[10] = bigbeat;
                break; // 11-3332
            case 12:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = bigbeat;
                beatSwing[9] = bigbeat;
                break; // 11-443
            case 13:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = bigbeat;
                beatSwing[11] = bigbeat;
                break; // 11-551
            case 14:
                beatAccent[7] = 0.9;
                beatSwing[7] = bigbeat;
                break; // 11-65
            case 15:
                beatAccent[4] = 0.9;
                beatAccent[7] = 0.8;
                beatAccent[10] = 0.7;
                beatSwing[4] = bigbeat;
                beatSwing[7] = bigbeat;
                beatSwing[10] = bigbeat;
                break; // 13-3334
            case 16:
                beatAccent[9] = 0.9;
                beatSwing[9] = bigbeat;
                break; // 16-88
            case 17:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.8;
                beatSwing[5] = bigbeat;
                beatSwing[9] = bigbeat;
                break; // 13-445
            case 18:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatSwing[6] = bigbeat;
                beatSwing[11] = bigbeat;
                break; // 13-553
            case 19:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = bigbeat;
                beatSwing[9] = bigbeat;
                beatSwing[13] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 17-44441
            case 20:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = bigbeat;
                beatSwing[11] = bigbeat;
                beatSwing[16] = bigbeat;
                break; // 17-5552
            case 21:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = bigbeat;
                beatSwing[15] = bigbeat;
                break; // 17-773
            case 22:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 17-881
            case 23:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatSwing[5] = bigbeat;
                beatSwing[9] = bigbeat;
                beatSwing[13] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 19-44443
            case 24:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 24-888
            case 25:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = bigbeat;
                beatSwing[11] = bigbeat;
                beatSwing[16] = bigbeat;
                break; // 19-5554
            case 26:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatSwing[8] = bigbeat;
                beatSwing[15] = bigbeat;
                break; // 19-775
            case 27:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 19-883
            case 28:
                beatAccent[5] = 0.9;
                beatAccent[9] = 0.85;
                beatAccent[13] = 0.8;
                beatAccent[17] = 0.75;
                beatAccent[21] = 0.7;
                beatSwing[5] = bigbeat;
                beatSwing[9] = bigbeat;
                beatSwing[13] = bigbeat;
                beatSwing[17] = bigbeat;
                beatSwing[21] = bigbeat;
                break; // 23-444443
            case 29:
                beatAccent[6] = 0.9;
                beatAccent[11] = 0.8;
                beatAccent[16] = 0.7;
                beatSwing[6] = bigbeat;
                beatSwing[11] = bigbeat;
                beatSwing[16] = bigbeat;
                break; // 23-5558
            case 30:
                beatAccent[8] = 0.9;
                beatAccent[15] = 0.8;
                beatAccent[22] = 0.7;
                beatSwing[8] = bigbeat;
                beatSwing[15] = bigbeat;
                beatSwing[22] = bigbeat;
                break; // 23-7772
            case 31:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatSwing[9] = bigbeat;
                beatSwing[17] = bigbeat;
                break; // 23-887
            case 32:
                beatAccent[9] = 0.9;
                beatAccent[17] = 0.8;
                beatAccent[25] = 0.7;
                beatSwing[9] = bigbeat;
                beatSwing[17] = bigbeat;
                beatSwing[25] = bigbeat;
                break; // 32-8888
            default: break;
        }
        double accent = 1.0 - pow(1.0 - F, 2);
        double chaseSpeed = ((H * 0.00016) + 0.000016) / overallscale;
        double rootSpeed = 1.0 - (chaseSpeed * ((1.0 - H) + 0.5) * 4.0);
        double pulseWidth = ((G * 0.2) - ((1.0 - H) * 0.03)) / chaseSpeed;
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
} // namespace airwindohhs::softclock
