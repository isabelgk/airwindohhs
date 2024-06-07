#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::shape {

constexpr std::string_view k_name{ "Shape" };
constexpr std::string_view k_short_description{
    "Shape is an asymmetrical waveshaper for peak manipulating."
};
constexpr std::string_view k_long_description{
    "Here’s a really basic building block for ya. The key word here is asymmetrical.Shape is a simple, often subtle, little plugin for either pulling out, or squishing in, parts of the waveform. It’s based on the same basic technology as Console: complementary sin() and asin() functions. But in Shape, you’re picking which you want to apply, and you’re offsetting things so you can hit one side of the waveform more than the other.Works like this. The Shape control determines what you’re doing. In the middle (0.5) you’re not doing anything. Increase it to 1.0 and you are stretching out the peaks of the waveform. Turn it down to 0.0 and you’re squishing the peaks of the waveform: like a soft-saturation but no boost, just restricting the waveform and making it distorted. The control scales all this so if you’re not cranking it out, you have lots of headroom and very gentle effects.The Offset control interacts with this by shifting things from one side to the other: 0.5 is no offset, 0.0 is negative and 1.0 is positive (in my sound editor in the video, this is the top half of the waveform as positive). The effect of this, especially if you’ve not cranked out the Shape control, is to make the Shape control do its thing to only one side of the wave. You shouldn’t automatically crank this one out either: you can, for nasty waveshaping tricks, but the power of Shape is in how transparently it can manage the positive and negative swings of your waveform.Also, when we’re reshaping the waveform so asymmetrically, that’s built-in even harmonics: this is a recipe for warmth. It’s like a turbo version of my PurestWarm plugins but even smoother, or like a variation of my SingleEndedTriode plugin. Use it gracefully (not cranking out the controls all the way) and everything you do will be tonal adjustments of great purity and subtlety, which is why it can be a bit hard to hear its effect when the controls are very near the middle.So the trick is to know what way you’re using it. If you’re doing sound design and you’re trying to get a sound to swing fully in both directions, you can either pick the part that you think is swinging too far (pos or neg), set the offset to favor that, and pull back that side by setting Shape lower than halfway (cutting BACK the side you’re focussing on), or you can pick the side that doesn’t go far enough, set the offset to favor it, and turn Shape UP to bring that side’s peaks up to match the other side.Or if you’re working on a bass and want it to be warmer, you can just choose between more Shape (more of a triangle-ey result out of a sine input) or less Shape (more of a squared result out of a sine input) and then lean the Offset one way or the other to get the warmth you want (not all the way, or it won’t be as smooth).Or, you can take your bass that you want to be purer, and try to identify whether it’s too triangle-ey or too square-ey, and then apply Shape to nudge it more in the direction of a pure sine-ey tone. It’s a waveshaper, so you can stack them up to get a VERY soft square or a soft-pointed triangle out of any sine-bass you start with, or put ’em back to the sine shape, or lean the offset one way or another, all very transparently.I don’t know how clear all this is, because you probably don’t have this function laid out in this way, and it’s too subtle for normal plugin makers to be able to sell you. Any commercial plugin where turning it up doesn’t equal ‘more’ plus you can’t always hear what it does, is in big trouble. But I’m Patreon-supported so I don’t have to follow those rules, so I can give you Shape even if most people don’t get a big charge out of it and throw their money :)And so I have. Enjoy Shape. My hope is that it serves its useful purpose for those who need to tweak exactly this, as transparently as possible. And if you didn’t need to… maybe you just didn’t know it was a thing. It is now! :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Shape final : public Effect<T>
{
    enum
    {
        fix_freq,
        fix_reso,
        fix_a0,
        fix_a1,
        fix_a2,
        fix_b1,
        fix_b2,
        fix_sL1,
        fix_sL2,
        fix_sR1,
        fix_sR2,
        fix_total

        public :
            Shape(){
                A = 0.5;
                B = 0.5;
                fpdL = 1.0; while (fpdL < 16386) fpdL = rand() * UINT32_MAX;
                fpdR = 1.0; while (fpdR < 16386) fpdR = rand() * UINT32_MAX;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.5;
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
            return "shape";
            break;
        kParamB:
            return "neg/pos";
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
            return "Shape";
            break;
        kParamB:
            return "Neg/Pos";
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

        double shape = -((A * 2.0) - 1.0);
        double gainstage = fabs(shape) + 0.01; // no divide by zero
        double offset = (B * 2.0) - 1.0;
        double postOffset = 0.0;
        if (shape > 0) {
            gainstage += 0.99;
            postOffset = sin(offset);
        }
        if (shape < 0) {
            postOffset = asin(offset);
        }
        double cutoff = 25000.0 / Effect<T>::getSampleRate();
        if (cutoff > 0.49) {
            cutoff = 0.49; // don't crash if run at 44.1k
        }
        fixA[fix_freq] = cutoff;
        fixA[fix_reso] = 0.70710678; // butterworth Q
        double K = tan(M_PI * fixA[fix_freq]); // lowpass
        double norm = 1.0 / (1.0 + K / fixA[fix_reso] + K * K);
        fixA[fix_a0] = K * K * norm;
        fixA[fix_a1] = 2.0 * fixA[fix_a0];
        fixA[fix_a2] = fixA[fix_a0];
        fixA[fix_b1] = 2.0 * (K * K - 1.0) * norm;
        fixA[fix_b2] = (1.0 - K / fixA[fix_reso] + K * K) * norm;
        fixA[fix_sL1] = 0.0;
        fixA[fix_sL2] = 0.0;
        fixA[fix_sR1] = 0.0;
        fixA[fix_sR2] = 0.0;
        // define filters here: on VST you can't define them in reset 'cos getSampleRate isn't returning good information yet
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
            double outSample = (inputSampleL * fixA[fix_a0]) + fixA[fix_sL1];
            fixA[fix_sL1] = (inputSampleL * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sL2];
            fixA[fix_sL2] = (inputSampleL * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleL = outSample; // fixed biquad filtering ultrasonics
            inputSampleL *= gainstage;
            inputSampleL += offset;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (shape > 0) {
                inputSampleL = sin(inputSampleL);
            }
            if (shape < 0) {
                inputSampleL = asin(inputSampleL);
            }
            inputSampleL -= postOffset;
            inputSampleL = ((inputSampleL / gainstage) * fabs(shape)) + (drySampleL * (1.0 - fabs(shape)));
            outSample = (inputSampleR * fixA[fix_a0]) + fixA[fix_sR1];
            fixA[fix_sR1] = (inputSampleR * fixA[fix_a1]) - (outSample * fixA[fix_b1]) + fixA[fix_sR2];
            fixA[fix_sR2] = (inputSampleR * fixA[fix_a2]) - (outSample * fixA[fix_b2]);
            inputSampleR = outSample; // fixed biquad filtering ultrasonics
            inputSampleR *= gainstage;
            inputSampleR += offset;
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (shape > 0) {
                inputSampleR = sin(inputSampleR);
            }
            if (shape < 0) {
                inputSampleR = asin(inputSampleR);
            }
            inputSampleR -= postOffset;
            inputSampleR = ((inputSampleR / gainstage) * fabs(shape)) + (drySampleR * (1.0 - fabs(shape)));
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
} // namespace airwindohhs::shape
