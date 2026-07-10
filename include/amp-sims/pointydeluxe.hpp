#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::pointydeluxe {

constexpr std::string_view k_name{ "PointyDeluxe" };
constexpr std::string_view k_short_description{
    "PointyDeluxe devours all mix space."
};
constexpr std::string_view k_long_description{
    "So this is one of the strangest things I've ever made. This is not normal.What happens with PointyGuitar is, successive stages of EQ and saturation zero in on the tone of a guitar sound, which is also gated and run through a separate filterbank that's just a very steep and intensely colorful cutoff. You get intense tone shaping and the type of sonic confinement a speaker cab would give, at least to some extent: and though the colorfulness twists the phase like a physical speaker might, there are no resonances or colorations so there's a sonority there.So what happens when you remove the gate and also the extra filterbank and use all the controls solely for fine-grained control over the core amp engine?Terrifying, bad things :)The thing is, PointyGuitar has the same number of bands PointyDeluxe has. Internally, it uses double the filter bands that you see in the controls. What it's doing is splitting the difference, smoothing the curve by applying intermediate settings.With PointyDeluxe, you get to make a radical departure from this, and boost or cut much narrower bands… but it's using the AngleEQ code, which is not as stable as you'd think. So, in the same way that PointyGuitar can get frisky when you twist the knobs too much, PointyDeluxe can be unstable… more. In particular, if you boost a band and try to cut the band just below it, things get real messy. If you boost any band, that's where added gain comes from: it'll track whatever is the loudest band and set the gain from that. If you cut, there's a built-in pad which attenuates the whole output based on which band is the least. Any band set to zero will silence the plugin. If you set bands to almost nearly zero… it will probably not stop PointyDeluxe from exploding with strange howling noise.Such are the fates of deeply abnormal plugins… at least Airwindows ones. This is one of the best plugins I've ever made for resembling a strange circuit board found in a drawer, which delivers wild pungent noises before it burns out. However, PointyDeluxe, being software, needn't burn out.When you try to use it for good, what happens? The bands are named after Slipperman's Distorted Guitars From Hell, which seems appropriate. You can take a suitable band like 'Pick' and crank it up until it chugs. You can cut back the top two bands, Fizz and Hell (as in 'road to hell, lose a windshield up here') until the chug isn't that grating. You'll have to dial back the lows as the boost affects them too, and you'll have to make two slopes, one dialing it back until you reach another boost at H Meat or L Meat, and then below that you'll have to dial it back more, though you can't go too steep. All the 'sweet spots' for every band will be very fussy. But because of the topology of PointyDeluxe, you can do it. It'll be trying to blow up every second… some settings will sound like the amp is literally melting… but it can be done, you can replicate the tonal signature of a huge high-gain amp in PointyDeluxe.And it sounds totally insane. What is it? It's distilled essence of heavy guitar without any blur or coloration. It's the searing intensity of a heavy amp (because of up to fifteen stages of EQ and overdrive) but this applies only to the chug, and because of how high gain distortion works, the full range sound is also constantly present around the edges. Functionally, that means the guitar sound gets shaped into chug or other sorts of intensity, but it also takes up ALL the space. There's no gloss to the highs, no rumble to the lows, it's stripped and bare in the weirdest way and three guitars of high gain PointyDeluxe might as well be a hundred.My hope was to make the densest, most brutal heavy guitar tone ever. Didn't occur to me to ask 'is there such a thing as too brutal?' until I'd fired it up, using a new budget extended range guitar I got just for the purpose, and heard what I'd made.There is probably such a thing as too brutal, and this is probably it. If you have a place for that, have fun!"
};
constexpr std::string_view k_tags{
    "amp-sims"
};

