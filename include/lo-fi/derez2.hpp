#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::derez2 {

constexpr std::string_view k_name{ "DeRez2" };
constexpr std::string_view k_short_description{
    "DeRez2 brings more authentic retro-digital hardware tones."
};
constexpr std::string_view k_long_description{
    "DeRez is the Airwindows bitcrusher that interpolates a sample between sample-rate-crushed outputs so the top end is smooth rather than gritty, and the only (far as I know) ANALOG bitcrusher (or at least floating point resolution?). That means you can set it to 32 and a third K sample rate, and seven point one three five bits. By ear, please: if you are needing to set a third of a K of sample rate without hearing it, I can’t help you. The point being, DeRez was already cool as a continuous-rate rate-crusher and arbitrary bit depth linear bitcrusher. I don’t think anyone else has that (of course now they can: it’s open source MIT license, so just credit Airwindows and code away)How do you make that not just better but way better?DeRez2’s ‘Hard’ control maxes out as the previous plugin (with a few behind-the-scenes upgrades, but exactly the same algorithm at the heart). But the interesting part is when you turn it OFF: set ‘Hard’ to zero. Two things happen.The sample-rate crusher begins to incorporate intermediate samples in a different way. When it’s changing, it saves up the previous sample… and uses that, not an interpolation, as the intermediate value. It’s trying to bridge the gap between rate-crushed values with a dry sample value. This causes a strange grungy transparency and a zone between ‘clean’ and rate-crushed that’s eerily reminiscent of old digital hardware. It stops sounding in-the-box, even though it remains completely bitcrushed with a totally different texture.The bit-crusher remains ‘analog’ (arbitrary bit depth, like 12 and a half bits) but on full soft, it uses uLaw encode and decode, so it becomes nonlinear! Same as the famous Aphex Twin ‘long play DAT’ and old retro nonlinear digital hardware, the loud parts get bigger ‘steps’ and quiet stuff gets smaller ‘steps’, producing a totally different tonality. You can use this and the sample-rate crush at the same time, subtly or obviously, to dial in vintage-digital tonalities that are totally satisfying and convincing, but completely different from the source audio. You’d never know it started out different because it winds up sounding completely right.I’ve been asked for dedicated emulations of vintage sampler gear. Instead, try this: no copying, but a new way to get that kind of tonality and dial it in to taste. If you need the darkening and texture of classic samplers, DeRez2 will do the same job in a new way with features the real retro gear didn’t have.Why does this one have the dry/wet? Because since the rate-crusher uses the previous sample for transitions, blending it with dry makes the transitions further softened with averaging. You can fade between pristine and clear, dark and cloudy, and totally retro-sampler thanks to that effect (which wouldn’t have happened with the previous DeRez, though you can try it on full Hard and see)What’s with the halfway settings between Soft and Hard? It engages wet/dry balance on the uLaws inside the plugin. If you do that to uLaw, you get weird broken results and it doesn’t work nicely. It just so happens that going from soft to halfway gives a big volume and grunge boost. So rather than have it as a clean off/on control, the Hard control lets you use that unforeseen weirdness as an intentional effect. If you have it dialed in but you’d like to punch up the aggression for effect, automate the Hard control and use it as a booster, for a unique result."
};
constexpr std::string_view k_tags{
    "lo-fi"
};

