#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::interstage {

constexpr std::string_view k_name{ "Interstage" };
constexpr std::string_view k_short_description{
    "Interstage is a subtle and sophisticated analogifier."
};
constexpr std::string_view k_long_description{
    "Interstage is one half of an experiment. I pitted my ability to use my weird techniques (interleaved IIR, slew limiting, etc) against my new biquad filtering techniques, in the battle of the bandpasses. The biquads could give me total DC rejection and total rejection near Nyquist. My more distinctly Airwindows tricks could give me new sorts of nonlinearity and tone. Which would win?This isn’t called ‘Biquadstage’, so that might be a bit of a giveaway.Interstage isn’t a loudenator. It doesn’t really clip (though it does go dark as you push the highs harder, in a way I’ve never done before). It doesn’t even preserve the output peaks of heavily limited material: it’ll reshape lows in such a way that the peaks might go up slightly as the deepest lows get rearranged, and it doesn’t really eliminate DC offset either. So what does it actually do?*deep breath*Two level total of three pole IIR highpass which subtracts a pre-averaged sample and slew limits (all right, clips) against not the direct signal but the initial stage IIR lowpassed reference point used as part of the highpass. Oh, also the average it uses isn’t the previous input sample, but the slew limited highpassed output.*crickets*No, I am NOT making that up. That’s literally what it does, you can see the code.What does it sound like? It sounds like running through an optimal analog stage. The lows are reshaped in a characteristic way for a capacitor-coupled circuit that still allows extended lows: this doesn’t suppress much if any extreme bass. It just massages it a bit. The highs run into active component electronic limits, but unlike other approaches (Channel for a bit of grit, Acceleration for ultra-clear) this is restricting treble slews based on the general amount of energy in the circuit. So it goes darker in a peculiarly analog-like way I’ve not done before, sounding still clear and trebly for most audio, but confining the craziest most digital-sounding treble swings into a zone that sounds like hardware. And this is without thousands of math operations of heavy processing: unlike overprocessed analog modeling, this one nails the ‘energy coming out of analog circuitry’ without blurring or thinning the audio at all. If you don’t need what it’s doing you’ll hear no change at all. Only when ‘excessively digital bass and treble’ show up to interfere, does Interstage kick in."
};
constexpr std::string_view k_tags{
    "subtlety"
};

template <typename T>
class Interstage final : public Effect<T>
{
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double lastSampleL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    bool flip;

  public:
    Interstage()
    {
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        lastSampleL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        lastSampleR = 0.0;
        fpdL = 1.0;
        while (fpdL < 16386) {
            fpdL = rand() * UINT32_MAX;
        }
        fpdR = 1.0;
        while (fpdR < 16386) {
            fpdR = rand() * UINT32_MAX;
        }
        flip = true;
        // this is reset: values being initialized only once. Startup values, whatever they are.
    }

    enum params
    {
        kNumParameters = 0
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {

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
        double firstStage = 0.381966011250105 / overallscale;
        double iirAmount = 0.00295 / overallscale;
        double threshold = 0.381966011250105;
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
            inputSampleL = (inputSampleL + lastSampleL) * 0.5;
            inputSampleR = (inputSampleR + lastSampleR) * 0.5; // start the lowpassing with an average
            if (flip) {
                iirSampleAL = (iirSampleAL * (1 - firstStage)) + (inputSampleL * firstStage);
                inputSampleL = iirSampleAL;
                iirSampleCL = (iirSampleCL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleCL;
                iirSampleEL = (iirSampleEL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleEL;
                inputSampleL = drySampleL - inputSampleL;
                // make highpass
                if (inputSampleL - iirSampleAL > threshold) {
                    inputSampleL = iirSampleAL + threshold;
                }
                if (inputSampleL - iirSampleAL < -threshold) {
                    inputSampleL = iirSampleAL - threshold;
                }
                // slew limit against lowpassed reference point
                iirSampleAR = (iirSampleAR * (1 - firstStage)) + (inputSampleR * firstStage);
                inputSampleR = iirSampleAR;
                iirSampleCR = (iirSampleCR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleCR;
                iirSampleER = (iirSampleER * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleER;
                inputSampleR = drySampleR - inputSampleR;
                // make highpass
                if (inputSampleR - iirSampleAR > threshold) {
                    inputSampleR = iirSampleAR + threshold;
                }
                if (inputSampleR - iirSampleAR < -threshold) {
                    inputSampleR = iirSampleAR - threshold;
                }
                // slew limit against lowpassed reference point
            }
            else {
                iirSampleBL = (iirSampleBL * (1 - firstStage)) + (inputSampleL * firstStage);
                inputSampleL = iirSampleBL;
                iirSampleDL = (iirSampleDL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleDL;
                iirSampleFL = (iirSampleFL * (1 - iirAmount)) + (inputSampleL * iirAmount);
                inputSampleL = iirSampleFL;
                inputSampleL = drySampleL - inputSampleL;
                // make highpass
                if (inputSampleL - iirSampleBL > threshold) {
                    inputSampleL = iirSampleBL + threshold;
                }
                if (inputSampleL - iirSampleBL < -threshold) {
                    inputSampleL = iirSampleBL - threshold;
                }
                // slew limit against lowpassed reference point
                iirSampleBR = (iirSampleBR * (1 - firstStage)) + (inputSampleR * firstStage);
                inputSampleR = iirSampleBR;
                iirSampleDR = (iirSampleDR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleDR;
                iirSampleFR = (iirSampleFR * (1 - iirAmount)) + (inputSampleR * iirAmount);
                inputSampleR = iirSampleFR;
                inputSampleR = drySampleR - inputSampleR;
                // make highpass
                if (inputSampleR - iirSampleBR > threshold) {
                    inputSampleR = iirSampleBR + threshold;
                }
                if (inputSampleR - iirSampleBR < -threshold) {
                    inputSampleR = iirSampleBR - threshold;
                }
                // slew limit against lowpassed reference point
            }
            flip = !flip;
            lastSampleL = inputSampleL;
            lastSampleR = inputSampleR;
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
} // namespace airwindohhs::interstage
