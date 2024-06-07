#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::peaksonly {

constexpr std::string_view k_name{ "PeaksOnly" };
constexpr std::string_view k_short_description{
    "PeaksOnly is a transformative mix check tool."
};
constexpr std::string_view k_long_description{
    "PeaksOnly is a mix check, like SubsOnly and SlewOnly before it, or Silhouette and other even stranger plugins. You pretty much need to not master through this one :D it totally wrecks the sound. BUT, it telegraphs information you might never have had, makes it more obvious than it’s ever been before.Here’s how it works. PeaksOnly runs a little pile of allpass filters, but rather than just make them nice-sounding, it expands and expands and expands them, each time phase-rotating things a little more. It turns transient attacks of any kind into a little colorful wash, a flag of energy that stays at roughly the same level, but gets smeared out across time in a way you’d never normally hear.Why does this matter? Because you wouldn’t hear it. Especially if you’re trying to work on headphones, brief sharp transients can become almost totally invisible, especially if they’re being peak-limited or get blended in with other sounds. That stuff can make your mix come alive, but how do you balance it? And if you’re on headphones, forget it: some peaks are just too brief, and unless you have a strangely powerful sense of energy levels that are normally invisible, there will be no managing the stuff that you simply can’t hear, the spiky intense sparks of audio like you get from passionate performances or tricks like using compression to spike up attack transients.But with PeaksOnly, everything stays at exactly the same frequency balance (a trick of allpasses, especially mine which are prime-number spaced) but the bursts of energy, no matter how brief, get transformed into recognizable signals. If you’re short on transient impact, you’ll notice it. If you’re over-squashing attacks, you’ll end up with audio porridge. But if you’ve got a powerful, kicking, lively mix… every detail of it will be laid bare, turned into a sort of pantomime that exposes every transient at every frequency. Whether it’s how loud to make that snare or hi-hat, or how much sub-kick you can get away with, it’s all exposed. PeaksOnly is particularly fond of taking excess subsonic peaks and distorting ’em: you’ll never pack too much into the subs again.It also tries to keep you at a sane loudness level (suitable for sending to mastering, if you really think you need to loudenate). Strangely, if you feed ‘mastered’ loud stuff into PeaksOnly and tolerate the distortion, it still reveals everything: there’s a difference between good ‘too loud’ and bad ‘too loud’, and PeaksOnly exposes that too. Anything out of balance, even in loudness land, will stick out."
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class PeaksOnly final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double aL[1503];
    double bL[1503];
    double cL[1503];
    double dL[1503];
    double aR[1503];
    double bR[1503];
    double cR[1503];
    double dR[1503];
    int ax, bx, cx, dx;

  public:
    PeaksOnly()
    {
        for (int count = 0; count < 1502; count++) {
            aL[count] = 0.0;
            bL[count] = 0.0;
            cL[count] = 0.0;
            dL[count] = 0.0;
            aR[count] = 0.0;
            bR[count] = 0.0;
            cR[count] = 0.0;
            dR[count] = 0.0;
        }
        ax = 1;
        bx = 1;
        cx = 1;
        dx = 1;
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
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
        int am = (int)149.0 * overallscale;
        int bm = (int)179.0 * overallscale;
        int cm = (int)191.0 * overallscale;
        int dm = (int)223.0 * overallscale; // these are 'good' primes, spacing out the allpasses
        int allpasstemp = 0;
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
            allpasstemp = ax - 1;
            if (allpasstemp < 0 || allpasstemp > am) {
                allpasstemp = am;
            }
            inputSampleL -= aL[allpasstemp] * 0.5;
            inputSampleR -= aR[allpasstemp] * 0.5;
            aL[ax] = inputSampleL;
            aR[ax] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            ax--;
            if (ax < 0 || ax > am) {
                ax = am;
            }
            inputSampleL += (aL[ax]);
            inputSampleR += (aR[ax]);
            // a single Midiverb-style allpass
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
            allpasstemp = bx - 1;
            if (allpasstemp < 0 || allpasstemp > bm) {
                allpasstemp = bm;
            }
            inputSampleL -= bL[allpasstemp] * 0.5;
            inputSampleR -= bR[allpasstemp] * 0.5;
            bL[bx] = inputSampleL;
            bR[bx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            bx--;
            if (bx < 0 || bx > bm) {
                bx = bm;
            }
            inputSampleL += (bL[bx]);
            inputSampleR += (bR[bx]);
            // a single Midiverb-style allpass
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
            allpasstemp = cx - 1;
            if (allpasstemp < 0 || allpasstemp > cm) {
                allpasstemp = cm;
            }
            inputSampleL -= cL[allpasstemp] * 0.5;
            inputSampleR -= cR[allpasstemp] * 0.5;
            cL[cx] = inputSampleL;
            cR[cx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            cx--;
            if (cx < 0 || cx > cm) {
                cx = cm;
            }
            inputSampleL += (cL[cx]);
            inputSampleR += (cR[cx]);
            // a single Midiverb-style allpass
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
            allpasstemp = dx - 1;
            if (allpasstemp < 0 || allpasstemp > dm) {
                allpasstemp = dm;
            }
            inputSampleL -= dL[allpasstemp] * 0.5;
            inputSampleR -= dR[allpasstemp] * 0.5;
            dL[dx] = inputSampleL;
            dR[dx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            dx--;
            if (dx < 0 || dx > dm) {
                dx = dm;
            }
            inputSampleL += (dL[dx]);
            inputSampleR += (dR[dx]);
            // a single Midiverb-style allpass
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
            inputSampleL *= 0.63679; // scale it to 0dB output at full blast
            inputSampleR *= 0.63679; // scale it to 0dB output at full blast
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
} // namespace airwindohhs::peaksonly
