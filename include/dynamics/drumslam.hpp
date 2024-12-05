#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::drumslam {

constexpr std::string_view k_name{ "DrumSlam" };
constexpr std::string_view k_short_description{
    "DrumSlam is a heavy-processing tape modeler. This entry contains personal details about what was happening in 2018 for me, and is included in full because there are things that have a place in AirwindowsPedia, as a sort of 'historical section'."
};
constexpr std::string_view k_long_description{
    "So this is not a sad plugin but it’ll be sort of a sad post and I may as well do it anyway: I’ll keep it brief and to the point, and it’s kind of relevant to the Airwindows plugin release schedule.First, the plugin: this is DrumSlam. It was originally meant to sound like Massey TapeHead. It’s sort of a multiband tape emulation/distortion, and it didn’t end up sounding like TapeHead but it does have a sound of its own: all the more since it’s a technique I don’t normally use (multiband stuff rarely makes sense to my ear). I see it as an effect plugin more than general purpose, but as always I’m not the boss of you and you can use it however you please. It’s open source, so you can also use the code or do variations on it: all you have to do is credit that you’re using Airwindows code, and you can even charge for your own DrumSlam-based plugin provided you make the credit clear: in fact, since it’s not the ‘advertising clause’ version, you get to cite Airwindows and suggest that it makes your plugin sound special: if you ask, I’ll help you make that be true. I just don’t take personal responsibility for the sonics of plugins that use ‘Airwindows Open Source’ in their promotion, because there are still ways you could screw it up cooge if you’re also open source, I’d be able to look at the code and give my opinion on whether it’s maintaining the integrity of the input data.So that’s DrumSlam. Try it, slam it, do stuff with it, it’s simply another type of tape emulation done somewhat Airwindows style, and it’s got its own sound that you might like.Also, this is my Patreon, if you find these plugins indispensable please join it at the rate of however many plugins a year you think you’d be buying from me if they were sold at around $50 each, perpetual license complete with source code. It’s kind of a bargain, and the opposite of DRM: in soviet Airwindows, your rights manage me! coogeNow, over to my status report. I think I’m going to be able to keep up my plugin releases at least through StereoFX next week and probably won’t even have to skip a week but it’s possible I won’t be able to focus, or I might derp some of the releases and get them wrong and have to fix them, like with Console5 last Xmas. Apology in advance, if so. The reason is the same as it was then. Last December 6th, my Mom died. She’d kept me from starving when I was starting up the Patreon, and I owe her everything. After that, January 23rd of this year, the cat many of you have seen in videos (my last cat since the other one got hit by a truck the year before) died. She was very old, and around 3 AM that morning she fell down and no vet office was open at 3 AM and I just cuddled her and was with her as she died, which was all I could do (I couldn’t have afforded veterinary care at the time anyhow, I was living on $858 a month from the Patreon). So it’s been a lot of loss around the turn of the year.So, three days ago, Thursday July 5th, my Dad died. I’d got word he was in the hospital (he was VERY old and feeble) and I tore ass down to Pennsylvania to see him: he’d fallen and hurt himself and was unresponsive (like me, he lived alone and got a lot out of being in his home, his final years were good). This was the price of that freedom: he’d hurt himself at a time when nobody would check on him for at least half a day. Once in the hospital, he came to for long enough that my sister got to speak to him, and he was grateful to see her. But then he took another downturn, they resuscitated him, and had to do it so fiercely that it harmed him further. I did see him but he was mostly gone by then, was with my sister as the doctors broke the news that they couldn’t safely keep rescuscitating him, and we told them that in that case it was clear: keep caring for him, but there was nothing more they could really do. Roughly an hour later he passed on.If I’m demanding of myself he’s part of the reason why, for fair and foul reasons. This was a guy I couldn’t really give gifts to: he was a brilliant scientist in his day, and there were no emotional things that didn’t go through that rational filter so if my gift wasn’t perfect he’d be openly unhappy with it. There was an exception: I’ve painted and cartooned and stuff like that, and he valued those things more than I do, and in his declining years I even drew some comic strips just for him. Maybe I’ll scan them now and put them up somewhere as a memorial of the only thing I could do that he uncritically loved. That sounds dark, but there was more to him than only that. When I was tiny he used to indulge a fierce and violent temper, and Mom got him to stop it… and he did, some of my siblings never saw that in all their lives. He was sad a lot of his life, but he was also the Dad who literally built our childhood television set from a Heathkit electronics kit, so I grew up playing Atari 2600 games on the TV my Dad had literally built from parts. To a nerd like me, that is awfully cool. He was a hard act to follow and a hard man to please, and though it was sometimes a rocky path I know that I was able to be supportive in his final years, and the last time I saw him alive and aware, I hugged him and I meant it… and he hugged back.I think I’ll be able to keep the Airwindows release schedule going without missing a week. I think he would respect me wanting to do that: it’s the sort of thing he would do. Also, there’s nobody much left to DIE anymore now that my parents and my cat have gone, so at this stage it’s time to just keep doing what I do, and that’s what would please me and would’ve pleased them.Well, not the cat. She would have demanded cuddles. But I was good for those, too.Love you guys, and I will try not to put out any awful buggy derps of plugins, but fair warning in case I do: it’s been sort of a difficult half-year. It ought to start getting better, I feel I must be through the worst of it and I’m still here and will be OK. <3"
};
constexpr std::string_view k_tags{
    "dynamics"
};