template <typename T>
class PointyDeluxe final : public Effect<T>
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
    double angSL[18][16];
    double angAL[18][16];
    double angSR[18][16];
    double angAR[18][16];
    double angG[16];
    double lastSampleL;
    double intermediateL[16];
    bool wasPosClipL;
    bool wasNegClipL;
    double lastSampleR;
    double intermediateR[16];
    bool wasPosClipR;
    bool wasNegClipR; // Stereo ClipOnly2
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    PointyDeluxe()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 0.5;
        G = 0.5;
        H = 0.5;
        I = 0.5;
        J = 0.5;
        for (int x = 0; x < 17; x++) {
            for (int y = 0; y < 14; y++) {
                angSL[x][y] = 0.0;
                angAL[x][y] = 0.0;
                angSR[x][y] = 0.0;
                angAR[x][y] = 0.0;
            }
        }
        for (int y = 0; y < 14; y++) {
            angG[y] = 0.0;
        }
        lastSampleL = 0.0;
        wasPosClipL = false;
        wasNegClipL = false;
        lastSampleR = 0.0;
        wasPosClipR = false;
        wasNegClipR = false;
        for (int x = 0; x < 16; x++) {
            intermediateL[x] = 0.0;
            intermediateR[x] = 0.0;
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
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.5; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "hell"; break;
            case kParamB: return "fizz"; break;
            case kParamC: return "pick"; break;
            case kParamD: return "satan"; break;
            case kParamE: return "danger"; break;
            case kParamF: return "crtical"; break;
            case kParamG: return "h meat"; break;
            case kParamH: return "l meat"; break;
            case kParamI: return "swing"; break;
            case kParamJ: return "rarely"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Hell"; break;
            case kParamB: return "Fizz"; break;
            case kParamC: return "Pick"; break;
            case kParamD: return "Satan"; break;
            case kParamE: return "Danger"; break;
            case kParamF: return "Crtical"; break;
            case kParamG: return "H Meat"; break;
            case kParamH: return "L Meat"; break;
            case kParamI: return "Swing"; break;
            case kParamJ: return "Rarely"; break;

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
        double drive = 1.0;
        double pad = 1.0;
        angG[1] = sqrt(A * 2.0);
        if (pad > angG[1]) {
            pad = angG[1];
        }
        if (drive < angG[1]) {
            drive = angG[1];
        }
        angG[0] = (1.0 + angG[1]) * 0.5; // if boost, boost half
        if (angG[0] > angG[1]) {
            angG[0] = angG[1];
        }
        angG[2] = sqrt(B * 2.0);
        if (pad > angG[2]) {
            pad = angG[2];
        }
        if (drive < angG[2]) {
            drive = angG[2];
        }
        angG[3] = sqrt(C * 2.0);
        if (pad > angG[3]) {
            pad = angG[3];
        }
        if (drive < angG[3]) {
            drive = angG[3];
        }
        angG[4] = sqrt(D * 2.0);
        if (pad > angG[4]) {
            pad = angG[4];
        }
        if (drive < angG[4]) {
            drive = angG[4];
        }
        angG[5] = sqrt(E * 2.0);
        if (pad > angG[5]) {
            pad = angG[5];
        }
        if (drive < angG[5]) {
            drive = angG[5];
        }
        angG[6] = sqrt(F * 2.0);
        if (pad > angG[6]) {
            pad = angG[6];
        }
        if (drive < angG[6]) {
            drive = angG[6];
        }
        angG[7] = sqrt(G * 2.0);
        if (pad > angG[7]) {
            pad = angG[7];
        }
        if (drive < angG[7]) {
            drive = angG[7];
        }
        angG[8] = sqrt(H * 2.0);
        if (pad > angG[8]) {
            pad = angG[8];
        }
        if (drive < angG[8]) {
            drive = angG[8];
        }
        angG[9] = sqrt(I * 2.0);
        if (pad > angG[9]) {
            pad = angG[9];
        }
        if (drive < angG[9]) {
            drive = angG[9];
        }
        angG[10] = sqrt(J * 2.0);
        if (pad > angG[10]) {
            pad = angG[10];
        }
        if (drive < angG[10]) {
            drive = angG[10];
        }
        angG[11] = 1.0;
        angG[12] = 1.0;
        pad = sqrt(pad);
        int poles = (int)(drive * 11.0);
        double tune = 0.618 + (overallscale * 0.0055);
        int spacing = floor(overallscale); // should give us working basic scaling, usually 2 or 4
        if (spacing < 1) {
            spacing = 1;
        }
        if (spacing > 16) {
            spacing = 16;
        }
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            for (int x = 0; x < poles; x++) {
                double fr = (0.92 / overallscale) + (overallscale * 0.01);
                double band = inputSampleL;
                inputSampleL = 0.0;
                for (int y = 0; y < 12; y++) {
                    angAL[x][y] = (angAL[x][y] * (1.0 - fr)) + ((band - angSL[x][y]) * fr);
                    double temp = band;
                    band = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSL[x][y] = ((angSL[x][y] + (angAL[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleL += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleL += band;
                inputSampleL *= drive;
                inputSampleL -= fmin(fmax((inputSampleL * (fabs(inputSampleL) * 0.654) * (fabs(inputSampleL) * 0.654)), -1.0), 1.0);
                fr = (0.92 / overallscale) + (overallscale * 0.01);
                band = inputSampleR;
                inputSampleR = 0.0;
                for (int y = 0; y < 12; y++) {
                    angAR[x][y] = (angAR[x][y] * (1.0 - fr)) + ((band - angSR[x][y]) * fr);
                    double temp = band;
                    band = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    angSR[x][y] = ((angSR[x][y] + (angAR[x][y] * fr)) * (1.0 - fr)) + (band * fr);
                    inputSampleR += ((temp - band) * angG[y]);
                    fr *= tune;
                }
                inputSampleR += band;
                inputSampleR *= drive;
                inputSampleR -= fmin(fmax((inputSampleR * (fabs(inputSampleR) * 0.654) * (fabs(inputSampleR) * 0.654)), -1.0), 1.0);
            }
            if (pad < 1.0) {
                inputSampleL *= pad;
                inputSampleR *= pad;
            }
            // begin ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
            if (inputSampleL > 4.0) {
                inputSampleL = 4.0;
            }
            if (inputSampleL < -4.0) {
                inputSampleL = -4.0;
            }
            if (wasPosClipL == true) { // current will be over
                if (inputSampleL < lastSampleL) {
                    lastSampleL = 0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = 0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasPosClipL = false;
            if (inputSampleL > 0.9549925859) {
                wasPosClipL = true;
                inputSampleL = 0.7058208 + (lastSampleL * 0.2609148);
            }
            if (wasNegClipL == true) { // current will be -over
                if (inputSampleL > lastSampleL) {
                    lastSampleL = -0.7058208 + (inputSampleL * 0.2609148);
                }
                else {
                    lastSampleL = -0.2491717 + (lastSampleL * 0.7390851);
                }
            }
            wasNegClipL = false;
            if (inputSampleL < -0.9549925859) {
                wasNegClipL = true;
                inputSampleL = -0.7058208 + (lastSampleL * 0.2609148);
            }
            intermediateL[spacing] = inputSampleL;
            inputSampleL = lastSampleL; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateL[x - 1] = intermediateL[x];
            }
            lastSampleL = intermediateL[0]; // run a little buffer to handle this
            if (inputSampleR > 4.0) {
                inputSampleR = 4.0;
            }
            if (inputSampleR < -4.0) {
                inputSampleR = -4.0;
            }
            if (wasPosClipR == true) { // current will be over
                if (inputSampleR < lastSampleR) {
                    lastSampleR = 0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = 0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasPosClipR = false;
            if (inputSampleR > 0.9549925859) {
                wasPosClipR = true;
                inputSampleR = 0.7058208 + (lastSampleR * 0.2609148);
            }
            if (wasNegClipR == true) { // current will be -over
                if (inputSampleR > lastSampleR) {
                    lastSampleR = -0.7058208 + (inputSampleR * 0.2609148);
                }
                else {
                    lastSampleR = -0.2491717 + (lastSampleR * 0.7390851);
                }
            }
            wasNegClipR = false;
            if (inputSampleR < -0.9549925859) {
                wasNegClipR = true;
                inputSampleR = -0.7058208 + (lastSampleR * 0.2609148);
            }
            intermediateR[spacing] = inputSampleR;
            inputSampleR = lastSampleR; // Latency is however many samples equals one 44.1k sample
            for (int x = spacing; x > 0; x--) {
                intermediateR[x - 1] = intermediateR[x];
            }
            lastSampleR = intermediateR[0]; // run a little buffer to handle this
            // end ClipOnly2 stereo as a little, compressed chunk that can be dropped into code
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
} // namespace airwindohhs::pointydeluxe
