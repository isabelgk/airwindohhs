#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::unbox {

constexpr std::string_view k_name{ "UnBox" };
constexpr std::string_view k_short_description{
    "UnBox is a distortion where only the harmonics that don't alias are allowed to distort."
};
constexpr std::string_view k_long_description{
    "While I’m putting out my library of plugins according to plan, sometimes I need to take a detour into new stuff. UnBox is one example.The idea’s as follows: if you distort stuff digitally, it aliases. This can be seen as harmonics seemingly bouncing off the highest frequency, and going back down again. The idea is that if you have digital saturation or distortion without massive oversampling, you’ll always have aliasing and everything is ruined forever.That’s not quite true. It depends on the frequency, and the form of distortion… and many of my plugins have gentle enough distortion curves that they throw a limited number of harmonics. If you are only generating harmonics within the range of digital audio’s frequencies, you’re fine and there will be no problem until you feed the system a frequency that’s too high. You’re not automatically feeding superhigh frequencies all the time if you’re working with natural recordings: not all sounds contain that kind of high frequency content.If you DO have that sort of high frequency content, what then? It occurred to me I could take the difference between dry and distorted, store it in an averaging filter, and average it. This would suppress high frequency content in only the distortion artifacts. (I then learned that I needed to average the signal being fed to the distortion part, which is Spiral again: it got a little complicated)And I could even highpass the distortion part… and all this is applying only to the distortion part. It’s all handled as a single subtract from the raw signal coming in.What that means is this: UnBox is a distortion that cuts down the level of the signal, but ONLY the mids. Depending on how it’s set, it will let through more and more of the ‘dry’ highs, unaffected. It’ll also let through a hint of bass for definition. Underneath this layer of clarity, the distorted part can be made pretty distorted, but it’ll stay free of aliasing even up into the high frequencies, because those frequencies aren’t actually getting applied to the distortion, and the distortion output’s also being smoothed after the fact. So you’ve got a texture-thickener, an energy-adder, that retains a very analog quality because all of the overtones stay clear of aliasing WITHOUT oversampling. The raw sound is still a direct pass-through and that’s where the clarity comes from."
};
constexpr std::string_view k_tags{
    "saturation"
};

