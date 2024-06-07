#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::discontinuity {

constexpr std::string_view k_name{ "Discontinuity" };
constexpr std::string_view k_short_description{
    "Discontinuity models air under intense loudness."
};
constexpr std::string_view k_long_description{
    "This might be the most important subtle sound effect I've ever done.Air isn't linear. That's why DAWs don't sound like reality: they are literally too perfect, in that their transients, their sound combining, every aspect of their operation has no error at all. One would assume this would produce perfect sound, but some of us have never shut up about our grievances with it. (we just lost a titan of that grievance in Steve Albini, but he's far from alone in that.)If you have sounds in air, they sound real even while the air itself distorts them. Much like my recent work with capacitors modulating their values under voltage pressure (up to 80% in some cases!), air modulates the speed of sound under AIR pressure. This makes incredibly obvious and intense crackles on loud sounds like rocket takeoffs, but many of us have heard this crackle at things like rock concerts, especially in a really live room like a hockey rink.Discontinuity simply adds THAT distortion. At loudnesses from 70 dB to 140 dB. That's all it does, and at loudnesses below 110 dB or so it's quite subtle… but I've found use for it as quiet as 71dB (the voice tracks on my last two videos!)I won't say it's correct and accurate at 140dB: I include that because people will enjoy it so much, but I'm not using that much. I just have to… because people will enjoy it, and because it's impossible not to hear when cranked that high.How to use Discontinuity? At any point in the mix where there's a sound, apply it so that the loudest possible sound (typically 0dB, or clipping) matches the loudness you need. If your sound peaks at 10 dB quieter than clipping, and the sound needs to seem like it's 102 dB, set Discontinuity to 112 dB. And listen! There will be an obvious sweet spot where it starts to seem exactly right, and you can dial in the apparent loudness as if it was a tone or EQ move.Discontinuity does its frequency modulation using sample buffers. For that reason, it permanently has a bit of latency and it's never quite the same latency because it's frequency modulated by the track it's on. For that reason, you could put it on every track and drum mic as long as you give up the idea of phase coherence. It's better on minimally miked things or possibly submixes, and on distinct sounds that don't need to keep perfect phase alignment with each other.Depending on who you are and what you've dreamed of being able to make sound do, you might immediately not care about any of that, and immediately start using it on everything and never stop. That's me. It's like when I invented Console, only more so. The interesting thing is how useful I find the quiet, subtle settings when getting a mix to gel and come alive like it's a real sonic event happening. I can set very delicate and quiet, against super loud, and have them all just work.Discontinuity is a fundamental part of ConsoleX, which I'm still working on. I hope you like getting a little piece of the mixing revolution early, so you can learn about it :)"
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Discontinuity final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double dBaL[dscBuf + 5];
    double dBaPosL;
    int dBaXL;
    double dBbL[dscBuf + 5];
    double dBbPosL;
    int dBbXL;
    double dBcL[dscBuf + 5];
    double dBcPosL;
    int dBcXL;
    double dBaR[dscBuf + 5];
    double dBaPosR;
    int dBaXR;
    double dBbR[dscBuf + 5];
    double dBbPosR;
    int dBbXR;
    double dBcR[dscBuf + 5];
    double dBcPosR;
    int dBcXR;
    float A;

  public:
    Discontinuity()
    {
        A = 0.5;
        for (int count = 0; count < dscBuf + 2; count++) {
            dBaL[count] = 0.0;
            dBbL[count] = 0.0;
            dBcL[count] = 0.0;
            dBaR[count] = 0.0;
            dBbR[count] = 0.0;
            dBcR[count] = 0.0;
        }
        dBaPosL = 0.0;
        dBbPosL = 0.0;
        dBcPosL = 0.0;
        dBaPosR = 0.0;
        dBbPosR = 0.0;
        dBcPosR = 0.0;
        dBaXL = 1;
        dBbXL = 1;
        dBcXL = 1;
        dBaXR = 1;
        dBbXR = 1;
        dBcXR = 1;
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
            case kParamA: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "top db"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Top dB"; break;

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
            case kParamA: return "dB"; break;

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
        double refdB = (A * 70.0) + 70.0;
        double topdB = 0.000000075 * pow(10.0, refdB / 20.0) * overallscale;
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
            inputSampleL *= topdB;
            if (inputSampleL < -0.222) {
                inputSampleL = -0.222;
            }
            if (inputSampleL > 0.222) {
                inputSampleL = 0.222;
            }
            // Air Discontinuity A begin
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
            // Air Discontinuity A end
            // Air Discontinuity B begin
            dBbL[dBbXL] = inputSampleL;
            dBbPosL *= 0.5;
            dBbPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBbPosL * dscBuf);
            dBi = (dBbPosL * dscBuf) - dBdly;
            inputSampleL = dBbL[dBbXL - dBdly + ((dBbXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBbL[dBbXL - dBdly + ((dBbXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBbXL++;
            if (dBbXL < 0 || dBbXL >= dscBuf) {
                dBbXL = 0;
            }
            // Air Discontinuity B end
            // Air Discontinuity C begin
            dBcL[dBcXL] = inputSampleL;
            dBcPosL *= 0.5;
            dBcPosL += fabs((inputSampleL * ((inputSampleL * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBcPosL * dscBuf);
            dBi = (dBcPosL * dscBuf) - dBdly;
            inputSampleL = dBcL[dBcXL - dBdly + ((dBcXL - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleL += dBcL[dBcXL - dBdly + ((dBcXL - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBcXL++;
            if (dBcXL < 0 || dBcXL >= dscBuf) {
                dBcXL = 0;
            }
            // Air Discontinuity C end
            inputSampleL /= topdB;
            inputSampleR *= topdB;
            if (inputSampleR < -0.222) {
                inputSampleR = -0.222;
            }
            if (inputSampleR > 0.222) {
                inputSampleR = 0.222;
            }
            // Air Discontinuity A begin
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
            // Air Discontinuity A end
            // Air Discontinuity B begin
            dBbR[dBbXR] = inputSampleR;
            dBbPosR *= 0.5;
            dBbPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBbPosR * dscBuf);
            dBi = (dBbPosR * dscBuf) - dBdly;
            inputSampleR = dBbR[dBbXR - dBdly + ((dBbXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBbR[dBbXR - dBdly + ((dBbXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBbXR++;
            if (dBbXR < 0 || dBbXR >= dscBuf) {
                dBbXR = 0;
            }
            // Air Discontinuity B end
            // Air Discontinuity C begin
            dBcR[dBcXR] = inputSampleR;
            dBcPosR *= 0.5;
            dBcPosR += fabs((inputSampleR * ((inputSampleR * 0.25) - 0.5)) * 0.5);
            dBdly = floor(dBcPosR * dscBuf);
            dBi = (dBcPosR * dscBuf) - dBdly;
            inputSampleR = dBcR[dBcXR - dBdly + ((dBcXR - dBdly < 0) ? dscBuf : 0)] * (1.0 - dBi);
            dBdly++;
            inputSampleR += dBcR[dBcXR - dBdly + ((dBcXR - dBdly < 0) ? dscBuf : 0)] * dBi;
            dBcXR++;
            if (dBcXR < 0 || dBcXR >= dscBuf) {
                dBcXR = 0;
            }
            // Air Discontinuity C end
            inputSampleR /= topdB;
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
} // namespace airwindohhs::discontinuity
