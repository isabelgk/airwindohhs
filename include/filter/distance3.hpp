#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::distance3 {

constexpr std::string_view k_name{ "Distance3" };
constexpr std::string_view k_short_description{
    "Distance3 combines the best parts of Distance and Discontinuity."
};
constexpr std::string_view k_long_description{
    "By request from my livestreams, let's jump right back into the Discontinuity thing, but this time combined with a much older plugin: the original Distance!This is a kind of plugin meant to darken the sound and make stuff sound really far away. Originally, I was thinking something that could take all the highs out and accentuate rumble, like turning a sound into the thunder version of itself. And so the first Distance worked basically as an EQ: three stacked stages of processing that combined to make stuff huge, kind of like my monitoring plugin SubsOnly.Thing is, that doesn't have any nonlinearities to speak of in it, not the kind that happen in the real world over that much air. And at the time I was working on a Console version called Atmosphere, and thought I had a handle on bringing in that kind of nonlinearity. And so the next one was Distance2… but it lost some of the purity and depth of Distance, but didn't sound quite the way I wanted. It was the best I could do at the time, and is still there if you're interested in different sorts of darken/distort.And then I brought in Discontinuity and was working on it in livestreams and someone mentioned, what if it was part of Distance? And the interesting thing is that Discontinuity also gets its sound from… three stages of processing, stacked. (as in, not side-by-side but in series, one after the other.)Anytime you look at a situation like that, you can think to yourself: well, I could run these two plugins on after the other, but what if I interleaved the stages? One of Distance, one of Discontinuity, another Distance, another Discontinuity, and so on? Surely that would combine the effects in a more interesting way, merge them into a new distinct thing as they work on each other in turn?And so here is Distance3. It goes right back to the tone quality of Distance, but it has all of the 'loud vibe' from Discontinuity, and outperforms either of them if you need the synthesis of both. There's probably lots of uses for this and my hope is that it'll be very easy to find those uses: if a thing has to be convincingly far away and you've already got reverb and ambience taken care of, Distance3 should immediately get you there in the best possible way."
};
constexpr std::string_view k_tags{
    "filter"
};

constexpr int dscBuf = 90;

