#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::studiotan {

constexpr std::string_view k_name{ "StudioTan" };
constexpr std::string_view k_short_description{
    "StudioTan is all the 'non-dither' dithers, up to date and convenient."
};
constexpr std::string_view k_long_description{
    "If you were curious about the new work in quantization I’ve been doing… where I’ve put out a series of experimental plugins with names like Dither Me Timbers and RawGlitters and then said they weren’t dithers, or if you tried stuff like Dither Me Timbers and then found your limiter set to 0dB was now giving you overs, or if it just didn’t make sense at the time…This is for you.StudioTan is the sum total of all I’ve been doing with ‘dither’ that isn’t dither, for the last two years. It’s got three algorithms, StudioTan, Dither Me Timbers, and Not Just Another Dither. Each one is brought up to date, optimized, voiced to do what it does best. The first two begin with the use of quantization to apply EQ at ‘noise floor’ levels and the third applies Benford Realness statistics at noise floor levels, and they all use noise shaping to refine that behavior and get a specific tonal result. Not one of these have been quite available before, even from previous versions of Dither Me Timbers or Not Just Another Dither: it’s the latest finetunings of the algorithms. None of them can produce signal outputs beyond -1 to 1 so they can’t produce overs (if you’re ‘mastering’ so hot you get intersample overs, firstly that’s not a great idea and secondly StudioTan can’t help you there). All three come with 24 and 16 bit options right there in the plugin, as experimenter time is over and this is the final form, requiring no more fussing with plugins.The effect of each is more pronounced at 16 bit, and that’ll give you a sense of what is being subliminally imparted at the far subtler 24 bit level.StudioTan is like the posh, glossy, high class output. It sounds kind of like expensive studios and tape, brings up micro-detail but suppresses harshness. It’s slick and makes things sound more flawless and possibly more boring, but satisfying.Dither Me Timbers is like the spatially enhanced, electrically charged output. It makes sonic events spark out of a darker, spacious background, and gives a little energy to transients and attacks. It makes things sound more human and attitude-laden, so it’s more dramatic and attention-getting. It’s got different noise shaping behavior and voicing but in basic character it’s the opposite of StudioTan, and can be approached from that direction.Not Just Another Dither is like the holographic, hi-fi output. It’s a bit darker than previous Not Just Another Dithers and its purpose is to have detail go down to a digital noise floor that’s a balance of all types of artifact (since you’re always going to have a noise floor of something, no matter what you do) without changing character in any way. Since this perceived character is so balanced (this time, not even airy hiss as a ‘identifiable’ floor change) the new Not Just Another Dither is the best choice to seem like infinite resolution without even a noise floor being there. Instead of making ‘a sound’ like the previous two, it’s the one that sounds completely open and unaffected.None of these give sensible results with test tones: you can’t take a sine and enhance high frequency details, so you get purely garbage data. Don’t use Airwindows non-dither dithers for scientific processing or your rocket ships will blow up on the launch pad ;) these are literally tone controls in two cases and an obscure data handling technique for the third, and they are re-voiced for 2019 to best deliver their sonic mojo. You can do test tone stuff to satisfy yourself they aren’t placebo effect: they really do the noise-floor-EQing or Benford Realness stuff, and the source code is open. But for evaluating how they really work, the proof is in the listening.What they do cannot be done any other way, and they’re the final artistic polish on sonic creations done in or outside the DAW. Use them on mixes, or when mastering raw analog captures to lower word lengths for CD. They are ‘final dither’ in function if not strict definition, the way to crystallize high-res audio data into an output file that retains all the magic you intended, whatever flavor of magic that’s meant to be. The three categories of tone color ought to cover your bases there.I know this is all I’ll be using from now on :)(followup: it absolutely was not. I continued to use NJAD in Monitoring until I wrote Dark, have used that some including in Monitoring2 and then came up with a new dither based on Ten Nines into Dark, which is in the original output buss of Console8)"
};
constexpr std::string_view k_tags{
    "dithers"
};

