#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::rightomono {

constexpr std::string_view k_name{ "RightoMono" };
constexpr std::string_view k_short_description{
    "RightoMono copies one channel to both, losslessly."
};
constexpr std::string_view k_long_description{
    "Time for another tiny utility plugin or two!LeftoMono or RightoMono exist for only one purpose: if your input is either left or right, it’s coming in on a stereo channel, and you have no use for the other side.No, worse: you’re coming off some kind of camera or camcorder and the other side is not only ‘nothing’, it shows some noise or interference that’s actively bad. In those situations you are often given the option, ‘Convert to mono?’ but doing that will combine the real signal with the garbage, interference, not-really-clean-silence signal.And so, ToMono. They do exactly what the name suggests. There’s nothing to them, no controls or UI of any kind, you don’t need to open any interface or do anything. Instantiate the one you need and boom: you are now copying the input data words from the side you want, to the side you don’t. Pure cloned mono of the highest possible quality in a single plugin. And since it’s so simple it’ll work in places like OBS where maybe sometimes they don’t handle generic VSTs properly or generate controls: hey, none needed!This is a super boring plugin… except when you need it most. Hope you like it. (I no longer need it as I’m running my lav into the real mixing board… but I used LeftoMono last week, before putting it out. It can’t save the camera from other issues like robot-voice over HDMI, but it rescued me from the ‘interference and bad silence’ extra stereo channel. :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class RightoMono final : public Effect<T>
{

  public:
    RightoMono()
    {
    }

    enum params
    {
        kNumParameters = 0

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        while (--sampleFrames >= 0)
        {
            *out1 = *in2;
            *out2 = *in2;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::rightomono
