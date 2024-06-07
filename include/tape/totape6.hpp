#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::totape6 {

constexpr std::string_view k_name{ "ToTape6" };
constexpr std::string_view k_short_description{
    "ToTape6 is Airwindows tape emulation for 2020!"
};
constexpr std::string_view k_long_description{
    "Here’s ToTape6. There are still things I want (simplification) but this is all the things you wanted :)ToTape6 brings in tech from UnBox, from Mojo, from BiquadOneHalf… it’s quite literally the culmination of all the stuff I’ve been doing over the last year (or indeed three years). I think it’ll register immediately as a killer tape emulation, perhaps THE killer tape emulation to beat for all that it’s an abstraction and not meant to be any specific tape machine. No GUI, no brandnames: just the tone, and the ability to soak up intense audio gracefully. And of course those delicious tape-machine lows, courtesy of the new head bump.Unlike ToTape5, the new ToTape6 is extremely well behaved in the sub-octaves. By that I mean it has them, tons of them, it’s got that ToTape/BassKit ability to uncork profoundly deep subs, but thanks to the interleaved biquad filters it cuts them right off and doesn’t let DC through. It’s got a special kind of servo damping built in because I found when you stacked them up four deep, the tape machines started subtly motorboating around 20 hz, so I fixed it. Even in its fixed state, be cautious about pushing the head bump beyond its default halfway setting as it’ll produce a LOT of serious lows, with an interesting compressed quality you won’t hear outside of ToTape and the real thing.Due to the influences of UnBox and Mojo, the new ToTape6 puts forth a lot of sonority when you hit it: warm harmonics stand out and project, due to the boosting happening in an overall midrange band that covers most of the audio range. The Soften control is subtly improved as well: if you’re looking to tape to warm up digital overtones and edges, ToTape6 does that effortlessly.Flutter is so profoundly improved that ToTape6 defaults to having it on, at 0.5. That causes the plugin to run a small, varying latency (bear this in mind). It’s also a step into a weirder realm: I was being asked to do a ‘trashed tape’ plugin. This might not be ALL of that, but the new Flutter algorithm is both clean up to extreme settings, and more capable of bonkers warble and wacky over-flutter. Check that out at the far reaches of the Flutter control. At settings below 0.5 the flutter is more usable in serious contexts, and as always if you set Flutter to zero you bypass the whole thing and get zero latency and a lack of interpolation between samples, giving you clearer highs to use Soften on. For serious mastering purposes I still recommend not using flutter, plus there are some electronic genres where it’d be disruptive.The dry/wet control has a trick in store: it only works in the normal way if you’re NOT using flutter. If you’re full-wet you can flutter as you please, but as you bring dry in, you’ll get a striking flanging effect that’s different depending on where your flutter setting is (flutter changing can cause sonic artifacts, but you can still do it for effect). The AU is ‘N to N’ meaning it will randomize stereo or even 5.1 surround: the VST runs true 2-channel stereo so it will more accurately resemble a stereo tape flange. Either way, it’s more of a stunt than an important effect, but it does mean you can be running subtle tape flutter effects and then seamlessly fade into an obvious full-mix flange, and then back to the solid, loud tape effect. This will work differently each time you do it, so be sure to print your results: flutter is a random process.Oh, one more thing: ToTape6 has the guts of ADClip built into it as a safety clipper. So you can use it as a final loudenator, at which it ought to be about as good as they get: if the tape slam isn’t getting you there, you can push the output volume past 0 dB to intentionally clip harder, or pull it back to make sure ToTape is the only distortion stage. If you’re seeing extended periods of -0.09 dB you’re seeing ADClip working."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class ToTape6 final : public Effect<T>
{
    double dL[502];
    double dR[502];
    int gcount;
    double rateof;
    double sweep;
    double nextmax;
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
    float C;
    float D;
    float E;
    float F;

  public:
    ToTape6()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 1.0;
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
        for (int temp = 0; temp < 501; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        gcount = 0;
        rateof = 0.5;
        sweep = M_PI;
        nextmax = 0.5;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
        flip = 0;
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
        kParamF = 5,
        kNumParameters = 6
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
        kParamF:
            F = value;
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
        kParamF:
            return F;
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
        kParamC:
            return 0.5;
            break;
        kParamD:
            return 0.5;
            break;
        kParamE:
            return 0.5;
            break;
        kParamF:
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
            return "input";
            break;
        kParamB:
            return "soften";
            break;
        kParamC:
            return "head b";
            break;
        kParamD:
            return "flutter";
            break;
        kParamE:
            return "output";
            break;
        kParamF:
            return "dry/wet";
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
            return "Input";
            break;
        kParamB:
            return "Soften";
            break;
        kParamC:
            return "Head B";
            break;
        kParamD:
            return "Flutter";
            break;
        kParamE:
            return "Output";
            break;
        kParamF:
            return "Dry/Wet";
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
        kParamF:
            return std::to_string(F);
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
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;
        kParamE:
            return "dB";
            break;
        kParamF:
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
        overallscale *= getSampleRate();
        double inputgain = pow(10.0, ((A - 0.5) * 24.0) / 20.0);
        double SoftenControl = pow(B, 2);
        double RollAmount = (1.0 - (SoftenControl * 0.45)) / overallscale;
        double HeadBumpControl = C * 0.25 * inputgain;
        double HeadBumpFreq = 0.12 / overallscale;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        biquadAL[0] = biquadBL[0] = biquadAR[0] = biquadBR[0] = 0.007 / overallscale;
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
        double depth = pow(D, 2) * overallscale * 70;
        double fluttertrim = (0.0024 * pow(D, 2)) / overallscale;
        double outputgain = pow(10.0, ((E - 0.5) * 24.0) / 20.0);
        double refclip = 0.99;
        double softness = 0.618033988749894848204586;
        double wet = F;
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
            if (inputgain < 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            } // gain cut before plugin
            double flutterrandy = (double(fpdL) / UINT32_MAX);
            // now we've got a random flutter, so we're messing with the pitch before tape effects go on
            if (gcount < 0 || gcount > 499) {
                gcount = 499;
            }
            dL[gcount] = inputSampleL;
            dR[gcount] = inputSampleR;
            int count = gcount;
            if (depth != 0.0) {
                double offset = depth + (depth * pow(rateof, 2) * sin(sweep));
                count += (int)floor(offset);
                inputSampleL = (dL[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
                inputSampleR = (dR[count - ((count > 499) ? 500 : 0)] * (1 - (offset - floor(offset))));
                inputSampleL += (dL[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));
                inputSampleR += (dR[count + 1 - ((count + 1 > 499) ? 500 : 0)] * (offset - floor(offset)));
                rateof = (rateof * (1.0 - fluttertrim)) + (nextmax * fluttertrim);
                sweep += rateof * fluttertrim;
                if (sweep >= (M_PI * 2.0)) {
                    sweep -= M_PI;
                    nextmax = 0.24 + (flutterrandy * 0.74);
                }
                // apply to input signal only when flutter is present, interpolate samples
            }
            gcount--;
            double vibDrySampleL = inputSampleL;
            double vibDrySampleR = inputSampleR;
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
            double groundSampleL = vibDrySampleL - inputSampleL; // set up UnBox on fluttered audio
            double groundSampleR = vibDrySampleR - inputSampleR; // set up UnBox on fluttered audio
            if (inputgain > 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            }
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
            inputSampleL += ((iirHeadBumpAL + iirHeadBumpBL) * HeadBumpControl);
            inputSampleR += ((iirHeadBumpAR + iirHeadBumpBR) * HeadBumpControl);
            // apply Fatten.
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            double mojo;
            mojo = pow(fabs(inputSampleL), 0.25);
            if (mojo > 0.0) {
                inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo);
            }
            // mojo is the one that flattens WAAAAY out very softly before wavefolding
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            mojo = pow(fabs(inputSampleR), 0.25);
            if (mojo > 0.0) {
                inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo);
            }
            // mojo is the one that flattens WAAAAY out very softly before wavefolding
            inputSampleL += groundSampleL; // apply UnBox processing
            inputSampleR += groundSampleR; // apply UnBox processing
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
            if (lastSampleL >= refclip)
            {
                if (inputSampleL < refclip) {
                    lastSampleL = ((refclip * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = refclip;
                }
            }
            if (lastSampleL <= -refclip)
            {
                if (inputSampleL > -refclip) {
                    lastSampleL = ((-refclip * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = -refclip;
                }
            }
            if (inputSampleL > refclip)
            {
                if (lastSampleL < refclip) {
                    inputSampleL = ((refclip * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = refclip;
                }
            }
            if (inputSampleL < -refclip)
            {
                if (lastSampleL > -refclip) {
                    inputSampleL = ((-refclip * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = -refclip;
                }
            }
            lastSampleL = inputSampleL; // end ADClip L
            if (lastSampleR >= refclip)
            {
                if (inputSampleR < refclip) {
                    lastSampleR = ((refclip * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = refclip;
                }
            }
            if (lastSampleR <= -refclip)
            {
                if (inputSampleR > -refclip) {
                    lastSampleR = ((-refclip * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = -refclip;
                }
            }
            if (inputSampleR > refclip)
            {
                if (lastSampleR < refclip) {
                    inputSampleR = ((refclip * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = refclip;
                }
            }
            if (inputSampleR < -refclip)
            {
                if (lastSampleR > -refclip) {
                    inputSampleR = ((-refclip * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = -refclip;
                }
            }
            lastSampleR = inputSampleR; // end ADClip R
            if (inputSampleL > refclip) {
                inputSampleL = refclip;
            }
            if (inputSampleL < -refclip) {
                inputSampleL = -refclip;
            }
            // final iron bar
            if (inputSampleR > refclip) {
                inputSampleR = refclip;
            }
            if (inputSampleR < -refclip) {
                inputSampleR = -refclip;
            }
            // final iron bar
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::totape6