template <typename T>
class DeRez2 final : public Effect<T>
{
    double lastSampleL;
    double heldSampleL;
    double lastDrySampleL;
    double lastOutputSampleL;
    double lastSampleR;
    double heldSampleR;
    double lastDrySampleR;
    double lastOutputSampleR;
    double position;
    double incrementA;
    double incrementB;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    DeRez2()
    {
        A = 1.0;
        B = 1.0;
        C = 1.0;
        D = 1.0;
        lastSampleL = 0.0;
        heldSampleL = 0.0;
        lastDrySampleL = 0.0;
        lastOutputSampleL = 0.0;
        lastSampleR = 0.0;
        heldSampleR = 0.0;
        lastDrySampleR = 0.0;
        lastOutputSampleR = 0.0;
        position = 0.0;
        incrementA = 0.0;
        incrementB = 0.0;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 1.0;
            break;
        kParamC:
            return 1.0;
            break;
        kParamD:
            return 1.0;
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
            return "rate";
            break;
        kParamB:
            return "rez";
            break;
        kParamC:
            return "hard";
            break;
        kParamD:
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
            return "Rate";
            break;
        kParamB:
            return "Rez";
            break;
        kParamC:
            return "Hard";
            break;
        kParamD:
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

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
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

        double targetA = pow(A, 3) + 0.0005;
        if (targetA > 1.0) {
            targetA = 1.0;
        }
        double soften = (1.0 + targetA) / 2;
        double targetB = pow(1.0 - B, 3) / 3;
        double hard = C;
        double wet = D;
        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        targetA /= overallscale;
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
            incrementA = ((incrementA * 999.0) + targetA) / 1000.0;
            incrementB = ((incrementB * 999.0) + targetB) / 1000.0;
            // incrementA is the frequency derez
            // incrementB is the bit depth derez
            position += incrementA;
            double outputSampleL = heldSampleL;
            double outputSampleR = heldSampleR;
            if (position > 1.0)
            {
                position -= 1.0;
                heldSampleL = (lastSampleL * position) + (inputSampleL * (1.0 - position));
                outputSampleL = (outputSampleL * (1.0 - soften)) + (heldSampleL * soften);
                // softens the edge of the derez
                heldSampleR = (lastSampleR * position) + (inputSampleR * (1.0 - position));
                outputSampleR = (outputSampleR * (1.0 - soften)) + (heldSampleR * soften);
                // softens the edge of the derez
            }
            inputSampleL = outputSampleL;
            inputSampleR = outputSampleR;
            double tempL = inputSampleL;
            double tempR = inputSampleR;
            if (inputSampleL != lastOutputSampleL) {
                tempL = inputSampleL;
                inputSampleL = (inputSampleL * hard) + (lastDrySampleL * (1.0 - hard));
                // transitions get an intermediate dry sample
                lastOutputSampleL = tempL; // only one intermediate sample
            }
            else {
                lastOutputSampleL = inputSampleL;
            }
            if (inputSampleR != lastOutputSampleR) {
                tempR = inputSampleR;
                inputSampleR = (inputSampleR * hard) + (lastDrySampleR * (1.0 - hard));
                // transitions get an intermediate dry sample
                lastOutputSampleR = tempR; // only one intermediate sample
            }
            else {
                lastOutputSampleR = inputSampleR;
            }
            lastDrySampleL = drySampleL;
            lastDrySampleR = drySampleR;
            // freq section of soft/hard interpolates dry samples
            tempL = inputSampleL;
            tempR = inputSampleR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleL < 0) {
                inputSampleL = -log(1.0 + (255 * fabs(inputSampleL))) / log(256);
            }
            if (inputSampleR > 0) {
                inputSampleR = log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
            if (inputSampleR < 0) {
                inputSampleR = -log(1.0 + (255 * fabs(inputSampleR))) / log(256);
            }
            inputSampleL = (tempL * hard) + (inputSampleL * (1.0 - hard));
            inputSampleR = (tempR * hard) + (inputSampleR * (1.0 - hard)); // uLaw encode as part of soft/hard
            tempL = inputSampleL;
            tempR = inputSampleR;
            if (incrementB > 0.0005)
            {
                if (inputSampleL > 0)
                {
                    tempL = inputSampleL;
                    while (tempL > 0) {
                        tempL -= incrementB;
                    }
                    inputSampleL -= tempL;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleR > 0)
                {
                    tempR = inputSampleR;
                    while (tempR > 0) {
                        tempR -= incrementB;
                    }
                    inputSampleR -= tempR;
                    // it's below 0 so subtracting adds the remainder
                }
                if (inputSampleL < 0)
                {
                    tempL = inputSampleL;
                    while (tempL < 0) {
                        tempL += incrementB;
                    }
                    inputSampleL -= tempL;
                    // it's above 0 so subtracting subtracts the remainder
                }
                if (inputSampleR < 0)
                {
                    tempR = inputSampleR;
                    while (tempR < 0) {
                        tempR += incrementB;
                    }
                    inputSampleR -= tempR;
                    // it's above 0 so subtracting subtracts the remainder
                }
                inputSampleL *= (1.0 - incrementB);
                inputSampleR *= (1.0 - incrementB);
            }
            tempL = inputSampleL;
            tempR = inputSampleR;
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            if (inputSampleL > 0) {
                inputSampleL = (pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleL < 0) {
                inputSampleL = -(pow(256, fabs(inputSampleL)) - 1.0) / 255;
            }
            if (inputSampleR > 0) {
                inputSampleR = (pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            if (inputSampleR < 0) {
                inputSampleR = -(pow(256, fabs(inputSampleR)) - 1.0) / 255;
            }
            inputSampleL = (tempL * hard) + (inputSampleL * (1.0 - hard));
            inputSampleR = (tempR * hard) + (inputSampleR * (1.0 - hard)); // uLaw decode as part of soft/hard
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
            lastSampleL = drySampleL;
            lastSampleR = drySampleR;
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
} // namespace airwindohhs::derez2
