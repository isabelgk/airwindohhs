#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class NaturalizeDither final : public Effect<T>
{
    const std::string m_name{ "NaturalizeDither" };

    double bynL[13];
    double bynR[13];
    uint32_t fpdL;
    uint32_t fpdR;

    enum params
    {
        kNumParameters = 0

    };

  public:
    NaturalizeDither()
    {
        bynL[0] = 1000;
        bynL[1] = 301;
        bynL[2] = 176;
        bynL[3] = 125;
        bynL[4] = 97;
        bynL[5] = 79;
        bynL[6] = 67;
        bynL[7] = 58;
        bynL[8] = 51;
        bynL[9] = 46;
        bynL[10] = 1000;
        bynR[0] = 1000;
        bynR[1] = 301;
        bynR[2] = 176;
        bynR[3] = 125;
        bynR[4] = 97;
        bynR[5] = 79;
        bynR[6] = 67;
        bynR[7] = 58;
        bynR[8] = 51;
        bynR[9] = 46;
        bynR[10] = 1000;
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

        double inputSampleL;
        double inputSampleR;
        double benfordize;
        int hotbinA;
        int hotbinB;
        double totalA;
        double totalB;
        while (--sampleFrames >= 0)
        {
            inputSampleL = *in1;
            inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            inputSampleL *= 8388608.0;
            inputSampleR *= 8388608.0;
            // 0-1 is now one bit, now we dither
            if (inputSampleL > 0) {
                inputSampleL += (0.3333333333);
            }
            if (inputSampleL < 0) {
                inputSampleL -= (0.3333333333);
            }
            inputSampleL += (double(fpdL) / UINT32_MAX) * 0.6666666666;
            if (inputSampleR > 0) {
                inputSampleR += (0.3333333333);
            }
            if (inputSampleR < 0) {
                inputSampleR -= (0.3333333333);
            }
            inputSampleR += (double(fpdR) / UINT32_MAX) * 0.6666666666;
            // begin L
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
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynL[hotbinA] += 1;
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
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynL[hotbinB] += 1;
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
                inputSampleL = floor(inputSampleL);
            }
            else
            {
                bynL[hotbinB] += 1;
                inputSampleL = ceil(inputSampleL);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            totalA = bynL[1] + bynL[2] + bynL[3] + bynL[4] + bynL[5] + bynL[6] + bynL[7] + bynL[8] + bynL[9];
            totalA /= 1000;
            totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
            bynL[1] /= totalA;
            bynL[2] /= totalA;
            bynL[3] /= totalA;
            bynL[4] /= totalA;
            bynL[5] /= totalA;
            bynL[6] /= totalA;
            bynL[7] /= totalA;
            bynL[8] /= totalA;
            bynL[9] /= totalA;
            bynL[10] /= 2; // catchall for garbage data
            // end L
            // begin R
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
            hotbinA = floor(benfordize);
            // hotbin becomes the Benford bin value for this number floored
            totalA = 0;
            if ((hotbinA > 0) && (hotbinA < 10))
            {
                bynR[hotbinA] += 1;
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
            hotbinB = floor(benfordize);
            // hotbin becomes the Benford bin value for this number ceiled
            totalB = 0;
            if ((hotbinB > 0) && (hotbinB < 10))
            {
                bynR[hotbinB] += 1;
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
                inputSampleR = floor(inputSampleR);
            }
            else
            {
                bynR[hotbinB] += 1;
                inputSampleR = ceil(inputSampleR);
            }
            // assign the relevant one to the delay line
            // and floor/ceil signal accordingly
            totalA = bynR[1] + bynR[2] + bynR[3] + bynR[4] + bynR[5] + bynR[6] + bynR[7] + bynR[8] + bynR[9];
            totalA /= 1000;
            totalA = 1; // spotted by Laserbat: this 'scaling back' code doesn't. It always divides by the fallback of 1. Old NJAD doesn't scale back the things we're comparing against. Kept to retain known behavior, use the one in StudioTan and Monitoring for a tuned-as-intended NJAD.
            bynR[1] /= totalA;
            bynR[2] /= totalA;
            bynR[3] /= totalA;
            bynR[4] /= totalA;
            bynR[5] /= totalA;
            bynR[6] /= totalA;
            bynR[7] /= totalA;
            bynR[8] /= totalA;
            bynR[9] /= totalA;
            bynR[10] /= 2; // catchall for garbage data
            // end R
            inputSampleL /= 8388608.0;
            inputSampleR /= 8388608.0;
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // pseudorandom number updater
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