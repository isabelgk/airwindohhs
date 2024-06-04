#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::brassrider {

constexpr std::string_view k_name{ "BrassRider" };
constexpr std::string_view k_short_description{
    "BrassRider is One Weird Trick for drum overheads in metal!"
};
constexpr std::string_view k_long_description{
    "So this is pretty simple, and pretty distinctive. I made it for my friend Tim (one day I’ll repost his amazing rant, on the brink of his possible death, to musicians about ‘stop buying DAWs! Stop buying mic pres and plugins and becoming sound engineers! Learn to play your instrument!’) and it’s possible it has only one user, Tim. If so, it’s okay. This is a weird trick, a very specialized little toy. I do think it’s good at its strange purpose, though.BrassRider is a drum overhead rider for metal mixers.Here’s the situation. If you are doing a metal mix of extreme brutality, you are probably making very intense, sculpted spot-mic sounds. You probably have a mic on every tom, every drum, perhaps also on key cymbals. And you probably have overheads, because hey, overheads. Drums use overheads, right?And your overheads probably aren’t helping you AT ALL. You’re getting ‘jazz drummer’, ‘classic rock’ snare and tom sounds poking through from the overheads, and you hate them. Yet, there are some cymbal crashes that should get into the mix. What to do?BrassRider does one thing (one weird trick!) to help you. It watches for crashes (noise like white noise) and it cranks the volume WAY up when that goes beyond a threshold. You turn up the threshold control to start engaging this behavior. There’s a dry wet control that you might not even use if your mixes are truly brutal and heavy (who needs reality?)So then, BrassRider is cranking your overheads when the crashes are hit. So what you do is you bury the overhead mics in the mix, completely. And you use BrassRider to make them peek out only when there has to be a decent hint of crash cymbal in there. And most of the time, BrassRider totally kills the overheads so your drum sounds have maximum sculpted brutality and work the way you want them.I don’t sit around making metal mixes so I can’t demo this properly, and I made it so I’m not that concerned with doing the ultimate demo for people who don’t know what they’re doing and have to be sold on the tool. Tim would be disgusted with me if I went around trying to popularize this one and teach noobs how to use it. So, if all that confused you, this is not the plugin for you. Also, I’m not going to teach you how to use it in a metal mix (except maybe on Monday livestreams. ;) ) And if you know exactly what I’m talking about and you’re already dialing it in, a wild mad light in your eyes, well… you’re welcome :)"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class BrassRider final : public Effect<T>
{
    double d[80002];
    double e[80002];
    double highIIRL;
    double slewIIRL;
    double highIIR2L;
    double slewIIR2L;
    double highIIRR;
    double slewIIRR;
    double highIIR2R;
    double slewIIR2R;
    double control;
    double clamp;
    double lastSampleL;
    double lastSlewL;
    double lastSampleR;
    double lastSlewR;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    BrassRider()
    {
        A = 0.0;
        B = 0.0;
        for (int count = 0; count < 80001; count++) {
            d[count] = 0.0;
            e[count] = 0.0;
        }
        control = 0.0;
        clamp = 0.0;
        highIIRL = 0.0;
        slewIIRL = 0.0;
        highIIR2L = 0.0;
        slewIIR2L = 0.0;
        lastSampleL = 0.0;
        lastSlewL = 0.0;
        highIIRR = 0.0;
        slewIIRR = 0.0;
        highIIR2R = 0.0;
        slewIIR2R = 0.0;
        lastSampleR = 0.0;
        lastSlewR = 0.0;
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
        kNumParameters = 2

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0;
            case kParamB: return 0.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "thresh";
            case kParamB: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Thresh";
            case kParamB: return "Dry/Wet";

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
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double limitOut = A * 16;
        int offsetA = 13500;
        int offsetB = 16700;
        double wet = B;
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
            inputSampleL *= limitOut;
            highIIRL = (highIIRL * 0.5);
            highIIRL += (inputSampleL * 0.5);
            inputSampleL -= highIIRL;
            highIIR2L = (highIIR2L * 0.5);
            highIIR2L += (inputSampleL * 0.5);
            inputSampleL -= highIIR2L;
            double slewSampleL = fabs(inputSampleL - lastSampleL);
            lastSampleL = inputSampleL;
            slewSampleL /= fabs(inputSampleL * lastSampleL) + 0.2;
            slewIIRL = (slewIIRL * 0.5);
            slewIIRL += (slewSampleL * 0.5);
            slewSampleL = fabs(slewSampleL - slewIIRL);
            slewIIR2L = (slewIIR2L * 0.5);
            slewIIR2L += (slewSampleL * 0.5);
            slewSampleL = fabs(slewSampleL - slewIIR2L);
            double bridgerectifier = slewSampleL;
            // there's the left channel, now to feed it to overall clamp
            if (bridgerectifier > 3.1415) {
                bridgerectifier = 0.0;
            }
            bridgerectifier = sin(bridgerectifier);
            if (gcount < 0 || gcount > 40000) {
                gcount = 40000;
            }
            d[gcount + 40000] = d[gcount] = bridgerectifier;
            control += (d[gcount] / (offsetA + 1));
            control -= (d[gcount + offsetA] / offsetA);
            double ramp = (control * control) * 16.0;
            e[gcount + 40000] = e[gcount] = ramp;
            clamp += (e[gcount] / (offsetB + 1));
            clamp -= (e[gcount + offsetB] / offsetB);
            if (clamp > wet * 8) {
                clamp = wet * 8;
            }
            gcount--;
            inputSampleR *= limitOut;
            highIIRR = (highIIRR * 0.5);
            highIIRR += (inputSampleR * 0.5);
            inputSampleR -= highIIRR;
            highIIR2R = (highIIR2R * 0.5);
            highIIR2R += (inputSampleR * 0.5);
            inputSampleR -= highIIR2R;
            double slewSampleR = fabs(inputSampleR - lastSampleR);
            lastSampleR = inputSampleR;
            slewSampleR /= fabs(inputSampleR * lastSampleR) + 0.2;
            slewIIRR = (slewIIRR * 0.5);
            slewIIRR += (slewSampleR * 0.5);
            slewSampleR = fabs(slewSampleR - slewIIRR);
            slewIIR2R = (slewIIR2R * 0.5);
            slewIIR2R += (slewSampleR * 0.5);
            slewSampleR = fabs(slewSampleR - slewIIR2R);
            bridgerectifier = slewSampleR;
            // there's the right channel, now to feed it to overall clamp
            if (bridgerectifier > 3.1415) {
                bridgerectifier = 0.0;
            }
            bridgerectifier = sin(bridgerectifier);
            if (gcount < 0 || gcount > 40000) {
                gcount = 40000;
            }
            d[gcount + 40000] = d[gcount] = bridgerectifier;
            control += (d[gcount] / (offsetA + 1));
            control -= (d[gcount + offsetA] / offsetA);
            ramp = (control * control) * 16.0;
            e[gcount + 40000] = e[gcount] = ramp;
            clamp += (e[gcount] / (offsetB + 1));
            clamp -= (e[gcount + offsetB] / offsetB);
            if (clamp > wet * 8) {
                clamp = wet * 8;
            }
            gcount--;
            inputSampleL = (drySampleL * (1.0 - wet)) + (drySampleL * clamp * wet * 16.0);
            inputSampleR = (drySampleR * (1.0 - wet)) + (drySampleR * clamp * wet * 16.0);
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
} // namespace airwindohhs::brassrider
