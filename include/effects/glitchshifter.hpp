#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::glitchshifter {

constexpr std::string_view k_name{ "GlitchShifter" };
constexpr std::string_view k_short_description{
    "GlitchShifter is a really gnarly, raw-sounding pitch shifter with a dose of insanity!"
};
constexpr std::string_view k_long_description{
    "And then some days it’s NOT BORING… :DThis is one of the craziest secret weapons I’ve ever done. In fact, I’ve got plans for refining this one and making it do subtler things that are of use in pop mixing… but right now, this is Glitch Shifter! It is an audio monster, and it’s all yours.You can do equal-temperament pitch shifting, unquantized, or both. You can tell it to feed back, or keep it as a tightly tracked subtle dry/wet blend. But that’s just the start… that’s the top two sliders, the top one being Note (in semitones) and the bottom being trim (unquantized: if you’re on VST and can’t reset it to its default easily, be careful not to change it unless you want detuned effects). That gives you the base pitch shifting, like any pitch shifter plugin. Its tone comes from the algorithm: instead of smoothing the transitions, it always tries to find a spot where it can switch inside the position of its buffer in just a sample or two, seamlessly. That gives it a distinctive raw tone, less processed, but still essentially a pitch shifter plugin.Except this is NOT like other pitch shifter plugins, because it’s got that Tightness control, and that takes Glitch Shifter into full glitch in two different ways. If you’re trying to do a ‘nice’ pitch shift you’ll be wanting to tune this to your underlying track, but if you want to create sonic mayhem here is how you do it.Turning up Tightness all the way shortens the buffer zone in which the plugin finds its transitions. It makes the pitch shifted sound track the underlying sound more tightly… or MUCH more tightly… or so tightly that it glitches out and turns into a harsh de-rezzing bitscrunch sound, because the buffer’s not nearly long enough to contain a seamless loop of the underlying sound. Back off the Tightness, to get back to a ‘nice’ pitch shift, or turn it up for tightly tracked, robotic, nasty artifacts. All the way up and the pitch shifting is totally defeated.Turning DOWN Tightness has yet another effect. Since the larger buffer occupies more time, it’s easier and easier for the pitch shifter to find spots to seamlessly transition, so you can quickly get smooth legato effects without grind, and the lower the Tightness the smoother the pitch shift effect. Except, it’s not that simple. As the buffer size expands ever outward, the pitch shifter loses track of where it is. It decouples, unhooks from the underlying sound, and begins to delay and sample-chop the audio randomly. Not really randomly: it’s finding the most seamless transitions. But it starts acting like a granular effect… except it is NOT a granular effect, because those fade their grains in and out (typically) and GlitchShifter works entirely by splicing audio WITHOUT fades to smooth things. So when you drop Tightness super low, you get an uncontrollable pitch-delay thing going on. And then, if you add feedback, you’ve got many layers of stacked pitch-delay going on, unpredictably…"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class GlitchShifter final : public Effect<T>
{
    VstInt32 pL[131076];
    VstInt32 offsetL[258];
    VstInt32 pastzeroL[258];
    VstInt32 previousL[258];
    VstInt32 thirdL[258];
    VstInt32 fourthL[258];
    VstInt32 tempL;
    VstInt32 lasttempL;
    VstInt32 thirdtempL;
    VstInt32 fourthtempL;
    VstInt32 sincezerocrossL;
    int crossesL;
    int realzeroesL;
    double positionL;
    bool splicingL;
    double airPrevL;
    double airEvenL;
    double airOddL;
    double airFactorL;
    VstInt32 pR[131076];
    VstInt32 offsetR[258];
    VstInt32 pastzeroR[258];
    VstInt32 previousR[258];
    VstInt32 thirdR[258];
    VstInt32 fourthR[258];
    VstInt32 tempR;
    VstInt32 lasttempR;
    VstInt32 thirdtempR;
    VstInt32 fourthtempR;
    VstInt32 sincezerocrossR;
    int crossesR;
    int realzeroesR;
    double positionR;
    bool splicingR;
    double airPrevR;
    double airEvenR;
    double airOddR;
    double airFactorR;
    int gcount;
    VstInt32 lastwidth;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    GlitchShifter()
    {
        for (int count = 0; count < 131074; count++) {
            pL[count] = 0;
            pR[count] = 0;
        }
        for (int count = 0; count < 257; count++) {
            offsetL[count] = 0;
            pastzeroL[count] = 0;
            previousL[count] = 0;
            thirdL[count] = 0;
            fourthL[count] = 0;
            offsetR[count] = 0;
            pastzeroR[count] = 0;
            previousR[count] = 0;
            thirdR[count] = 0;
            fourthR[count] = 0;
        }
        crossesL = 0;
        realzeroesL = 0;
        tempL = 0;
        lasttempL = 0;
        thirdtempL = 0;
        fourthtempL = 0;
        sincezerocrossL = 0;
        airPrevL = 0.0;
        airEvenL = 0.0;
        airOddL = 0.0;
        airFactorL = 0.0;
        positionL = 0.0;
        splicingL = false;
        crossesR = 0;
        realzeroesR = 0;
        tempR = 0;
        lasttempR = 0;
        thirdtempR = 0;
        fourthtempR = 0;
        sincezerocrossR = 0;
        airPrevR = 0.0;
        airEvenR = 0.0;
        airOddR = 0.0;
        airFactorR = 0.0;
        positionR = 0.0;
        splicingR = false;
        gcount = 0;
        lastwidth = 16386;
        flip = false;
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        E = 0.5;
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
            return 0.0;
            break;
        kParamE:
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
            return "note";
            break;
        kParamB:
            return "trim";
            break;
        kParamC:
            return "tighten";
            break;
        kParamD:
            return "feedbck";
            break;
        kParamE:
            return "dry/wet";
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
            return "Note";
            break;
        kParamB:
            return "Trim";
            break;
        kParamC:
            return "Tighten";
            break;
        kParamD:
            return "Feedbck";
            break;
        kParamE:
            return "Dry/Wet";
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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "semi";
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

        int note = (int)(A * 24.999) - 12;
        double trim = (B * 2.0) - 1.0;
        double speed = ((1.0 / 12.0) * note) + trim;
        if (speed < 0) {
            speed *= 0.5;
        }
        // include sanity check- maximum pitch lowering cannot be to 0 hz.
        int width = (int)(65536 - ((1 - pow(1 - C, 2)) * 65530.0));
        double bias = pow(C, 3);
        double feedback = D / 1.5;
        double wet = E;
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
            airFactorL = airPrevL - inputSampleL;
            if (flip) {
                airEvenL += airFactorL;
                airOddL -= airFactorL;
                airFactorL = airEvenL;
            }
            else {
                airOddL += airFactorL;
                airEvenL -= airFactorL;
                airFactorL = airOddL;
            }
            airOddL = (airOddL - ((airOddL - airEvenL) / 256.0)) / 1.0001;
            airEvenL = (airEvenL - ((airEvenL - airOddL) / 256.0)) / 1.0001;
            airPrevL = inputSampleL;
            inputSampleL += airFactorL;
            airFactorR = airPrevR - inputSampleR;
            if (flip) {
                airEvenR += airFactorR;
                airOddR -= airFactorR;
                airFactorR = airEvenR;
            }
            else {
                airOddR += airFactorR;
                airEvenR -= airFactorR;
                airFactorR = airOddR;
            }
            airOddR = (airOddR - ((airOddR - airEvenR) / 256.0)) / 1.0001;
            airEvenR = (airEvenR - ((airEvenR - airOddR) / 256.0)) / 1.0001;
            airPrevR = inputSampleR;
            inputSampleR += airFactorR;
            flip = !flip;
            // air, compensates for loss of highs of interpolation
            if (lastwidth != width) {
                crossesL = 0;
                realzeroesL = 0;
                crossesR = 0;
                realzeroesR = 0;
                lastwidth = width;
            }
            // global: changing this resets both channels
            gcount++;
            if (gcount < 0 || gcount > width) {
                gcount = 0;
            }
            int count = gcount;
            int countone = count - 1;
            int counttwo = count - 2;
            while (count < 0) {
                count += width;
            }
            while (countone < 0) {
                countone += width;
            }
            while (counttwo < 0) {
                counttwo += width;
            }
            while (count > width) {
                count -= width;
            } // this can only happen with very insane variables
            while (countone > width) {
                countone -= width;
            }
            while (counttwo > width) {
                counttwo -= width;
            }
            // yay sanity checks
            // now we have counts zero, one, two, none of which have sanity checked values
            // we are tracking most recent samples and must SUBTRACT.
            // this is a wrap on the overall buffers, so count, one and two are also common to both channels
            pL[count + width] = pL[count] = (int)((inputSampleL * 8388352.0) + (double)(lasttempL * feedback));
            pR[count + width] = pR[count] = (int)((inputSampleR * 8388352.0) + (double)(lasttempR * feedback));
            // double buffer -8388352 to 8388352 is equal to 24 bit linear space
            if ((pL[countone] > 0 && pL[count] < 0) || (pL[countone] < 0 && pL[count] > 0)) // source crossed zero
            {
                crossesL++;
                realzeroesL++;
                if (crossesL > 256) {
                    crossesL = 0;
                } // wrap crosses to keep adding new crosses
                if (realzeroesL > 256) {
                    realzeroesL = 256;
                } // don't wrap realzeroes, full buffer, use all
                offsetL[crossesL] = count;
                pastzeroL[crossesL] = pL[count];
                previousL[crossesL] = pL[countone];
                thirdL[crossesL] = pL[counttwo];
                // we load the zero crosses register with crosses to examine
            } // we just put in a source zero cross in the registry
            if ((pR[countone] > 0 && pR[count] < 0) || (pR[countone] < 0 && pR[count] > 0)) // source crossed zero
            {
                crossesR++;
                realzeroesR++;
                if (crossesR > 256) {
                    crossesR = 0;
                } // wrap crosses to keep adding new crosses
                if (realzeroesR > 256) {
                    realzeroesR = 256;
                } // don't wrap realzeroes, full buffer, use all
                offsetR[crossesR] = count;
                pastzeroR[crossesR] = pR[count];
                previousR[crossesR] = pR[countone];
                thirdR[crossesR] = pR[counttwo];
                // we load the zero crosses register with crosses to examine
            } // we just put in a source zero cross in the registry
            // in this we don't update count at all, so we can run them one after another because this is
            // feeding the system, not tracking the output of two parallel but non-matching output taps
            positionL -= speed; // this is individual to each channel!
            if (positionL > width) { // we just caught up to the buffer end
                if (realzeroesL > 0) { // we just caught up to the buffer end with zero crosses in the bin
                    positionL = 0;
                    double diff = 99999999.0;
                    int best = 0; // these can be local, I think
                    int scan;
                    for (scan = (realzeroesL - 1); scan >= 0; scan--) {
                        int scanone = scan + crossesL;
                        if (scanone > 256) {
                            scanone -= 256;
                        }
                        // try to track the real most recent ones more closely
                        double howdiff = (double)((tempL - pastzeroL[scanone]) + (lasttempL - previousL[scanone]) + (thirdtempL - thirdL[scanone]) + (fourthtempL - fourthL[scanone]));
                        // got difference factor between things
                        howdiff -= (double)(scan * bias);
                        // try to bias in favor of more recent crosses
                        if (howdiff < diff) {
                            diff = howdiff;
                            best = scanone;
                        }
                    } // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
                    positionL = offsetL[best] - sincezerocrossL;
                    crossesL = 0;
                    realzeroesL = 0;
                    splicingL = true; // we just kicked the delay tap back, changing positionL
                }
                else { // we just caught up to the buffer end with no crosses- glitch speeds.
                    positionL -= width;
                    crossesL = 0;
                    realzeroesL = 0;
                    splicingL = true; // so, hard splice it.
                }
            }
            if (positionL < 0) { // we just caught up to the dry tap.
                if (realzeroesL > 0) { // we just caught up to the dry tap with zero crosses in the bin
                    positionL = 0;
                    double diff = 99999999.0;
                    int best = 0; // these can be local, I think
                    int scan;
                    for (scan = (realzeroesL - 1); scan >= 0; scan--) {
                        int scanone = scan + crossesL;
                        if (scanone > 256) {
                            scanone -= 256;
                        }
                        // try to track the real most recent ones more closely
                        double howdiff = (double)((tempL - pastzeroL[scanone]) + (lasttempL - previousL[scanone]) + (thirdtempL - thirdL[scanone]) + (fourthtempL - fourthL[scanone]));
                        // got difference factor between things
                        howdiff -= (double)(scan * bias);
                        // try to bias in favor of more recent crosses
                        if (howdiff < diff) {
                            diff = howdiff;
                            best = scanone;
                        }
                    } // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
                    positionL = offsetL[best] - sincezerocrossL;
                    crossesL = 0;
                    realzeroesL = 0;
                    splicingL = true; // we just kicked the delay tap back, changing positionL
                }
                else { // we just caught up to the dry tap with no crosses- glitch speeds.
                    positionL += width;
                    crossesL = 0;
                    realzeroesL = 0;
                    splicingL = true; // so, hard splice it.
                }
            }
            positionR -= speed; // this is individual to each channel!
            if (positionR > width) { // we just caught up to the buffer end
                if (realzeroesR > 0) { // we just caught up to the buffer end with zero crosses in the bin
                    positionR = 0;
                    double diff = 99999999.0;
                    int best = 0; // these can be local, I think
                    int scan;
                    for (scan = (realzeroesR - 1); scan >= 0; scan--) {
                        int scanone = scan + crossesR;
                        if (scanone > 256) {
                            scanone -= 256;
                        }
                        // try to track the real most recent ones more closely
                        double howdiff = (double)((tempR - pastzeroR[scanone]) + (lasttempR - previousR[scanone]) + (thirdtempR - thirdR[scanone]) + (fourthtempR - fourthR[scanone]));
                        // got difference factor between things
                        howdiff -= (double)(scan * bias);
                        // try to bias in favor of more recent crosses
                        if (howdiff < diff) {
                            diff = howdiff;
                            best = scanone;
                        }
                    } // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
                    positionR = offsetR[best] - sincezerocrossR;
                    crossesR = 0;
                    realzeroesR = 0;
                    splicingR = true; // we just kicked the delay tap back, changing positionL
                }
                else { // we just caught up to the buffer end with no crosses- glitch speeds.
                    positionR -= width;
                    crossesR = 0;
                    realzeroesR = 0;
                    splicingR = true; // so, hard splice it.
                }
            }
            if (positionR < 0) { // we just caught up to the dry tap.
                if (realzeroesR > 0) { // we just caught up to the dry tap with zero crosses in the bin
                    positionR = 0;
                    double diff = 99999999.0;
                    int best = 0; // these can be local, I think
                    int scan;
                    for (scan = (realzeroesR - 1); scan >= 0; scan--) {
                        int scanone = scan + crossesR;
                        if (scanone > 256) {
                            scanone -= 256;
                        }
                        // try to track the real most recent ones more closely
                        double howdiff = (double)((tempR - pastzeroR[scanone]) + (lasttempR - previousR[scanone]) + (thirdtempR - thirdR[scanone]) + (fourthtempR - fourthR[scanone]));
                        // got difference factor between things
                        howdiff -= (double)(scan * bias);
                        // try to bias in favor of more recent crosses
                        if (howdiff < diff) {
                            diff = howdiff;
                            best = scanone;
                        }
                    } // now we have 'best' as the closest match to the current rate of zero cross and positioning- a splice.
                    positionR = offsetR[best] - sincezerocrossR;
                    crossesR = 0;
                    realzeroesR = 0;
                    splicingR = true; // we just kicked the delay tap back, changing positionL
                }
                else { // we just caught up to the dry tap with no crosses- glitch speeds.
                    positionR += width;
                    crossesR = 0;
                    realzeroesR = 0;
                    splicingR = true; // so, hard splice it.
                }
            }
            count = gcount - (int)floor(positionL);
            // we go back because the buffer goes forward this time
            countone = count + 1;
            counttwo = count + 2;
            // now we have counts zero, one, two, none of which have sanity checked values
            // we are interpolating, we ADD
            while (count < 0) {
                count += width;
            }
            while (countone < 0) {
                countone += width;
            }
            while (counttwo < 0) {
                counttwo += width;
            }
            while (count > width) {
                count -= width;
            } // this can only happen with very insane variables
            while (countone > width) {
                countone -= width;
            }
            while (counttwo > width) {
                counttwo -= width;
            }
            // here's where we do our shift against the rotating buffer
            tempL = 0;
            tempL += (int)(pL[count] * (1 - (positionL - floor(positionL)))); // less as value moves away from .0
            tempL += pL[count + 1]; // we can assume always using this in one way or another?
            tempL += (int)(pL[count + 2] * (positionL - floor(positionL))); // greater as value moves away from .0
            tempL -= (int)(((pL[count] - pL[count + 1]) - (pL[count + 1] - pL[count + 2])) / 50); // interpolation hacks 'r us
            tempL /= 2; // gotta make temp be the same level scale as buffer
            // now we have our delay tap, which is going to do our pitch shifting
            if (abs(tempL) > 8388352.0) {
                tempL = (lasttempL + (lasttempL - thirdtempL));
            }
            // kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*
            sincezerocrossL++;
            if (sincezerocrossL < 0 || sincezerocrossL > width) {
                sincezerocrossL = 0;
            } // just a sanity check
            if (splicingL) {
                tempL = (tempL + (lasttempL + (lasttempL - thirdtempL))) / 2;
                splicingL = false;
            }
            // do a smoother transition by taking the sample of transition and going half with it
            if ((lasttempL > 0 && tempL < 0) || (lasttempL < 0 && tempL > 0)) // delay tap crossed zero
            {
                sincezerocrossL = 0;
            } // we just restarted counting from the delay tap zero cross
            count = gcount - (int)floor(positionR);
            // we go back because the buffer goes forward this time
            countone = count + 1;
            counttwo = count + 2;
            // now we have counts zero, one, two, none of which have sanity checked values
            // we are interpolating, we ADD
            while (count < 0) {
                count += width;
            }
            while (countone < 0) {
                countone += width;
            }
            while (counttwo < 0) {
                counttwo += width;
            }
            while (count > width) {
                count -= width;
            } // this can only happen with very insane variables
            while (countone > width) {
                countone -= width;
            }
            while (counttwo > width) {
                counttwo -= width;
            }
            tempR = 0;
            tempR += (int)(pR[count] * (1 - (positionR - floor(positionR)))); // less as value moves away from .0
            tempR += pR[count + 1]; // we can assume always using this in one way or another?
            tempR += (int)(pR[count + 2] * (positionR - floor(positionR))); // greater as value moves away from .0
            tempR -= (int)(((pR[count] - pR[count + 1]) - (pR[count + 1] - pR[count + 2])) / 50); // interpolation hacks 'r us
            tempR /= 2; // gotta make temp be the same level scale as buffer
            // now we have our delay tap, which is going to do our pitch shifting
            if (abs(tempR) > 8388352.0) {
                tempR = (lasttempR + (lasttempR - thirdtempR));
            }
            // kill ticks of bad buffer mojo by sticking with the trajectory. Ugly hack *shrug*
            sincezerocrossR++;
            if (sincezerocrossR < 0 || sincezerocrossR > width) {
                sincezerocrossR = 0;
            } // just a sanity check
            if (splicingR) {
                tempR = (tempR + (lasttempR + (lasttempR - thirdtempR))) / 2;
                splicingR = false;
            }
            // do a smoother transition by taking the sample of transition and going half with it
            if ((lasttempR > 0 && tempR < 0) || (lasttempR < 0 && tempR > 0)) // delay tap crossed zero
            {
                sincezerocrossR = 0;
            } // we just restarted counting from the delay tap zero cross
            fourthtempL = thirdtempL;
            thirdtempL = lasttempL;
            lasttempL = tempL;
            fourthtempR = thirdtempR;
            thirdtempR = lasttempR;
            lasttempR = tempR;
            inputSampleL = (drySampleL * (1 - wet)) + ((double)(tempL / (8388352.0)) * wet);
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            inputSampleR = (drySampleR * (1 - wet)) + ((double)(tempR / (8388352.0)) * wet);
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            // this plugin can throw insane outputs so we'll put in a hard clip
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
} // namespace airwindohhs::glitchshifter
