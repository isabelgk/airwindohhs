#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::hull2 {

constexpr std::string_view k_name{ "Hull2" };
constexpr std::string_view k_short_description{
    "Hull2 is a very clear three-band EQ."
};
constexpr std::string_view k_long_description{
    "We ended up using the Hull algorithm in something! It's what makes the high band of ConsoleLA work. Hull2 is taking the guts of that code and giving it to you as a pristine, no-saturation, no analog mojo, pure EQ.Note that I didn't say 'normal' ;)The idea here is that it's very, very simple algorithms that combine to produce complicated results. When I describe what happens here, keep that in mind: the code that produces it is incredibly pure and simple, and the tone of these odd and complicated effects is very transparent and hangs onto expressiveness instead of degrading the tone.You've got a treble, mid, and bass control. If you move them all together, you get a simple gain control that's roughly as good as PurestGain. It's very close to PurestGain, if you've moved all three controls exactly together, and that's how transparent Hull2 can be.If you boost treble relative to mid (at any position), you get the 10k-centered boost from ConsoleLA, but without any harmonics or other alterations. It's an even clearer effect. It centers on 10k and falls off slightly above that (remembering that, flat, it's a perfect bypass).If you cut treble relative to mid, you get at first a soft notch, then increasingly steep. And then, the notch gets shallower again, and then it becomes a very steep roll-off slightly higher than that.If you boost lows relative to mid (at any position) you begin to lift the lows, while subtly cutting around 700 hz causing the sensation that the bass region is shifting lower while boosting.If you cut lows relative to mid, it'll subtly lift those same lower-mids, so again it's like shifting the voicing of the track rather than just 'adding and removing exact frequencies'. It's very broad-stroke EQ, like two tilt-EQs with a hinge in the middle, if that makes any sense.All this is designed in, but it's not done by banks of EQs doing elaborate (and unaccountable) things. It comes out of how very simple algorithms interact with each other, so the behaviors are somewhat designable but it's kind of unavoidable. It's the cost of using these crossovers at these steepnesses, and the trick is to design it so the weirdnesses do musically useful things. And then, the other trick is to construct the three-band EQ by deconstructing the input in such a way that you can just add it together again and get the input back.You could have the craziest, wildest crossover behavior, with all sorts of pre-ring or whatever (Hull2 doesn't, but you could have this) and subtract it from the highs to get a mid band. If you do that, both the bands will have exactly matching pre-ripple, if there's pre-ring (same with phase issues, etc).And then if you put 'em back together you have the original back: no more ripple, phase or anything.And of course if you apply only a tiny amount, you get only a tiny amount of whatever character is part of the crossover. And that's the principle in ConsoleLA, and in ConsoleMC (and MD), and now it's in Hull2, where ConsoleLA's treble crossover was developed.Hope you find some use for it :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Hull2 final : public Effect<T>
{
    double hullL[225];
    double hullR[225];
    int hullp;
    double hullbL[5];
    double hullbR[5];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

  public:
    Hull2()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        for (int count = 0; count < 222; count++) {
            hullL[count] = 0.0;
            hullR[count] = 0.0;
        }
        for (int count = 0; count < 4; count++) {
            hullbL[count] = 0.0;
            hullbR[count] = 0.0;
        }
        hullp = 1;
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
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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
        kParamC:
            return C;
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
        kParamC:
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
            return "treble";
            break;
        kParamB:
            return "mid";
            break;
        kParamC:
            return "bass";
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
            return "Treble";
            break;
        kParamB:
            return "Mid";
            break;
        kParamC:
            return "Bass";
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
        kParamC:
            return std::to_string(C);
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
        kParamC:
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
        overallscale *= getSampleRate();
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // max out at 4x, 192k
        double treble = A * 2.0;
        double mid = B * 2.0;
        double bass = C * 2.0;
        double iirAmount = 0.125 / cycleEnd;
        int limit = 4 * cycleEnd;
        double divisor = 2.0 / limit;
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
            // begin Hull2 treble crossover
            hullp--;
            if (hullp < 0) {
                hullp += 60;
            }
            hullL[hullp] = hullL[hullp + 60] = inputSampleL;
            hullR[hullp] = hullR[hullp + 60] = inputSampleR;
            int x = hullp;
            double midSampleL = 0.0;
            double midSampleR = 0.0;
            while (x < hullp + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 20] = hullL[hullp + 80] = midSampleL;
            hullR[hullp + 20] = hullR[hullp + 80] = midSampleR;
            x = hullp + 20;
            midSampleL = midSampleR = 0.0;
            while (x < hullp + 20 + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + 20 + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            hullL[hullp + 40] = hullL[hullp + 100] = midSampleL;
            hullR[hullp + 40] = hullR[hullp + 100] = midSampleR;
            x = hullp + 40;
            midSampleL = midSampleR = 0.0;
            while (x < hullp + 40 + (limit / 2)) {
                midSampleL += hullL[x] * divisor;
                midSampleR += hullR[x] * divisor;
                x++;
            }
            midSampleL += midSampleL * 0.125;
            midSampleR += midSampleR * 0.125;
            while (x < hullp + 40 + limit) {
                midSampleL -= hullL[x] * 0.125 * divisor;
                midSampleR -= hullR[x] * 0.125 * divisor;
                x++;
            }
            double trebleSampleL = drySampleL - midSampleL;
            double trebleSampleR = drySampleR - midSampleR;
            // end Hull2 treble crossover
            // begin Hull2 midbass crossover
            double bassSampleL = midSampleL;
            double bassSampleR = midSampleR;
            x = 0;
            while (x < 3) {
                hullbL[x] = (hullbL[x] * (1.0 - iirAmount)) + (bassSampleL * iirAmount);
                hullbR[x] = (hullbR[x] * (1.0 - iirAmount)) + (bassSampleR * iirAmount);
                if (fabs(hullbL[x]) < 1.18e-37) {
                    hullbL[x] = 0.0;
                }
                if (fabs(hullbR[x]) < 1.18e-37) {
                    hullbR[x] = 0.0;
                }
                bassSampleL = hullbL[x];
                bassSampleR = hullbR[x];
                x++;
            }
            midSampleL -= bassSampleL;
            midSampleR -= bassSampleR;
            // end Hull2 midbass crossover
            inputSampleL = (bassSampleL * bass) + (midSampleL * mid) + (trebleSampleL * treble);
            inputSampleR = (bassSampleR * bass) + (midSampleR * mid) + (trebleSampleR * treble);
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
} // namespace airwindohhs::hull2
