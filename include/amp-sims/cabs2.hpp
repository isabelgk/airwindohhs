#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::cabs2 {

constexpr std::string_view k_name{ "Cabs2" };
constexpr std::string_view k_short_description{
    "Cabs2 uses really phasey filters to be a speaker cabinet simulator."
};
constexpr std::string_view k_long_description{
    "There's been many ways I've tried to do speaker sounds. Impulse responses, dynamic modifications of them, filtering, loudness factors… lots of things to try. This is the second Cabs plugin but not even the second time I've tried this.This is the first time I ended up wanting to put it in a Console plugin and build it into the mixing desk, for every channel, though.Cabs2 follows the line of experimentation PointyGuitar began. It started with PointyGuitar, delivering an interesting amplike tone entirely in the box. Set that up right and it's very Airwindows: a pungent, intense tone that's not produced by ANY of the normal means for doing such a thing. It all comes out of a very poorly behaved filter (AngleFilter, specifically) that gets stacked up to make a strange sort of hyper-saturation that repeatedly filters between gain stages using the same method I use on SmoothEQ2 and the recent PearEQ. PointyGuitar was that used on the most unhinged, raw filter code I could make. The same code was used in the speaker simulation for PointyGuitar.PointyDeluxe went even harder. Those filters can go crazy if adjacent bands are too different, and PointyGuitar actually has twice the filter bands you'd think it does: there are buffer bands between the ones you use, smoothing the behavior. So PointyDeluxe takes away everything else and just gives you the bands, even if the sound's going to go crazy, and it removes the speaker simulating and tries to do it all with the one algorithm. PointyDeluxe is a monster, and either the worst sounding junk you could imagine, or the best brutal industrial metal sound you could imagine. This is because, when you distort the way PointyDeluxe does, you're hypersaturating a certain frequency range but that brings out all the other frequencies as well, until the result is the most mix-filling, dense, full-range sound ever. Two guitars and there's no room for anything else, automatically.Cabs2 is another pivot. Yes, you can 100 percent run it after PointyDeluxe and have a speaker-cabinet style tone again. It will roll highs like a speaker if you set it just right, and reshape bass like a big cabinet which will fatten it up and move some more air, and both of these are why you'd use a cabinet simulator… though in fact Cabs2 really is not a speaker model at all. What it's doing is showing you how the phase gets really messy and nonlinear when you're running into a real live speaker. It does all that phase messiness just from what kind of filter it is. If you always made filters to be 'correct' you'd never stumble across it in a million years.But… what if you just started using it for everything?And that's where it started showing its stuff. You see, exactly modeling a speaker cab is complicated. It's not simply an EQ curve, sure, but it's also about as complicated as simulating a live room, only tiny. These things honk and resonate and buzz and the cones break up in bell mode resonances you don't even get in a room, and when they do they burn up energy in physical distortion that affects the length of the resonance and its character. This is why I've been thinking in terms of trying to design a mix speaker that's more NS-10-ey than NS-10s: if you accept that the breakup modes of the flat-sided cone are part of how that classic mix speaker 'gets out of the way' of the sound so quickly, you can imagine other ways to do that.Meanwhile, Cabs2 is just a bandpass (a lowpass up high, and a highpass down low). But it's a bandpass that screws up the phase in both places, always to steepen the cutoff at all costs… but without any of the resonances or other behaviors from a real speaker. It's like a purified cab sim without the cab: just the essence, through a weird algorithm that shall we say is not phase linear. A 'bad EQ' in the way a speaker is a 'bad EQ'.We can use the heck out of this.So, now you don't have to put PointyGuitar on non-guitar-related things to shape the sound in this way. When ConsoleX came out, it had (and has) lowpass and highpass filters based on my Isolator filter, which is purely a biquad-designed 'real' filter for isolating frequencies without totally screwing them up. It's not phase-linear or anything, but Isolator is still relatively 'nice'. Cabs2 is not. It's kind of messy, but in a good way. I think it's very likely that if you want to go for highpasses and lowpasses in mix, you might want to not only cut away the frequencies but also transition in that speakerlike way, the colorful way. And I think it'll work great for any situation in mix where you're trying to sit something in the mix and have it feel right.This is finding its way into ConsoleX2, but I'm doing all I can to invent ConsoleH first: let me know if this speaker-y textural thing is also what you'd want for a hiphop Console, try it on stuff. I've not yet decided whether the highpass/lowpass role in that should be this type of filter, or something more radical like a BezEQ concept where it's gritty and reeks of bad digital. I feel like I can get enough abrasion into ConsoleH in other ways and maybe I should use this to unleash totally different textures. Hope you like it!"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class Cabs2 final : public Effect<T>
{
    float A;
    float B;
    double iirHPositionL[23];
    double iirHAngleL[23];
    double iirHPositionR[23];
    double iirHAngleR[23];
    bool hBypass;
    double iirLPositionL[15];
    double iirLAngleL[15];
    double iirLPositionR[15];
    double iirLAngleR[15];
    bool lBypass;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Cabs2()
    {
        A = 1.0;
        B = 0.0;
        for (int count = 0; count < 22; count++) {
            iirHPositionL[count] = 0.0;
            iirHAngleL[count] = 0.0;
            iirHPositionR[count] = 0.0;
            iirHAngleR[count] = 0.0;
        }
        hBypass = false;
        for (int count = 0; count < 14; count++) {
            iirLPositionL[count] = 0.0;
            iirLAngleL[count] = 0.0;
            iirLPositionR[count] = 0.0;
            iirLAngleR[count] = 0.0;
        }
        lBypass = false;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;
            case kParamB: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "lowpass"; break;
            case kParamB: return "hipass"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Lowpass"; break;
            case kParamB: return "Hipass"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;

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
        double lFreq = pow(fmax(A, 0.002), overallscale); // the lowpass
        double hFreq = pow(B, overallscale + 2.0); // the highpass
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (hFreq > 0.0) {
                double lowSampleL = inputSampleL;
                double lowSampleR = inputSampleR;
                for (int count = 0; count < 21; count++) {
                    iirHAngleL[count] = (iirHAngleL[count] * (1.0 - hFreq)) + ((lowSampleL - iirHPositionL[count]) * hFreq);
                    lowSampleL = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleL * hFreq);
                    iirHPositionL[count] = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleL * hFreq);
                    inputSampleL -= (lowSampleL * (1.0 / 21.0)); // left
                    iirHAngleR[count] = (iirHAngleR[count] * (1.0 - hFreq)) + ((lowSampleR - iirHPositionR[count]) * hFreq);
                    lowSampleR = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleR * hFreq);
                    iirHPositionR[count] = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (lowSampleR * hFreq);
                    inputSampleR -= (lowSampleR * (1.0 / 21.0)); // right
                } // the highpass
                hBypass = false;
            }
            else {
                if (!hBypass) {
                    hBypass = true;
                    for (int count = 0; count < 22; count++) {
                        iirHPositionL[count] = 0.0;
                        iirHAngleL[count] = 0.0;
                        iirHPositionR[count] = 0.0;
                        iirHAngleR[count] = 0.0;
                    }
                } // blank out highpass if jut switched off
            }
            if (lFreq < 1.0) {
                for (int count = 0; count < 13; count++) {
                    iirLAngleL[count] = (iirLAngleL[count] * (1.0 - lFreq)) + ((inputSampleL - iirLPositionL[count]) * lFreq);
                    inputSampleL = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleL * lFreq);
                    iirLPositionL[count] = ((iirLPositionL[count] + (iirLAngleL[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleL * lFreq); // left
                    iirLAngleR[count] = (iirLAngleR[count] * (1.0 - lFreq)) + ((inputSampleR - iirLPositionR[count]) * lFreq);
                    inputSampleR = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleR * lFreq);
                    iirLPositionR[count] = ((iirLPositionR[count] + (iirLAngleR[count] * lFreq)) * (1.0 - lFreq)) + (inputSampleR * lFreq); // right
                } // the lowpass
                lBypass = false;
            }
            else {
                if (!lBypass) {
                    lBypass = true;
                    for (int count = 0; count < 14; count++) {
                        iirLPositionL[count] = 0.0;
                        iirLAngleL[count] = 0.0;
                        iirLPositionR[count] = 0.0;
                        iirLAngleR[count] = 0.0;
                    }
                } // blank out lowpass if just switched off
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
} // namespace airwindohhs::cabs2
