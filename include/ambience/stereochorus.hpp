#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::stereochorus {

constexpr std::string_view k_name{ "StereoChorus" };
constexpr std::string_view k_short_description{
    "StereoChorus is a nice basic stereo chorus."
};
constexpr std::string_view k_long_description{
    "Hi! This plugin is actually the one that kicked off the work that gave you StereoEnsemble. I got a request: please give me StereoChorus, like you used to have back in the Kagi days, I need to use it as nothing else will do.It’s a bit funny as I didn’t remember it being that special. I mean, it’s got some odd tricks in the interpolation, it scales according to chorus speed so all the settings feel about equally intense, but it didn’t seem to me like anything that amazing, so I hadn’t got round to porting it to VST and open-sourcing it.So now I have. It’s still got some of the interesting choices I made back when I coded it: for instance, it’s actually running a fixed point buffer at a rather long word length. Maybe this has something to do with the sound my user wanted to have back? However, I’ve added a few things. It’s now got modern dithering to floating point (on 32 bit busses) and I’ve added undersampling… so that it can sound the way it was meant to, even at elevated sample rates, while using lower CPU to do it. All in all it’s not the most outrageous plugin, but you know I’ve got folks fond of specifically it, and so I’ve brought it to VST form and open-sourced it just as it was, and maybe you too will find something special in it."
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class StereoChorus final : public Effect<T>
{
    int pL[65536];
    int pR[65536];
    double sweepL;
    double sweepR;
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
    double lastRefL[7];
    double lastRefR[7];
    int cycle;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    StereoChorus()
    {
        A = 0.0;
        B = 0.0;
        for (int count = 0; count < 65535; count++) {
            pL[count] = 0;
            pR[count] = 0;
        }
        sweepL = 3.141592653589793238 / 2.7;
        sweepR = 3.141592653589793238;
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
        for (int count = 0; count < 6; count++) {
            lastRefL[count] = 0.0;
            lastRefR[count] = 0.0;
        }
        cycle = 0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
            break;
        kParamB:
            return 0.0;
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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        if (cycle > cycleEnd - 1) {
            cycle = cycleEnd - 1; // sanity check
        }
        double speed = pow(0.32 + (A / 6), 10);
        double depth = (B / 60) / speed;
        double tupi = 3.141592653589793238 * 2.0;
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
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do a chorus sample
                // assign working variables
                airFactorL = airPrevL - inputSampleL;
                if (flip) {
                    airEvenL += airFactorL;
                    airOddL -= airFactorL;
                    airFactorL = airEvenL;
                }
                else {
                    airOddL += airFactorL;
                    airEvenL -= airFactorL;
                    airFactorL = airOddL;
                }
                airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
                airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
                airPrevL = inputSampleL;
                inputSampleL += airFactorL;
                // left
                airFactorR = airPrevR - inputSampleR;
                if (flip) {
                    airEvenR += airFactorR;
                    airOddR -= airFactorR;
                    airFactorR = airEvenR;
                }
                else {
                    airOddR += airFactorR;
                    airEvenR -= airFactorR;
                    airFactorR = airOddR;
                }
                airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
                airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
                airPrevR = inputSampleR;
                inputSampleR += airFactorR;
                // right
                flip = !flip;
                // air, compensates for loss of highs in flanger's interpolation
                int tempL = 0;
                int tempR = 0;
                if (gcount < 1 || gcount > 32760) {
                    gcount = 32760;
                }
                int count = gcount;
                pL[count + 32760] = pL[count] = (int)(inputSampleL * 8388352.0);
                // double buffer -8388352 to 8388352 is equal to 24 bit linear space
                double offset = depth + (depth * sin(sweepL));
                count += (int)floor(offset);
                tempL += (int)(pL[count] * (1 - (offset - floor(offset)))); // less as value moves away from .0
                tempL += pL[count + 1]; // we can assume always using this in one way or another?
                tempL += (int)(pL[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
                tempL -= (int)(((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
                // left
                count = gcount;
                pR[count + 32760] = pR[count] = (int)(inputSampleR * 8388352.0);
                // double buffer -8388352 to 8388352 is equal to 24 bit linear space
                offset = depth + (depth * sin(sweepR));
                count += (int)floor(offset);
                tempR += (int)(pR[count] * (1 - (offset - floor(offset)))); // less as value moves away from .0
                tempR += pR[count + 1]; // we can assume always using this in one way or another?
                tempR += (int)(pR[count + 2] * (offset - floor(offset))); // greater as value moves away from .0
                tempR -= (int)(((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
                // right
                sweepL += speed;
                sweepR += speed;
                if (sweepL > tupi) {
                    sweepL -= tupi;
                }
                if (sweepR > tupi) {
                    sweepR -= tupi;
                }
                gcount--;
                // still scrolling through the samples, remember
                inputSampleL = ((double)(tempL / 16776704.0));
                inputSampleR = ((double)(tempR / 16776704.0));
                if (cycleEnd == 4) {
                    lastRefL[0] = lastRefL[4]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[1] = (lastRefL[0] + lastRefL[2]) / 2; // one quarter
                    lastRefL[3] = (lastRefL[2] + inputSampleL) / 2; // three quarters
                    lastRefL[4] = inputSampleL; // full
                    lastRefR[0] = lastRefR[4]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[1] = (lastRefR[0] + lastRefR[2]) / 2; // one quarter
                    lastRefR[3] = (lastRefR[2] + inputSampleR) / 2; // three quarters
                    lastRefR[4] = inputSampleR; // full
                }
                if (cycleEnd == 3) {
                    lastRefL[0] = lastRefL[3]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + lastRefL[0] + inputSampleL) / 3; // third
                    lastRefL[1] = (lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
                    lastRefL[3] = inputSampleL; // full
                    lastRefR[0] = lastRefR[3]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + lastRefR[0] + inputSampleR) / 3; // third
                    lastRefR[1] = (lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
                    lastRefR[3] = inputSampleR; // full
                }
                if (cycleEnd == 2) {
                    lastRefL[0] = lastRefL[2]; // start from previous last
                    lastRefL[1] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[2] = inputSampleL; // full
                    lastRefR[0] = lastRefR[2]; // start from previous last
                    lastRefR[1] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[2] = inputSampleR; // full
                }
                if (cycleEnd == 1) {
                    lastRefL[0] = inputSampleL;
                    lastRefR[0] = inputSampleR;
                }
                cycle = 0; // reset
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
            }
            else {
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
                // we are going through our references now
            }
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
} // namespace airwindohhs::stereochorus
