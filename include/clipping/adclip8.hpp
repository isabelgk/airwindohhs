#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::adclip8 {

constexpr std::string_view k_name{ "ADClip8" };
constexpr std::string_view k_short_description{
    "ADClip8 is the ultimate Airwindows loudenator/biggenator."
};
constexpr std::string_view k_long_description{
    "The last time we saw an Airwindows ADClip, the full version rather than a neat and tidy stripped-down version, was October 8… of 2017. It's been more than six years since the king of loudenators saw an update, in part because ClipOnly got followed by ClipOnly2, which was such a good high-sample-rate clipper that it was all I needed for years. ClipOnly2 is still the cleanest, purest way to take small amounts of peak clipping to your signal: it's great as a safety clip.But, while working on a new version tailored to the needs of Final Cut Pro (which is also coming), something new arose.What would happen if the FCP version, which has a built-in slew clip and makes heavy use of Golden Ratios to arrive at its results, was adapted to a new ADClip? What if it was scaled to regular clipping levels, given a ceiling control to dial back its maximum to taste, what would happen if it was stacked up with a whole array of FinalClips in series like some of the other Airwindows plugins like to do? We're no longer dealing with ClipOnly2. Much like the original ADClip7, we're adding extra stuff and running a more complicated algorithm. But we've still got some of the special features of ADClip7, like the gain-matched mode and the Clips Only setting.Enter ADClip8. It has more settings. They go Normal, Gain Matched, Clip Only… then, Afterburner, Explode, Nuke, Apocalypse, and lastly Apotheosis. No, really! That's the new settings.What they represent is added stages, so Normal is a single stage (as is Gain Matched), Clip Only shows you where clips begin, and then the new modes are named for the sounds they produce, at two, three, four, five and six stages of special slew-clipped ClipOnly2 style sample-rate aware clipping. In practice, think 'bigger, bassier, thicker' as you continue to add stages. You may not always want to run the full six stages, you might want to voice it to your needs.The interesting part is how all this affects intersample peaking. Because the process brings in slew clipping and repeated use of the ClipOnly2 algorithm, which resists harsh entry into and exit from a clipped state, dialing up ADClip8 and cranking the gain produces an intensely saturated sound that resists intersample peaking not through filtration, but mechanically. It will literally put in transition samples to resist the intersample peaks from going beyond regular clipping. The more stages, the more thoroughly it resists 'true peak'.I would say don't use this for evil but who am I kidding. If you beat everyone with 'competitive loudness' this way, please never expect me to listen to what you did, I'll be hiding under my bed, regretting everything. :)For what it's worth, 'Apotheosis' mode should also be a very nice way to do extremely moderate transparent clipping of challenging material: ClipOnly2 will still be more transparent but only sounds like a slightly brighter version of ADClip8 Normal mode. Apotheosis mode simply does more and I wouldn't blame anybody for preferring it. Hope you like ADClip8!"
};
constexpr std::string_view k_tags{
    "clipping"
};

