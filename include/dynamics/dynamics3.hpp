#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::dynamics3 {

constexpr std::string_view k_name{ "Dynamics3" };
constexpr std::string_view k_short_description{
    "Dynamics3 morphs between vari-mu and expander!"
};
constexpr std::string_view k_long_description{
    "This is an essay in finding what something's meant to be, and then running like mad to expand it.What was once a Bezier-spline compressor with extra release speed if you needed it, turned into that, further tweaked, but with vari-mu: it modulates dry/wet now, not just amplitude. Does that make it a Fairchild? Absolutely not, in fact I didn't even reference Fairchild sounds in developing it. The way that was dialed in was to get the sound, not to get 'that' sound. Yes, vari-mu references a vintage beast, even an expensive one, but it's also a simple description of what happenes to ratio (mu) and it brought so much to the behavior of Dynamics3 that it stands on its own.And yes, ratio can be adjusted through the dry/wet, meaning this version of Dynamics can naturally take on 2-buss duties without breaking a sweat… but it's not a dry/wet, it's an inv/wet. Normally, that has a specific meaning: you can apply the plugin, or you can subtract it to produce the opposite effect. Does this subtract the compression, hence the 'invert' part of the control?No. What the inv/wet does, is let you do the opposite of compression, adjusted to get as close as possible to what the result SHOULD be if you got the perfect expanded sound as a complement to the compressed one.That's phrased a bit awkwardly, I'll try again. If you have a compressed sound, and the start of transient attacks is a little dynamic 'pop' before the compression hits, the 'inv' range gives you an expansion, where the start of the transient is suppressed and then the volume flares up after that.How much? It's sensitive to threshold, and to release speed. Back off on either one, and the expansion will back off too. Or if you ramp it up, it'll go into distortion but that's better than the kind of spikes you'd otherwise get. If threshold is almost, but not quite, 1.0 (pure 1.0 is also a bypass to the plugin) then full inv (expansion) leaves everything much quieter, because it's expanding down from that point. It doesn't go to complete silence because it's not a gate. If you speed up the release time, this also helps it not go to crazy expansion boosts, just like if you're heavily compressed, faster release time will help it not be squished into silence.The thing is, though, it's almost not worth trying to put it into words. The whole Dynamics3 experience is that, if you have a sense of compression, you should be able to dial that in effortlessly and immediately. Attack to get the speed of attack, release for how fast it springs back, threshold down to where it does what you want, ratio (dry/wet) for subtlety, boom. If you don't have a sense of compression, this plugin will teach you and make it seem easy and discoverable, as it should be.Then, with one slide from compression to expansion, it can broaden the impact of transients until they're dense and solid, or it can flare up the body behind a thin percussive attack, or pretend it's a gated reverb effect, or de-ambience a room sound, or all those things combined, to taste, with exactly the same controls (under the hood, it's a whole different algorithm, but it's meant to feel like an extension of the same thing).…as it should be.Dynamics3 has been getting revision after revision, to get it to this place: coming out as the first glimpse of what's going to be an amazing new ConsoleX3, where all the parts are this good and work this well together. If you've ever liked anything about any of the Bezier spline compressors… this'll probably be your new favorite, as it is mine. All the others are of course still there, it's just that I can't imagine you needing them now :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Dynamics3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    enum
    {
        bez_A,
        bez_B,
        bez_C,
        bez_Ctrl,
        bez_cycle,
        bez_min,
        bez_comp,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezComp[bez_total];
    // Dynamics3
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Dynamics3()
    {
        A = 1.0;
        B = 0.382;
        C = 0.618;
        D = 1.0;
        for (int x = 0; x < bez_total; x++) {
            bezComp[x] = 0.0;
        }
        // Dynamics3
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;
            case kParamB: return 0.382; break;
            case kParamC: return 0.618; break;
            case kParamD: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "thresh"; break;
            case kParamB: return "attack"; break;
            case kParamC: return "release"; break;
            case kParamD: return "inv/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Thresh"; break;
            case kParamB: return "Attack"; break;
            case kParamC: return "Release"; break;
            case kParamD: return "Inv/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
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
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
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

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        // begin Dynamics3
        double bezThresh = pow(A + 0.6180339887498949, 2.0) * 1.6180339887498949;
        double sqrThresh = sqrt(bezThresh);
        double bezRez = fmax(pow((1.0 - B) * 0.4, 4.0) / overallscale, 0.0001);
        bezRez /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
        int stepped = 999999;
        if (bezRez > 0.000001) {
            stepped = (int)(1.0 / bezRez);
        }
        bezRez = 0.99999999 / stepped;
        double bezTrim = 1.0 - (bezRez * ((double)stepped / (stepped + 1.0)));
        double sloRez = fmax(pow((1.0 - C) * 0.4, 4.0) / overallscale, 0.00001);
        sloRez /= (2.0 / pow(overallscale, 0.5 - ((overallscale - 1.0) * 0.0375)));
        double invDry = pow(1.0 - (fabs(D - 0.5) * 2.0), 1.6180339887498949);
        bool compress = (D > 0.499999);
        bool compBypass = (bezThresh > 4.236);
        // end Dynamics3
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (!compBypass) {
                // begin Dynamics3
                double dryCompL = inputSampleL;
                double dryCompR = inputSampleR;
                if (compress) {
                    inputSampleL *= (bezComp[bez_comp] / bezThresh);
                    inputSampleR *= (bezComp[bez_comp] / bezThresh);
                }
                else {
                    inputSampleL /= bezThresh;
                    inputSampleR /= bezThresh;
                }
                double ctrl = fmin(fmax(fabs(inputSampleL), fabs(inputSampleR)), sqrThresh * bezComp[bez_comp] * 0.6180339887498949);
                bezComp[bez_min] = fmax(bezComp[bez_min] - sloRez, ctrl);
                bezComp[bez_Ctrl] += (bezComp[bez_min] * bezRez);
                bezComp[bez_cycle] += bezRez;
                if (bezComp[bez_cycle] > bezTrim) {
                    bezComp[bez_cycle] = 0.0;
                    bezComp[bez_C] = bezComp[bez_B];
                    bezComp[bez_B] = bezComp[bez_A];
                    bezComp[bez_A] = bezComp[bez_Ctrl];
                    bezComp[bez_Ctrl] = 0.0;
                }
                double X = bezComp[bez_cycle];
                bezComp[bez_comp] = bezComp[bez_B] + (bezComp[bez_C] * (1.0 - X) * (1.0 - X)) + (bezComp[bez_B] * 2.0 * (1.0 - X) * X) + (bezComp[bez_A] * X * X);
                bezComp[bez_comp] = ((1.0 - (fmin(bezComp[bez_comp], 0.9999))));
                if (compress) {
                    inputSampleL = inputSampleL * (1.0 - invDry) * bezComp[bez_comp] * bezThresh;
                    inputSampleL = fmax(fmin(inputSampleL, 2.305929007734908), -2.305929007734908);
                    double addtwo = inputSampleL * inputSampleL;
                    double empower = inputSampleL * addtwo; // inputSampleL to the third power
                    inputSampleL -= (empower / 6.0);
                    empower *= addtwo; // to the fifth power
                    inputSampleL += (empower / 69.0);
                    empower *= addtwo; // seventh
                    inputSampleL -= (empower / 2530.08);
                    empower *= addtwo; // ninth
                    inputSampleL += (empower / 224985.6);
                    empower *= addtwo; // eleventh
                    inputSampleL -= (empower / 9979200.0f);
                    // this is a degenerate form of a Taylor Series to approximate sin()
                    inputSampleL += dryCompL * invDry * (1.0 - (bezComp[bez_comp] * (1.0 - invDry)));
                    inputSampleR = inputSampleR * (1.0 - invDry) * bezComp[bez_comp] * bezThresh;
                    inputSampleR = fmax(fmin(inputSampleR, 2.305929007734908), -2.305929007734908);
                    addtwo = inputSampleR * inputSampleR;
                    empower = inputSampleR * addtwo; // inputSampleR to the third power
                    inputSampleR -= (empower / 6.0);
                    empower *= addtwo; // to the fifth power
                    inputSampleR += (empower / 69.0);
                    empower *= addtwo; // seventh
                    inputSampleR -= (empower / 2530.08);
                    empower *= addtwo; // ninth
                    inputSampleR += (empower / 224985.6);
                    empower *= addtwo; // eleventh
                    inputSampleR -= (empower / 9979200.0f);
                    // this is a degenerate form of a Taylor Series to approximate sin()
                    inputSampleR += dryCompR * invDry * (1.0 - (bezComp[bez_comp] * (1.0 - invDry)));
                }
                else {
                    inputSampleL = ((inputSampleL / (0.1 + bezThresh)) * (1.0 - invDry)) / bezComp[bez_comp];
                    inputSampleL = fmax(fmin(inputSampleL, 2.305929007734908), -2.305929007734908);
                    double addtwo = inputSampleL * inputSampleL;
                    double empower = inputSampleL * addtwo; // inputSampleL to the third power
                    inputSampleL -= (empower / 6.0);
                    empower *= addtwo; // to the fifth power
                    inputSampleL += (empower / 69.0);
                    empower *= addtwo; // seventh
                    inputSampleL -= (empower / 2530.08);
                    empower *= addtwo; // ninth
                    inputSampleL += (empower / 224985.6);
                    empower *= addtwo; // eleventh
                    inputSampleL -= (empower / 9979200.0f);
                    // this is a degenerate form of a Taylor Series to approximate sin()
                    inputSampleL += dryCompL * invDry * (1.0 - (bezComp[bez_comp] * (1.0 - invDry)));
                    inputSampleR = ((inputSampleR / (0.1 + bezThresh)) * (1.0 - invDry)) / bezComp[bez_comp];
                    inputSampleR = fmax(fmin(inputSampleR, 2.305929007734908), -2.305929007734908);
                    addtwo = inputSampleR * inputSampleR;
                    empower = inputSampleR * addtwo; // inputSampleR to the third power
                    inputSampleR -= (empower / 6.0);
                    empower *= addtwo; // to the fifth power
                    inputSampleR += (empower / 69.0);
                    empower *= addtwo; // seventh
                    inputSampleR -= (empower / 2530.08);
                    empower *= addtwo; // ninth
                    inputSampleR += (empower / 224985.6);
                    empower *= addtwo; // eleventh
                    inputSampleR -= (empower / 9979200.0f);
                    // this is a degenerate form of a Taylor Series to approximate sin()
                    inputSampleR += dryCompR * invDry * (1.0 - (bezComp[bez_comp] * (1.0 - invDry)));
                }
            } // end Dynamics3
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
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
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
} // namespace airwindohhs::dynamics3
