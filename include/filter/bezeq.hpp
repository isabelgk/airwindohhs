#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::bezeq {

constexpr std::string_view k_name{ "BezEQ" };
constexpr std::string_view k_short_description{
    "BezEQ is a strange, alien three-band shelving EQ."
};
constexpr std::string_view k_long_description{
    "You don't have an EQ anything like this. Nobody does. This is guaranteed new.The reason is, nobody with audio training would try to apply Bezier curves to audio reconstruction (since it doesn't make sense, though it's nice and efficient to do), and if you did you'd get my latest DeRez which doesn't work like a normal frequency crush and so that would scare more people off, and then if you tried to use two DeRezzes in series as brickwall filters for a three-band shelving EQ, you'd so quickly run into problems (for instance, with test tones) that you'd immediately run away, never to return, knowing it would be impossible to get away with it.And more fool anybody who did try ALL of these things and then gave up… because this is a hell of a thing, and now it's mine, which means now it's yours. I do Patreon so it doesn't matter if one of my plugins is scorned as an EQ and behaves terribly in testing. I can put it out anyway! And even explore the idea of building it into ConsoleH… but that's in development, and BezEQ is yours right now.If you keep Treble, Mid and Bass exactly the same volume (any value, the settings will match) it will behave quite perfectly. Doesn't matter what the crossovers do. They're assembled subtractively so they'll add up to perfect, every time, if they're set to 'flat'. I often do that with 'general purpose' EQs or with things like StoneFire: it's good to have 'flat' be really, really free from issues. It's the opposite of breaking things into an FFT transform or an mp3 and then reconstructing them from the (limited) data.Then, if you shut off Treble, the output is DeRez3 using the X control between Treble and Mid to set the cutoff.And if you shut off Treble and Mid, the output is DeRez3 using the X control between Mid and Bass.And if you shut off everything BUT Treble, it's the dry signal MINUS the first DeRez3. It's only the highs from what would've been a de-rezzed, pseudo-brickwall cutoff.And if you have Treble AND Bass, you have that plus the derezzed Bass, which is what remains after the Mid goes through another 'filter' that's not really a filter. Same with if you only have Mid: it's derezzed treble-removed, but with the other derez subtracted.Except you'll quickly learn that none of these are clean crossovers at all: you'll get a strange overtone that comes out at the filter cutoff and acts weird. You can add a LOT of deep bass and an extra tone, like a ring mod, through boosting bass and adjusting that cutoff. You can add bass through that extra tone by CUTTING bass and adjusting the cutoff. It's a bizarre toy but it works predictably and has its own sound, to love or hate.And that's not even getting into when you do this, and then make the cutoffs fight each other by setting TxM very low and MxB very high, causing the distortions to distort and interact… you can get into a lot of trouble with this, and so totally reinvent the sound that it becomes unrecognizable.And then dial it back to as subtle as you like, simply by making Treble, Mid and Bass be closer to each other.Have fun and don't say I didn't warn you :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class BezEQ final : public Effect<T>
{
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
        bez_InL,
        bez_InR,
        bez_UnInL,
        bez_UnInR,
        bez_SampL,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezA[bez_total];
    double bezB[bez_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    BezEQ()
    {
        A = 0.5;
        B = 0.618;
        C = 0.5;
        D = 0.382;
        E = 0.5;
        for (int x = 0; x < bez_total; x++) {
            bezA[x] = 0.0;
            bezB[x] = 0.0;
        }
        bezA[bez_cycle] = 1.0;
        bezB[bez_cycle] = 1.0;
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
            case kParamB: return 0.618; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.382; break;
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
        double trebleGain = A * 2.0;
        trebleGain *= trebleGain;
        double derezA = B / overallscale;
        if (derezA < 0.01) {
            derezA = 0.01;
        }
        if (derezA > 1.0) {
            derezA = 1.0;
        }
        derezA = 1.0 / ((int)(1.0 / derezA));
        double midGain = C * 2.0;
        midGain *= midGain;
        double derezB = pow(D, 4.0) / overallscale;
        if (derezB < 0.0001) {
            derezB = 0.0001;
        }
        if (derezB > 1.0) {
            derezB = 1.0;
        }
        derezB = 1.0 / ((int)(1.0 / derezB));
        double bassGain = E * 2.0;
        bassGain *= bassGain;
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
            bezA[bez_SampL] += ((inputSampleL + bezA[bez_InL]) * derezA);
            bezA[bez_InL] = inputSampleL;
            bezA[bez_SampR] += ((inputSampleR + bezA[bez_InR]) * derezA);
            bezA[bez_InR] = inputSampleR;
            if (bezA[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bezA[bez_cycle] = 0.0;
                bezA[bez_CL] = bezA[bez_BL];
                bezA[bez_BL] = bezA[bez_AL];
                bezA[bez_AL] = inputSampleL;
                bezA[bez_SampL] = 0.0;
                bezA[bez_CR] = bezA[bez_BR];
                bezA[bez_BR] = bezA[bez_AR];
                bezA[bez_AR] = inputSampleR;
                bezA[bez_SampR] = 0.0;
            }
            double CBL = (bezA[bez_CL] * (1.0 - bezA[bez_cycle])) + (bezA[bez_BL] * bezA[bez_cycle]);
            double BAL = (bezA[bez_BL] * (1.0 - bezA[bez_cycle])) + (bezA[bez_AL] * bezA[bez_cycle]);
            double CBAL = (bezA[bez_BL] + (CBL * (1.0 - bezA[bez_cycle])) + (BAL * bezA[bez_cycle])) * 0.5;
            double midL = CBAL;
            double trebleL = inputSampleL - CBAL;
            double CBR = (bezA[bez_CR] * (1.0 - bezA[bez_cycle])) + (bezA[bez_BR] * bezA[bez_cycle]);
            double BAR = (bezA[bez_BR] * (1.0 - bezA[bez_cycle])) + (bezA[bez_AR] * bezA[bez_cycle]);
            double CBAR = (bezA[bez_BR] + (CBR * (1.0 - bezA[bez_cycle])) + (BAR * bezA[bez_cycle])) * 0.5;
            double midR = CBAR;
            double trebleR = inputSampleR - CBAR;
            bezB[bez_cycle] += derezB;
            bezB[bez_SampL] += ((midL + bezB[bez_InL]) * derezB);
            bezB[bez_InL] = midL;
            bezB[bez_SampR] += ((midR + bezB[bez_InR]) * derezB);
            bezB[bez_InR] = midR;
            if (bezB[bez_cycle] > 1.0) { // hit the end point and we do a reverb sample
                bezB[bez_cycle] = 0.0;
                bezB[bez_CL] = bezB[bez_BL];
                bezB[bez_BL] = bezB[bez_AL];
                bezB[bez_AL] = inputSampleL;
                bezB[bez_SampL] = 0.0;
                bezB[bez_CR] = bezB[bez_BR];
                bezB[bez_BR] = bezB[bez_AR];
                bezB[bez_AR] = inputSampleR;
                bezB[bez_SampR] = 0.0;
            }
            CBL = (bezB[bez_CL] * (1.0 - bezB[bez_cycle])) + (bezB[bez_BL] * bezB[bez_cycle]);
            BAL = (bezB[bez_BL] * (1.0 - bezB[bez_cycle])) + (bezB[bez_AL] * bezB[bez_cycle]);
            CBAL = (bezB[bez_BL] + (CBL * (1.0 - bezB[bez_cycle])) + (BAL * bezB[bez_cycle])) * 0.5;
            double bassL = CBAL;
            midL -= bassL;
            CBR = (bezB[bez_CR] * (1.0 - bezB[bez_cycle])) + (bezB[bez_BR] * bezB[bez_cycle]);
            BAR = (bezB[bez_BR] * (1.0 - bezB[bez_cycle])) + (bezB[bez_AR] * bezB[bez_cycle]);
            CBAR = (bezB[bez_BR] + (CBR * (1.0 - bezB[bez_cycle])) + (BAR * bezB[bez_cycle])) * 0.5;
            double bassR = CBAR;
            midR -= bassR;
            inputSampleL = (bassL * bassGain) + (midL * midGain) + (trebleL * trebleGain);
            inputSampleR = (bassR * bassGain) + (midR * midGain) + (trebleR * trebleGain);
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
} // namespace airwindohhs::bezeq
