#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::stereodoubler {

constexpr std::string_view k_name{ "StereoDoubler" };
constexpr std::string_view k_short_description{
    "StereoDoubler is like GlitchShifter optimized for pitch shift doubling and tripling."
};
constexpr std::string_view k_long_description{
    "StereoDoubler is another retro plugin I’ve had for a while, that is now available as open source and VST2 and M1 Mac and Raspberry Pi and so on. It’s using the basic concept of Glitch Shifter, so I should explain what that is first.Glitch Shifter is my plugin for doing pitch shifting (and feedback on it, if you like) in a different way. Instead of smoothly interpolating over relatively small loops of sound to pitch shift, it works with potentially much larger loops, and searches for spots where it can seamlessly (or near-seamlessly) switch over without ever blending or blurring the sound. For that reason, it’s more up front and edgy, more personality, but it can also disconnect from the source audio in weird ways or glitch out like mad, hence the name.StereoDoubler’s like two of those, tamed. Well, mostly tamed. It takes the source audio, and gives you a pitched-up version in one channel, a pitched-down version in the other, and lets you bring in dry for a center channel if you want. Because it’s still Glitch Shifter, it’ll give you faint ticking noises if it’s struggling to make its loops work, but it’s a lot tighter and more normal than Glitch Shifter usually is, and it’s simultaneously shifting up and down so the two sides will each have their own distinct glitch ‘personality’ while being as upfront and direct as they possibly can.I hope you like it. Sometimes taking a wild experiment and reining it in a bit, is just the thing. StereoDoubler isn’t meant to work on every possible situation, it’s designed to be amazing when it’s in its element. Maybe your mix is its element :)"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class StereoDoubler final : public Effect<T>
{
    double pL[5001];
    double pR[5001];
    int gcountL;
    int lastcountL;
    int gcountR;
    int lastcountR;
    int prevwidth;
    double trackingL[9];
    double tempL[9];
    double positionL[9];
    double lastpositionL[9];
    double trackingR[9];
    double tempR[9];
    double positionR[9];
    double lastpositionR[9];
    int activeL;
    int bestspliceL;
    int activeR;
    int bestspliceR;
    double bestyetL;
    double bestyetR;
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
    StereoDoubler()
    {
        A = 0.6;
        B = 0.6;
        for (int count = 0; count < 5000; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        for (int count = 0; count < 8; count++)
        {
            tempL[count] = 0.0;
            positionL[count] = 0.0;
            lastpositionL[count] = 0.0;
            trackingL[count] = 0.0;
        }
        for (int count = 0; count < 8; count++)
        {
            tempR[count] = 0.0;
            positionR[count] = 0.0;
            lastpositionR[count] = 0.0;
            trackingR[count] = 0.0;
        }
        gcountL = 0;
        lastcountL = 0;
        gcountR = 0;
        lastcountR = 0;
        prevwidth = 643;
        activeL = 0;
        bestspliceL = 4;
        activeR = 0;
        bestspliceR = 4;
        bestyetL = 1.0;
        bestyetR = 1.0;
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
            case kParamA: return 0.6; break;
            case kParamB: return 0.6; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "detune"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Detune"; break;
            case kParamB: return "Dry/Wet"; break;

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
        double trim = (A * 2.0) - 1.0;
        trim *= fabs(trim);
        trim /= 40;
        double speedL = trim + 1.0;
        double speedR = (-trim) + 1.0;
        if (speedL < 0.0) {
            speedL = 0.0;
        }
        if (speedR < 0.0) {
            speedR = 0.0;
        }
        int width = 2300;
        if (prevwidth != width)
        {
            positionL[0] = 0;
            positionL[1] = (int)(width / 3);
            positionL[2] = (int)((width / 3) * 2);
            positionL[3] = (int)(width / 5);
            positionL[4] = (int)((width / 5) * 2);
            positionL[5] = (int)((width / 5) * 3);
            positionL[6] = (int)((width / 5) * 4);
            positionL[7] = (int)(width / 2);
            positionR[0] = 0;
            positionR[1] = (int)(width / 3);
            positionR[2] = (int)((width / 3) * 2);
            positionR[3] = (int)(width / 5);
            positionR[4] = (int)((width / 5) * 2);
            positionR[5] = (int)((width / 5) * 3);
            positionR[6] = (int)((width / 5) * 4);
            positionR[7] = (int)(width / 2);
            prevwidth = width;
        }
        double adjust = 1100;
        double wet = B;
        int gplusL;
        int lastplusL;
        int gplusR;
        int lastplusR;
        double posplusL;
        double lastposplusL;
        double posplusR;
        double lastposplusR;
        double depth;
        double crossfade;
        int count;
        int bcountL;
        int bcountR;
        int base;
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
            if (cycle == cycleEnd) { // hit the end point and we do a doubler sample
                double drySampleL = inputSampleL;
                double drySampleR = inputSampleR;
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
                gcountL++;
                gcountR++;
                for (count = 0; count < 8; count++) {
                    positionL[count] += speedL;
                    positionR[count] += speedR;
                }
                gplusL = gcountL + (int)adjust;
                lastplusL = lastcountL + (int)adjust;
                if (gplusL > width) {
                    gplusL -= width;
                }
                if (lastplusL > width) {
                    lastplusL -= width;
                }
                gplusR = gcountR + (int)adjust;
                lastplusR = lastcountR + (int)adjust;
                if (gplusR > width) {
                    gplusR -= width;
                }
                if (lastplusR > width) {
                    lastplusR -= width;
                }
                if (trackingL[activeL] == 0.0)
                {
                    posplusL = positionL[activeL] + adjust;
                    lastposplusL = lastpositionL[activeL] + adjust;
                    if (posplusL > width) {
                        posplusL -= width;
                    }
                    if (lastposplusL > width) {
                        lastposplusL -= width;
                    }
                    if ((gplusL > positionL[activeL]) && (lastplusL < lastpositionL[activeL])) {
                        trackingL[activeL] = 1.0;
                    }
                    if ((posplusL > gcountL) && (lastposplusL < lastcountL)) {
                        trackingL[activeL] = 1.0;
                    }
                    // fire splice based on whether somebody moved past somebody else just now
                }
                if (trackingR[activeR] == 0.0)
                {
                    posplusR = positionR[activeR] + adjust;
                    lastposplusR = lastpositionR[activeR] + adjust;
                    if (posplusR > width) {
                        posplusR -= width;
                    }
                    if (lastposplusR > width) {
                        lastposplusR -= width;
                    }
                    if ((gplusR > positionR[activeR]) && (lastplusR < lastpositionR[activeR])) {
                        trackingR[activeR] = 1.0;
                    }
                    if ((posplusR > gcountR) && (lastposplusR < lastcountR)) {
                        trackingR[activeR] = 1.0;
                    }
                    // fire splice based on whether somebody moved past somebody else just now
                }
                for (count = 0; count < 8; count++)
                {
                    if (positionL[count] > width) {
                        positionL[count] -= width;
                    }
                    if (positionR[count] > width) {
                        positionR[count] -= width;
                    }
                    lastpositionL[count] = positionL[count];
                    lastpositionR[count] = positionR[count];
                }
                if (gcountL < 0 || gcountL > width) {
                    gcountL -= width;
                }
                lastcountL = bcountL = gcountL;
                if (gcountR < 0 || gcountR > width) {
                    gcountR -= width;
                }
                lastcountR = bcountR = gcountR;
                pL[bcountL + width] = pL[bcountL] = inputSampleL;
                pR[bcountR + width] = pR[bcountR] = inputSampleR;
                for (count = 0; count < 8; count++)
                {
                    base = (int)floor(positionL[count]);
                    tempL[count] = (pL[base] * (1 - (positionL[count] - base))); // less as value moves away from .0
                    tempL[count] += pL[base + 1]; // we can assume always using this in one way or another?
                    tempL[count] += (pL[base + 2] * (positionL[count] - base)); // greater as value moves away from .0
                    tempL[count] -= (((pL[base] - pL[base + 1]) - (pL[base + 1] - pL[base + 2])) / 50); // interpolation hacks 'r us
                    tempL[count] /= 2;
                    base = (int)floor(positionR[count]);
                    tempR[count] = (pR[base] * (1 - (positionR[count] - base))); // less as value moves away from .0
                    tempR[count] += pR[base + 1]; // we can assume always using this in one way or another?
                    tempR[count] += (pR[base + 2] * (positionR[count] - base)); // greater as value moves away from .0
                    tempR[count] -= (((pR[base] - pR[base + 1]) - (pR[base + 1] - pR[base + 2])) / 50); // interpolation hacks 'r us
                    tempR[count] /= 2;
                }
                if (trackingL[activeL] > 0.0)
                {
                    crossfade = sin(trackingL[bestspliceL] * 1.57);
                    inputSampleL = (tempL[activeL] * crossfade) + (tempL[bestspliceL] * (1.0 - crossfade));
                    for (count = 0; count < 8; count++)
                    {
                        depth = (0.5 - fabs(tempL[activeL] - tempL[count]));
                        if ((depth > 0) && (count != activeL))
                        {
                            trackingL[count] -= (depth / adjust);
                            bestspliceL = count;
                        }
                        // take down the splicings but skip the current one
                    }
                    bestyetL = 1.0;
                    for (count = 0; count < 8; count++)
                    {
                        if ((trackingL[count] < bestyetL) && (count != activeL))
                        {
                            bestspliceL = count;
                            bestyetL = trackingL[count];
                        }
                    }
                    if (trackingL[bestspliceL] < 0.0)
                    {
                        for (count = 0; count < 8; count++)
                        {
                            trackingL[count] = 1.0;
                        }
                        activeL = bestspliceL;
                        trackingL[activeL] = 0.0;
                    }
                }
                else
                {
                    inputSampleL = tempL[activeL];
                }
                if (trackingR[activeR] > 0.0)
                {
                    crossfade = sin(trackingR[bestspliceR] * 1.57);
                    inputSampleR = (tempR[activeR] * crossfade) + (tempR[bestspliceR] * (1.0 - crossfade));
                    for (count = 0; count < 8; count++)
                    {
                        depth = (0.5 - fabs(tempR[activeR] - tempR[count]));
                        if ((depth > 0) && (count != activeR))
                        {
                            trackingR[count] -= (depth / adjust);
                            bestspliceR = count;
                        }
                        // take down the splicings but skip the current one
                    }
                    bestyetR = 1.0;
                    for (count = 0; count < 8; count++)
                    {
                        if ((trackingR[count] < bestyetR) && (count != activeR))
                        {
                            bestspliceR = count;
                            bestyetR = trackingR[count];
                        }
                    }
                    if (trackingR[bestspliceR] < 0.0)
                    {
                        for (count = 0; count < 8; count++)
                        {
                            trackingR[count] = 1.0;
                        }
                        activeR = bestspliceR;
                        trackingR[activeR] = 0.0;
                    }
                }
                else
                {
                    inputSampleR = tempR[activeR];
                }
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::stereodoubler
