#pragma once
#include <string>

typedef int VstInt32;

template <typename T>
class Effect
{
    T m_samplerate{};

  public:
    virtual ~Effect() = default;

    T getSampleRate()
    {
        return m_samplerate;
    }

    void setSampleRate(T samplerate)
    {
        m_samplerate = samplerate;
    }
};
