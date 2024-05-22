#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class DeEss final : public Effect<T>
{
    std::string m_name{ "DeEss" };

    double s1L;
    double s2L;
    double s3L;
    double s4L;
    double s5L;
    double s6L;
    double s7L;
    double m1L;
    double m2L;
    double m3L;
    double m4L;
    double m5L;
    double m6L;
    double c1L;
    double c2L;
    double c3L;
    double c4L;
    double c5L;
    double ratioAL;
    double ratioBL;
    double iirSampleAL;
    double iirSampleBL;
    double s1R;
    double s2R;
    double s3R;
    double s4R;
    double s5R;
    double s6R;
    double s7R;
    double m1R;
    double m2R;
    double m3R;
    double m4R;
    double m5R;
    double m6R;
    double c1R;
    double c2R;
    double c3R;
    double c4R;
    double c5R;
    double ratioAR;
    double ratioBR;
    double iirSampleAR;
    double iirSampleBR;
    bool flip;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;

    enum params
    {
        kParamA = 0,
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3

    };

  public:
    DeEss()
    {
        A = 0.0;
        B = 0.5; //-48.0 to 0.0
        C = 0.5;
        s1L = s2L = s3L = s4L = s5L = s6L = s7L = 0.0;
        m1L = m2L = m3L = m4L = m5L = m6L = 0.0;
        c1L = c2L = c3L = c4L = c5L = 0.0;
        ratioAL = ratioBL = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        s1R = s2R = s3R = s4R = s5R = s6R = s7R = 0.0;
        m1R = m2R = m3R = m4R = m5R = m6R = 0.0;
        c1R = c2R = c3R = c4R = c5R = 0.0;
        ratioAR = ratioBR = 1.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        flip = false;
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

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Intense";
            case kParamB: return "Max DS";
            case kParamC: return "Freq";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A);
            case kParamB: return std::to_string((B - 1.0) * 48.0);
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
        double intensity = pow(A, 5) * (8192 / overallscale);
        double maxdess = 1.0 / pow(10.0, ((B - 1.0) * 48.0) / 20);
        double iirAmount = pow(C, 2) / overallscale;
        double offset;
        double sense;
        double recovery;
        double attackspeed;
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            static int noisesourceL = 0;
            static int noisesourceR = 850010;
            int residue;
            double applyresidue;
            noisesourceL = noisesourceL % 1700021;
            noisesourceL++;
            residue = noisesourceL * noisesourceL;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleL += applyresidue;
            if (inputSampleL < 1.2e-38 && -inputSampleL < 1.2e-38) {
                inputSampleL -= applyresidue;
            }
            noisesourceR = noisesourceR % 1700021;
            noisesourceR++;
            residue = noisesourceR * noisesourceR;
            residue = residue % 170003;
            residue *= residue;
            residue = residue % 17011;
            residue *= residue;
            residue = residue % 1709;
            residue *= residue;
            residue = residue % 173;
            residue *= residue;
            residue = residue % 17;
            applyresidue = residue;
            applyresidue *= 0.00000001;
            applyresidue *= 0.00000001;
            inputSampleR += applyresidue;
            if (inputSampleR < 1.2e-38 && -inputSampleR < 1.2e-38) {
                inputSampleR -= applyresidue;
            }
            // for live air, we always apply the dither noise. Then, if our result is
            // effectively digital black, we'll subtract it aDeEss. We want a 'air' hiss
            s3L = s2L;
            s2L = s1L;
            s1L = inputSampleL;
            m1L = (s1L - s2L) * ((s1L - s2L) / 1.3);
            m2L = (s2L - s3L) * ((s1L - s2L) / 1.3);
            sense = fabs((m1L - m2L) * ((m1L - m2L) / 1.3));
            // this will be 0 for smooth, high for SSS
            attackspeed = 7.0 + (sense * 1024);
            // this does not vary with intensity, but it does react to onset transients
            sense = 1.0 + (intensity * intensity * sense);
            if (sense > intensity) {
                sense = intensity;
            }
            // this will be 1 for smooth, 'intensity' for SSS
            recovery = 1.0 + (0.01 / sense);
            // this will be 1.1 for smooth, 1.0000000...1 for SSS
            offset = 1.0 - fabs(inputSampleL);
            if (flip) {
                iirSampleAL = (iirSampleAL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                if (ratioAL < sense)
                {
                    ratioAL = ((ratioAL * attackspeed) + sense) / (attackspeed + 1.0);
                }
                else
                {
                    ratioAL = 1.0 + ((ratioAL - 1.0) / recovery);
                }
                // returny to 1/1 code
                if (ratioAL > maxdess) {
                    ratioAL = maxdess;
                }
                inputSampleL = iirSampleAL + ((inputSampleL - iirSampleAL) / ratioAL);
            }
            else {
                iirSampleBL = (iirSampleBL * (1 - (offset * iirAmount))) + (inputSampleL * (offset * iirAmount));
                if (ratioBL < sense)
                {
                    ratioBL = ((ratioBL * attackspeed) + sense) / (attackspeed + 1.0);
                }
                else
                {
                    ratioBL = 1.0 + ((ratioBL - 1.0) / recovery);
                }
                // returny to 1/1 code
                if (ratioBL > maxdess) {
                    ratioBL = maxdess;
                }
                inputSampleL = iirSampleBL + ((inputSampleL - iirSampleBL) / ratioBL);
            } // have the ratio chase Sense
            s3R = s2R;
            s2R = s1R;
            s1R = inputSampleR;
            m1R = (s1R - s2R) * ((s1R - s2R) / 1.3);
            m2R = (s2R - s3R) * ((s1R - s2R) / 1.3);
            sense = fabs((m1R - m2R) * ((m1R - m2R) / 1.3));
            // this will be 0 for smooth, high for SSS
            attackspeed = 7.0 + (sense * 1024);
            // this does not vary with intensity, but it does react to onset transients
            sense = 1.0 + (intensity * intensity * sense);
            if (sense > intensity) {
                sense = intensity;
            }
            // this will be 1 for smooth, 'intensity' for SSS
            recovery = 1.0 + (0.01 / sense);
            // this will be 1.1 for smooth, 1.0000000...1 for SSS
            offset = 1.0 - fabs(inputSampleR);
            if (flip) {
                iirSampleAR = (iirSampleAR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                if (ratioAR < sense)
                {
                    ratioAR = ((ratioAR * attackspeed) + sense) / (attackspeed + 1.0);
                }
                else
                {
                    ratioAR = 1.0 + ((ratioAR - 1.0) / recovery);
                }
                // returny to 1/1 code
                if (ratioAR > maxdess) {
                    ratioAR = maxdess;
                }
                inputSampleR = iirSampleAR + ((inputSampleR - iirSampleAR) / ratioAR);
            }
            else {
                iirSampleBR = (iirSampleBR * (1 - (offset * iirAmount))) + (inputSampleR * (offset * iirAmount));
                if (ratioBR < sense)
                {
                    ratioBR = ((ratioBR * attackspeed) + sense) / (attackspeed + 1.0);
                }
                else
                {
                    ratioBR = 1.0 + ((ratioBR - 1.0) / recovery);
                }
                // returny to 1/1 code
                if (ratioBR > maxdess) {
                    ratioBR = maxdess;
                }
                inputSampleR = iirSampleBR + ((inputSampleR - iirSampleBR) / ratioBR);
            } // have the ratio chase Sense
            flip = !flip;
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
} // namespace airwindohhs
