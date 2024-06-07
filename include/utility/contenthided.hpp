#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::contenthided {

constexpr std::string_view k_name{ "ContentHideD" };
constexpr std::string_view k_short_description{
    "ContentHideD mutes audio except for brief glimpses."
};
constexpr std::string_view k_long_description{
    "This is a utility plugin meant for use with YouTube… by me. I'm not sure it'll be useful to anyone else, but who knows?ContentHideD simply mutes audio, almost all the time. It'll wait a few seconds, not so long that you get bored waiting but not super-quick or regular, and then it'll let through a blip of audio less than a second in length. To the best of my knowledge, you could play highly protected audio through this all day long and ContentID will not engage. I've heard that you can go as long as five seconds of audio, but remember that legally you get ZERO seconds of grace: if the sound can be identified, copyright can be claimed.So this is not for getting rights to audio. It's for being able to play brief glimpses of audio that is otherwise highly contentious and not allowed for plain old listening. You wouldn't use ContentHideD if you wanted to enjoy the music, as you effectively won't get the music. But, I have a need for playing clips of audio to illustrate points I'm making regarding the metering and measurement of such audio.And in situations where I know I can't put the audio on and show the metering to use the audio for educational purposes in teaching people what to do with peak energy and headroom… ContentHideD ought to let me expose brief snippets of what's being shown on a meter, even if it's the Beatles. After all, it's not the song you're getting: it's rendered useless for that purpose, but you'll still get glimpses of the overall sound of the track, and that's the point.My demo video tests this theory: I play all of Back In Black, by AC/DC, to show how it registers on my meter (predictably, it does incredibly well from start to finish) and you barely hear the song, but you get many glimpses of what various tones sound like coming from the original vinyl record, uncompressed and unlimited. I'll follow up if my video gets claimed, as the point of this plugin is to stop you getting claimed, but I suspect it deletes SO much audio in a slightly randomized way that ContentID will not kick in. It really shouldn't as it's said you can play several seconds of audio, but I wanted to make this plugin unequivocally just for exposing bits of TONE and revealing no actual music.If that works, I ought to be able to follow up with other tracks where I can play the whole song unmuted because react channels are allowed to play 'em and the record company simply claims the revenue. On that note if anyone has thoughts about what they'd like to see and hear, I'm taking requests. No limits, because now I can do a whole Evergreens video and then toss this plugin on that part of the video and export again, giving me an immediate replacement if it turns out the song's owner does not want the song heard as a song…Meanwhile, I'll get back to work on the next batch of plugins, things are going to start to get lively again pretty soon :)"
};
constexpr std::string_view k_tags{
    "utility"
};

template <typename T>
class ContentHideD final : public Effect<T>
{
    double demotimer;
    uint32_t fpd;
    // default stuff

  public:
    ContentHideD()
    {
        demotimer = 0.0;
        fpd = 1.0;
        while (fpd < 16386) {
            fpd = rand() * UINT32_MAX;
        }
        // this is reset: values being initialized only once. Startup values, whatever they are.
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

            default: break;
        }
        return {};
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 44100.0;
        overallscale *= Effect<T>::getSampleRate();
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            double duck = 0.0;
            demotimer -= (1.0 / overallscale);
            if (demotimer < 441.0) {
                duck = 1.0 - (demotimer / 441.0);
            }
            if (demotimer > 44100.0) {
                duck = ((demotimer - 44100.0) / 441.0);
            }
            if (duck > 1.0) {
                duck = 1.0;
            }
            duck = sin((1.0 - duck) * 1.57);
            if ((demotimer < 1) || (demotimer > 441000.0)) {
                double randy = (double(fpd) / (double)UINT32_MAX); // 0 to 1 the noise, may not be needed
                demotimer = 100000.0 * (randy + 2.0);
            }
            inputSampleL *= duck;
            inputSampleR *= duck;
            fpd ^= fpd << 13;
            fpd ^= fpd >> 17;
            fpd ^= fpd << 5;
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::contenthided
