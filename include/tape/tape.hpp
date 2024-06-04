#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::tape {

constexpr std::string_view k_name{ "Tape" };
constexpr std::string_view k_short_description{
    "TapeFat is the tone control from TapeDelay."
};
constexpr std::string_view k_long_description{
    "This one’s for me. It’s very similar to last week’s ToTape, but with the following differences:-simpler name-overdrive uses Spiral, not the Mojo algorithm-simpler controls (not exactly ‘input gain’ but close)-changes to the Head Bump algorithm-no flutterThis is what I WANTED to do with ToTape6, and didn’t. In some ways, that’s good: if I’d axed flutter I would not have spent all day struggling with it and coming up with a better algorithm that more closely resembles real physical tape, something that could come in handy for future echo plugins etc. If I’d done the things I’ve done with Tape, to ToTape6, then ToTape6 wouldn’t be as adjustable as it is. There’s room for both, and I gave people the complicated many-knobs version because I know you too well ;) and I know what people like, and I’m there for you.But I also have dreams of my own. So, the plugin (first ever from Airwindows) that carries just the generic name ‘Tape’ is Airwindows tape emulation MY way.I’ve heard a lot of tape in my time, being over 50 years old. I’ve dubbed and re-dubbed tapes a lot. So I dialed in (and re-programmed) Tape by loading up eight instances of it, in a row, and making it behave itself as well as could be expected while running audio through eight instances of Tape. It’s not meant to be clean if you do that: it’s meant to be eightfold trash, but the right kind of trash I’m familiar with when you’ve got that much generation loss and head bump buildup. I knew that if I could get that right, if I could get it to behave okay under that kind of duress, I could rely on it as a go-to output stage (going just before Monitoring) that would condition the sound in the right kind of way.And so it does.Tape will be heard from again, but much as ToTape5 bore the standard for Airwindows tape emulation for years, Tape is my personal choice for ‘mix into’ DAW output stage and it’ll stand for a while, I think. If you need more phat or more flutter or more controls etc etc, use ToTape6, which is just as good in many ways. This is just my ‘director’s cut’ version, designed to my tastes, for if you trust my ears and my choices. Since it’s Airwindows, ToTape6 (and 5) still works and you can have both. This one is for those of you who pursue the simple creed: I have a (virtual) tape machine. I record to my tape machine. I am happy. :)This plugin came out with an original version and then had a Redux update in which I added a control: a slider for the head bump, as people were finding the head bump excessive. Cranking it up all the way gave you the original behavior, but I defaulted it to 0.5.Tape Delay is the reissue (in updated and VST-ified form) of one of my first plugins. This is a precursor to Iron Oxide: it uses a technique for tone shaping that’s like a simplified Iron Oxide. Instead of being a direct EQ, it’s a huge cluster of delay taps, and also a little bit like a convolution impulse combined with an averaging: you get an averaging of just prime-numbered sample delay times. This turns out to work quite well (a direct version of this tone shaper by itself is also coming out)The delay part is what makes this a fun toy. It can do some outlandish things, and also has some gotchas. It chases the delay setting in such a way that you get wild pitch bends from manipulating the control, not just buffer-smashings and dropouts. But, you also get buffer smashings and dropouts, so I wouldn’t dignify this with calling it ’emulation’: it’s just a way of fooling with the delay time control, live. If you’re rough with the control, it’ll get quite choppy. Also, if you intend to use this without glitches, you’ve got to enable the plugin and give it maybe a quarter or half second to chase to its desired setting, or you’ll hear a pitch zoop as the plugin starts up. That’s because this one starts at zero delay on reset, so if there’s audio happening it’ll get caught in the initial zip of the delay time.If you can work with that, or don’t mind the weirdness, then you can enjoy this blast from the past that used to be AU-only for years and years: there’s a lot of interesting stuff like that, for instance Glitch Shifter. This one can give you mad dubby effects, either decaying into heavy Memory Man-like darkness or doing a tape slapback or decaying into bright airyness. Because of the prime-number based tone shaping, regeneration doesn’t produce reinforced artifacts, just continues to emphasize the tone shift you dialed in. (the tone-only version of this that’s coming, could be used in Blue Cat’s Late Replies plugin to make that a Tape Delay-alike, but without the delay time weirdness.)Again, be careful using this on audio regions that are tightly trimmed to the very beginning of the sound: this one needs a second to stabilize if you’re using it to sound like a normal delay. Or, you can just do whatever you like :)TapeDelay2 gives you substantial changes over my original TapeDelay. It’s no longer trying to do the ‘Iron Oxide’ style tone shaping the original one did. In fact, it’s not even a delay in the same sense as its predecessor.Instead, it’s a brand new, undersampled, Airwindows-bandpassed monster ready to make a whole pile of very convincing sounds. As plugin delays go, this covers a big range of purposes.You’ve got a nice long delay time that’s still available at higher sample rates, thanks to the undersampling techniques. The delay line (and the regeneration) makes use of Airwindows bandpasses, but only on the undersampled content: meaning that if you’ve got it set to very nearly full range (resonance of zero) it’ll give the same subtle highpassing and lowpassing no matter what sample rate you’re at (a normal bandpass would have to roll off closer to the sample rate’s Nyquist frequency, in other words it would let through too many highs to do a proper tape emulation). You can tighten the bandwidth by increasing resonance. You can adjust the region you’re highlighting. And you can still adjust the frequency control even when set to full wide, which gives you more of a tilt EQ. It really turned out to have a lot of flexibility, and there’s two separate bandpasses so that you can shape the overall tone and also focus in on the regenerations if you like. Vintage sounds in the classic Airwindows way, meaning ‘no overprocessing, just high fidelity clean and simple processing’. Also, the dry/wet operates like my recent reverbs: 50% means full dry AND full wet, so you can bring in subtle echoes without altering the gain of your underlying track. Use it like a kind of reverb, with whatever tone and resonance works for you!And lastly, just to top it off, Tape Flutter. This is a new implementation that I’ve never tried before. Instead of a simple vibrato, in TapeDelay2 the flutter keys off the amplitude of the underlying track, making it a lot more wavery and irregular. Subtle effects are easily achieved in most settings, and crank it up for more of a warbly dirty-tape quality. It should be irregular enough to sound like real tape wobble. It’s done by modulating the tape speed… because unlike any previous Airwindows tape effect, TapeDelay2 works by taking a full-length tape loop and literally speeding it up, rather than trying to change the length of the delay in any way. So both the warble, and any manipulations you make to the delay time, act like messing with the pitch of a physical tape machine with a set record and playback head… which turns out to be the best way to do this :)This is one of the good ones. Hope ya like it!I thought I had it all worked out. Put out a proper, well-coded TPDF dither, the highpassed variation on it I’ve called PaulDither, and move on to the fancy boutique stuff.But there was this one experiment I had in mind. If you could do the highpass by delaying and then subtracting the random noise (and it gained you a bit of efficiency in the bargain), why not delay it more? It’d create comb filtering, a cancellation node. Why not keep delaying until the cancellation node dropped right down to around 1.5K?Surely this would give me a nice cancellation notch right where the ear is most sensitive, and heightened clarity. What could go wrong?Nope! I made a prototype, called it ‘NodeDither’, began experimenting, and immediately found that I’d made… a flanger! The long delay settings were useless. It made obvious overtones just out of the dither noise, a blatant tone color cast that wouldn’t produce the desired effect. It didn’t even produce an obvious notch in the response where I wanted it. The only thing it did do, was continue to function as a working TPDF dither no matter what the setting was (more on this later).But, something else turned up in the experiments, and that’s what brought you TapeDither.If you use one sample of delay and inverting the noise, you get PaulDither: simple one-pole highpass. If you use two samples of delay, you get another sort of texture: kind of silky, but still digitally bright and intrusive. Using lots of delay, such as ten samples, starts to sound like the flanger, undesirable.But, there’s something interesting about powers-of-two delay times. One, two, four, eight and so on, these delay times are slightly less ‘colored’ in tone than the others. I think it has to do with interactions with the sample rate: they seem to line up more neatly, making it slightly more easy not to hear the pitch of the ‘flangey’ quality.And four samples of delay (and then inverting the noise) produces something rather special: a noise profile that closely resembles what you hear off reel-to-reel tape.I can’t specify particular brands because (a) I hate when people do that to brand names not their own and (b) it’s a technical discovery, not some complicated way of forcing digital audio to mimic a particular brand. It’s no specific tape stock or tape machine. But what it is, is a voicing for TPDF dither that rolls off in an obvious way, around where tape noise rolls off. There’s another little bump past that, which many people won’t be able to hear, and then it begins to roll off again as it reaches the Nyquist frequency beyond where digital audio can’t go. Compare that to any normal flat, TPDF, or highpassed dither. Those keep putting out noise energy right up to the frequency limit.TapeDither is every bit a TPDF dither, technically correct and flawless as far as dither goes. But it also is a highpassed dither with a softer tonal voicing that resembles good tape machines, and that doesn’t affect the dither performance at all. It doesn’t attenuate the audio content at all. Only the background dither noise is turned into what you’d get off a tape deck, all while the audio is protected from truncation and digital artifacts.I would use this anywhere I was sending processed stems or tracks outboard to a multichannel DAC, for mixing in the analog domain or summing with an outboard summing box. Without hesitation. I’d use fancier boutique dithers on the 2-buss, though this will work there too… but if I was doing an analog mix from a multichannel converter, there is nothing I’d rather use than TapeDither. (maybe Naturalize. Maybe.)That’s because it’s one thing to apply an interesting dither (like Spatialize, for instance, or some noise shaper) on the 2-buss and get a unique texture or presentation on the audio. But if you’re stacking up lots of tracks and all of them get the same treatment, it’s vital to use something that won’t develop an exaggerated tone signature when multiplied that way. For dither, TPDF gives you that behavior, and TapeDither is the TPDF that most closely mimics the reality of analog mixing back in the days of big reel-to-reel machines. The tone of its noise floor is already more pillowy, mellow and relaxing than most TPDF or highpassed TPDF, and then when you stack up lots of tracks of it, the texture’s better still.I’m still excited to bring out Naturalize, but I’m just as excited about what TapeDither means. It makes me want to build analog consoles and mix into them, just so I can put TapeDither on any output that’s not playing raw 24 bit data exactly as captured. Anything with processing or gain on it would get a dithered output, and it would be a matter of—faders up—hello, Seventies :)Well, I like audio from the Seventies. I suppose I’d better work out how to do some of the sonic quirks of Doubly while I’m at it! Not that a rock band would use Doubly…Sometimes it’s good to have just a little specialty plugin that does a useful thing. Hard to do that in the commercial plugin biz, where everything has to be the biggest hype to date: but hey! Thanks to Patreon I’m free of all that, and can follow my vision.It helps that I put out a lot of plugins: it’d be weird to do this as my only plugin for the month. But, while I work on Desk4 and StarChild from the greatest-hits list, I thought I’d sprinkle a little TapeDust for you.This is slightly different from the tape noise in Iron Oxide, though it’s the same general principle. It is a slew noise plugin. What that means is, the noise ONLY hits high frequencies or anywhere the signal’s moving rapidly (there’s a teeny bit of other noise added at high settings, but it’s mostly that).Note the ‘or anywhere the signal’s moving rapidly’. This isn’t a crossover. If you put in a sine wave that’s low and loud, you’ll get very obvious noise only as it crosses through zero, and that’ll sound odd. In general, cranking this up is weird. It’ll depend hugely on what kind of signal you’ve got… but that’s the beauty of it if you can master where to use this plugin.Basses? Probably no way. Full mix? Getouttahere, no chance outside very low settings. Guitar? Hmmm. Drums, loops? A pattern emerges.TapeDust can convert ugly sharp treble attacks on pointy percussive atonal sounds to pretty much any degree of dense, noisy, natural-sounding crunch. It’s a type of noise, so it also gives analog-style variance to repeated samples that might sound over-digital. And the less tonal, or the less ‘pure clear note’ the signal is, the more TapeDust you can get away with. Since it’s a slew noise, it hits the treble of your signal HARD, but since it’s a noise, it’s not filtering or softening the sound as much as it’s just eroding it, weathering it, making it more natural. Anywhere you’ve got bright highs on a nonpure sound, you can grind them off with TapeDust.Of course, if you’re cool with using super-low settings, you can do that anywhere: it’s just important to register that this very specialized and dedicated tool is super picky about what it likes to work on. It’s a beautiful example of taking your production skills deeper: use something that can sound horrible and wrong, and find places where it’s in its element. You can do outlandish textural things, taking something like a clean electro mix with deep clean bass, and sticking heavy TapeDust on just one element in the mix to contrast with the un-grungy elements. I hope you like TapeDust. It’s the kind of plugin I love to make.(note: there has been a bug in this plugin causing it to sound different than intended)TapeFat is just the tone control from TapeDelay. It works like an averaging filter that you can use to either roll off highs (or eventually mids), or subtract the effect to create a highpass and take out the lows.The reason this is interesting is, that tone control is completely bizarre. It’s an averaging filter, but on a pile of delay taps arranged according to prime numbers. Works more like an ambience control, but more densely packed. If you put an impulse through it you don’t get a smoothed-out lowpass so much as a bizarre micro-reverb. Since it’s using primes, it doesn’t reinforce any particular frequency. Since it’s an ambience, it doesn’t have any pre-echoes like linear phase EQs, and the artifacts it produces become a tone of their own (either in-phase, or inverted).You can hear it on the video, which has a number of things updated, not least this: the new audio is directly captured analog sorta-house music out of my livestreams. This way you ought to be able to really hear the way my plugins retain analog qualities, because now the demo music is essentially AAD: not products of other DAW mixes or digital synthesis, but source material."
};
constexpr std::string_view k_tags{
    "tape"
};

