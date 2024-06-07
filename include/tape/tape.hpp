#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tape {

constexpr std::string_view k_name{ "Tape" };
constexpr std::string_view k_short_description{
    "Tape is simplified, all-purpose tape mojo: my personal jam."
};
constexpr std::string_view k_long_description{
    "This one’s for me. It’s very similar to last week’s ToTape, but with the following differences:-simpler name-overdrive uses Spiral, not the Mojo algorithm-simpler controls (not exactly ‘input gain’ but close)-changes to the Head Bump algorithm-no flutterThis is what I WANTED to do with ToTape6, and didn’t. In some ways, that’s good: if I’d axed flutter I would not have spent all day struggling with it and coming up with a better algorithm that more closely resembles real physical tape, something that could come in handy for future echo plugins etc. If I’d done the things I’ve done with Tape, to ToTape6, then ToTape6 wouldn’t be as adjustable as it is. There’s room for both, and I gave people the complicated many-knobs version because I know you too well ;) and I know what people like, and I’m there for you.But I also have dreams of my own. So, the plugin (first ever from Airwindows) that carries just the generic name ‘Tape’ is Airwindows tape emulation MY way.I’ve heard a lot of tape in my time, being over 50 years old. I’ve dubbed and re-dubbed tapes a lot. So I dialed in (and re-programmed) Tape by loading up eight instances of it, in a row, and making it behave itself as well as could be expected while running audio through eight instances of Tape. It’s not meant to be clean if you do that: it’s meant to be eightfold trash, but the right kind of trash I’m familiar with when you’ve got that much generation loss and head bump buildup. I knew that if I could get that right, if I could get it to behave okay under that kind of duress, I could rely on it as a go-to output stage (going just before Monitoring) that would condition the sound in the right kind of way.And so it does.Tape will be heard from again, but much as ToTape5 bore the standard for Airwindows tape emulation for years, Tape is my personal choice for ‘mix into’ DAW output stage and it’ll stand for a while, I think. If you need more phat or more flutter or more controls etc etc, use ToTape6, which is just as good in many ways. This is just my ‘director’s cut’ version, designed to my tastes, for if you trust my ears and my choices. Since it’s Airwindows, ToTape6 (and 5) still works and you can have both. This one is for those of you who pursue the simple creed: I have a (virtual) tape machine. I record to my tape machine. I am happy. :)This plugin came out with an original version and then had a Redux update in which I added a control: a slider for the head bump, as people were finding the head bump excessive. Cranking it up all the way gave you the original behavior, but I defaulted it to 0.5."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class Tape final : public Effect<T>
{
    double iirMidRollerAL;
    double iirMidRollerBL;
    double iirHeadBumpAL;
    double iirHeadBumpBL;
    double iirMidRollerAR;
    double iirMidRollerBR;
    double iirHeadBumpAR;
    double iirHeadBumpBR;
    double biquadAL[9];
    double biquadBL[9];
    double biquadCL[9];
    double biquadDL[9];
    double biquadAR[9];
    double biquadBR[9];
    double biquadCR[9];
    double biquadDR[9];
    bool flip;
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Tape()
    {
        A = 0.5;
        B = 0.5;
        iirMidRollerAL = 0.0;
        iirMidRollerBL = 0.0;
        iirHeadBumpAL = 0.0;
        iirHeadBumpBL = 0.0;
        iirMidRollerAR = 0.0;
        iirMidRollerBR = 0.0;
        iirHeadBumpAR = 0.0;
        iirHeadBumpBR = 0.0;
        for (int x = 0; x < 9; x++) {
            biquadAL[x] = 0.0;
            biquadBL[x] = 0.0;
            biquadCL[x] = 0.0;
            biquadDL[x] = 0.0;
            biquadAR[x] = 0.0;
            biquadBR[x] = 0.0;
            biquadCR[x] = 0.0;
            biquadDR[x] = 0.0;
        }
        flip = false;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
            return 0.5;
            break;
        kParamB:
            return 0.5;
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
            return "slam";
            break;
        kParamB:
            return "bump";
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
            return "Slam";
            break;
        kParamB:
            return "Bump";
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
            return "dB";
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
        double inputgain = pow(10.0, ((A - 0.5) * 24.0) / 20.0);
        double bumpgain = B * 0.1;
        double HeadBumpFreq = 0.12 / overallscale;
        double softness = 0.618033988749894848204586;
        double RollAmount = (1.0 - softness) / overallscale;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        biquadAL[0] = biquadBL[0] = biquadAR[0] = biquadBR[0] = 0.0072 / overallscale;
        biquadAL[1] = biquadBL[1] = biquadAR[1] = biquadBR[1] = 0.0009;
        double K = tan(M_PI * biquadBR[0]);
        double norm = 1.0 / (1.0 + K / biquadBR[1] + K * K);
        biquadAL[2] = biquadBL[2] = biquadAR[2] = biquadBR[2] = K / biquadBR[1] * norm;
        biquadAL[4] = biquadBL[4] = biquadAR[4] = biquadBR[4] = -biquadBR[2];
        biquadAL[5] = biquadBL[5] = biquadAR[5] = biquadBR[5] = 2.0 * (K * K - 1.0) * norm;
        biquadAL[6] = biquadBL[6] = biquadAR[6] = biquadBR[6] = (1.0 - K / biquadBR[1] + K * K) * norm;
        biquadCL[0] = biquadDL[0] = biquadCR[0] = biquadDR[0] = 0.032 / overallscale;
        biquadCL[1] = biquadDL[1] = biquadCR[1] = biquadDR[1] = 0.0007;
        K = tan(M_PI * biquadDR[0]);
        norm = 1.0 / (1.0 + K / biquadDR[1] + K * K);
        biquadCL[2] = biquadDL[2] = biquadCR[2] = biquadDR[2] = K / biquadDR[1] * norm;
        biquadCL[4] = biquadDL[4] = biquadCR[4] = biquadDR[4] = -biquadDR[2];
        biquadCL[5] = biquadDL[5] = biquadCR[5] = biquadDR[5] = 2.0 * (K * K - 1.0) * norm;
        biquadCL[6] = biquadDL[6] = biquadCR[6] = biquadDR[6] = (1.0 - K / biquadDR[1] + K * K) * norm;
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
            if (inputgain < 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            } // gain cut before anything, even dry
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            double HighsSampleL = 0.0;
            double HighsSampleR = 0.0;
            double NonHighsSampleL = 0.0;
            double NonHighsSampleR = 0.0;
            double tempSample;
            if (flip)
            {
                iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                HighsSampleL = inputSampleL - iirMidRollerAL;
                HighsSampleR = inputSampleR - iirMidRollerAR;
                NonHighsSampleL = iirMidRollerAL;
                NonHighsSampleR = iirMidRollerAR;
                iirHeadBumpAL += (inputSampleL * 0.05);
                iirHeadBumpAR += (inputSampleR * 0.05);
                iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
                iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
                iirHeadBumpAL = sin(iirHeadBumpAL);
                iirHeadBumpAR = sin(iirHeadBumpAR);
                tempSample = (iirHeadBumpAL * biquadAL[2]) + biquadAL[7];
                biquadAL[7] = (iirHeadBumpAL * biquadAL[3]) - (tempSample * biquadAL[5]) + biquadAL[8];
                biquadAL[8] = (iirHeadBumpAL * biquadAL[4]) - (tempSample * biquadAL[6]);
                iirHeadBumpAL = tempSample; // interleaved biquad
                if (iirHeadBumpAL > 1.0) {
                    iirHeadBumpAL = 1.0;
                }
                if (iirHeadBumpAL < -1.0) {
                    iirHeadBumpAL = -1.0;
                }
                iirHeadBumpAL = asin(iirHeadBumpAL);
                tempSample = (iirHeadBumpAR * biquadAR[2]) + biquadAR[7];
                biquadAR[7] = (iirHeadBumpAR * biquadAR[3]) - (tempSample * biquadAR[5]) + biquadAR[8];
                biquadAR[8] = (iirHeadBumpAR * biquadAR[4]) - (tempSample * biquadAR[6]);
                iirHeadBumpAR = tempSample; // interleaved biquad
                if (iirHeadBumpAR > 1.0) {
                    iirHeadBumpAR = 1.0;
                }
                if (iirHeadBumpAR < -1.0) {
                    iirHeadBumpAR = -1.0;
                }
                iirHeadBumpAR = asin(iirHeadBumpAR);
                inputSampleL = sin(inputSampleL);
                tempSample = (inputSampleL * biquadCL[2]) + biquadCL[7];
                biquadCL[7] = (inputSampleL * biquadCL[3]) - (tempSample * biquadCL[5]) + biquadCL[8];
                biquadCL[8] = (inputSampleL * biquadCL[4]) - (tempSample * biquadCL[6]);
                inputSampleL = tempSample; // interleaved biquad
                if (inputSampleL > 1.0) {
                    inputSampleL = 1.0;
                }
                if (inputSampleL < -1.0) {
                    inputSampleL = -1.0;
                }
                inputSampleL = asin(inputSampleL);
                inputSampleR = sin(inputSampleR);
                tempSample = (inputSampleR * biquadCR[2]) + biquadCR[7];
                biquadCR[7] = (inputSampleR * biquadCR[3]) - (tempSample * biquadCR[5]) + biquadCR[8];
                biquadCR[8] = (inputSampleR * biquadCR[4]) - (tempSample * biquadCR[6]);
                inputSampleR = tempSample; // interleaved biquad
                if (inputSampleR > 1.0) {
                    inputSampleR = 1.0;
                }
                if (inputSampleR < -1.0) {
                    inputSampleR = -1.0;
                }
                inputSampleR = asin(inputSampleR);
            }
            else {
                iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                HighsSampleL = inputSampleL - iirMidRollerBL;
                HighsSampleR = inputSampleR - iirMidRollerBR;
                NonHighsSampleL = iirMidRollerBL;
                NonHighsSampleR = iirMidRollerBR;
                iirHeadBumpBL += (inputSampleL * 0.05);
                iirHeadBumpBR += (inputSampleR * 0.05);
                iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
                iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
                iirHeadBumpBL = sin(iirHeadBumpBL);
                iirHeadBumpBR = sin(iirHeadBumpBR);
                tempSample = (iirHeadBumpBL * biquadBL[2]) + biquadBL[7];
                biquadBL[7] = (iirHeadBumpBL * biquadBL[3]) - (tempSample * biquadBL[5]) + biquadBL[8];
                biquadBL[8] = (iirHeadBumpBL * biquadBL[4]) - (tempSample * biquadBL[6]);
                iirHeadBumpBL = tempSample; // interleaved biquad
                if (iirHeadBumpBL > 1.0) {
                    iirHeadBumpBL = 1.0;
                }
                if (iirHeadBumpBL < -1.0) {
                    iirHeadBumpBL = -1.0;
                }
                iirHeadBumpBL = asin(iirHeadBumpBL);
                tempSample = (iirHeadBumpBR * biquadBR[2]) + biquadBR[7];
                biquadBR[7] = (iirHeadBumpBR * biquadBR[3]) - (tempSample * biquadBR[5]) + biquadBR[8];
                biquadBR[8] = (iirHeadBumpBR * biquadBR[4]) - (tempSample * biquadBR[6]);
                iirHeadBumpBR = tempSample; // interleaved biquad
                if (iirHeadBumpBR > 1.0) {
                    iirHeadBumpBR = 1.0;
                }
                if (iirHeadBumpBR < -1.0) {
                    iirHeadBumpBR = -1.0;
                }
                iirHeadBumpBR = asin(iirHeadBumpBR);
                inputSampleL = sin(inputSampleL);
                tempSample = (inputSampleL * biquadDL[2]) + biquadDL[7];
                biquadDL[7] = (inputSampleL * biquadDL[3]) - (tempSample * biquadDL[5]) + biquadDL[8];
                biquadDL[8] = (inputSampleL * biquadDL[4]) - (tempSample * biquadDL[6]);
                inputSampleL = tempSample; // interleaved biquad
                if (inputSampleL > 1.0) {
                    inputSampleL = 1.0;
                }
                if (inputSampleL < -1.0) {
                    inputSampleL = -1.0;
                }
                inputSampleL = asin(inputSampleL);
                inputSampleR = sin(inputSampleR);
                tempSample = (inputSampleR * biquadDR[2]) + biquadDR[7];
                biquadDR[7] = (inputSampleR * biquadDR[3]) - (tempSample * biquadDR[5]) + biquadDR[8];
                biquadDR[8] = (inputSampleR * biquadDR[4]) - (tempSample * biquadDR[6]);
                inputSampleR = tempSample; // interleaved biquad
                if (inputSampleR > 1.0) {
                    inputSampleR = 1.0;
                }
                if (inputSampleR < -1.0) {
                    inputSampleR = -1.0;
                }
                inputSampleR = asin(inputSampleR);
            }
            flip = !flip;
            double groundSampleL = drySampleL - inputSampleL; // set up UnBox
            double groundSampleR = drySampleR - inputSampleR; // set up UnBox
            if (inputgain > 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            } // gain boost inside UnBox: do not boost fringe audio
            double applySoften = fabs(HighsSampleL) * 1.57079633;
            if (applySoften > 1.57079633) {
                applySoften = 1.57079633;
            }
            applySoften = 1 - cos(applySoften);
            if (HighsSampleL > 0) {
                inputSampleL -= applySoften;
            }
            if (HighsSampleL < 0) {
                inputSampleL += applySoften;
            }
            // apply Soften depending on polarity
            applySoften = fabs(HighsSampleR) * 1.57079633;
            if (applySoften > 1.57079633) {
                applySoften = 1.57079633;
            }
            applySoften = 1 - cos(applySoften);
            if (HighsSampleR > 0) {
                inputSampleR -= applySoften;
            }
            if (HighsSampleR < 0) {
                inputSampleR += applySoften;
            }
            // apply Soften depending on polarity
            if (inputSampleL > 1.2533141373155) {
                inputSampleL = 1.2533141373155;
            }
            if (inputSampleL < -1.2533141373155) {
                inputSampleL = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            // Spiral, for cleanest most optimal tape effect
            if (inputSampleR > 1.2533141373155) {
                inputSampleR = 1.2533141373155;
            }
            if (inputSampleR < -1.2533141373155) {
                inputSampleR = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            // Spiral, for cleanest most optimal tape effect
            double suppress = (1.0 - fabs(inputSampleL)) * 0.00013;
            if (iirHeadBumpAL > suppress) {
                iirHeadBumpAL -= suppress;
            }
            if (iirHeadBumpAL < -suppress) {
                iirHeadBumpAL += suppress;
            }
            if (iirHeadBumpBL > suppress) {
                iirHeadBumpBL -= suppress;
            }
            if (iirHeadBumpBL < -suppress) {
                iirHeadBumpBL += suppress;
            }
            // restrain resonant quality of head bump algorithm
            suppress = (1.0 - fabs(inputSampleR)) * 0.00013;
            if (iirHeadBumpAR > suppress) {
                iirHeadBumpAR -= suppress;
            }
            if (iirHeadBumpAR < -suppress) {
                iirHeadBumpAR += suppress;
            }
            if (iirHeadBumpBR > suppress) {
                iirHeadBumpBR -= suppress;
            }
            if (iirHeadBumpBR < -suppress) {
                iirHeadBumpBR += suppress;
            }
            // restrain resonant quality of head bump algorithm
            inputSampleL += groundSampleL; // apply UnBox processing
            inputSampleR += groundSampleR; // apply UnBox processing
            inputSampleL += ((iirHeadBumpAL + iirHeadBumpBL) * bumpgain); // and head bump
            inputSampleR += ((iirHeadBumpAR + iirHeadBumpBR) * bumpgain); // and head bump
            if (lastSampleL >= 0.99)
            {
                if (inputSampleL < 0.99) {
                    lastSampleL = ((0.99 * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = 0.99;
                }
            }
            if (lastSampleL <= -0.99)
            {
                if (inputSampleL > -0.99) {
                    lastSampleL = ((-0.99 * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = -0.99;
                }
            }
            if (inputSampleL > 0.99)
            {
                if (lastSampleL < 0.99) {
                    inputSampleL = ((0.99 * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = 0.99;
                }
            }
            if (inputSampleL < -0.99)
            {
                if (lastSampleL > -0.99) {
                    inputSampleL = ((-0.99 * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = -0.99;
                }
            }
            lastSampleL = inputSampleL; // end ADClip L
            if (lastSampleR >= 0.99)
            {
                if (inputSampleR < 0.99) {
                    lastSampleR = ((0.99 * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = 0.99;
                }
            }
            if (lastSampleR <= -0.99)
            {
                if (inputSampleR > -0.99) {
                    lastSampleR = ((-0.99 * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = -0.99;
                }
            }
            if (inputSampleR > 0.99)
            {
                if (lastSampleR < 0.99) {
                    inputSampleR = ((0.99 * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = 0.99;
                }
            }
            if (inputSampleR < -0.99)
            {
                if (lastSampleR > -0.99) {
                    inputSampleR = ((-0.99 * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = -0.99;
                }
            }
            lastSampleR = inputSampleR; // end ADClip R
            if (inputSampleL > 0.99) {
                inputSampleL = 0.99;
            }
            if (inputSampleL < -0.99) {
                inputSampleL = -0.99;
            }
            // final iron bar
            if (inputSampleR > 0.99) {
                inputSampleR = 0.99;
            }
            if (inputSampleR < -0.99) {
                inputSampleR = -0.99;
            }
            // final iron bar
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
} // namespace airwindohhs::tape
