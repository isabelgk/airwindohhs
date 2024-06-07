#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::distortion {

constexpr std::string_view k_name{ "Distortion" };
constexpr std::string_view k_short_description{
    "Distortion is a slightly dark analog-style distortion with several presets, like Focus."
};
constexpr std::string_view k_long_description{
    "I think this one ought to go over well. People often want ‘analog-style distortion’. Here’s a collection of Airwindows distortion algorithms in a handy plugin with a new twist: it’s optimized for high sample rate in an innovative way.What does that mean? Distortion is a slightly ‘dark’ distortion plugin. Not only are the algorithms pretty smooth (with some exceptions: check the end of the video for one!), it uses sample averaging to stop extreme highs from getting through. At 44.1k, there’s an averaging stage before the distortion, darkening the output but also interfering with aliasing. Averaging also has some tonal benefits to midrange and bass, so this gives the plugin more of a ‘voice’. That’s at 44.1k, CD quality.But, if you use higher rates like 96k or 192k, Distortion begins to use more ‘poles’ of simple averaging. It’ll bring in averaging after the distortion, softening the tone. It’ll use more and more stages of this treble-softening, still always on just the most recent two samples, even though that becomes more and more finely grained as the sample rate goes up. Because of how averaging works, this never brings in weird notches or cancellations (like broader averages) but it does produce a sort of bleed-off of highs. Run at 44.1k this would give you an increasingly wide roll-off.At the high sample rates this was designed for, what you get instead is exactly the same ‘dark distortion’ tonality, but increasingly better aliasing rejection as the sample rate goes up. I think 96k is a sweet spot for this: at 96k, still a basically normal sample rate, you get extremely warm and fluid distortion, even when pushed to extremes.If you like extremes, look into the final two distortion modes. In Distortion, the modes Mojo and Dyno operate in their purest form, where the distortion shape can curl around again to produce strange effects if you slam them ruthlessly. In the video you see me discovering this on Mojo, running a simple DI bass at far too high a volume into Mojo, and then doubling it up to make crazy organic synth-like effects. Other modes like Density, Drive or Spiral will just go to ultra-clipping normally.Distortion will give you truly warm overdrive effects, especially at high sample rates, without a bunch of fake analog modeling. I don’t recommend using it on everything (because maybe some sounds are good WITH extended high-end, especially if you’re working at 96k and all) but as always, I’m not the boss of you.Again, the notable thing about this relative to other cascaded aliasing-suppressed distortions like Console7Cascade is that Distortion is doing it all with simple averaging filters. That will give you a distinct, softer sound without the clarity you get from multi-pole biquad rolloffs. Distortion is for when you want warm, warm, warm and are OK with letting the highs soften, plus if you want to pick different sorts of crunch edge (not often a feature on Airwindows plugins)."
};
constexpr std::string_view k_tags{
    "distortion"
};

