#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::dubly3 {

constexpr std::string_view k_name{ "Dubly3" };
constexpr std::string_view k_short_description{
    "Dubly3 refines and transforms the Dubly sound."
};
constexpr std::string_view k_long_description{
    "So here's where the Dubly stuff suddenly gets more useful for a lot of people.This whole time, Dubly's been using a transfer function called uLaw encoding to take the place of 'compression', because it gets to roughly the same 2:1 ratio that the famous noise reduction system liked to use. And this is all well and good… but transfer function modifications without time constants aren't compression. They're distortion. And so all the versions of Dubly have used brightened but distorted audio to encode and decode with. And this is fine for a number of reasons (one of them being, Dubly is a Spinal Tap joke and not an attempt to pirate still very defended IP) up until ToTape7 came out, and people started trying to use it for all manner of things.Dubly and Dubly2 are the right basic sort of thing, but gritty and alias-y, and I vowed I'd work on it in order to develop a ToTape8 that performed better, ideally with a heck of a lot less aliasing but still zero latency and with the immediacy Airwindows ToTape has been known for. And so, towards that goal, Dubly3 is here.What's changed? A bunch of things. Notably, Dubly3 uses real compression: but not in any sense a clone of other gear. Instead, it's a new trick I might look into further: it's applying a compression based not on the input signal, but the uLaw version of the input signal, so the 'DNA' of original Dubly is very much still in there. But the time constants totally alter the texture of how Dubly3 behaves. It acts a lot more like a real compressor, as in a sense it is one, just one that's following an unusual form of signal (much like having the compression try to follow a tube-drive style control signal).That applies to both encode and decode. On top of that, Dubly3 does adopt the classic trick of clipping the compressed highs slightly, which also helps it sound authentic. But beyond that, Dubly3 reinvents the controls in a way that I think will be very helpful, and in a way that lets the upcoming ToTape8 add some asked-for features.Instead of direct control over amounts of encode, decode, and their crossover frequencies, Dubly3 boils it down to input and output gains (labeled as such, not 'tape drive') and tilt and shape controls, and that's how you dial it in. What's tilt? At 0.5 tilt is 'neutral', but as you boost it, you're adding encode and cutting decode. The similarity to a tilt EQ is obvious. More tilt means more bright, and if you cut it below 0.5 you're leaving decode fully engaged but reducing encode, causing it still to be the Dubly sound, but darker.Shape is even more interesting. Where Tilt balances the encode and decode levels against each other, Shape balances the crossover frequencies against each other… and lets you push them to unreasonable extremes, in case it gives you usefully weird sounds to play with. For real sounds you'll be keeping it a lot nearer 0.5 where it defaults to. What happens is this: if you increase Shape, you're encoding at lower frequencies, but decoding at higher. That means the brightness factor stays roughly the same, but the mids get more intense, more compressed, more lively. You'll hear it in the chug of guitars and in the energy of percussion, but it's not purely a treble hype: it's reshaping the whole midrange where Dubly crosses it over to treble. Then, when you cut Shape, you're encoding at higher frequencies but decoding more in lower frequencies… and since decoding is more subtractive, and since Dubly3 is a compressy thing, you're darkening and expanding the mids instead.Using these controls, you can use Dubly3 to start dialing in a number of classic record sounds, particular albums where perhaps the hardware noise reduction of the day had drifted and produced a distinctive sound. And then, once ToTape8 is done, you can apply it to the additional controls of ToTape, adding refinements in bias and using the classic ToTape algorithm to make a plugin you can hit like it was a real tape machine.Which real tape machine? ANY real tape machine, pretty much. If you can identify the sound (things like where the head bump sits, how overbiased or underbiased the sound is, whether it's a brighter or darker sound, more saturated and hyped in the mids or cleaner and more dark and studio-y… at that point it ought to be possible to decode the classic sounds and either mimic them, or learn from them and use the plugin to make the choices that work for YOUR mix and your sound.It starts with Dubly3, and once I've got ToTape8 done, you'll have everything ToTape7 has, except with this Dubly instead of the previous, and thanks to the use of two fewer tone controls, ToTape8 will also have an Input control at the top and an Output control at the bottom, to make the gain staging simpler for people to deal with. And it will still fit in Airwindows Consolidated. But for now, play with Dubly3 and see if that works better for ya than Dubly2 did. And Dubly2 is still supported and available because it's an entirely different sound and you should still have it in case you can use it.Next, a ToTape8, but in weeks rather than another couple years. And then… more work on ConsoleX :)"
};
constexpr std::string_view k_tags{
    "effects"
};

