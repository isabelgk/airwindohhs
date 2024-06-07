#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::adt {

constexpr std::string_view k_name{ "ADT" };
constexpr std::string_view k_short_description{
    "ADT is a double short delay tap with saturation."
};
constexpr std::string_view k_long_description{
    "ADT means ‘artificial double-tracking’. You could also call it fixed flanging. It’s a single short delay, as heard on lots of Beatles tracks. It’s now my job to explain what’s different about the Airwindows ADT, what you can do with it beyond the obvious ‘stick it on like a preset and pretend you are a Beatle’, and why you’d bother.With the Airwindows ADT, you get two delay taps (making it A3T?) and an important feature: the mix sliders used to apply the delayed taps are ‘attenuverters’. That’s a word from Modular Synthesizer Land, which means you get both output level control and the ability to invert the output. Here’s why that matters.If you apply a fixed delay tap, you get an effect called ‘comb filtering’ where you’re emphasizing and cancelling frequencies based on how long the delay is. You’d think that would sound really strange, but it’s the same way you hear a direct sound and also the sound bouncing off a floor or wall: we naturally hear through comb filtering rather well, which is why room design is important in studio control rooms. (you could have a bass frequency getting cancelled, be unable to hear it at all at your listening position, and yet things will still sound perfectly normal.) Applying a quick delay like this can make your sound richer and more textural, and a little more ambient. If it’s a very short delay it may not be heard as an echo at all.But, if you’re using an INVERTED delay tap, something else happens: the shorter the delay and the closer to the volume of the dry signal, the more it’ll cancel out the bass. You’ll still get all the comb filtering effects, but you’ll also be removing lows, either the deep lows or even low midrange if the delay’s really short.If you blend two taps that are both inverted, you can cut bass while averaging out the comb-filter effects. If the taps are in phase (not inverted) what you’re doing is reinforcing the bass, because the cancellation effects will run out below a certain frequency and just add together. All this is using very quick delays, though ADT lets you lengthen them to where they’re slapbacks. Don’t be too distracted by that, part of what makes ADT its own effect is the ability to shape the tone with delays too quick to hear as echoes.Finally, now that you know you can cut bass using these very quick delays, or reinforce it, or any combination you like… there’s a headroom control. ADT will distort like crazy using low headroom. That can be used as a distortion effect… but it’s not just ordinary distortion, it’s a combination of Spiral into Console5’s buss (PurestConsole, for clarity and well-behavedness). These don’t perfectly cancel out. Instead, it produces a slightly leaned-out, skinnier tone to complement the way ADT fattens things up. If you’re using it to thin bass, it’ll be even more effective. If you’re reinforcing the body of the sound, it’ll color things in a subtle but interesting way. And of course if you love it, you can set up mixes that way (swap out Console5Channel for original Spiral, either on mix elements you’d like a little thinner and more energetic, or the whole thing). Sometimes there are new types of coloration that owe nothing to EQ or traditional processing: this is one of those times."
};
constexpr std::string_view k_tags{
    "ambience"
};

template <typename T>
class ADT final : public Effect<T>
{
    double pL[10000];
    double pR[10000];
    int gcount;
    double offsetA;
    double offsetB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;