template <typename T>
class Tape final : public Effect<T>
{
    double iirMidRollerAL;
    double iirMidRollerBL;
    double iirHeadBumpAL;
    double iirHeadBumpBL;
    double iirMidRollerAR;
    double iirMidRollerBR;
    double iirHeadBumpAR;
    double iirHeadBumpBR;
    double biquadAL[9];
    double biquadBL[9];
    double biquadCL[9];
    double biquadDL[9];
    double biquadAR[9];
    double biquadBR[9];
    double biquadCR[9];
    double biquadDR[9];
    bool flip;
    double lastSampleL;
    double lastSampleR;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;

  public:
    Tape()
    {
        A = 0.5;
        B = 0.5;
        iirMidRollerAL = 0.0;
        iirMidRollerBL = 0.0;
        iirHeadBumpAL = 0.0;
        iirHeadBumpBL = 0.0;
        iirMidRollerAR = 0.0;
        iirMidRollerBR = 0.0;
        iirHeadBumpAR = 0.0;
        iirHeadBumpBR = 0.0;
        for (int x = 0; x < 9; x++) {
            biquadAL[x] = 0.0;
            biquadBL[x] = 0.0;
            biquadCL[x] = 0.0;
            biquadDL[x] = 0.0;
            biquadAR[x] = 0.0;
            biquadBR[x] = 0.0;
            biquadCR[x] = 0.0;
            biquadDR[x] = 0.0;
        }
        flip = false;
        lastSampleL = 0.0;
        lastSampleR = 0.0;
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
        kNumParameters = 2

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return A;
            case kParamB: return B;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.5;
            case kParamB: return 0.5;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "slam";
            case kParamB: return "bump";

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Slam";
            case kParamB: return "Bump";

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return std::to_string((A - 0.5) * 24.0);
            case kParamB: return std::to_string(B);

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "dB";
            case kParamB: return "";
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
        overallscale *= getSampleRate();
        double inputgain = pow(10.0, ((A - 0.5) * 24.0) / 20.0);
        double bumpgain = B * 0.1;
        double HeadBumpFreq = 0.12 / overallscale;
        double softness = 0.618033988749894848204586;
        double RollAmount = (1.0 - softness) / overallscale;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        biquadAL[0] = biquadBL[0] = biquadAR[0] = biquadBR[0] = 0.0072 / overallscale;
        biquadAL[1] = biquadBL[1] = biquadAR[1] = biquadBR[1] = 0.0009;
        double K = tan(M_PI * biquadBR[0]);
        double norm = 1.0 / (1.0 + K / biquadBR[1] + K * K);
        biquadAL[2] = biquadBL[2] = biquadAR[2] = biquadBR[2] = K / biquadBR[1] * norm;
        biquadAL[4] = biquadBL[4] = biquadAR[4] = biquadBR[4] = -biquadBR[2];
        biquadAL[5] = biquadBL[5] = biquadAR[5] = biquadBR[5] = 2.0 * (K * K - 1.0) * norm;
        biquadAL[6] = biquadBL[6] = biquadAR[6] = biquadBR[6] = (1.0 - K / biquadBR[1] + K * K) * norm;
        biquadCL[0] = biquadDL[0] = biquadCR[0] = biquadDR[0] = 0.032 / overallscale;
        biquadCL[1] = biquadDL[1] = biquadCR[1] = biquadDR[1] = 0.0007;
        K = tan(M_PI * biquadDR[0]);
        norm = 1.0 / (1.0 + K / biquadDR[1] + K * K);
        biquadCL[2] = biquadDL[2] = biquadCR[2] = biquadDR[2] = K / biquadDR[1] * norm;
        biquadCL[4] = biquadDL[4] = biquadCR[4] = biquadDR[4] = -biquadDR[2];
        biquadCL[5] = biquadDL[5] = biquadCR[5] = biquadDR[5] = 2.0 * (K * K - 1.0) * norm;
        biquadCL[6] = biquadDL[6] = biquadCR[6] = biquadDR[6] = (1.0 - K / biquadDR[1] + K * K) * norm;
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
            if (inputgain < 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            } // gain cut before anything, even dry
            double drySampleL = inputSampleL;
            double drySampleR = inputSampleR;
            double HighsSampleL = 0.0;
            double HighsSampleR = 0.0;
            double NonHighsSampleL = 0.0;
            double NonHighsSampleR = 0.0;
            double tempSample;
            if (flip)
            {
                iirMidRollerAL = (iirMidRollerAL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                iirMidRollerAR = (iirMidRollerAR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                HighsSampleL = inputSampleL - iirMidRollerAL;
                HighsSampleR = inputSampleR - iirMidRollerAR;
                NonHighsSampleL = iirMidRollerAL;
                NonHighsSampleR = iirMidRollerAR;
                iirHeadBumpAL += (inputSampleL * 0.05);
                iirHeadBumpAR += (inputSampleR * 0.05);
                iirHeadBumpAL -= (iirHeadBumpAL * iirHeadBumpAL * iirHeadBumpAL * HeadBumpFreq);
                iirHeadBumpAR -= (iirHeadBumpAR * iirHeadBumpAR * iirHeadBumpAR * HeadBumpFreq);
                iirHeadBumpAL = sin(iirHeadBumpAL);
                iirHeadBumpAR = sin(iirHeadBumpAR);
                tempSample = (iirHeadBumpAL * biquadAL[2]) + biquadAL[7];
                biquadAL[7] = (iirHeadBumpAL * biquadAL[3]) - (tempSample * biquadAL[5]) + biquadAL[8];
                biquadAL[8] = (iirHeadBumpAL * biquadAL[4]) - (tempSample * biquadAL[6]);
                iirHeadBumpAL = tempSample; // interleaved biquad
                if (iirHeadBumpAL > 1.0) {
                    iirHeadBumpAL = 1.0;
                }
                if (iirHeadBumpAL < -1.0) {
                    iirHeadBumpAL = -1.0;
                }
                iirHeadBumpAL = asin(iirHeadBumpAL);
                tempSample = (iirHeadBumpAR * biquadAR[2]) + biquadAR[7];
                biquadAR[7] = (iirHeadBumpAR * biquadAR[3]) - (tempSample * biquadAR[5]) + biquadAR[8];
                biquadAR[8] = (iirHeadBumpAR * biquadAR[4]) - (tempSample * biquadAR[6]);
                iirHeadBumpAR = tempSample; // interleaved biquad
                if (iirHeadBumpAR > 1.0) {
                    iirHeadBumpAR = 1.0;
                }
                if (iirHeadBumpAR < -1.0) {
                    iirHeadBumpAR = -1.0;
                }
                iirHeadBumpAR = asin(iirHeadBumpAR);
                inputSampleL = sin(inputSampleL);
                tempSample = (inputSampleL * biquadCL[2]) + biquadCL[7];
                biquadCL[7] = (inputSampleL * biquadCL[3]) - (tempSample * biquadCL[5]) + biquadCL[8];
                biquadCL[8] = (inputSampleL * biquadCL[4]) - (tempSample * biquadCL[6]);
                inputSampleL = tempSample; // interleaved biquad
                if (inputSampleL > 1.0) {
                    inputSampleL = 1.0;
                }
                if (inputSampleL < -1.0) {
                    inputSampleL = -1.0;
                }
                inputSampleL = asin(inputSampleL);
                inputSampleR = sin(inputSampleR);
                tempSample = (inputSampleR * biquadCR[2]) + biquadCR[7];
                biquadCR[7] = (inputSampleR * biquadCR[3]) - (tempSample * biquadCR[5]) + biquadCR[8];
                biquadCR[8] = (inputSampleR * biquadCR[4]) - (tempSample * biquadCR[6]);
                inputSampleR = tempSample; // interleaved biquad
                if (inputSampleR > 1.0) {
                    inputSampleR = 1.0;
                }
                if (inputSampleR < -1.0) {
                    inputSampleR = -1.0;
                }
                inputSampleR = asin(inputSampleR);
            }
            else {
                iirMidRollerBL = (iirMidRollerBL * (1.0 - RollAmount)) + (inputSampleL * RollAmount);
                iirMidRollerBR = (iirMidRollerBR * (1.0 - RollAmount)) + (inputSampleR * RollAmount);
                HighsSampleL = inputSampleL - iirMidRollerBL;
                HighsSampleR = inputSampleR - iirMidRollerBR;
                NonHighsSampleL = iirMidRollerBL;
                NonHighsSampleR = iirMidRollerBR;
                iirHeadBumpBL += (inputSampleL * 0.05);
                iirHeadBumpBR += (inputSampleR * 0.05);
                iirHeadBumpBL -= (iirHeadBumpBL * iirHeadBumpBL * iirHeadBumpBL * HeadBumpFreq);
                iirHeadBumpBR -= (iirHeadBumpBR * iirHeadBumpBR * iirHeadBumpBR * HeadBumpFreq);
                iirHeadBumpBL = sin(iirHeadBumpBL);
                iirHeadBumpBR = sin(iirHeadBumpBR);
                tempSample = (iirHeadBumpBL * biquadBL[2]) + biquadBL[7];
                biquadBL[7] = (iirHeadBumpBL * biquadBL[3]) - (tempSample * biquadBL[5]) + biquadBL[8];
                biquadBL[8] = (iirHeadBumpBL * biquadBL[4]) - (tempSample * biquadBL[6]);
                iirHeadBumpBL = tempSample; // interleaved biquad
                if (iirHeadBumpBL > 1.0) {
                    iirHeadBumpBL = 1.0;
                }
                if (iirHeadBumpBL < -1.0) {
                    iirHeadBumpBL = -1.0;
                }
                iirHeadBumpBL = asin(iirHeadBumpBL);
                tempSample = (iirHeadBumpBR * biquadBR[2]) + biquadBR[7];
                biquadBR[7] = (iirHeadBumpBR * biquadBR[3]) - (tempSample * biquadBR[5]) + biquadBR[8];
                biquadBR[8] = (iirHeadBumpBR * biquadBR[4]) - (tempSample * biquadBR[6]);
                iirHeadBumpBR = tempSample; // interleaved biquad
                if (iirHeadBumpBR > 1.0) {
                    iirHeadBumpBR = 1.0;
                }
                if (iirHeadBumpBR < -1.0) {
                    iirHeadBumpBR = -1.0;
                }
                iirHeadBumpBR = asin(iirHeadBumpBR);
                inputSampleL = sin(inputSampleL);
                tempSample = (inputSampleL * biquadDL[2]) + biquadDL[7];
                biquadDL[7] = (inputSampleL * biquadDL[3]) - (tempSample * biquadDL[5]) + biquadDL[8];
                biquadDL[8] = (inputSampleL * biquadDL[4]) - (tempSample * biquadDL[6]);
                inputSampleL = tempSample; // interleaved biquad
                if (inputSampleL > 1.0) {
                    inputSampleL = 1.0;
                }
                if (inputSampleL < -1.0) {
                    inputSampleL = -1.0;
                }
                inputSampleL = asin(inputSampleL);
                inputSampleR = sin(inputSampleR);
                tempSample = (inputSampleR * biquadDR[2]) + biquadDR[7];
                biquadDR[7] = (inputSampleR * biquadDR[3]) - (tempSample * biquadDR[5]) + biquadDR[8];
                biquadDR[8] = (inputSampleR * biquadDR[4]) - (tempSample * biquadDR[6]);
                inputSampleR = tempSample; // interleaved biquad
                if (inputSampleR > 1.0) {
                    inputSampleR = 1.0;
                }
                if (inputSampleR < -1.0) {
                    inputSampleR = -1.0;
                }
                inputSampleR = asin(inputSampleR);
            }
            flip = !flip;
            double groundSampleL = drySampleL - inputSampleL; // set up UnBox
            double groundSampleR = drySampleR - inputSampleR; // set up UnBox
            if (inputgain > 1.0) {
                inputSampleL *= inputgain;
                inputSampleR *= inputgain;
            } // gain boost inside UnBox: do not boost fringe audio
            double applySoften = fabs(HighsSampleL) * 1.57079633;
            if (applySoften > 1.57079633) {
                applySoften = 1.57079633;
            }
            applySoften = 1 - cos(applySoften);
            if (HighsSampleL > 0) {
                inputSampleL -= applySoften;
            }
            if (HighsSampleL < 0) {
                inputSampleL += applySoften;
            }
            // apply Soften depending on polarity
            applySoften = fabs(HighsSampleR) * 1.57079633;
            if (applySoften > 1.57079633) {
                applySoften = 1.57079633;
            }
            applySoften = 1 - cos(applySoften);
            if (HighsSampleR > 0) {
                inputSampleR -= applySoften;
            }
            if (HighsSampleR < 0) {
                inputSampleR += applySoften;
            }
            // apply Soften depending on polarity
            if (inputSampleL > 1.2533141373155) {
                inputSampleL = 1.2533141373155;
            }
            if (inputSampleL < -1.2533141373155) {
                inputSampleL = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            inputSampleL = sin(inputSampleL * fabs(inputSampleL)) / ((fabs(inputSampleL) == 0.0) ? 1 : fabs(inputSampleL));
            // Spiral, for cleanest most optimal tape effect
            if (inputSampleR > 1.2533141373155) {
                inputSampleR = 1.2533141373155;
            }
            if (inputSampleR < -1.2533141373155) {
                inputSampleR = -1.2533141373155;
            }
            // clip to 1.2533141373155 to reach maximum output
            inputSampleR = sin(inputSampleR * fabs(inputSampleR)) / ((fabs(inputSampleR) == 0.0) ? 1 : fabs(inputSampleR));
            // Spiral, for cleanest most optimal tape effect
            double suppress = (1.0 - fabs(inputSampleL)) * 0.00013;
            if (iirHeadBumpAL > suppress) {
                iirHeadBumpAL -= suppress;
            }
            if (iirHeadBumpAL < -suppress) {
                iirHeadBumpAL += suppress;
            }
            if (iirHeadBumpBL > suppress) {
                iirHeadBumpBL -= suppress;
            }
            if (iirHeadBumpBL < -suppress) {
                iirHeadBumpBL += suppress;
            }
            // restrain resonant quality of head bump algorithm
            suppress = (1.0 - fabs(inputSampleR)) * 0.00013;
            if (iirHeadBumpAR > suppress) {
                iirHeadBumpAR -= suppress;
            }
            if (iirHeadBumpAR < -suppress) {
                iirHeadBumpAR += suppress;
            }
            if (iirHeadBumpBR > suppress) {
                iirHeadBumpBR -= suppress;
            }
            if (iirHeadBumpBR < -suppress) {
                iirHeadBumpBR += suppress;
            }
            // restrain resonant quality of head bump algorithm
            inputSampleL += groundSampleL; // apply UnBox processing
            inputSampleR += groundSampleR; // apply UnBox processing
            inputSampleL += ((iirHeadBumpAL + iirHeadBumpBL) * bumpgain); // and head bump
            inputSampleR += ((iirHeadBumpAR + iirHeadBumpBR) * bumpgain); // and head bump
            if (lastSampleL >= 0.99)
            {
                if (inputSampleL < 0.99) {
                    lastSampleL = ((0.99 * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = 0.99;
                }
            }
            if (lastSampleL <= -0.99)
            {
                if (inputSampleL > -0.99) {
                    lastSampleL = ((-0.99 * softness) + (inputSampleL * (1.0 - softness)));
                }
                else {
                    lastSampleL = -0.99;
                }
            }
            if (inputSampleL > 0.99)
            {
                if (lastSampleL < 0.99) {
                    inputSampleL = ((0.99 * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = 0.99;
                }
            }
            if (inputSampleL < -0.99)
            {
                if (lastSampleL > -0.99) {
                    inputSampleL = ((-0.99 * softness) + (lastSampleL * (1.0 - softness)));
                }
                else {
                    inputSampleL = -0.99;
                }
            }
            lastSampleL = inputSampleL; // end ADClip L
            if (lastSampleR >= 0.99)
            {
                if (inputSampleR < 0.99) {
                    lastSampleR = ((0.99 * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = 0.99;
                }
            }
            if (lastSampleR <= -0.99)
            {
                if (inputSampleR > -0.99) {
                    lastSampleR = ((-0.99 * softness) + (inputSampleR * (1.0 - softness)));
                }
                else {
                    lastSampleR = -0.99;
                }
            }
            if (inputSampleR > 0.99)
            {
                if (lastSampleR < 0.99) {
                    inputSampleR = ((0.99 * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = 0.99;
                }
            }
            if (inputSampleR < -0.99)
            {
                if (lastSampleR > -0.99) {
                    inputSampleR = ((-0.99 * softness) + (lastSampleR * (1.0 - softness)));
                }
                else {
                    inputSampleR = -0.99;
                }
            }
            lastSampleR = inputSampleR; // end ADClip R
            if (inputSampleL > 0.99) {
                inputSampleL = 0.99;
            }
            if (inputSampleL < -0.99) {
                inputSampleL = -0.99;
            }
            // final iron bar
            if (inputSampleR > 0.99) {
                inputSampleR = 0.99;
            }
            if (inputSampleR < -0.99) {
                inputSampleR = -0.99;
            }
            // final iron bar
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
} // namespace airwindohhs::tape
