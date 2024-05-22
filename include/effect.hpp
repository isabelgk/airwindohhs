#pragma once
#include <string>

template <typename T>
class Effect
{
    T m_samplerate{};

  public:
    virtual ~Effect() = default;

    virtual std::string name() = 0;
    virtual void process(T** inputs, T** outputs, long sampleframes) = 0;

    virtual float get_parameter_value(int index) = 0;
    virtual void set_parameter_value(int index, float value) = 0;
    virtual std::string get_parameter_name(int index) = 0;
    virtual std::string get_parameter_display(int index) = 0;
    virtual std::string get_parameter_label(int index) = 0;

    T getSampleRate()
    {
        return m_samplerate;
    }

    void setSampleRate(T samplerate)
    {
        m_samplerate = samplerate;
    }
};
