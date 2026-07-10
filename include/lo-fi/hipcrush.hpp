#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::hipcrush {

constexpr std::string_view k_name{ "HipCrush" };
constexpr std::string_view k_short_description{
    "HipCrush is the de-rez section for ConsoleH."
};
constexpr std::string_view k_long_description{
    "So when I said I needed to write transformative plugins in order to give ConsoleH a true ability to invent new sounds and genres, this is what I meant.Is it de-rez, bit-crush, sampler emulations? Yes and no. In particular, it's very very bitcrush, but in a way you've certainly not heard before. But to start explaining, I need to talk about another plugin I did once, ResEQ. That was among my first plugins, and it generated multiple resonant peaks and added them to a convolution kernel in order to make a tone entirely out of multiple really tight resonances.HipCrush begins as a replacement for what, in ConsoleX, is an SSL-like four-band parametric. The ConsoleX one even uses nonlinearity to emulate real-world capacitors in order to bring a more ratty old SSL vibe to its boosts and cuts, all pretty tight and raunchy-sounding. All good fun. That's not what HipCrush is about at all.Instead, it's not boosts and cuts. You pick three boosts (in ConsoleH it will be four) and the lowest setting isn't a cut, it's silence. With all the levels at zero, it makes no sound (unless you're using the dry/wet). As you bring up the gains, the boosts start pretty broad, and get tighter and more intense as they go. With all three (or four in ConsoleH) bands in play, the level is roughly the same across various settings, but the original tone is completely replaced by pungent, resonant wah-like effects.This is already useful in its way. If you reconstruct a tone so it more or less covers the frequencies you need, it'll have a similar vibe to the dry signal but will be WAY more clear in the mix. You're doing an opposite-Soothe: you're replacing the whole sound with vivid resonant tones like cocked wahs, several at once, and this is incredible at clearing up a mix. HipCrush always does this unless you use the dry/wet (in ConsoleH, you'll crossfade between this and a simple three-band great-sounding EQ, SmoothEQ3, which is also coming soon).You can layer tracks using HipCrush just as a three-peak resonant filter, balance the frequency ranges and find areas of power to reinforce, and get a really intense mix where your layering tracks refuse to get in the way of full-range tracks because they're doing that ResEQ thing.But then it gets REALLY transformative, as in 'use Eagles songs as your backing rhythms, why not, they'll be unrecognizable' transformative.The remaining control for each band is called Crush. Like some of my recent reverb controls, at the center it's disabled and isn't crushing anything (if it's perfectly centered at 0.5 all crush is bypassed and your filters are full double precision floating point) but if you turn it clockwise (to the right) it's Compress and counterclockwise, to the left, it's Gate.It doesn't really compress, or gate. That's just what you get to do with the bitcrush, in a continuous adjustment from 16 bit right down to one bit or less.What does that mean? Turn up the control for the 'compress' and you progressively bitcrush with the transition between bits located exactly at 0.0. Unless your actual signal is DC-offset, what this will do is bring up a bitcrush 'noise floor' that roars and screams and is a blast of noise. (In ConsoleH, you'll have a gate in the dynamics section that can turn it into gated roar, as it'll trigger off the signal before the effect hits).If you turn the control down from 0.5, you bring up the same noisefloor, but offset exactly half a bit. That means that where the roaring out of control version becomes full blast, that's where the gate version will cut out entirely. In this way you can isolate stuff in the sound and make it pop in with a gating effect and cut out again, cut off exactly where you want it to cut off. Or, do a crunchy low-fi effect that's less extreme but still defines and reshapes the sound of the band's contribution.That's right: these independent bidirectional bitcrushes are PER BAND. But I've not made myself entirely clear. Everything I said?It runs between the two staggered filters that make up each resonant band. All separate.That means you can crank up the treble one to catch a hi-hat, gate that hat to make it much louder, brighter and more staccato, and the gated sound runs through ANOTHER level of even more resonant peak-filter. Adds flavor. It means you can isolate a kick drum, have the bass bitcrush-gate ONLY trigger off the kick, and then it filters that bitcrush again for a beefy, reinvented sound. If other parts of the kick show up in mids or highs, you can include them too, with separate gate dynamics! And you can balance all this with dry/wet if you really want to bring in the original sound.And then you can crank the mid crush UP all the way and sweep it around for the most savage de-rezzed roar you ever heard. And on ConsoleH you'll be able to apply a gate… but you'll also be able to apply speaker-like lowpass and highpass to give your nastiness a more organic, textural flavor after you used HipCrush to generate it. (and you can do that now, by using last week's plugin, Cabs2)Looking forward to getting out ConsoleH before 2026, and now I really do believe I'll be able to give hip-hop and all related/subsequent genres something by which new sounds can be invented, even for kids on laptops or Raspberry Pis. And then I'll put out ConsoleX2, and you can mix and match any and all of those as you please. Have fun! :)"
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class HipCrush final : public Effect<T>
{
    float TRF;
    float TRG;
    float TRB;
    float HMF;
    float HMG;
    float HMB;
    float LMF;
    float LMG;
    float LMB;
    float DW;
    enum
    {
        biqs_freq,
        biqs_reso,
        biqs_level,
        biqs_temp,
        biqs_bit,
        biqs_a0,
        biqs_a1,
        biqs_b1,
        biqs_b2,
        biqs_c0,
        biqs_c1,
        biqs_d1,
        biqs_d2,
        biqs_aL1,
        biqs_aL2,
        biqs_aR1,
        biqs_aR2,
        biqs_cL1,
        biqs_cL2,
        biqs_cR1,
        biqs_cR2,
        biqs_outL,
        biqs_outR,
        biqs_total
    };
    double high[biqs_total];
    double hmid[biqs_total];
    double lmid[biqs_total];
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    HipCrush()
    {
        TRF = 0.5;
        TRG = 0.0;
        TRB = 0.5;
        HMF = 0.5;
        HMG = 0.0;
        HMB = 0.5;
        LMF = 0.5;
        LMG = 0.0;
        LMB = 0.5;
        DW = 1.0;
        for (int x = 0; x < biqs_total; x++) {
            high[x] = 0.0;
            hmid[x] = 0.0;
            lmid[x] = 0.0;
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
            case kParamA: TRF = value; break;
            case kParamB: TRG = value; break;
            case kParamC: TRB = value; break;
            case kParamD: HMF = value; break;
            case kParamE: HMG = value; break;
            case kParamF: HMB = value; break;
            case kParamG: LMF = value; break;
            case kParamH: LMG = value; break;
            case kParamI: LMB = value; break;
            case kParamJ: DW = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return TRF; break;
            case kParamB: return TRG; break;
            case kParamC: return TRB; break;
            case kParamD: return HMF; break;
            case kParamE: return HMG; break;
            case kParamF: return HMB; break;
            case kParamG: return LMF; break;
            case kParamH: return LMG; break;
            case kParamI: return LMB; break;
            case kParamJ: return DW; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.5; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.0; break;
            case kParamF: return 0.5; break;
            case kParamG: return 0.5; break;
            case kParamH: return 0.0; break;
            case kParamI: return 0.5; break;
            case kParamJ: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "hi freq"; break;
            case kParamB: return "high"; break;
            case kParamC: return "hicrush"; break;
            case kParamD: return "midfreq"; break;
            case kParamE: return "mid"; break;
            case kParamF: return "mdcrush"; break;
            case kParamG: return "lo freq"; break;
            case kParamH: return "low"; break;
            case kParamI: return "locrush"; break;
            case kParamJ: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Hi Freq"; break;
            case kParamB: return "High"; break;
            case kParamC: return "HiCrush"; break;
            case kParamD: return "MidFreq"; break;
            case kParamE: return "Mid"; break;
            case kParamF: return "MdCrush"; break;
            case kParamG: return "Lo Freq"; break;
            case kParamH: return "Low"; break;
            case kParamI: return "LoCrush"; break;
            case kParamJ: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return std::to_string(TRF); break;
            case kParamB: return std::to_string(TRG); break;
            case kParamC: return std::to_string(TRB); break;
            case kParamD: return std::to_string(HMF); break;
            case kParamE: return std::to_string(HMG); break;
            case kParamF: return std::to_string(HMB); break;
            case kParamG: return std::to_string(LMF); break;
            case kParamH: return std::to_string(LMG); break;
            case kParamI: return std::to_string(LMB); break;
            case kParamJ: return std::to_string(DW); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "High"; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;
            case kParamD: return "Mid"; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;
            case kParamG: return "Low"; break;
            case kParamH: return ""; break;
            case kParamI: return ""; break;
            case kParamJ: return "Wet"; break;

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
        high[biqs_freq] = (((pow(TRF, 3) * 16000.0) + 1000.0) / Effect<T>::getSampleRate());
        if (high[biqs_freq] < 0.0001) {
            high[biqs_freq] = 0.0001;
        }
        high[biqs_bit] = (TRB * 2.0) - 1.0;
        high[biqs_level] = (1.0 - pow(1.0 - TRG, 2.0)) * 1.618033988749894848204586;
        high[biqs_reso] = pow(TRG + 0.618033988749894848204586, 2.0);
        double K = tan(M_PI * high[biqs_freq]);
        double norm = 1.0 / (1.0 + K / (high[biqs_reso] * 0.618033988749894848204586) + K * K);
        high[biqs_a0] = K / (high[biqs_reso] * 0.618033988749894848204586) * norm;
        high[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_b2] = (1.0 - K / (high[biqs_reso] * 0.618033988749894848204586) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 1.618033988749894848204586) + K * K);
        high[biqs_c0] = K / (high[biqs_reso] * 1.618033988749894848204586) * norm;
        high[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_d2] = (1.0 - K / (high[biqs_reso] * 1.618033988749894848204586) + K * K) * norm;
        // high
        hmid[biqs_freq] = (((pow(HMF, 3) * 7000.0) + 300.0) / Effect<T>::getSampleRate());
        if (hmid[biqs_freq] < 0.0001) {
            hmid[biqs_freq] = 0.0001;
        }
        hmid[biqs_bit] = (HMB * 2.0) - 1.0;
        hmid[biqs_level] = (1.0 - pow(1.0 - HMG, 2.0)) * 1.618033988749894848204586;
        hmid[biqs_reso] = pow(HMG + 0.618033988749894848204586, 2.0);
        K = tan(M_PI * hmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 0.618033988749894848204586) + K * K);
        hmid[biqs_a0] = K / (hmid[biqs_reso] * 0.618033988749894848204586) * norm;
        hmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_b2] = (1.0 - K / (hmid[biqs_reso] * 0.618033988749894848204586) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 1.618033988749894848204586) + K * K);
        hmid[biqs_c0] = K / (hmid[biqs_reso] * 1.618033988749894848204586) * norm;
        hmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_d2] = (1.0 - K / (hmid[biqs_reso] * 1.618033988749894848204586) + K * K) * norm;
        // hmid
        lmid[biqs_freq] = (((pow(LMF, 3) * 3000.0) + 20.0) / Effect<T>::getSampleRate());
        if (lmid[biqs_freq] < 0.00001) {
            lmid[biqs_freq] = 0.00001;
        }
        lmid[biqs_bit] = (LMB * 2.0) - 1.0;
        lmid[biqs_level] = (1.0 - pow(1.0 - LMG, 2.0)) * 1.618033988749894848204586;
        lmid[biqs_reso] = pow(LMG + 0.618033988749894848204586, 2.0);
        K = tan(M_PI * lmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 0.618033988749894848204586) + K * K);
        lmid[biqs_a0] = K / (lmid[biqs_reso] * 0.618033988749894848204586) * norm;
        lmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_b2] = (1.0 - K / (lmid[biqs_reso] * 0.618033988749894848204586) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 1.618033988749894848204586) + K * K);
        lmid[biqs_c0] = K / (lmid[biqs_reso] * 1.618033988749894848204586) * norm;
        lmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_d2] = (1.0 - K / (lmid[biqs_reso] * 1.618033988749894848204586) + K * K) * norm;
        // lmid
        double wet = DW;
        while (--sampleFrames >= 0) {
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
            // begin Stacked Biquad With Reversed Neutron Flow L
            high[biqs_outL] = inputSampleL * fabs(high[biqs_level]);
            high[biqs_temp] = (high[biqs_outL] * high[biqs_a0]) + high[biqs_aL1];
            high[biqs_aL1] = high[biqs_aL2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aL2] = (high[biqs_outL] * -high[biqs_a0]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outL] = high[biqs_temp];
            if (high[biqs_bit] != 0.0) {
                double bitFactor = high[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                high[biqs_outL] *= bitFactor;
                high[biqs_outL] = floor(high[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                high[biqs_outL] /= bitFactor;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_c0]) + high[biqs_cL1];
            high[biqs_cL1] = high[biqs_cL2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cL2] = (high[biqs_outL] * -high[biqs_c0]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_outL] *= high[biqs_level];
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            hmid[biqs_outL] = inputSampleL * fabs(hmid[biqs_level]);
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_a0]) + hmid[biqs_aL1];
            hmid[biqs_aL1] = hmid[biqs_aL2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aL2] = (hmid[biqs_outL] * -hmid[biqs_a0]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            if (hmid[biqs_bit] != 0.0) {
                double bitFactor = hmid[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                hmid[biqs_outL] *= bitFactor;
                hmid[biqs_outL] = floor(hmid[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                hmid[biqs_outL] /= bitFactor;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_c0]) + hmid[biqs_cL1];
            hmid[biqs_cL1] = hmid[biqs_cL2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cL2] = (hmid[biqs_outL] * -hmid[biqs_c0]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_outL] *= hmid[biqs_level];
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            lmid[biqs_outL] = inputSampleL * fabs(lmid[biqs_level]);
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_a0]) + lmid[biqs_aL1];
            lmid[biqs_aL1] = lmid[biqs_aL2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aL2] = (lmid[biqs_outL] * -lmid[biqs_a0]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            if (lmid[biqs_bit] != 0.0) {
                double bitFactor = lmid[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                lmid[biqs_outL] *= bitFactor;
                lmid[biqs_outL] = floor(lmid[biqs_outL] + (crushGate ? 0.5 / bitFactor : 0.0));
                lmid[biqs_outL] /= bitFactor;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_c0]) + lmid[biqs_cL1];
            lmid[biqs_cL1] = lmid[biqs_cL2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cL2] = (lmid[biqs_outL] * -lmid[biqs_c0]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_outL] *= lmid[biqs_level];
            double parametricL = high[biqs_outL] + hmid[biqs_outL] + lmid[biqs_outL];
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow R
            high[biqs_outR] = inputSampleR * fabs(high[biqs_level]);
            high[biqs_temp] = (high[biqs_outR] * high[biqs_a0]) + high[biqs_aR1];
            high[biqs_aR1] = high[biqs_aR2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aR2] = (high[biqs_outR] * -high[biqs_a0]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outR] = high[biqs_temp];
            if (high[biqs_bit] != 0.0) {
                double bitFactor = high[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                high[biqs_outR] *= bitFactor;
                high[biqs_outR] = floor(high[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                high[biqs_outR] /= bitFactor;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_c0]) + high[biqs_cR1];
            high[biqs_cR1] = high[biqs_cR2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cR2] = (high[biqs_outR] * -high[biqs_c0]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_outR] *= high[biqs_level];
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            hmid[biqs_outR] = inputSampleR * fabs(hmid[biqs_level]);
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_a0]) + hmid[biqs_aR1];
            hmid[biqs_aR1] = hmid[biqs_aR2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aR2] = (hmid[biqs_outR] * -hmid[biqs_a0]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            if (hmid[biqs_bit] != 0.0) {
                double bitFactor = hmid[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                hmid[biqs_outR] *= bitFactor;
                hmid[biqs_outR] = floor(hmid[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                hmid[biqs_outR] /= bitFactor;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_c0]) + hmid[biqs_cR1];
            hmid[biqs_cR1] = hmid[biqs_cR2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cR2] = (hmid[biqs_outR] * -hmid[biqs_c0]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_outR] *= hmid[biqs_level];
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            lmid[biqs_outR] = inputSampleR * fabs(lmid[biqs_level]);
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_a0]) + lmid[biqs_aR1];
            lmid[biqs_aR1] = lmid[biqs_aR2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aR2] = (lmid[biqs_outR] * -lmid[biqs_a0]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            if (lmid[biqs_bit] != 0.0) {
                double bitFactor = lmid[biqs_bit];
                bool crushGate = (bitFactor < 0.0);
                bitFactor = pow(2.0, fmin(fmax((1.0 - fabs(bitFactor)) * 16.0, 0.5), 16.0));
                lmid[biqs_outR] *= bitFactor;
                lmid[biqs_outR] = floor(lmid[biqs_outR] + (crushGate ? 0.5 / bitFactor : 0.0));
                lmid[biqs_outR] /= bitFactor;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_c0]) + lmid[biqs_cR1];
            lmid[biqs_cR1] = lmid[biqs_cR2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cR2] = (lmid[biqs_outR] * -lmid[biqs_c0]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_outR] *= lmid[biqs_level];
            double parametricR = high[biqs_outR] + hmid[biqs_outR] + lmid[biqs_outR];
            // end Stacked Biquad With Reversed Neutron Flow R
            inputSampleL = (drySampleL * (1.0 - wet)) + (parametricL * wet);
            inputSampleR = (drySampleR * (1.0 - wet)) + (parametricR * wet);
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
} // namespace airwindohhs::hipcrush
