#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::distance2 {

constexpr std::string_view k_name{ "Distance2" };
constexpr std::string_view k_short_description{
    "Distance2 is a versatile space shaper for creating depth."
};
constexpr std::string_view k_long_description{
    "As requested, this is the unholy hybrid of Distance and Atmosphere. This one doesn’t work like a Console5 system, it’s strictly ‘put it on and get a sound’, but I wasn’t expecting how cool it would be. Turns out this thing is completely absurdly good at taking tracks like drums, and making them huge and pounding and stage-like, without even the use of reverb or compression.The beginning of the video’s about using Distance2 as a loudenator, though I think using it on a full mix is overkill (maybe you want SOME elements to be up front and present). Bear in mind that you can keep the Atmosphere control set very low and still get an effect: the equivalent to the Atmosphere mixing system is to have it incredibly low, like 0.1 or less. The more you push it, the more nasty it’ll get, because that algorithm alone is NOT enough to make a distance sound. It’s not doing any of the high frequency attenuation you’d get, so technically the sound of high Atmosphere settings is the sound of extreme loudness rupturing the air and your eardrums: pushed hard, it’s unrecognizable as any natural sound. You wouldn’t survive exposure to a sound so loud that it broke the air like that.But when you also include the Darken control, that’s when things start sounding realistic again. This is one of those plugins where I could have built these into a single control to deliver good-sounding results no matter what setting you used… and where I chose to give you access to the wrongest possible settings because people NEED to break rules sometimes. Somebody out there is going to be able to get a great sound by taking the right source, and obliterating it with extreme Distance2 settings, and who am I to stand in the way? And you can also apply a dry/wet that will conceal the wreckage: surprisingly small amounts of dry signal will mask the amount of distortion going on.And the reason I’m able to put out a plugin where I KNOW that some people will set it wrongly and then hate it, is my Patreon. The thing about Patreon is that when it works, I’m completely protected from having to make things market friendly. Market friendly is a curse: it makes you do only predictable things that most people would like, and it punishes you if you want to do something unpopular, or if you want to take something great and widen the range until people get into trouble with it. It’s safer to give people presets that are known to behave on all source audio, that always sound nice. It’s safer to give people a pile of mulch than a chainsaw.Patreon lets me give you the chainsaw :D now, whether you do damage with it is your own affair. But I think once you strap it across some buss with drums or guitars or whatever, and fire it up, you’ll like chainsaws too. Just remember to dial it back when you need it not to be distractingly obnoxious. Or not: hey, it IS a distinct new distortion voice, with a whole new approach to slew clipping not previously available. Darken it or not, as it pleases you. Have fun."
};
constexpr std::string_view k_tags{
    "filter"
};

