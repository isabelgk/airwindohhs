#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::brightambience3 {

constexpr std::string_view k_name{ "BrightAmbience3" };
constexpr std::string_view k_short_description{
    "BrightAmbience3 adds undersampling for high sample rates, and better feedback."
};
constexpr std::string_view k_long_description{
    "BrightAmbience is a very old plugin. The original was all about taking sounds coming in, and transforming them into lengths of extruded and very bright reverb. BrightAmbience2 transformed that, in turn, into a more adaptable creation that used inter-aural delays to create a subtle stereo effect like an aura around mono content.BrightAmbience3 adds undersampling. Now high sample rate mixes retain a consistent tone and reverb length to what the CD-rate plugin would do… and it’s more CPU-efficient running at the elevated rates… and the subtle darkening in tone makes it worth a re-listen.But now, on top of all that, we’ve got a new way to apply feedback at the ‘wider’ reverb settings, which allows you to feed THOSE back too. And that means, BrightAmbience3 has just taken on a new life for a variety of vibey, distinctly analog-feeling blurred delay effects. Even the really wide reverb settings will still feed back at full crank (though they just give you a sort of droney resonant quality) and the medium settings produce a variety of unusual sounds that are a little bit like when you have a crummy old antique echo effect, and it has no clarity, but when you turn up the feedback strong retro flavors begin to take over… you can’t get clean infinite regeneration that way, but tune it to taste and dial back the feedback control until you have enough echo for your purposes.Or, ignore the feedback and just use it as BrightAmbience, but with a greater range of effect at higher sample rates, and a richer tone thanks to the undersampling."
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class BrightAmbience3 final : public Effect<T>
{
    int gcount;
    double pL[32768];
    double pR[32768];
    double feedbackA;
    double feedbackB;
    double figureL[9];
    double figureR[9];
    double lastRefL[10];
    double lastRefR[10];
    int cycle;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    BrightAmbience3()
    {
        A = 1.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        for (int count = 0; count < 32767; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        feedbackA = feedbackB = 0.0;
        for (int x = 0; x < 9; x++) {
            figureL[x] = 0.0;
            figureR[x] = 0.0;
        }
        for (int c = 0; c < 9; c++) {
            lastRefL[c] = 0.0;
            lastRefR[c] = 0.0;
        }
        cycle = 0;
        gcount = 0;
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
        kNumParameters = 4
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.0;
            break;
        kParamD:
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
            return "start";
            break;
        kParamB:
            return "length";
            break;
        kParamC:
            return "feedback";
            break;
        kParamD:
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
            return "Start";
            break;
        kParamB:
            return "Length";
            break;
        kParamC:
            return "Feedback";
            break;
        kParamD:
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
        int start = (int)(A * 400) + 88;
        int length = (int)(pow(B, 2) * 487) + 1;
        if (start + length > 488) {
            start = 488 - length;
        }
        double feedbackAmount = C * 0.25;
        double wet = D;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        figureL[0] = figureR[0] = 1000.0 / Effect<T>::getSampleRate(); // fixed frequency, 3.515775k
        figureL[1] = figureR[1] = pow(length * 0.037 * feedbackAmount, 2) + 0.01; // resonance
        double K = tan(M_PI * figureR[0]);
        double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
        figureL[2] = figureR[2] = K / figureR[1] * norm;
        figureL[4] = figureR[4] = -figureR[2];
        figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
        figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
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
                double tempL = 0.0;
                double tempR = 0.0;
                if (gcount < 0 || gcount > 32767) {
                    gcount = 32767;
                }
                int count = gcount;
                pL[count] = inputSampleL + feedbackB;
                pR[count] = inputSampleR + feedbackA;
                for (int offset = start; offset < start + length; offset++) {
                    tempL += pL[count + primeL[offset] - ((count + primeL[offset] > 32767) ? 32768 : 0)];
                    tempR += pR[count + primeR[offset] - ((count + primeR[offset] > 32767) ? 32768 : 0)];
                }
                inputSampleL = tempL / cbrt(length);
                inputSampleR = tempR / cbrt(length);
                double tempSample = (inputSampleL * figureL[2]) + figureL[7];
                figureL[7] = -(tempSample * figureL[5]) + figureL[8];
                figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
                feedbackA = sin(tempSample) * feedbackAmount;
                tempSample = (inputSampleR * figureR[2]) + figureR[7];
                figureR[7] = -(tempSample * figureR[5]) + figureR[8];
                figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
                feedbackB = sin(tempSample) * feedbackAmount;
                gcount--;
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
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
} // namespace airwindohhs::brightambience3