  public:
    ADT()
    {
        A = 0.5; // 0.0 to 2.0 headroom
        B = 0.5; // delay time
        C = 0.5; //-1.0  to 1.0 inv/out
        D = 0.5; // delay time
        E = 0.5; //-1.0 to 1.0 inv/out
        F = 0.5; // 0.0 to 2.0 output level
        for (int count = 0; count < 9999; count++) {
            pL[count] = 0;
            pR[count] = 0;
        }
        offsetA = 9001;
        offsetB = 9001; // :D
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
        kParamF = 5,
        kNumParameters = 6
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
        kParamD:
            D = value;
            break;
        kParamE:
            E = value;
            break;
        kParamF:
            F = value;
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
        kParamD:
            return D;
            break;
        kParamE:
            return E;
            break;
        kParamF:
            return F;
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
        kParamD:
            return 0.5;
            break;
        kParamE:
            return 0.5;
            break;
        kParamF:
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
            return "headrm";
            break;
        kParamB:
            return "a delay";
            break;
        kParamC:
            return "a level";
            break;
        kParamD:
            return "b delay";
            break;
        kParamE:
            return "b level";
            break;
        kParamF:
            return "output";
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
            return "Headrm";
            break;
        kParamB:
            return "A Delay";
            break;
        kParamC:
            return "A Level";
            break;
        kParamD:
            return "B Delay";
            break;
        kParamE:
            return "B Level";
            break;
        kParamF:
            return "Output";
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
        kParamD:
            return std::to_string(D);
            break;
        kParamE:
            return std::to_string(E);
            break;
        kParamF:
            return std::to_string(F);
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
        kParamD:
            return "";
            break;
        kParamE:
            return "";
            break;
        kParamF:
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

        double gain = A * 1.272;
        double targetA = pow(B, 4) * 4790.0;
        double fractionA;
        double minusA;
        double intensityA = C - 0.5;
        // first delay
        double targetB = (pow(D, 4) * 4790.0);
        double fractionB;
        double minusB;
        double intensityB = E - 0.5;
        // second delay
        double output = F * 2.0;
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
            if (fabs(offsetA - targetA) > 1000) {
                offsetA = targetA;
            }
            offsetA = ((offsetA * 999.0) + targetA) / 1000.0;
            fractionA = offsetA - floor(offsetA);
            minusA = 1.0 - fractionA;
            if (fabs(offsetB - targetB) > 1000) {
                offsetB = targetB;
            }
            offsetB = ((offsetB * 999.0) + targetB) / 1000.0;
            fractionB = offsetB - floor(offsetB);
            minusB = 1.0 - fractionB;
            // chase delay taps for smooth action
            if (gain > 0) {
                inputSampleL /= gain;
                inputSampleR /= gain;
            }
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
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            // Spiral: lean out the sound a little when decoded by ConsoleBuss
            if (gcount < 1 || gcount > 4800) {
                gcount = 4800;
            }
            int count = gcount;
            double totalL = 0.0;
            double totalR = 0.0;
            double tempL;
            double tempR;
            pL[count + 4800] = pL[count] = inputSampleL;
            pR[count + 4800] = pR[count] = inputSampleR;
            // double buffer
            if (intensityA != 0.0)
            {
                count = (int)(gcount + floor(offsetA));
                tempL = (pL[count] * minusA); // less as value moves away from .0
                tempL += pL[count + 1]; // we can assume always using this in one way or another?
                tempL += (pL[count + 2] * fractionA); // greater as value moves away from .0
                tempL -= (((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
                totalL += (tempL * intensityA);
                tempR = (pR[count] * minusA); // less as value moves away from .0
                tempR += pR[count + 1]; // we can assume always using this in one way or another?
                tempR += (pR[count + 2] * fractionA); // greater as value moves away from .0
                tempR -= (((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
                totalR += (tempR * intensityA);
            }
            if (intensityB != 0.0)
            {
                count = (int)(gcount + floor(offsetB));
                tempL = (pL[count] * minusB); // less as value moves away from .0
                tempL += pL[count + 1]; // we can assume always using this in one way or another?
                tempL += (pL[count + 2] * fractionB); // greater as value moves away from .0
                tempL -= (((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
                totalL += (tempL * intensityB);
                tempR = (pR[count] * minusB); // less as value moves away from .0
                tempR += pR[count + 1]; // we can assume always using this in one way or another?
                tempR += (pR[count + 2] * fractionB); // greater as value moves away from .0
                tempR -= (((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
                totalR += (tempR * intensityB);
            }
            gcount--;
            // still scrolling through the samples, remember
            inputSampleL += totalL;
            inputSampleR += totalR;
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            inputSampleL *= gain;
            inputSampleR *= gain;
            if (output < 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
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
} // namespace airwindohhs::adt