template <typename T>
class Distance3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    double lastclampAL;
    double clampAL;
    double changeAL;
    double prevresultAL;
    double lastAL;
    double lastclampBL;
    double clampBL;
    double changeBL;
    double prevresultBL;
    double lastBL;
    double lastclampCL;
    double clampCL;
    double changeCL;
    double prevresultCL;
    double lastCL;
    double dBaL[dscBuf + 5];
    double dBaPosL;
    int dBaXL;
    double dBbL[dscBuf + 5];
    double dBbPosL;
    int dBbXL;
    double dBcL[dscBuf + 5];
    double dBcPosL;
    int dBcXL;
    double lastclampAR;
    double clampAR;
    double changeAR;
    double prevresultAR;
    double lastAR;
    double lastclampBR;
    double clampBR;
    double changeBR;
    double prevresultBR;
    double lastBR;
    double lastclampCR;
    double clampCR;
    double changeCR;
    double prevresultCR;
    double lastCR;
    double dBaR[dscBuf + 5];
    double dBaPosR;
    int dBaXR;
    double dBbR[dscBuf + 5];
    double dBbPosR;
    int dBbXR;
    double dBcR[dscBuf + 5];
    double dBcPosR;
    int dBcXR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Distance3()
    {
        A = 0.5;
        B = 0.5;
        C = 1.0;
        prevresultAL = lastclampAL = clampAL = changeAL = lastAL = 0.0;
        prevresultBL = lastclampBL = clampBL = changeBL = lastBL = 0.0;
        prevresultCL = lastclampCL = clampCL = changeCL = lastCL = 0.0;
        prevresultAR = lastclampAR = clampAR = changeAR = lastAR = 0.0;
        prevresultBR = lastclampBR = clampBR = changeBR = lastBR = 0.0;
        prevresultCR = lastclampCR = clampCR = changeCR = lastCR = 0.0;
        for (int count = 0; count < dscBuf + 2; count++) {
            dBaL[count] = 0.0;
            dBbL[count] = 0.0;
            dBcL[count] = 0.0;
            dBaR[count] = 0.0;
            dBbR[count] = 0.0;
            dBcR[count] = 0.0;
        }
        dBaPosL = 0.0;
        dBbPosL = 0.0;
        dBcPosL = 0.0;
        dBaPosR = 0.0;
        dBbPosR = 0.0;
        dBcPosR = 0.0;
        dBaXL = 1;
        dBbXL = 1;
        dBcXL = 1;
        dBaXR = 1;
        dBbXR = 1;
        dBcXR = 1;
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
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

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
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "distance"; break;
            case kParamB: return "top db"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Distance"; break;
            case kParamB: return "Top dB"; break;
            case kParamC: return "Dry/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "miles"; break;
            case kParamB: return "dB"; break;
            case kParamC: return ""; break;

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
        double softslew = (A * 100.0) + 0.5;
        softslew *= overallscale;
        double outslew = softslew * (1.0 - (A * 0.333));
        double refdB = (B * 70.0) + 70.0;
        double topdB = 0.000000075 * pow(10.0, refdB / 20.0) * overallscale;
        double wet = C;
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
            inputSampleL *= softslew;
            lastclampAL = clampAL;
            clampAL = inputSampleL - lastAL;
            double postfilter = changeAL = fabs(clampAL - lastclampAL);
            postfilter += (softslew / 2.0);
            inputSampleL /= outslew;
            inputSampleL += (prevresultAL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            prevresultAL = inputSampleL;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleL *= topdB;
            if (inputSampleL < -0.222) {
                inputSampleL = -0.222;
            }
            if (inputSampleL > 0.222) {
                inputSampleL = 0.222;
            }
            // Air Discontinuity A begin
            dBaL[dBaXL] = inputSampleL;
            dBaPosL *= 0.5;
            dBaPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            int dBdly = floor(dBaPosL * dscBuf);
            double dBi = (dBaPosL * dscBuf) - dBdly;
            inputSampleL = dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXL++;
            if (dBaXL < 0 || dBaXL >= dscBuf) {
                dBaXL = 0;
            }
            // Air Discontinuity A end
            inputSampleL /= topdB;
            inputSampleL *= softslew;
            lastclampBL = clampBL;
            clampBL = inputSampleL - lastBL;
            postfilter = changeBL = fabs(clampBL - lastclampBL);
            postfilter += (softslew / 2.0);
            lastBL = inputSampleL;
            inputSampleL /= outslew;
            inputSampleL += (prevresultBL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            prevresultBL = inputSampleL;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleL *= topdB;
            if (inputSampleL < -0.222) {
                inputSampleL = -0.222;
            }
            if (inputSampleL > 0.222) {
                inputSampleL = 0.222;
            }
            // Air Discontinuity B begin
            dBbL[dBbXL] = inputSampleL;
            dBbPosL *= 0.5;
            dBbPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBbPosL * dscBuf);
            dBi = (dBbPosL * dscBuf) - dBdly;
            inputSampleL = dBbL[dBbXL - dBdly + ((dBbXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBbL[dBbXL - dBdly + ((dBbXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBbXL++;
            if (dBbXL < 0 || dBbXL >= dscBuf) {
                dBbXL = 0;
            }
            // Air Discontinuity B end
            inputSampleL /= topdB;
            inputSampleL *= softslew;
            lastclampCL = clampCL;
            clampCL = inputSampleL - lastCL;
            postfilter = changeCL = fabs(clampCL - lastclampCL);
            postfilter += (softslew / 2.0);
            lastCL = inputSampleL;
            inputSampleL /= softslew; // don't boost the final time!
            inputSampleL += (prevresultCL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            prevresultCL = inputSampleL;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleL *= topdB;
            if (inputSampleL < -0.222) {
                inputSampleL = -0.222;
            }
            if (inputSampleL > 0.222) {
                inputSampleL = 0.222;
            }
            // Air Discontinuity C begin
            dBcL[dBcXL] = inputSampleL;
            dBcPosL *= 0.5;
            dBcPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBcPosL * dscBuf);
            dBi = (dBcPosL * dscBuf) - dBdly;
            inputSampleL = dBcL[dBcXL - dBdly + ((dBcXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBcL[dBcXL - dBdly + ((dBcXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBcXL++;
            if (dBcXL < 0 || dBcXL >= dscBuf) {
                dBcXL = 0;
            }
            // Air Discontinuity C end
            inputSampleL /= topdB;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * (1.0 - wet)) + (inputSampleL * wet);
            }
            inputSampleR *= softslew;
            lastclampAR = clampAR;
            clampAR = inputSampleR - lastAR;
            postfilter = changeAR = fabs(clampAR - lastclampAR);
            postfilter += (softslew / 2.0);
            inputSampleR /= outslew;
            inputSampleR += (prevresultAR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            prevresultAR = inputSampleR;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleR *= topdB;
            if (inputSampleR < -0.222) {
                inputSampleR = -0.222;
            }
            if (inputSampleR > 0.222) {
                inputSampleR = 0.222;
            }
            // Air Discontinuity A begin
            dBaR[dBaXR] = inputSampleR;
            dBaPosR *= 0.5;
            dBaPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBaPosR * dscBuf);
            dBi = (dBaPosR * dscBuf) - dBdly;
            inputSampleR = dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXR++;
            if (dBaXR < 0 || dBaXR >= dscBuf) {
                dBaXR = 0;
            }
            // Air Discontinuity A end
            inputSampleR /= topdB;
            inputSampleR *= softslew;
            lastclampBR = clampBR;
            clampBR = inputSampleR - lastBR;
            postfilter = changeBR = fabs(clampBR - lastclampBR);
            postfilter += (softslew / 2.0);
            lastBR = inputSampleR;
            inputSampleR /= outslew;
            inputSampleR += (prevresultBR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            prevresultBR = inputSampleR;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleR *= topdB;
            if (inputSampleR < -0.222) {
                inputSampleR = -0.222;
            }
            if (inputSampleR > 0.222) {
                inputSampleR = 0.222;
            }
            // Air Discontinuity B begin
            dBbR[dBbXR] = inputSampleR;
            dBbPosR *= 0.5;
            dBbPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBbPosR * dscBuf);
            dBi = (dBbPosR * dscBuf) - dBdly;
            inputSampleR = dBbR[dBbXR - dBdly + ((dBbXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBbR[dBbXR - dBdly + ((dBbXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBbXR++;
            if (dBbXR < 0 || dBbXR >= dscBuf) {
                dBbXR = 0;
            }
            // Air Discontinuity B end
            inputSampleR /= topdB;
            inputSampleR *= softslew;
            lastclampCR = clampCR;
            clampCR = inputSampleR - lastCR;
            postfilter = changeCR = fabs(clampCR - lastclampCR);
            postfilter += (softslew / 2.0);
            lastCR = inputSampleR;
            inputSampleR /= softslew; // don't boost the final time!
            inputSampleR += (prevresultCR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            prevresultCR = inputSampleR;
            // do an IIR like thing to further squish superdistant stuff
            inputSampleR *= topdB;
            if (inputSampleR < -0.222) {
                inputSampleR = -0.222;
            }
            if (inputSampleR > 0.222) {
                inputSampleR = 0.222;
            }
            // Air Discontinuity C begin
            dBcR[dBcXR] = inputSampleR;
            dBcPosR *= 0.5;
            dBcPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBcPosR * dscBuf);
            dBi = (dBcPosR * dscBuf) - dBdly;
            inputSampleR = dBcR[dBcXR - dBdly + ((dBcXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBcR[dBcXR - dBdly + ((dBcXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBcXR++;
            if (dBcXR < 0 || dBcXR >= dscBuf) {
                dBcXR = 0;
            }
            // Air Discontinuity C end
            inputSampleR /= topdB;
            if (wet < 1.0) {
                inputSampleR = (drySampleR * (1.0 - wet)) + (inputSampleR * wet);
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
} // namespace airwindohhs::distance3
