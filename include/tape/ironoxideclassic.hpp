#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::ironoxideclassic {

constexpr std::string_view k_name{ "IronOxideClassic" };
constexpr std::string_view k_short_description{
    "IronOxideClassic is the purer, simpler, early form of Iron Oxide before all the features."
};
constexpr std::string_view k_long_description{
    "As promised, here is the 2017-ized version of the pure, sweet, original Iron Oxide. No more grit or tape flutter or noise!It’s funny how this works. If you’re a commercial developer, and you release a plugin that’s real popular, one thing that happens is people begin asking for more. More features, more variations, this and that and the other. The flutter in Iron Oxide 5 came about that way: it migrated over from ToTape (which is also coming to free VST).Every new thing added is something lost. But since I’m no longer doing strictly commercial development (it’s steadily all becoming free, backed by my Patreon which allows all this to happen) I can do things like confuse the ‘market’ and release both the feature-full Iron Oxide 5, and the stripped-down Iron Oxide Classic. This one is just like the original: input trim, ips control, and output trim. Better yet, it has the pure unsullied tone of the very first Iron Oxide, only brought up to date so it noise shapes to the 32 bit buss etc.Even if you liked the grunge factor of the very adjustable Iron Oxide 5 (more controls may be added but bear in mind I have a commitment to release plugins like BussColors, not just keep revising Iron Oxide!) you might want to check this out. And if later versions of Iron Oxide wandered away from what works for you, for instance if you’re making electronic music and needed much cleaner handling of synthetic tones… this is your lucky day!Hope you like Iron Oxide Classic. It is, truly, one of the Airwindows classics, now for free VST and brought up to date. :)"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class IronOxideClassic final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double dL[264];
    double fastIIRAL;
    double fastIIRBL;
    double slowIIRAL;
    double slowIIRBL;
    double iirSampleAR;
    double iirSampleBR;
    double dR[264];
    double fastIIRAR;
    double fastIIRBR;
    double slowIIRAR;
    double slowIIRBR;
    int gcount;
    bool fpFlip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    IronOxideClassic()
    {
        A = 0.5;
        B = 0.562341325190349;
        C = 0.5;
        for (int temp = 0; temp < 263; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        gcount = 0;
        fastIIRAL = fastIIRBL = slowIIRAL = slowIIRBL = 0.0;
        iirSampleAL = iirSampleBL = 0.0;
        fastIIRAR = fastIIRBR = slowIIRAR = slowIIRBR = 0.0;
        iirSampleAR = iirSampleBR = 0.0;
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
            case kParamB: return 0.562341325190349; break;
            case kParamC: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input trim"; break;
            case kParamB: return "tape speed"; break;
            case kParamC: return "output trim"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input Trim"; break;
            case kParamB: return "Tape Speed"; break;
            case kParamC: return "Output Trim"; break;

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
            case kParamA: return "dB"; break;
            case kParamB: return "ips"; break;
            case kParamC: return "dB"; break;

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
        double inputgain = pow(10.0, ((A * 36.0) - 18.0) / 20.0);
        double outputgain = pow(10.0, ((C * 36.0) - 18.0) / 20.0);
        double ips = (((B * B) * (B * B) * 148.5) + 1.5) * 1.1;
        // slight correction to dial in convincing ips settings
        if (ips < 1 || ips > 200) {
            ips = 33.0;
        }
        // sanity checks are always key
        double iirAmount = ips / 430.0; // for low leaning
        double bridgerectifierL;
        double bridgerectifierR;
        double fastTaper = ips / 15.0;
        double slowTaper = 2.0 / (ips * ips);
        double lowspeedscale = (5.0 / ips);
        int count;
        double temp;
        if (overallscale == 0) {
            fastTaper += 1.0;
            slowTaper += 1.0;
        }
        else
        {
            iirAmount /= overallscale;
            lowspeedscale *= overallscale;
            fastTaper = 1.0 + (fastTaper / overallscale);
            slowTaper = 1.0 + (slowTaper / overallscale);
        }
        double inputSampleL;
        double inputSampleR;
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
            if (fpFlip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleAL;
                inputSampleR -= iirSampleAR;
            }
            else
            {
                iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleL -= iirSampleBL;
                inputSampleR -= iirSampleBR;
            }
            // do IIR highpass for leaning out
            if (inputgain != 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            }
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // preliminary gain stage using antialiasing
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // preliminary gain stage using antialiasing
            // over to the Iron Oxide shaping code using inputsample
            if (gcount < 0 || gcount > 131) {
                gcount = 131;
            }
            count = gcount;
            // increment the counter
            dL[count + 131] = dL[count] = inputSampleL;
            dR[count + 131] = dR[count] = inputSampleR;
            if (fpFlip)
            {
                fastIIRAL = fastIIRAL / fastTaper;
                slowIIRAL = slowIIRAL / slowTaper;
                fastIIRAL += dL[count];
                // scale stuff down
                fastIIRAR = fastIIRAR / fastTaper;
                slowIIRAR = slowIIRAR / slowTaper;
                fastIIRAR += dR[count];
                // scale stuff down
                count += 3;
                temp = dL[count + 127];
                temp += dL[count + 113];
                temp += dL[count + 109];
                temp += dL[count + 107];
                temp += dL[count + 103];
                temp += dL[count + 101];
                temp += dL[count + 97];
                temp += dL[count + 89];
                temp += dL[count + 83];
                temp /= 2;
                temp += dL[count + 79];
                temp += dL[count + 73];
                temp += dL[count + 71];
                temp += dL[count + 67];
                temp += dL[count + 61];
                temp += dL[count + 59];
                temp += dL[count + 53];
                temp += dL[count + 47];
                temp += dL[count + 43];
                temp += dL[count + 41];
                temp += dL[count + 37];
                temp += dL[count + 31];
                temp += dL[count + 29];
                temp /= 2;
                temp += dL[count + 23];
                temp += dL[count + 19];
                temp += dL[count + 17];
                temp += dL[count + 13];
                temp += dL[count + 11];
                temp /= 2;
                temp += dL[count + 7];
                temp += dL[count + 5];
                temp += dL[count + 3];
                temp /= 2;
                temp += dL[count + 2];
                temp += dL[count + 1]; // end L
                slowIIRAL += (temp / 128);
                temp = dR[count + 127];
                temp += dR[count + 113];
                temp += dR[count + 109];
                temp += dR[count + 107];
                temp += dR[count + 103];
                temp += dR[count + 101];
                temp += dR[count + 97];
                temp += dR[count + 89];
                temp += dR[count + 83];
                temp /= 2;
                temp += dR[count + 79];
                temp += dR[count + 73];
                temp += dR[count + 71];
                temp += dR[count + 67];
                temp += dR[count + 61];
                temp += dR[count + 59];
                temp += dR[count + 53];
                temp += dR[count + 47];
                temp += dR[count + 43];
                temp += dR[count + 41];
                temp += dR[count + 37];
                temp += dR[count + 31];
                temp += dR[count + 29];
                temp /= 2;
                temp += dR[count + 23];
                temp += dR[count + 19];
                temp += dR[count + 17];
                temp += dR[count + 13];
                temp += dR[count + 11];
                temp /= 2;
                temp += dR[count + 7];
                temp += dR[count + 5];
                temp += dR[count + 3];
                temp /= 2;
                temp += dR[count + 2];
                temp += dR[count + 1]; // end R
                slowIIRAR += (temp / 128);
                inputSampleL = fastIIRAL - (slowIIRAL / slowTaper);
                inputSampleR = fastIIRAR - (slowIIRAR / slowTaper);
            }
            else
            {
                fastIIRBL = fastIIRBL / fastTaper;
                slowIIRBL = slowIIRBL / slowTaper;
                fastIIRBL += dL[count];
                // scale stuff down
                fastIIRBR = fastIIRBR / fastTaper;
                slowIIRBR = slowIIRBR / slowTaper;
                fastIIRBR += dR[count];
                // scale stuff down
                count += 3;
                temp = dL[count + 127];
                temp += dL[count + 113];
                temp += dL[count + 109];
                temp += dL[count + 107];
                temp += dL[count + 103];
                temp += dL[count + 101];
                temp += dL[count + 97];
                temp += dL[count + 89];
                temp += dL[count + 83];
                temp /= 2;
                temp += dL[count + 79];
                temp += dL[count + 73];
                temp += dL[count + 71];
                temp += dL[count + 67];
                temp += dL[count + 61];
                temp += dL[count + 59];
                temp += dL[count + 53];
                temp += dL[count + 47];
                temp += dL[count + 43];
                temp += dL[count + 41];
                temp += dL[count + 37];
                temp += dL[count + 31];
                temp += dL[count + 29];
                temp /= 2;
                temp += dL[count + 23];
                temp += dL[count + 19];
                temp += dL[count + 17];
                temp += dL[count + 13];
                temp += dL[count + 11];
                temp /= 2;
                temp += dL[count + 7];
                temp += dL[count + 5];
                temp += dL[count + 3];
                temp /= 2;
                temp += dL[count + 2];
                temp += dL[count + 1];
                slowIIRBL += (temp / 128);
                temp = dR[count + 127];
                temp += dR[count + 113];
                temp += dR[count + 109];
                temp += dR[count + 107];
                temp += dR[count + 103];
                temp += dR[count + 101];
                temp += dR[count + 97];
                temp += dR[count + 89];
                temp += dR[count + 83];
                temp /= 2;
                temp += dR[count + 79];
                temp += dR[count + 73];
                temp += dR[count + 71];
                temp += dR[count + 67];
                temp += dR[count + 61];
                temp += dR[count + 59];
                temp += dR[count + 53];
                temp += dR[count + 47];
                temp += dR[count + 43];
                temp += dR[count + 41];
                temp += dR[count + 37];
                temp += dR[count + 31];
                temp += dR[count + 29];
                temp /= 2;
                temp += dR[count + 23];
                temp += dR[count + 19];
                temp += dR[count + 17];
                temp += dR[count + 13];
                temp += dR[count + 11];
                temp /= 2;
                temp += dR[count + 7];
                temp += dR[count + 5];
                temp += dR[count + 3];
                temp /= 2;
                temp += dR[count + 2];
                temp += dR[count + 1];
                slowIIRBR += (temp / 128);
                inputSampleL = fastIIRBL - (slowIIRBL / slowTaper);
                inputSampleR = fastIIRBR - (slowIIRBR / slowTaper);
            }
            inputSampleL /= fastTaper;
            inputSampleR /= fastTaper;
            inputSampleL /= lowspeedscale;
            inputSampleR /= lowspeedscale;
            bridgerectifierL = fabs(inputSampleL);
            if (bridgerectifierL > 1.57079633) {
                bridgerectifierL = 1.57079633;
            }
            bridgerectifierL = sin(bridgerectifierL);
            // can use as an output limiter
            if (inputSampleL > 0.0) {
                inputSampleL = bridgerectifierL;
            }
            else {
                inputSampleL = -bridgerectifierL;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            bridgerectifierR = fabs(inputSampleR);
            if (bridgerectifierR > 1.57079633) {
                bridgerectifierR = 1.57079633;
            }
            bridgerectifierR = sin(bridgerectifierR);
            // can use as an output limiter
            if (inputSampleR > 0.0) {
                inputSampleR = bridgerectifierR;
            }
            else {
                inputSampleR = -bridgerectifierR;
            }
            // second stage of overdrive to prevent overs and allow bloody loud extremeness
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
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
} // namespace airwindohhs::ironoxideclassic
