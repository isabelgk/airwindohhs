#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::logical4 {

constexpr std::string_view k_name{ "Logical4" };
constexpr std::string_view k_short_description{
    "Logical4 is a classic 2-buss compressor."
};
constexpr std::string_view k_long_description{
    "Logical’s a compressor. It was designed from the start to work on the 2-buss in the most demanding conditions: people are really picky about their 2-buss compressors, and you can’t mess around. The tone has to be spot-on and it’s got to be transparent and able to let the music through. Additionally, when we’re talking about ‘tone’ and something called ‘Logical’ you can see that it’s going to be in the SSL style: there’s a sparkliness which requires some extra coding attention.You can approach compression duties from several directions with Logical. It has three distinct stages, and will entirely bypass stages it’s not using. It’ll go from 1/1 compression, up to 2/1 using just a single stage (for the utmost transparency): keep it below 2/1 ratio and use the threshold control to bring in the compression. This is a traditional 2-buss natural-sounding compression. From 2/1 to 4/1 ratio, you can get various behaviors and the two stages in use still sound very clean: the speed control will give you different kinds of ‘swing’ and spring-back out of the compression.Then as you pass 4/1 ratio and go off to a max of 16/1 (approximate, but that’s the basic idea) there’s a tone change, and as you get into crazy high ratios, Logical goes a little bonkers. This was NOT available in previous Logicals. The issue was, if you rely heavily on that final compression stage, things can get messy. You can push Logical until it’s nasty and so full of energy it’s forcing you to use the makeup gain to PAD the output, just to handle all the madness.This time, and in honor of Logical going free VST format, it’s not set up for only good behavior. This time, it’s your responsibility to not blow up your outputs by thoughtlessly cranking the ratio. Consider it an audio chainsaw made of silk and glorious victories. Not every top-selling plugin got this much better when I revisited it. I’m very pleased with how Logical4 came out, and I hope to see it talked about a whole bunch. This one’s worth a lot of ‘did you hear?’."
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Logical4 final : public Effect<T>
{
    // begin ButterComp
    double controlAposL;
    double controlAnegL;
    double controlBposL;
    double controlBnegL;
    double targetposL;
    double targetnegL;
    double controlAposBL;
    double controlAnegBL;
    double controlBposBL;
    double controlBnegBL;
    double targetposBL;
    double targetnegBL;
    double controlAposCL;
    double controlAnegCL;
    double controlBposCL;
    double controlBnegCL;
    double targetposCL;
    double targetnegCL;
    double avgAL;
    double avgBL;
    double avgCL;
    double avgDL;
    double avgEL;
    double avgFL;
    double nvgAL;
    double nvgBL;
    double nvgCL;
    double nvgDL;
    double nvgEL;
    double nvgFL;
    // end ButterComp
    // begin Power Sag
    double dL[1000];
    double controlL;
    double bL[1000];
    double controlBL;
    double cL[1000];
    double controlCL;
    // end Power Sag
    // begin ButterComp
    double controlAposR;
    double controlAnegR;
    double controlBposR;
    double controlBnegR;
    double targetposR;
    double targetnegR;
    double controlAposBR;
    double controlAnegBR;
    double controlBposBR;
    double controlBnegBR;
    double targetposBR;
    double targetnegBR;
    double controlAposCR;
    double controlAnegCR;
    double controlBposCR;
    double controlBnegCR;
    double targetposCR;
    double targetnegCR;
    double avgAR;
    double avgBR;
    double avgCR;
    double avgDR;
    double avgER;
    double avgFR;
    double nvgAR;
    double nvgBR;
    double nvgCR;
    double nvgDR;
    double nvgER;
    double nvgFR;
    // end ButterComp
    // begin Power Sag
    double dR[1000];
    double controlR;
    double bR[1000];
    double controlBR;
    double cR[1000];
    double controlCR;
    // end Power Sag
    int gcount;
    bool fpFlip;
    // default stuff
    uint32_t fpdL;
    uint32_t fpdR;
    float A;
    float B;
    float C;
    float D;
    float E;

  public:
    Logical4()
    {
        A = 0.5;
        B = 0.2;
        C = 0.19202020202020202;
        D = 0.5;
        E = 1.0;
        // begin ButterComps
        controlAposL = 1.0;
        controlAnegL = 1.0;
        controlBposL = 1.0;
        controlBnegL = 1.0;
        targetposL = 1.0;
        targetnegL = 1.0;
        controlAposBL = 1.0;
        controlAnegBL = 1.0;
        controlBposBL = 1.0;
        controlBnegBL = 1.0;
        targetposBL = 1.0;
        targetnegBL = 1.0;
        controlAposCL = 1.0;
        controlAnegCL = 1.0;
        controlBposCL = 1.0;
        controlBnegCL = 1.0;
        targetposCL = 1.0;
        targetnegCL = 1.0;
        avgAL = avgBL = avgCL = avgDL = avgEL = avgFL = 0.0;
        nvgAL = nvgBL = nvgCL = nvgDL = nvgEL = nvgFL = 0.0;
        // end ButterComps
        // begin ButterComps
        controlAposR = 1.0;
        controlAnegR = 1.0;
        controlBposR = 1.0;
        controlBnegR = 1.0;
        targetposR = 1.0;
        targetnegR = 1.0;
        controlAposBR = 1.0;
        controlAnegBR = 1.0;
        controlBposBR = 1.0;
        controlBnegBR = 1.0;
        targetposBR = 1.0;
        targetnegBR = 1.0;
        controlAposCR = 1.0;
        controlAnegCR = 1.0;
        controlBposCR = 1.0;
        controlBnegCR = 1.0;
        targetposCR = 1.0;
        targetnegCR = 1.0;
        avgAR = avgBR = avgCR = avgDR = avgER = avgFR = 0.0;
        nvgAR = nvgBR = nvgCR = nvgDR = nvgER = nvgFR = 0.0;
        // end ButterComps
        // begin Power Sags
        for (int count = 0; count < 999; count++) {
            dL[count] = 0;
            bL[count] = 0;
            cL[count] = 0;
            dR[count] = 0;
            bR[count] = 0;
            cR[count] = 0;
        }
        controlL = 0;
        controlBL = 0;
        controlCL = 0;
        controlR = 0;
        controlBR = 0;
        controlCR = 0;
        gcount = 0;
        // end Power Sags
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
            case kParamB: return 0.2; break;
            case kParamC: return 0.192020202020202; break;
            case kParamD: return 0.5; break;
            case kParamE: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "threshold"; break;
            case kParamB: return "ratio"; break;
            case kParamC: return "speed"; break;
            case kParamD: return "makeupgn"; break;
            case kParamE: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Threshold"; break;
            case kParamB: return "Ratio"; break;
            case kParamC: return "Speed"; break;
            case kParamD: return "MakeupGn"; break;
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
            case kParamA: return "dB"; break;
            case kParamB: return "/1"; break;
            case kParamC: return "ms"; break;
            case kParamD: return "dB"; break;
            case kParamE: return " "; break;

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
        float drySampleL;
        float drySampleR;
        double inputSampleL;
        double inputSampleR;
        // begin ButterComp
        double inputpos;
        double inputneg;
        double calcpos;
        double calcneg;
        double outputpos;
        double outputneg;
        double totalmultiplier;
        double inputgain = pow(10.0, (-((A * 40.0) - 20.0)) / 20.0);
        // fussing with the controls to make it hit the right threshold values
        double compoutgain = inputgain; // let's try compensating for this
        double attackspeed = ((C * C) * 99.0) + 1.0;
        // is in ms
        attackspeed = 10.0 / sqrt(attackspeed);
        // convert to a remainder for use in comp
        double divisor = 0.000782 * attackspeed;
        // First Speed control
        divisor /= overallscale;
        double remainder = divisor;
        divisor = 1.0 - divisor;
        double divisorB = 0.000819 * attackspeed;
        // Second Speed control
        divisorB /= overallscale;
        double remainderB = divisorB;
        divisorB = 1.0 - divisorB;
        double divisorC = 0.000857;
        // Third Speed control
        divisorC /= overallscale;
        double remainderC = divisorC * attackspeed;
        divisorC = 1.0 - divisorC;
        // end ButterComp
        double dynamicDivisor;
        double dynamicRemainder;
        // used for variable release
        // begin Desk Power Sag
        double intensity = 0.0445556;
        double depthA = 2.42;
        int offsetA = (int)(depthA * overallscale);
        if (offsetA < 1) {
            offsetA = 1;
        }
        if (offsetA > 498) {
            offsetA = 498;
        }
        double depthB = 2.42; // was 3.38
        int offsetB = (int)(depthB * overallscale);
        if (offsetB < 1) {
            offsetB = 1;
        }
        if (offsetB > 498) {
            offsetB = 498;
        }
        double depthC = 2.42; // was 4.35
        int offsetC = (int)(depthC * overallscale);
        if (offsetC < 1) {
            offsetC = 1;
        }
        if (offsetC > 498) {
            offsetC = 498;
        }
        double clamp;
        double thickness;
        double out;
        double bridgerectifier;
        double powerSag = 0.003300223685324102874217; // was .00365
        // the Power Sag constant is how much the sag is cut back in high compressions. Increasing it
        // increases the guts and gnarl of the music, decreasing it or making it negative causes the texture to go
        //'ethereal' and unsolid under compression. Very subtle!
        // end Desk Power Sag
        double ratio = sqrt(((B * B) * 15.0) + 1.0) - 1.0;
        if (ratio > 2.99999) {
            ratio = 2.99999;
        }
        if (ratio < 0.0) {
            ratio = 0.0;
        }
        // anything we do must adapt to our dry/a/b/c output stages
        int ratioselector = floor(ratio);
        // zero to three, it'll become, always with 3 as the max
        ratio -= ratioselector;
        double invRatio = 1.0 - ratio;
        // for all processing we've settled on the 'stage' and are just interpolating between top and bottom
        // ratio is the more extreme case, invratio becomes our 'floor' case including drySample
        double outSampleAL = 0.0;
        double outSampleBL = 0.0;
        double outSampleCL = 0.0;
        double outSampleAR = 0.0;
        double outSampleBR = 0.0;
        double outSampleCR = 0.0;
        // what we interpolate between using ratio
        double outputgain = pow(10.0, ((D * 40.0) - 20.0) / 20.0);
        double wet = E;
        // removed extra dry variable
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            gcount--;
            if (gcount < 0 || gcount > 499) {
                gcount = 499;
            }
            // begin first Power SagL
            dL[gcount + 499] = dL[gcount] = fabs(inputSampleL) * (intensity - ((controlAposL + controlBposL + controlAnegL + controlBnegL) * powerSag));
            controlL += (dL[gcount] / offsetA);
            controlL -= (dL[gcount + offsetA] / offsetA);
            controlL -= 0.000001;
            clamp = 1;
            if (controlL < 0) {
                controlL = 0;
            }
            if (controlL > 1) {
                clamp -= (controlL - 1);
                controlL = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            // control = 0 to 1
            thickness = ((1.0 - controlL) * 2.0) - 1.0;
            out = fabs(thickness);
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            if (clamp != 1.0) {
                inputSampleL *= clamp;
            }
            // end first Power SagL
            // begin first Power SagR
            dR[gcount + 499] = dR[gcount] = fabs(inputSampleR) * (intensity - ((controlAposR + controlBposR + controlAnegR + controlBnegR) * powerSag));
            controlR += (dR[gcount] / offsetA);
            controlR -= (dR[gcount + offsetA] / offsetA);
            controlR -= 0.000001;
            clamp = 1;
            if (controlR < 0) {
                controlR = 0;
            }
            if (controlR > 1) {
                clamp -= (controlR - 1);
                controlR = 1;
            }
            if (clamp < 0.5) {
                clamp = 0.5;
            }
            // control = 0 to 1
            thickness = ((1.0 - controlR) * 2.0) - 1.0;
            out = fabs(thickness);
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thickness > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            if (clamp != 1.0) {
                inputSampleR *= clamp;
            }
            // end first Power SagR
            // begin first compressorL
            if (inputgain != 1.0) {
                inputSampleL *= inputgain;
            }
            inputpos = (inputSampleL * fpOld) + (avgAL * fpNew) + 1.0;
            avgAL = inputSampleL;
            // hovers around 1 when there's no signal
            if (inputpos < 0.001) {
                inputpos = 0.001;
            }
            outputpos = inputpos / 2.0;
            if (outputpos > 1.0) {
                outputpos = 1.0;
            }
            inputpos *= inputpos;
            // will be very high for hot input, can be 0.00001-1 for other-polarity
            dynamicRemainder = remainder * (inputpos + 1.0);
            if (dynamicRemainder > 1.0) {
                dynamicRemainder = 1.0;
            }
            dynamicDivisor = 1.0 - dynamicRemainder;
            // calc chases much faster if input swing is high
            targetposL *= dynamicDivisor;
            targetposL += (inputpos * dynamicRemainder);
            calcpos = pow((1.0 / targetposL), 2);
            // extra tiny, quick, if the inputpos of this polarity is high
            inputneg = (-inputSampleL * fpOld) + (nvgAL * fpNew) + 1.0;
            nvgAL = -inputSampleL;
            if (inputneg < 0.001) {
                inputneg = 0.001;
            }
            outputneg = inputneg / 2.0;
            if (outputneg > 1.0) {
                outputneg = 1.0;
            }
            inputneg *= inputneg;
            // will be very high for hot input, can be 0.00001-1 for other-polarity
            dynamicRemainder = remainder * (inputneg + 1.0);
            if (dynamicRemainder > 1.0) {
                dynamicRemainder = 1.0;
            }
            dynamicDivisor = 1.0 - dynamicRemainder;
            // calc chases much faster if input swing is high
            targetnegL *= dynamicDivisor;
            targetnegL += (inputneg * dynamicRemainder);
            calcneg = pow((1.0 / targetnegL), 2);
            // now we have mirrored targets for comp
            // each calc is a blowed up chased target from tiny (at high levels of that polarity) to 1 at no input
            // calc is the one we want to react differently: go tiny quick,
            // outputpos and outputneg go from 0 to 1
            if (inputSampleL > 0)
            { // working on pos
                if (true == fpFlip)
                {
                    controlAposL *= divisor;
                    controlAposL += (calcpos * remainder);
                    if (controlAposR > controlAposL) {
                        controlAposR = (controlAposR + controlAposL) * 0.5;
                    }
                    // this part makes the compressor linked: both channels will converge towards whichever
                    // is the most compressed at the time.
                }
                else
                {
                    controlBposL *= divisor;
                    controlBposL += (calcpos * remainder);
                    if (controlBposR > controlBposL) {
                        controlBposR = (controlBposR + controlBposL) * 0.5;
                    }
                }
            }
            else
            { // working on neg
                if (true == fpFlip)
                {
                    controlAnegL *= divisor;
                    controlAnegL += (calcneg * remainder);
                    if (controlAnegR > controlAnegL) {
                        controlAnegR = (controlAnegR + controlAnegL) * 0.5;
                    }
                }
                else
                {
                    controlBnegL *= divisor;
                    controlBnegL += (calcneg * remainder);
                    if (controlBnegR > controlBnegL) {
                        controlBnegR = (controlBnegR + controlBnegL) * 0.5;
                    }
                }
            }
            // this causes each of the four to update only when active and in the correct 'fpFlip'
            if (true == fpFlip)
            {
                totalmultiplier = (controlAposL * outputpos) + (controlAnegL * outputneg);
            }
            else
            {
                totalmultiplier = (controlBposL * outputpos) + (controlBnegL * outputneg);
            }
            // this combines the sides according to fpFlip, blending relative to the input value
            if (totalmultiplier != 1.0) {
                inputSampleL *= totalmultiplier;
            }
            // if (compoutgain != 1.0) inputSample /= compoutgain;
            if (inputSampleL > 36.0) {
                inputSampleL = 36.0;
            }
            if (inputSampleL < -36.0) {
                inputSampleL = -36.0;
            }
            // build in +18db hard clip on insano inputs
            outSampleAL = inputSampleL / compoutgain;
            // end first compressorL
            // begin first compressorR
            if (inputgain != 1.0) {
                inputSampleR *= inputgain;
            }
            inputpos = (inputSampleR * fpOld) + (avgAR * fpNew) + 1.0;
            avgAR = inputSampleR;
            // hovers around 1 when there's no signal
            if (inputpos < 0.001) {
                inputpos = 0.001;
            }
            outputpos = inputpos / 2.0;
            if (outputpos > 1.0) {
                outputpos = 1.0;
            }
            inputpos *= inputpos;
            // will be very high for hot input, can be 0.00001-1 for other-polarity
            dynamicRemainder = remainder * (inputpos + 1.0);
            if (dynamicRemainder > 1.0) {
                dynamicRemainder = 1.0;
            }
            dynamicDivisor = 1.0 - dynamicRemainder;
            // calc chases much faster if input swing is high
            targetposR *= dynamicDivisor;
            targetposR += (inputpos * dynamicRemainder);
            calcpos = pow((1.0 / targetposR), 2);
            // extra tiny, quick, if the inputpos of this polarity is high
            inputneg = (-inputSampleR * fpOld) + (nvgAR * fpNew) + 1.0;
            nvgAR = -inputSampleR;
            if (inputneg < 0.001) {
                inputneg = 0.001;
            }
            outputneg = inputneg / 2.0;
            if (outputneg > 1.0) {
                outputneg = 1.0;
            }
            inputneg *= inputneg;
            // will be very high for hot input, can be 0.00001-1 for other-polarity
            dynamicRemainder = remainder * (inputneg + 1.0);
            if (dynamicRemainder > 1.0) {
                dynamicRemainder = 1.0;
            }
            dynamicDivisor = 1.0 - dynamicRemainder;
            // calc chases much faster if input swing is high
            targetnegR *= dynamicDivisor;
            targetnegR += (inputneg * dynamicRemainder);
            calcneg = pow((1.0 / targetnegR), 2);
            // now we have mirrored targets for comp
            // each calc is a blowed up chased target from tiny (at high levels of that polarity) to 1 at no input
            // calc is the one we want to react differently: go tiny quick,
            // outputpos and outputneg go from 0 to 1
            if (inputSampleR > 0)
            { // working on pos
                if (true == fpFlip)
                {
                    controlAposR *= divisor;
                    controlAposR += (calcpos * remainder);
                    if (controlAposL > controlAposR) {
                        controlAposL = (controlAposR + controlAposL) * 0.5;
                    }
                    // this part makes the compressor linked: both channels will converge towards whichever
                    // is the most compressed at the time.
                }
                else
                {
                    controlBposR *= divisor;
                    controlBposR += (calcpos * remainder);
                    if (controlBposL > controlBposR) {
                        controlBposL = (controlBposR + controlBposL) * 0.5;
                    }
                }
            }
            else
            { // working on neg
                if (true == fpFlip)
                {
                    controlAnegR *= divisor;
                    controlAnegR += (calcneg * remainder);
                    if (controlAnegL > controlAnegR) {
                        controlAnegL = (controlAnegR + controlAnegL) * 0.5;
                    }
                }
                else
                {
                    controlBnegR *= divisor;
                    controlBnegR += (calcneg * remainder);
                    if (controlBnegL > controlBnegR) {
                        controlBnegL = (controlBnegR + controlBnegL) * 0.5;
                    }
                }
            }
            // this causes each of the four to update only when active and in the correct 'fpFlip'
            if (true == fpFlip)
            {
                totalmultiplier = (controlAposR * outputpos) + (controlAnegR * outputneg);
            }
            else
            {
                totalmultiplier = (controlBposR * outputpos) + (controlBnegR * outputneg);
            }
            // this combines the sides according to fpFlip, blending relative to the input value
            if (totalmultiplier != 1.0) {
                inputSampleR *= totalmultiplier;
            }
            // if (compoutgain != 1.0) inputSample /= compoutgain;
            if (inputSampleR > 36.0) {
                inputSampleR = 36.0;
            }
            if (inputSampleR < -36.0) {
                inputSampleR = -36.0;
            }
            // build in +18db hard clip on insano inputs
            outSampleAR = inputSampleR / compoutgain;
            // end first compressorR
            if (ratioselector > 0) {
                // begin second Power SagL
                bL[gcount + 499] = bL[gcount] = fabs(inputSampleL) * (intensity - ((controlAposBL + controlBposBL + controlAnegBL + controlBnegBL) * powerSag));
                controlBL += (bL[gcount] / offsetB);
                controlBL -= (bL[gcount + offsetB] / offsetB);
                controlBL -= 0.000001;
                clamp = 1;
                if (controlBL < 0) {
                    controlBL = 0;
                }
                if (controlBL > 1) {
                    clamp -= (controlBL - 1);
                    controlBL = 1;
                }
                if (clamp < 0.5) {
                    clamp = 0.5;
                }
                // control = 0 to 1
                thickness = ((1.0 - controlBL) * 2.0) - 1.0;
                out = fabs(thickness);
                bridgerectifier = fabs(inputSampleL);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (thickness > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (inputSampleL > 0) {
                    inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
                }
                else {
                    inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
                }
                // blend according to density control
                if (clamp != 1.0) {
                    inputSampleL *= clamp;
                }
                // end second Power SagL
                // begin second Power SagR
                bR[gcount + 499] = bR[gcount] = fabs(inputSampleR) * (intensity - ((controlAposBR + controlBposBR + controlAnegBR + controlBnegBR) * powerSag));
                controlBR += (bR[gcount] / offsetB);
                controlBR -= (bR[gcount + offsetB] / offsetB);
                controlBR -= 0.000001;
                clamp = 1;
                if (controlBR < 0) {
                    controlBR = 0;
                }
                if (controlBR > 1) {
                    clamp -= (controlBR - 1);
                    controlBR = 1;
                }
                if (clamp < 0.5) {
                    clamp = 0.5;
                }
                // control = 0 to 1
                thickness = ((1.0 - controlBR) * 2.0) - 1.0;
                out = fabs(thickness);
                bridgerectifier = fabs(inputSampleR);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                if (thickness > 0) {
                    bridgerectifier = sin(bridgerectifier);
                }
                else {
                    bridgerectifier = 1 - cos(bridgerectifier);
                }
                // produce either boosted or starved version
                if (inputSampleR > 0) {
                    inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
                }
                else {
                    inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
                }
                // blend according to density control
                if (clamp != 1.0) {
                    inputSampleR *= clamp;
                }
                // end second Power SagR
                // begin second compressorL
                inputpos = (inputSampleL * fpOld) + (avgBL * fpNew) + 1.0;
                avgBL = inputSampleL;
                if (inputpos < 0.001) {
                    inputpos = 0.001;
                }
                outputpos = inputpos / 2.0;
                if (outputpos > 1.0) {
                    outputpos = 1.0;
                }
                inputpos *= inputpos;
                // will be very high for hot input, can be 0.00001-1 for other-polarity
                dynamicRemainder = remainderB * (inputpos + 1.0);
                if (dynamicRemainder > 1.0) {
                    dynamicRemainder = 1.0;
                }
                dynamicDivisor = 1.0 - dynamicRemainder;
                // calc chases much faster if input swing is high
                targetposBL *= dynamicDivisor;
                targetposBL += (inputpos * dynamicRemainder);
                calcpos = pow((1.0 / targetposBL), 2);
                inputneg = (-inputSampleL * fpOld) + (nvgBL * fpNew) + 1.0;
                nvgBL = -inputSampleL;
                if (inputneg < 0.001) {
                    inputneg = 0.001;
                }
                outputneg = inputneg / 2.0;
                if (outputneg > 1.0) {
                    outputneg = 1.0;
                }
                inputneg *= inputneg;
                // will be very high for hot input, can be 0.00001-1 for other-polarity
                dynamicRemainder = remainderB * (inputneg + 1.0);
                if (dynamicRemainder > 1.0) {
                    dynamicRemainder = 1.0;
                }
                dynamicDivisor = 1.0 - dynamicRemainder;
                // calc chases much faster if input swing is high
                targetnegBL *= dynamicDivisor;
                targetnegBL += (inputneg * dynamicRemainder);
                calcneg = pow((1.0 / targetnegBL), 2);
                // now we have mirrored targets for comp
                // outputpos and outputneg go from 0 to 1
                if (inputSampleL > 0)
                { // working on pos
                    if (true == fpFlip)
                    {
                        controlAposBL *= divisorB;
                        controlAposBL += (calcpos * remainderB);
                        if (controlAposBR > controlAposBL) {
                            controlAposBR = (controlAposBR + controlAposBL) * 0.5;
                        }
                        // this part makes the compressor linked: both channels will converge towards whichever
                        // is the most compressed at the time.
                    }
                    else
                    {
                        controlBposBL *= divisorB;
                        controlBposBL += (calcpos * remainderB);
                        if (controlBposBR > controlBposBL) {
                            controlBposBR = (controlBposBR + controlBposBL) * 0.5;
                        }
                    }
                }
                else
                { // working on neg
                    if (true == fpFlip)
                    {
                        controlAnegBL *= divisorB;
                        controlAnegBL += (calcneg * remainderB);
                        if (controlAnegBR > controlAnegBL) {
                            controlAnegBR = (controlAnegBR + controlAnegBL) * 0.5;
                        }
                    }
                    else
                    {
                        controlBnegBL *= divisorB;
                        controlBnegBL += (calcneg * remainderB);
                        if (controlBnegBR > controlBnegBL) {
                            controlBnegBR = (controlBnegBR + controlBnegBL) * 0.5;
                        }
                    }
                }
                // this causes each of the four to update only when active and in the correct 'fpFlip'
                if (true == fpFlip)
                {
                    totalmultiplier = (controlAposBL * outputpos) + (controlAnegBL * outputneg);
                }
                else
                {
                    totalmultiplier = (controlBposBL * outputpos) + (controlBnegBL * outputneg);
                }
                // this combines the sides according to fpFlip, blending relative to the input value
                if (totalmultiplier != 1.0) {
                    inputSampleL *= totalmultiplier;
                }
                // if (compoutgain != 1.0) inputSample /= compoutgain;
                if (inputSampleL > 36.0) {
                    inputSampleL = 36.0;
                }
                if (inputSampleL < -36.0) {
                    inputSampleL = -36.0;
                }
                // build in +18db hard clip on insano inputs
                outSampleBL = inputSampleL / compoutgain;
                // end second compressorL
                // begin second compressorR
                inputpos = (inputSampleR * fpOld) + (avgBR * fpNew) + 1.0;
                avgBR = inputSampleR;
                if (inputpos < 0.001) {
                    inputpos = 0.001;
                }
                outputpos = inputpos / 2.0;
                if (outputpos > 1.0) {
                    outputpos = 1.0;
                }
                inputpos *= inputpos;
                // will be very high for hot input, can be 0.00001-1 for other-polarity
                dynamicRemainder = remainderB * (inputpos + 1.0);
                if (dynamicRemainder > 1.0) {
                    dynamicRemainder = 1.0;
                }
                dynamicDivisor = 1.0 - dynamicRemainder;
                // calc chases much faster if input swing is high
                targetposBR *= dynamicDivisor;
                targetposBR += (inputpos * dynamicRemainder);
                calcpos = pow((1.0 / targetposBR), 2);
                inputneg = (-inputSampleR * fpOld) + (nvgBR * fpNew) + 1.0;
                nvgBR = -inputSampleR;
                if (inputneg < 0.001) {
                    inputneg = 0.001;
                }
                outputneg = inputneg / 2.0;
                if (outputneg > 1.0) {
                    outputneg = 1.0;
                }
                inputneg *= inputneg;
                // will be very high for hot input, can be 0.00001-1 for other-polarity
                dynamicRemainder = remainderB * (inputneg + 1.0);
                if (dynamicRemainder > 1.0) {
                    dynamicRemainder = 1.0;
                }
                dynamicDivisor = 1.0 - dynamicRemainder;
                // calc chases much faster if input swing is high
                targetnegBR *= dynamicDivisor;
                targetnegBR += (inputneg * dynamicRemainder);
                calcneg = pow((1.0 / targetnegBR), 2);
                // now we have mirrored targets for comp
                // outputpos and outputneg go from 0 to 1
                if (inputSampleR > 0)
                { // working on pos
                    if (true == fpFlip)
                    {
                        controlAposBR *= divisorB;
                        controlAposBR += (calcpos * remainderB);
                        if (controlAposBL > controlAposBR) {
                            controlAposBL = (controlAposBR + controlAposBL) * 0.5;
                        }
                        // this part makes the compressor linked: both channels will converge towards whichever
                        // is the most compressed at the time.
                    }
                    else
                    {
                        controlBposBR *= divisorB;
                        controlBposBR += (calcpos * remainderB);
                        if (controlBposBL > controlBposBR) {
                            controlBposBL = (controlBposBR + controlBposBL) * 0.5;
                        }
                    }
                }
                else
                { // working on neg
                    if (true == fpFlip)
                    {
                        controlAnegBR *= divisorB;
                        controlAnegBR += (calcneg * remainderB);
                        if (controlAnegBL > controlAnegBR) {
                            controlAnegBL = (controlAnegBR + controlAnegBL) * 0.5;
                        }
                    }
                    else
                    {
                        controlBnegBR *= divisorB;
                        controlBnegBR += (calcneg * remainderB);
                        if (controlBnegBL > controlBnegBR) {
                            controlBnegBL = (controlBnegBR + controlBnegBL) * 0.5;
                        }
                    }
                }
                // this causes each of the four to update only when active and in the correct 'fpFlip'
                if (true == fpFlip)
                {
                    totalmultiplier = (controlAposBR * outputpos) + (controlAnegBR * outputneg);
                }
                else
                {
                    totalmultiplier = (controlBposBR * outputpos) + (controlBnegBR * outputneg);
                }
                // this combines the sides according to fpFlip, blending relative to the input value
                if (totalmultiplier != 1.0) {
                    inputSampleR *= totalmultiplier;
                }
                // if (compoutgain != 1.0) inputSample /= compoutgain;
                if (inputSampleR > 36.0) {
                    inputSampleR = 36.0;
                }
                if (inputSampleR < -36.0) {
                    inputSampleR = -36.0;
                }
                // build in +18db hard clip on insano inputs
                outSampleBR = inputSampleR / compoutgain;
                // end second compressorR
                if (ratioselector > 1) {
                    // begin third Power SagL
                    cL[gcount + 499] = cL[gcount] = fabs(inputSampleL) * (intensity - ((controlAposCL + controlBposCL + controlAnegCL + controlBnegCL) * powerSag));
                    controlCL += (cL[gcount] / offsetC);
                    controlCL -= (cL[gcount + offsetB] / offsetC);
                    controlCL -= 0.000001;
                    clamp = 1;
                    if (controlCL < 0) {
                        controlCL = 0;
                    }
                    if (controlCL > 1) {
                        clamp -= (controlCL - 1);
                        controlCL = 1;
                    }
                    if (clamp < 0.5) {
                        clamp = 0.5;
                    }
                    // control = 0 to 1
                    thickness = ((1.0 - controlCL) * 2.0) - 1.0;
                    out = fabs(thickness);
                    bridgerectifier = fabs(inputSampleL);
                    if (bridgerectifier > 1.57079633) {
                        bridgerectifier = 1.57079633;
                    }
                    // max value for sine function
                    if (thickness > 0) {
                        bridgerectifier = sin(bridgerectifier);
                    }
                    else {
                        bridgerectifier = 1 - cos(bridgerectifier);
                    }
                    // produce either boosted or starved version
                    if (inputSampleL > 0) {
                        inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
                    }
                    else {
                        inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
                    }
                    // blend according to density control
                    if (clamp != 1.0) {
                        inputSampleL *= clamp;
                    }
                    // end third Power SagL
                    // begin third Power SagR
                    cR[gcount + 499] = cR[gcount] = fabs(inputSampleR) * (intensity - ((controlAposCR + controlBposCR + controlAnegCR + controlBnegCR) * powerSag));
                    controlCR += (cR[gcount] / offsetC);
                    controlCR -= (cR[gcount + offsetB] / offsetC);
                    controlCR -= 0.000001;
                    clamp = 1;
                    if (controlCR < 0) {
                        controlCR = 0;
                    }
                    if (controlCR > 1) {
                        clamp -= (controlCR - 1);
                        controlCR = 1;
                    }
                    if (clamp < 0.5) {
                        clamp = 0.5;
                    }
                    // control = 0 to 1
                    thickness = ((1.0 - controlCR) * 2.0) - 1.0;
                    out = fabs(thickness);
                    bridgerectifier = fabs(inputSampleR);
                    if (bridgerectifier > 1.57079633) {
                        bridgerectifier = 1.57079633;
                    }
                    // max value for sine function
                    if (thickness > 0) {
                        bridgerectifier = sin(bridgerectifier);
                    }
                    else {
                        bridgerectifier = 1 - cos(bridgerectifier);
                    }
                    // produce either boosted or starved version
                    if (inputSampleR > 0) {
                        inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
                    }
                    else {
                        inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
                    }
                    // blend according to density control
                    if (clamp != 1.0) {
                        inputSampleR *= clamp;
                    }
                    // end third Power SagR
                    // begin third compressorL
                    inputpos = (inputSampleL * fpOld) + (avgCL * fpNew) + 1.0;
                    avgCL = inputSampleL;
                    if (inputpos < 0.001) {
                        inputpos = 0.001;
                    }
                    outputpos = inputpos / 2.0;
                    if (outputpos > 1.0) {
                        outputpos = 1.0;
                    }
                    inputpos *= inputpos;
                    // will be very high for hot input, can be 0.00001-1 for other-polarity
                    dynamicRemainder = remainderC * (inputpos + 1.0);
                    if (dynamicRemainder > 1.0) {
                        dynamicRemainder = 1.0;
                    }
                    dynamicDivisor = 1.0 - dynamicRemainder;
                    // calc chases much faster if input swing is high
                    targetposCL *= dynamicDivisor;
                    targetposCL += (inputpos * dynamicRemainder);
                    calcpos = pow((1.0 / targetposCL), 2);
                    inputneg = (-inputSampleL * fpOld) + (nvgCL * fpNew) + 1.0;
                    nvgCL = -inputSampleL;
                    if (inputneg < 0.001) {
                        inputneg = 0.001;
                    }
                    outputneg = inputneg / 2.0;
                    if (outputneg > 1.0) {
                        outputneg = 1.0;
                    }
                    inputneg *= inputneg;
                    // will be very high for hot input, can be 0.00001-1 for other-polarity
                    dynamicRemainder = remainderC * (inputneg + 1.0);
                    if (dynamicRemainder > 1.0) {
                        dynamicRemainder = 1.0;
                    }
                    dynamicDivisor = 1.0 - dynamicRemainder;
                    // calc chases much faster if input swing is high
                    targetnegCL *= dynamicDivisor;
                    targetnegCL += (inputneg * dynamicRemainder);
                    calcneg = pow((1.0 / targetnegCL), 2);
                    // now we have mirrored targets for comp
                    // outputpos and outputneg go from 0 to 1
                    if (inputSampleL > 0)
                    { // working on pos
                        if (true == fpFlip)
                        {
                            controlAposCL *= divisorC;
                            controlAposCL += (calcpos * remainderC);
                            if (controlAposCR > controlAposCL) {
                                controlAposCR = (controlAposCR + controlAposCL) * 0.5;
                            }
                            // this part makes the compressor linked: both channels will converge towards whichever
                            // is the most compressed at the time.
                        }
                        else
                        {
                            controlBposCL *= divisorC;
                            controlBposCL += (calcpos * remainderC);
                            if (controlBposCR > controlBposCL) {
                                controlBposCR = (controlBposCR + controlBposCL) * 0.5;
                            }
                        }
                    }
                    else
                    { // working on neg
                        if (true == fpFlip)
                        {
                            controlAnegCL *= divisorC;
                            controlAnegCL += (calcneg * remainderC);
                            if (controlAnegCR > controlAnegCL) {
                                controlAnegCR = (controlAnegCR + controlAnegCL) * 0.5;
                            }
                        }
                        else
                        {
                            controlBnegCL *= divisorC;
                            controlBnegCL += (calcneg * remainderC);
                            if (controlBnegCR > controlBnegCL) {
                                controlBnegCR = (controlBnegCR + controlBnegCL) * 0.5;
                            }
                        }
                    }
                    // this causes each of the four to update only when active and in the correct 'fpFlip'
                    if (true == fpFlip)
                    {
                        totalmultiplier = (controlAposCL * outputpos) + (controlAnegCL * outputneg);
                    }
                    else
                    {
                        totalmultiplier = (controlBposCL * outputpos) + (controlBnegCL * outputneg);
                    }
                    // this combines the sides according to fpFlip, blending relative to the input value
                    if (totalmultiplier != 1.0) {
                        inputSampleL *= totalmultiplier;
                    }
                    if (inputSampleL > 36.0) {
                        inputSampleL = 36.0;
                    }
                    if (inputSampleL < -36.0) {
                        inputSampleL = -36.0;
                    }
                    // build in +18db hard clip on insano inputs
                    outSampleCL = inputSampleL / compoutgain;
                    // if (compoutgain != 1.0) inputSample /= compoutgain;
                    // end third compressorL
                    // begin third compressorR
                    inputpos = (inputSampleR * fpOld) + (avgCR * fpNew) + 1.0;
                    avgCR = inputSampleR;
                    if (inputpos < 0.001) {
                        inputpos = 0.001;
                    }
                    outputpos = inputpos / 2.0;
                    if (outputpos > 1.0) {
                        outputpos = 1.0;
                    }
                    inputpos *= inputpos;
                    // will be very high for hot input, can be 0.00001-1 for other-polarity
                    dynamicRemainder = remainderC * (inputpos + 1.0);
                    if (dynamicRemainder > 1.0) {
                        dynamicRemainder = 1.0;
                    }
                    dynamicDivisor = 1.0 - dynamicRemainder;
                    // calc chases much faster if input swing is high
                    targetposCL *= dynamicDivisor;
                    targetposCL += (inputpos * dynamicRemainder);
                    calcpos = pow((1.0 / targetposCR), 2);
                    inputneg = (-inputSampleR * fpOld) + (nvgCR * fpNew) + 1.0;
                    nvgCR = -inputSampleR;
                    if (inputneg < 0.001) {
                        inputneg = 0.001;
                    }
                    outputneg = inputneg / 2.0;
                    if (outputneg > 1.0) {
                        outputneg = 1.0;
                    }
                    inputneg *= inputneg;
                    // will be very high for hot input, can be 0.00001-1 for other-polarity
                    dynamicRemainder = remainderC * (inputneg + 1.0);
                    if (dynamicRemainder > 1.0) {
                        dynamicRemainder = 1.0;
                    }
                    dynamicDivisor = 1.0 - dynamicRemainder;
                    // calc chases much faster if input swing is high
                    targetnegCR *= dynamicDivisor;
                    targetnegCR += (inputneg * dynamicRemainder);
                    calcneg = pow((1.0 / targetnegCR), 2);
                    // now we have mirrored targets for comp
                    // outputpos and outputneg go from 0 to 1
                    if (inputSampleR > 0)
                    { // working on pos
                        if (true == fpFlip)
                        {
                            controlAposCR *= divisorC;
                            controlAposCR += (calcpos * remainderC);
                            if (controlAposCL > controlAposCR) {
                                controlAposCL = (controlAposCR + controlAposCL) * 0.5;
                            }
                            // this part makes the compressor linked: both channels will converge towards whichever
                            // is the most compressed at the time.
                        }
                        else
                        {
                            controlBposCR *= divisorC;
                            controlBposCR += (calcpos * remainderC);
                            if (controlBposCL > controlBposCR) {
                                controlBposCL = (controlBposCR + controlBposCL) * 0.5;
                            }
                        }
                    }
                    else
                    { // working on neg
                        if (true == fpFlip)
                        {
                            controlAnegCR *= divisorC;
                            controlAnegCR += (calcneg * remainderC);
                            if (controlAnegCL > controlAnegCR) {
                                controlAnegCL = (controlAnegCR + controlAnegCL) * 0.5;
                            }
                        }
                        else
                        {
                            controlBnegCR *= divisorC;
                            controlBnegCR += (calcneg * remainderC);
                            if (controlBnegCL > controlBnegCR) {
                                controlBnegCL = (controlBnegCR + controlBnegCL) * 0.5;
                            }
                        }
                    }
                    // this causes each of the four to update only when active and in the correct 'fpFlip'
                    if (true == fpFlip)
                    {
                        totalmultiplier = (controlAposCR * outputpos) + (controlAnegCR * outputneg);
                    }
                    else
                    {
                        totalmultiplier = (controlBposCR * outputpos) + (controlBnegCR * outputneg);
                    }
                    // this combines the sides according to fpFlip, blending relative to the input value
                    if (totalmultiplier != 1.0) {
                        inputSampleR *= totalmultiplier;
                    }
                    if (inputSampleR > 36.0) {
                        inputSampleR = 36.0;
                    }
                    if (inputSampleR < -36.0) {
                        inputSampleR = -36.0;
                    }
                    // build in +18db hard clip on insano inputs
                    outSampleCR = inputSampleR / compoutgain;
                    // if (compoutgain != 1.0) inputSample /= compoutgain;
                    // end third compressorR
                }
            } // these nested if blocks are not indented because the old xCode doesn't support it
            // here we will interpolate between dry, and the three post-stages of processing
            switch (ratioselector) {
                case 0:
                    inputSampleL = (drySampleL * invRatio) + (outSampleAL * ratio);
                    inputSampleR = (drySampleR * invRatio) + (outSampleAR * ratio);
                    break;
                case 1:
                    inputSampleL = (outSampleAL * invRatio) + (outSampleBL * ratio);
                    inputSampleR = (outSampleAR * invRatio) + (outSampleBR * ratio);
                    break;
                default:
                    inputSampleL = (outSampleBL * invRatio) + (outSampleCL * ratio);
                    inputSampleR = (outSampleBR * invRatio) + (outSampleCR * ratio);
                    break;
            }
            // only then do we reconstruct the output, but our ratio is built here
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs::logical4
