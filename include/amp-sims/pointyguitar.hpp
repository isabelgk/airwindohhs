#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::pointyguitar {

constexpr std::string_view k_name{ "PointyGuitar" };
constexpr std::string_view k_short_description{
    "PointyGuitar is a supremely adaptable instrument amp."
};
constexpr std::string_view k_long_description{
    "Sometimes the best way to get the performance of a very analog thing in a very digital format is to accentuate, not the spirit of the analog, but the capacities of the digital. What you get then is not fake gutless imitations of the artifacts of the analog thing, but the heart of why we go for that analog thing. Meet PointyGuitar, which is all amps. Just adjust… if, that is, you know what the analog sound is made of.Here's how it works. SmoothEQ was a sort of breakthrough: the ability to get very steep EQ crossovers while still zeroing out at perfect, unaltered fidelity. It sounds the way it does because it reconstitutes the original source between every stage, rather than doing independent EQ bands and trusting that they'll be 'just as good' because the gains and EQ slopes match up closely enough.Then there's AngleEQ, a sort of disaster: a very heavily colored EQ where the crossovers are so phasey that it's like running stuff through a speaker. The opposite: whether as a highpass or a lowpass, it produces a weird physicality and scrambles the sound pretty good.PointyGuitar is both, in series, with a basic distortion (as found in FireAmp) between every EQ stage. There can be lots of these EQ stages, so there can be lots of stages of this simple softclip, but it's always on the full sound, not a 'multiband'.What happens? Any given section might add small amounts of overdrive. It's not an electrical circuit and it's run at double precision so there is no thermal noise/Johnson noise: it's just a bit of overdrive if you're boosting a band. Otherwise, if it's flat it's extremely flat (interestingly, inside the flat EQ it's still using the AngleEQ math, but in such a way that it sums to perfectly flat. Using biquads would've got steeper crossovers, but requiring more stages turned out perfect here)As it's passed on to the next stage, any distortion harmonic that doesn't fit into the following band just plain gets filtered out. This is most notable in the amount you can crank the 'High' band. It's around 3K for guitar chug, but it's resisting aliasing super hard considering the amount of gain you can add. Same applies for every band except Presnce. You get that, High, Mid, Low and Sub. For high gain '5150' type sounds, you boost High and cut Low.Once you have your sound, be it super high gain or clean Fendery stuff (again, boosting High often helps things sound more amp-like) it's over to the highpass and lowpass. HSpeakr cuts off the bright, and LSpeakr limits the size of your virtual cabinet. They're pure AngleEQ and combine to produce a bandlimited sound without use of a cabinet impulse, harnessing the intensity of the EQ/saturation stages so that the tone sits right. Very delicate adjustments are what you want here, but both those controls have pretty much full range adjustability. 50 foot speaker, or clock radio, are available if you should need them.In this way, first building a sound out of identifying which frequency bands need most saturation and which to back off, and then channeling that very saturated but very clean sound through extremely colorful bandpassing, you produce an amplike sound (guitar or bass, any amp of any kind, it just depends how you adjust it) which fits immediately into whatever space in the mix you need, with the right tone colorings……at ZERO LATENCY.It runs without oversampling, and all the EQs operate directly without pre-ring, as analog circuits do. So you can track directly with PointyGuitar, set any way you like, dial in the basic sound of any sort of amp no matter how clean or dirty, and have it respond so immediately that you'll notice if anything else in your recording chain is adding latency. It'll feed back like any real amp, it'll feel connected to your fingers, and you can dial it to do anything you want, pretty much. If there's a cabinet honk or something that you actually want to add (rather than just remove) you can run the whole thing into ConsoleX and dial that on the parametric, perhaps in conjunction with the Speakr controls.I may have just replaced my little tube amp and iso cab and new speaker DI box at a stroke. Didn't expect that, but both my ears and Airwindows Meter tell me PointyGuitar is in the zone, even for really difficult sought-after guitar tones. The range is pretty shocking: it ought to do bass amps, Plexis on Variacs, ratty little Peaveys, you name it.Oh, and you get a gate (basically DigitalBlack) that triggers off the input before the distortion, but gates between the 'amp' and Speakr so that even if you've got it firing really staccato and quick, it'll merge with your string-damping and act like part of your playing, for guitar OR bass.Enjoy. This should work both for vintage tone guys AND Reznorian madmen. :)"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class PointyGuitar final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float I;
    float J;
    double angSL[18][12];
    double angAL[18][12];
    double iirHPositionL[37];
    double iirHAngleL[37];
    double iirBPositionL[37];
    double iirBAngleL[37];
    bool WasNegativeL;
    int ZeroCrossL;
    double gaterollerL;
    double gateL;
    double angSR[18][12];
    double angAR[18][12];
    double iirHPositionR[37];
    double iirHAngleR[37];
    double iirBPositionR[37];
    double iirBAngleR[37];
    bool WasNegativeR;
    int ZeroCrossR;
    double gaterollerR;
    double gateR;
    double angG[12];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PointyGuitar()
    {
        A = 0.6;
        B = 0.5;
        C = 0.8;
        D = 0.6;
        E = 0.4;
        F = 0.5;
        G = 0.7;
        H = 0.4;
        I = 0.0;
        J = 0.7;
        for (int x = 0; x < 17; x++) {
            for (int y = 0; y < 11; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 11; y++) {
            angG[y] = 0.0;
        }
        for (int count = 0; count < 36; count++) {
            iirHPositionL[count] = 0.0;
            iirHAngleL[count] = 0.0;
            iirBPositionL[count] = 0.0;
            iirBAngleL[count] = 0.0;
            iirHPositionR[count] = 0.0;
            iirHAngleR[count] = 0.0;
            iirBPositionR[count] = 0.0;
            iirBAngleR[count] = 0.0;
        }
        WasNegativeL = false;
        ZeroCrossL = 0;
        gaterollerL = 0.0;
        WasNegativeR = false;
        ZeroCrossR = 0;
        gaterollerR = 0.0;
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
        kParamG = 6,
        kParamH = 7,
        kParamI = 8,
        kParamJ = 9,
        kNumParameters = 10
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            case kParamI: I = value; break;
            case kParamJ: J = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;
            case kParamH: return H; break;
            case kParamI: return I; break;
            case kParamJ: return J; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.6; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.8; break;
            case kParamD: return 0.6; break;
            case kParamE: return 0.4; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.7; break;
            case kParamH: return 0.4; break;
            case kParamI: return 0.0; break;
            case kParamJ: return 0.7; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "drive"; break;
            case kParamB: return "presnce"; break;
            case kParamC: return "high"; break;
            case kParamD: return "mid"; break;
            case kParamE: return "low"; break;
            case kParamF: return "sub"; break;
            case kParamG: return "hspeakr"; break;
            case kParamH: return "lspeakr"; break;
            case kParamI: return "gate"; break;
            case kParamJ: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Drive"; break;
            case kParamB: return "Presnce"; break;
            case kParamC: return "High"; break;
            case kParamD: return "Mid"; break;
            case kParamE: return "Low"; break;
            case kParamF: return "Sub"; break;
            case kParamG: return "HSpeakr"; break;
            case kParamH: return "LSpeakr"; break;
            case kParamI: return "Gate"; break;
            case kParamJ: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;
            case kParamG: return std::to_string(G); break;
            case kParamH: return std::to_string(H); break;
            case kParamI: return std::to_string(I); break;
            case kParamJ: return std::to_string(J); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return ""; break;
            case kParamH: return ""; break;
            case kParamI: return ""; break;
            case kParamJ: return ""; break;

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
        double drive = A + 0.618033988749894;
        angG[0] = sqrt(B * 2.0);
        angG[2] = sqrt(C * 2.0);
        angG[4] = sqrt(D * 2.0);
        angG[6] = sqrt(E * 2.0);
        angG[8] = sqrt(F * 2.0);
        angG[1] = (angG[0] + angG[2]) * 0.5;
        angG[3] = (angG[2] + angG[4]) * 0.5;
        angG[5] = (angG[4] + angG[6]) * 0.5;
        angG[7] = (angG[6] + angG[8]) * 0.5;
        angG[9] = angG[8];
        int poles = (int)(drive * 10.0);
        double hFreq = pow(G, overallscale);
        double lFreq = pow(H, overallscale + 3.0);
        // begin Gate
        double onthreshold = (pow(I, 3) / 3) + 0.00018;
        double offthreshold = onthreshold * 1.1;
        double release = 0.028331119964586;
        double absmax = 220.9;
        // end Gate
        double output = J;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            // begin Gate
            if (inputSampleL > 0.0) {
                if (WasNegativeL == true) {
                    ZeroCrossL = absmax * 0.3;
                }
                WasNegativeL = false;
            }
            else {
                ZeroCrossL += 1;
                WasNegativeL = true;
            }
            if (ZeroCrossL > absmax) {
                ZeroCrossL = absmax;
            }
            if (gateL == 0.0) {
                // if gateL is totally silent
                if (fabs(inputSampleL) > onthreshold) {
                    if (gaterollerL == 0.0) {
                        gaterollerL = ZeroCrossL;
                    }
                    else {
                        gaterollerL -= release;
                    }
                    // trigger from total silence only- if we're active then signal must clear offthreshold
                }
                else {
                    gaterollerL -= release;
                }
            }
            else {
                // gateL is not silent but closing
                if (fabs(inputSampleL) > offthreshold) {
                    if (gaterollerL < ZeroCrossL) {
                        gaterollerL = ZeroCrossL;
                    }
                    else {
                        gaterollerL -= release;
                    }
                    // always trigger if gateL is over offthreshold, otherwise close anyway
                }
                else {
                    gaterollerL -= release;
                }
            }
            if (gaterollerL < 0.0) {
                gaterollerL = 0.0;
            }
            for (int x = 0; x < poles; x++) {
                double fr = 0.9 / overallscale;
                double band = inputSampleL;
                inputSampleL = 0.0;
                for (int y = 0; y < 9; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((band - angSL[x][y]) * fr);
                    double temp = band;
                    band = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleL += ((temp - band) * angG[y]);
                    fr *= 0.618033988749894;
                }
                inputSampleL += (band * angG[9]);
                inputSampleL *= drive;
                inputSampleL -= fmin(fmax((inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654)), -1.0), 1.0);
            }
            if (gaterollerL < 1.0) {
                gateL = gaterollerL;
                double bridgerectifier = 1 - cos(fabs(inputSampleL));
                if (inputSampleL > 0) {
                    inputSampleL = (inputSampleL * gateL) + (bridgerectifier * (1.0 - gateL));
                }
                else {
                    inputSampleL = (inputSampleL * gateL) - (bridgerectifier * (1.0 - gateL));
                }
                if (gateL == 0.0) {
                    inputSampleL = 0.0;
                }
            }
            else {
                gateL = 1.0;
            }
            // end Gate
            double lowSample = inputSampleL;
            for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
                iirBAngleL[count] = (iirBAngleL[count] * (1.0 - lFreq)) + ((lowSample - iirBPositionL[count]) * lFreq);
                lowSample = ((iirBPositionL[count] + (iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                iirBPositionL[count] = ((iirBPositionL[count] + (iirBAngleL[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                inputSampleL -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
            }
            for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
                iirHAngleL[count] = (iirHAngleL[count] * (1.0 - hFreq)) + ((inputSampleL - iirHPositionL[count]) * hFreq);
                inputSampleL = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
                iirHPositionL[count] = ((iirHPositionL[count] + (iirHAngleL[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleL * hFreq);
            } // the lowpass
            inputSampleL *= output;
            // begin Gate
            if (inputSampleR > 0.0) {
                if (WasNegativeR == true) {
                    ZeroCrossR = absmax * 0.3;
                }
                WasNegativeR = false;
            }
            else {
                ZeroCrossR += 1;
                WasNegativeR = true;
            }
            if (ZeroCrossR > absmax) {
                ZeroCrossR = absmax;
            }
            if (gateR == 0.0) {
                // if gateR is totally silent
                if (fabs(inputSampleR) > onthreshold) {
                    if (gaterollerR == 0.0) {
                        gaterollerR = ZeroCrossR;
                    }
                    else {
                        gaterollerR -= release;
                    }
                    // trigger from total silence only- if we're active then signal must clear offthreshold
                }
                else {
                    gaterollerR -= release;
                }
            }
            else {
                // gateR is not silent but closing
                if (fabs(inputSampleR) > offthreshold) {
                    if (gaterollerR < ZeroCrossR) {
                        gaterollerR = ZeroCrossR;
                    }
                    else {
                        gaterollerR -= release;
                    }
                    // always trigger if gateR is over offthreshold, otherwise close anyway
                }
                else {
                    gaterollerR -= release;
                }
            }
            if (gaterollerR < 0.0) {
                gaterollerR = 0.0;
            }
            for (int x = 0; x < poles; x++) {
                double fr = 0.9 / overallscale;
                double band = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 9; y++) {
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((band - angSR[x][y]) * fr);
                    double temp = band;
                    band = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleR += ((temp - band) * angG[y]);
                    fr *= 0.618033988749894;
                }
                inputSampleR += (band * angG[9]);
                inputSampleR *= drive;
                inputSampleR -= fmin(fmax((inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654)), -1.0), 1.0);
            }
            if (gaterollerR < 1.0) {
                gateR = gaterollerR;
                double bridgerectifier = 1 - cos(fabs(inputSampleR));
                if (inputSampleR > 0) {
                    inputSampleR = (inputSampleR * gateR) + (bridgerectifier * (1.0 - gateR));
                }
                else {
                    inputSampleR = (inputSampleR * gateR) - (bridgerectifier * (1.0 - gateR));
                }
                if (gateR == 0.0) {
                    inputSampleR = 0.0;
                }
            }
            else {
                gateR = 1.0;
            }
            // end Gate
            lowSample = inputSampleR;
            for (int count = 0; count < (3.0 + (lFreq * 32.0)); count++) {
                iirBAngleR[count] = (iirBAngleR[count] * (1.0 - lFreq)) + ((lowSample - iirBPositionR[count]) * lFreq);
                lowSample = ((iirBPositionR[count] + (iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                iirBPositionR[count] = ((iirBPositionR[count] + (iirBAngleR[count] * lFreq)) * (1.0 - lFreq)) + (lowSample * lFreq);
                inputSampleR -= (lowSample * (1.0 / (3.0 + (lFreq * 32.0))));
            }
            for (int count = 0; count < (3.0 + (hFreq * 32.0)); count++) {
                iirHAngleR[count] = (iirHAngleR[count] * (1.0 - hFreq)) + ((inputSampleR - iirHPositionR[count]) * hFreq);
                inputSampleR = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
                iirHPositionR[count] = ((iirHPositionR[count] + (iirHAngleR[count] * hFreq)) * (1.0 - hFreq)) + (inputSampleR * hFreq);
            } // the lowpass
            inputSampleR *= output;
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
} // namespace airwindohhs::pointyguitar