template <typename T>
class DrumSlam final : public Effect<T>
{
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    double iirSampleAL;
    double iirSampleBL;
    double iirSampleCL;
    double iirSampleDL;
    double iirSampleEL;
    double iirSampleFL;
    double iirSampleGL;
    double iirSampleHL;
    double lastSampleL;
    double iirSampleAR;
    double iirSampleBR;
    double iirSampleCR;
    double iirSampleDR;
    double iirSampleER;
    double iirSampleFR;
    double iirSampleGR;
    double iirSampleHR;
    double lastSampleR;
    bool fpFlip;
    float A;
    float B;
    float C;
    float D;
    float E; // parameters. Always 0-1, and we scale/alter them elsewhere.

  public:
    DrumSlam()
    {
        A = 0.0;
        B = 1.0;
        C = 1.0;
        iirSampleAL = 0.0;
        iirSampleBL = 0.0;
        iirSampleCL = 0.0;
        iirSampleDL = 0.0;
        iirSampleEL = 0.0;
        iirSampleFL = 0.0;
        iirSampleGL = 0.0;
        iirSampleHL = 0.0;
        lastSampleL = 0.0;
        iirSampleAR = 0.0;
        iirSampleBR = 0.0;
        iirSampleCR = 0.0;
        iirSampleDR = 0.0;
        iirSampleER = 0.0;
        iirSampleFR = 0.0;
        iirSampleGR = 0.0;
        iirSampleHR = 0.0;
        lastSampleR = 0.0;
        fpFlip = true;
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
        kParamB = 1,
        kParamC = 2,
        kNumParameters = 3
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.0; break;
            case kParamB: return 1.0; break;
            case kParamC: return 1.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "drive"; break;
            case kParamB: return "output"; break;
            case kParamC: return "dry/wet"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Drive"; break;
            case kParamB: return "Output"; break;
            case kParamC: return "Dry/Wet"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string(A); break;
            case kParamB: return std::to_string(B); break;
            case kParamC: return std::to_string(C); break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return ""; break;
            case kParamB: return ""; break;
            case kParamC: return ""; break;

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
        double iirAmountL = 0.0819;
        iirAmountL /= overallscale;
        double iirAmountH = 0.377933067;
        iirAmountH /= overallscale;
        double drive = (A * 3.0) + 1.0;
        double out = B;
        double wet = C;
        // removed extra dry variable
        while (--sampleFrames >= 0)
        {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            double lowSampleL;
            double lowSampleR;
            double midSampleL;
            double midSampleR;
            double highSampleL;
            double highSampleR;
            inputSampleL *= drive;
            inputSampleR *= drive;
            if (fpFlip)
            {
                iirSampleAL = (iirSampleAL * (1 - iirAmountL)) + (inputSampleL * iirAmountL);
                iirSampleBL = (iirSampleBL * (1 - iirAmountL)) + (iirSampleAL * iirAmountL);
                lowSampleL = iirSampleBL;
                iirSampleAR = (iirSampleAR * (1 - iirAmountL)) + (inputSampleR * iirAmountL);
                iirSampleBR = (iirSampleBR * (1 - iirAmountL)) + (iirSampleAR * iirAmountL);
                lowSampleR = iirSampleBR;
                iirSampleEL = (iirSampleEL * (1 - iirAmountH)) + (inputSampleL * iirAmountH);
                iirSampleFL = (iirSampleFL * (1 - iirAmountH)) + (iirSampleEL * iirAmountH);
                midSampleL = iirSampleFL - iirSampleBL;
                iirSampleER = (iirSampleER * (1 - iirAmountH)) + (inputSampleR * iirAmountH);
                iirSampleFR = (iirSampleFR * (1 - iirAmountH)) + (iirSampleER * iirAmountH);
                midSampleR = iirSampleFR - iirSampleBR;
                highSampleL = inputSampleL - iirSampleFL;
                highSampleR = inputSampleR - iirSampleFR;
            }
            else
            {
                iirSampleCL = (iirSampleCL * (1 - iirAmountL)) + (inputSampleL * iirAmountL);
                iirSampleDL = (iirSampleDL * (1 - iirAmountL)) + (iirSampleCL * iirAmountL);
                lowSampleL = iirSampleDL;
                iirSampleCR = (iirSampleCR * (1 - iirAmountL)) + (inputSampleR * iirAmountL);
                iirSampleDR = (iirSampleDR * (1 - iirAmountL)) + (iirSampleCR * iirAmountL);
                lowSampleR = iirSampleDR;
                iirSampleGL = (iirSampleGL * (1 - iirAmountH)) + (inputSampleL * iirAmountH);
                iirSampleHL = (iirSampleHL * (1 - iirAmountH)) + (iirSampleGL * iirAmountH);
                midSampleL = iirSampleHL - iirSampleDL;
                iirSampleGR = (iirSampleGR * (1 - iirAmountH)) + (inputSampleR * iirAmountH);
                iirSampleHR = (iirSampleHR * (1 - iirAmountH)) + (iirSampleGR * iirAmountH);
                midSampleR = iirSampleHR - iirSampleDR;
                highSampleL = inputSampleL - iirSampleHL;
                highSampleR = inputSampleR - iirSampleHR;
            }
            fpFlip = !fpFlip;
            // generate the tone bands we're using
            if (lowSampleL > 1.0) {
                lowSampleL = 1.0;
            }
            if (lowSampleL < -1.0) {
                lowSampleL = -1.0;
            }
            if (lowSampleR > 1.0) {
                lowSampleR = 1.0;
            }
            if (lowSampleR < -1.0) {
                lowSampleR = -1.0;
            }
            lowSampleL -= (lowSampleL * (fabs(lowSampleL) * 0.448) * (fabs(lowSampleL) * 0.448));
            lowSampleR -= (lowSampleR * (fabs(lowSampleR) * 0.448) * (fabs(lowSampleR) * 0.448));
            lowSampleL *= drive;
            lowSampleR *= drive;
            if (highSampleL > 1.0) {
                highSampleL = 1.0;
            }
            if (highSampleL < -1.0) {
                highSampleL = -1.0;
            }
            if (highSampleR > 1.0) {
                highSampleR = 1.0;
            }
            if (highSampleR < -1.0) {
                highSampleR = -1.0;
            }
            highSampleL -= (highSampleL * (fabs(highSampleL) * 0.599) * (fabs(highSampleL) * 0.599));
            highSampleR -= (highSampleR * (fabs(highSampleR) * 0.599) * (fabs(highSampleR) * 0.599));
            highSampleL *= drive;
            highSampleR *= drive;
            midSampleL = midSampleL * drive;
            midSampleR = midSampleR * drive;
            double skew = (midSampleL - lastSampleL);
            lastSampleL = midSampleL;
            // skew will be direction/angle
            double bridgerectifier = fabs(skew);
            if (bridgerectifier > 3.1415926) {
                bridgerectifier = 3.1415926;
            }
            // for skew we want it to go to zero effect again, so we use full range of the sine
            bridgerectifier = sin(bridgerectifier);
            if (skew > 0) {
                skew = bridgerectifier * 3.1415926;
            }
            else {
                skew = -bridgerectifier * 3.1415926;
            }
            // skew is now sined and clamped and then re-amplified again
            skew *= midSampleL;
            // cools off sparkliness and crossover distortion
            skew *= 1.557079633;
            // crank up the gain on this so we can make it sing
            bridgerectifier = fabs(midSampleL);
            bridgerectifier += skew;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            bridgerectifier *= drive;
            bridgerectifier += skew;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            if (midSampleL > 0)
            {
                midSampleL = bridgerectifier;
            }
            else
            {
                midSampleL = -bridgerectifier;
            }
            // blend according to positive and negative controls, left
            skew = (midSampleR - lastSampleR);
            lastSampleR = midSampleR;
            // skew will be direction/angle
            bridgerectifier = fabs(skew);
            if (bridgerectifier > 3.1415926) {
                bridgerectifier = 3.1415926;
            }
            // for skew we want it to go to zero effect again, so we use full range of the sine
            bridgerectifier = sin(bridgerectifier);
            if (skew > 0) {
                skew = bridgerectifier * 3.1415926;
            }
            else {
                skew = -bridgerectifier * 3.1415926;
            }
            // skew is now sined and clamped and then re-amplified again
            skew *= midSampleR;
            // cools off sparkliness and crossover distortion
            skew *= 1.557079633;
            // crank up the gain on this so we can make it sing
            bridgerectifier = fabs(midSampleR);
            bridgerectifier += skew;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            bridgerectifier *= drive;
            bridgerectifier += skew;
            if (bridgerectifier > 1.57079633) {
                bridgerectifier = 1.57079633;
            }
            bridgerectifier = sin(bridgerectifier);
            if (midSampleR > 0)
            {
                midSampleR = bridgerectifier;
            }
            else
            {
                midSampleR = -bridgerectifier;
            }
            // blend according to positive and negative controls, right
            inputSampleL = ((lowSampleL + midSampleL + highSampleL) / drive) * out;
            inputSampleR = ((lowSampleR + midSampleR + highSampleR) / drive) * out;
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 1.1e-44l * pow(2,expon+62));
            // end 64 bit stereo floating point dither
            *out1 = inputSampleL;
            *out2 = inputSampleR;
            *in1++;
            *in2++;
            *out1++;
            *out2++;
        }
    }
};
} // namespace airwindohhs::drumslam
