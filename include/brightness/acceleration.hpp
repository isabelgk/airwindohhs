#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::acceleration {

constexpr std::string_view k_name{ "Acceleration" };
constexpr std::string_view k_short_description{
    "Acceleration is an acceleration limiter that tames edge, leaves brightness."
};
constexpr std::string_view k_long_description{
    "As requested, I’ve made a dedicated acceleration limiter, like the one in ToVinyl4. You don’t have to mess with Groove Wear, or highpass anything: as a result, this ends up being the ‘high end’ acceleration limiter, mastering grade and useful in situations where ToVinyl might be overkill (ToVinyl was made in an era where I still had to make ‘amazing marketable plugins with lots of features so people would pay $50 quickly before it got all pirated etc’)Ah, I remember those days ;PBut now, it’s different because Acceleration is free! AU, Mac and PC VST (stay tuned for a surprise on that front) and if you can’t pay, go ahead and use it anyway. If you can, please do because I think the usefulness of me doing this work will become increasingly obvious, and the usefulness of me opensourcing plugin code has also become increasingly obvious. And it’s brought you Acceleration, here and now.The reason to be excited about that is, Acceleration’s not a simple filter. It pretty much targets exactly the digital behavior that creates Gibb effect overshoots (the reason you often have to pad heavily limited material down to stop it being crackly and glare-y) and would probably work very well as literally an acceleration limiter for record cutting heads. But, in a largely digital world, it might be even more useful because you can feed it any old ITB mix, any weird edgy source material, and it’ll clean up the nasty digital brightness while not sounding like it’s doing anything. Check the video (it should be relatively obvious if you’ve got half decent tweeters) and try it out. The key is to not try and get the apparent volume of the highs to be less: instead, you’re going after the character, or the presentation. If highs seem to be coming forward with nasty spikey extra energy, Acceleration will fix that tonal problem without altering the basic mix balance."
};
constexpr std::string_view k_tags{
    "brightness"
};

template <typename T>
class Acceleration final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double ataLastOutL;
    double s1L;
    double s2L;
    double s3L;
    double o1L;
    double o2L;
    double o3L;
    double m1L;
    double m2L;
    double desL;
    double ataLastOutR;
    double s1R;
    double s2R;
    double s3R;
    double o1R;
    double o2R;
    double o3R;
    double m1R;
    double m2R;
    double desR;
    float A;
    float B;

  public:
    Acceleration()
    {
        A = 0.32;
        B = 1.0;
        ataLastOutL = 0.0;
        s1L = s2L = s3L = 0.0;
        o1L = o2L = o3L = 0.0;
        m1L = m2L = desL = 0.0;
        ataLastOutR = 0.0;
        s1R = s2R = s3R = 0.0;
        o1R = o2R = o3R = 0.0;
        m1R = m2R = desR = 0.0;
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
            case kParamA: return A; break;
            case kParamB: return B; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.32; break;
            case kParamB: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "limit"; break;
            case kParamB: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Limit"; break;
            case kParamB: return "Dry/Wet"; break;

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
        double intensity = pow(A, 3) * (32 / overallscale);
        double wet = B;
        // removed extra dry variable
        double senseL;
        double smoothL;
        double senseR;
        double smoothR;
        double accumulatorSample;
        double drySampleL;
        double drySampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            s3L = s2L;
            s2L = s1L;
            s1L = inputSampleL;
            smoothL = (s3L + s2L + s1L) / 3.0;
            m1L = (s1L - s2L) * ((s1L - s2L) / 1.3);
            m2L = (s2L - s3L) * ((s1L - s2L) / 1.3);
            senseL = fabs(m1L - m2L);
            senseL = (intensity * intensity * senseL);
            o3L = o2L;
            o2L = o1L;
            o1L = senseL;
            if (o2L > senseL) {
                senseL = o2L;
            }
            if (o3L > senseL) {
                senseL = o3L;
            }
            // sense on the most intense
            s3R = s2R;
            s2R = s1R;
            s1R = inputSampleR;
            smoothR = (s3R + s2R + s1R) / 3.0;
            m1R = (s1R - s2R) * ((s1R - s2R) / 1.3);
            m2R = (s2R - s3R) * ((s1R - s2R) / 1.3);
            senseR = fabs(m1R - m2R);
            senseR = (intensity * intensity * senseR);
            o3R = o2R;
            o2R = o1R;
            o1R = senseR;
            if (o2R > senseR) {
                senseR = o2R;
            }
            if (o3R > senseR) {
                senseR = o3R;
            }
            // sense on the most intense
            if (senseL > 1.0) {
                senseL = 1.0;
            }
            if (senseR > 1.0) {
                senseR = 1.0;
            }
            inputSampleL *= (1.0 - senseL);
            inputSampleR *= (1.0 - senseR);
            inputSampleL += (smoothL * senseL);
            inputSampleR += (smoothR * senseR);
            senseL /= 2.0;
            senseR /= 2.0;
            accumulatorSample = (ataLastOutL * senseL) + (inputSampleL * (1.0 - senseL));
            ataLastOutL = inputSampleL;
            inputSampleL = accumulatorSample;
            accumulatorSample = (ataLastOutR * senseR) + (inputSampleR * (1.0 - senseR));
            ataLastOutR = inputSampleR;
            inputSampleR = accumulatorSample;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs::acceleration
