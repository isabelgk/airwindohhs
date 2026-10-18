#pragma once
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
