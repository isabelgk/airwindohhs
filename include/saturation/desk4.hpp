#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs {
template <typename T>
class Desk4 final : public Effect<T>
{
    const std::string m_name{ "Desk4" };

    double dL[10000];
    double controlL;
    double lastSampleL;
    double lastOutSampleL;
    double lastSlewL;
    double dR[10000];
    double controlR;
    double lastSampleR;
    double lastOutSampleR;
    double lastSlewR;
    int gcount;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
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
    Desk4()
    {
        A = 0.27;
        B = 0.18;
        C = 0.26;
        D = 0.54;
        E = 0.84;
        F = 1.0;
        for (int count = 0; count < 9999; count++) {
            dL[count] = 0;
            dR[count] = 0;
        }
        controlL = 0;
        lastSampleL = 0.0;
        lastOutSampleL = 0.0;
        lastSlewL = 0.0;
        controlR = 0;
        lastSampleR = 0.0;
        lastOutSampleR = 0.0;
        lastSlewR = 0.0;
        gcount = 0;
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
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index) override
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

    std::string get_parameter_name(int index) override
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Overdrive";
            case kParamB: return "Hi Choke";
            case kParamC: return "Power Sag";
            case kParamD: return "Frequency";
            case kParamE: return "Output Trim";
            case kParamF: return "Dry/Wet";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) override
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

    std::string get_parameter_label(int index) override
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

    void process(T** inputs, T** outputs, long sampleFrames) override
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= getSampleRate();
        double gain = (pow(A, 2) * 10) + 0.0001;
        double gaintrim = (pow(A, 2) * 2) + 1.0;
        double slewgain = (pow(B, 3) * 40) + 0.0001;
        double prevslew = 0.105;
        double intensity = (pow(C, 6) * 15) + 0.0001;
        double depthA = (pow(D, 4) * 940) + 0.00001;
        int offsetA = (int)(depthA * overallscale);
        if (offsetA < 1) {
            offsetA = 1;
        }
        if (offsetA > 4880) {
            offsetA = 4880;
        }
        double balanceB = 0.0001;
        slewgain *= overallscale;
        prevslew *= overallscale;
        balanceB /= overallscale;
        double outputgain = E;
        double wet = F;
        // removed extra dry variable
        double clampL;
        double clampR;
        double thicknessL;
        double thicknessR;
        double out;
        double balanceA = 1.0 - balanceB;
        double bridgerectifier;
        double slewL;
        double slewR;
        double combSampleL;
        double combSampleR;
        double drySampleL;
        double drySampleR;
        double inputSampleL;
        double inputSampleR;
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
            drySampleL = inputSampleL;
            drySampleR = inputSampleR;
            if (gcount < 0 || gcount > 4900) {
                gcount = 4900;
            }
            dL[gcount + 4900] = dL[gcount] = fabs(inputSampleL) * intensity;
            controlL += (dL[gcount] / offsetA);
            controlL -= (dL[gcount + offsetA] / offsetA);
            controlL -= 0.000001;
            clampL = 1;
            if (controlL < 0) {
                controlL = 0;
            }
            if (controlL > 1) {
                clampL -= (controlL - 1);
                controlL = 1;
            }
            if (clampL < 0.5) {
                clampL = 0.5;
            }
            dR[gcount + 4900] = dR[gcount] = fabs(inputSampleR) * intensity;
            controlR += (dR[gcount] / offsetA);
            controlR -= (dR[gcount + offsetA] / offsetA);
            controlR -= 0.000001;
            clampR = 1;
            if (controlR < 0) {
                controlR = 0;
            }
            if (controlR > 1) {
                clampR -= (controlR - 1);
                controlR = 1;
            }
            if (clampR < 0.5) {
                clampR = 0.5;
            }
            gcount--;
            // control = 0 to 1
            thicknessL = ((1.0 - controlL) * 2.0) - 1.0;
            thicknessR = ((1.0 - controlR) * 2.0) - 1.0;
            out = fabs(thicknessL);
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thicknessL > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleL > 0) {
                inputSampleL = (inputSampleL * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleL = (inputSampleL * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            out = fabs(thicknessR);
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            // max value for sine function
            if (thicknessR > 0) {
                bridgerectifier = sin(bridgerectifier);
            }
            else {
                bridgerectifier = 1 - cos(bridgerectifier);
            }
            // produce either boosted or starved version
            if (inputSampleR > 0) {
                inputSampleR = (inputSampleR * (1 - out)) + (bridgerectifier * out);
            }
            else {
                inputSampleR = (inputSampleR * (1 - out)) - (bridgerectifier * out);
            }
            // blend according to density control
            inputSampleL *= clampL;
            inputSampleR *= clampR;
            slewL = inputSampleL - lastSampleL;
            lastSampleL = inputSampleL;
            // Set up direct reference for slew
            slewR = inputSampleR - lastSampleR;
            lastSampleR = inputSampleR;
            // Set up direct reference for slew
            bridgerectifier = fabs(slewL * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slewL > 0) {
                slewL = bridgerectifier / slewgain;
            }
            else {
                slewL = -(bridgerectifier / slewgain);
            }
            bridgerectifier = fabs(slewR * slewgain);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (slewR > 0) {
                slewR = bridgerectifier / slewgain;
            }
            else {
                slewR = -(bridgerectifier / slewgain);
            }
            inputSampleL = (lastOutSampleL * balanceA) + (lastSampleL * balanceB) + slewL;
            // go from last slewed, but include some raw values
            lastOutSampleL = inputSampleL;
            // Set up slewed reference
            inputSampleR = (lastOutSampleR * balanceA) + (lastSampleR * balanceB) + slewR;
            // go from last slewed, but include some raw values
            lastOutSampleR = inputSampleR;
            // Set up slewed reference
            combSampleL = fabs(drySampleL * lastSampleL);
            if (combSampleL > 1.0) {
                combSampleL = 1.0;
            }
            // bailout for very high input gains
            combSampleR = fabs(drySampleR * lastSampleR);
            if (combSampleR > 1.0) {
                combSampleR = 1.0;
            }
            // bailout for very high input gains
            inputSampleL -= (lastSlewL * combSampleL * prevslew);
            lastSlewL = slewL;
            // slew interaction with previous slew
            inputSampleR -= (lastSlewR * combSampleR * prevslew);
            lastSlewR = slewR;
            // slew interaction with previous slew
            inputSampleL *= gain;
            bridgerectifier = fabs(inputSampleL);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleL > 0) {
                inputSampleL = bridgerectifier;
            }
            else {
                inputSampleL = -bridgerectifier;
            }
            // drive section
            inputSampleL /= gain;
            inputSampleL *= gaintrim;
            // end of Desk section
            inputSampleR *= gain;
            bridgerectifier = fabs(inputSampleR);
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.0;
            }
            else {
                bridgerectifier = sin(bridgerectifier);
            }
            if (inputSampleR > 0) {
                inputSampleR = bridgerectifier;
            }
            else {
                inputSampleR = -bridgerectifier;
            }
            // drive section
            inputSampleR /= gain;
            inputSampleR *= gaintrim;
            // end of Desk section
            if (outputgain != 1.0) {
                inputSampleL *= outputgain;
                inputSampleR *= outputgain;
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
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
} // namespace airwindohhs
