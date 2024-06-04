#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::%%NAMESPACE%%
{

constexpr std::string_view k_name{"%%CLASSNAME%%"};
constexpr std::string_view k_short_description{
    "%%SHORT_DESCRIPTION%%"
};
constexpr std::string_view k_long_description{
    "%%LONG_DESCRIPTION%%"
};
constexpr std::string_view k_tags{
    "%%TAGS%%"
};

template <typename T>
class %%CLASSNAME%% final : public Effect<T>
{
%%PRIVATEVARS%%

public:
    %%CLASSNAME%%()
    {
        %%INITIALIZATION%%
    }

    enum params
    {
        %%PARAM_ENUM%%
    };

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

    T get_parameter_default(int index)
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMDEFAULTSWITCH%%
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

    constexpr std::string_view get_parameter_title(int index)
    {
        switch(static_cast<params>(index))
        {
        %%GETPARAMTITLESWITCH%%
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
} // namespace airwindohhs::%%NAMESPACE%%
