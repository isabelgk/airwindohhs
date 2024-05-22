#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class OrbitKick final : public Effect<T>
{
    std::string m_name{ "OrbitKick" };

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

    constexpr std::string_view name()
    {
        return m_name;
    }

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

    constexpr std::string_view get_parameter_name(int index)
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
} // namespace airwindohhs
