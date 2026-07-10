#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::longhand {

constexpr std::string_view k_name{ "Longhand" };
constexpr std::string_view k_short_description{
    "Longhand is an experiment in softclipping."
};
constexpr std::string_view k_long_description{
    "A sine is a very interesting thing, in part because it's so very simple.Think of a circle. A sine (and cosine) is what gives you that shape. It's a simple arc in space. If you use the mathematical function sin(), it gives you a waveshaper, but if you don't clamp it to half of pi (a constant called M_PI_2) you get a wave FOLDER for your trouble. As your wave gets louder it starts to wrap around again, first to zero and then all the way to negative 1 (the output of sin(), no matter what the input, will return something between 1 and -1).Very simple, and it's the waveshaper with the absolute gentlest transtion between linearity and clipping. Interestingly this means there's a sharp transition at exactly zero when the rate of curvature flips, and this gave rise to my plugin Spiral (and newer versions of Channel), but as a simple function sin() has the softest clipping anything can have.Does that make sin(), in every C compiler and every computer, perfect?There's a catch. To implement sin(), nothing out there really does it in the cumbersome, calculating way. There's optimizations. As you hit exactly 1.0 (or -1.0) in the output, that's when you can begin referring to a lookup table, but backwards. You can store a series of values and interpolate between them. You can build a dedicated hardware unit into your CPU to do it, or your GPU. You might get ambitious with this, since you're probably drawing triangles and can't be bothered with details. sin() is simple, but implementing it can be hacktastic. And of course it'll still be perfect, because it's math and it's a computer, and what else could it be?Longhand's an experiment in checking this notion, for the purposes of audio.First, there was PurestSaturation. It uses a form of approximation known as a Taylor Series, where you make a temporary number, and keep multiplying it by itself, and then alternately adding and subtracting the results by very specific amounts. The purpose of PurestSaturation was, 'what if we make a sin()-like saturation, but intentionally use fewer terms than you'd normally have, so there's less math in it? and then to top it off, what if we only use powers of 2 for the adding and subtracting and the scaling, so we're retaining the mantissas of the temp variables even though our sine isn't accurate anymore? It'll be close, but it'll be a different thing. What happens then?'And so, that's PurestSaturation. TapeHack has some similarities to this. Both are intentional departures from the majesty of sin().But… what if we put our working variables in long double precision just for sheer overkill, and then rather than departing, we worked out LOTS of terms for a Taylor Series of sin(), beyond the point that even long double can follow? For starters, we are only doing simple multiplies and additions and subtractions. That's all a Taylor Series is. What that does is, rather than model a sine in discrete quadrants and mapping it to where it's needed, we're calculating the wave as ONE continuous function. Every new term means more calculation, but also another layer by which this continuous function can wrap around, wavefold again and again, and produce a sine as a result.Once we stop adding more powers to the temp variable, it stops being a sine and becomes a runaway wave-shaper, producing an output that's no longer countered by a higher order of temp variable pushing the opposite direction… and at that point we have to clip or otherwise stop, or we get infinity out of the equation. It's never a perfect wavefolder unless it's infinite processing… but one thing it is, is a continuous function. It'll wavefold around without ever transitioning to a lookup table or using any sort of optimization. Longhand, in a word.What does it SOUND like?The answer to that question is also why the dry/wet control doesn't work as expected. Turns out I followed this one all the way through to release, without removing one little detail: it includes 'dry' in its gain trim, which can slam the processing right up to its final, no-further-wavefolds, ultimate peak. I had some consternation trying to figure out what I was even doing but that's what it was. The maximum gain (substantial!) is what it takes to get Longhand to its final peak. The inputs are hardclipped, -1.0 to 1.0, for that exact reason and no other. There's a taper to the control but 0.5 is not unity gain, it's just pretty close to it. All of this was so I could run dry into normal 'sin()' and then subtract it and use BitShiftGain to bump up the delta and see what, if anything, was there.And there really is the tiniest difference between the math library, optimized one, and Longhand. It made sounds when nulled out in this way. Does that make Longhand better? It's certainly the tiniest amount more linear as the waveform rounds over the top, because it's continuing the same algorithm. Now, of course, the dry/wet is returned to its original purpose… sort of. But since it's not true Dry, it instead is an output that can boost past clipping. Any DAW that can run Longhand is a floating point buss, so you can pad it down again, getting a dry/wet where you're effectively blending between dry and a soft-saturation that's also lowering its clip threshold as it goes. Or, you can ignore dry/wet unless you're using Longhand for an ultra soft clip and it's not soft ENOUGH. If you've got any Dry in the signal and you're not clipping the output, that guarantees Longhand is giving you a softclip that's even softer than sin().This was an experiment that got out of the laboratory since I've been scrambling so hard to finish up more demanding plugins. If you leave dry/wet out of it, Longhand is the purest and most continuous of soft saturators. Apologies for the odd behavior of the dry/wet, it got past me and there is so much else to do that I'm going to have to let it stand. Apart from that, is this a giant improvement over math_library sin()? No, because there's no way to have it be a big change and also still be sin(). But it is different, in that it's a very very high accuracy approximation that is continuous through the point where the math library uses an approximation, mirroring, and quadrants to return a result from any value you pass in.This is not that, it's very restricted and goes the cumbersome long way round to return a value. But if there's fluidity to be had by working the answer out in longhand, this is how that's done, and that's the experiment."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class Longhand final : public Effect<T>
{
    float A;
    float B;
    float C;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Longhand()
    {
        A = 0.445;
        B = 1.0;
        C = 1.0;
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
        switch (static_cast<params>(index)) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.445; break;
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "input"; break;
            case kParamB: return "output"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Input"; break;
            case kParamB: return "Output"; break;
            case kParamC: return "Dry/Wet"; break;

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

        double inputGain = pow(A, 2.0) * 7.72; // normalized input does not wavefold too far
        double outputGain = B;
        double wet = C;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL = fmin(fmax(inputSampleL, -1.0), 1.0);
            inputSampleR = fmin(fmax(inputSampleR, -1.0), 1.0);
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
                drySampleL *= inputGain;
                drySampleR *= inputGain;
            }
            long double X = inputSampleL;
            X *= X; // long double for even
            long double temp = inputSampleL * X; // the initial multiplies
            inputSampleL -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            inputSampleL += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            inputSampleL -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            inputSampleL += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            inputSampleL -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            inputSampleL += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            inputSampleL -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            inputSampleL += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            inputSampleL -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            inputSampleL += temp * 0.00000000000000000001957294106339126;
            X = inputSampleR;
            X *= X; // long double for even
            temp = inputSampleR * X; // the initial multiplies
            inputSampleR -= temp * 0.16666666666666666666666666666666666;
            temp *= X;
            inputSampleR += temp * 0.00833333333333333333333333333333333;
            temp *= X;
            inputSampleR -= temp * 0.00019841269841269841269841269841269;
            temp *= X;
            inputSampleR += temp * 0.00000275573192239858906525573192239;
            temp *= X;
            inputSampleR -= temp * 0.00000002505210838544171877521083854;
            temp *= X;
            inputSampleR += temp * 0.00000000016059043836821614599392377;
            temp *= X;
            inputSampleR -= temp * 0.00000000000076471637318198164759011;
            temp *= X;
            inputSampleR += temp * 0.00000000000000281145725434552076319;
            temp *= X;
            inputSampleR -= temp * 0.00000000000000000822063524662432971;
            temp *= X;
            inputSampleR += temp * 0.00000000000000000001957294106339126;
            // retain mantissa of a long double increasing power function
            // long double probably doesn't handle more than 36 digits or so
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
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
} // namespace airwindohhs::longhand
