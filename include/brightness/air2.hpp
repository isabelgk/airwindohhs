#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::air2 {

constexpr std::string_view k_name{ "Air2" };
constexpr std::string_view k_short_description{
    "Air2 is a different bright EQ with silk tone and high sample rate support."
};
constexpr std::string_view k_long_description{
    "Here’s what you get: Air2 has three treble-boost bands, Hiss, Glitter and Air. The first two are very like what’s in Energy2, but Air2 is NOT the same: it’s a different algorithm, not done the samw way Energy2’s ‘Rat’ band is done, and it has a broader, softer air band that’s less ‘raw harsh energy injection’ and more ‘bright and pretty’.And then, they all run through a ‘Silk’ control… inspired by but NOT the same as the Neve Portico Silk circuit. That is a real hardware transformer biased into second harmonic creation by a DC current. Air2’s Silk control is the same thing as Single Ended Triode (which you can download and use already) but only applied to the highs out of Air2, so it’s the same general concept but is not a clone of the Portico preamp. Plus if they freak out I will rename it to ‘Sow’s Ear’: it’s part of a treble brightener and the same basic functionality and as far as I’m concerned, nothing is stopping me from asymmetrically distorting highs, and Silk is the best general term for what that does. Except with mine you can push it too far for effect, because of course you can :)And finally, unlike Energy2, the Dry/Wet control for Air2 strikes a new balance between the Energy style of increasing the effect, and the traditional Dry/Wet control. New with Air2, you can now turn it to full-wet and get ONLY the output of the brightener bands. They’re not exactly filters but they act like it, and by adjusting them against each other you can produce insanely treble-boosted sounds and wipe out everything else using Dry/Wet. The way it works is, up to 75% (0.75) you still have full volume Dry. It won’t re-balance your track, just add whatever Air2 highs you’re looking for. Then, between 0.75 and 1 (full Wet) the dry goes away, so if you crank it up all the way you get the fullest extreme, but for most normal use it’s like Energy2, taking an unvarying dry signal (which is NOT undersampled) and adding however much of the effect you like."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Air2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastRefL[10];
    double lastRefR[10];
    int cycle;
    double airPrevAL;
    double airEvenAL;
    double airOddAL;
    double airFactorAL;
    double airPrevBL;
    double airEvenBL;
    double airOddBL;
    double airFactorBL;
    double airPrevCL;
    double airEvenCL;
    double airOddCL;
    double airFactorCL;
    double tripletPrevL;
    double tripletMidL;
    double tripletAL;
    double tripletBL;
    double tripletCL;
    double tripletFactorL;
    double airPrevAR;
    double airEvenAR;
    double airOddAR;
    double airFactorAR;
    double airPrevBR;
    double airEvenBR;
    double airOddBR;
    double airFactorBR;
    double airPrevCR;
    double airEvenCR;
    double airOddCR;
    double airFactorCR;
    double tripletPrevR;
    double tripletMidR;
    double tripletAR;
    double tripletBR;
    double tripletCR;
    double tripletFactorR;
    bool flipA;
    bool flipB;
    bool flop;
    int count;
    double postsine;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    Air2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        E = 1.0;
        for (int c = 0; c < 9; c++) {
            lastRefL[c] = 0.0;
            lastRefR[c] = 0.0;
        }
        cycle = 0;
        airPrevAL = 0.0;
        airEvenAL = 0.0;
        airOddAL = 0.0;
        airFactorAL = 0.0;
        airPrevBL = 0.0;
        airEvenBL = 0.0;
        airOddBL = 0.0;
        airFactorBL = 0.0;
        airPrevCL = 0.0;
        airEvenCL = 0.0;
        airOddCL = 0.0;
        airFactorCL = 0.0;
        tripletPrevL = 0.0;
        tripletMidL = 0.0;
        tripletAL = 0.0;
        tripletBL = 0.0;
        tripletCL = 0.0;
        tripletFactorL = 0.0;
        airPrevAR = 0.0;
        airEvenAR = 0.0;
        airOddAR = 0.0;
        airFactorAR = 0.0;
        airPrevBR = 0.0;
        airEvenBR = 0.0;
        airOddBR = 0.0;
        airFactorBR = 0.0;
        airPrevCR = 0.0;
        airEvenCR = 0.0;
        airOddCR = 0.0;
        airFactorCR = 0.0;
        tripletPrevR = 0.0;
        tripletMidR = 0.0;
        tripletAR = 0.0;
        tripletBR = 0.0;
        tripletCR = 0.0;
        tripletFactorR = 0.0;
        flipA = false;
        flipB = false;
        flop = false;
        count = 1;
        postsine = sin(1.0);
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
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "hiss"; break;
            case kParamB: return "glitter"; break;
            case kParamC: return "air"; break;
            case kParamD: return "silk"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Hiss"; break;
            case kParamB: return "Glitter"; break;
            case kParamC: return "Air"; break;
            case kParamD: return "Silk"; break;
            case kParamE: return "Dry/Wet"; break;

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
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;

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
        double hiIntensity = (A * 2.0) - 1.0;
        double tripletIntensity = (B * 2.0) - 1.0;
        double airIntensity = (C * 2.0) - 1.0;
        if (hiIntensity < 0.0) {
            hiIntensity *= 0.57525;
        }
        if (tripletIntensity < 0.0) {
            tripletIntensity *= 0.71325;
        }
        if (airIntensity < 0.0) {
            airIntensity *= 0.5712;
        }
        hiIntensity = -pow(hiIntensity, 3);
        tripletIntensity = -pow(tripletIntensity, 3);
        airIntensity = -pow(airIntensity, 3);
        double hiQ = 1.5 + fabs(hiIntensity * 0.5);
        double tripletQ = 1.5 + fabs(tripletIntensity * 0.5);
        double airQ = 1.5 + fabs(airIntensity * 0.5);
        double intensity = (pow(D, 3) * 4.0) + 0.0001;
        double mix = E;
        double drymix = (1.0 - mix) * 4.0;
        if (drymix > 1.0) {
            drymix = 1.0;
        }
        // all types of air band are running in parallel, not series
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
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do an Air sample
                double correctionL = 0.0;
                double correctionR = 0.0;
                if (fabs(hiIntensity) > 0.0001) {
                    airFactorCL = airPrevCL - inputSampleL;
                    airFactorCR = airPrevCR - inputSampleR;
                    if (flop)
                    {
                        airEvenCL += airFactorCL;
                        airOddCL -= airFactorCL;
                        airFactorCL = airEvenCL * hiIntensity;
                        airEvenCR += airFactorCR;
                        airOddCR -= airFactorCR;
                        airFactorCR = airEvenCR * hiIntensity;
                    }
                    else
                    {
                        airOddCL += airFactorCL;
                        airEvenCL -= airFactorCL;
                        airFactorCL = airOddCL * hiIntensity;
                        airOddCR += airFactorCR;
                        airEvenCR -= airFactorCR;
                        airFactorCR = airOddCR * hiIntensity;
                    }
                    airOddCL = (airOddCL - ((airOddCL - airEvenCL) / 256.0)) / hiQ;
                    airEvenCL = (airEvenCL - ((airEvenCL - airOddCL) / 256.0)) / hiQ;
                    airPrevCL = inputSampleL;
                    correctionL = correctionL + airFactorCL;
                    airOddCR = (airOddCR - ((airOddCR - airEvenCR) / 256.0)) / hiQ;
                    airEvenCR = (airEvenCR - ((airEvenCR - airOddCR) / 256.0)) / hiQ;
                    airPrevCR = inputSampleR;
                    correctionR = correctionR + airFactorCR;
                    flop = !flop;
                } // 22k
                if (fabs(tripletIntensity) > 0.0001) {
                    tripletFactorL = tripletPrevL - inputSampleL;
                    tripletFactorR = tripletPrevR - inputSampleR;
                    if (count < 1 || count > 3) {
                        count = 1;
                    }
                    switch (count)
                    {
                        case 1:
                            tripletAL += tripletFactorL;
                            tripletCL -= tripletFactorL;
                            tripletFactorL = tripletAL * tripletIntensity;
                            tripletPrevL = tripletMidL;
                            tripletMidL = inputSampleL;
                            tripletAR += tripletFactorR;
                            tripletCR -= tripletFactorR;
                            tripletFactorR = tripletAR * tripletIntensity;
                            tripletPrevR = tripletMidR;
                            tripletMidR = inputSampleR;
                            break;
                        case 2:
                            tripletBL += tripletFactorL;
                            tripletAL -= tripletFactorL;
                            tripletFactorL = tripletBL * tripletIntensity;
                            tripletPrevL = tripletMidL;
                            tripletMidL = inputSampleL;
                            tripletBR += tripletFactorR;
                            tripletAR -= tripletFactorR;
                            tripletFactorR = tripletBR * tripletIntensity;
                            tripletPrevR = tripletMidR;
                            tripletMidR = inputSampleR;
                            break;
                        case 3:
                            tripletCL += tripletFactorL;
                            tripletBL -= tripletFactorL;
                            tripletFactorL = tripletCL * tripletIntensity;
                            tripletPrevL = tripletMidL;
                            tripletMidL = inputSampleL;
                            tripletCR += tripletFactorR;
                            tripletBR -= tripletFactorR;
                            tripletFactorR = tripletCR * tripletIntensity;
                            tripletPrevR = tripletMidR;
                            tripletMidR = inputSampleR;
                            break;
                    }
                    tripletAL /= tripletQ;
                    tripletBL /= tripletQ;
                    tripletCL /= tripletQ;
                    tripletAR /= tripletQ;
                    tripletBR /= tripletQ;
                    tripletCR /= tripletQ;
                    correctionL = correctionL + tripletFactorL;
                    correctionR = correctionR + tripletFactorR;
                    count++;
                } // 15K
                if (fabs(airIntensity) > 0.0001) {
                    if (flop)
                    {
                        airFactorAL = airPrevAL - inputSampleL;
                        airFactorAR = airPrevAR - inputSampleR;
                        if (flipA)
                        {
                            airEvenAL += airFactorAL;
                            airOddAL -= airFactorAL;
                            airFactorAL = airEvenAL * airIntensity;
                            airEvenAR += airFactorAR;
                            airOddAR -= airFactorAR;
                            airFactorAR = airEvenAR * airIntensity;
                        }
                        else
                        {
                            airOddAL += airFactorAL;
                            airEvenAL -= airFactorAL;
                            airFactorAL = airOddAL * airIntensity;
                            airOddAR += airFactorAR;
                            airEvenAR -= airFactorAR;
                            airFactorAR = airOddAR * airIntensity;
                        }
                        airOddAL = (airOddAL - ((airOddAL - airEvenAL) / 256.0)) / airQ;
                        airEvenAL = (airEvenAL - ((airEvenAL - airOddAL) / 256.0)) / airQ;
                        airPrevAL = inputSampleL;
                        correctionL = correctionL + airFactorAL;
                        airOddAR = (airOddAR - ((airOddAR - airEvenAR) / 256.0)) / airQ;
                        airEvenAR = (airEvenAR - ((airEvenAR - airOddAR) / 256.0)) / airQ;
                        airPrevAR = inputSampleR;
                        correctionR = correctionR + airFactorAR;
                        flipA = !flipA;
                    }
                    else
                    {
                        airFactorBL = airPrevBL - inputSampleL;
                        airFactorBR = airPrevBR - inputSampleR;
                        if (flipB)
                        {
                            airEvenBL += airFactorBL;
                            airOddBL -= airFactorBL;
                            airFactorBL = airEvenBL * airIntensity;
                            airEvenBR += airFactorBR;
                            airOddBR -= airFactorBR;
                            airFactorBR = airEvenBR * airIntensity;
                        }
                        else
                        {
                            airOddBL += airFactorBL;
                            airEvenBL -= airFactorBL;
                            airFactorBL = airOddBL * airIntensity;
                            airOddBR += airFactorBR;
                            airEvenBR -= airFactorBR;
                            airFactorBR = airOddBR * airIntensity;
                        }
                        airOddBL = (airOddBL - ((airOddBL - airEvenBL) / 256.0)) / airQ;
                        airEvenBL = (airEvenBL - ((airEvenBL - airOddBL) / 256.0)) / airQ;
                        airPrevBL = inputSampleL;
                        correctionL = correctionL + airFactorBL;
                        airOddBR = (airOddBR - ((airOddBR - airEvenBR) / 256.0)) / airQ;
                        airEvenBR = (airEvenBR - ((airEvenBR - airOddBR) / 256.0)) / airQ;
                        airPrevBR = inputSampleR;
                        correctionR = correctionR + airFactorBR;
                        flipB = !flipB;
                    }
                } // 11K one
                correctionL *= intensity;
                correctionL -= 1.0;
                double bridgerectifier = fabs(correctionL);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (correctionL > 0) {
                    correctionL = bridgerectifier;
                }
                else {
                    correctionL = -bridgerectifier;
                }
                correctionL += postsine;
                correctionL /= intensity;
                correctionR *= intensity;
                correctionR -= 1.0;
                bridgerectifier = fabs(correctionR);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (correctionR > 0) {
                    correctionR = bridgerectifier;
                }
                else {
                    correctionR = -bridgerectifier;
                }
                correctionR += postsine;
                correctionR /= intensity;
                inputSampleL = correctionL * 4.0 * mix;
                inputSampleR = correctionR * 4.0 * mix;
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
            switch (cycleEnd) // multi-pole average using lastRef[] variables
            {
                case 4:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[7]) * 0.5;
                    lastRefL[7] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[7]) * 0.5;
                    lastRefR[7] = lastRefR[8]; // continue, do not break
                case 3:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[6]) * 0.5;
                    lastRefL[6] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[6]) * 0.5;
                    lastRefR[6] = lastRefR[8]; // continue, do not break
                case 2:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[5]) * 0.5;
                    lastRefL[5] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[5]) * 0.5;
                    lastRefR[5] = lastRefR[8]; // continue, do not break
                case 1:
                    break; // no further averaging
            }
            if (drymix < 1.0) {
                drySampleL *= drymix;
                drySampleR *= drymix;
            }
            inputSampleL += drySampleL;
            inputSampleR += drySampleR;
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
} // namespace airwindohhs::air2
