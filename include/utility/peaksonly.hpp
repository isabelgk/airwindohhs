#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class PeaksOnly final : public Effect<T>
{
    const std::string m_name{ "PeaksOnly" };

    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double aL[1503];
    double bL[1503];
    double cL[1503];
    double dL[1503];
    double aR[1503];
    double bR[1503];
    double cR[1503];
    double dR[1503];
    int ax, bx, cx, dx;

    enum params
    {
        kNumParameters = 0

    };

  public:
    PeaksOnly()
    {
        for (int count = 0; count < 1502; count++) {
            aL[count] = 0.0;
            bL[count] = 0.0;
            cL[count] = 0.0;
            dL[count] = 0.0;
            aR[count] = 0.0;
            bR[count] = 0.0;
            cR[count] = 0.0;
            dR[count] = 0.0;
        }
        ax = 1;
        bx = 1;
        cx = 1;
        dx = 1;
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

    std::string name() override { return m_name; }

    void set_parameter_value(int index, float value) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_label(int index) override
    {
        switch (static_cast<params>(index))
        {
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        int am = (int)149.0 * overallscale;
        int bm = (int)179.0 * overallscale;
        int cm = (int)191.0 * overallscale;
        int dm = (int)223.0 * overallscale; // these are 'good' primes, spacing out the allpasses
        int allpasstemp = 0;
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            allpasstemp = ax - 1;
            if (allpasstemp < 0 || allpasstemp > am) {
                allpasstemp = am;
            }
            inputSampleL -= aL[allpasstemp] * 0.5;
            inputSampleR -= aR[allpasstemp] * 0.5;
            aL[ax] = inputSampleL;
            aR[ax] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            ax--;
            if (ax < 0 || ax > am) {
                ax = am;
            }
            inputSampleL += (aL[ax]);
            inputSampleR += (aR[ax]);
            // a single Midiverb-style allpass
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            allpasstemp = bx - 1;
            if (allpasstemp < 0 || allpasstemp > bm) {
                allpasstemp = bm;
            }
            inputSampleL -= bL[allpasstemp] * 0.5;
            inputSampleR -= bR[allpasstemp] * 0.5;
            bL[bx] = inputSampleL;
            bR[bx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            bx--;
            if (bx < 0 || bx > bm) {
                bx = bm;
            }
            inputSampleL += (bL[bx]);
            inputSampleR += (bR[bx]);
            // a single Midiverb-style allpass
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            allpasstemp = cx - 1;
            if (allpasstemp < 0 || allpasstemp > cm) {
                allpasstemp = cm;
            }
            inputSampleL -= cL[allpasstemp] * 0.5;
            inputSampleR -= cR[allpasstemp] * 0.5;
            cL[cx] = inputSampleL;
            cR[cx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            cx--;
            if (cx < 0 || cx > cm) {
                cx = cm;
            }
            inputSampleL += (cL[cx]);
            inputSampleR += (cR[cx]);
            // a single Midiverb-style allpass
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            allpasstemp = dx - 1;
            if (allpasstemp < 0 || allpasstemp > dm) {
                allpasstemp = dm;
            }
            inputSampleL -= dL[allpasstemp] * 0.5;
            inputSampleR -= dR[allpasstemp] * 0.5;
            dL[dx] = inputSampleL;
            dR[dx] = inputSampleR;
            inputSampleL *= 0.5;
            inputSampleR *= 0.5;
            dx--;
            if (dx < 0 || dx > dm) {
                dx = dm;
            }
            inputSampleL += (dL[dx]);
            inputSampleR += (dR[dx]);
            // a single Midiverb-style allpass
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
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            inputSampleL *= 0.63679; // scale it to 0dB output at full blast
            inputSampleR *= 0.63679; // scale it to 0dB output at full blast
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
