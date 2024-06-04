#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::purestecho {

constexpr std::string_view k_name{ "PurestEcho" };
constexpr std::string_view k_short_description{
    "PurestEcho is optimized Airwindows echo with exactly four evenly spaced taps on tap."
};
constexpr std::string_view k_long_description{
    "PurestEcho gives you four echoes, sample-averaged to deliver extremely precise timing, each with its own tap. That means you can use it as a delay line and set up discrete echoes which then stop completely: no feedback, only a series of trailing echoes which then get out of the way and don’t mud up your mix. Or you can use it for just a single slapback (use the last, for the longest delay time). Or, you can set up odd effects like increasing volume slapbacks, or use of only certain taps.Or, you can get a tone that’s part of the 90s: turn all the delay taps on full and then set the delay time really short. You’ll find there’s a tightly controlled pipe-like honk that emphasizes a musical tone… sort of a Dalek-like robot-like thing… and you can play this using the slider. Note: this slider is NOT smoothed, because it’s smashing the buffer anyway, but more importantly if you’re programming in a bassline using this effect you’d need it to abruptly switch frequencies. So think of it as something you’d play via control automation, not so much through live entry on the slider.Between that and the ability to throw a big rich echo that fakes ‘feedback’ but then gets right out of the way, I think PurestEcho will find its way into peoples’ workflows. I hope you like it :)"
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class PurestEcho final : public Effect<T>
{
    const static int totalsamples = 65535;
    double dL[totalsamples];
    double dR[totalsamples];
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    PurestEcho()
    {
        A = 1.0;
        B = 1.0;
        C = 0.0;
        D = 0.0;
        E = 0.0;
        for (int count = 0; count < totalsamples - 1; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        gcount = 0;
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
        kNumParameters = 5

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

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;
            case kParamD: return D;
            case kParamE: return E;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 1.0;
            case kParamB: return 1.0;
            case kParamC: return 0.0;
            case kParamD: return 0.0;
            case kParamE: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "time";
            case kParamB: return "tap 1";
            case kParamC: return "tap 2";
            case kParamD: return "tap 3";
            case kParamE: return "tap 4";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Time";
            case kParamB: return "Tap 1";
            case kParamC: return "Tap 2";
            case kParamD: return "Tap 3";
            case kParamE: return "Tap 4";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
            case kParamD: return "";
            case kParamE: return "";
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        int loopLimit = (int)(totalsamples * 0.499);
        // this is a double buffer so we will be splitting it in two
        double time = pow(A, 2) * 0.999;
        double tap1 = B;
        double tap2 = C;
        double tap3 = D;
        double tap4 = E;
        double gainTrim = 1.0 / (1.0 + tap1 + tap2 + tap3 + tap4);
        // this becomes our equal-loudness mechanic. 0.2 to 1.0 gain on all things.
        double tapsTrim = gainTrim * 0.5;
        // the taps interpolate and require half that gain: 0.1 to 0.5 on all taps.
        int position1 = (int)(loopLimit * time * 0.25);
        int position2 = (int)(loopLimit * time * 0.5);
        int position3 = (int)(loopLimit * time * 0.75);
        int position4 = (int)(loopLimit * time);
        // basic echo information: we're taking four equally spaced echoes and setting their levels as desired.
        // position4 is what you'd have for 'just set a delay time'
        double volAfter1 = (loopLimit * time * 0.25) - position1;
        double volAfter2 = (loopLimit * time * 0.5) - position2;
        double volAfter3 = (loopLimit * time * 0.75) - position3;
        double volAfter4 = (loopLimit * time) - position4;
        // these are 0-1: casting to an (int) truncates fractional numbers towards zero (and is faster than floor() )
        // so, when we take the integer number (all above zero) and subtract it from the real value, we get 0-1
        double volBefore1 = (1.0 - volAfter1) * tap1;
        double volBefore2 = (1.0 - volAfter2) * tap2;
        double volBefore3 = (1.0 - volAfter3) * tap3;
        double volBefore4 = (1.0 - volAfter4) * tap4;
        // and if we are including a bit of the previous/next sample to interpolate, then if the sample position is 1.0001
        // we'll be leaning most heavily on the 'before' sample which is nearer to us, and the 'after' sample is almost not used.
        // if the sample position is 1.9999, the 'after' sample is strong and 'before' is almost not used.
        volAfter1 *= tap1;
        volAfter2 *= tap2;
        volAfter3 *= tap3;
        volAfter4 *= tap4;
        // and like with volBefore, we also want to scale this 'interpolate' to the loudness of this tap.
        // We do it here because we can do it only once per audio buffer, not on every sample. This assumes we're
        // not moving the tap every sample: if so we'd have to do this every sample as well.
        int oneBefore1 = position1 - 1;
        int oneBefore2 = position2 - 1;
        int oneBefore3 = position3 - 1;
        int oneBefore4 = position4 - 1;
        if (oneBefore1 < 0) {
            oneBefore1 = 0;
        }
        if (oneBefore2 < 0) {
            oneBefore2 = 0;
        }
        if (oneBefore3 < 0) {
            oneBefore3 = 0;
        }
        if (oneBefore4 < 0) {
            oneBefore4 = 0;
        }
        int oneAfter1 = position1 + 1;
        int oneAfter2 = position2 + 1;
        int oneAfter3 = position3 + 1;
        int oneAfter4 = position4 + 1;
        // this is setting up the way we interpolate samples: we're doing an echo-darkening thing
        // to make it sound better. Pretty much no acoustic delay in human-breathable air will give
        // you zero attenuation at 22 kilohertz: forget this at your peril ;)
        double delaysBufferL;
        double delaysBufferR;
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
            if (gcount < 0 || gcount > loopLimit) {
                gcount = loopLimit;
            }
            dL[gcount + loopLimit] = dL[gcount] = inputSampleL * tapsTrim;
            dR[gcount + loopLimit] = dR[gcount] = inputSampleR * tapsTrim; // this is how the double buffer works:
            // we can look for delay taps without ever having to 'wrap around' within our calculation.
            // As long as the delay tap is less than our loop limit we can always just add it to where we're
            // at, and get a valid sample back right away, no matter where we are in the buffer.
            // The 0.5 is taking into account the interpolation, by padding down the whole buffer.
            delaysBufferL = (dL[gcount + oneBefore4] * volBefore4);
            delaysBufferL += (dL[gcount + oneAfter4] * volAfter4);
            delaysBufferL += (dL[gcount + oneBefore3] * volBefore3);
            delaysBufferL += (dL[gcount + oneAfter3] * volAfter3);
            delaysBufferL += (dL[gcount + oneBefore2] * volBefore2);
            delaysBufferL += (dL[gcount + oneAfter2] * volAfter2);
            delaysBufferL += (dL[gcount + oneBefore1] * volBefore1);
            delaysBufferL += (dL[gcount + oneAfter1] * volAfter1);
            delaysBufferR = (dR[gcount + oneBefore4] * volBefore4);
            delaysBufferR += (dR[gcount + oneAfter4] * volAfter4);
            delaysBufferR += (dR[gcount + oneBefore3] * volBefore3);
            delaysBufferR += (dR[gcount + oneAfter3] * volAfter3);
            delaysBufferR += (dR[gcount + oneBefore2] * volBefore2);
            delaysBufferR += (dR[gcount + oneAfter2] * volAfter2);
            delaysBufferR += (dR[gcount + oneBefore1] * volBefore1);
            delaysBufferR += (dR[gcount + oneAfter1] * volAfter1);
            // These are the interpolated samples. We're adding them first, because we know they're smaller
            // and while the value of delaysBuffer is small we'll add similarly small values to it. Note the order.
            delaysBufferL += (dL[gcount + position4] * tap4);
            delaysBufferL += (dL[gcount + position3] * tap3);
            delaysBufferL += (dL[gcount + position2] * tap2);
            delaysBufferL += (dL[gcount + position1] * tap1);
            delaysBufferR += (dR[gcount + position4] * tap4);
            delaysBufferR += (dR[gcount + position3] * tap3);
            delaysBufferR += (dR[gcount + position2] * tap2);
            delaysBufferR += (dR[gcount + position1] * tap1);
            // These are the primary samples for the echo, and we're adding them last. As before we're starting with the
            // most delayed echoes, and ending with what we think might be the loudest echo. We're building this delaybuffer
            // from the faintest noises to the loudest, to avoid adding a bunch of teeny values at the end.
            // You can of course put the last echo as loudest, but with diminishing echo volumes this is optimal.
            // This technique is also present in other plugins such as Iron Oxide.
            inputSampleL = (inputSampleL * gainTrim) + delaysBufferL;
            inputSampleR = (inputSampleR * gainTrim) + delaysBufferR;
            // this could be just inputSample += d[gcount+position1];
            // for literally a single, full volume echo combined with dry.
            // What I'm doing is making the echoes more interesting.
            gcount--;
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
} // namespace airwindohhs::purestecho
