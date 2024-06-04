#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::orbitkick {

constexpr std::string_view k_name{ "OrbitKick" };
constexpr std::string_view k_short_description{
    "OrbitKick is a bass generator and kick drum reinforcer."
};
constexpr std::string_view k_long_description{
    "If you've seen modern recording, you've probably seen a little drumlike object in front of bass drums. This device is a 'SubKick'. It's basically an NS10 woofer hung in front of the kick, so it can be knocked around by subsonic bass waves, and transfer that to electricity for recording. It might or might not be in an actual drum shell, needs to be recorded with an actual preamp (these can have very high output though!) and acts as just another mic, but one that captures the very deepest bass in conjunction with your full drumkit and all your other mics.But what if you could do that… without the pre? And also without the little drum shell, and without the NS10 speaker, and for that matter without the drumkit. Just 'deep bass subs' like a kick drum. On anything.Enter OrbitKick. This is basically a little physical simulation, like a planet orbiting a sun. When your input sound hits, it kicks this thing into orbit and it just spins, gradually slowing down as its orbit gets bigger and bigger. It's a little like a second-order sine wave, in that it's one of those Airwindows things that can get out of hand, which is what the controls are for.Drop is how fast the note will drop, from 'not at all' to real quick. Shape is the same, but makes it drop quicker to start off, so it's your taper: it gives you punchier attacks, rather than 'modular guy beeeoooo' kicks that have no impact. Start controls how high up your attack goes, in conjunction with Thresh, which is the threshold at which the note is kicked off… and Finish is where the note cuts off (set super low, you can get clicky releases).That's all. If you want shorter kicks, make it drop faster, start lower, or finish higher. These controls do it all. That includes weird nasty effects where the note is triggered in a scruffy, inconsistent way. OrbitKick does NOT sound like a sample. It's like a living bass thing, able to put the lowest of bottom octaves on whatever percussive thing you like, or add a thump or 'pewww!' laser sound to any other thing so long as it has a distinct attack. And without a distinct attack it will still work but it'll make an unpleasant noise. And if you can only listen on a laptop or cellphone you may never hear what it does at all…This one goes out to DnB friends of mine in London :) rarely do I get a plugin that will wreak so much mayhem on really, really big sound systems. Be careful out there, or don't :)"
};
constexpr std::string_view k_tags{
    "bass"
};

template <typename T>
class OrbitKick final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double orbit;
    double position;
    double speed;
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;

  public:
    OrbitKick()
    {
        A = 0.5;
        B = 0.5;
        C = 0.5;
        D = 0.5;
        E = 0.5;
        F = 1.0;
        orbit = 0.0;
        position = 0.0;
        speed = 0.0;
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
        kParamE = 4,
        kParamF = 5,
        kNumParameters = 6

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

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
            case kParamD: return D;
            case kParamE: return E;
            case kParamF: return F;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;
            case kParamC: return 0.5;
            case kParamD: return 0.5;
            case kParamE: return 0.5;
            case kParamF: return 1.0;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drop";
            case kParamB: return "shape";
            case kParamC: return "start";
            case kParamD: return "finish";
            case kParamE: return "thresh";
            case kParamF: return "drywet";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drop";
            case kParamB: return "Shape";
            case kParamC: return "Start";
            case kParamD: return "Finish";
            case kParamE: return "Thresh";
            case kParamF: return "Dry/Wet";

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
            case kParamD: return std::to_string(D);
            case kParamE: return std::to_string(E);
            case kParamF: return std::to_string(F);

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
            case kParamD: return "";
            case kParamE: return "";
            case kParamF: return "";
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
        double drop = 1.0 + (A * (0.001 / overallscale)); // more is briefer bass
        double zone = B * 0.01; // the max exponentiality of the falloff
        double start = C; // higher attack
        double envelope = 9.0 - ((1.0 - pow(1.0 - D, 2)) * 4.0); // higher is allowing more subs before gate
        envelope *= ((start * 0.4) + 0.6);
        double threshold = pow(E, 3); // trigger threshold
        double wet = F * 2.0;
        double dry = 2.0 - wet;
        if (wet > 1.0) {
            wet = 1.0;
        }
        if (dry > 1.0) {
            dry = 1.0;
        }
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
            if ((inputSampleL > speed * start * 2.0) && (inputSampleL > threshold)) {
                speed = inputSampleL * start;
            }
            if ((inputSampleR > speed * start * 2.0) && (inputSampleR > threshold)) {
                speed = inputSampleR * start;
            }
            position += (speed * start);
            speed /= (drop + (speed * zone * start));
            if (position > 31415.92653589793) {
                position -= 31415.92653589793;
            }
            orbit += (cos(position) * 0.001);
            orbit *= 0.998272;
            double applySpeed = cbrt(speed) * envelope;
            if (applySpeed < 1.0) {
                orbit *= applySpeed;
            }
            inputSampleL = inputSampleR = orbit * 2.0;
            inputSampleL = (inputSampleL * wet) + (drySampleL * dry);
            inputSampleR = (inputSampleR * wet) + (drySampleR * dry);
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
} // namespace airwindohhs::orbitkick