template <typename T>
class Distance2 final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double lastSampleAL;
    double lastSampleBL;
    double lastSampleCL;
    double lastSampleDL;
    double lastSampleEL;
    double lastSampleFL;
    double lastSampleGL;
    double lastSampleHL;
    double lastSampleIL;
    double lastSampleJL;
    double lastSampleKL;
    double lastSampleLL;
    double lastSampleML;
    double lastSampleAR;
    double lastSampleBR;
    double lastSampleCR;
    double lastSampleDR;
    double lastSampleER;
    double lastSampleFR;
    double lastSampleGR;
    double lastSampleHR;
    double lastSampleIR;
    double lastSampleJR;
    double lastSampleKR;
    double lastSampleLR;
    double lastSampleMR;
    double thresholdA;
    double thresholdB;
    double thresholdC;
    double thresholdD;
    double thresholdE;
    double thresholdF;
    double thresholdG;
    double thresholdH;
    double thresholdI;
    double thresholdJ;
    double thresholdK;
    double thresholdL;
    double thresholdM;
    double thirdSampleL;
    double lastSampleL;
    double thirdSampleR;
    double lastSampleR;
    float A;
    float B;
    float C;

  public:
    Distance2()
    {
        A = 0.85;
        B = 0.618;
        C = 0.618;
        thirdSampleL = lastSampleL = 0.0;
        thirdSampleR = lastSampleR = 0.0;
        lastSampleAL = 0.0;
        lastSampleBL = 0.0;
        lastSampleCL = 0.0;
        lastSampleDL = 0.0;
        lastSampleEL = 0.0;
        lastSampleFL = 0.0;
        lastSampleGL = 0.0;
        lastSampleHL = 0.0;
        lastSampleIL = 0.0;
        lastSampleJL = 0.0;
        lastSampleKL = 0.0;
        lastSampleLL = 0.0;
        lastSampleML = 0.0;
        lastSampleAR = 0.0;
        lastSampleBR = 0.0;
        lastSampleCR = 0.0;
        lastSampleDR = 0.0;
        lastSampleER = 0.0;
        lastSampleFR = 0.0;
        lastSampleGR = 0.0;
        lastSampleHR = 0.0;
        lastSampleIR = 0.0;
        lastSampleJR = 0.0;
        lastSampleKR = 0.0;
        lastSampleLR = 0.0;
        lastSampleMR = 0.0;
        thresholdA = 0.618033988749894;
        thresholdB = 0.679837387624884;
        thresholdC = 0.747821126387373;
        thresholdD = 0.82260323902611;
        thresholdE = 0.904863562928721;
        thresholdF = 0.995349919221593;
        thresholdG = 1.094884911143752;
        thresholdH = 1.204373402258128;
        thresholdI = 1.32481074248394;
        thresholdJ = 1.457291816732335;
        thresholdK = 1.603020998405568;
        thresholdL = 1.763323098246125;
        thresholdM = 1.939655408070737;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;
            case kParamC: return C;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.85;
            case kParamB: return 0.618;
            case kParamC: return 0.618;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "atmosph";
            case kParamB: return "darken";
            case kParamC: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Atmosph";
            case kParamB: return "Darken";
            case kParamC: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string(B);
            case kParamC: return std::to_string(C);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "";
            case kParamB: return "";
            case kParamC: return "";
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
        overallscale *= getSampleRate();
        thresholdA = 0.618033988749894 / overallscale;
        thresholdB = 0.679837387624884 / overallscale;
        thresholdC = 0.747821126387373 / overallscale;
        thresholdD = 0.82260323902611 / overallscale;
        thresholdE = 0.904863562928721 / overallscale;
        thresholdF = 0.995349919221593 / overallscale;
        thresholdG = 1.094884911143752 / overallscale;
        thresholdH = 1.204373402258128 / overallscale;
        thresholdI = 1.32481074248394 / overallscale;
        thresholdJ = 1.457291816732335 / overallscale;
        thresholdK = 1.603020998405568 / overallscale;
        thresholdL = 1.763323098246125 / overallscale;
        thresholdM = 1.939655408070737 / overallscale;
        double softslew = (pow(A, 3) * 24) + .6;
        softslew *= overallscale;
        double filter = softslew * B;
        double secondfilter = filter / 3.0;
        double thirdfilter = filter / 5.0;
        double offsetScale = A * 0.1618;
        double levelcorrect = 1.0 + ((filter / 12.0) * A);
        // bring in top slider again to manage boost level for lower settings
        double wet = C;
        // removed extra dry variable
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
            double offsetL = offsetScale - (lastSampleL - inputSampleL);
            double offsetR = offsetScale - (lastSampleR - inputSampleR);
            inputSampleL += (offsetL * offsetScale); // extra bit from Loud: offset air compression
            inputSampleL *= wet; // clean up w. dry introduced
            inputSampleL *= softslew; // scale into Atmosphere algorithm
            inputSampleR += (offsetR * offsetScale); // extra bit from Loud: offset air compression
            inputSampleR *= wet; // clean up w. dry introduced
            inputSampleR *= softslew; // scale into Atmosphere algorithm
            // left
            double clamp = inputSampleL - lastSampleAL;
            if (clamp > thresholdA) {
                inputSampleL = lastSampleAL + thresholdA;
            }
            if (-clamp > thresholdA) {
                inputSampleL = lastSampleAL - thresholdA;
            }
            clamp = inputSampleL - lastSampleBL;
            if (clamp > thresholdB) {
                inputSampleL = lastSampleBL + thresholdB;
            }
            if (-clamp > thresholdB) {
                inputSampleL = lastSampleBL - thresholdB;
            }
            clamp = inputSampleL - lastSampleCL;
            if (clamp > thresholdC) {
                inputSampleL = lastSampleCL + thresholdC;
            }
            if (-clamp > thresholdC) {
                inputSampleL = lastSampleCL - thresholdC;
            }
            clamp = inputSampleL - lastSampleDL;
            if (clamp > thresholdD) {
                inputSampleL = lastSampleDL + thresholdD;
            }
            if (-clamp > thresholdD) {
                inputSampleL = lastSampleDL - thresholdD;
            }
            clamp = inputSampleL - lastSampleEL;
            if (clamp > thresholdE) {
                inputSampleL = lastSampleEL + thresholdE;
            }
            if (-clamp > thresholdE) {
                inputSampleL = lastSampleEL - thresholdE;
            }
            clamp = inputSampleL - lastSampleFL;
            if (clamp > thresholdF) {
                inputSampleL = lastSampleFL + thresholdF;
            }
            if (-clamp > thresholdF) {
                inputSampleL = lastSampleFL - thresholdF;
            }
            clamp = inputSampleL - lastSampleGL;
            if (clamp > thresholdG) {
                inputSampleL = lastSampleGL + thresholdG;
            }
            if (-clamp > thresholdG) {
                inputSampleL = lastSampleGL - thresholdG;
            }
            clamp = inputSampleL - lastSampleHL;
            if (clamp > thresholdH) {
                inputSampleL = lastSampleHL + thresholdH;
            }
            if (-clamp > thresholdH) {
                inputSampleL = lastSampleHL - thresholdH;
            }
            clamp = inputSampleL - lastSampleIL;
            if (clamp > thresholdI) {
                inputSampleL = lastSampleIL + thresholdI;
            }
            if (-clamp > thresholdI) {
                inputSampleL = lastSampleIL - thresholdI;
            }
            clamp = inputSampleL - lastSampleJL;
            if (clamp > thresholdJ) {
                inputSampleL = lastSampleJL + thresholdJ;
            }
            if (-clamp > thresholdJ) {
                inputSampleL = lastSampleJL - thresholdJ;
            }
            clamp = inputSampleL - lastSampleKL;
            if (clamp > thresholdK) {
                inputSampleL = lastSampleKL + thresholdK;
            }
            if (-clamp > thresholdK) {
                inputSampleL = lastSampleKL - thresholdK;
            }
            clamp = inputSampleL - lastSampleLL;
            if (clamp > thresholdL) {
                inputSampleL = lastSampleLL + thresholdL;
            }
            if (-clamp > thresholdL) {
                inputSampleL = lastSampleLL - thresholdL;
            }
            clamp = inputSampleL - lastSampleML;
            if (clamp > thresholdM) {
                inputSampleL = lastSampleML + thresholdM;
            }
            if (-clamp > thresholdM) {
                inputSampleL = lastSampleML - thresholdM;
            }
            // right
            clamp = inputSampleR - lastSampleAR;
            if (clamp > thresholdA) {
                inputSampleR = lastSampleAR + thresholdA;
            }
            if (-clamp > thresholdA) {
                inputSampleR = lastSampleAR - thresholdA;
            }
            clamp = inputSampleR - lastSampleBR;
            if (clamp > thresholdB) {
                inputSampleR = lastSampleBR + thresholdB;
            }
            if (-clamp > thresholdB) {
                inputSampleR = lastSampleBR - thresholdB;
            }
            clamp = inputSampleR - lastSampleCR;
            if (clamp > thresholdC) {
                inputSampleR = lastSampleCR + thresholdC;
            }
            if (-clamp > thresholdC) {
                inputSampleR = lastSampleCR - thresholdC;
            }
            clamp = inputSampleR - lastSampleDR;
            if (clamp > thresholdD) {
                inputSampleR = lastSampleDR + thresholdD;
            }
            if (-clamp > thresholdD) {
                inputSampleR = lastSampleDR - thresholdD;
            }
            clamp = inputSampleR - lastSampleER;
            if (clamp > thresholdE) {
                inputSampleR = lastSampleER + thresholdE;
            }
            if (-clamp > thresholdE) {
                inputSampleR = lastSampleER - thresholdE;
            }
            clamp = inputSampleR - lastSampleFR;
            if (clamp > thresholdF) {
                inputSampleR = lastSampleFR + thresholdF;
            }
            if (-clamp > thresholdF) {
                inputSampleR = lastSampleFR - thresholdF;
            }
            clamp = inputSampleR - lastSampleGR;
            if (clamp > thresholdG) {
                inputSampleR = lastSampleGR + thresholdG;
            }
            if (-clamp > thresholdG) {
                inputSampleR = lastSampleGR - thresholdG;
            }
            clamp = inputSampleR - lastSampleHR;
            if (clamp > thresholdH) {
                inputSampleR = lastSampleHR + thresholdH;
            }
            if (-clamp > thresholdH) {
                inputSampleR = lastSampleHR - thresholdH;
            }
            clamp = inputSampleR - lastSampleIR;
            if (clamp > thresholdI) {
                inputSampleR = lastSampleIR + thresholdI;
            }
            if (-clamp > thresholdI) {
                inputSampleR = lastSampleIR - thresholdI;
            }
            clamp = inputSampleR - lastSampleJR;
            if (clamp > thresholdJ) {
                inputSampleR = lastSampleJR + thresholdJ;
            }
            if (-clamp > thresholdJ) {
                inputSampleR = lastSampleJR - thresholdJ;
            }
            clamp = inputSampleR - lastSampleKR;
            if (clamp > thresholdK) {
                inputSampleR = lastSampleKR + thresholdK;
            }
            if (-clamp > thresholdK) {
                inputSampleR = lastSampleKR - thresholdK;
            }
            clamp = inputSampleR - lastSampleLR;
            if (clamp > thresholdL) {
                inputSampleR = lastSampleLR + thresholdL;
            }
            if (-clamp > thresholdL) {
                inputSampleR = lastSampleLR - thresholdL;
            }
            clamp = inputSampleR - lastSampleMR;
            if (clamp > thresholdM) {
                inputSampleR = lastSampleMR + thresholdM;
            }
            if (-clamp > thresholdM) {
                inputSampleR = lastSampleMR - thresholdM;
            }
            lastSampleML = lastSampleLL;
            lastSampleLL = lastSampleKL;
            lastSampleKL = lastSampleJL;
            lastSampleJL = lastSampleIL;
            lastSampleIL = lastSampleHL;
            lastSampleHL = lastSampleGL;
            lastSampleGL = lastSampleFL;
            lastSampleFL = lastSampleEL;
            lastSampleEL = lastSampleDL;
            lastSampleDL = lastSampleCL;
            lastSampleCL = lastSampleBL;
            lastSampleBL = lastSampleAL;
            lastSampleAL = drySampleL;
            // store the raw input sample again for use next time
            lastSampleMR = lastSampleLR;
            lastSampleLR = lastSampleKR;
            lastSampleKR = lastSampleJR;
            lastSampleJR = lastSampleIR;
            lastSampleIR = lastSampleHR;
            lastSampleHR = lastSampleGR;
            lastSampleGR = lastSampleFR;
            lastSampleFR = lastSampleER;
            lastSampleER = lastSampleDR;
            lastSampleDR = lastSampleCR;
            lastSampleCR = lastSampleBR;
            lastSampleBR = lastSampleAR;
            lastSampleAR = drySampleR;
            // store the raw input sample again for use next time
            inputSampleL *= levelcorrect;
            inputSampleL /= softslew;
            inputSampleL -= (offsetL * offsetScale);
            // begin IIR stage
            inputSampleR *= levelcorrect;
            inputSampleR /= softslew;
            inputSampleR -= (offsetR * offsetScale);
            // begin IIR stage
            inputSampleL += (thirdSampleL * thirdfilter);
            inputSampleL /= (thirdfilter + 1.0);
            inputSampleL += (lastSampleL * secondfilter);
            inputSampleL /= (secondfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            inputSampleR += (thirdSampleR * thirdfilter);
            inputSampleR /= (thirdfilter + 1.0);
            inputSampleR += (lastSampleR * secondfilter);
            inputSampleR /= (secondfilter + 1.0);
            // do an IIR like thing to further squish superdistant stuff
            thirdSampleL = lastSampleL;
            lastSampleL = inputSampleL;
            inputSampleL *= levelcorrect;
            thirdSampleR = lastSampleR;
            lastSampleR = inputSampleR;
            inputSampleR *= levelcorrect;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::distance2