template <typename T>
class Distortion final : public Effect<T>
{
    double previousInL[9];
    double previousOutL[9];
    double previousInR[9];
    double previousOutR[9];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    Distortion()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 1.0;
        for (int x = 0; x < 9; x++) {
            previousInL[x] = 0.0;
            previousOutL[x] = 0.0;
            previousInR[x] = 0.0;
            previousOutR[x] = 0.0;
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
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;
            case kParamB: return "mode"; break;
            case kParamC: return "output"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;
            case kParamB: return "Mode"; break;
            case kParamC: return "Output"; break;
            case kParamD: return "Dry/Wet"; break;

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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB"; break;
            case kParamB: return ""; break;
            case kParamC: return "dB"; break;
            case kParamD: return ""; break;

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

        int stages = (int)floor(Effect<T>::getSampleRate() / 25000.0);
        if (stages > 8) {
            stages = 8;
        }
        double input = pow(10.0, ((A - 0.5) * 24.0) / 20.0);
        int mode = (int)(B * 4.999);
        double output = pow(10.0, ((C - 0.5) * 24.0) / 20.0);
        double wet = D;
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
            inputSampleL *= input;
            inputSampleR *= input;
            for (int x = 0; x < stages; x++) {
                double temp;
                temp = (inputSampleL + previousInL[x]) * 0.5;
                previousInL[x] = inputSampleL;
                inputSampleL = temp;
                temp = (inputSampleR + previousInR[x]) * 0.5;
                previousInR[x] = inputSampleR;
                inputSampleR = temp;
            }
            switch (mode)
            {
                case 0: // Density
                    if (inputSampleL > 1.570796326794897) {
                        inputSampleL = 1.570796326794897;
                    }
                    if (inputSampleL < -1.570796326794897) {
                        inputSampleL = -1.570796326794897;
                    }
                    if (inputSampleR > 1.570796326794897) {
                        inputSampleR = 1.570796326794897;
                    }
                    if (inputSampleR < -1.570796326794897) {
                        inputSampleR = -1.570796326794897;
                    }
                    // clip to 1.570796326794897 to reach maximum output
                    inputSampleL = sin(inputSampleL);
                    inputSampleR = sin(inputSampleR);
                    break;
                case 1: // Drive
                    if (inputSampleL > 1.0) {
                        inputSampleL = 1.0;
                    }
                    if (inputSampleL < -1.0) {
                        inputSampleL = -1.0;
                    }
                    if (inputSampleR > 1.0) {
                        inputSampleR = 1.0;
                    }
                    if (inputSampleR < -1.0) {
                        inputSampleR = -1.0;
                    }
                    inputSampleL -= (inputSampleL * (fabs(inputSampleL) * 0.6) * (fabs(inputSampleL) * 0.6));
                    inputSampleR -= (inputSampleR * (fabs(inputSampleR) * 0.6) * (fabs(inputSampleR) * 0.6));
                    inputSampleL *= 1.5;
                    inputSampleR *= 1.5;
                    break;
                case 2: // Spiral
                    if (inputSampleL > 1.2533141373155) {
                        inputSampleL = 1.2533141373155;
                    }
                    if (inputSampleL < -1.2533141373155) {
                        inputSampleL = -1.2533141373155;
                    }
                    if (inputSampleR > 1.2533141373155) {
                        inputSampleR = 1.2533141373155;
                    }
                    if (inputSampleR < -1.2533141373155) {
                        inputSampleR = -1.2533141373155;
                    }
                    // clip to 1.2533141373155 to reach maximum output
                    inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
                    inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
                    break;
                case 3: // Mojo
                    double mojo;
                    mojo = pow(fabs(inputSampleL), 0.25);
                    if (mojo > 0.0) {
                        inputSampleL = (sin(inputSampleL * mojo * M_PI * 0.5) / mojo) * 0.987654321;
                    }
                    mojo = pow(fabs(inputSampleR), 0.25);
                    if (mojo > 0.0) {
                        inputSampleR = (sin(inputSampleR * mojo * M_PI * 0.5) / mojo) * 0.987654321;
                    }
                    // mojo is the one that flattens WAAAAY out very softly before wavefolding
                    break;
                case 4: // Dyno
                    double dyno;
                    dyno = pow(fabs(inputSampleL), 4);
                    if (dyno > 0.0) {
                        inputSampleL = (sin(inputSampleL * dyno) / dyno) * 1.1654321;
                    }
                    dyno = pow(fabs(inputSampleR), 4);
                    if (dyno > 0.0) {
                        inputSampleR = (sin(inputSampleR * dyno) / dyno) * 1.1654321;
                    }
                    // dyno is the one that tries to raise peak energy
                    break;
            }
            for (int x = 1; x < (stages / 2); x++) {
                double temp;
                temp = (inputSampleL + previousOutL[x]) * 0.5;
                previousOutL[x] = inputSampleL;
                inputSampleL = temp;
                temp = (inputSampleR + previousOutR[x]) * 0.5;
                previousOutR[x] = inputSampleR;
                inputSampleR = temp;
            }
            if (output != 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
            }
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
} // namespace airwindohhs::distortion