template <typename T>
class Dubly3 final : public Effect<T>
{
    float A;
    float B;
    float C;
    float D;
    double iirEncL;
    double iirDecL;
    double compEncL;
    double compDecL;
    double avgEncL;
    double avgDecL;
    double iirEncR;
    double iirDecR;
    double compEncR;
    double compDecR;
    double avgEncR;
    double avgDecR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Dubly3()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        iirEncL = 0.0;
        iirEncR = 0.0;
        iirDecL = 0.0;
        iirDecR = 0.0;
        compEncL = 1.0;
        compEncR = 1.0;
        compDecL = 1.0;
        compDecR = 1.0;
        avgEncL = 0.0;
        avgEncR = 0.0;
        avgDecL = 0.0;
        avgDecR = 0.0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5; break;
            case kParamB: return 0.5; break;
            case kParamC: return 0.5; break;
            case kParamD: return 0.5; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "input"; break;
            case kParamB: return "tilt"; break;
            case kParamC: return "shape"; break;
            case kParamD: return "output"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Input"; break;
            case kParamB: return "Tilt"; break;
            case kParamC: return "Shape"; break;
            case kParamD: return "Output"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
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
        switch (static_cast<params>(index))
        {
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
        double inputGain = pow(A * 2.0, 2.0);
        double dublyAmount = B * 2.0;
        double outlyAmount = (1.0 - B) * -2.0;
        if (outlyAmount < -1.0) {
            outlyAmount = -1.0;
        }
        double iirEncFreq = (1.0 - C) / overallscale;
        double iirDecFreq = C / overallscale;
        double outputGain = D * 2.0;
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
            if (inputGain != 1.0) {
                inputSampleL *= inputGain;
                inputSampleR *= inputGain;
            }
            // Dubly encode
            iirEncL = (iirEncL * (1.0 - iirEncFreq)) + (inputSampleL * iirEncFreq);
            double highPart = ((inputSampleL - iirEncL) * 2.848);
            highPart += avgEncL;
            avgEncL = (inputSampleL - iirEncL) * 1.152;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            double dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compEncL = (compEncL * (1.0 - iirEncFreq)) + (dubly * iirEncFreq);
                inputSampleL += ((highPart * compEncL) * dublyAmount);
            } // end Dubly encode L
            iirEncR = (iirEncR * (1.0 - iirEncFreq)) + (inputSampleR * iirEncFreq);
            highPart = ((inputSampleR - iirEncR) * 2.848);
            highPart += avgEncR;
            avgEncR = (inputSampleR - iirEncR) * 1.152;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compEncR = (compEncR * (1.0 - iirEncFreq)) + (dubly * iirEncFreq);
                inputSampleR += ((highPart * compEncR) * dublyAmount);
            } // end Dubly encode R
            if (inputSampleL > 1.57079633) {
                inputSampleL = 1.57079633;
            }
            if (inputSampleL < -1.57079633) {
                inputSampleL = -1.57079633;
            }
            inputSampleL = sin(inputSampleL);
            if (inputSampleR > 1.57079633) {
                inputSampleR = 1.57079633;
            }
            if (inputSampleR < -1.57079633) {
                inputSampleR = -1.57079633;
            }
            inputSampleR = sin(inputSampleR);
            // Dubly decode
            iirDecL = (iirDecL * (1.0 - iirDecFreq)) + (inputSampleL * iirDecFreq);
            highPart = ((inputSampleL - iirDecL) * 2.628);
            highPart += avgDecL;
            avgDecL = (inputSampleL - iirDecL) * 1.372;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compDecL = (compDecL * (1.0 - iirDecFreq)) + (dubly * iirDecFreq);
                inputSampleL += ((highPart * compDecL) * outlyAmount);
            } // end Dubly decode L
            iirDecR = (iirDecR * (1.0 - iirDecFreq)) + (inputSampleR * iirDecFreq);
            highPart = ((inputSampleR - iirDecR) * 2.628);
            highPart += avgDecR;
            avgDecR = (inputSampleR - iirDecR) * 1.372;
            if (highPart > 1.0) {
                highPart = 1.0;
            }
            if (highPart < -1.0) {
                highPart = -1.0;
            }
            dubly = fabs(highPart);
            if (dubly > 0.0) {
                double adjust = log(1.0 + (255.0 * dubly)) / 2.40823996531;
                if (adjust > 0.0) {
                    dubly /= adjust;
                }
                compDecR = (compDecR * (1.0 - iirDecFreq)) + (dubly * iirDecFreq);
                inputSampleR += ((highPart * compDecR) * outlyAmount);
            } // end Dubly decode R
            if (outputGain != 1.0) {
                inputSampleL *= outputGain;
                inputSampleR *= outputGain;
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
} // namespace airwindohhs::dubly3