template <typename T>
class ADClip8 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleL[8];
    double intermediateL[16][8];
    bool wasPosClipL[8];
    bool wasNegClipL[8];
    double lastSampleR[8];
    double intermediateR[16][8];
    bool wasPosClipR[8];
    bool wasNegClipR[8];
    float A;
    float B;
    float C;

  public:
    ADClip8()
    {
        A = 0.0;
        B = 0.883;
        C = 0.0;
        for (int stage = 0; stage < 8; stage++) {
            lastSampleL[stage] = 0.0;
            lastSampleR[stage] = 0.0;
            wasPosClipL[stage] = false;
            wasPosClipR[stage] = false;
            wasNegClipL[stage] = false;
            wasNegClipR[stage] = false;
            for (int x = 0; x < 16; x++) {
                intermediateL[x][stage] = 0.0;
                intermediateR[x][stage] = 0.0;
            }
        }
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
            return 0.0;
            break;
        kParamB:
            return 0.883;
            break;
        kParamC:
            return 0.0;
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
        kParamB:
            return "ceiling";
            break;
        kParamC:
            return "mode";
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
        kParamB:
            return "Ceiling";
            break;
        kParamC:
            return "Mode";
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
            return "dB";
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
        overallscale *= Effect<T>::getSampleRate();
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        // double hardness = 0.618033988749894; // golden ratio
        // double softness = 0.381966011250105; // 1.0 - hardness
        // double refclip = 1.618033988749894; // -0.2dB we're making all this pure raw code
        // refclip*hardness = 1.0  to use ClipOnly as a prefab code-chunk.
        // refclip*softness = 0.618033988749894	Seven decimal places is plenty as it's
        // not related to the original sound much: it's an arbitrary position in softening.
        double inputGain = pow(10.0, (A * 18.0) / 20.0);
        double ceiling = (1.0 + (B * 0.23594733)) * 0.5;
        int mode = (int)(C * 7.999) + 1;
        int stageSetting = mode - 2;
        if (stageSetting < 1) {
            stageSetting = 1;
        }
        inputGain = ((inputGain - 1.0) / stageSetting) + 1.0;
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
            double overshootL = 0.0;
            double overshootR = 0.0;
            inputSampleL *= 1.618033988749894;
            inputSampleR *= 1.618033988749894;
            for (int stage = 0; stage < stageSetting; stage++) {
                if (inputGain != 1.0) {
                    inputSampleL *= inputGain;
                    inputSampleR *= inputGain;
                }
                if (stage == 0) {
                    overshootL = fabs(inputSampleL) - 1.618033988749894;
                    if (overshootL < 0.0) {
                        overshootL = 0.0;
                    }
                    overshootR = fabs(inputSampleR) - 1.618033988749894;
                    if (overshootR < 0.0) {
                        overshootR = 0.0;
                    }
                }
                if (inputSampleL > 4.0) {
                    inputSampleL = 4.0;
                }
                if (inputSampleL < -4.0) {
                    inputSampleL = -4.0;
                }
                if (inputSampleL - lastSampleL[stage] > 0.618033988749894) {
                    inputSampleL = lastSampleL[stage] + 0.618033988749894;
                }
                if (inputSampleL - lastSampleL[stage] < -0.618033988749894) {
                    inputSampleL = lastSampleL[stage] - 0.618033988749894;
                }
                if (inputSampleR > 4.0) {
                    inputSampleR = 4.0;
                }
                if (inputSampleR < -4.0) {
                    inputSampleR = -4.0;
                }
                if (inputSampleR - lastSampleR[stage] > 0.618033988749894) {
                    inputSampleR = lastSampleR[stage] + 0.618033988749894;
                }
                if (inputSampleR - lastSampleR[stage] < -0.618033988749894) {
                    inputSampleR = lastSampleR[stage] - 0.618033988749894;
                }
                // same as slew clippage
                // begin ClipOnly2 as a little, compressed chunk that can be dropped into code
                if (wasPosClipL[stage] == true) { // current will be over
                    if (inputSampleL < lastSampleL[stage]) {
                        lastSampleL[stage] = 1.0 + (inputSampleL * 0.381966011250105);
                    }
                    else {
                        lastSampleL[stage] = 0.618033988749894 + (lastSampleL[stage] * 0.618033988749894);
                    }
                }
                wasPosClipL[stage] = false;
                if (inputSampleL > 1.618033988749894) {
                    wasPosClipL[stage] = true;
                    inputSampleL = 1.0 + (lastSampleL[stage] * 0.381966011250105);
                }
                if (wasNegClipL[stage] == true) { // current will be -over
                    if (inputSampleL > lastSampleL[stage]) {
                        lastSampleL[stage] = -1.0 + (inputSampleL * 0.381966011250105);
                    }
                    else {
                        lastSampleL[stage] = -0.618033988749894 + (lastSampleL[stage] * 0.618033988749894);
                    }
                }
                wasNegClipL[stage] = false;
                if (inputSampleL < -1.618033988749894) {
                    wasNegClipL[stage] = true;
                    inputSampleL = -1.0 + (lastSampleL[stage] * 0.381966011250105);
                }
                intermediateL[spacing][stage] = inputSampleL;
                inputSampleL = lastSampleL[stage]; // Latency is however many samples equals one 44.1k sample
                for (int x = spacing; x > 0; x--) {
                    intermediateL[x - 1][stage] = intermediateL[x][stage];
                }
                lastSampleL[stage] = intermediateL[0][stage]; // run a little buffer to handle this
                // end ClipOnly2 as a little, compressed chunk that can be dropped into code
                // begin ClipOnly2 as a little, compressed chunk that can be dropped into code
                if (wasPosClipR[stage] == true) { // current will be over
                    if (inputSampleR < lastSampleR[stage]) {
                        lastSampleR[stage] = 1.0 + (inputSampleR * 0.381966011250105);
                    }
                    else {
                        lastSampleR[stage] = 0.618033988749894 + (lastSampleR[stage] * 0.618033988749894);
                    }
                }
                wasPosClipR[stage] = false;
                if (inputSampleR > 1.618033988749894) {
                    wasPosClipR[stage] = true;
                    inputSampleR = 1.0 + (lastSampleR[stage] * 0.381966011250105);
                }
                if (wasNegClipR[stage] == true) { // current will be -over
                    if (inputSampleR > lastSampleR[stage]) {
                        lastSampleR[stage] = -1.0 + (inputSampleR * 0.381966011250105);
                    }
                    else {
                        lastSampleR[stage] = -0.618033988749894 + (lastSampleR[stage] * 0.618033988749894);
                    }
                }
                wasNegClipR[stage] = false;
                if (inputSampleR < -1.618033988749894) {
                    wasNegClipR[stage] = true;
                    inputSampleR = -1.0 + (lastSampleR[stage] * 0.381966011250105);
                }
                intermediateR[spacing][stage] = inputSampleR;
                inputSampleR = lastSampleR[stage]; // Latency is however many samples equals one 44.1k sample
                for (int x = spacing; x > 0; x--) {
                    intermediateR[x - 1][stage] = intermediateR[x][stage];
                }
                lastSampleR[stage] = intermediateR[0][stage]; // run a little buffer to handle this
                // end ClipOnly2 as a little, compressed chunk that can be dropped into code
            }
            switch (mode)
            {
                case 1: break; // Normal
                case 2:
                    inputSampleL /= inputGain;
                    inputSampleR /= inputGain;
                    break; // Gain Match
                case 3:
                    inputSampleL = overshootL;
                    inputSampleR = overshootR;
                    break; // Clip Only
                case 4: break; // Afterburner
                case 5: break; // Explode
                case 6: break; // Nuke
                case 7: break; // Apocalypse
                case 8: break; // Apotheosis
            }
            // this is our output mode switch, showing the effects
            inputSampleL *= ceiling;
            inputSampleR *= ceiling;
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
} // namespace airwindohhs::adclip8
