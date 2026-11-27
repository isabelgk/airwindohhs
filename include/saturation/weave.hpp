#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::weave {

constexpr std::string_view k_name{ "Weave" };
constexpr std::string_view k_short_description{
    "Weave is a fancy overdrive with a good texture."
};
constexpr std::string_view k_long_description{
    "There's been a lineage of Airwindows custom overdrives. The most basic overdrive is based on the sin() function, and that'll get you the softest, fattest distortion where harmonics are added one at a time, not all at once as with hard clipping. There's more recent experiments like Longhand, designed to do exactly what sin() does but with no possible optimizations, instead plotting one continuing equation around the sine curve. Want more? Spiral was based on the sine, but with a twist, gain staging the saturation based on how loud the signal was. This isn't compression, because it's not using a time factor and just uses the input sample directly. Spiral gives you a leaner body to the sound, its saturation curve steepening as it gets closer to complete distortion. More of a bark to it, edgier, but still one continuous equation. Weave takes one further step, another layer of fancy. Where Spiral scales things by the input, attenuating way down for quiet sounds so there's next to no clipping and then putting the gain back post saturation, Weave steps back in the direction of thick and fat in a novel way: taking the square root of the control signal. What this does is put that 'pre-saturation gain' part back towards sin() again, softening the overdrive up, but the very center is still cleaned up and leaned out. It produces a full-sounding overdrive that isn't tubby and overfat like straignt sin() can be, and the saturation curve is still one unbroken algorithm, but the lean part is more towards the center and the quiet stuff, much like I did in TapeHack. Then, to top it off, Weave uses the derivative trick I came up with to suppress high frequencies near Nyquist, the one that isn't elliptic BLEP aliasing removal, but instead just sort of interacts with the frequencies and alters them. What I found designing this was, the technique stopped brittle highs from coming forward under high gain distortion. I've been using it for things and am happy with the sound. All this combines into Weave to produce a saturation with a lot of character and flavor. You can run great tones into it and not diminish their quality, even though the harmonics come right out and make it obvious saturation is present. It doesn't get bloated, but it holds onto whatever thickness your sound has. It doesn't get bright or gritty, but it hangs onto detail very very well. Weave became the default saturation in my video-making vocal chain. It became the saturation in ConsoleX3, beating out TapeHack for that purpose. And it's available standalone now, in the 'X filters' configuration where if the input drive is at 0.1 that's unity gain: this gives you ten times the gain if you want it, upwards of 18dB boost, and Weave can stand up to that pretty much no matter what kind of sound you have. Hope you like Weave :)"
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Weave final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'weav';
    float A;
    float B;
    float C;
    enum
    {
        bezierInput_A,
        bezierInput_B,
        bezierInput_C,
        bezierInput_Out,
        bezierOutput_A,
        bezierOutput_B,
        bezierOutput_C,
        bezierOutput_Out,
        bezier_cycle,
        bezier_total
    }; // control smoothing as basic use of bezier curves
    double bezier[bezier_total];
    enum
    {
        bip_dvLA,
        bip_dvLB,
        bip_dvLC,
        bip_dvLD,
        bip_pvLA,
        bip_pvLB,
        bip_pvLC,
        bip_pvLD,
        bip_dvRA,
        bip_dvRB,
        bip_dvRC,
        bip_dvRD,
        bip_pvRA,
        bip_pvRB,
        bip_pvRC,
        bip_pvRD,
        bip_total // each distortion section can have one of these, it stacks well
    }; // not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
    double bip[bip_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Weave()
    {
        A = 0.1;
        B = 0.5;
        C = 1.0;
        for (int x = 0; x < bezier_total; x++) {
            bezier[x] = 0.0;
        }
        for (int x = 0; x < bip_total; x++) {
            bip[x] = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.1; break;
            case kParamB: return 0.5; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "smooth"; break;
            case kParamC: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Smooth"; break;
            case kParamC: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(A, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(B, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(C, text, kVstMaxParamStrLen); break;
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
        double input = A;
        double bezierRez = fmax(pow((1.0 - B) * 0.25, 3.0) / overallscale, 0.00001);
        int stepped = 999999;
        if (bezierRez > 0.000001) {
            stepped = (int)(1.0 / bezierRez);
        }
        bezierRez = 0.99999999 / stepped;
        double bezierTrim = 1.0 - (bezierRez * ((double)stepped / (stepped + 1.0)));
        // manages the overall Bezier control smoothing system plugin-wide and feed all controls
        // into bezier[] as just 0-1 values, unprocessed. do it IN the control smoothing engine
        double output = C;
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
                bezier[bezierInput_C] = bezier[bezierInput_B];
                bezier[bezierInput_B] = bezier[bezierInput_A];
                bezier[bezierOutput_C] = bezier[bezierOutput_B];
                bezier[bezierOutput_B] = bezier[bezierOutput_A];
                // one of these bucket brigade lines for every smoothed control
                // begin expensive control calculations
                bezier[bezierInput_A] = input * 10.0;
                bezier[bezierOutput_A] = output;
                // end expensive control calculations
            }
            double lerp = bezier[bezier_cycle];
            bezier[bezierInput_Out] = (bezier[bezierInput_B] + (bezier[bezierInput_C] * (1.0 - lerp) * (1.0 - lerp)) + (bezier[bezierInput_B] * 2.0 * (1.0 - lerp) * lerp) + (bezier[bezierInput_A] * lerp * lerp)) * 0.5;
            bezier[bezierOutput_Out] = (bezier[bezierOutput_B] + (bezier[bezierOutput_C] * (1.0 - lerp) * (1.0 - lerp)) + (bezier[bezierOutput_B] * 2.0 * (1.0 - lerp) * lerp) + (bezier[bezierOutput_A] * lerp * lerp)) * 0.5;
            // end Bezier control smoothing engine
            inputSampleL *= bezier[bezierInput_Out];
            double bip_delta = inputSampleL; // delta can be just local and re-used
            if (inputSampleL != 0.0) {
                inputSampleL = fmax(fmin(inputSampleL, 1.0), -1.0);
                inputSampleL *= 1.27;
                long double weave = sqrt(fabs(inputSampleL));
                long double spiral = inputSampleL * weave;
                long double X = spiral;
                X *= X;
                long double temp = spiral * X;
                spiral -= temp * 0.16666666666666666666666666666666666;
                temp *= X; //-8 dB
                spiral += temp * 0.00833333333333333333333333333333333;
                temp *= X; //-20 dB
                spiral -= temp * 0.00019841269841269841269841269841269;
                temp *= X; //-37 dB
                spiral += temp * 0.00000275573192239858906525573192239;
                temp *= X; //-55 dB
                spiral -= temp * 0.00000002505210838544171877521083854;
                temp *= X; //-76 dB
                spiral += temp * 0.00000000016059043836821614599392377;
                temp *= X; //-108 dB
                spiral -= temp * 0.00000000000076471637318198164759011;
                temp *= X; //-121 dB
                spiral += temp * 0.00000000000000281145725434552076319;
                temp *= X; //-146 dB
                spiral -= temp * 0.00000000000000000822063524662432971;
                temp *= X; //-171 dB
                spiral += temp * 0.00000000000000000001957294106339126; //-197 dB
                inputSampleL = spiral / weave; // purest form is full Spiral
            }
            bip[bip_dvLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
            bip[bip_dvLB] = bip[bip_pvLA] - bip[bip_dvLA];
            bip[bip_pvLA] = bip[bip_dvLA]; // velocity
            bip[bip_dvLC] = bip[bip_pvLB] - bip[bip_dvLB];
            bip[bip_pvLB] = bip[bip_dvLB]; // acceleration
            bip[bip_dvLD] = bip[bip_pvLC] - bip[bip_dvLC];
            bip[bip_pvLC] = bip[bip_dvLC]; // jerk
            double bip_dvE = bip[bip_pvLD] - bip[bip_dvLD];
            bip[bip_pvLD] = bip[bip_dvLD]; // snap
            inputSampleL *= (1.0 + (fabs(bip[bip_dvLC]) * 0.0618) + (fabs(bip[bip_dvLD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
            inputSampleL *= bezier[bezierOutput_Out];
            inputSampleR *= bezier[bezierInput_Out];
            bip_delta = inputSampleR; // delta can be just local and re-used
            if (inputSampleR != 0.0) {
                inputSampleR = fmax(fmin(inputSampleR, 1.0), -1.0);
                inputSampleR *= 1.27;
                long double weave = sqrt(fabs(inputSampleR));
                long double spiral = inputSampleR * weave;
                long double X = spiral;
                X *= X;
                long double temp = spiral * X;
                spiral -= temp * 0.16666666666666666666666666666666666;
                temp *= X; //-8 dB
                spiral += temp * 0.00833333333333333333333333333333333;
                temp *= X; //-20 dB
                spiral -= temp * 0.00019841269841269841269841269841269;
                temp *= X; //-37 dB
                spiral += temp * 0.00000275573192239858906525573192239;
                temp *= X; //-55 dB
                spiral -= temp * 0.00000002505210838544171877521083854;
                temp *= X; //-76 dB
                spiral += temp * 0.00000000016059043836821614599392377;
                temp *= X; //-108 dB
                spiral -= temp * 0.00000000000076471637318198164759011;
                temp *= X; //-121 dB
                spiral += temp * 0.00000000000000281145725434552076319;
                temp *= X; //-146 dB
                spiral -= temp * 0.00000000000000000822063524662432971;
                temp *= X; //-171 dB
                spiral += temp * 0.00000000000000000001957294106339126; //-197 dB
                inputSampleR = spiral / weave; // purest form is full Spiral
            }
            bip[bip_dvRA] = bip_delta - inputSampleR; // these are derivatives: raw clip is position
            bip[bip_dvRB] = bip[bip_pvRA] - bip[bip_dvRA];
            bip[bip_pvRA] = bip[bip_dvRA]; // velocity
            bip[bip_dvRC] = bip[bip_pvRB] - bip[bip_dvRB];
            bip[bip_pvRB] = bip[bip_dvRB]; // acceleration
            bip[bip_dvRD] = bip[bip_pvRC] - bip[bip_dvRC];
            bip[bip_pvRC] = bip[bip_dvRC]; // jerk
            bip_dvE = bip[bip_pvRD] - bip[bip_dvRD];
            bip[bip_pvRD] = bip[bip_dvRD]; // snap
            inputSampleR *= (1.0 + (fabs(bip[bip_dvRC]) * 0.0618) + (fabs(bip[bip_dvRD]) * -0.05982) + (fabs(bip_dvE) * 0.0206));
            inputSampleR *= bezier[bezierOutput_Out];
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
} // namespace airwindohhs::weave