template <typename T>
class UnBox final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double aL[5];
    double bL[5];
    double cL[11];
    double aR[5];
    double bR[5];
    double cR[11];
    double e[5];
    double f[11];
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleAR;
    double iirSampleBR;
    float A;
    float B;
    float C;

  public:
    UnBox()
    {
        A = 0.5;
        B = 0.0;
        C = 0.5;
        for (int count = 0; count < 5; count++) {
            aL[count] = 0.0;
            bL[count] = 0.0;
            aR[count] = 0.0;
            bR[count] = 0.0;
            e[count] = 0.0;
        }
        for (int count = 0; count < 11; count++) {
            cL[count] = 0.0;
            cR[count] = 0.0;
            f[count] = 0.0;
        }
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
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
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.5;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.5;
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
            return "input";
            break;
        kParamB:
            return "unbox";
            break;
        kParamC:
            return "output";
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
            return "Input";
            break;
        kParamB:
            return "UnBox";
            break;
        kParamC:
            return "Output";
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
        overallscale *= getSampleRate();
        double input = A * 2.0;
        double unbox = B + 1.0;
        unbox *= unbox; // let's get some more gain into this
        double iirAmount = (unbox * 0.00052) / overallscale;
        double output = C * 2.0;
        double treble = unbox; // averaging taps 1-4
        double gain = treble;
        if (gain > 1.0) {
            e[0] = 1.0;
            gain -= 1.0;
        }
        else {
            e[0] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            e[1] = 1.0;
            gain -= 1.0;
        }
        else {
            e[1] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            e[2] = 1.0;
            gain -= 1.0;
        }
        else {
            e[2] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            e[3] = 1.0;
            gain -= 1.0;
        }
        else {
            e[3] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            e[4] = 1.0;
            gain -= 1.0;
        }
        else {
            e[4] = gain;
            gain = 0.0;
        }
        // there, now we have a neat little moving average with remainders
        if (treble < 1.0) {
            treble = 1.0;
        }
        e[0] /= treble;
        e[1] /= treble;
        e[2] /= treble;
        e[3] /= treble;
        e[4] /= treble;
        // and now it's neatly scaled, too
        treble = unbox * 2.0; // averaging taps 1-8
        gain = treble;
        if (gain > 1.0) {
            f[0] = 1.0;
            gain -= 1.0;
        }
        else {
            f[0] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[1] = 1.0;
            gain -= 1.0;
        }
        else {
            f[1] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[2] = 1.0;
            gain -= 1.0;
        }
        else {
            f[2] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[3] = 1.0;
            gain -= 1.0;
        }
        else {
            f[3] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[4] = 1.0;
            gain -= 1.0;
        }
        else {
            f[4] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[5] = 1.0;
            gain -= 1.0;
        }
        else {
            f[5] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[6] = 1.0;
            gain -= 1.0;
        }
        else {
            f[6] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[7] = 1.0;
            gain -= 1.0;
        }
        else {
            f[7] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[8] = 1.0;
            gain -= 1.0;
        }
        else {
            f[8] = gain;
            gain = 0.0;
        }
        if (gain > 1.0) {
            f[9] = 1.0;
            gain -= 1.0;
        }
        else {
            f[9] = gain;
            gain = 0.0;
        }
        // there, now we have a neat little moving average with remainders
        if (treble < 1.0) {
            treble = 1.0;
        }
        f[0] /= treble;
        f[1] /= treble;
        f[2] /= treble;
        f[3] /= treble;
        f[4] /= treble;
        f[5] /= treble;
        f[6] /= treble;
        f[7] /= treble;
        f[8] /= treble;
        f[9] /= treble;
        // and now it's neatly scaled, too
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (input != 1.0) {
                inputSampleL *= input;
                inputSampleR *= input;
            }
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            aL[4] = aL[3];
            aL[3] = aL[2];
            aL[2] = aL[1];
            aL[1] = aL[0];
            aL[0] = inputSampleL;
            inputSampleL *= e[0];
            inputSampleL += (aL[1] * e[1]);
            inputSampleL += (aL[2] * e[2]);
            inputSampleL += (aL[3] * e[3]);
            inputSampleL += (aL[4] * e[4]);
            // this is now an average of inputSampleL
            aR[4] = aR[3];
            aR[3] = aR[2];
            aR[2] = aR[1];
            aR[1] = aR[0];
            aR[0] = inputSampleR;
            inputSampleR *= e[0];
            inputSampleR += (aR[1] * e[1]);
            inputSampleR += (aR[2] * e[2]);
            inputSampleR += (aR[3] * e[3]);
            inputSampleR += (aR[4] * e[4]);
            // this is now an average of inputSampleR
            bL[4] = bL[3];
            bL[3] = bL[2];
            bL[2] = bL[1];
            bL[1] = bL[0];
            bL[0] = inputSampleL;
            inputSampleL *= e[0];
            inputSampleL += (bL[1] * e[1]);
            inputSampleL += (bL[2] * e[2]);
            inputSampleL += (bL[3] * e[3]);
            inputSampleL += (bL[4] * e[4]);
            // this is now an average of an average of inputSampleL. Two poles
            bR[4] = bR[3];
            bR[3] = bR[2];
            bR[2] = bR[1];
            bR[1] = bR[0];
            bR[0] = inputSampleR;
            inputSampleR *= e[0];
            inputSampleR += (bR[1] * e[1]);
            inputSampleR += (bR[2] * e[2]);
            inputSampleR += (bR[3] * e[3]);
            inputSampleR += (bR[4] * e[4]);
            // this is now an average of an average of inputSampleR. Two poles
            inputSampleL *= unbox;
            inputSampleR *= unbox;
            // clip to 1.2533141373155 to reach maximum output
            if (inputSampleL > 1.2533141373155) {
                inputSampleL = 1.2533141373155;
            }
            if (inputSampleL < -1.2533141373155) {
                inputSampleL = -1.2533141373155;
            }
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            if (inputSampleR > 1.2533141373155) {
                inputSampleR = 1.2533141373155;
            }
            if (inputSampleR < -1.2533141373155) {
                inputSampleR = -1.2533141373155;
            }
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            inputSampleL /= unbox;
            inputSampleR /= unbox;
            // now we have a distorted inputSample at the correct volume relative to drySample
            double accumulatorSampleL = (drySampleL - inputSampleL);
            cL[9] = cL[8];
            cL[8] = cL[7];
            cL[7] = cL[6];
            cL[6] = cL[5];
            cL[5] = cL[4];
            cL[4] = cL[3];
            cL[3] = cL[2];
            cL[2] = cL[1];
            cL[1] = cL[0];
            cL[0] = accumulatorSampleL;
            accumulatorSampleL *= f[0];
            accumulatorSampleL += (cL[1] * f[1]);
            accumulatorSampleL += (cL[2] * f[2]);
            accumulatorSampleL += (cL[3] * f[3]);
            accumulatorSampleL += (cL[4] * f[4]);
            accumulatorSampleL += (cL[5] * f[5]);
            accumulatorSampleL += (cL[6] * f[6]);
            accumulatorSampleL += (cL[7] * f[7]);
            accumulatorSampleL += (cL[8] * f[8]);
            accumulatorSampleL += (cL[9] * f[9]);
            // this is now an average of all the recent variances from dry
            double accumulatorSampleR = (drySampleR - inputSampleR);
            cR[9] = cR[8];
            cR[8] = cR[7];
            cR[7] = cR[6];
            cR[6] = cR[5];
            cR[5] = cR[4];
            cR[4] = cR[3];
            cR[3] = cR[2];
            cR[2] = cR[1];
            cR[1] = cR[0];
            cR[0] = accumulatorSampleR;
            accumulatorSampleR *= f[0];
            accumulatorSampleR += (cR[1] * f[1]);
            accumulatorSampleR += (cR[2] * f[2]);
            accumulatorSampleR += (cR[3] * f[3]);
            accumulatorSampleR += (cR[4] * f[4]);
            accumulatorSampleR += (cR[5] * f[5]);
            accumulatorSampleR += (cR[6] * f[6]);
            accumulatorSampleR += (cR[7] * f[7]);
            accumulatorSampleR += (cR[8] * f[8]);
            accumulatorSampleR += (cR[9] * f[9]);
            // this is now an average of all the recent variances from dry
            iirSampleAL = (iirSampleAL * (1 - iirAmount)) + (accumulatorSampleL * iirAmount);
            accumulatorSampleL -= iirSampleAL;
            // two poles of IIR
            iirSampleAR = (iirSampleAR * (1 - iirAmount)) + (accumulatorSampleR * iirAmount);
            accumulatorSampleR -= iirSampleAR;
            // two poles of IIR
            iirSampleBL = (iirSampleBL * (1 - iirAmount)) + (accumulatorSampleL * iirAmount);
            accumulatorSampleL -= iirSampleBL;
            // highpass section
            iirSampleBR = (iirSampleBR * (1 - iirAmount)) + (accumulatorSampleR * iirAmount);
            accumulatorSampleR -= iirSampleBR;
            // highpass section
            // this is now a highpassed average of all the recent variances from dry
            inputSampleL = drySampleL - accumulatorSampleL;
            inputSampleR = drySampleR - accumulatorSampleR;
            // we apply it as one operation, to get the result.
            if (output != 1.0) {
                inputSampleL *= output;
                inputSampleR *= output;
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
} // namespace airwindohhs::unbox
