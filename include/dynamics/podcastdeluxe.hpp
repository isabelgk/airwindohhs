#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::podcastdeluxe {

constexpr std::string_view k_name{ "PodcastDeluxe" };
constexpr std::string_view k_short_description{
    "PodcastDeluxe is a pile of compressors (curve style) and phase rotators."
};
constexpr std::string_view k_long_description{
    "PodcastDeluxe is the precursor to curve, along with its counterpart Podcast. Both of them have five curve-like compressors in series (slightly less refined), but PodcastDeluxe has five phase rotators and an ADClip-style output stage. The idea is that it would be radio station style processing. The reason it didn’t show up sooner is that it didn’t really work to my satisfaction. It’s still not perfect, but it’s different! I demonstrate it on house-type music, and on raw drums. It’s not really clean, not really dirty, not really squish-capable thanks to the curve-style compression (even five stages doesn’t give you ‘compression pumping’) but it’ll give a perhaps interesting, definitely processed-sounding effect.Maybe you’ll like it on a mix because you’re not fussy about distorting, maybe you’ll find some useful place for it elsewhere. I think it’s got a knack for high-impact drum busses without distorting them too obviously. At any rate, there’s nothing quite like it. Enjoy!"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class PodcastDeluxe final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double d1L[503];
    double d2L[503];
    double d3L[503];
    double d4L[503];
    double d5L[503];
    // the phase rotator
    double c1L;
    double c2L;
    double c3L;
    double c4L;
    double c5L;
    // the compressor
    double lastSampleL;
    double lastOutSampleL;
    double d1R[503];
    double d2R[503];
    double d3R[503];
    double d4R[503];
    double d5R[503];
    int tap1, tap2, tap3, tap4, tap5, maxdelay1, maxdelay2, maxdelay3, maxdelay4, maxdelay5;
    // the phase rotator
    double c1R;
    double c2R;
    double c3R;
    double c4R;
    double c5R;
    // the compressor
    double lastSampleR;
    double lastOutSampleR;
    float A;

  public:
    PodcastDeluxe()
    {
        A = 0.5;
        for (int count = 0; count < 502; count++) {
            d1L[count] = 0.0;
            d2L[count] = 0.0;
            d3L[count] = 0.0;
            d4L[count] = 0.0;
            d5L[count] = 0.0;
            d1R[count] = 0.0;
            d2R[count] = 0.0;
            d3R[count] = 0.0;
            d4R[count] = 0.0;
            d5R[count] = 0.0;
        }
        c1L = 2.0;
        c2L = 2.0;
        c3L = 2.0;
        c4L = 2.0;
        c5L = 2.0; // startup comp gains
        lastSampleL = lastOutSampleL = lastSampleR = lastOutSampleR = 0.0;
        tap1 = 1;
        tap2 = 1;
        tap3 = 1;
        tap4 = 1;
        tap5 = 1;
        maxdelay1 = 9001;
        maxdelay2 = 9001;
        maxdelay3 = 9001;
        maxdelay4 = 9001;
        maxdelay5 = 9001;
        c1R = 2.0;
        c2R = 2.0;
        c3R = 2.0;
        c4R = 2.0;
        c5R = 2.0; // startup comp gains
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "boost";
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
            return "Boost";
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
        int allpasstemp;
        double outallpass = 0.618033988749894848204586;
        double compress = 1.0 + pow(A * 0.8, 2);
        double speed1 = 128.0 / pow(compress, 2);
        speed1 *= overallscale;
        double speed2 = speed1 * 1.4;
        double speed3 = speed2 * 1.5;
        double speed4 = speed3 * 1.6;
        double speed5 = speed4 * 1.7;
        maxdelay1 = (int)(23.0 * overallscale);
        maxdelay2 = (int)(19.0 * overallscale);
        maxdelay3 = (int)(17.0 * overallscale);
        maxdelay4 = (int)(13.0 * overallscale);
        maxdelay5 = (int)(11.0 * overallscale);
        // set up the prime delays
        double refclip = 0.999;
        double softness = 0.435;
        double invsoft = 0.56;
        double outsoft = 0.545;
        double trigger;
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
            allpasstemp = tap1 - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay1) {
                allpasstemp = maxdelay1;
            }
            // set up the delay position
            // using 'tap' and 'allpasstemp' to position the tap
            inputSampleL -= d1L[allpasstemp] * outallpass;
            d1L[tap1] = inputSampleL;
            inputSampleL *= outallpass;
            inputSampleL += (d1L[allpasstemp]);
            // allpass stage
            inputSampleR -= d1R[allpasstemp] * outallpass;
            d1R[tap1] = inputSampleR;
            inputSampleR *= outallpass;
            inputSampleR += (d1R[allpasstemp]);
            // allpass stage
            tap1--;
            if (tap1 < 0 || tap1 > maxdelay1) {
                tap1 = maxdelay1;
            }
            // decrement the position for reals
            inputSampleL *= c1L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1L += trigger / speed5;
            if (c1L > compress) {
                c1L = compress;
            }
            // compress stage
            inputSampleR *= c1R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c1R += trigger / speed5;
            if (c1R > compress) {
                c1R = compress;
            }
            // compress stage
            allpasstemp = tap2 - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay2) {
                allpasstemp = maxdelay2;
            }
            // set up the delay position
            // using 'tap' and 'allpasstemp' to position the tap
            inputSampleL -= d2L[allpasstemp] * outallpass;
            d2L[tap2] = inputSampleL;
            inputSampleL *= outallpass;
            inputSampleL += (d2L[allpasstemp]);
            // allpass stage
            inputSampleR -= d2R[allpasstemp] * outallpass;
            d2R[tap2] = inputSampleR;
            inputSampleR *= outallpass;
            inputSampleR += (d2R[allpasstemp]);
            // allpass stage
            tap2--;
            if (tap2 < 0 || tap2 > maxdelay2) {
                tap2 = maxdelay2;
            }
            // decrement the position for reals
            inputSampleL *= c2L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2L += trigger / speed4;
            if (c2L > compress) {
                c2L = compress;
            }
            // compress stage
            inputSampleR *= c2R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c2R += trigger / speed4;
            if (c2R > compress) {
                c2R = compress;
            }
            // compress stage
            allpasstemp = tap3 - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay3) {
                allpasstemp = maxdelay3;
            }
            // set up the delay position
            // using 'tap' and 'allpasstemp' to position the tap
            inputSampleL -= d3L[allpasstemp] * outallpass;
            d3L[tap3] = inputSampleL;
            inputSampleL *= outallpass;
            inputSampleL += (d3L[allpasstemp]);
            // allpass stage
            inputSampleR -= d3R[allpasstemp] * outallpass;
            d3R[tap3] = inputSampleR;
            inputSampleR *= outallpass;
            inputSampleR += (d3R[allpasstemp]);
            // allpass stage
            tap3--;
            if (tap3 < 0 || tap3 > maxdelay3) {
                tap3 = maxdelay3;
            }
            // decrement the position for reals
            inputSampleL *= c3L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3L += trigger / speed3;
            if (c3L > compress) {
                c3L = compress;
            }
            // compress stage
            inputSampleR *= c3R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c3R += trigger / speed3;
            if (c3R > compress) {
                c3R = compress;
            }
            // compress stage
            allpasstemp = tap4 - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay4) {
                allpasstemp = maxdelay4;
            }
            // set up the delay position
            // using 'tap' and 'allpasstemp' to position the tap
            inputSampleL -= d4L[allpasstemp] * outallpass;
            d4L[tap4] = inputSampleL;
            inputSampleL *= outallpass;
            inputSampleL += (d4L[allpasstemp]);
            // allpass stage
            inputSampleR -= d4R[allpasstemp] * outallpass;
            d4R[tap4] = inputSampleR;
            inputSampleR *= outallpass;
            inputSampleR += (d4R[allpasstemp]);
            // allpass stage
            tap4--;
            if (tap4 < 0 || tap4 > maxdelay4) {
                tap4 = maxdelay4;
            }
            // decrement the position for reals
            inputSampleL *= c4L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4L += trigger / speed2;
            if (c4L > compress) {
                c4L = compress;
            }
            // compress stage
            inputSampleR *= c4R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c4R += trigger / speed2;
            if (c4R > compress) {
                c4R = compress;
            }
            // compress stage
            allpasstemp = tap5 - 1;
            if (allpasstemp < 0 || allpasstemp > maxdelay5) {
                allpasstemp = maxdelay5;
            }
            // set up the delay position
            // using 'tap' and 'allpasstemp' to position the tap
            inputSampleL -= d5L[allpasstemp] * outallpass;
            d5L[tap5] = inputSampleL;
            inputSampleL *= outallpass;
            inputSampleL += (d5L[allpasstemp]);
            // allpass stage
            inputSampleR -= d5R[allpasstemp] * outallpass;
            d5R[tap5] = inputSampleR;
            inputSampleR *= outallpass;
            inputSampleR += (d5R[allpasstemp]);
            // allpass stage
            tap5--;
            if (tap5 < 0 || tap5 > maxdelay5) {
                tap5 = maxdelay5;
            }
            // decrement the position for reals
            inputSampleL *= c5L;
            trigger = fabs(inputSampleL) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5L += trigger / speed1;
            if (c5L > compress) {
                c5L = compress;
            }
            // compress stage
            inputSampleR *= c5R;
            trigger = fabs(inputSampleR) * 4.7;
            if (trigger > 4.7) {
                trigger = 4.7;
            }
            trigger = sin(trigger);
            if (trigger < 0) {
                trigger *= 8.0;
            }
            if (trigger < -4.2) {
                trigger = -4.2;
            }
            c5R += trigger / speed1;
            if (c5R > compress) {
                c5R = compress;
            }
            // compress stage
            if (compress > 1.0) {
                inputSampleL /= compress;
                inputSampleR /= compress;
            }
            // Begin L ADClip
            if (lastSampleL >= refclip)
            {
                if (inputSampleL < refclip)
                {
                    lastSampleL = (outsoft + (inputSampleL * softness));
                }
                else {
                    lastSampleL = refclip;
                }
            }
            if (lastSampleL <= -refclip)
            {
                if (inputSampleL > -refclip)
                {
                    lastSampleL = (-outsoft + (inputSampleL * softness));
                }
                else {
                    lastSampleL = -refclip;
                }
            }
            if (inputSampleL > refclip)
            {
                if (lastSampleL < refclip)
                {
                    inputSampleL = (invsoft + (lastSampleL * softness));
                }
                else {
                    inputSampleL = refclip;
                }
            }
            if (inputSampleL < -refclip)
            {
                if (lastSampleL > -refclip)
                {
                    inputSampleL = (-invsoft + (lastSampleL * softness));
                }
                else {
                    inputSampleL = -refclip;
                }
            }
            // Completed L ADClip
            // Begin R ADClip
            if (lastSampleR >= refclip)
            {
                if (inputSampleR < refclip)
                {
                    lastSampleR = (outsoft + (inputSampleR * softness));
                }
                else {
                    lastSampleR = refclip;
                }
            }
            if (lastSampleR <= -refclip)
            {
                if (inputSampleR > -refclip)
                {
                    lastSampleR = (-outsoft + (inputSampleR * softness));
                }
                else {
                    lastSampleR = -refclip;
                }
            }
            if (inputSampleR > refclip)
            {
                if (lastSampleR < refclip)
                {
                    inputSampleR = (invsoft + (lastSampleR * softness));
                }
                else {
                    inputSampleR = refclip;
                }
            }
            if (inputSampleR < -refclip)
            {
                if (lastSampleR > -refclip)
                {
                    inputSampleR = (-invsoft + (lastSampleR * softness));
                }
                else {
                    inputSampleR = -refclip;
                }
            }
            // Completed R ADClip
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
} // namespace airwindohhs::podcastdeluxe
