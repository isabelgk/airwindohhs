#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::bezeq2 {

constexpr std::string_view k_name{ "BezEQ2" };
constexpr std::string_view k_short_description{
    "BezEQ2 is a unique, subtle three-band shelving EQ."
};
constexpr std::string_view k_long_description{
    "Turns out there's a reason BezEQ sounded so weird! Bugs. Both in the sense of 'what's that doing there' and 'hey, this could be very different and it just might work'. So now there's two BezEQs, the first one is still a strange strange beast, and this one is……fugitive?It's hard to explain how different this became. It's still a three-band EQ made by crossing over filters in such a way that re-adding them produces perfect fidelity: that works great even on steep filters like SmoothEQ. It even works if you do funny things like heavy phase shifting in the filter… or if, like in this filter, you're delaying bands to more accurately cancel out bass frequencies. (This filter incurs a little bit of latency, depending on how the crossovers are set, because of these delays.)Far from the first BezEQ, this BezEQ2 has a special knack for hiding what it's doing. It can boost highs, boost bass, and the result is weirdly like no change was made. There's a quirk where if you cut the middle all the way to zero, the highs will also cut out (a side-effect of how the delays are set up). It's still not exactly normal.And then, perhaps a factor of how the bands are summed together, if you go to cut highs or lows, BezEQ does the opposite. It gives you a midrangey sound with a lot of punch and character. This, even though it's doing the opposite of last week's FatEQ, and isn't distorting anything. Something about how the Bezier curves handle transient impact, combined with the phase-altering delay lines, makes the plugin a heck of a secret weapon for heightening midrange drama and impact. Or, you can just set it up as first a mids cut, and then even more of a bass cut, and get a really articulate, natural-sounding energy lift that sounds remarkably unhyped for all the hype it sneaks into a track.I have a feeling this is gonna be a real 'sleeper' plugin. It seems to do anything I ask while sounding like nothing was done. Those who are trying to mix by adding color upon color to their sounds, probably won't get this at all. But those of a simpler, rawer approach might really be pleasantly surprised. Even startled, in the best possible way."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class BezEQ2 final : public Effect<T>
{
    static constexpr int predelay = 4096;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'bzer';
    float A;
    float B;
    float C;
    float D;
    float E;
    enum
    {
        bez_AL,
        bez_AR,
        bez_BL,
        bez_BR,
        bez_CL,
        bez_CR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezA[bez_total];
    double bezB[bez_total];
    double aL[predelay + 5];
    double aR[predelay + 5];
    int countA;
    double bL[predelay + 5];
    double bR[predelay + 5];
    int countB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BezEQ2()
    {
        A = 0.5;
        B = 0.75;
        C = 0.5;
        D = 0.25;
        E = 0.5;
        for (int x = 0; x < bez_total; x++) {
            bezA[x] = 0.0;
            bezB[x] = 0.0;
        }
        bezA[bez_cycle] = 1.0;
        bezB[bez_cycle] = 1.0;
        for (int count = 0; count < predelay + 2; count++) {
            aL[count] = 0.0;
            bL[count] = 0.0;
            aR[count] = 0.0;
            bR[count] = 0.0;
        }
        countA = 1;
        countB = 1;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.75; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.25; break;
            case kParamE: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "treble"; break;
            case kParamB: return "x"; break;
            case kParamC: return "mid"; break;
            case kParamD: return "x"; break;
            case kParamE: return "bass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Treble"; break;
            case kParamB: return "x"; break;
            case kParamC: return "Mid"; break;
            case kParamD: return "x"; break;
            case kParamE: return "Bass"; break;

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
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
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
        double trebleGain = (A * 2.0);
        double derezA = B / overallscale;
        if (derezA < 0.00001) {
            derezA = 0.00001;
        }
        if (derezA > 1.0) {
            derezA = 1.0;
        }
        int midDelay = (int)(1.0 / derezA);
        if (midDelay > 4096) {
            midDelay = 4096;
        }
        derezA = 1.0 / midDelay;
        double midTrim = (double)midDelay / (midDelay + 1.0);
        midTrim = 1.0 - (derezA * midTrim);
        midDelay = (int)(midDelay * 0.5 * midTrim);
        double midGain = (C * 2.0);
        double derezB = pow(D, 2.0) / overallscale;
        if (derezB < 0.00001) {
            derezB = 0.00001;
        }
        if (derezB > 1.0) {
            derezB = 1.0;
        }
        int bassDelay = (int)(1.0 / derezB);
        if (bassDelay > 4096) {
            bassDelay = 4096;
        }
        derezB = 1.0 / bassDelay;
        double bassTrim = (double)bassDelay / (bassDelay + 1.0);
        bassTrim = 1.0 - (derezB * bassTrim);
        bassDelay = (int)(bassDelay * 0.5 * bassTrim);
        double bassGain = (E * 2.0);
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            bezA[bez_cycle] += derezA;
            bezA[bez_SampL] += (inputSampleL * derezA);
            bezA[bez_SampR] += (inputSampleR * derezA);
            if (bezA[bez_cycle] > 1.0) {
                bezA[bez_cycle] = 0.0;
                bezA[bez_CL] = bezA[bez_BL];
                bezA[bez_BL] = bezA[bez_AL];
                bezA[bez_AL] = bezA[bez_SampL];
                bezA[bez_SampL] = 0.0;
                bezA[bez_CR] = bezA[bez_BR];
                bezA[bez_BR] = bezA[bez_AR];
                bezA[bez_AR] = bezA[bez_SampR];
                bezA[bez_SampR] = 0.0;
            }
            double X = bezA[bez_cycle] * midTrim;
            double CBL = (bezA[bez_CL] * (1.0 - X)) + (bezA[bez_BL] * X);
            double BAL = (bezA[bez_BL] * (1.0 - X)) + (bezA[bez_AL] * X);
            double midL = (bezA[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * 0.25;
            double CBR = (bezA[bez_CR] * (1.0 - X)) + (bezA[bez_BR] * X);
            double BAR = (bezA[bez_BR] * (1.0 - X)) + (bezA[bez_AR] * X);
            double midR = (bezA[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * 0.25;
            // predelay
            aL[countA] = inputSampleL;
            aR[countA] = inputSampleR;
            countA++;
            if (countA < 0 || countA > midDelay) {
                countA = 0;
            }
            inputSampleL = aL[countA - ((countA > midDelay) ? midDelay + 1 : 0)];
            inputSampleR = aR[countA - ((countA > midDelay) ? midDelay + 1 : 0)];
            // end predelay
            double trebleL = inputSampleL - midL;
            double trebleR = inputSampleR - midR;
            bezB[bez_cycle] += derezB;
            bezB[bez_SampL] += (midL * derezB);
            bezB[bez_SampR] += (midR * derezB);
            if (bezB[bez_cycle] > 1.0) {
                bezB[bez_cycle] = 0.0;
                bezB[bez_CL] = bezB[bez_BL];
                bezB[bez_BL] = bezB[bez_AL];
                bezB[bez_AL] = bezB[bez_SampL];
                bezB[bez_SampL] = 0.0;
                bezB[bez_CR] = bezB[bez_BR];
                bezB[bez_BR] = bezB[bez_AR];
                bezB[bez_AR] = bezB[bez_SampR];
                bezB[bez_SampR] = 0.0;
            }
            X = bezB[bez_cycle] * bassTrim;
            CBL = (bezB[bez_CL] * (1.0 - X)) + (bezB[bez_BL] * X);
            BAL = (bezB[bez_BL] * (1.0 - X)) + (bezB[bez_AL] * X);
            double bassL = (bezB[bez_BL] + (CBL * (1.0 - X)) + (BAL * X)) * 0.25;
            CBR = (bezB[bez_CR] * (1.0 - X)) + (bezB[bez_BR] * X);
            BAR = (bezB[bez_BR] * (1.0 - X)) + (bezB[bez_AR] * X);
            double bassR = (bezB[bez_BR] + (CBR * (1.0 - X)) + (BAR * X)) * 0.25;
            if (midGain > 0.0001) {
                midL += (trebleL * (trebleGain / midGain));
                midR += (trebleR * (trebleGain / midGain));
            }
            // predelay
            bL[countB] = midL;
            bR[countB] = midR;
            countB++;
            if (countB < 0 || countB > bassDelay) {
                countB = 0;
            }
            midL = bL[countB - ((countB > bassDelay) ? bassDelay + 1 : 0)];
            midR = bR[countB - ((countB > bassDelay) ? bassDelay + 1 : 0)];
            // end predelay
            midL -= bassL;
            midR -= bassR;
            inputSampleL = (bassL * bassGain) + (midL * midGain);
            inputSampleR = (bassR * bassGain) + (midR * midGain);
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
} // namespace airwindohhs::bezeq2
