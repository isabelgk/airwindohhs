#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::vibrato {

constexpr std::string_view k_name{ "Vibrato" };
constexpr std::string_view k_short_description{
    "Vibrato lets you vibrato, chorus, flange, and make odd FM noises."
};
constexpr std::string_view k_long_description{
    "The heart of Vibrato is the Airwindows moving-delay-tap interpolation code also found in Chorus and Flanger, but here there’s some extra functionality plus ways to partially simulate those other plugins: while you can make the full-wet sound do a nice vibrato (automate to taste), there are other fun things to do. You can use the dry/wet to get a chorusing effect, or with less depth, a flange: or set it to ‘inverse’ to get the same but with a through-zero flange that’ll cancel almost totally to silence. This can also be used as an interesting sort of highpass (or of course, using normal wet, as a sort of lowpassy effect). That’s all with the main vibrato control, which has an extremely wide speed range.Or, if that’s not enough, you can bring in the FM vibrato. This has the same type of speed control, and a depth control of its own, but instead of affecting the audio directly, it affects the speed of the main vibrato. You can use this at low speeds to provide an interesting modulation to the main vibrato, flange or chorus, or crank it up to produce distinctive overtones. And again, automate it to do even more interesting things.Vibrato is a nice little utility plugin, and I think it deserves a place as a go-to ‘time modulation’ plugin for people who have a solid understanding of how these effects work. It’s not hugely complex, or specialized, and it’s just complicated enough to be sophisticated. Want a lush chorus? Vibrato. Warbly effect? Vibrato. Thin things out in a way that sounds airy and interesting? Vibrato, inverse-zone, near 0.5 for maximum effect. Funny overtones and resonances? Vibrato, full-wet, up in the audio range. Even more metallic? Bring in some FM."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class Vibrato final : public Effect<T>
{
    double pL[16386]; // this is processed, not raw incoming samples
    double pR[16386]; // this is processed, not raw incoming samples
    double sweep;
    double sweepB;
    int gcount;
    double airPrevL;
    double airEvenL;
    double airOddL;
    double airFactorL;
    double airPrevR;
    double airEvenR;
    double airOddR;
    double airFactorR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Vibrato()
    {
        A = 0.3;
        B = 0.0;
        C = 0.4;
        D = 0.0;
        E = 1.0;
        for (int count = 0; count < 16385; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        sweep = 3.141592653589793238 / 2.0;
        sweepB = 3.141592653589793238 / 2.0;
        gcount = 0;
        airPrevL = 0.0;
        airEvenL = 0.0;
        airOddL = 0.0;
        airFactorL = 0.0;
        airPrevR = 0.0;
        airEvenR = 0.0;
        airOddR = 0.0;
        airFactorR = 0.0;
        flip = false;
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
            return 0.3;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.4;
            break;
        kParamD:
            return 0.0;
            break;
        kParamE:
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
            return "speed";
            break;
        kParamB:
            return "depth";
            break;
        kParamC:
            return "fmspeed";
            break;
        kParamD:
            return "fmdepth";
            break;
        kParamE:
            return "inv/wet";
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
            return "Speed";
            break;
        kParamB:
            return "Depth";
            break;
        kParamC:
            return "FMSpeed";
            break;
        kParamD:
            return "FMDepth";
            break;
        kParamE:
            return "Inv/Wet";
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

        double speed = pow(0.1 + A, 6);
        double depth = (pow(B, 3) / sqrt(speed)) * 4.0;
        double speedB = pow(0.1 + C, 6);
        double depthB = pow(D, 3) / sqrt(speedB);
        double tupi = 3.141592653589793238 * 2.0;
        double wet = (E * 2.0) - 1.0; // note: inv/dry/wet
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
            airFactorL = airPrevL - inputSampleL;
            airFactorR = airPrevR - inputSampleR;
            if (flip) {
                airEvenL += airFactorL;
                airOddL -= airFactorL;
                airFactorL = airEvenL;
                airEvenR += airFactorR;
                airOddR -= airFactorR;
                airFactorR = airEvenR;
            }
            else {
                airOddL += airFactorL;
                airEvenL -= airFactorL;
                airFactorL = airOddL;
                airOddR += airFactorR;
                airEvenR -= airFactorR;
                airFactorR = airOddR;
            }
            airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
            airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
            airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
            airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
            airPrevL = inputSampleL;
            airPrevR = inputSampleR;
            inputSampleL += airFactorL;
            inputSampleR += airFactorR;
            flip = !flip;
            // air, compensates for loss of highs in the interpolation
            if (gcount < 1 || gcount > 8192) {
                gcount = 8192;
            }
            int count = gcount;
            pL[count + 8192] = pL[count] = inputSampleL;
            pR[count + 8192] = pR[count] = inputSampleR;
            double offset = depth + (depth * sin(sweep));
            count += (int)floor(offset);
            inputSampleL = pL[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleL += pL[count + 1]; // we can assume always using this in one way or another?
            inputSampleL += pL[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
            inputSampleL -= ((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50.0; // interpolation hacks 'r us
            inputSampleL *= 0.5; // gain trim
            inputSampleR = pR[count] * (1.0 - (offset - floor(offset))); // less as value moves away from .0
            inputSampleR += pR[count + 1]; // we can assume always using this in one way or another?
            inputSampleR += pR[count + 2] * (offset - floor(offset)); // greater as value moves away from .0
            inputSampleR -= ((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50.0; // interpolation hacks 'r us
            inputSampleR *= 0.5; // gain trim
            sweep += (speed + (speedB * sin(sweepB) * depthB));
            sweepB += speedB;
            if (sweep > tupi) {
                sweep -= tupi;
            }
            if (sweep < 0.0) {
                sweep += tupi;
            } // through zero FM
            if (sweepB > tupi) {
                sweepB -= tupi;
            }
            gcount--;
            // still scrolling through the samples, remember
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
            }
            // Inv/Dry/Wet control
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::vibrato
