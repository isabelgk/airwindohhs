#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::pitchnasty {

constexpr std::string_view k_name{ "PitchNasty" };
constexpr std::string_view k_short_description{
    "PitchNasty is a primitive pitch shifter that also has primitive time-stretch artifacts."
};
constexpr std::string_view k_long_description{
    "So this is a bit unusual. Though this is a sound mangler, there's no bit crushing here, and in fact most of the time it's delivering a very high-res immediate and punchy pitch shift, super clean and tight.It's just that the rest of the time it's throwing a nasty, loud sample-glitch, at audio rates. No, beyond that. It's throwing digital trash at you so hard it becomes a musical note!And therein lies the secret of PitchNasty. This plugin brings you the heart of old school digital like your classic Akai stuff, back when they did not have the luxury of doing anything elegantly or nicely. Instead, you got things like time stretches that just plain looped a tight time cycle and overlapped it, producing a weird digital honk. Some folks really seem to crave that stuff, and there's a reason.Turns out if you do that, your results tend to be very punchy, direct and intense, except for the weird robotic overtone that's welded to the sound like it's a musical note. Things like drums love being timestretched or repitched in this way! It's a whole retro tone, which PitchNasty starts off with. The crossover is very slightly 'clever' in a way the retro stuff isn't, for the purpose of making it sound more retro and less DAW-like: it keeps the presence very high while slightly masking the high frequency edge of the 'note' you get.But then PitchNasty goes way beyond, in that Airwindows way. You've got two pitch controls, one giving you note intervals in half-steps, and the other being a pure pitch bend. They stack, for really high or low bends. Then, there's a control that's the same as the classic Akai method of setting the sample buffer size… but for this one, you specify the buffer as a musical note (six octave range). Set it insanely high and you're basically not able to pitch shift anymore because the buffer's too small, set it insanely low and it barely registers as a note anymore. And then after that, how about a feedback? How about a feedback that can be cranked to more or less constant regeneration? This gives you horrible wonderful old Eventide noises of many descriptions, or you can use traces of it to make your existing sound more complicated and harmonic-dense.And then the whole thing's followed by a Dry/Wet, and you can see that PitchNasty sticks so tightly to the underlying sound that you can get it acting like a giant flanger or strange overtone generator. And that's the other secret of the crude old Akai-like time/pitch processing: when you don't have any RAM or CPU to work with, you can only do naive primitive things that happen to sound really immediate, direct, alive. It doesn't lose the impact of a drum track. If you set it up to thicken a snare by applying, Eventide-like, a 30% layer of pitch up (or down, with feedback), there is no flam or hesitation to the sound like more sophisticated algorithms would have to do.  Instead, it's just THERE in the sound, with a hefty dose of digital gnarliness, but woven right in to make a very 80s composite sound that's huge and fierce.If you would like to use this as a time stretch, what you should do is open the source sound in an editor, change the sample rate without resampling until you have the new pitch you want and apply that, resample it back to what your working sample rate is (use a good resampling method, you don't need to use a bad one), THEN use PitchNasty to re-pitch it to what the target pitch is. That'll give you the time-stretch artifacts, because they are really just pitch-change artifacts used in a different light. Hope you like PitchNasty!"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class PitchNasty final : public Effect<T>
{
    double dL[10002];
    double dR[10002];
    int inCount;
    double outCount;
    bool switchTransition;
    double switchAmountL;
    double switchAmountR;
    double feedbackSampleL;
    double feedbackSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    PitchNasty()
    {
        A = 0.0;
        B = 0.0;
        C = 0.25;
        D = 0.0;
        E = 1.0;
        F = 1.0;
        for (int temp = 0; temp < 10001; temp++) {
            dL[temp] = 0.0;
            dR[temp] = 0.0;
        }
        inCount = 1;
        outCount = 1.0;
        switchTransition = false;
        switchAmountL = 0.0;
        switchAmountR = 0.0;
        feedbackSampleL = 0.0;
        feedbackSampleR = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

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
            case kParamE: return E; break;
            case kParamF: return F; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.25; break;
            case kParamD: return 0.0; break;
            case kParamE: return 1.0; break;
            case kParamF: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "note"; break;
            case kParamB: return "bend"; break;
            case kParamC: return "grind"; break;
            case kParamD: return "feedbck"; break;
            case kParamE: return "output"; break;
            case kParamF: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Note"; break;
            case kParamB: return "Bend"; break;
            case kParamC: return "Grind"; break;
            case kParamD: return "Feedbck"; break;
            case kParamE: return "Output"; break;
            case kParamF: return "Dry/Wet"; break;

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
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;

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

        double note = (int)(A * 24) - 12; // -12 to +12
        double bend = (B * 24.0) - 12.0; // -12 to +12
        double speed = pow(2, note / 12.0) * pow(2, bend / 12.0);
        double grindRef = (1.0 / 261.6) * Effect<T>::getSampleRate(); // samples per Middle C cycle
        double grindNote = pow(2, ((int)(C * 72) - 36) / 12.0);
        double width = grindRef / grindNote;
        if (width > 9990) {
            width = 9990; // safety check
        }
        double feedback = D;
        double trim = E;
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
            inputSampleL += (feedbackSampleL * feedback);
            inputSampleR += (feedbackSampleR * feedback);
            if (fabs(feedbackSampleL) > 1.0) {
                inputSampleL /= fabs(feedbackSampleL);
            }
            if (fabs(feedbackSampleR) > 1.0) {
                inputSampleR /= fabs(feedbackSampleR);
            }
            dL[inCount] = inputSampleL;
            dR[inCount] = inputSampleR;
            inCount++;
            outCount += speed;
            if (outCount > inCount && switchTransition) {
                switchAmountL = 1.0;
                switchAmountR = 1.0;
                switchTransition = false;
            }
            if (inCount > width) {
                inCount -= width;
            }
            if (outCount > width) {
                outCount -= width;
                switchTransition = true;
                // reset the transition point
            }
            int count = floor(outCount);
            int arrayWidth = floor(width);
            inputSampleL = (dL[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (outCount - floor(outCount))));
            inputSampleL += (dL[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (outCount - floor(outCount)));
            inputSampleR = (dR[count - ((count > arrayWidth) ? arrayWidth + 1 : 0)] * (1 - (outCount - floor(outCount))));
            inputSampleR += (dR[count + 1 - ((count + 1 > arrayWidth) ? arrayWidth + 1 : 0)] * (outCount - floor(outCount)));
            if (switchAmountL > fabs(inputSampleL - dL[inCount]) * 2.0) {
                switchAmountL = (switchAmountL * 0.5) + (fabs(inputSampleL - dL[inCount]));
            }
            if (switchAmountR > fabs(inputSampleR - dR[inCount]) * 2.0) {
                switchAmountR = (switchAmountR * 0.5) + (fabs(inputSampleR - dR[inCount]));
            }
            inputSampleL = (dL[inCount] * switchAmountL) + (inputSampleL * (1.0 - switchAmountL));
            inputSampleR = (dR[inCount] * switchAmountR) + (inputSampleR * (1.0 - switchAmountR));
            feedbackSampleL = inputSampleL;
            feedbackSampleR = inputSampleR;
            if (trim != 1.0) {
                inputSampleL *= trim;
                inputSampleR *= trim;
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
} // namespace airwindohhs::pitchnasty
