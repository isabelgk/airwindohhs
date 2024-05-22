#pragma once
#include <string>

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
