#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::singleendedtriode {

constexpr std::string_view k_name{ "SingleEndedTriode" };
constexpr std::string_view k_short_description{
    "SingleEndedTriode is unusual analog modeling effects."
};
constexpr std::string_view k_long_description{
    "Everybody knows that analog modeling means distortion. (well… noise and distortion. And EQ, and overprocessing… but mostly distortion.)However, it’s always the same sorts of distortion: soft or hard clipping generating harmonics. Here, have three totally different kinds!Single Ended Triode does three things, and you don’t have to do them all at once (in fact you probably don’t want to).The actual Single Ended Triode control is a special gain-staged saturation that’s asymmetrical. It’s a little like PurestWarm, only not, because rather than put a soft saturation on one-half of the waveform, it offsets everything and goes into the saturation with a bias voltage. Then it subtracts a related voltage, and scales the whole thing up or down based on how much distortion you’re looking to get. (shown in the source code, of course)That means you have a ‘second harmonic generating’ asymmetrical distortion, but with NO crossover point. Unlike PurestWarm, SET is a continuous waveshaper just like using a real triode tube single-ended, and while you can crank it up to get obvious effects, its real magic is in using just a tiny amount to warm and sweeten things. It’s perhaps not ideal for the 2-buss because you’d simply be removing some of your mix energy on one half of the wave, but if the sweetening is what you need it might be worth it, because it’s a super clean way to do that. It’s only the asymmetrical distortion, and the interesting thing about that is: know how Spiral smooths the transition between sides of the sin() waveshaping, and that made it sound better? With Single Ended Triode, it’s capable of doing that transition when cranked way up… but used subtly, the entire audio output sits within one sin() calculation, and you don’t see a transition in the first place. This is literally why high end SET stereo rigs perform well for musicality and fluidness of sound: they have obvious faults but they’re great at avoiding crossover issues between push/pull sides of the circuit because they don’t have two sides to the circuit. It’s single-ended, and so is this algorithm unless you’re wildly distorting the heck out of it.Crossover issues, you say?Why yes. Meet Class AB (and Class B) Distortion.This is the opposite. It adds nonlinearities as the signal passes through zero. It’s a STRONG tone coloring and certainly not for the 2-buss or nice mellow music: the Class B is downright nasty and you should be careful about using it if you have delicate tweeters, as it’ll create extremely harsh treble grit (though, interestingly, without Gibb effect converter clipping: that is when reconstruction of the wave makes treble go past what bass can do, when it clips the converter on highs. This is a kind that could blow your tweeters at super high volumes but does not clip the converter doing it)The Class AB transitions through the middle of the wave in a more curvy way, causing the effect to lean towards the gritty upper-mids. Where might you find this kind of noise? Certainly not in any acoustic instrument. But… listen to certain old nasty tube Hammond organ sounds. Certain big guitar amps. Past a certain wattage, nearly every old tube amp is run push-pull (same with many transistor amps). They run hot, their calibrations drift… and one of the things that can happen to an amp is problems with that transition zone. Use Class B distortion and you’ll get very much the sound of purely transistor amps breaking down and going cold and gritty. Use Class AB (because with tubes, you’re probably going through output transformers and speakers that don’t have hi-fi tweeters) and you’ll get a bit of that gnarly rock-and-roll grit. There are expensive boutique stompboxes that can do this. Now you have it in a free, open-source plugin: open source devs, take note, because this one’s not often talked about or modeled. Most attention to amplifier crossover distortion has come from High End audiophile circles, and those guys have been getting mocked for decades. This stuff will not affect a frequency response plot. In certain systems of measurement you won’t even see it at all, and for years people have done naive measurement and claimed ‘it’s perfect, no further work needed!’ and the audiophiles were tearing their hair out, swearing that certain amps sounded like butt even if they measured ‘perfect’.Little did they know that in 2018, musicians would be turning to those same horrible distortions for creative purposes, sweetening with Single Ended Triode, adding grit and attitude with Class AB distortion, and being able to put a layer of really brittle edgy brightness onto the occasional sound with Class B.PS: some of you are having a lot of fun modeling existing hardware using elaborate combinations of Airwindows plugins. Just saying, these three sources of really specific coloration are exactly what you need to do that. Be careful of Class B as a little goes a long way (in fact, I would pick either Class AB or Class B but not both: study the circuit topology and only use AB and B where push/pull circuits actually exist, and remember they’re designed not to cause this kind of problem. AB contains overlap that stops the transition point from being exactly in the middle. Apply wisely.)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class SingleEndedTriode final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double postsine;
    float A;
    float B;
    float C;
    float D;

  public:
    SingleEndedTriode()
    {
        A = 0.0;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        postsine = sin(0.5);
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
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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
            case kParamD: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "triode"; break;
            case kParamB: return "clas ab"; break;
            case kParamC: return "clas b"; break;
            case kParamD: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Triode"; break;
            case kParamB: return "Clas AB"; break;
            case kParamC: return "Clas B"; break;
            case kParamD: return "Dry/Wet"; break;

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

        double intensity = pow(A, 2) * 8.0;
        double triode = intensity;
        intensity += 0.001;
        double softcrossover = pow(B, 3) / 8.0;
        double hardcrossover = pow(C, 7) / 8.0;
        double wet = D;
        // removed extra dry variable
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
            if (triode > 0.0)
            {
                inputSampleL *= intensity;
                inputSampleR *= intensity;
                inputSampleL -= 0.5;
                inputSampleR -= 0.5;
                double bridgerectifier = fabs(inputSampleL);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleL > 0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                if (bridgerectifier > 1.57079633) {
                    bridgerectifier = 1.57079633;
                }
                bridgerectifier = sin(bridgerectifier);
                if (inputSampleR > 0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
                inputSampleL += postsine;
                inputSampleR += postsine;
                inputSampleL /= intensity;
                inputSampleR /= intensity;
            }
            if (softcrossover > 0.0)
            {
                double bridgerectifier = fabs(inputSampleL);
                if (bridgerectifier > 0.0) {
                    bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
                }
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0;
                }
                if (inputSampleL > 0.0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                if (bridgerectifier > 0.0) {
                    bridgerectifier -= (softcrossover * (bridgerectifier + sqrt(bridgerectifier)));
                }
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0;
                }
                if (inputSampleR > 0.0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
            }
            if (hardcrossover > 0.0)
            {
                double bridgerectifier = fabs(inputSampleL);
                bridgerectifier -= hardcrossover;
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0.0;
                }
                if (inputSampleL > 0.0) {
                    inputSampleL = bridgerectifier;
                }
                else {
                    inputSampleL = -bridgerectifier;
                }
                bridgerectifier = fabs(inputSampleR);
                bridgerectifier -= hardcrossover;
                if (bridgerectifier < 0.0) {
                    bridgerectifier = 0.0;
                }
                if (inputSampleR > 0.0) {
                    inputSampleR = bridgerectifier;
                }
                else {
                    inputSampleR = -bridgerectifier;
                }
            }
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
} // namespace airwindohhs::singleendedtriode
