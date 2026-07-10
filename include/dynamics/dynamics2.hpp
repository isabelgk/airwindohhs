#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dynamics2 {

constexpr std::string_view k_name{ "Dynamics2" };
constexpr std::string_view k_short_description{
    "Dynamics2 is the compressor gate for new Console plugins!"
};
constexpr std::string_view k_long_description{
    "Dynamics is the compressor/gate found in ConsoleX. Well, what's new in compression between last year and this year? BeziComp is what. BeziComp is out, and is my Bezier curve stuff as applied to a compressor. The curve is simply tracking the loudness of the signal and using it as control points.What's that do? It constructs a compression curve that can react VERY quickly, but doesn't follow the normal rules as far as such curves are concerned. It's a gain change with no aliasing because it can never change gain faster than the speed of the curve: it can never get caught on the edges of a waveform and induce artifacts, because it's generating a curve on the fly that can work up to high speed but never just abruptly change the gain causing a noise.My plugins Surge and SurgeTide are a bit like this, and just as free of artifact, but they can't compress in the normal sense, and this can… to some extent. You see, BeziComp is a little unpredictable. You can barely hear it until it's doing crazy things, and it's a striking compression style but not very familiar.So, Dynamics2 adds something else… a whole separate compression routine, also BeziComp, that runs slower. The first compression is run by the Attack control, and you can turn that up to respond extremely fast, or slow it down until attacks are easy to here. Then, the second compression's run by the Release control, and it's always slower than Attack is, and spreads out the response a little. My hope is that it responds more like people need, while retaining the transparency and bounciness BeziCompe can provide.And then, there's the Gate control. Its release speed is shared with the compressor's release speed, and it's a bit sensitive: if it's not kicking in, you might not have the release speed fast enough. The thing about it is, this is another Bezier-curve feature. So, it can snap off very quickly, but without providing a click on either gating or releasing gating.I've got more elaborate gates in store (there's this little thing called DeNoise that's coming out) and it's even possible this will see revision before new versions of Console come out, so this is a good place for feedback on whether this is working nicely for people. The whole engine runs on Bezier curves, so it has some characteristics that automatically beat its predecessor, Dynamics, but that's not to say I can't find ways to update it further.But on a recent livestream, at the end of the day, I asked what people wanted the coming weekend. And I immediately heard back, Dynamics2! And so it is: this week's plugin is a preview or prototype of what goes in ConsoleH, and in ConsoleX2.AU versions contain an extra 'Dynamics2Mono' version, which is coded N to N, meaning it will run pure mono, but also unlinked stereo and any larger number of channels you like (all the AU plugins with 'mono' in the name do this).Let me know how this works for you! There's time to fuss with it a little more, before it's 2026 and it gets built into Console systems!"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class Dynamics2 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    enum
    {
        bez_AL,
        bez_BL,
        bez_CL,
        bez_InL,
        bez_UnInL,
        bez_SampL,
        bez_AR,
        bez_BR,
        bez_CR,
        bez_InR,
        bez_UnInR,
        bez_SampR,
        bez_cycle,
        bez_total
    }; // the new undersampling. bez signifies the bezier curve reconstruction
    double bezCompF[bez_total];
    double bezMaxF;
    double bezCompS[bez_total];
    double bezGate;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Dynamics2()
    {
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 0.0;
        for (int x = 0; x < bez_total; x++) {
            bezCompF[x] = 0.0;
            bezCompS[x] = 0.0;
        }
        bezCompF[bez_cycle] = 1.0;
        bezMaxF = 0.0;
        bezCompS[bez_cycle] = 1.0;
        bezGate = 2.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 1.0; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "thresh"; break;
            case kParamB: return "attack"; break;
            case kParamC: return "release"; break;
            case kParamD: return "gate"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Thresh"; break;
            case kParamB: return "Attack"; break;
            case kParamC: return "Release"; break;
            case kParamD: return "Gate"; break;

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
        double bezCThresh = pow(1.0 - A, 6.0) * 8.0;
        double bezRez = pow(1.0 - B, 8.0) / overallscale;
        double sloRez = pow(1.0 - C, 12.0) / overallscale;
        sloRez = fmin(fmax(sloRez - (bezRez * 0.5), 0.00001), 1.0);
        bezRez = fmin(fmax(bezRez, 0.0001), 1.0);
        double gate = pow(pow(D, 4.0), sqrt(bezCThresh + 1.0));
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            if (fmax(fabs(inputSampleL), fabs(inputSampleR)) > gate + (sloRez * bezGate)) {
                bezGate = ((bezGate * overallscale * 3.0) + 3.0) * (0.25 / overallscale);
            }
            else {
                bezGate = fmax(0.0, bezGate - (sloRez * sloRez));
            }
            if (bezCThresh > 0.0) {
                inputSampleL *= ((bezCThresh * 0.5) + 1.0);
                inputSampleR *= ((bezCThresh * 0.5) + 1.0);
            }
            bezCompF[bez_cycle] += bezRez;
            bezCompF[bez_SampL] += (fabs(inputSampleL) * bezRez);
            bezCompF[bez_SampR] += (fabs(inputSampleR) * bezRez);
            bezMaxF = fmax(bezMaxF, fmax(fabs(inputSampleL), fabs(inputSampleR)));
            if (bezCompF[bez_cycle] > 1.0) {
                bezCompF[bez_cycle] -= 1.0;
                if (bezMaxF < gate) {
                    bezCompF[bez_SampL] = bezMaxF / gate; // note: SampL is a control voltage,
                }
                if (bezCompF[bez_SampL] < gate) {
                    bezCompF[bez_SampL] = 0.0; // not a bipolar audio signal
                }
                bezCompF[bez_CL] = bezCompF[bez_BL];
                bezCompF[bez_BL] = bezCompF[bez_AL];
                bezCompF[bez_AL] = bezCompF[bez_SampL];
                bezCompF[bez_SampL] = 0.0;
                if (bezMaxF < gate) {
                    bezCompF[bez_SampR] = bezMaxF / gate; // note: SampR is a control voltage,
                }
                if (bezCompF[bez_SampR] < gate) {
                    bezCompF[bez_SampR] = 0.0; // not a bipolar audio signal
                }
                bezCompF[bez_CR] = bezCompF[bez_BR];
                bezCompF[bez_BR] = bezCompF[bez_AR];
                bezCompF[bez_AR] = bezCompF[bez_SampR];
                bezCompF[bez_SampR] = 0.0;
                bezMaxF = 0.0;
            }
            bezCompS[bez_cycle] += sloRez;
            bezCompS[bez_SampL] += (fabs(inputSampleL) * sloRez); // note: SampL is a control voltage
            bezCompS[bez_SampR] += (fabs(inputSampleR) * sloRez); // note: SampR is a control voltage
            if (bezCompS[bez_cycle] > 1.0) {
                bezCompS[bez_cycle] -= 1.0;
                if (bezCompS[bez_SampL] < gate) {
                    bezCompS[bez_SampL] = 0.0;
                }
                bezCompS[bez_CL] = bezCompS[bez_BL];
                bezCompS[bez_BL] = bezCompS[bez_AL];
                bezCompS[bez_AL] = bezCompS[bez_SampL];
                bezCompS[bez_SampL] = 0.0;
                if (bezCompS[bez_SampR] < gate) {
                    bezCompS[bez_SampR] = 0.0;
                }
                bezCompS[bez_CR] = bezCompS[bez_BR];
                bezCompS[bez_BR] = bezCompS[bez_AR];
                bezCompS[bez_AR] = bezCompS[bez_SampR];
                bezCompS[bez_SampR] = 0.0;
            }
            double CBFL = (bezCompF[bez_CL] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_BL] * bezCompF[bez_cycle]);
            double BAFL = (bezCompF[bez_BL] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_AL] * bezCompF[bez_cycle]);
            double CBAFL = (bezCompF[bez_BL] + (CBFL * (1.0 - bezCompF[bez_cycle])) + (BAFL * bezCompF[bez_cycle])) * 0.5;
            double CBSL = (bezCompS[bez_CL] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_BL] * bezCompS[bez_cycle]);
            double BASL = (bezCompS[bez_BL] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_AL] * bezCompS[bez_cycle]);
            double CBASL = (bezCompS[bez_BL] + (CBSL * (1.0 - bezCompS[bez_cycle])) + (BASL * bezCompS[bez_cycle])) * 0.5;
            double CBAMax = fmax(CBASL, CBAFL);
            if (CBAMax > 0.0) {
                CBAMax = 1.0 / CBAMax;
            }
            double CBAFade = ((CBASL * -CBAMax) + (CBAFL * CBAMax) + 1.0) * 0.5;
            if (bezCThresh > 0.0) {
                inputSampleL *= 1.0 - (fmin(((CBASL * (1.0 - CBAFade)) + (CBAFL * CBAFade)) * bezCThresh, 1.0));
            }
            double CBFR = (bezCompF[bez_CR] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_BR] * bezCompF[bez_cycle]);
            double BAFR = (bezCompF[bez_BR] * (1.0 - bezCompF[bez_cycle])) + (bezCompF[bez_AR] * bezCompF[bez_cycle]);
            double CBAFR = (bezCompF[bez_BR] + (CBFR * (1.0 - bezCompF[bez_cycle])) + (BAFR * bezCompF[bez_cycle])) * 0.5;
            double CBSR = (bezCompS[bez_CR] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_BR] * bezCompS[bez_cycle]);
            double BASR = (bezCompS[bez_BR] * (1.0 - bezCompS[bez_cycle])) + (bezCompS[bez_AR] * bezCompS[bez_cycle]);
            double CBASR = (bezCompS[bez_BR] + (CBSR * (1.0 - bezCompS[bez_cycle])) + (BASR * bezCompS[bez_cycle])) * 0.5;
            CBAMax = fmax(CBASR, CBAFR);
            if (CBAMax > 0.0) {
                CBAMax = 1.0 / CBAMax;
            }
            CBAFade = ((CBASR * -CBAMax) + (CBAFR * CBAMax) + 1.0) * 0.5;
            if (bezCThresh > 0.0) {
                inputSampleR *= 1.0 - (fmin(((CBASR * (1.0 - CBAFade)) + (CBAFR * CBAFade)) * bezCThresh, 1.0));
            }
            if (bezGate < 1.0 && gate > 0.0) {
                inputSampleL *= bezGate;
                inputSampleR *= bezGate;
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::dynamics2
