#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tapehack2 {

constexpr std::string_view k_name{ "TapeHack2" };
constexpr std::string_view k_short_description{
    "TapeHack2 brings Airwindows tape to a new level."
};
constexpr std::string_view k_long_description{
    "Two wrongs may not make a right… but sometimes, two goofy hacks might make a better 'tape compression' than having a real tape machine.So hear me out, because this one went some strange places, and, well, listen to it ;)The original TapeHack was a pretty big deal, went over real well, in spite of the fact that there was nothing to it but a software 'transfer function'. It's literally nothing but a 'vaguely sinelike softclip' with special characteristics. Everything about it was made to shift the 'saturation' upwards in dB and lean out the middle part a bit, in accordance to how real tape turns out to work. I'm of course not alone in figuring this stuff out, I'm just the one trying to doctor softclip algorithms to do it. That part worked great, and because it's a softclip, it even sounds a lot like 'tape compression', certainly more than anything I'd had before.There's tests you can do to determine whether your tape biasing and the amount of your ultrasonic energy is high enough to saturate things to produce real-deal, genuine 'tape compression' of highs. And it really is true that you don't have to use literal tape machines to do that anymore. I would go so far to say that is the new bar to reach: nobody's going to duplicate that short of simulating the whole apparatus at a megahertz or something, and this I am not going to do: I already didn't like oversampling, and it seems wasteful.But what if, on top of faking the 'bias tone bridging the gap between magnetic coercivity' through just doctoring a softclip to do it (wrong, but it worked), I instead just came up with a new sort of filter that was very free of typical sharp-filter irregularities… and modulated that at audio rates, in an attempt to not let the signal do high frequencies beyond too much of a slew rate?Meet TapeHack2. If you've downloaded ConsoleH or ConsoleX2 in the last day or two, you have already met it, because it worked so well I had to include it in the 'More' fat-boost section of those plugins. My video for TapeHack2 demonstrates why, showing the original mix, the subtly but intensely better mix where nothing has been changed but swapping in the TapeHack2 algorithm for the original, and lastly the delta function showing HOW much aliasing and untapelike brightness was fluidly removed from just the parts of the mix hitting More hard. It was so strong it affected the buss compression enough to bring in totally unrelated elements like bass, just by altering how More handled supersonic highs in the 96k mix.Functionally, this is the 'tape compression of highs' effect. Is it a clone, or emulation? AW HELL no, not even slightly. It's totally different, measurably, observably different, not like a tape at all, but now it does two tape behaviors as hard as it can. Run sine sweeps into it if you want to see how different it is, how crazy it'll act. It's a HACK, doesn't have anything to do with analog anything really.So what is happening?It's running a stack of filters. They're very efficient: they're stacked-up averaging filters. These are very good in the time domain, don't do pre-rings or weird filter ripples, but the cost is if you just use a simple averaging of a bunch of samples in a row, you get massive notches and odd peaks in the response. They are awful at being accurate good filters.But, turns out if you stack them up right you can have the notch of one overlap the peak of another, and get a really steep roll-off with good time behavior where the rolloff is so steep you hardly mind the funny added peaks and response issues. It basically goes 'ok, very smooth natural super rolloff NOW' and then rather than needing to work over an enormous window, and have a huge filter ripple (for linear phase) it just does the thing and gets out of the way. You trade off the accuracy of the 'curve' for just 'make the highs be gone, completely!'. I've got a filter plugin coming, Slew4, that's an expanded version of this.That's not 'tape compression', though. Tape compression is when you let the raw signal through, but track how sharp a departure the new sound is from whatever the filtering was doing last sample. Instead of just tracking the slew of the input signal and working with that, you barely touch the sound unless the new sample is gonna be really far from where the last output was, even if that's the result of filtering… and that is why it acts like compressing, there's a point beyond which highs can't get louder and you can't force them. It's not even on a time constant, it's just 'nope'.And is this the point at which the filtering prevents aliasing from happening? NOPE. It absolutely still aliases, and I'll tell you why. In setting it up, I got this part working and then tried to find the point at which I prevented all aliasing. It didn't want to. It's not an oversampling: it's still raw, still running fully at whatever sample rate you're using, still producing 'semi-valid' information including above where the apparent tape compression kicks in.It's just that the way this thing works, when set up to sound its best, it's not reducing superhigh sine waves to silence above a certain frequency. It's changing them into quieter triangle waves instead. Along the way, if you're doing intense saturation and making the wave flat-top, it turns it into sort of trapezoids in the manner that big square waves on analog gear turn into slanted-topped, altered waves. So basically what I'm saying is it does the weirdest waveshaping I've ever seen, with all the very highest frequency stuff, in such a way that it's arguably sharpening the frequencies while also attenuating and suppressing them. And it's all off a filter (set of filters) that is very free of ripple and resonant qualities.This, applied to real music, sounds pretty amazing, and it's going to find its way into a new ToTape, as well as my trusty TapeDelay plugin, and it is already in ConsoleH and ConsoleX2. And honest, it's the farthest thing from emulating anything, much less real tape. But my interest is never in trying to duplicate a thing: digital is not good at that, something dies when you overprocess. Instead, TapeHack2 is about bringing very minimalist, very unusual sorts of processing to achieve a result, which in this case is 'softclip like a tape, but ALSO stop highs from getting louder, also like a tape, except you're not remotely a tape'.Have fun with this one, I know I'm going to be enjoying the results of it a whole lot :)"
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class TapeHack2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    double avg32L[33];
    double avg32R[33];
    double avg16L[17];
    double avg16R[17];
    double avg8L[9];
    double avg8R[9];
    double avg4L[5];
    double avg4R[5];
    double avg2L[3];
    double avg2R[3];
    double post32L[33];
    double post32R[33];
    double post16L[17];
    double post16R[17];
    double post8L[9];
    double post8R[9];
    double post4L[5];
    double post4R[5];
    double post2L[3];
    double post2R[3];
    double lastDarkL;
    double lastDarkR;
    int avgPos;
    // preTapeHack
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    TapeHack2()
    {
        A = 0.1;
        B = 1.0;
        C = 1.0;
        for (int x = 0; x < 33; x++) {
            avg32L[x] = 0.0;
            post32L[x] = 0.0;
            avg32R[x] = 0.0;
            post32R[x] = 0.0;
        }
        for (int x = 0; x < 17; x++) {
            avg16L[x] = 0.0;
            post16L[x] = 0.0;
            avg16R[x] = 0.0;
            post16R[x] = 0.0;
        }
        for (int x = 0; x < 9; x++) {
            avg8L[x] = 0.0;
            post8L[x] = 0.0;
            avg8R[x] = 0.0;
            post8R[x] = 0.0;
        }
        for (int x = 0; x < 5; x++) {
            avg4L[x] = 0.0;
            post4L[x] = 0.0;
            avg4R[x] = 0.0;
            post4R[x] = 0.0;
        }
        for (int x = 0; x < 3; x++) {
            avg2L[x] = 0.0;
            post2L[x] = 0.0;
            avg2R[x] = 0.0;
            post2R[x] = 0.0;
        }
        avgPos = 0;
        lastDarkL = 0.0;
        lastDarkR = 0.0;
        // preTapeHack
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
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
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "output"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Output"; break;
            case kParamC: return "Dry/Wet"; break;

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
        int spacing = floor(overallscale * 2.0);
        if (spacing < 2) {
            spacing = 2;
        }
        if (spacing > 32) {
            spacing = 32;
        }
        double inputGain = A * 10.0;
        double outputGain = B * 0.9239;
        double wet = C;
        while (--sampleFrames >= 0) {
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
            inputSampleL *= inputGain;
            inputSampleR *= inputGain;
            double darkSampleL = inputSampleL;
            double darkSampleR = inputSampleR;
            if (avgPos > 31) {
                avgPos = 0;
            }
            if (spacing > 31) {
                avg32L[avgPos] = darkSampleL;
                avg32R[avgPos] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {
                    darkSampleL += avg32L[x];
                    darkSampleR += avg32R[x];
                }
                darkSampleL /= 32.0;
                darkSampleR /= 32.0;
            }
            if (spacing > 15) {
                avg16L[avgPos % 16] = darkSampleL;
                avg16R[avgPos % 16] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {
                    darkSampleL += avg16L[x];
                    darkSampleR += avg16R[x];
                }
                darkSampleL /= 16.0;
                darkSampleR /= 16.0;
            }
            if (spacing > 7) {
                avg8L[avgPos % 8] = darkSampleL;
                avg8R[avgPos % 8] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {
                    darkSampleL += avg8L[x];
                    darkSampleR += avg8R[x];
                }
                darkSampleL /= 8.0;
                darkSampleR /= 8.0;
            }
            if (spacing > 3) {
                avg4L[avgPos % 4] = darkSampleL;
                avg4R[avgPos % 4] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {
                    darkSampleL += avg4L[x];
                    darkSampleR += avg4R[x];
                }
                darkSampleL /= 4.0;
                darkSampleR /= 4.0;
            }
            if (spacing > 1) {
                avg2L[avgPos % 2] = darkSampleL;
                avg2R[avgPos % 2] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {
                    darkSampleL += avg2L[x];
                    darkSampleR += avg2R[x];
                }
                darkSampleL /= 2.0;
                darkSampleR /= 2.0;
            } // only update avgPos after the post-distortion filter stage
            double avgSlewL = fmin(fabs(lastDarkL - inputSampleL) * 0.12 * overallscale, 1.0);
            avgSlewL = 1.0 - (1.0 - avgSlewL * 1.0 - avgSlewL);
            inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
            lastDarkL = darkSampleL;
            double avgSlewR = fmin(fabs(lastDarkR - inputSampleR) * 0.12 * overallscale, 1.0);
            avgSlewR = 1.0 - (1.0 - avgSlewR * 1.0 - avgSlewR);
            inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
            lastDarkR = darkSampleR;
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
            darkSampleL = inputSampleL;
            darkSampleR = inputSampleR;
            if (avgPos > 31) {
                avgPos = 0;
            }
            if (spacing > 31) {
                post32L[avgPos] = darkSampleL;
                post32R[avgPos] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 32; x++) {
                    darkSampleL += post32L[x];
                    darkSampleR += post32R[x];
                }
                darkSampleL /= 32.0;
                darkSampleR /= 32.0;
            }
            if (spacing > 15) {
                post16L[avgPos % 16] = darkSampleL;
                post16R[avgPos % 16] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 16; x++) {
                    darkSampleL += post16L[x];
                    darkSampleR += post16R[x];
                }
                darkSampleL /= 16.0;
                darkSampleR /= 16.0;
            }
            if (spacing > 7) {
                post8L[avgPos % 8] = darkSampleL;
                post8R[avgPos % 8] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 8; x++) {
                    darkSampleL += post8L[x];
                    darkSampleR += post8R[x];
                }
                darkSampleL /= 8.0;
                darkSampleR /= 8.0;
            }
            if (spacing > 3) {
                post4L[avgPos % 4] = darkSampleL;
                post4R[avgPos % 4] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 4; x++) {
                    darkSampleL += post4L[x];
                    darkSampleR += post4R[x];
                }
                darkSampleL /= 4.0;
                darkSampleR /= 4.0;
            }
            if (spacing > 1) {
                post2L[avgPos % 2] = darkSampleL;
                post2R[avgPos % 2] = darkSampleR;
                darkSampleL = 0.0;
                darkSampleR = 0.0;
                for (int x = 0; x < 2; x++) {
                    darkSampleL += post2L[x];
                    darkSampleR += post2R[x];
                }
                darkSampleL /= 2.0;
                darkSampleR /= 2.0;
            }
            avgPos++;
            inputSampleL = (inputSampleL * (1.0 - avgSlewL)) + (darkSampleL * avgSlewL);
            inputSampleR = (inputSampleR * (1.0 - avgSlewR)) + (darkSampleR * avgSlewR);
            // use the previously calculated depth of the filter
            inputSampleL = (inputSampleL * outputGain * wet) + (drySampleL * (1.0 - wet));
            inputSampleR = (inputSampleR * outputGain * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::tapehack2
