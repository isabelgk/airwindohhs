#pragma once
#include <cmath>
#include <cstdio>
#include <string>

typedef int VstInt32;

template <typename T>
class Effect
{
    // The VST SDK's AudioEffect starts at 44100 too; plugins divide by
    // getSampleRate() / 44100.0, so 0 would produce inf/NaN output.
    T m_samplerate{ 44100 };

  public:
    virtual ~Effect() = default;

    T getSampleRate() const
    {
        return m_samplerate;
    }

    void setSampleRate(T samplerate)
    {
        m_samplerate = samplerate;
    }
};

namespace airwindohhs {

// Stand-ins for the VST SDK string helpers that upstream parameter label and
// display code calls. They write the whole string: the SDK's 8-character
// limit is a host-side constraint this library doesn't have.
inline constexpr VstInt32 kVstMaxParamStrLen = 8;

inline void vst_strncpy(std::string& text, const char* source, VstInt32)
{
    text = source;
}

// Matches the SDK's layout of 8 characters including the point and sign
// ("0.500000", "12.50000"), rounding the last digit where the SDK truncates.
inline void float2string(float value, std::string& text, VstInt32)
{
    double v = value;
    if (v > 99999999.0 || v < -9999999.0) {
        text = "Huge!";
        return;
    }
    int integer_digits = 1;
    for (double magnitude = std::fabs(v); magnitude >= 10.0; magnitude /= 10.0) {
        integer_digits++;
    }
    int decimals = 8 - 1 - integer_digits - (v < 0 ? 1 : 0);
    if (decimals < 0) {
        decimals = 0;
    }
    char buffer[32];
    std::snprintf(buffer, sizeof(buffer), "%.*f", decimals, v);
    text = buffer;
}

inline void int2string(VstInt32 value, std::string& text, VstInt32)
{
    if (value >= 100000000) {
        text = "Huge!";
        return;
    }
    text = std::to_string(value);
}

inline void dB2string(float value, std::string& text, VstInt32 maxLen)
{
    if (value <= 0) {
        text = "-oo";
        return;
    }
    float2string(static_cast<float>(20.0 * std::log10(value)), text, maxLen);
}

} // namespace airwindohhs
