#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::distance {

constexpr std::string_view k_name{ "Distance" };
constexpr std::string_view k_short_description{
    "Distance3 combines the best parts of Distance and Discontinuity."
};
constexpr std::string_view k_long_description{
    "Here’s another utility plugin: Distance is specifically set up to mimic through-air high frequency attenuation. It’s from my initial wave of Airwindows plugins, come to VST and with a new twist: though in the video it’s a one-knobber, when you download it you’ll find that it’s got a Dry/Wet control, just to expand the things you can do with it. That’s new! I try to listen to people, even when it’s tempting to make it a super-dedicated one-trick pony.As you can see from how it behaves, Distance is a lot more complicated than just running a shelf. For that reason, I suggest this plugin for sound design and creative mixing purposes. Don’t try to use it for mastering or 2-buss, I feel it’s too intensely colored. However, for creative use it’s exactly what it says on the tin! Stick it on anything that’s supposed to ‘read’ sonically like it’s super far away, and you’ll be able to hear for miles and miles. Works on anything from pads to thunder to basses to reverb returns (I suggest using it on reverb returns rather than sends: it will be able to add thunder and size to the output of the reverb algorithm)As requested, this is the unholy hybrid of Distance and Atmosphere. This one doesn’t work like a Console5 system, it’s strictly ‘put it on and get a sound’, but I wasn’t expecting how cool it would be. Turns out this thing is completely absurdly good at taking tracks like drums, and making them huge and pounding and stage-like, without even the use of reverb or compression.The beginning of the video’s about using Distance2 as a loudenator, though I think using it on a full mix is overkill (maybe you want SOME elements to be up front and present). Bear in mind that you can keep the Atmosphere control set very low and still get an effect: the equivalent to the Atmosphere mixing system is to have it incredibly low, like 0.1 or less. The more you push it, the more nasty it’ll get, because that algorithm alone is NOT enough to make a distance sound. It’s not doing any of the high frequency attenuation you’d get, so technically the sound of high Atmosphere settings is the sound of extreme loudness rupturing the air and your eardrums: pushed hard, it’s unrecognizable as any natural sound. You wouldn’t survive exposure to a sound so loud that it broke the air like that.But when you also include the Darken control, that’s when things start sounding realistic again. This is one of those plugins where I could have built these into a single control to deliver good-sounding results no matter what setting you used… and where I chose to give you access to the wrongest possible settings because people NEED to break rules sometimes. Somebody out there is going to be able to get a great sound by taking the right source, and obliterating it with extreme Distance2 settings, and who am I to stand in the way? And you can also apply a dry/wet that will conceal the wreckage: surprisingly small amounts of dry signal will mask the amount of distortion going on.And the reason I’m able to put out a plugin where I KNOW that some people will set it wrongly and then hate it, is my Patreon. The thing about Patreon is that when it works, I’m completely protected from having to make things market friendly. Market friendly is a curse: it makes you do only predictable things that most people would like, and it punishes you if you want to do something unpopular, or if you want to take something great and widen the range until people get into trouble with it. It’s safer to give people presets that are known to behave on all source audio, that always sound nice. It’s safer to give people a pile of mulch than a chainsaw.Patreon lets me give you the chainsaw :D now, whether you do damage with it is your own affair. But I think once you strap it across some buss with drums or guitars or whatever, and fire it up, you’ll like chainsaws too. Just remember to dial it back when you need it not to be distractingly obnoxious. Or not: hey, it IS a distinct new distortion voice, with a whole new approach to slew clipping not previously available. Darken it or not, as it pleases you. Have fun.By request from my livestreams, let's jump right back into the Discontinuity thing, but this time combined with a much older plugin: the original Distance!This is a kind of plugin meant to darken the sound and make stuff sound really far away. Originally, I was thinking something that could take all the highs out and accentuate rumble, like turning a sound into the thunder version of itself. And so the first Distance worked basically as an EQ: three stacked stages of processing that combined to make stuff huge, kind of like my monitoring plugin SubsOnly.Thing is, that doesn't have any nonlinearities to speak of in it, not the kind that happen in the real world over that much air. And at the time I was working on a Console version called Atmosphere, and thought I had a handle on bringing in that kind of nonlinearity. And so the next one was Distance2… but it lost some of the purity and depth of Distance, but didn't sound quite the way I wanted. It was the best I could do at the time, and is still there if you're interested in different sorts of darken/distort.And then I brought in Discontinuity and was working on it in livestreams and someone mentioned, what if it was part of Distance? And the interesting thing is that Discontinuity also gets its sound from… three stages of processing, stacked. (as in, not side-by-side but in series, one after the other.)Anytime you look at a situation like that, you can think to yourself: well, I could run these two plugins on after the other, but what if I interleaved the stages? One of Distance, one of Discontinuity, another Distance, another Discontinuity, and so on? Surely that would combine the effects in a more interesting way, merge them into a new distinct thing as they work on each other in turn?And so here is Distance3. It goes right back to the tone quality of Distance, but it has all of the 'loud vibe' from Discontinuity, and outperforms either of them if you need the synthesis of both. There's probably lots of uses for this and my hope is that it'll be very easy to find those uses: if a thing has to be convincingly far away and you've already got reverb and ambience taken care of, Distance3 should immediately get you there in the best possible way."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Distance final : public Effect<T>
{
    double lastclampL;
    double clampL;
    double changeL;
    double thirdresultL;
    double prevresultL;
    double lastL;
    double lastclampR;
    double clampR;
    double changeR;
    double thirdresultR;
    double prevresultR;
    double lastR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Distance()
    {
        A = 0.0;
        B = 1.0;
        thirdresultL = prevresultL = lastclampL = clampL = changeL = lastL = 0.0;
        thirdresultR = prevresultR = lastclampR = clampR = changeR = lastR = 0.0;
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
            case kParamB: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "distance";
            case kParamB: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Distance";
            case kParamB: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A * 2.0);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "miles";
            case kParamB: return " ";
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
        double softslew = (pow(A * 2.0, 3.0) * 12.0) + 0.6;
        softslew *= overallscale;
        double filtercorrect = softslew / 2.0;
        double thirdfilter = softslew / 3.0;
        double levelcorrect = 1.0 + (softslew / 6.0);
        double postfilter;
        double wet = B;
        double dry = 1.0 - wet;
        double bridgerectifier;
        double inputSampleL;
        double inputSampleR;
        double drySampleL;
        double drySampleR;
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
            inputSampleL *= softslew;
            lastclampL = clampL;
            clampL = inputSampleL - lastL;
            postfilter = changeL = fabs(clampL - lastclampL);
            postfilter += filtercorrect;
            if (changeL > 1.5707963267949) {
                changeL = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeL));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleL = lastL + (clampL * bridgerectifier);
            lastL = inputSampleL;
            inputSampleL /= softslew;
            inputSampleL += (thirdresultL * thirdfilter);
            inputSampleL /= (thirdfilter + 1.0);
            inputSampleL += (prevresultL * postfilter);
            inputSampleL /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultL = prevresultL;
            prevresultL = inputSampleL;
            inputSampleL *= levelcorrect;
            inputSampleR *= softslew;
            lastclampR = clampR;
            clampR = inputSampleR - lastR;
            postfilter = changeR = fabs(clampR - lastclampR);
            postfilter += filtercorrect;
            if (changeR > 1.5707963267949) {
                changeR = 1.5707963267949;
            }
            bridgerectifier = (1.0 - sin(changeR));
            if (bridgerectifier < 0.0) {
                bridgerectifier = 0.0;
            }
            inputSampleR = lastR + (clampR * bridgerectifier);
            lastR = inputSampleR;
            inputSampleR /= softslew;
            inputSampleR += (thirdresultR * thirdfilter);
            inputSampleR /= (thirdfilter + 1.0);
            inputSampleR += (prevresultR * postfilter);
            inputSampleR /= (postfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdresultR = prevresultR;
            prevresultR = inputSampleR;
            inputSampleR *= levelcorrect;
            if (wet < 1.0) {
                inputSampleL = (drySampleL * dry) + (inputSampleL * wet);
                inputSampleR = (drySampleR * dry) + (inputSampleR * wet);
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
} // namespace airwindohhs::distance
