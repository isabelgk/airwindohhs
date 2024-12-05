#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hermepass {

constexpr std::string_view k_name{ "Hermepass" };
constexpr std::string_view k_short_description{
    "Hermepass is a mastering highpass to set by ear only."
};
constexpr std::string_view k_long_description{
    "This is specially by request from Gregg of Hermetech Mastering: it’s my try at a specific plugin he wanted. He wasn’t able to find a fantastic-sounding highpass with JUST a frequency control and slope, no bling, no other stuff, as small as possible.This is of course right up my alley :)All the more when he responded warmly to my idea of having both the cutoff and slope have NO LABELING to guide you: just 0-1 sliders. You have to listen. That’s the whole point, and I delighted in taking it that one stage further.Two stages further, because I worked out how to make the slope control continuous. It uses up to six poles (staggered, an idea that Gregg and I independently came up with: it’s present in my ToVinyl2 and ToVinyl3) but as each pole is added it gets its own little dry/wet internally, so you can have two and a half or four and a third poles. Smooth continuous adjustment of how many poles (and how steep the slope), much like my bit-crusher has continuous sample rate crush and bit depth crush.Three stages since it uses Airwindows interleaved IIR filtering for the very first stage to start off at a slope even shallower than one pole of IIR: sort of ‘half a pole of filtering’ to start off.The controls are set up to give useful results around the middle of their travel. I’m not sure exactly how many poles that is, or what frequency: use your ears, says me and Gregg (for whom I made this). I do know that depending on how you set it, the transparent cutting of extreme low frequencies WILL give rise to higher peaks, so either gain stage it or use limiting or clipping. This plugin is not a loudenator. It’s a tone shaper, for retaining every possible bit of tonality while reshaping the extreme lows to trim the lowest frequencies: it’s sound balancing, not ‘make louder-ing’."
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class Hermepass final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirAL;
    double iirBL; // first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
    double iirCL; // we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
    double iirDL;
    double iirEL;
    double iirFL; // our slope control will have a pow() on it so that the high orders are way to the right side
    double iirGL;
    double iirHL; // seven poles max, and the final pole is always at 20hz directly.
    double iirAR;
    double iirBR; // first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
    double iirCR; // we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
    double iirDR;
    double iirER;
    double iirFR; // our slope control will have a pow() on it so that the high orders are way to the right side
    double iirGR;
    double iirHR; // seven poles max, and the final pole is always at 20hz directly.
    bool fpFlip;
    float A;
    float B;

  public:
    Hermepass()
    {
        A = 0.5;
        B = 0.5;
        iirAL = 0.0;
        iirBL = 0.0;
        iirCL = 0.0;
        iirDL = 0.0;
        iirEL = 0.0;
        iirFL = 0.0;
        iirGL = 0.0;
        iirHL = 0.0;
        iirAR = 0.0;
        iirBR = 0.0;
        iirCR = 0.0;
        iirDR = 0.0;
        iirER = 0.0;
        iirFR = 0.0;
        iirGR = 0.0;
        iirHR = 0.0;
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
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "cutoff"; break;
            case kParamB: return "slope"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Cutoff"; break;
            case kParamB: return "Slope"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return " "; break;
            case kParamB: return " "; break;

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
        double fpOld = 0.618033988749894848204586; // golden ratio!
        double fpNew = 1.0 - fpOld;
        double rangescale = 0.1 / overallscale;
        double cutoff = pow(A, 3);
        double slope = pow(B, 3) * 6.0;
        double newA = cutoff * rangescale;
        double newB = newA; // other part of interleaved IIR is the same
        double newC = cutoff * rangescale; // first extra pole is the same
        cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
        double newD = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
        double newE = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
        double newF = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
        double newG = cutoff * rangescale;
        cutoff = (cutoff * fpOld) + (0.00001 * fpNew);
        double newH = cutoff * rangescale;
        // converge toward the unvarying fixed cutoff value
        double oldA = 1.0 - newA;
        double oldB = 1.0 - newB;
        double oldC = 1.0 - newC;
        double oldD = 1.0 - newD;
        double oldE = 1.0 - newE;
        double oldF = 1.0 - newF;
        double oldG = 1.0 - newG;
        double oldH = 1.0 - newH;
        double polesC;
        double polesD;
        double polesE;
        double polesF;
        double polesG;
        double polesH;
        polesC = slope;
        if (slope > 1.0) {
            polesC = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        polesD = slope;
        if (slope > 1.0) {
            polesD = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        polesE = slope;
        if (slope > 1.0) {
            polesE = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        polesF = slope;
        if (slope > 1.0) {
            polesF = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        polesG = slope;
        if (slope > 1.0) {
            polesG = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        polesH = slope;
        if (slope > 1.0) {
            polesH = 1.0;
        }
        slope -= 1.0;
        if (slope < 0.0) {
            slope = 0.0;
        }
        // each one will either be 0.0, the fractional slope value, or 1
        double inputSampleL;
        double inputSampleR;
        double tempSampleL;
        double tempSampleR;
        double correction;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            tempSampleL = inputSampleL;
            tempSampleR = inputSampleR;
            // begin L channel
            if (fpFlip) {
                iirAL = (iirAL * oldA) + (tempSampleL * newA);
                tempSampleL -= iirAL;
                correction = iirAL;
            }
            else {
                iirBL = (iirBL * oldB) + (tempSampleL * newB);
                tempSampleL -= iirBL;
                correction = iirBL;
            }
            iirCL = (iirCL * oldC) + (tempSampleL * newC);
            tempSampleL -= iirCL;
            iirDL = (iirDL * oldD) + (tempSampleL * newD);
            tempSampleL -= iirDL;
            iirEL = (iirEL * oldE) + (tempSampleL * newE);
            tempSampleL -= iirEL;
            iirFL = (iirFL * oldF) + (tempSampleL * newF);
            tempSampleL -= iirFL;
            iirGL = (iirGL * oldG) + (tempSampleL * newG);
            tempSampleL -= iirGL;
            iirHL = (iirHL * oldH) + (tempSampleL * newH);
            tempSampleL -= iirHL;
            // set up all the iir filters in case they are used
            if (polesC == 1.0) {
                correction += iirCL;
            }
            if (polesC > 0.0 && polesC < 1.0) {
                correction += (iirCL * polesC);
            }
            if (polesD == 1.0) {
                correction += iirDL;
            }
            if (polesD > 0.0 && polesD < 1.0) {
                correction += (iirDL * polesD);
            }
            if (polesE == 1.0) {
                correction += iirEL;
            }
            if (polesE > 0.0 && polesE < 1.0) {
                correction += (iirEL * polesE);
            }
            if (polesF == 1.0) {
                correction += iirFL;
            }
            if (polesF > 0.0 && polesF < 1.0) {
                correction += (iirFL * polesF);
            }
            if (polesG == 1.0) {
                correction += iirGL;
            }
            if (polesG > 0.0 && polesG < 1.0) {
                correction += (iirGL * polesG);
            }
            if (polesH == 1.0) {
                correction += iirHL;
            }
            if (polesH > 0.0 && polesH < 1.0) {
                correction += (iirHL * polesH);
            }
            // each of these are added directly if they're fully engaged,
            // multiplied by 0-1 if they are the interpolated one, or skipped if they are beyond the interpolated one.
            // the purpose is to do all the math at the floating point exponent nearest to the tiny value in use.
            // also, it's formatted that way to easily substitute the next variable: this could be written as a loop
            // with everything an array value. However, this makes just as much sense for this few poles.
            inputSampleL -= correction;
            // end L channel
            // begin R channel
            if (fpFlip) {
                iirAR = (iirAR * oldA) + (tempSampleR * newA);
                tempSampleR -= iirAR;
                correction = iirAR;
            }
            else {
                iirBR = (iirBR * oldB) + (tempSampleR * newB);
                tempSampleR -= iirBR;
                correction = iirBR;
            }
            iirCR = (iirCR * oldC) + (tempSampleR * newC);
            tempSampleR -= iirCR;
            iirDR = (iirDR * oldD) + (tempSampleR * newD);
            tempSampleR -= iirDR;
            iirER = (iirER * oldE) + (tempSampleR * newE);
            tempSampleR -= iirER;
            iirFR = (iirFR * oldF) + (tempSampleR * newF);
            tempSampleR -= iirFR;
            iirGR = (iirGR * oldG) + (tempSampleR * newG);
            tempSampleR -= iirGR;
            iirHR = (iirHR * oldH) + (tempSampleR * newH);
            tempSampleR -= iirHR;
            // set up all the iir filters in case they are used
            if (polesC == 1.0) {
                correction += iirCR;
            }
            if (polesC > 0.0 && polesC < 1.0) {
                correction += (iirCR * polesC);
            }
            if (polesD == 1.0) {
                correction += iirDR;
            }
            if (polesD > 0.0 && polesD < 1.0) {
                correction += (iirDR * polesD);
            }
            if (polesE == 1.0) {
                correction += iirER;
            }
            if (polesE > 0.0 && polesE < 1.0) {
                correction += (iirER * polesE);
            }
            if (polesF == 1.0) {
                correction += iirFR;
            }
            if (polesF > 0.0 && polesF < 1.0) {
                correction += (iirFR * polesF);
            }
            if (polesG == 1.0) {
                correction += iirGR;
            }
            if (polesG > 0.0 && polesG < 1.0) {
                correction += (iirGR * polesG);
            }
            if (polesH == 1.0) {
                correction += iirHR;
            }
            if (polesH > 0.0 && polesH < 1.0) {
                correction += (iirHR * polesH);
            }
            // each of these are added directly if they're fully engaged,
            // multiplied by 0-1 if they are the interpolated one, or skipped if they are beyond the interpolated one.
            // the purpose is to do all the math at the floating point exponent nearest to the tiny value in use.
            // also, it's formatted that way to easily substitute the next variable: this could be written as a loop
            // with everything an array value. However, this makes just as much sense for this few poles.
            inputSampleR -= correction;
            // end R channel
            fpFlip = !fpFlip;
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
} // namespace airwindohhs::hermepass
