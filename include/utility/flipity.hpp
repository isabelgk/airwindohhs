#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::flipity {

constexpr std::string_view k_name{ "Flipity" };
constexpr std::string_view k_short_description{
    "Flipity is a channel phase flipper/swapper utility."
};
constexpr std::string_view k_long_description{
    "This is per a request. Someone asked for a plugin to flip the phase of the left channel :) well, how about flip the phase of the left or the right channel? Or the phase of the left AND the right channel? Or swap the channels, L for R, R for L? Or, swap the channels and then flip the phase of what is now the left channel? Or swap the channels and then flip the phase of what is now the right channel? Or flip both? That's Flipity. Utility plugin that apparently isn't available on some DAWs. Fair enough then :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class Flipity final : public Effect<T>
{
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'fpty';
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;

  public:
    Flipity()
    {
        A = 0.0;
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
        kNumParameters = 1
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: A = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "flipity"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Flipity"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA:
                switch ((VstInt32)(A * 7.999)) // 0 to almost edge of # of params
                {
                    case 0: vst_strncpy(text, "Dry", kVstMaxParamStrLen); break;
                    case 1: vst_strncpy(text, "Flip L", kVstMaxParamStrLen); break;
                    case 2: vst_strncpy(text, "Flip R", kVstMaxParamStrLen); break;
                    case 3: vst_strncpy(text, "Flip LR", kVstMaxParamStrLen); break;
                    case 4: vst_strncpy(text, "Swap LR", kVstMaxParamStrLen); break;
                    case 5: vst_strncpy(text, "Swip L", kVstMaxParamStrLen); break;
                    case 6: vst_strncpy(text, "Swip R", kVstMaxParamStrLen); break;
                    case 7: vst_strncpy(text, "Swip LR", kVstMaxParamStrLen); break;
                    default: break; // unknown parameter, shouldn't happen!
                }
                break; // E as example 'popup' parameter with four values
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        int flipity = (int)(A * 7.999);
        static const int kDry = 0;
        static const int kFlipL = 1;
        static const int kFlipR = 2;
        static const int kFlipLR = 3;
        static const int kSwap = 4;
        static const int kSwipL = 5;
        static const int kSwipR = 6;
        static const int kSwipLR = 7;
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            double temp;
            switch (flipity) {
                case kDry:
                    break;
                case kFlipL:
                    inputSampleL = -inputSampleL;
                    break;
                case kFlipR:
                    inputSampleR = -inputSampleR;
                    break;
                case kFlipLR:
                    inputSampleL = -inputSampleL;
                    inputSampleR = -inputSampleR;
                    break;
                case kSwap:
                    temp = inputSampleL;
                    inputSampleL = inputSampleR;
                    inputSampleR = temp;
                    break;
                case kSwipL:
                    temp = inputSampleL;
                    inputSampleL = -inputSampleR;
                    inputSampleR = temp;
                    break;
                case kSwipR:
                    temp = inputSampleL;
                    inputSampleL = inputSampleR;
                    inputSampleR = -temp;
                    break;
                case kSwipLR:
                    temp = inputSampleL;
                    inputSampleL = -inputSampleR;
                    inputSampleR = -temp;
                    break;
            }
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::flipity
