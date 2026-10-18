#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::consolexpre {

constexpr std::string_view k_name{ "ConsoleXPre" };
constexpr std::string_view k_short_description{
    "ConsoleXPre is just the tone shaping from ConsoleX."
};
constexpr std::string_view k_long_description{
    "ConsoleX is my ultimate mixing system, whether for retro vibe or the next big thing, and I'll try to explain what I've built into it. There's too much here to tell all at once, so here are the highlights. The basic signal flow uses Stonefire's Kalman filters combined with Air3 for an air band, and applies the dynamics you'd find on a big-console channel strip, but parallel across Stone and Fire bands.That means it's easy to apply simple compression that still lets sub-lows through, or to tighten the deep bass with gating, or to do a variety of wild dynamics tricks. It's like two big-console dynamics sections in one (or, broad-band EQ-like tone shaping with a difference). Four EQ bands lend themselves to sharp, sonorous areas of focus. When not in use it's like no processing is there, but dig in and the bass, low mid, high mid and treble really thump, punch, honk and glitter. The EQ design's similar to some things about ToTape7 and 8's head bump, but more adaptable. Each band is fed by raw sound pre-compression, so they saturate the audio with peak energy, but they gate along with Stone or Fire so you can still clamp 'em if needed. There's a special trick for those willing to run at elevated sample rates: the lowpass and highpass are distributed, spread out through the signal flow of the whole plugin. They bypass when turned all the way down (these things follow SSL rules, like no highpass or lowpass being counterclockwise, and the narrowest Q of the EQs being counterclockwise) but when you crack open the lowpass it begins doing a supersonic filtering that'll directly address aliasing through the plugin and the whole mix, while allowing nonlinearities to produce the harmonics you'd get off analog gear. There's a special knob, Discontinuity. It's available as a standalone plugin, but ConsoleX puts Discontinuity on every channel and on the 2-buss. Hard counterclockwise means you're applying air nonlinearity as if your sound topped out at 70 dB: in other words, practically none. Middle is around 100 dB and you'll begin to feel the effects. Beyond that, you're getting into air distortion beyond the hugest PA systems, just because it might be fun. This is your key to making stuff sound huge, sound loud, sound completely beyond what we're used to hearing from DAWs of the last thirty years. Treat it like dynamics: wisely, and with caution. It's there to set the peak apparent loudness of your track. There's a vibey meter based on what I learned from Airwindows Meter, but instead of producing charts and measurements to study, it produces a light show. Blue light represents sonority at whatever volume those peaks are at (quiet is at the center, clipping is the edges of the meter). Where Meter draws red and green dots on white to represent peaks brighter or mellower than the optimum, the ConsoleX meter sustains those lights and reverses them, against a background of black. That means if you see red light, that's bassy. If you see green flashes, that's treble. And if the meter lights up a brilliant cyan, that's the bright end of maximum intensity. So you can watch for red or purple if you're looking for bass, greens and cyans if you want brightness, blue for the loudest parts, and you won't get over-analytical about it. The ConsoleX meter shows you roughly how loud the peaks are, and their character, in a way you can vibe to without losing mixing focus. There's a ConsoleXPre plugin, which is all the tone shaping stuff of the Channels and only lacks the Console processing. This is for if you want to run ToTape7 or 8 on channels for things like heavy guitars, but you want to condition the sound going TO the tape, and then also have full control over the sound coming back OFF the tape. There's great missives and narratives by the ever-respected Slipperman on how to do just this, and it absolutely works in the box using ToTape and these plugins. You can also run things like reverbs into other full ConsoleXChannels, by putting Console9Buss at the top of that aux channel, adding the reverb or what have you, then back to ConsoleXChannel to fully control and augment the processing (that's something I saw Chris Lord-Alge doing in videos, and it's fantastic) Lastly, every single thing I've mentioned exists exactly the same way on the Buss plugin (decoding the output of the Channel plugins, please maintain unity gain between Channel and Buss) so anything you can do on a channel you can do on the 2-buss. This drastically changes the character of the mix, making ConsoleX future-proof: it ought to be able to handle anything you can dream up. And then, I'm introducing AirwindowsGlobals, a user-accessible configuring text file that lets you do completely outrageous things with the look of all Airwindows Pamplejuce-based plugins. And you'd think that would mean 'the ConsoleX plugins', except that Airwindows Meter already works with AirwindowsGlobals. Surprise! There will be more. And for my fellow Reaper users, with the help of Airwindows fan Robert Kennedy, I bring you Airwindowmation. This is a Reaper script, that should work for Mac and Windows directly if you can install ReaLearn and get Reaper scripts working on your machine and sort out the trickiness of control surfaces. I simplified it as much as I could. It is not actually automation, it's grouping. You can take a bank of faders (like the 12 long-throw faders on my Yaeltex dedicated ConsoleX control surface) and colorcode them, and then have a given colored fader automatically control the 'Fader' parameter on ALL ConsoleXChannel instances with a matching track color, on the fly. Much like how you can update AirwindowsGlobals on the fly. It's deeply spontaneous, which I think is essential for modern human music-making. I'll be doing some livestreams for the purpose of walking through mixes using ConsoleX, and will resume regular plugin posting in January at some point. I look forward to sharing all this with everyone… and diving into the music making like I haven't been able to do for years. Join me!"
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class ConsoleXPre final : public Effect<T>
{
    static constexpr int dscBuf = 90;
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'cnxp';
    float HIP;
    float LOP;
    float AIR;
    float FIR;
    float STO;
    float RNG;
    float FCT;
    float SCT;
    float FCR;
    float SCR;
    float FCA;
    float SCA;
    float FCL;
    float SCL;
    float FGT;
    float SGT;
    float FGR;
    float SGR;
    float FGS;
    float SGS;
    float FGL;
    float SGL;
    float TRF;
    float TRG;
    float TRR;
    float HMF;
    float HMG;
    float HMR;
    float LMF;
    float LMG;
    float LMR;
    float BSF;
    float BSG;
    float BSR;
    float DSC;
    float PAN;
    float FAD;
    enum
    {
        hilp_freq,
        hilp_temp,
        hilp_a0,
        hilp_a1,
        hilp_b1,
        hilp_b2,
        hilp_c0,
        hilp_c1,
        hilp_d1,
        hilp_d2,
        hilp_e0,
        hilp_e1,
        hilp_f1,
        hilp_f2,
        hilp_aL1,
        hilp_aL2,
        hilp_aR1,
        hilp_aR2,
        hilp_cL1,
        hilp_cL2,
        hilp_cR1,
        hilp_cR2,
        hilp_eL1,
        hilp_eL2,
        hilp_eR1,
        hilp_eR2,
        hilp_total
    };
    double highpass[hilp_total];
    double lowpass[hilp_total];
    enum
    {
        pvAL1,
        pvSL1,
        accSL1,
        acc2SL1,
        pvAL2,
        pvSL2,
        accSL2,
        acc2SL2,
        pvAL3,
        pvSL3,
        accSL3,
        pvAL4,
        pvSL4,
        gndavgL,
        outAL,
        gainAL,
        pvAR1,
        pvSR1,
        accSR1,
        acc2SR1,
        pvAR2,
        pvSR2,
        accSR2,
        acc2SR2,
        pvAR3,
        pvSR3,
        accSR3,
        pvAR4,
        pvSR4,
        gndavgR,
        outAR,
        gainAR,
        air_total
    };
    double air[air_total];
    enum
    {
        prevSampL1,
        prevSlewL1,
        accSlewL1,
        prevSampL2,
        prevSlewL2,
        accSlewL2,
        prevSampL3,
        prevSlewL3,
        accSlewL3,
        kalGainL,
        kalOutL,
        prevSampR1,
        prevSlewR1,
        accSlewR1,
        prevSampR2,
        prevSlewR2,
        accSlewR2,
        prevSampR3,
        prevSlewR3,
        accSlewR3,
        kalGainR,
        kalOutR,
        kal_total
    };
    double kal[kal_total];
    double fireCompL;
    double fireCompR;
    double fireGate;
    double stoneCompL;
    double stoneCompR;
    double stoneGate;
    double airGainA;
    double airGainB;
    double fireGainA;
    double fireGainB;
    double stoneGainA;
    double stoneGainB;
    enum
    {
        biqs_freq,
        biqs_reso,
        biqs_level,
        biqs_nonlin,
        biqs_temp,
        biqs_dis,
        biqs_a0,
        biqs_a1,
        biqs_b1,
        biqs_b2,
        biqs_c0,
        biqs_c1,
        biqs_d1,
        biqs_d2,
        biqs_e0,
        biqs_e1,
        biqs_f1,
        biqs_f2,
        biqs_aL1,
        biqs_aL2,
        biqs_aR1,
        biqs_aR2,
        biqs_cL1,
        biqs_cL2,
        biqs_cR1,
        biqs_cR2,
        biqs_eL1,
        biqs_eL2,
        biqs_eR1,
        biqs_eR2,
        biqs_outL,
        biqs_outR,
        biqs_total
    };
    double high[biqs_total];
    double hmid[biqs_total];
    double lmid[biqs_total];
    double bass[biqs_total];
    double dBaL[dscBuf + 5];
    double dBaR[dscBuf + 5];
    double dBaPosL;
    double dBaPosR;
    int dBaXL;
    int dBaXR;
    double panA;
    double panB;
    double inTrimA;
    double inTrimB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    ConsoleXPre()
    {
        HIP = 0.0;
        LOP = 0.0;
        AIR = 0.5;
        FIR = 0.5;
        STO = 0.5;
        RNG = 0.5;
        FCT = 1.0;
        SCT = 1.0;
        FCR = 1.0;
        SCR = 1.0;
        FCA = 0.5;
        SCA = 0.5;
        FCL = 0.5;
        SCL = 0.5;
        FGT = 0.0;
        SGT = 0.0;
        FGR = 1.0;
        SGR = 1.0;
        FGS = 0.5;
        SGS = 0.5;
        FGL = 0.5;
        SGL = 0.5;
        TRF = 0.5;
        TRG = 0.5;
        TRR = 0.5;
        HMF = 0.5;
        HMG = 0.5;
        HMR = 0.5;
        LMF = 0.5;
        LMG = 0.5;
        LMR = 0.5;
        BSF = 0.5;
        BSG = 0.5;
        BSR = 0.5;
        DSC = 0.5;
        PAN = 0.5;
        FAD = 0.5;
        for (int x = 0; x < hilp_total; x++) {
            highpass[x] = 0.0;
            lowpass[x] = 0.0;
        }
        for (int x = 0; x < air_total; x++) {
            air[x] = 0.0;
        }
        for (int x = 0; x < kal_total; x++) {
            kal[x] = 0.0;
        }
        fireCompL = 1.0;
        fireCompR = 1.0;
        fireGate = 1.0;
        stoneCompL = 1.0;
        stoneCompR = 1.0;
        stoneGate = 1.0;
        for (int x = 0; x < biqs_total; x++) {
            high[x] = 0.0;
            hmid[x] = 0.0;
            lmid[x] = 0.0;
            bass[x] = 0.0;
        }
        for (int count = 0; count < dscBuf + 2; count++) {
            dBaL[count] = 0.0;
            dBaR[count] = 0.0;
        }
        dBaPosL = 0.0;
        dBaPosR = 0.0;
        dBaXL = 1;
        dBaXR = 1;
        airGainA = 0.5;
        airGainB = 0.5;
        fireGainA = 0.5;
        fireGainB = 0.5;
        stoneGainA = 0.5;
        stoneGainB = 0.5;
        panA = 0.5;
        panB = 0.5;
        inTrimA = 1.0;
        inTrimB = 1.0;
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
        kParamK = 10,
        kParamL = 11,
        kParamM = 12,
        kParamN = 13,
        kParamO = 14,
        kParamP = 15,
        kParamQ = 16,
        kParamR = 17,
        kParamS = 18,
        kParamT = 19,
        kParamU = 20,
        kParamV = 21,
        kParamW = 22,
        kParamX = 23,
        kParamY = 24,
        kParamZ = 25,
        kParamAA = 26,
        kParamAB = 27,
        kParamAC = 28,
        kParamAD = 29,
        kParamAE = 30,
        kParamAF = 31,
        kParamAG = 32,
        kParamAH = 33,
        kParamAI = 34,
        kParamAJ = 35,
        kParamAK = 36,
        kNumParameters = 37
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: HIP = value; break;
            case kParamB: LOP = value; break;
            case kParamC: AIR = value; break;
            case kParamD: FIR = value; break;
            case kParamE: STO = value; break;
            case kParamF: RNG = value; break;
            case kParamG: FCT = value; break;
            case kParamH: SCT = value; break;
            case kParamI: FCR = value; break;
            case kParamJ: SCR = value; break;
            case kParamK: FCA = value; break;
            case kParamL: SCA = value; break;
            case kParamM: FCL = value; break;
            case kParamN: SCL = value; break;
            case kParamO: FGT = value; break;
            case kParamP: SGT = value; break;
            case kParamQ: FGR = value; break;
            case kParamR: SGR = value; break;
            case kParamS: FGS = value; break;
            case kParamT: SGS = value; break;
            case kParamU: FGL = value; break;
            case kParamV: SGL = value; break;
            case kParamW: TRF = value; break;
            case kParamX: TRG = value; break;
            case kParamY: TRR = value; break;
            case kParamZ: HMF = value; break;
            case kParamAA: HMG = value; break;
            case kParamAB: HMR = value; break;
            case kParamAC: LMF = value; break;
            case kParamAD: LMG = value; break;
            case kParamAE: LMR = value; break;
            case kParamAF: BSF = value; break;
            case kParamAG: BSG = value; break;
            case kParamAH: BSR = value; break;
            case kParamAI: DSC = value; break;
            case kParamAJ: PAN = value; break;
            case kParamAK: FAD = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return HIP; break;
            case kParamB: return LOP; break;
            case kParamC: return AIR; break;
            case kParamD: return FIR; break;
            case kParamE: return STO; break;
            case kParamF: return RNG; break;
            case kParamG: return FCT; break;
            case kParamH: return SCT; break;
            case kParamI: return FCR; break;
            case kParamJ: return SCR; break;
            case kParamK: return FCA; break;
            case kParamL: return SCA; break;
            case kParamM: return FCL; break;
            case kParamN: return SCL; break;
            case kParamO: return FGT; break;
            case kParamP: return SGT; break;
            case kParamQ: return FGR; break;
            case kParamR: return SGR; break;
            case kParamS: return FGS; break;
            case kParamT: return SGS; break;
            case kParamU: return FGL; break;
            case kParamV: return SGL; break;
            case kParamW: return TRF; break;
            case kParamX: return TRG; break;
            case kParamY: return TRR; break;
            case kParamZ: return HMF; break;
            case kParamAA: return HMG; break;
            case kParamAB: return HMR; break;
            case kParamAC: return LMF; break;
            case kParamAD: return LMG; break;
            case kParamAE: return LMR; break;
            case kParamAF: return BSF; break;
            case kParamAG: return BSG; break;
            case kParamAH: return BSR; break;
            case kParamAI: return DSC; break;
            case kParamAJ: return PAN; break;
            case kParamAK: return FAD; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.5; break;
            case kParamG: return 1.0; break;
            case kParamH: return 1.0; break;
            case kParamI: return 1.0; break;
            case kParamJ: return 1.0; break;
            case kParamK: return 0.5; break;
            case kParamL: return 0.5; break;
            case kParamM: return 0.5; break;
            case kParamN: return 0.5; break;
            case kParamO: return 0.0; break;
            case kParamP: return 0.0; break;
            case kParamQ: return 1.0; break;
            case kParamR: return 1.0; break;
            case kParamS: return 0.5; break;
            case kParamT: return 0.5; break;
            case kParamU: return 0.5; break;
            case kParamV: return 0.5; break;
            case kParamW: return 0.5; break;
            case kParamX: return 0.5; break;
            case kParamY: return 0.5; break;
            case kParamZ: return 0.5; break;
            case kParamAA: return 0.5; break;
            case kParamAB: return 0.5; break;
            case kParamAC: return 0.5; break;
            case kParamAD: return 0.5; break;
            case kParamAE: return 0.5; break;
            case kParamAF: return 0.5; break;
            case kParamAG: return 0.5; break;
            case kParamAH: return 0.5; break;
            case kParamAI: return 0.5; break;
            case kParamAJ: return 0.5; break;
            case kParamAK: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "highpas"; break;
            case kParamB: return "lowpass"; break;
            case kParamC: return "air"; break;
            case kParamD: return "fire"; break;
            case kParamE: return "stone"; break;
            case kParamF: return "range"; break;
            case kParamG: return "fcthrs"; break;
            case kParamH: return "scthrs"; break;
            case kParamI: return "fcrati"; break;
            case kParamJ: return "scrati"; break;
            case kParamK: return "fcatk"; break;
            case kParamL: return "scatk"; break;
            case kParamM: return "fcrls"; break;
            case kParamN: return "scrls"; break;
            case kParamO: return "fgthrs"; break;
            case kParamP: return "sgthrs"; break;
            case kParamQ: return "fgrati"; break;
            case kParamR: return "sgrati"; break;
            case kParamS: return "fgsust"; break;
            case kParamT: return "sgsust"; break;
            case kParamU: return "fgrls"; break;
            case kParamV: return "sgrls"; break;
            case kParamW: return "trfreq"; break;
            case kParamX: return "treble"; break;
            case kParamY: return "trreso"; break;
            case kParamZ: return "hmfreq"; break;
            case kParamAA: return "highmid"; break;
            case kParamAB: return "hmreso"; break;
            case kParamAC: return "lmfreq"; break;
            case kParamAD: return "lowmid"; break;
            case kParamAE: return "lmreso"; break;
            case kParamAF: return "bsfreq"; break;
            case kParamAG: return "bass"; break;
            case kParamAH: return "bsreso"; break;
            case kParamAI: return "topdb"; break;
            case kParamAJ: return "pan"; break;
            case kParamAK: return "fader"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Highpas"; break;
            case kParamB: return "Lowpass"; break;
            case kParamC: return "Air"; break;
            case kParamD: return "Fire"; break;
            case kParamE: return "Stone"; break;
            case kParamF: return "Range"; break;
            case kParamG: return "FC Thrs"; break;
            case kParamH: return "SC Thrs"; break;
            case kParamI: return "FC Rati"; break;
            case kParamJ: return "SC Rati"; break;
            case kParamK: return "FC Atk"; break;
            case kParamL: return "SC Atk"; break;
            case kParamM: return "FC Rls"; break;
            case kParamN: return "SC Rls"; break;
            case kParamO: return "FG Thrs"; break;
            case kParamP: return "SG Thrs"; break;
            case kParamQ: return "FG Rati"; break;
            case kParamR: return "SG Rati"; break;
            case kParamS: return "FG Sust"; break;
            case kParamT: return "SG Sust"; break;
            case kParamU: return "FG Rls"; break;
            case kParamV: return "SG Rls"; break;
            case kParamW: return "Tr Freq"; break;
            case kParamX: return "Treble"; break;
            case kParamY: return "Tr Reso"; break;
            case kParamZ: return "HM Freq"; break;
            case kParamAA: return "HighMid"; break;
            case kParamAB: return "HM Reso"; break;
            case kParamAC: return "LM Freq"; break;
            case kParamAD: return "LowMid"; break;
            case kParamAE: return "LM Reso"; break;
            case kParamAF: return "Bs Freq"; break;
            case kParamAG: return "Bass"; break;
            case kParamAH: return "Bs Reso"; break;
            case kParamAI: return "Top dB"; break;
            case kParamAJ: return "Pan"; break;
            case kParamAK: return "Fader"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: float2string(HIP, text, kVstMaxParamStrLen); break;
            case kParamB: float2string(LOP, text, kVstMaxParamStrLen); break;
            case kParamC: float2string(AIR, text, kVstMaxParamStrLen); break;
            case kParamD: float2string(FIR, text, kVstMaxParamStrLen); break;
            case kParamE: float2string(STO, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(RNG, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(FCT, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(SCT, text, kVstMaxParamStrLen); break;
            case kParamI: float2string(FCR, text, kVstMaxParamStrLen); break;
            case kParamJ: float2string(SCR, text, kVstMaxParamStrLen); break;
            case kParamK: float2string(FCA, text, kVstMaxParamStrLen); break;
            case kParamL: float2string(SCA, text, kVstMaxParamStrLen); break;
            case kParamM: float2string(FCL, text, kVstMaxParamStrLen); break;
            case kParamN: float2string(SCL, text, kVstMaxParamStrLen); break;
            case kParamO: float2string(FGT, text, kVstMaxParamStrLen); break;
            case kParamP: float2string(SGT, text, kVstMaxParamStrLen); break;
            case kParamQ: float2string(FGR, text, kVstMaxParamStrLen); break;
            case kParamR: float2string(SGR, text, kVstMaxParamStrLen); break;
            case kParamS: float2string(FGS, text, kVstMaxParamStrLen); break;
            case kParamT: float2string(SGS, text, kVstMaxParamStrLen); break;
            case kParamU: float2string(FGL, text, kVstMaxParamStrLen); break;
            case kParamV: float2string(SGL, text, kVstMaxParamStrLen); break;
            case kParamW: float2string(TRF, text, kVstMaxParamStrLen); break;
            case kParamX: float2string(TRG, text, kVstMaxParamStrLen); break;
            case kParamY: float2string(TRR, text, kVstMaxParamStrLen); break;
            case kParamZ: float2string(HMF, text, kVstMaxParamStrLen); break;
            case kParamAA: float2string(HMG, text, kVstMaxParamStrLen); break;
            case kParamAB: float2string(HMR, text, kVstMaxParamStrLen); break;
            case kParamAC: float2string(LMF, text, kVstMaxParamStrLen); break;
            case kParamAD: float2string(LMG, text, kVstMaxParamStrLen); break;
            case kParamAE: float2string(LMR, text, kVstMaxParamStrLen); break;
            case kParamAF: float2string(BSF, text, kVstMaxParamStrLen); break;
            case kParamAG: float2string(BSG, text, kVstMaxParamStrLen); break;
            case kParamAH: float2string(BSR, text, kVstMaxParamStrLen); break;
            case kParamAI: float2string((DSC * 70.0) + 70.0, text, kVstMaxParamStrLen); break;
            case kParamAJ: float2string(PAN, text, kVstMaxParamStrLen); break;
            case kParamAK: float2string(FAD, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamI: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamK: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamL: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamM: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamN: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamO: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamP: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamQ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamR: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamS: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamT: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamU: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamV: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamW: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamX: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamY: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamZ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAI: vst_strncpy(text, "dB", kVstMaxParamStrLen); break;
            case kParamAJ: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamAK: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        VstInt32 inFramesToProcess = sampleFrames; // vst doesn't give us this as a separate variable so we'll make it
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        highpass[hilp_freq] = ((HIP * 330.0) + 20.0) / Effect<T>::getSampleRate();
        bool highpassEngage = true;
        if (HIP == 0.0) {
            highpassEngage = false;
        }
        lowpass[hilp_freq] = ((pow(1.0 - LOP, 2) * 17000.0) + 3000.0) / Effect<T>::getSampleRate();
        bool lowpassEngage = true;
        if (LOP == 0.0) {
            lowpassEngage = false;
        }
        double K = tan(M_PI * highpass[hilp_freq]); // highpass
        double norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
        highpass[hilp_a0] = norm;
        highpass[hilp_a1] = -2.0 * highpass[hilp_a0];
        highpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
        highpass[hilp_c0] = norm;
        highpass[hilp_c1] = -2.0 * highpass[hilp_c0];
        highpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
        highpass[hilp_e0] = norm;
        highpass[hilp_e1] = -2.0 * highpass[hilp_e0];
        highpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
        highpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
        K = tan(M_PI * lowpass[hilp_freq]); // lowpass
        norm = 1.0 / (1.0 + K / 1.93185165 + K * K);
        lowpass[hilp_a0] = K * K * norm;
        lowpass[hilp_a1] = 2.0 * lowpass[hilp_a0];
        lowpass[hilp_b1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_b2] = (1.0 - K / 1.93185165 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.70710678 + K * K);
        lowpass[hilp_c0] = K * K * norm;
        lowpass[hilp_c1] = 2.0 * lowpass[hilp_c0];
        lowpass[hilp_d1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_d2] = (1.0 - K / 0.70710678 + K * K) * norm;
        norm = 1.0 / (1.0 + K / 0.51763809 + K * K);
        lowpass[hilp_e0] = K * K * norm;
        lowpass[hilp_e1] = 2.0 * lowpass[hilp_e0];
        lowpass[hilp_f1] = 2.0 * (K * K - 1.0) * norm;
        lowpass[hilp_f2] = (1.0 - K / 0.51763809 + K * K) * norm;
        airGainA = airGainB;
        airGainB = AIR * 2.0;
        fireGainA = fireGainB;
        fireGainB = FIR * 2.0;
        stoneGainA = stoneGainB;
        stoneGainB = STO * 2.0;
        // simple three band to adjust
        double kalmanRange = 1.0 - (pow(RNG, 2) / overallscale);
        // crossover frequency between mid/bass
        double compFThresh = pow(FCT, 4);
        double compSThresh = pow(SCT, 4);
        double compFRatio = 1.0 - pow(1.0 - FCR, 2);
        double compSRatio = 1.0 - pow(1.0 - SCR, 2);
        double compFAttack = 1.0 / (((pow(FCA, 3) * 5000.0) + 500.0) * overallscale);
        double compSAttack = 1.0 / (((pow(SCA, 3) * 5000.0) + 500.0) * overallscale);
        double compFRelease = 1.0 / (((pow(FCL, 5) * 50000.0) + 500.0) * overallscale);
        double compSRelease = 1.0 / (((pow(SCL, 5) * 50000.0) + 500.0) * overallscale);
        double gateFThresh = pow(FGT, 4);
        double gateSThresh = pow(SGT, 4);
        double gateFRatio = 1.0 - pow(1.0 - FGR, 2);
        double gateSRatio = 1.0 - pow(1.0 - SGR, 2);
        double gateFSustain = M_PI_2 * pow(FGS + 1.0, 4.0);
        double gateSSustain = M_PI_2 * pow(SGS + 1.0, 4.0);
        double gateFRelease = 1.0 / (((pow(FGL, 5) * 500000.0) + 500.0) * overallscale);
        double gateSRelease = 1.0 / (((pow(SGL, 5) * 500000.0) + 500.0) * overallscale);
        high[biqs_freq] = (((pow(TRF, 3) * 14500.0) + 1500.0) / Effect<T>::getSampleRate());
        if (high[biqs_freq] < 0.0001) {
            high[biqs_freq] = 0.0001;
        }
        high[biqs_nonlin] = TRG;
        high[biqs_level] = (high[biqs_nonlin] * 2.0) - 1.0;
        if (high[biqs_level] > 0.0) {
            high[biqs_level] *= 2.0;
        }
        high[biqs_reso] = ((0.5 + (high[biqs_nonlin] * 0.5) + sqrt(high[biqs_freq])) - (1.0 - pow(1.0 - TRR, 2.0))) + 0.5 + (high[biqs_nonlin] * 0.5);
        K = tan(M_PI * high[biqs_freq]);
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 1.93185165) + K * K);
        high[biqs_a0] = K / (high[biqs_reso] * 1.93185165) * norm;
        high[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_b2] = (1.0 - K / (high[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 0.70710678) + K * K);
        high[biqs_c0] = K / (high[biqs_reso] * 0.70710678) * norm;
        high[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_d2] = (1.0 - K / (high[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (high[biqs_reso] * 0.51763809) + K * K);
        high[biqs_e0] = K / (high[biqs_reso] * 0.51763809) * norm;
        high[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        high[biqs_f2] = (1.0 - K / (high[biqs_reso] * 0.51763809) + K * K) * norm;
        // high
        hmid[biqs_freq] = (((pow(HMF, 3) * 6400.0) + 600.0) / Effect<T>::getSampleRate());
        if (hmid[biqs_freq] < 0.0001) {
            hmid[biqs_freq] = 0.0001;
        }
        hmid[biqs_nonlin] = HMG;
        hmid[biqs_level] = (hmid[biqs_nonlin] * 2.0) - 1.0;
        if (hmid[biqs_level] > 0.0) {
            hmid[biqs_level] *= 2.0;
        }
        hmid[biqs_reso] = ((0.5 + (hmid[biqs_nonlin] * 0.5) + sqrt(hmid[biqs_freq])) - (1.0 - pow(1.0 - HMR, 2.0))) + 0.5 + (hmid[biqs_nonlin] * 0.5);
        K = tan(M_PI * hmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 1.93185165) + K * K);
        hmid[biqs_a0] = K / (hmid[biqs_reso] * 1.93185165) * norm;
        hmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_b2] = (1.0 - K / (hmid[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 0.70710678) + K * K);
        hmid[biqs_c0] = K / (hmid[biqs_reso] * 0.70710678) * norm;
        hmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_d2] = (1.0 - K / (hmid[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (hmid[biqs_reso] * 0.51763809) + K * K);
        hmid[biqs_e0] = K / (hmid[biqs_reso] * 0.51763809) * norm;
        hmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        hmid[biqs_f2] = (1.0 - K / (hmid[biqs_reso] * 0.51763809) + K * K) * norm;
        // hmid
        lmid[biqs_freq] = (((pow(LMF, 3) * 2200.0) + 200.0) / Effect<T>::getSampleRate());
        if (lmid[biqs_freq] < 0.0001) {
            lmid[biqs_freq] = 0.0001;
        }
        lmid[biqs_nonlin] = LMG;
        lmid[biqs_level] = (lmid[biqs_nonlin] * 2.0) - 1.0;
        if (lmid[biqs_level] > 0.0) {
            lmid[biqs_level] *= 2.0;
        }
        lmid[biqs_reso] = ((0.5 + (lmid[biqs_nonlin] * 0.5) + sqrt(lmid[biqs_freq])) - (1.0 - pow(1.0 - LMR, 2.0))) + 0.5 + (lmid[biqs_nonlin] * 0.5);
        K = tan(M_PI * lmid[biqs_freq]);
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 1.93185165) + K * K);
        lmid[biqs_a0] = K / (lmid[biqs_reso] * 1.93185165) * norm;
        lmid[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_b2] = (1.0 - K / (lmid[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 0.70710678) + K * K);
        lmid[biqs_c0] = K / (lmid[biqs_reso] * 0.70710678) * norm;
        lmid[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_d2] = (1.0 - K / (lmid[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (lmid[biqs_reso] * 0.51763809) + K * K);
        lmid[biqs_e0] = K / (lmid[biqs_reso] * 0.51763809) * norm;
        lmid[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        lmid[biqs_f2] = (1.0 - K / (lmid[biqs_reso] * 0.51763809) + K * K) * norm;
        // lmid
        bass[biqs_freq] = (((pow(BSF, 3) * 570.0) + 30.0) / Effect<T>::getSampleRate());
        if (bass[biqs_freq] < 0.0001) {
            bass[biqs_freq] = 0.0001;
        }
        bass[biqs_nonlin] = BSG;
        bass[biqs_level] = (bass[biqs_nonlin] * 2.0) - 1.0;
        if (bass[biqs_level] > 0.0) {
            bass[biqs_level] *= 2.0;
        }
        bass[biqs_reso] = ((0.5 + (bass[biqs_nonlin] * 0.5) + sqrt(bass[biqs_freq])) - (1.0 - pow(1.0 - BSR, 2.0))) + 0.5 + (bass[biqs_nonlin] * 0.5);
        K = tan(M_PI * bass[biqs_freq]);
        norm = 1.0 / (1.0 + K / (bass[biqs_reso] * 1.93185165) + K * K);
        bass[biqs_a0] = K / (bass[biqs_reso] * 1.93185165) * norm;
        bass[biqs_b1] = 2.0 * (K * K - 1.0) * norm;
        bass[biqs_b2] = (1.0 - K / (bass[biqs_reso] * 1.93185165) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (bass[biqs_reso] * 0.70710678) + K * K);
        bass[biqs_c0] = K / (bass[biqs_reso] * 0.70710678) * norm;
        bass[biqs_d1] = 2.0 * (K * K - 1.0) * norm;
        bass[biqs_d2] = (1.0 - K / (bass[biqs_reso] * 0.70710678) + K * K) * norm;
        norm = 1.0 / (1.0 + K / (bass[biqs_reso] * 0.51763809) + K * K);
        bass[biqs_e0] = K / (bass[biqs_reso] * 0.51763809) * norm;
        bass[biqs_f1] = 2.0 * (K * K - 1.0) * norm;
        bass[biqs_f2] = (1.0 - K / (bass[biqs_reso] * 0.51763809) + K * K) * norm;
        // bass
        double refdB = (DSC * 70.0) + 70.0;
        double topdB = 0.000000075 * pow(10.0, refdB / 20.0) * overallscale;
        panA = panB;
        panB = PAN * 1.57079633;
        inTrimA = inTrimB;
        inTrimB = FAD * 2.0;
        while (--sampleFrames >= 0) {
            long double inputSampleL = *in1;
            long double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_a0]) + highpass[hilp_aL1];
                highpass[hilp_aL1] = (inputSampleL * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aL2];
                highpass[hilp_aL2] = (inputSampleL * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_a0]) + highpass[hilp_aR1];
                highpass[hilp_aR1] = (inputSampleR * highpass[hilp_a1]) - (highpass[hilp_temp] * highpass[hilp_b1]) + highpass[hilp_aR2];
                highpass[hilp_aR2] = (inputSampleR * highpass[hilp_a0]) - (highpass[hilp_temp] * highpass[hilp_b2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_aR1] = highpass[hilp_aR2] = highpass[hilp_aL1] = highpass[hilp_aL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_a0]) + lowpass[hilp_aL1];
                lowpass[hilp_aL1] = (inputSampleL * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aL2];
                lowpass[hilp_aL2] = (inputSampleL * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_a0]) + lowpass[hilp_aR1];
                lowpass[hilp_aR1] = (inputSampleR * lowpass[hilp_a1]) - (lowpass[hilp_temp] * lowpass[hilp_b1]) + lowpass[hilp_aR2];
                lowpass[hilp_aR2] = (inputSampleR * lowpass[hilp_a0]) - (lowpass[hilp_temp] * lowpass[hilp_b2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_aR1] = lowpass[hilp_aR2] = lowpass[hilp_aL1] = lowpass[hilp_aL2] = 0.0;
            }
            // first Highpass/Lowpass blocks aliasing before the nonlinearity of ConsoleXBuss and Parametric
            // get all Parametric bands before any other processing is done
            // begin Stacked Biquad With Reversed Neutron Flow L
            high[biqs_outL] = inputSampleL * fabs(high[biqs_level]);
            high[biqs_dis] = fabs(high[biqs_a0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_aL1];
            high[biqs_aL1] = high[biqs_aL2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_c0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_cL1];
            high[biqs_cL1] = high[biqs_cL2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_e0] * (1.0 + (high[biqs_outL] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outL] * high[biqs_dis]) + high[biqs_eL1];
            high[biqs_eL1] = high[biqs_eL2] - (high[biqs_temp] * high[biqs_f1]);
            high[biqs_eL2] = (high[biqs_outL] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_f2]);
            high[biqs_outL] = high[biqs_temp];
            high[biqs_outL] *= high[biqs_level];
            if (high[biqs_level] > 1.0) {
                high[biqs_outL] *= high[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            hmid[biqs_outL] = inputSampleL * fabs(hmid[biqs_level]);
            hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_aL1];
            hmid[biqs_aL1] = hmid[biqs_aL2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_cL1];
            hmid[biqs_cL1] = hmid[biqs_cL2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0 + (hmid[biqs_outL] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outL] * hmid[biqs_dis]) + hmid[biqs_eL1];
            hmid[biqs_eL1] = hmid[biqs_eL2] - (hmid[biqs_temp] * hmid[biqs_f1]);
            hmid[biqs_eL2] = (hmid[biqs_outL] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_f2]);
            hmid[biqs_outL] = hmid[biqs_temp];
            hmid[biqs_outL] *= hmid[biqs_level];
            if (hmid[biqs_level] > 1.0) {
                hmid[biqs_outL] *= hmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            lmid[biqs_outL] = inputSampleL * fabs(lmid[biqs_level]);
            lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_aL1];
            lmid[biqs_aL1] = lmid[biqs_aL2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_cL1];
            lmid[biqs_cL1] = lmid[biqs_cL2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0 + (lmid[biqs_outL] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outL] * lmid[biqs_dis]) + lmid[biqs_eL1];
            lmid[biqs_eL1] = lmid[biqs_eL2] - (lmid[biqs_temp] * lmid[biqs_f1]);
            lmid[biqs_eL2] = (lmid[biqs_outL] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_f2]);
            lmid[biqs_outL] = lmid[biqs_temp];
            lmid[biqs_outL] *= lmid[biqs_level];
            if (lmid[biqs_level] > 1.0) {
                lmid[biqs_outL] *= lmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow L
            bass[biqs_outL] = inputSampleL * fabs(bass[biqs_level]);
            bass[biqs_dis] = fabs(bass[biqs_a0] * (1.0 + (bass[biqs_outL] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_aL1];
            bass[biqs_aL1] = bass[biqs_aL2] - (bass[biqs_temp] * bass[biqs_b1]);
            bass[biqs_aL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_b2]);
            bass[biqs_outL] = bass[biqs_temp];
            bass[biqs_dis] = fabs(bass[biqs_c0] * (1.0 + (bass[biqs_outL] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_cL1];
            bass[biqs_cL1] = bass[biqs_cL2] - (bass[biqs_temp] * bass[biqs_d1]);
            bass[biqs_cL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_d2]);
            bass[biqs_outL] = bass[biqs_temp];
            bass[biqs_dis] = fabs(bass[biqs_e0] * (1.0 + (bass[biqs_outL] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outL] * bass[biqs_dis]) + bass[biqs_eL1];
            bass[biqs_eL1] = bass[biqs_eL2] - (bass[biqs_temp] * bass[biqs_f1]);
            bass[biqs_eL2] = (bass[biqs_outL] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_f2]);
            bass[biqs_outL] = bass[biqs_temp];
            bass[biqs_outL] *= bass[biqs_level];
            if (bass[biqs_level] > 1.0) {
                bass[biqs_outL] *= bass[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow L
            // begin Stacked Biquad With Reversed Neutron Flow R
            high[biqs_outR] = inputSampleR * fabs(high[biqs_level]);
            high[biqs_dis] = fabs(high[biqs_a0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_aR1];
            high[biqs_aR1] = high[biqs_aR2] - (high[biqs_temp] * high[biqs_b1]);
            high[biqs_aR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_b2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_c0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_cR1];
            high[biqs_cR1] = high[biqs_cR2] - (high[biqs_temp] * high[biqs_d1]);
            high[biqs_cR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_d2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_dis] = fabs(high[biqs_e0] * (1.0 + (high[biqs_outR] * high[biqs_nonlin])));
            if (high[biqs_dis] > 1.0) {
                high[biqs_dis] = 1.0;
            }
            high[biqs_temp] = (high[biqs_outR] * high[biqs_dis]) + high[biqs_eR1];
            high[biqs_eR1] = high[biqs_eR2] - (high[biqs_temp] * high[biqs_f1]);
            high[biqs_eR2] = (high[biqs_outR] * -high[biqs_dis]) - (high[biqs_temp] * high[biqs_f2]);
            high[biqs_outR] = high[biqs_temp];
            high[biqs_outR] *= high[biqs_level];
            if (high[biqs_level] > 1.0) {
                high[biqs_outR] *= high[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            hmid[biqs_outR] = inputSampleR * fabs(hmid[biqs_level]);
            hmid[biqs_dis] = fabs(hmid[biqs_a0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_aR1];
            hmid[biqs_aR1] = hmid[biqs_aR2] - (hmid[biqs_temp] * hmid[biqs_b1]);
            hmid[biqs_aR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_b2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_c0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_cR1];
            hmid[biqs_cR1] = hmid[biqs_cR2] - (hmid[biqs_temp] * hmid[biqs_d1]);
            hmid[biqs_cR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_d2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_dis] = fabs(hmid[biqs_e0] * (1.0 + (hmid[biqs_outR] * hmid[biqs_nonlin])));
            if (hmid[biqs_dis] > 1.0) {
                hmid[biqs_dis] = 1.0;
            }
            hmid[biqs_temp] = (hmid[biqs_outR] * hmid[biqs_dis]) + hmid[biqs_eR1];
            hmid[biqs_eR1] = hmid[biqs_eR2] - (hmid[biqs_temp] * hmid[biqs_f1]);
            hmid[biqs_eR2] = (hmid[biqs_outR] * -hmid[biqs_dis]) - (hmid[biqs_temp] * hmid[biqs_f2]);
            hmid[biqs_outR] = hmid[biqs_temp];
            hmid[biqs_outR] *= hmid[biqs_level];
            if (hmid[biqs_level] > 1.0) {
                hmid[biqs_outR] *= hmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            lmid[biqs_outR] = inputSampleR * fabs(lmid[biqs_level]);
            lmid[biqs_dis] = fabs(lmid[biqs_a0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_aR1];
            lmid[biqs_aR1] = lmid[biqs_aR2] - (lmid[biqs_temp] * lmid[biqs_b1]);
            lmid[biqs_aR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_b2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_c0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_cR1];
            lmid[biqs_cR1] = lmid[biqs_cR2] - (lmid[biqs_temp] * lmid[biqs_d1]);
            lmid[biqs_cR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_d2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_dis] = fabs(lmid[biqs_e0] * (1.0 + (lmid[biqs_outR] * lmid[biqs_nonlin])));
            if (lmid[biqs_dis] > 1.0) {
                lmid[biqs_dis] = 1.0;
            }
            lmid[biqs_temp] = (lmid[biqs_outR] * lmid[biqs_dis]) + lmid[biqs_eR1];
            lmid[biqs_eR1] = lmid[biqs_eR2] - (lmid[biqs_temp] * lmid[biqs_f1]);
            lmid[biqs_eR2] = (lmid[biqs_outR] * -lmid[biqs_dis]) - (lmid[biqs_temp] * lmid[biqs_f2]);
            lmid[biqs_outR] = lmid[biqs_temp];
            lmid[biqs_outR] *= lmid[biqs_level];
            if (lmid[biqs_level] > 1.0) {
                lmid[biqs_outR] *= lmid[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            // begin Stacked Biquad With Reversed Neutron Flow R
            bass[biqs_outR] = inputSampleR * fabs(bass[biqs_level]);
            bass[biqs_dis] = fabs(bass[biqs_a0] * (1.0 + (bass[biqs_outR] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_aR1];
            bass[biqs_aR1] = bass[biqs_aR2] - (bass[biqs_temp] * bass[biqs_b1]);
            bass[biqs_aR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_b2]);
            bass[biqs_outR] = bass[biqs_temp];
            bass[biqs_dis] = fabs(bass[biqs_c0] * (1.0 + (bass[biqs_outR] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_cR1];
            bass[biqs_cR1] = bass[biqs_cR2] - (bass[biqs_temp] * bass[biqs_d1]);
            bass[biqs_cR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_d2]);
            bass[biqs_outR] = bass[biqs_temp];
            bass[biqs_dis] = fabs(bass[biqs_e0] * (1.0 + (bass[biqs_outR] * bass[biqs_nonlin])));
            if (bass[biqs_dis] > 1.0) {
                bass[biqs_dis] = 1.0;
            }
            bass[biqs_temp] = (bass[biqs_outR] * bass[biqs_dis]) + bass[biqs_eR1];
            bass[biqs_eR1] = bass[biqs_eR2] - (bass[biqs_temp] * bass[biqs_f1]);
            bass[biqs_eR2] = (bass[biqs_outR] * -bass[biqs_dis]) - (bass[biqs_temp] * bass[biqs_f2]);
            bass[biqs_outR] = bass[biqs_temp];
            bass[biqs_outR] *= bass[biqs_level];
            if (bass[biqs_level] > 1.0) {
                bass[biqs_outR] *= bass[biqs_level];
            }
            // end Stacked Biquad With Reversed Neutron Flow R
            double temp = (double)sampleFrames / inFramesToProcess;
            double gainR = (panA * temp) + (panB * (1.0 - temp));
            double gainL = 1.57079633 - gainR;
            gainR = sin(gainR);
            gainL = sin(gainL);
            double gain = (inTrimA * temp) + (inTrimB * (1.0 - temp));
            if (gain > 1.0) {
                gain *= gain;
            }
            else {
                gain = 1.0 - pow(1.0 - gain, 2);
            }
            gain *= 0.763932022500211;
            double airGain = (airGainA * temp) + (airGainB * (1.0 - temp));
            if (airGain > 1.0) {
                airGain *= airGain;
            }
            else {
                airGain = 1.0 - pow(1.0 - airGain, 2);
            }
            double fireGain = (fireGainA * temp) + (fireGainB * (1.0 - temp));
            if (fireGain > 1.0) {
                fireGain *= fireGain;
            }
            else {
                fireGain = 1.0 - pow(1.0 - fireGain, 2);
            }
            double firePad = fireGain;
            if (firePad > 1.0) {
                firePad = 1.0;
            }
            double stoneGain = (stoneGainA * temp) + (stoneGainB * (1.0 - temp));
            if (stoneGain > 1.0) {
                stoneGain *= stoneGain;
            }
            else {
                stoneGain = 1.0 - pow(1.0 - stoneGain, 2);
            }
            double stonePad = stoneGain;
            if (stonePad > 1.0) {
                stonePad = 1.0;
            }
            // set up smoothed gain controls
            // begin Air3L
            double drySampleL = inputSampleL;
            air[pvSL4] = air[pvAL4] - air[pvAL3];
            air[pvSL3] = air[pvAL3] - air[pvAL2];
            air[pvSL2] = air[pvAL2] - air[pvAL1];
            air[pvSL1] = air[pvAL1] - inputSampleL;
            air[accSL3] = air[pvSL4] - air[pvSL3];
            air[accSL2] = air[pvSL3] - air[pvSL2];
            air[accSL1] = air[pvSL2] - air[pvSL1];
            air[acc2SL2] = air[accSL3] - air[accSL2];
            air[acc2SL1] = air[accSL2] - air[accSL1];
            air[outAL] = -(air[pvAL1] + air[pvSL3] + air[acc2SL2] - ((air[acc2SL2] + air[acc2SL1]) * 0.5));
            air[gainAL] *= 0.5;
            air[gainAL] += fabs(drySampleL - air[outAL]) * 0.5;
            if (air[gainAL] > 0.3 * sqrt(overallscale)) {
                air[gainAL] = 0.3 * sqrt(overallscale);
            }
            air[pvAL4] = air[pvAL3];
            air[pvAL3] = air[pvAL2];
            air[pvAL2] = air[pvAL1];
            air[pvAL1] = (air[gainAL] * air[outAL]) + drySampleL;
            double fireL = drySampleL - ((air[outAL] * 0.5) + (drySampleL * (0.457 - (0.017 * overallscale))));
            temp = (fireL + air[gndavgL]) * 0.5;
            air[gndavgL] = fireL;
            fireL = temp;
            double airL = (drySampleL - fireL) * airGain;
            inputSampleL = fireL;
            // end Air3L
            // begin Air3R
            double drySampleR = inputSampleR;
            air[pvSR4] = air[pvAR4] - air[pvAR3];
            air[pvSR3] = air[pvAR3] - air[pvAR2];
            air[pvSR2] = air[pvAR2] - air[pvAR1];
            air[pvSR1] = air[pvAR1] - inputSampleR;
            air[accSR3] = air[pvSR4] - air[pvSR3];
            air[accSR2] = air[pvSR3] - air[pvSR2];
            air[accSR1] = air[pvSR2] - air[pvSR1];
            air[acc2SR2] = air[accSR3] - air[accSR2];
            air[acc2SR1] = air[accSR2] - air[accSR1];
            air[outAR] = -(air[pvAR1] + air[pvSR3] + air[acc2SR2] - ((air[acc2SR2] + air[acc2SR1]) * 0.5));
            air[gainAR] *= 0.5;
            air[gainAR] += fabs(drySampleR - air[outAR]) * 0.5;
            if (air[gainAR] > 0.3 * sqrt(overallscale)) {
                air[gainAR] = 0.3 * sqrt(overallscale);
            }
            air[pvAR4] = air[pvAR3];
            air[pvAR3] = air[pvAR2];
            air[pvAR2] = air[pvAR1];
            air[pvAR1] = (air[gainAR] * air[outAR]) + drySampleR;
            double fireR = drySampleR - ((air[outAR] * 0.5) + (drySampleR * (0.457 - (0.017 * overallscale))));
            temp = (fireR + air[gndavgR]) * 0.5;
            air[gndavgR] = fireR;
            fireR = temp;
            double airR = (drySampleR - fireR) * airGain;
            inputSampleR = fireR;
            // end Air3R
            // begin KalmanL
            temp = inputSampleL = inputSampleL * (1.0 - kalmanRange) * 0.777;
            inputSampleL *= (1.0 - kalmanRange);
            // set up gain levels to control the beast
            kal[prevSlewL3] += kal[prevSampL3] - kal[prevSampL2];
            kal[prevSlewL3] *= 0.5;
            kal[prevSlewL2] += kal[prevSampL2] - kal[prevSampL1];
            kal[prevSlewL2] *= 0.5;
            kal[prevSlewL1] += kal[prevSampL1] - inputSampleL;
            kal[prevSlewL1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewL2] += kal[prevSlewL3] - kal[prevSlewL2];
            kal[accSlewL2] *= 0.5;
            kal[accSlewL1] += kal[prevSlewL2] - kal[prevSlewL1];
            kal[accSlewL1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewL3] += (kal[accSlewL2] - kal[accSlewL1]);
            kal[accSlewL3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutL] += kal[prevSampL1] + kal[prevSlewL2] + kal[accSlewL3];
            kal[kalOutL] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainL] += fabs(temp - kal[kalOutL]) * kalmanRange * 8.0;
            kal[kalGainL] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainL] > kalmanRange * 0.5) {
                kal[kalGainL] = kalmanRange * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutL] += (temp * (1.0 - (0.68 + (kalmanRange * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampL3] = kal[prevSampL2];
            kal[prevSampL2] = kal[prevSampL1];
            kal[prevSampL1] = (kal[kalGainL] * kal[kalOutL]) + ((1.0 - kal[kalGainL]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampL1] > 1.0) {
                kal[prevSampL1] = 1.0;
            }
            if (kal[prevSampL1] < -1.0) {
                kal[prevSampL1] = -1.0;
            }
            double stoneL = kal[kalOutL] * 0.777;
            fireL -= stoneL;
            // end KalmanL
            // begin KalmanR
            temp = inputSampleR = inputSampleR * (1.0 - kalmanRange) * 0.777;
            inputSampleR *= (1.0 - kalmanRange);
            // set up gain levels to control the beast
            kal[prevSlewR3] += kal[prevSampR3] - kal[prevSampR2];
            kal[prevSlewR3] *= 0.5;
            kal[prevSlewR2] += kal[prevSampR2] - kal[prevSampR1];
            kal[prevSlewR2] *= 0.5;
            kal[prevSlewR1] += kal[prevSampR1] - inputSampleR;
            kal[prevSlewR1] *= 0.5;
            // make slews from each set of samples used
            kal[accSlewR2] += kal[prevSlewR3] - kal[prevSlewR2];
            kal[accSlewR2] *= 0.5;
            kal[accSlewR1] += kal[prevSlewR2] - kal[prevSlewR1];
            kal[accSlewR1] *= 0.5;
            // differences between slews: rate of change of rate of change
            kal[accSlewR3] += (kal[accSlewR2] - kal[accSlewR1]);
            kal[accSlewR3] *= 0.5;
            // entering the abyss, what even is this
            kal[kalOutR] += kal[prevSampR1] + kal[prevSlewR2] + kal[accSlewR3];
            kal[kalOutR] *= 0.5;
            // resynthesizing predicted result (all iir smoothed)
            kal[kalGainR] += fabs(temp - kal[kalOutR]) * kalmanRange * 8.0;
            kal[kalGainR] *= 0.5;
            // madness takes its toll. Kalman Gain: how much dry to retain
            if (kal[kalGainR] > kalmanRange * 0.5) {
                kal[kalGainR] = kalmanRange * 0.5;
            }
            // attempts to avoid explosions
            kal[kalOutR] += (temp * (1.0 - (0.68 + (kalmanRange * 0.157))));
            // this is for tuning a really complete cancellation up around Nyquist
            kal[prevSampR3] = kal[prevSampR2];
            kal[prevSampR2] = kal[prevSampR1];
            kal[prevSampR1] = (kal[kalGainR] * kal[kalOutR]) + ((1.0 - kal[kalGainR]) * temp);
            // feed the chain of previous samples
            if (kal[prevSampR1] > 1.0) {
                kal[prevSampR1] = 1.0;
            }
            if (kal[prevSampR1] < -1.0) {
                kal[prevSampR1] = -1.0;
            }
            double stoneR = kal[kalOutR] * 0.777;
            fireR -= stoneR;
            // end KalmanR
            // fire dynamics
            if (fabs(fireL) > compFThresh) { // compression L
                fireCompL -= (fireCompL * compFAttack);
                fireCompL += ((compFThresh / fabs(fireL)) * compFAttack);
            }
            else {
                fireCompL = (fireCompL * (1.0 - compFRelease)) + compFRelease;
            }
            if (fabs(fireR) > compFThresh) { // compression R
                fireCompR -= (fireCompR * compFAttack);
                fireCompR += ((compFThresh / fabs(fireR)) * compFAttack);
            }
            else {
                fireCompR = (fireCompR * (1.0 - compFRelease)) + compFRelease;
            }
            if (fireCompL > fireCompR) {
                fireCompL -= (fireCompL * compFAttack);
            }
            if (fireCompR > fireCompL) {
                fireCompR -= (fireCompR * compFAttack);
            }
            if (fabs(fireL) > gateFThresh) {
                fireGate = gateFSustain;
            }
            else if (fabs(fireR) > gateFThresh) {
                fireGate = gateFSustain;
            }
            else {
                fireGate *= (1.0 - gateFRelease);
            }
            if (fireGate < 0.0) {
                fireGate = 0.0;
            }
            fireCompL = fmax(fmin(fireCompL, 1.0), 0.0);
            fireCompR = fmax(fmin(fireCompR, 1.0), 0.0);
            fireL *= (((1.0 - compFRatio) * firePad) + (fireCompL * compFRatio * fireGain));
            fireR *= (((1.0 - compFRatio) * firePad) + (fireCompR * compFRatio * fireGain));
            if (fireGate < M_PI_2) {
                temp = ((1.0 - gateFRatio) + (sin(fireGate) * gateFRatio));
                airL *= temp;
                airR *= temp;
                fireL *= temp;
                fireR *= temp;
                high[biqs_outL] *= temp;
                high[biqs_outR] *= temp;
                hmid[biqs_outL] *= temp; // if Fire gating, gate Air, high and hmid
                hmid[biqs_outR] *= temp; // note that we aren't compressing these
            }
            // stone dynamics
            if (fabs(stoneL) > compSThresh) { // compression L
                stoneCompL -= (stoneCompL * compSAttack);
                stoneCompL += ((compSThresh / fabs(stoneL)) * compSAttack);
            }
            else {
                stoneCompL = (stoneCompL * (1.0 - compSRelease)) + compSRelease;
            }
            if (fabs(stoneR) > compSThresh) { // compression R
                stoneCompR -= (stoneCompR * compSAttack);
                stoneCompR += ((compSThresh / fabs(stoneR)) * compSAttack);
            }
            else {
                stoneCompR = (stoneCompR * (1.0 - compSRelease)) + compSRelease;
            }
            if (stoneCompL > stoneCompR) {
                stoneCompL -= (stoneCompL * compSAttack);
            }
            if (stoneCompR > stoneCompL) {
                stoneCompR -= (stoneCompR * compSAttack);
            }
            if (fabs(stoneL) > gateSThresh) {
                stoneGate = gateSSustain;
            }
            else if (fabs(stoneR) > gateSThresh) {
                stoneGate = gateSSustain;
            }
            else {
                stoneGate *= (1.0 - gateSRelease);
            }
            if (stoneGate < 0.0) {
                stoneGate = 0.0;
            }
            stoneCompL = fmax(fmin(stoneCompL, 1.0), 0.0);
            stoneCompR = fmax(fmin(stoneCompR, 1.0), 0.0);
            stoneL *= (((1.0 - compSRatio) * stonePad) + (stoneCompL * compSRatio * stoneGain));
            stoneR *= (((1.0 - compSRatio) * stonePad) + (stoneCompR * compSRatio * stoneGain));
            if (stoneGate < M_PI_2) {
                temp = ((1.0 - gateSRatio) + (sin(stoneGate) * gateSRatio));
                stoneL *= temp;
                stoneR *= temp;
                lmid[biqs_outL] *= temp;
                lmid[biqs_outR] *= temp;
                bass[biqs_outL] *= temp; // if Stone gating, gate lmid and bass
                bass[biqs_outR] *= temp; // note that we aren't compressing these
            }
            inputSampleL = stoneL + fireL + airL;
            inputSampleR = stoneR + fireR + airR;
            // create Stonefire output
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_c0]) + highpass[hilp_cL1];
                highpass[hilp_cL1] = (inputSampleL * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cL2];
                highpass[hilp_cL2] = (inputSampleL * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_c0]) + highpass[hilp_cR1];
                highpass[hilp_cR1] = (inputSampleR * highpass[hilp_c1]) - (highpass[hilp_temp] * highpass[hilp_d1]) + highpass[hilp_cR2];
                highpass[hilp_cR2] = (inputSampleR * highpass[hilp_c0]) - (highpass[hilp_temp] * highpass[hilp_d2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_cR1] = highpass[hilp_cR2] = highpass[hilp_cL1] = highpass[hilp_cL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_c0]) + lowpass[hilp_cL1];
                lowpass[hilp_cL1] = (inputSampleL * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cL2];
                lowpass[hilp_cL2] = (inputSampleL * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_c0]) + lowpass[hilp_cR1];
                lowpass[hilp_cR1] = (inputSampleR * lowpass[hilp_c1]) - (lowpass[hilp_temp] * lowpass[hilp_d1]) + lowpass[hilp_cR2];
                lowpass[hilp_cR2] = (inputSampleR * lowpass[hilp_c0]) - (lowpass[hilp_temp] * lowpass[hilp_d2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_cR1] = lowpass[hilp_cR2] = lowpass[hilp_cL1] = lowpass[hilp_cL2] = 0.0;
            }
            // another stage of Highpass/Lowpass before bringing in the parametric bands
            inputSampleL += (high[biqs_outL] + hmid[biqs_outL] + lmid[biqs_outL] + bass[biqs_outL]);
            inputSampleR += (high[biqs_outR] + hmid[biqs_outR] + lmid[biqs_outR] + bass[biqs_outR]);
            // add parametric boosts or cuts: clean as possible for maximal rawness and sonority
            inputSampleL = inputSampleL * gainL * gain;
            inputSampleR = inputSampleR * gainR * gain;
            // applies pan section, and smoothed fader gain
            inputSampleL *= topdB;
            if (inputSampleL < -0.222) {
                inputSampleL = -0.222;
            }
            if (inputSampleL > 0.222) {
                inputSampleL = 0.222;
            }
            dBaL[dBaXL] = inputSampleL;
            dBaPosL *= 0.5;
            dBaPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            int dBdly = floor(dBaPosL * dscBuf);
            double dBi = (dBaPosL * dscBuf) - dBdly;
            inputSampleL = dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBaL[dBaXL - dBdly + ((dBaXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXL++;
            if (dBaXL < 0 || dBaXL >= dscBuf) {
                dBaXL = 0;
            }
            inputSampleL /= topdB;
            inputSampleR *= topdB;
            if (inputSampleR < -0.222) {
                inputSampleR = -0.222;
            }
            if (inputSampleR > 0.222) {
                inputSampleR = 0.222;
            }
            dBaR[dBaXR] = inputSampleR;
            dBaPosR *= 0.5;
            dBaPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBaPosR * dscBuf);
            dBi = (dBaPosR * dscBuf) - dBdly;
            inputSampleR = dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBaR[dBaXR - dBdly + ((dBaXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBaXR++;
            if (dBaXR < 0 || dBaXR >= dscBuf) {
                dBaXR = 0;
            }
            inputSampleR /= topdB;
            // top dB processing for distributed discontinuity modeling air nonlinearity
            if (highpassEngage) { // distributed Highpass
                highpass[hilp_temp] = (inputSampleL * highpass[hilp_e0]) + highpass[hilp_eL1];
                highpass[hilp_eL1] = (inputSampleL * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eL2];
                highpass[hilp_eL2] = (inputSampleL * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleL = highpass[hilp_temp];
                highpass[hilp_temp] = (inputSampleR * highpass[hilp_e0]) + highpass[hilp_eR1];
                highpass[hilp_eR1] = (inputSampleR * highpass[hilp_e1]) - (highpass[hilp_temp] * highpass[hilp_f1]) + highpass[hilp_eR2];
                highpass[hilp_eR2] = (inputSampleR * highpass[hilp_e0]) - (highpass[hilp_temp] * highpass[hilp_f2]);
                inputSampleR = highpass[hilp_temp];
            }
            else {
                highpass[hilp_eR1] = highpass[hilp_eR2] = highpass[hilp_eL1] = highpass[hilp_eL2] = 0.0;
            }
            if (lowpassEngage) { // distributed Lowpass
                lowpass[hilp_temp] = (inputSampleL * lowpass[hilp_e0]) + lowpass[hilp_eL1];
                lowpass[hilp_eL1] = (inputSampleL * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eL2];
                lowpass[hilp_eL2] = (inputSampleL * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleL = lowpass[hilp_temp];
                lowpass[hilp_temp] = (inputSampleR * lowpass[hilp_e0]) + lowpass[hilp_eR1];
                lowpass[hilp_eR1] = (inputSampleR * lowpass[hilp_e1]) - (lowpass[hilp_temp] * lowpass[hilp_f1]) + lowpass[hilp_eR2];
                lowpass[hilp_eR2] = (inputSampleR * lowpass[hilp_e0]) - (lowpass[hilp_temp] * lowpass[hilp_f2]);
                inputSampleR = lowpass[hilp_temp];
            }
            else {
                lowpass[hilp_eR1] = lowpass[hilp_eR2] = lowpass[hilp_eL1] = lowpass[hilp_eL2] = 0.0;
            }
            // final Highpass/Lowpass continues to address aliasing
            // final stacked biquad section is the softest Q for smoothness
            // begin 64 bit stereo floating point dither
            int expon;
            frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            inputSampleL += ((double(fpdL) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
            frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            inputSampleR += ((double(fpdR) - uint32_t(0x7fffffff)) * 1.1e-44l * pow(2, expon + 62));
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
} // namespace airwindohhs::consolexpre
