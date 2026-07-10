#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::slew4 {

constexpr std::string_view k_name{ "Slew4" };
constexpr std::string_view k_short_description{
    "Slew4 is tape compression without the tape, for brightness control."
};
constexpr std::string_view k_long_description{
    "You've heard of a cat without a grin and a grin without a cat, and you've heard of plenty of tape plugins without tape compression… but how about the compression without the tape?Slew4 does just one thing, but it does it incredibly well. It finds digital edges and glare and high frequency tizz, and it wipes out JUST that, to whatever extent you like, using new filtering ideas I don't think have been used before in this way. The Airwindows Slew plugins have long been a secret weapon for just this purpose, but this takes it completely beyond anything I had.That's why this is the technology in TapeHack2, and what got included in ConsoleH and ConsoleX2 as hasty updates soon after they came out, and why it's a significant part of ToTape9 which I'm doing everything I can to finish up. Now it's there for you to use and control, without any sort of saturation stage or any other sort of tape modeling, in its purest possible form.One thing that means is, if your sound isn't bright enough, there is no chance you'll ever hear it do anything. With the right kind of vocal track this is a de-esser all by itself. For other vocal tracks, even ones with pronounced esses, you'll find it does absolutely nothing. It ONLY cares about the very highest highs, and excises them so neatly you'd never know they were there.Slew4 runs two samples of lookahead to do what it does, and makes its filter by stacking up averaging filters with even numbers of samples in them. These produce stopbands with big cancellation nodes in them, but when you stack them up, each new cancellation node targets a bump (between nodes) from the previous one… so it becomes a very steep roll-off with good filtering past the cutoff, and no pre-ripple meaning it has incredible time domain performance for something that steep. The strangeness of the stop-band response is probably why this wouldn't have found use before, but it turns out to sound fantastic, especially when it's just reining in the highest highs without touching anything else.If you liked TapeHack2, you already like this! Enjoy playing with it as a dedicated brightness tamer :)"
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Slew4 final : public Effect<T>
{
    float A;
    double slewL[24][12];
    double sustainSlewL;
    double secondSampleL;
    double thirdSampleL;
    double slewR[24][12];
    double sustainSlewR;
    double secondSampleR;
    double thirdSampleR;
    int slewCount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Slew4()
    {
        A = 0.5;
        for (int x = 0; x < 23; x++) {
            for (int y = 0; y < 11; y++) {
                slewL[x][y] = 0.0;
                slewR[x][y] = 0.0;
            }
        }
        sustainSlewL = 0.0;
        secondSampleL = 0.0;
        thirdSampleL = 0.0;
        sustainSlewR = 0.0;
        secondSampleR = 0.0;
        thirdSampleR = 0.0;
        slewCount = 0;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "compres"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Compres"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;

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
        double compresity = pow(A * 0.95, 2.0) * 8.0 * overallscale;
        while (--sampleFrames >= 0) {
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
            // begin L
            sustainSlewL += fabs((inputSampleL - secondSampleL) - (secondSampleL - thirdSampleL));
            sustainSlewL *= 0.9;
            inputSampleL = thirdSampleL; // two sample lookahead
            double depthAmt = fmin(sin(fmin(sustainSlewL, M_PI_2)) * compresity, 9.0);
            int z = floor(depthAmt);
            double depthTrim = depthAmt - (double)z;
            for (int y = 0; y < z; y++) {
                switch (y) {
                    case 0:
                        slewL[slewCount % 2][y] = inputSampleL;
                        inputSampleL = (slewL[0][y] + slewL[1][y]) * 0.5;
                        break;
                    case 1:
                        slewL[slewCount % 4][y] = inputSampleL;
                        inputSampleL = (slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y]) * 0.25;
                        break;
                    case 2:
                        slewL[slewCount % 6][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y];
                        inputSampleL += slewL[3][y] + slewL[4][y] + slewL[5][y];
                        inputSampleL *= 0.1666666666;
                        break;
                    case 3:
                        slewL[slewCount % 8][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y];
                        inputSampleL += slewL[4][y] + slewL[5][y] + slewL[6][y] + slewL[7][y];
                        inputSampleL *= 0.125;
                        break;
                    case 4:
                        slewL[slewCount % 10][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y];
                        inputSampleL += slewL[5][y] + slewL[6][y] + slewL[7][y] + slewL[8][y] + slewL[9][y];
                        inputSampleL *= 0.1;
                        break;
                    case 5:
                        slewL[slewCount % 12][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y] + slewL[5][y];
                        inputSampleL += slewL[6][y] + slewL[7][y] + slewL[8][y] + slewL[9][y] + slewL[10][y] + slewL[11][y];
                        inputSampleL *= 0.0833333333;
                        break;
                    case 6:
                        slewL[slewCount % 14][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y] + slewL[5][y] + slewL[6][y];
                        inputSampleL += slewL[7][y] + slewL[8][y] + slewL[9][y] + slewL[10][y] + slewL[11][y] + slewL[12][y] + slewL[13][y];
                        inputSampleL *= 0.0714285;
                        break;
                    case 7:
                        slewL[slewCount % 16][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y] + slewL[5][y] + slewL[6][y] + slewL[7][y];
                        inputSampleL += slewL[8][y] + slewL[9][y] + slewL[10][y] + slewL[11][y] + slewL[12][y] + slewL[13][y] + slewL[14][y] + slewL[15][y];
                        inputSampleL *= 0.0625;
                        break;
                    case 8:
                        slewL[slewCount % 18][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y] + slewL[5][y] + slewL[6][y] + slewL[7][y] + slewL[8][y];
                        inputSampleL += slewL[9][y] + slewL[10][y] + slewL[11][y] + slewL[12][y] + slewL[13][y] + slewL[14][y] + slewL[15][y] + slewL[16][y] + slewL[17][y];
                        inputSampleL *= 0.05555555;
                        break;
                    case 9:
                        slewL[slewCount % 20][y] = inputSampleL;
                        inputSampleL = slewL[0][y] + slewL[1][y] + slewL[2][y] + slewL[3][y] + slewL[4][y] + slewL[5][y] + slewL[6][y] + slewL[7][y] + slewL[8][y] + slewL[9][y];
                        inputSampleL += slewL[10][y] + slewL[11][y] + slewL[12][y] + slewL[13][y] + slewL[14][y] + slewL[15][y] + slewL[16][y] + slewL[17][y] + slewL[18][y] + slewL[19][y];
                        inputSampleL *= 0.05;
                        break;
                } // generate sets of rectangular window averages to layer
            } // all of which have a 'node' (silence) at Nyquist to combine
            double remainderSampleL = 0.0;
            switch (z) {
                case 0:
                    slewL[slewCount % 2][z] = inputSampleL;
                    remainderSampleL = (slewL[0][z] + slewL[1][z]) * 0.5;
                    break;
                case 1:
                    slewL[slewCount % 4][z] = inputSampleL;
                    remainderSampleL = (slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z]) * 0.25;
                    break;
                case 2:
                    slewL[slewCount % 6][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z];
                    remainderSampleL += slewL[3][z] + slewL[4][z] + slewL[5][z];
                    remainderSampleL *= 0.1666666666;
                    break;
                case 3:
                    slewL[slewCount % 8][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z];
                    remainderSampleL += slewL[4][z] + slewL[5][z] + slewL[6][z] + slewL[7][z];
                    remainderSampleL *= 0.125;
                    break;
                case 4:
                    slewL[slewCount % 10][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z];
                    remainderSampleL += slewL[5][z] + slewL[6][z] + slewL[7][z] + slewL[8][z] + slewL[9][z];
                    remainderSampleL *= 0.1;
                    break;
                case 5:
                    slewL[slewCount % 12][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z] + slewL[5][z];
                    remainderSampleL += slewL[6][z] + slewL[7][z] + slewL[8][z] + slewL[9][z] + slewL[10][z] + slewL[11][z];
                    remainderSampleL *= 0.0833333333;
                    break;
                case 6:
                    slewL[slewCount % 14][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z] + slewL[5][z] + slewL[6][z];
                    remainderSampleL += slewL[7][z] + slewL[8][z] + slewL[9][z] + slewL[10][z] + slewL[11][z] + slewL[12][z] + slewL[13][z];
                    remainderSampleL *= 0.0714285;
                    break;
                case 7:
                    slewL[slewCount % 16][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z] + slewL[5][z] + slewL[6][z] + slewL[7][z];
                    remainderSampleL += slewL[8][z] + slewL[9][z] + slewL[10][z] + slewL[11][z] + slewL[12][z] + slewL[13][z] + slewL[14][z] + slewL[15][z];
                    remainderSampleL *= 0.0625;
                    break;
                case 8:
                    slewL[slewCount % 18][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z] + slewL[5][z] + slewL[6][z] + slewL[7][z] + slewL[8][z];
                    remainderSampleL += slewL[9][z] + slewL[10][z] + slewL[11][z] + slewL[12][z] + slewL[13][z] + slewL[14][z] + slewL[15][z] + slewL[16][z] + slewL[17][z];
                    remainderSampleL *= 0.0555555;
                    break;
                case 9:
                    slewL[slewCount % 20][z] = inputSampleL;
                    remainderSampleL = slewL[0][z] + slewL[1][z] + slewL[2][z] + slewL[3][z] + slewL[4][z] + slewL[5][z] + slewL[6][z] + slewL[7][z] + slewL[8][z] + slewL[9][z];
                    remainderSampleL += slewL[10][z] + slewL[11][z] + slewL[12][z] + slewL[13][z] + slewL[14][z] + slewL[15][z] + slewL[16][z] + slewL[17][z] + slewL[18][z] + slewL[19][z];
                    remainderSampleL *= 0.05;
                    break;
            } // generate the final fractional amount
            for (int y = z + 1; y < 9; y++) {
                switch (y) {
                    case 0:
                        slewL[slewCount % 2][y] = inputSampleL;
                        break;
                    case 1:
                        slewL[slewCount % 4][y] = inputSampleL;
                        break;
                    case 2:
                        slewL[slewCount % 6][y] = inputSampleL;
                        break;
                    case 3:
                        slewL[slewCount % 8][y] = inputSampleL;
                        break;
                    case 4:
                        slewL[slewCount % 10][y] = inputSampleL;
                        break;
                    case 5:
                        slewL[slewCount % 12][y] = inputSampleL;
                        break;
                    case 6:
                        slewL[slewCount % 14][y] = inputSampleL;
                        break;
                    case 7:
                        slewL[slewCount % 16][y] = inputSampleL;
                        break;
                    case 8:
                        slewL[slewCount % 18][y] = inputSampleL;
                        break;
                    case 9:
                        slewL[slewCount % 20][y] = inputSampleL;
                        break;
                }
            } // clean up the little buffers when not being actively used
            // end L
            // begin R
            sustainSlewR += fabs((inputSampleR - secondSampleR) - (secondSampleR - thirdSampleR));
            sustainSlewR *= 0.9;
            inputSampleR = thirdSampleR; // two sample lookahead
            depthAmt = fmin(sin(fmin(sustainSlewR, M_PI_2)) * compresity, 9.0);
            z = floor(depthAmt);
            depthTrim = depthAmt - (double)z;
            for (int y = 0; y < z; y++) {
                switch (y) {
                    case 0:
                        slewR[slewCount % 2][y] = inputSampleR;
                        inputSampleR = (slewR[0][y] + slewR[1][y]) * 0.5;
                        break;
                    case 1:
                        slewR[slewCount % 4][y] = inputSampleR;
                        inputSampleR = (slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y]) * 0.25;
                        break;
                    case 2:
                        slewR[slewCount % 6][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y];
                        inputSampleR += slewR[3][y] + slewR[4][y] + slewR[5][y];
                        inputSampleR *= 0.1666666666;
                        break;
                    case 3:
                        slewR[slewCount % 8][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y];
                        inputSampleR += slewR[4][y] + slewR[5][y] + slewR[6][y] + slewR[7][y];
                        inputSampleR *= 0.125;
                        break;
                    case 4:
                        slewR[slewCount % 10][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y];
                        inputSampleR += slewR[5][y] + slewR[6][y] + slewR[7][y] + slewR[8][y] + slewR[9][y];
                        inputSampleR *= 0.1;
                        break;
                    case 5:
                        slewR[slewCount % 12][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y] + slewR[5][y];
                        inputSampleR += slewR[6][y] + slewR[7][y] + slewR[8][y] + slewR[9][y] + slewR[10][y] + slewR[11][y];
                        inputSampleR *= 0.0833333333;
                        break;
                    case 6:
                        slewR[slewCount % 14][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y] + slewR[5][y] + slewR[6][y];
                        inputSampleR += slewR[7][y] + slewR[8][y] + slewR[9][y] + slewR[10][y] + slewR[11][y] + slewR[12][y] + slewR[13][y];
                        inputSampleR *= 0.0714285;
                        break;
                    case 7:
                        slewR[slewCount % 16][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y] + slewR[5][y] + slewR[6][y] + slewR[7][y];
                        inputSampleR += slewR[8][y] + slewR[9][y] + slewR[10][y] + slewR[11][y] + slewR[12][y] + slewR[13][y] + slewR[14][y] + slewR[15][y];
                        inputSampleR *= 0.0625;
                        break;
                    case 8:
                        slewR[slewCount % 18][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y] + slewR[5][y] + slewR[6][y] + slewR[7][y] + slewR[8][y];
                        inputSampleR += slewR[9][y] + slewR[10][y] + slewR[11][y] + slewR[12][y] + slewR[13][y] + slewR[14][y] + slewR[15][y] + slewR[16][y] + slewR[17][y];
                        inputSampleR *= 0.05555555;
                        break;
                    case 9:
                        slewR[slewCount % 20][y] = inputSampleR;
                        inputSampleR = slewR[0][y] + slewR[1][y] + slewR[2][y] + slewR[3][y] + slewR[4][y] + slewR[5][y] + slewR[6][y] + slewR[7][y] + slewR[8][y] + slewR[9][y];
                        inputSampleR += slewR[10][y] + slewR[11][y] + slewR[12][y] + slewR[13][y] + slewR[14][y] + slewR[15][y] + slewR[16][y] + slewR[17][y] + slewR[18][y] + slewR[19][y];
                        inputSampleR *= 0.05;
                        break;
                } // generate sets of rectangular window averages to layer
            } // all of which have a 'node' (silence) at Nyquist to combine
            double remainderSampleR = 0.0;
            switch (z) {
                case 0:
                    slewR[slewCount % 2][z] = inputSampleR;
                    remainderSampleR = (slewR[0][z] + slewR[1][z]) * 0.5;
                    break;
                case 1:
                    slewR[slewCount % 4][z] = inputSampleR;
                    remainderSampleR = (slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z]) * 0.25;
                    break;
                case 2:
                    slewR[slewCount % 6][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z];
                    remainderSampleR += slewR[3][z] + slewR[4][z] + slewR[5][z];
                    remainderSampleR *= 0.1666666666;
                    break;
                case 3:
                    slewR[slewCount % 8][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z];
                    remainderSampleR += slewR[4][z] + slewR[5][z] + slewR[6][z] + slewR[7][z];
                    remainderSampleR *= 0.125;
                    break;
                case 4:
                    slewR[slewCount % 10][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z];
                    remainderSampleR += slewR[5][z] + slewR[6][z] + slewR[7][z] + slewR[8][z] + slewR[9][z];
                    remainderSampleR *= 0.1;
                    break;
                case 5:
                    slewR[slewCount % 12][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z] + slewR[5][z];
                    remainderSampleR += slewR[6][z] + slewR[7][z] + slewR[8][z] + slewR[9][z] + slewR[10][z] + slewR[11][z];
                    remainderSampleR *= 0.0833333333;
                    break;
                case 6:
                    slewR[slewCount % 14][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z] + slewR[5][z] + slewR[6][z];
                    remainderSampleR += slewR[7][z] + slewR[8][z] + slewR[9][z] + slewR[10][z] + slewR[11][z] + slewR[12][z] + slewR[13][z];
                    remainderSampleR *= 0.0714285;
                    break;
                case 7:
                    slewR[slewCount % 16][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z] + slewR[5][z] + slewR[6][z] + slewR[7][z];
                    remainderSampleR += slewR[8][z] + slewR[9][z] + slewR[10][z] + slewR[11][z] + slewR[12][z] + slewR[13][z] + slewR[14][z] + slewR[15][z];
                    remainderSampleR *= 0.0625;
                    break;
                case 8:
                    slewR[slewCount % 18][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z] + slewR[5][z] + slewR[6][z] + slewR[7][z] + slewR[8][z];
                    remainderSampleR += slewR[9][z] + slewR[10][z] + slewR[11][z] + slewR[12][z] + slewR[13][z] + slewR[14][z] + slewR[15][z] + slewR[16][z] + slewR[17][z];
                    remainderSampleR *= 0.0555555;
                    break;
                case 9:
                    slewR[slewCount % 20][z] = inputSampleR;
                    remainderSampleR = slewR[0][z] + slewR[1][z] + slewR[2][z] + slewR[3][z] + slewR[4][z] + slewR[5][z] + slewR[6][z] + slewR[7][z] + slewR[8][z] + slewR[9][z];
                    remainderSampleR += slewR[10][z] + slewR[11][z] + slewR[12][z] + slewR[13][z] + slewR[14][z] + slewR[15][z] + slewR[16][z] + slewR[17][z] + slewR[18][z] + slewR[19][z];
                    remainderSampleR *= 0.05;
                    break;
            } // generate the final fractional amount
            for (int y = z + 1; y < 9; y++) {
                switch (y) {
                    case 0:
                        slewR[slewCount % 2][y] = inputSampleR;
                        break;
                    case 1:
                        slewR[slewCount % 4][y] = inputSampleR;
                        break;
                    case 2:
                        slewR[slewCount % 6][y] = inputSampleR;
                        break;
                    case 3:
                        slewR[slewCount % 8][y] = inputSampleR;
                        break;
                    case 4:
                        slewR[slewCount % 10][y] = inputSampleR;
                        break;
                    case 5:
                        slewR[slewCount % 12][y] = inputSampleR;
                        break;
                    case 6:
                        slewR[slewCount % 14][y] = inputSampleR;
                        break;
                    case 7:
                        slewR[slewCount % 16][y] = inputSampleR;
                        break;
                    case 8:
                        slewR[slewCount % 18][y] = inputSampleR;
                        break;
                    case 9:
                        slewR[slewCount % 20][y] = inputSampleR;
                        break;
                }
            } // clean up the little buffers when not being actively used
            // end R
            slewCount++;
            if (slewCount > 619315200) {
                slewCount = 0;
            }
            thirdSampleL = secondSampleL;
            secondSampleL = drySampleL;
            thirdSampleR = secondSampleR;
            secondSampleR = drySampleR;
            inputSampleL = (inputSampleL * (1.0 - depthTrim)) + (remainderSampleL * depthTrim);
            inputSampleR = (inputSampleR * (1.0 - depthTrim)) + (remainderSampleR * depthTrim);
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
} // namespace airwindohhs::slew4