template <typename T>
class StudioTan final : public Effect<T>
{
    double bynL[13];
    double noiseShapingL;
    double lastSampleL;
    double lastSample2L;
    double bynR[13];
    double noiseShapingR;
    double lastSampleR;
    double lastSample2R;
    float A;

  public:
    StudioTan()
    {
        A = 0.0;
        bynL[0] = 1000.0;
        bynL[1] = 301.0;
        bynL[2] = 176.0;
        bynL[3] = 125.0;
        bynL[4] = 97.0;
        bynL[5] = 79.0;
        bynL[6] = 67.0;
        bynL[7] = 58.0;
        bynL[8] = 51.0;
        bynL[9] = 46.0;
        bynL[10] = 1000.0;
        noiseShapingL = 0.0;
        lastSampleL = 0.0;
        lastSample2L = 0.0;
        bynR[0] = 1000.0;
        bynR[1] = 301.0;
        bynR[2] = 176.0;
        bynR[3] = 125.0;
        bynR[4] = 97.0;
        bynR[5] = 79.0;
        bynR[6] = 67.0;
        bynR[7] = 58.0;
        bynR[8] = 51.0;
        bynR[9] = 46.0;
        bynR[10] = 1000.0;
        noiseShapingR = 0.0;
        lastSampleR = 0.0;
        lastSample2R = 0.0;
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
        kParamA:
            A = value;
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

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 0.0;
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
            return "quantzr";
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
            return "Quantzr";
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

        bool highres = true; // for 24 bit: false for 16 bit
        bool brightfloor = true; // for Studio Tan: false for Dither Me Timbers
        bool benford = true; // for Not Just Another Dither: false for newer two
        bool cutbins = false; // for NJAD: only attenuate bins if one gets very full
        switch ((VstInt32)(A * 5.999))
        {
            case 0: benford = false; break; // Studio Tan 24
            case 1:
                benford = false;
                brightfloor = false;
                break; // Dither Me Timbers 24
            case 2: break; // Not Just Another Dither 24
            case 3:
                benford = false;
                highres = false;
                break; // Studio Tan 16
            case 4:
                benford = false;
                brightfloor = false;
                highres = false;
                break; // Dither Me Timbers 16
            case 5: highres = false; break; // Not Just Another Dither 16
        }
        while (--sampleFrames >= 0)
        {
            double inputSampleL;
            double outputSampleL;
            double drySampleL;
            double inputSampleR;
            double outputSampleR;
            double drySampleR;
            if (highres) {
                inputSampleL = *in1 * 8388608.0;
                inputSampleR = *in2 * 8388608.0;
            }
            else {
                inputSampleL = *in1 * 32768.0;
                inputSampleR = *in2 * 32768.0;
            }
            // shared input stage
            if (benford) {
                // begin Not Just Another Dither
                drySampleL = inputSampleL;
                drySampleR = inputSampleR;
                inputSampleL -= noiseShapingL;
                inputSampleR -= noiseShapingR;
                cutbins = false;
                double benfordize; // we get to re-use this for each channel
                // begin left channel NJAD
                benfordize = floor(inputSampleL);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                int hotbinA = floor(benfordize);
                // hotbin becomes the Benford bin value for this number floored
                double totalA = 0;
                if ((hotbinA > 0) && (hotbinA < 10))
                {
                    bynL[hotbinA] += 1;
                    if (bynL[hotbinA] > 982) {
                        cutbins = true;
                    }
                    totalA += (301 - bynL[1]);
                    totalA += (176 - bynL[2]);
                    totalA += (125 - bynL[3]);
                    totalA += (97 - bynL[4]);
                    totalA += (79 - bynL[5]);
                    totalA += (67 - bynL[6]);
                    totalA += (58 - bynL[7]);
                    totalA += (51 - bynL[8]);
                    totalA += (46 - bynL[9]);
                    bynL[hotbinA] -= 1;
                }
                else {
                    hotbinA = 10;
                }
                // produce total number- smaller is closer to Benford real
                benfordize = ceil(inputSampleL);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                int hotbinB = floor(benfordize);
                // hotbin becomes the Benford bin value for this number ceiled
                double totalB = 0;
                if ((hotbinB > 0) && (hotbinB < 10))
                {
                    bynL[hotbinB] += 1;
                    if (bynL[hotbinB] > 982) {
                        cutbins = true;
                    }
                    totalB += (301 - bynL[1]);
                    totalB += (176 - bynL[2]);
                    totalB += (125 - bynL[3]);
                    totalB += (97 - bynL[4]);
                    totalB += (79 - bynL[5]);
                    totalB += (67 - bynL[6]);
                    totalB += (58 - bynL[7]);
                    totalB += (51 - bynL[8]);
                    totalB += (46 - bynL[9]);
                    bynL[hotbinB] -= 1;
                }
                else {
                    hotbinB = 10;
                }
                // produce total number- smaller is closer to Benford real
                if (totalA < totalB)
                {
                    bynL[hotbinA] += 1;
                    outputSampleL = floor(inputSampleL);
                }
                else
                {
                    bynL[hotbinB] += 1;
                    outputSampleL = floor(inputSampleL + 1);
                }
                // assign the relevant one to the delay line
                // and floor/ceil signal accordingly
                if (cutbins) {
                    bynL[1] *= 0.99;
                    bynL[2] *= 0.99;
                    bynL[3] *= 0.99;
                    bynL[4] *= 0.99;
                    bynL[5] *= 0.99;
                    bynL[6] *= 0.99;
                    bynL[7] *= 0.99;
                    bynL[8] *= 0.99;
                    bynL[9] *= 0.99;
                    bynL[10] *= 0.99; // catchall for garbage data
                }
                noiseShapingL += outputSampleL - drySampleL;
                // end left channel NJAD
                // begin right channel NJAD
                cutbins = false;
                benfordize = floor(inputSampleR);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                hotbinA = floor(benfordize);
                // hotbin becomes the Benford bin value for this number floored
                totalA = 0;
                if ((hotbinA > 0) && (hotbinA < 10))
                {
                    bynR[hotbinA] += 1;
                    if (bynR[hotbinA] > 982) {
                        cutbins = true;
                    }
                    totalA += (301 - bynR[1]);
                    totalA += (176 - bynR[2]);
                    totalA += (125 - bynR[3]);
                    totalA += (97 - bynR[4]);
                    totalA += (79 - bynR[5]);
                    totalA += (67 - bynR[6]);
                    totalA += (58 - bynR[7]);
                    totalA += (51 - bynR[8]);
                    totalA += (46 - bynR[9]);
                    bynR[hotbinA] -= 1;
                }
                else {
                    hotbinA = 10;
                }
                // produce total number- smaller is closer to Benford real
                benfordize = ceil(inputSampleR);
                while (benfordize >= 1.0) {
                    benfordize /= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                if (benfordize < 1.0) {
                    benfordize *= 10;
                }
                hotbinB = floor(benfordize);
                // hotbin becomes the Benford bin value for this number ceiled
                totalB = 0;
                if ((hotbinB > 0) && (hotbinB < 10))
                {
                    bynR[hotbinB] += 1;
                    if (bynR[hotbinB] > 982) {
                        cutbins = true;
                    }
                    totalB += (301 - bynR[1]);
                    totalB += (176 - bynR[2]);
                    totalB += (125 - bynR[3]);
                    totalB += (97 - bynR[4]);
                    totalB += (79 - bynR[5]);
                    totalB += (67 - bynR[6]);
                    totalB += (58 - bynR[7]);
                    totalB += (51 - bynR[8]);
                    totalB += (46 - bynR[9]);
                    bynR[hotbinB] -= 1;
                }
                else {
                    hotbinB = 10;
                }
                // produce total number- smaller is closer to Benford real
                if (totalA < totalB)
                {
                    bynR[hotbinA] += 1;
                    outputSampleR = floor(inputSampleR);
                }
                else
                {
                    bynR[hotbinB] += 1;
                    outputSampleR = floor(inputSampleR + 1);
                }
                // assign the relevant one to the delay line
                // and floor/ceil signal accordingly
                if (cutbins) {
                    bynR[1] *= 0.99;
                    bynR[2] *= 0.99;
                    bynR[3] *= 0.99;
                    bynR[4] *= 0.99;
                    bynR[5] *= 0.99;
                    bynR[6] *= 0.99;
                    bynR[7] *= 0.99;
                    bynR[8] *= 0.99;
                    bynR[9] *= 0.99;
                    bynR[10] *= 0.99; // catchall for garbage data
                }
                noiseShapingR += outputSampleR - drySampleR;
                // end right channel NJAD
                // end Not Just Another Dither
            }
            else {
                // begin StudioTan or Dither Me Timbers
                if (brightfloor) {
                    lastSampleL -= (noiseShapingL * 0.8);
                    lastSampleR -= (noiseShapingR * 0.8);
                    if ((lastSampleL + lastSampleL) <= (inputSampleL + lastSample2L)) {
                        outputSampleL = floor(lastSampleL); // StudioTan
                    }
                    else {
                        outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
                    }
                    if ((lastSampleR + lastSampleR) <= (inputSampleR + lastSample2R)) {
                        outputSampleR = floor(lastSampleR); // StudioTan
                    }
                    else {
                        outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
                    }
                }
                else {
                    lastSampleL -= (noiseShapingL * 0.11);
                    lastSampleR -= (noiseShapingR * 0.11);
                    if ((lastSampleL + lastSampleL) >= (inputSampleL + lastSample2L)) {
                        outputSampleL = floor(lastSampleL); // DitherMeTimbers
                    }
                    else {
                        outputSampleL = floor(lastSampleL + 1.0); // round down or up based on whether it softens treble angles
                    }
                    if ((lastSampleR + lastSampleR) >= (inputSampleR + lastSample2R)) {
                        outputSampleR = floor(lastSampleR); // DitherMeTimbers
                    }
                    else {
                        outputSampleR = floor(lastSampleR + 1.0); // round down or up based on whether it softens treble angles
                    }
                }
                noiseShapingL += outputSampleL;
                noiseShapingL -= lastSampleL; // apply noise shaping
                lastSample2L = lastSampleL;
                lastSampleL = inputSampleL; // we retain three samples in a row
                noiseShapingR += outputSampleR;
                noiseShapingR -= lastSampleR; // apply noise shaping
                lastSample2R = lastSampleR;
                lastSampleR = inputSampleR; // we retain three samples in a row
                // end StudioTan or Dither Me Timbers
            }
            // shared output stage
            double noiseSuppressL = fabs(inputSampleL);
            if (noiseShapingL > noiseSuppressL) {
                noiseShapingL = noiseSuppressL;
            }
            if (noiseShapingL < -noiseSuppressL) {
                noiseShapingL = -noiseSuppressL;
            }
            double noiseSuppressR = fabs(inputSampleR);
            if (noiseShapingR > noiseSuppressR) {
                noiseShapingR = noiseSuppressR;
            }
            if (noiseShapingR < -noiseSuppressR) {
                noiseShapingR = -noiseSuppressR;
            }
            double ironBarL;
            double ironBarR;
            if (highres) {
                ironBarL = outputSampleL / 8388608.0;
                ironBarR = outputSampleR / 8388608.0;
            }
            else {
                ironBarL = outputSampleL / 32768.0;
                ironBarR = outputSampleR / 32768.0;
            }
            if (ironBarL > 1.0) {
                ironBarL = 1.0;
            }
            if (ironBarL < -1.0) {
                ironBarL = -1.0;
            }
            if (ironBarR > 1.0) {
                ironBarR = 1.0;
            }
            if (ironBarR < -1.0) {
                ironBarR = -1.0;
            }
            *out1 = ironBarL;
            *out2 = ironBarR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::studiotan
