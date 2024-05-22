#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs
{
template <typename T>
class %%CLASSNAME%% final : public Effect<T>
{
    std::string m_name{"%%CLASSNAME%%"};

%%PRIVATEVARS%%

    enum params
    {
        %%PARAM_ENUM%%
    };

public:
    %%CLASSNAME%%()
    {
        %%INITIALIZATION%%
    }

    constexpr std::string_view name() {
        return m_name;
    }

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        %%SETPARAMSWITCH%%
        default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMSWITCH%%
        default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMNAMESWITCH%%
        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMLABELDISPLAY%%
        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMLABELSWITCH%%
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T *in1 = inputs[0];
        T *in2 = inputs[1];
        T *out1 = outputs[0];
        T *out2 = outputs[1];

        %%PROCESS%%
    }

};
} // namespace airwindohhs
