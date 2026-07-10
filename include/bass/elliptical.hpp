#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::elliptical {

constexpr std::string_view k_name{ "Elliptical" };
constexpr std::string_view k_short_description{
    "Elliptical highpasses the side channel."
};
constexpr std::string_view k_long_description{
    "So this is by request: I got asked whether it was possible for me to make a great elliptical EQ. And of course I said 'well, ToVinyl does that though it also does some other things', and it's been a while since I did ToVinyl plus it's partly designed to produce sound as if it's FROM vinyl, with the groovewear control (which itself became other plugins like GrooveWear and CrunchyGrooveWear), and what if I was able to do a standalone elliptical EQ that's way better?Here's how I did. ToVinyl uses a weird early technique I liked, trying to steepen a very simple IIR filter by using many poles: this would be great if it worked! Instead, it doesn't really steepen nearly as much, and then it's implemented in a way where it kinda resonates. End result is almost a 'color' EQ, with huge amounts of phase shifting: it will take a distorted bassy clipped noise and turn it into giant bass peaks over a wide range, and that's ToVinyl.And so I was asked back then if I could make a better highpass, just a stereo mastering highpass, by Gregg of Hermetech Mastering, and Hermepass took shape, with way fewer poles but arranged differently: going up to the desired point, but then with each new pole dropping back by the Golden Ratio towards roughly 20 hz at the lowest, and lastly the poles got introduced with mini dry/wet controls between each step, something that got used in the four stages of cascaded filtering in the Z series filters, but Hermepass had this for six stages of increasingly deeper cutoff.It remains a solid, tonally clear highpass filter: not a brickwall, but one where you can cut the lows and really not hear where the cut is. That's the intention. And that's where I got the highpass for Elliptical. If you like the one you'll like the other. The interesting thing about using it for an elliptical filter is, you can filter incredibly hard without hearing it change the sound. In demonstrating it I had to make use of Golem, to show an out-of-phase sound that's literally just the side channel. Otherwise, it fools you into thinking everything's still full bass.And of course that's the goal with an elliptical filter: these aren't about sound effects, they're about stopping the side channel from modulating the depth of a vinyl groove, or about using stereo woofers most efficiently. And that's where Elliptical shines. Keep Golem handy if you have to spot check what the filter's doing, find useful settings and then trust that even though it sounds like there's a lot of heavy stereo bass, it's really just an illusion and Elliptical is controlling the heck out of it, while pretending the sound is unchanged :)"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class Elliptical final : public Effect<T>
{
    float A;
    float B;
    double iirA;
    double iirB; // first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
    double iirC; // we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
    double iirD;
    double iirE;
    double iirF; // our slope control will have a pow() on it so that the high orders are way to the right side
    double iirG;
    double iirH; // seven poles max, and the final pole is always at 20hz directly.
    bool fpFlip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Elliptical()
    {
        A = 0.5;
        B = 0.5;
        iirA = 0.0;
        iirB = 0.0;
        iirC = 0.0;
        iirD = 0.0;
        iirE = 0.0;
        iirF = 0.0;
        iirG = 0.0;
        iirH = 0.0;
        fpFlip = true;
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
        kNumParameters = 2
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "cutoff"; break;
            case kParamB: return "slope"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Cutoff"; break;
            case kParamB: return "Slope"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double rangescale = 0.1 / overallscale;
        double cutoff = pow(A, 3);
        double slope = pow(B, 3) * 6.0;
        double newA = cutoff * rangescale;
        double newB = newA; // other part of interleaved IIR is the same
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double newC = cutoff * rangescale; // first extra pole is the same
        cutoff = (cutoff * fpOld) + (0.00001 * (1.0 - fpOld));
        double newD = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * (1.0 - fpOld));
        double newE = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * (1.0 - fpOld));
        double newF = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * (1.0 - fpOld));
        double newG = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * (1.0 - fpOld));
        double newH = cutoff * rangescale;
        // converge toward the unvarying fixed cutoff value
        double polesC = slope;
        if (slope > 1.0) {
            polesC = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        double polesD = slope;
        if (slope > 1.0) {
            polesD = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        double polesE = slope;
        if (slope > 1.0) {
            polesE = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        double polesF = slope;
        if (slope > 1.0) {
            polesF = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        double polesG = slope;
        if (slope > 1.0) {
            polesG = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        double polesH = slope;
        if (slope > 1.0) {
            polesH = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        // each one will either be 0.0, the fractional slope value, or 1
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double mid = inputSampleL + inputSampleR;
            double side = inputSampleL - inputSampleR;
            // assign mid and side.Between these sections, you can do mid/side processing
            double temp = side;
            double correction;
            if (fpFlip) {
                iirA = (iirA * (1.0 - newA)) + (temp * newA);
                temp -= iirA;
                correction = iirA;
            }
            else {
                iirB = (iirB * (1.0 - newB)) + (temp * newB);
                temp -= iirB;
                correction = iirB;
            }
            iirC = (iirC * (1.0 - newC)) + (temp * newC);
            temp -= iirC;
            iirD = (iirD * (1.0 - newD)) + (temp * newD);
            temp -= iirD;
            iirE = (iirE * (1.0 - newE)) + (temp * newE);
            temp -= iirE;
            iirF = (iirF * (1.0 - newF)) + (temp * newF);
            temp -= iirF;
            iirG = (iirG * (1.0 - newG)) + (temp * newG);
            temp -= iirG;
            iirH = (iirH * (1.0 - newH)) + (temp * newH);
            temp -= iirH;
            // set up all the iir filters in case they are used
            if (polesC == 1.0) {
                correction += iirC;
            }
            if (polesC > 0.0 && polesC < 1.0) {
                correction += (iirC * polesC);
            }
            if (polesD == 1.0) {
                correction += iirD;
            }
            if (polesD > 0.0 && polesD < 1.0) {
                correction += (iirD * polesD);
            }
            if (polesE == 1.0) {
                correction += iirE;
            }
            if (polesE > 0.0 && polesE < 1.0) {
                correction += (iirE * polesE);
            }
            if (polesF == 1.0) {
                correction += iirF;
            }
            if (polesF > 0.0 && polesF < 1.0) {
                correction += (iirF * polesF);
            }
            if (polesG == 1.0) {
                correction += iirG;
            }
            if (polesG > 0.0 && polesG < 1.0) {
                correction += (iirG * polesG);
            }
            if (polesH == 1.0) {
                correction += iirH;
            }
            if (polesH > 0.0 && polesH < 1.0) {
                correction += (iirH * polesH);
            }
            // each of these are added directly if they're fully engaged,
            // multiplied by 0-1 if they are the interpolated one, or skipped if they are beyond the interpolated one.
            // the purpose is to do all the math at the floating point exponent nearest to the tiny value in use.
            // also, it's formatted that way to easily substitute the next variable: this could be written as a loop
            // with everything an array value. However, this makes just as much sense for this few poles.
            side -= correction;
            fpFlip = !fpFlip;
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
            // unassign mid and side
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
} // namespace airwindohhs::elliptical
