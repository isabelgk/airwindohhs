#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::purestconsole4channel {

constexpr std::string_view k_name{ "PurestConsole4Channel" };
constexpr std::string_view k_short_description{
    ""
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class PurestConsole4Channel final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'pc4c';
    float SMO;
    float PAN;
    float FAD;
    enum
    {
        bezierGainL_A,
        bezierGainL_B,
        bezierGainL_C,
        bezierGainL_Out,
        bezierGainR_A,
        bezierGainR_B,
        bezierGainR_C,
        bezierGainR_Out,
        bezier_cycle,
        bezier_total
    }; // control smoothing as basic use of bezier curves
    double bezier[bezier_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PurestConsole4Channel()
    {
        SMO = 0.5;
        PAN = 0.5;
        FAD = 1.0;
        for (int x = 0; x < bezier_total; x++) {
            bezier[x] = 0.0;
        }
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
        switch (index) {
            case kParamA: SMO = value; break;
            case kParamB: PAN = value; break;
            case kParamC: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return SMO; break;
            case kParamB: return PAN; break;
            case kParamC: return FAD; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "smooth"; break;
            case kParamB: return "pan"; break;
            case kParamC: return "fader"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Smooth"; break;
            case kParamB: return "Pan"; break;
            case kParamC: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(SMO, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(PAN, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(FAD, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
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
        double bezierRez = fmax(pow((1.0 - SMO) * 0.25, 3.0) / overallscale, 0.00001);
        int stepped = 999999;
        if (bezierRez > 0.000001) {
            stepped = (int)(1.0 / bezierRez);
        }
        bezierRez = 0.99999999 / stepped;
        double bezierTrim = 1.0 - (bezierRez * ((double)stepped / (stepped + 1.0)));
        // manages the overall Bezier control smoothing system plugin-wide and feed all controls
        // into bezier[] as just 0-1 values, unprocessed. do it IN the control smoothing engine
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // begin Bezier control smoothing engine
            bezier[bezier_cycle] += bezierRez;
            if (bezier[bezier_cycle] > bezierTrim) {
                bezier[bezier_cycle] = 0.0;
                bezier[bezierGainL_C] = bezier[bezierGainL_B];
                bezier[bezierGainL_B] = bezier[bezierGainL_A];
                bezier[bezierGainR_C] = bezier[bezierGainR_B];
                bezier[bezierGainR_B] = bezier[bezierGainR_A];
                // one of these bucket brigade lines for every smoothed control
                // begin expensive control calculations
                double gain = pow(FAD, 2.0) * 1.414213562373094; // Pan will pad this
                bezier[bezierGainL_A] = gain * sin(M_PI_2 - (PAN * M_PI_2));
                bezier[bezierGainR_A] = gain * sin(PAN * M_PI_2);
                // end expensive control calculations
            }
            double lerp = bezier[bezier_cycle];
            bezier[bezierGainL_Out] = bezier[bezierGainL_B] + (bezier[bezierGainL_C] * (1.0 - lerp) * (1.0 - lerp)) + (bezier[bezierGainL_B] * 2.0 * (1.0 - lerp) * lerp) + (bezier[bezierGainL_A] * lerp * lerp);
            bezier[bezierGainR_Out] = bezier[bezierGainR_B] + (bezier[bezierGainR_C] * (1.0 - lerp) * (1.0 - lerp)) + (bezier[bezierGainR_B] * 2.0 * (1.0 - lerp) * lerp) + (bezier[bezierGainR_A] * lerp * lerp);
            // end Bezier control smoothing engine
            inputSampleL *= bezier[bezierGainL_Out];
            inputSampleR *= bezier[bezierGainR_Out];
            inputSampleL = fmax(fmin(inputSampleL, M_PI_2), -M_PI_2);
            inputSampleR = fmax(fmin(inputSampleR, M_PI_2), -M_PI_2);
            long double X = inputSampleL;
            X *= X; // long double for even
            long double temp = inputSampleL * X; // the initial multiplies
            inputSampleL -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            inputSampleL += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            inputSampleL -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            inputSampleL += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            inputSampleL -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            inputSampleL += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            inputSampleL -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            inputSampleL += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            inputSampleL -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            inputSampleL += temp * 0.00000000000000000001957294106339126;
            X = inputSampleR;
            X *= X; // long double for even
            temp = inputSampleR * X; // the initial multiplies
            inputSampleR -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            inputSampleR += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            inputSampleR -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            inputSampleR += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            inputSampleR -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            inputSampleR += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            inputSampleR -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            inputSampleR += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            inputSampleR -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            inputSampleR += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::purestconsole4channel
