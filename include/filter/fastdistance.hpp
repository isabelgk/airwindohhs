#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::fastdistance {

constexpr std::string_view k_name{ "FastDistance" };
constexpr std::string_view k_short_description{
    "FastDistance is a sort of ultra-efficient lowpass filter."
};
constexpr std::string_view k_long_description{
    "This is work I'm doing in efforts to help the Godot game engine. Whether or not it's accepted by them, you get to have it now :)The idea of the distance filter is, something you can have on every sound source in a scene. It's gotta be efficient, because there can be an awful lot of them. It's gotta handle wild modulations, because who knows what might be happening there in the game engine. And it's gotta roll off high frequencies, because stuff that's far away doesn't sound as bright as stuff that's right next to you.Ideally (like with my previous Distance plugins) it's doing clever things to put across the sense of great distance, but how clever can you be if you might have to have hundreds of sound-makers all talking at once? And so, FastDistance specifically chooses forms of treble-filtering that are dirt simple. It's just a variation on an averaging filter crossed with an IIR. Need more? Layer on more of them. The IIR (infinite impulse response) is not there to be fancy, it's just there because it saves me allocating one extra variable. The way it uses more or less poles of filtering is literally jumping into a pile of them and letting them go. There's no way of controlling it other than just choosing how deeply you filter in this one particular way that's unreasonably efficient to calculate. In fact you don't 'calculate' it at all, you just use it directly.The weird thing is it actually sounds pretty awesome.Since it's so efficient to run, instead of doing three or four poles of filtering, you can do thirty or forty. They run about as quick as calculating the coefficients of a regular old filter. There's next to nothing to them. And when you take such a primitive filter and just do it more, what you get is increased cutoff steepness… but the cutoff is very high already, and it's not at all resonant. So the intense layering just gives you a very natural sounding darkening effect, combined with taking the highs and absolutely burying them an unthinkable number of dB down. It's just how it works. It's not really a 'steep cutoff' no matter how many poles you use, and is never the least bit resonant or filter-sweepy. Just a distance through air darkener.It's very likely that this will find other use but for now I'm gonna see if Godot likes it (and if it's competitive efficiency wise with the more traditional low shelf they're using). I'm working on a system for game engine reverb that I hope will be useful, also for Godot, and I'll have an ultra lightweight reverb soon that's teaching me many useful things I can incorporate into my flagship reverbs.And whatever else happens with all that, you have FastDistance as a free plugin, plus also as MIT licensed source code that you can use for anything so long as you credit. Enjoy the new lowpass :)"
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class FastDistance final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'fdis';
    float A;
    float firstDryL, firstAvgL;
    float lsA, lsB, lsC, lsD, lsE, lsF, lsG, lsH, lsI, lsJ, lsK, lsL, lsM;
    float lsN, lsO, lsP, lsQ, lsR, lsS, lsT, lsU, lsV, lsW, lsX, lsY, lsZ;
    float lsa, lsb, lsc, lsd, lse, lsf, lsg, lsh, lsi, lsj, lsk, lsl, lsm;
    float lsn, lso, lsp, lsq, lsr, lss, lst, lsu, lsv, lsw, lsx, lsy, lsz;
    float firstDryR, firstAvgR;
    float rsA, rsB, rsC, rsD, rsE, rsF, rsG, rsH, rsI, rsJ, rsK, rsL, rsM;
    float rsN, rsO, rsP, rsQ, rsR, rsS, rsT, rsU, rsV, rsW, rsX, rsY, rsZ;
    float rsa, rsb, rsc, rsd, rse, rsf, rsg, rsh, rsi, rsj, rsk, rsl, rsm;
    float rsn, rso, rsp, rsq, rsr, rss, rst, rsu, rsv, rsw, rsx, rsy, rsz;
    int prevDistance;
    float pointCycle, pointAL, pointBL, pointAR, pointBR;
    // this is a simple linear interpolation sample rate routine for the plugin version
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    FastDistance()
    {
        A = 0.0;
        firstDryL = firstAvgL = 0.0f;
        lsA = lsB = lsC = lsD = lsE = lsF = lsG = lsH = lsI = lsJ = lsK = lsL = lsM = 0.0f;
        lsN = lsO = lsP = lsQ = lsR = lsS = lsT = lsU = lsV = lsW = lsX = lsY = lsZ = 0.0f;
        lsa = lsb = lsc = lsd = lse = lsf = lsg = lsh = lsi = lsj = lsk = lsl = lsm = 0.0f;
        lsn = lso = lsp = lsq = lsr = lss = lst = lsu = lsv = lsw = lsx = lsy = lsz = 0.0f;
        firstDryR = firstAvgR = 0.0f;
        rsA = rsB = rsC = rsD = rsE = rsF = rsG = rsH = rsI = rsJ = rsK = rsL = rsM = 0.0f;
        rsN = rsO = rsP = rsQ = rsR = rsS = rsT = rsU = rsV = rsW = rsX = rsY = rsZ = 0.0f;
        rsa = rsb = rsc = rsd = rse = rsf = rsg = rsh = rsi = rsj = rsk = rsl = rsm = 0.0f;
        rsn = rso = rsp = rsq = rsr = rss = rst = rsu = rsv = rsw = rsx = rsy = rsz = 0.0f;
        prevDistance = 0;
        pointCycle = pointAL = pointBL = pointAR = pointBR = 0.0f;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "distnce"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Distnce"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;

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
        float pointDerez = 1.0f / overallscale; // forcing it to be simple
        float distance = pow(A * 7.28f, 2.0f); // 0 to 52.9984
        int distanceSteps = (int)distance; // 52 maximum
        distance -= (float)distanceSteps; // 0.9984
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
            pointCycle += pointDerez;
            if (pointCycle > 0.9999999) {
                // this is sample rate handling for the plugin, so that
                // a more primitive filter node can run at any rate.
                // begin just the distance filter L
                firstDryL = inputSampleL; // start by doing the interpolation
                inputSampleL += firstAvgL;
                inputSampleL *= 0.5f;
                firstAvgL = inputSampleL;
                inputSampleL = (firstDryL * (1.0f - distance)) + (inputSampleL * distance);
                // having done at least one interpolation we can now do the integer number of stages
                if (prevDistance < distanceSteps) {
                    switch (prevDistance)
                    {
                        case 0: lsA = inputSampleL; // if we've changed the setting
                        case 1: lsB = inputSampleL; // reset the ones being put back into play
                        case 2: lsC = inputSampleL; // this won't be happening constantly,
                        case 3: lsD = inputSampleL; // but since we don't have an array
                        case 4: lsE = inputSampleL; // this is the structure by which we
                        case 5: lsF = inputSampleL; // get the new averagings going
                        case 6: lsG = inputSampleL; // without pops or clicks
                        case 7: lsH = inputSampleL;
                        case 8: lsI = inputSampleL;
                        case 9: lsJ = inputSampleL;
                        case 10: lsK = inputSampleL;
                        case 11: lsL = inputSampleL;
                        case 12: lsM = inputSampleL;
                        case 13: lsN = inputSampleL;
                        case 14: lsO = inputSampleL;
                        case 15: lsP = inputSampleL;
                        case 16: lsQ = inputSampleL;
                        case 17: lsR = inputSampleL;
                        case 18: lsS = inputSampleL;
                        case 19: lsT = inputSampleL;
                        case 20: lsU = inputSampleL;
                        case 21: lsV = inputSampleL;
                        case 22: lsW = inputSampleL;
                        case 23: lsX = inputSampleL;
                        case 24: lsY = inputSampleL;
                        case 25: lsZ = inputSampleL;
                        case 26: lsa = inputSampleL;
                        case 27: lsb = inputSampleL;
                        case 28: lsc = inputSampleL;
                        case 29: lsd = inputSampleL;
                        case 30: lse = inputSampleL;
                        case 31: lsf = inputSampleL;
                        case 32: lsg = inputSampleL;
                        case 33: lsh = inputSampleL;
                        case 34: lsi = inputSampleL;
                        case 35: lsj = inputSampleL;
                        case 36: lsk = inputSampleL;
                        case 37: lsl = inputSampleL;
                        case 38: lsm = inputSampleL;
                        case 39: lsn = inputSampleL;
                        case 40: lso = inputSampleL;
                        case 41: lsp = inputSampleL;
                        case 42: lsq = inputSampleL;
                        case 43: lsr = inputSampleL;
                        case 44: lss = inputSampleL;
                        case 45: lst = inputSampleL;
                        case 46: lsu = inputSampleL;
                        case 47: lsv = inputSampleL;
                        case 48: lsw = inputSampleL;
                        case 49: lsx = inputSampleL;
                        case 50: lsy = inputSampleL;
                        case 51: lsz = inputSampleL;
                        case 52: break; // reset prevDistance after R channel
                    }
                }
                switch (52 - distanceSteps)
                { // apply the stack of filter steps to produce the distance filter
                    case 0:
                        inputSampleL += lsz;
                        inputSampleL *= 0.5;
                        lsz = inputSampleL;
                    case 1:
                        inputSampleL += lsy;
                        inputSampleL *= 0.5;
                        lsy = inputSampleL;
                    case 2:
                        inputSampleL += lsx;
                        inputSampleL *= 0.5;
                        lsx = inputSampleL;
                    case 3:
                        inputSampleL += lsw;
                        inputSampleL *= 0.5;
                        lsw = inputSampleL;
                    case 4:
                        inputSampleL += lsv;
                        inputSampleL *= 0.5;
                        lsv = inputSampleL;
                    case 5:
                        inputSampleL += lsu;
                        inputSampleL *= 0.5;
                        lsu = inputSampleL;
                    case 6:
                        inputSampleL += lst;
                        inputSampleL *= 0.5;
                        lst = inputSampleL;
                    case 7:
                        inputSampleL += lss;
                        inputSampleL *= 0.5;
                        lss = inputSampleL;
                    case 8:
                        inputSampleL += lsr;
                        inputSampleL *= 0.5;
                        lsr = inputSampleL;
                    case 9:
                        inputSampleL += lsq;
                        inputSampleL *= 0.5;
                        lsq = inputSampleL;
                    case 10:
                        inputSampleL += lsp;
                        inputSampleL *= 0.5;
                        lsp = inputSampleL;
                    case 11:
                        inputSampleL += lso;
                        inputSampleL *= 0.5;
                        lso = inputSampleL;
                    case 12:
                        inputSampleL += lsn;
                        inputSampleL *= 0.5;
                        lsn = inputSampleL;
                    case 13:
                        inputSampleL += lsm;
                        inputSampleL *= 0.5;
                        lsm = inputSampleL;
                    case 14:
                        inputSampleL += lsl;
                        inputSampleL *= 0.5;
                        lsl = inputSampleL;
                    case 15:
                        inputSampleL += lsk;
                        inputSampleL *= 0.5;
                        lsk = inputSampleL;
                    case 16:
                        inputSampleL += lsj;
                        inputSampleL *= 0.5;
                        lsj = inputSampleL;
                    case 17:
                        inputSampleL += lsi;
                        inputSampleL *= 0.5;
                        lsi = inputSampleL;
                    case 18:
                        inputSampleL += lsh;
                        inputSampleL *= 0.5;
                        lsh = inputSampleL;
                    case 19:
                        inputSampleL += lsg;
                        inputSampleL *= 0.5;
                        lsg = inputSampleL;
                    case 20:
                        inputSampleL += lsf;
                        inputSampleL *= 0.5;
                        lsf = inputSampleL;
                    case 21:
                        inputSampleL += lse;
                        inputSampleL *= 0.5;
                        lse = inputSampleL;
                    case 22:
                        inputSampleL += lsd;
                        inputSampleL *= 0.5;
                        lsd = inputSampleL;
                    case 23:
                        inputSampleL += lsc;
                        inputSampleL *= 0.5;
                        lsc = inputSampleL;
                    case 24:
                        inputSampleL += lsb;
                        inputSampleL *= 0.5;
                        lsb = inputSampleL;
                    case 25:
                        inputSampleL += lsa;
                        inputSampleL *= 0.5;
                        lsa = inputSampleL;
                    case 26:
                        inputSampleL += lsZ;
                        inputSampleL *= 0.5;
                        lsZ = inputSampleL;
                    case 27:
                        inputSampleL += lsY;
                        inputSampleL *= 0.5;
                        lsY = inputSampleL;
                    case 28:
                        inputSampleL += lsX;
                        inputSampleL *= 0.5;
                        lsX = inputSampleL;
                    case 29:
                        inputSampleL += lsW;
                        inputSampleL *= 0.5;
                        lsW = inputSampleL;
                    case 30:
                        inputSampleL += lsV;
                        inputSampleL *= 0.5;
                        lsV = inputSampleL;
                    case 31:
                        inputSampleL += lsU;
                        inputSampleL *= 0.5;
                        lsU = inputSampleL;
                    case 32:
                        inputSampleL += lsT;
                        inputSampleL *= 0.5;
                        lsT = inputSampleL;
                    case 33:
                        inputSampleL += lsS;
                        inputSampleL *= 0.5;
                        lsS = inputSampleL;
                    case 34:
                        inputSampleL += lsR;
                        inputSampleL *= 0.5;
                        lsR = inputSampleL;
                    case 35:
                        inputSampleL += lsQ;
                        inputSampleL *= 0.5;
                        lsQ = inputSampleL;
                    case 36:
                        inputSampleL += lsP;
                        inputSampleL *= 0.5;
                        lsP = inputSampleL;
                    case 37:
                        inputSampleL += lsO;
                        inputSampleL *= 0.5;
                        lsO = inputSampleL;
                    case 38:
                        inputSampleL += lsN;
                        inputSampleL *= 0.5;
                        lsN = inputSampleL;
                    case 39:
                        inputSampleL += lsM;
                        inputSampleL *= 0.5;
                        lsM = inputSampleL;
                    case 40:
                        inputSampleL += lsL;
                        inputSampleL *= 0.5;
                        lsL = inputSampleL;
                    case 41:
                        inputSampleL += lsK;
                        inputSampleL *= 0.5;
                        lsK = inputSampleL;
                    case 42:
                        inputSampleL += lsJ;
                        inputSampleL *= 0.5;
                        lsJ = inputSampleL;
                    case 43:
                        inputSampleL += lsI;
                        inputSampleL *= 0.5;
                        lsI = inputSampleL;
                    case 44:
                        inputSampleL += lsH;
                        inputSampleL *= 0.5;
                        lsH = inputSampleL;
                    case 45:
                        inputSampleL += lsG;
                        inputSampleL *= 0.5;
                        lsG = inputSampleL;
                    case 46:
                        inputSampleL += lsF;
                        inputSampleL *= 0.5;
                        lsF = inputSampleL;
                    case 47:
                        inputSampleL += lsE;
                        inputSampleL *= 0.5;
                        lsE = inputSampleL;
                    case 48:
                        inputSampleL += lsD;
                        inputSampleL *= 0.5;
                        lsD = inputSampleL;
                    case 49:
                        inputSampleL += lsC;
                        inputSampleL *= 0.5;
                        lsC = inputSampleL;
                    case 50:
                        inputSampleL += lsB;
                        inputSampleL *= 0.5;
                        lsB = inputSampleL;
                    case 51:
                        inputSampleL += lsA;
                        inputSampleL *= 0.5;
                        lsA = inputSampleL;
                    case 52: break;
                }
                // end distance filter L
                // begin just the distance filter R
                firstDryR = inputSampleR; // start by doing the interpolation
                inputSampleR += firstAvgR;
                inputSampleR *= 0.5f;
                firstAvgR = inputSampleR;
                inputSampleR = (firstDryR * (1.0f - distance)) + (inputSampleR * distance);
                // having done at least one interpolation we can now do the integer number of stages
                if (prevDistance < distanceSteps) {
                    switch (prevDistance)
                    {
                        case 0: rsA = inputSampleR; // if we've changed the setting
                        case 1: rsB = inputSampleR; // reset the ones being put back into play
                        case 2: rsC = inputSampleR; // this won't be happening constantly,
                        case 3: rsD = inputSampleR; // but since we don't have an array
                        case 4: rsE = inputSampleR; // this is the structure by which we
                        case 5: rsF = inputSampleR; // get the new averagings going
                        case 6: rsG = inputSampleR; // without pops or clicks
                        case 7: rsH = inputSampleR;
                        case 8: rsI = inputSampleR;
                        case 9: rsJ = inputSampleR;
                        case 10: rsK = inputSampleR;
                        case 11: rsL = inputSampleR;
                        case 12: rsM = inputSampleR;
                        case 13: rsN = inputSampleR;
                        case 14: rsO = inputSampleR;
                        case 15: rsP = inputSampleR;
                        case 16: rsQ = inputSampleR;
                        case 17: rsR = inputSampleR;
                        case 18: rsS = inputSampleR;
                        case 19: rsT = inputSampleR;
                        case 20: rsU = inputSampleR;
                        case 21: rsV = inputSampleR;
                        case 22: rsW = inputSampleR;
                        case 23: rsX = inputSampleR;
                        case 24: rsY = inputSampleR;
                        case 25: rsZ = inputSampleR;
                        case 26: rsa = inputSampleR;
                        case 27: rsb = inputSampleR;
                        case 28: rsc = inputSampleR;
                        case 29: rsd = inputSampleR;
                        case 30: rse = inputSampleR;
                        case 31: rsf = inputSampleR;
                        case 32: rsg = inputSampleR;
                        case 33: rsh = inputSampleR;
                        case 34: rsi = inputSampleR;
                        case 35: rsj = inputSampleR;
                        case 36: rsk = inputSampleR;
                        case 37: rsl = inputSampleR;
                        case 38: rsm = inputSampleR;
                        case 39: rsn = inputSampleR;
                        case 40: rso = inputSampleR;
                        case 41: rsp = inputSampleR;
                        case 42: rsq = inputSampleR;
                        case 43: rsr = inputSampleR;
                        case 44: rss = inputSampleR;
                        case 45: rst = inputSampleR;
                        case 46: rsu = inputSampleR;
                        case 47: rsv = inputSampleR;
                        case 48: rsw = inputSampleR;
                        case 49: rsx = inputSampleR;
                        case 50: rsy = inputSampleR;
                        case 51: rsz = inputSampleR;
                        case 52: prevDistance = distanceSteps;
                    }
                }
                switch (52 - distanceSteps)
                { // apply the stack of filter steps to produce the distance filter
                    case 0:
                        inputSampleR += rsz;
                        inputSampleR *= 0.5;
                        rsz = inputSampleR;
                    case 1:
                        inputSampleR += rsy;
                        inputSampleR *= 0.5;
                        rsy = inputSampleR;
                    case 2:
                        inputSampleR += rsx;
                        inputSampleR *= 0.5;
                        rsx = inputSampleR;
                    case 3:
                        inputSampleR += rsw;
                        inputSampleR *= 0.5;
                        rsw = inputSampleR;
                    case 4:
                        inputSampleR += rsv;
                        inputSampleR *= 0.5;
                        rsv = inputSampleR;
                    case 5:
                        inputSampleR += rsu;
                        inputSampleR *= 0.5;
                        rsu = inputSampleR;
                    case 6:
                        inputSampleR += rst;
                        inputSampleR *= 0.5;
                        rst = inputSampleR;
                    case 7:
                        inputSampleR += rss;
                        inputSampleR *= 0.5;
                        rss = inputSampleR;
                    case 8:
                        inputSampleR += rsr;
                        inputSampleR *= 0.5;
                        rsr = inputSampleR;
                    case 9:
                        inputSampleR += rsq;
                        inputSampleR *= 0.5;
                        rsq = inputSampleR;
                    case 10:
                        inputSampleR += rsp;
                        inputSampleR *= 0.5;
                        rsp = inputSampleR;
                    case 11:
                        inputSampleR += rso;
                        inputSampleR *= 0.5;
                        rso = inputSampleR;
                    case 12:
                        inputSampleR += rsn;
                        inputSampleR *= 0.5;
                        rsn = inputSampleR;
                    case 13:
                        inputSampleR += rsm;
                        inputSampleR *= 0.5;
                        rsm = inputSampleR;
                    case 14:
                        inputSampleR += rsl;
                        inputSampleR *= 0.5;
                        rsl = inputSampleR;
                    case 15:
                        inputSampleR += rsk;
                        inputSampleR *= 0.5;
                        rsk = inputSampleR;
                    case 16:
                        inputSampleR += rsj;
                        inputSampleR *= 0.5;
                        rsj = inputSampleR;
                    case 17:
                        inputSampleR += rsi;
                        inputSampleR *= 0.5;
                        rsi = inputSampleR;
                    case 18:
                        inputSampleR += rsh;
                        inputSampleR *= 0.5;
                        rsh = inputSampleR;
                    case 19:
                        inputSampleR += rsg;
                        inputSampleR *= 0.5;
                        rsg = inputSampleR;
                    case 20:
                        inputSampleR += rsf;
                        inputSampleR *= 0.5;
                        rsf = inputSampleR;
                    case 21:
                        inputSampleR += rse;
                        inputSampleR *= 0.5;
                        rse = inputSampleR;
                    case 22:
                        inputSampleR += rsd;
                        inputSampleR *= 0.5;
                        rsd = inputSampleR;
                    case 23:
                        inputSampleR += rsc;
                        inputSampleR *= 0.5;
                        rsc = inputSampleR;
                    case 24:
                        inputSampleR += rsb;
                        inputSampleR *= 0.5;
                        rsb = inputSampleR;
                    case 25:
                        inputSampleR += rsa;
                        inputSampleR *= 0.5;
                        rsa = inputSampleR;
                    case 26:
                        inputSampleR += rsZ;
                        inputSampleR *= 0.5;
                        rsZ = inputSampleR;
                    case 27:
                        inputSampleR += rsY;
                        inputSampleR *= 0.5;
                        rsY = inputSampleR;
                    case 28:
                        inputSampleR += rsX;
                        inputSampleR *= 0.5;
                        rsX = inputSampleR;
                    case 29:
                        inputSampleR += rsW;
                        inputSampleR *= 0.5;
                        rsW = inputSampleR;
                    case 30:
                        inputSampleR += rsV;
                        inputSampleR *= 0.5;
                        rsV = inputSampleR;
                    case 31:
                        inputSampleR += rsU;
                        inputSampleR *= 0.5;
                        rsU = inputSampleR;
                    case 32:
                        inputSampleR += rsT;
                        inputSampleR *= 0.5;
                        rsT = inputSampleR;
                    case 33:
                        inputSampleR += rsS;
                        inputSampleR *= 0.5;
                        rsS = inputSampleR;
                    case 34:
                        inputSampleR += rsR;
                        inputSampleR *= 0.5;
                        rsR = inputSampleR;
                    case 35:
                        inputSampleR += rsQ;
                        inputSampleR *= 0.5;
                        rsQ = inputSampleR;
                    case 36:
                        inputSampleR += rsP;
                        inputSampleR *= 0.5;
                        rsP = inputSampleR;
                    case 37:
                        inputSampleR += rsO;
                        inputSampleR *= 0.5;
                        rsO = inputSampleR;
                    case 38:
                        inputSampleR += rsN;
                        inputSampleR *= 0.5;
                        rsN = inputSampleR;
                    case 39:
                        inputSampleR += rsM;
                        inputSampleR *= 0.5;
                        rsM = inputSampleR;
                    case 40:
                        inputSampleR += rsL;
                        inputSampleR *= 0.5;
                        rsL = inputSampleR;
                    case 41:
                        inputSampleR += rsK;
                        inputSampleR *= 0.5;
                        rsK = inputSampleR;
                    case 42:
                        inputSampleR += rsJ;
                        inputSampleR *= 0.5;
                        rsJ = inputSampleR;
                    case 43:
                        inputSampleR += rsI;
                        inputSampleR *= 0.5;
                        rsI = inputSampleR;
                    case 44:
                        inputSampleR += rsH;
                        inputSampleR *= 0.5;
                        rsH = inputSampleR;
                    case 45:
                        inputSampleR += rsG;
                        inputSampleR *= 0.5;
                        rsG = inputSampleR;
                    case 46:
                        inputSampleR += rsF;
                        inputSampleR *= 0.5;
                        rsF = inputSampleR;
                    case 47:
                        inputSampleR += rsE;
                        inputSampleR *= 0.5;
                        rsE = inputSampleR;
                    case 48:
                        inputSampleR += rsD;
                        inputSampleR *= 0.5;
                        rsD = inputSampleR;
                    case 49:
                        inputSampleR += rsC;
                        inputSampleR *= 0.5;
                        rsC = inputSampleR;
                    case 50:
                        inputSampleR += rsB;
                        inputSampleR *= 0.5;
                        rsB = inputSampleR;
                    case 51:
                        inputSampleR += rsA;
                        inputSampleR *= 0.5;
                        rsA = inputSampleR;
                    case 52: break;
                }
                // end distance filter R
                pointAL = pointBL;
                pointBL = inputSampleL;
                pointAR = pointBR;
                pointBR = inputSampleR;
                pointCycle = 0.0; // interpolation point values
                // this is the sample rate adjustment for the plugin version
                // game engine may well always run at 44.1k or whatever
            }
            inputSampleL = pointAL + ((pointBL - pointAL) * pointCycle);
            inputSampleR = pointAR + ((pointBR - pointAR) * pointCycle);
            // this is the linear interpolation between reverb points
            // governed by sample rate, to work as a plugin
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::fastdistance
