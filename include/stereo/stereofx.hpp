#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::stereofx {

constexpr std::string_view k_name{ "StereoFX" };
constexpr std::string_view k_short_description{
    "StereoFX is an aggressive stereo widener. Historical note: this post included in full as it's right after my Dad died, and includes my thanks to Airwindows folks for supporting me through that:"
};
constexpr std::string_view k_long_description{
    "Here’s the last plugin I had in the pipeline, ready to post: no reason not to take the effort to post it now. Thank you all for the well wishes: I’m really touched, you’re all so sweet I can hardly believe it’s the internet :) (but then, people might well say the same about me!)StereoFX is a classic Airwindows plugin brought up to date and VSTified: it does three things that can contour the stereo image. None of them are as well behaved as Wider, but they’re interestingly different.Stereo Wide basically runs the code from High Impact on the side channel. It gives a really aggressive widening effect (which will cancel in mono of course) and can bring an edgy quality to the wideness of your stereo channel.Mono Bass is simply a highpass on the side channel: adjust to taste, by ear. It’s a very simple highpass, not that steep.Center Squish is a neat twist on ‘widening’. Instead of touching the side channel, it does a simple sine-based saturation or distortion (Density style, not Spiral style) on the mid channel only. If you engage it, it progressively steps on the output of the mid channel but leaves side untouched. That means you can squash a stereo track slightly, giving it a little distortion, and let it squirt out to the sides a bit. You can combine this with a touch of the Stereo Wide, which works on a different algorithm. So it’s some unique tone colorings and techniques to serve the purpose of stereo widening. I hope you like it.I got a new video light (well, a $30 lightbox to go on one of my existing LED lightbulbs) so I’m kind of excited to make more videos and stuff, and I’ve got a backlog of plugins to try, but understandably (see the DrumSlam post) I’m a little distracted. I want to bring my best for you guys but I also want to honor your kind wishes, so I will post StereoFX and then see if anything comes to mind that I can do, if not immediately for the end of July, then early August: I am also doing the occasional music livestreaming and find that I can express my feelings that way, even if it’s ‘skronk guitar over techno’. So I won’t try and force the plugins (they won’t run out, I promise) and soon I’ll be back in the swing of things. Oh, and the Patreon is doing OK, I won’t worry about that right now but that too is appreciated. If not for that, I wouldn’t have been able to go and support my sister when all this went down, because it made me able to drop everything and zip off to Pennsylvania: without the Patreon, I would struggle to have that much gas covered, and I wouldn’t have been free to go right then, and then I wouldn’t have seen my Dad alive for the last time. So I owe you guys things that can’t be expressed in software or money: thank you."
};
constexpr std::string_view k_tags{
    "stereo"
};

template <typename T>
class StereoFX final : public Effect<T>
{
    double iirSampleA;
    double iirSampleB;
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;
    // default stuff
    float A;
    float B;
    float C;

  public:
    StereoFX()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        iirSampleA = 0.0;
        iirSampleB = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = false;
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
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "wide"; break;
            case kParamB: return "monobs"; break;
            case kParamC: return "csquish"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Wide"; break;
            case kParamB: return "MonoBs"; break;
            case kParamC: return "CSquish"; break;

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
        double inputSampleL;
        double inputSampleR;
        double mid;
        double side;
        // High Impact section
        double stereowide = A;
        double centersquish = C;
        double density = stereowide * 2.4;
        double sustain = 1.0 - (1.0 / (1.0 + (density / 7.0)));
        // this way, enhance increases up to 50% and then mid falls off beyond that
        double bridgerectifier;
        double count;
        // Highpass section
        double iirAmount = pow(B, 3) / overallscale;
        double tight = -0.33333333333333;
        double offset;
        // we are setting it up so that to either extreme we can get an audible sound,
        // but sort of scaled so small adjustments don't shift the cutoff frequency yet.
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
            // assign working variables
            mid = inputSampleL + inputSampleR;
            side = inputSampleL - inputSampleR;
            // assign mid and side. Now, High Impact code
            count = density;
            while (count > 1.0)
            {
                bridgerectifier = fabs(side) * 1.57079633;
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                // max value for sine function
                bridgerectifier = sin(bridgerectifier);
                if (side > 0.0) {
                    side = bridgerectifier;
                }
                else {
                    side = -bridgerectifier;
                }
                count = count - 1.0;
            }
            // we have now accounted for any really high density settings.
            bridgerectifier = fabs(side) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            bridgerectifier = sin(bridgerectifier);
            if (side > 0) {
                side = (side * (1 - count)) + (bridgerectifier * count);
            }
            else {
                side = (side * (1 - count)) - (bridgerectifier * count);
            }
            // blend according to density control
            // done first density. Next, sustain-reducer
            bridgerectifier = fabs(side) * 1.57079633;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = (1 - cos(bridgerectifier)) * 3.141592653589793;
            if (side > 0) {
                side = (side * (1 - sustain)) + (bridgerectifier * sustain);
            }
            else {
                side = (side * (1 - sustain)) - (bridgerectifier * sustain);
            }
            // done with High Impact code
            // now, Highpass code
            offset = 0.666666666666666 + ((1 - fabs(side)) * tight);
            if (offset < 0) {
                offset = 0;
            }
            if (offset > 1) {
                offset = 1;
            }
            if (flip)
            {
                iirSampleA = (iirSampleA * (1 - (offset * iirAmount))) + (side * (offset * iirAmount));
                side = side - iirSampleA;
            }
            else
            {
                iirSampleB = (iirSampleB * (1 - (offset * iirAmount))) + (side * (offset * iirAmount));
                side = side - iirSampleB;
            }
            // done with Highpass code
            bridgerectifier = fabs(mid) / 1.273239544735162;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier) * 1.273239544735162;
            if (mid > 0) {
                mid = (mid * (1 - centersquish)) + (bridgerectifier * centersquish);
            }
            else {
                mid = (mid * (1 - centersquish)) - (bridgerectifier * centersquish);
            }
            // done with the mid saturating section.
            inputSampleL = (mid + side) / 2.0;
            inputSampleR = (mid - side) / 2.0;
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
} // namespace airwindohhs::stereofx
