#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::biquad {

constexpr std::string_view k_name{ "Biquad" };
constexpr std::string_view k_short_description{
    "BiquadTriple is a handy Airwindows cascaded biquad filter: steeper roll-off before resonance."
};
constexpr std::string_view k_long_description{
    "This little puppy is really flexible!That’s true of any biquad filter, but I find most of them are really flexible and slightly plastic. Traditionally biquads require special handling, because you can’t push them too far before they break, and also they sound ‘DAW-y’ and synthetic. Not in the same way as linear-phase EQs, mind you, but still with something about them that feels digital.So I fixed that :) with a couple caveats. Firstly, part of it is brute force. I figured that if biquads run into trouble with 32-bit coefficients on some filter curves, and it’s widely known that you should do things like run ’em in series to get better behavior, we could always run long doubles and be totally sure they work as intended. Overkill? Meh. Sure, but it’s known that these break due to losing control of themselves due to not long enough wordlengths. Why wouldn’t they work and sound better with long enough wordlengths? And so they do. Also, you get to run simpler biquads if you don’t have to run lots of them, so it averages out. I think it’s just a matter of audio DSP coders stubbornly insisting on not using long data words on the grounds it doesn’t matter to the sound. This, when the filters go obviously wrong under some conditions for just that reason? Anyway, here’s long double biquad, because nobody else was doing it far as I know.The second part is trickier, because if you use it you remove some functionality. Biquads make great DC blockers. But, biquads also work through the summing of many delay taps, all very close together. Some are on input, others on output. They’re a mixing of multiple copies of the signal.That means Console applies to the biquad filter.We already knew that, of course: I’ve long observed that you can put Console (or BussColors4/ConsoleBuss4) around a bog-standard DAW EQ and get an enhanced sound out of it. Mind you, I wasn’t controlling that EQ, but it still worked. But now, I have Console5 (not 6, but I COULD do 6 in the right context) built right into Biquad. So, it does expect to have a signal between -1 and 1 (or lower than 0dBFS in the DAW), and that means Biquad itself can’t remove huge DC offsets ten times the size of the audio content. (It can still remove smaller offsets just fine). And it runs into basically PurestConsoleChannel, does the biquad, then uses PurestConsoleBuss and goes out to an inverse/dry/wet control (which itself has multiple uses).What’s a biquad, you ask? The Airwindows biquad uses four of the six common biquad options (I prefer to do shelves with subtractive/additive use of filtered audio). The top control has settings one through four. They go lowpass, highpass, bandpass and notch. That’s what I consider the platonic set of biquads, and it lets the filter design be simple and clear. You can set a frequency (in ‘amount of the audio range’) from ‘zero’ to ‘one’. There’ll be EQs with more specific frequencies, that’s easy, but this is proof of concept and to be used by ear, plus it will always cover DC to Nyquist at any sample rate. Zero is not really DC, because that kills the biquad: also, One is not really the Nyquist frequency (half the sample rate) for the same reason. But, they act a lot like they range that far, because the high resolution lets Biquad calculate things accurately even that far out of the normal range.They don’t update/recalculate every sample, but the way I’ve defined the data structures means they could if you wanted them to (at a cost in higher CPU-eating). The code’s MIT licensed open source, so GPL people can just take it if they credit me, and all the projects that are using the Airwindows library are advised to get up to date and include this one. Set right, it is THE ultimate sample-instrument tone shaper to sit ‘under the hood’ and voice somebody’s musical product after the sample-playback stuff is taken care of. You’ll be seeing a lot of stuff come out that uses this code, as there are many plugins that require this type of filter to work, often ones that will benefit from the sonic improvements that are part of Biquad.More will be revealed. Suffice to say this is a very useful building block that’ll allow for some very special plugins.This time, it’s time for the impossible!As in, there was a reason nobody was doing sweepable, ‘synthy’ biquad filters…Turns out it simply doesn’t work. One uses a different type, like state variable filters, for the synthy stuff. The reason is that, while biquads can sound pretty great (especially implemented like I do ’em), they fundamentally can’t cope with changing the filter coefficients mid-calculation. They flip out: you’ll hear some of that, especially at the frequency extremes. Low Q makes low frequency motions flip out, and high Q (and boy do I have a high Q for you this time) makes ultra-high frequency motions flip out in a really wild. glitchy way.So obviously I gave up.:Dnope! Instead, I just kind of forced the filter into zones where it mostly is controllable. This is partly through REALLY smoothing the filter cutoff, especially at low Q and low frequencies. If you try and update biquad coefficients every sample (and I already changed the form from the more CPU-efficient to the more stable form: didn’t help much at all) the algorithm gets super twitchy, so part of what I’ve done is just stabilizing things. I tried for ages to come up with some bizarre hack to force the filter back into stability: no dice. So, the range has been limited a little, the Q doesn’t adjust below a Q of 1, and it reacts really slowly, because that was the only way I could get it to behave at all. (the original Biquad can sometimes be better behaved, because it’s only recalculating coefficients every new buffer. So, zipper noise. When you get rid of that your troubles get infinitely worse, with biquads)I’ve also got the resonance (on low and highpass) working differently. I’m scaling back loudness, but I’m also applying a distortion, then averaging after that, to try and get an ‘overdriving filter effect’ not present in the purer original Biquad. You can go quieter into Biquad2 and avoid this, or not use as much resonance: I feel it’s kind of like the Roland Alpha Juno filter resonance distortion, not an everyday thing but when it pops up it has its own distinct quality that’s interesting.Anyway here’s Biquad2 ;)So it’s a boring old biquad.Except, if you’re into the details of how stuff works, it’s not THAT boring. It’s two biquad filters stacked, which means you can get twice as much cutoff steepness before resonance. Since it’s Airwindows, you can have any possible resonance setting anyway, from ‘impossibly high’ to so wide that it’s basically full bandwidth. That interacts interestingly with the stacked filters: some of the weird effects with super-wide filters will act different here. On top of that, it’s two stacked filters inside an internal Console mixing system: so the tone of the filter itself is ‘expanded’ in a way not common to boring old biquad filters. And it’s the most efficient (but least cooperative) biquad, meaning that it runs and sounds great but doesn’t always cooperate with rapid automation, so that might get interesting if you’re not careful.Why do this at all?Because I sometimes like mocking up effects and plugins out of component parts. And if I was to use a stacked biquad filter as part of something else, it’s just more convenient to dial in the correct settings and get the tone exactly right: I could use two ‘Biquad’ instances, but remember that BiquadDouble stacks its filters INSIDE the Console processing, so it might not be as good to fake it, plus I’d have to set both the Biquad instances the same…The real answer is ‘because that’s how I roll’. Nobody asked for this. But maybe you reach for a biquad filter for simple tone shaping, and you keep trying to find a butter zone between too shallow, and too resonant. This might become your go-to basic filter. I can’t predict what will take off: for all I know, this is THE basic digital filter everyone’s been waiting for, the one that just sounds right for every purpose. Or not. But you can’t know until you try :)A biquad filter is a very basic piece of typical DAW EQs. If you were going to think of the most ordinary, normal, non-special digital EQ that would sound like every other digital EQ, you're probably going to be stacking up biquad filters. It is the most boring of digital filters, at least ones that are kind of flexible.Let's break it!BiquadNonLin takes the traditional biquad filter, complete with cutoff and Q factor, and applies the same technique used in Capacitor2. That was the one where I read about Murata ceramic capacitors showing striking variations in value, based upon how much voltage pressure was against them. Which is not to say all capacitors do that… but knowing this has happened in the real world suggests that it's an avenue to pursue. And so, Capacitor2 brought in this interesting nonlinearity based on real-world stuff, like a (in fact literally a) frequency modulation… and then it sat for a while. Because how would you even apply that to a carefully worked out algorithm like a biquad?The answer is 'you just do'. Does it produce a clean result? Nope. You can break things pretty thoroughly, especially up near the Nyquist frequency. But you can apply less… I find 0.5 tends to work quite well… and you can get the same kind of nonlinearity.It's just that this time, you can do it on a steep, resonant filter, and get that more analog texture on something that's otherwise very controllable. I'm looking to use this on many things: I think it'll make for a better-sounding distributed ultrasonic filter, and I'd also like to make a multistage version to use as the guts of a future ConsoleSL… because it should work more like a steeper but less vibey filter, except with the analogification.Anyway, here's BiquadNonLin: one biquad filter, lightly broken for your sonic pleasure. Type at 0 is a lowpass, Type at 1 is a bandpass. The biquad tended to explode when I tried to make it do highpasses or notches, but it turned out I could just make the dry/wet an Inv/Wet control, so that means you just use Inv instead, and Type at 0 becomes the highpass (it's subtracted) and Type at 1 is the notch. If you have a hard time hearing the nonlinearity, setting it to highpass and sweeping it up at high NonLin should make it real obvious.Oh, one more thing…Today, the first VST3 is out… kind of. I have it up on GitHub at https://github.com/airwindows/Meter and you see it in the video for BiquadDouble. The thing is, I can't build it anywhere else except a single VST3 on my own machine, for Mac Apple Silicon only. The project on GitHub is all the code, up to date, and it's supposed to be able to build all sorts of different versions.You can fork it, you can download it, you can try to get it to compile just like I did. There's lots of people smarter than me about computers and they didn't make this meter, so I had to. So, if you can make a Pamplejuce project compile, Meter is yours today, no matter what sort of computer you're on or what platform you're targeting. VST3? AU for iPad? AAX? Knock yourself out.If you get anything to work, tell me how you did it, and everyone can have the result. Otherwise, I will keep plugging away, and eventually I'll get this stuff sorted out. But if you're impatient… the GPLed code is there, it compiled for me, let me know how you do :)BiquadOneHalf is tech that’s going into my future plugins. You have to set the frequency twice as high as you would using Biquad, and it can only go up to 1/4 Nyquist, but the way it interacts with the sampling frequency is special and works very well for some types of filtering. You’ll be getting more developed plugins of that nature soon, and all through January, but for now I got this: working version of the new Biquad filter variation, that you can use on stuff.It’s the first version, not the ‘sweepable’ version, because this is going to find its way into fixed-frequency EQs, ‘head bump’ etc… compare it to Biquad remembering to use twice the frequency you otherwise would. It’s happiest on bass or midrange: much like regular Biquad, trying to force it to get real close to its high frequency limit can get messy.By request, a kind of bugfix!Understand, a simpler implementation of a biquad filter isn’t a ‘bug’, exactly. I may have not had everything figured out, but if you’re designing fixed filters or looking to tune in EQs on something in your mix, the biquad filters I’ve been making are actually better. Without the extra smoothing code they run more efficiently and eat less CPU, and they’re still useful, plus there’s less to them so they’re more approachable.But, a lot of musicians seem to enjoy cranking the filters around… and now that I’m using Bespoke all the time, I can put an LFO literally on any damn thing by rightclicking it. And it wasn’t all that difficult to do… I rolled it in to some documentation upgrades I did on the advice of Paul from Surge Synthesizer. One thing about hanging out with the open source music people is, stuff starts happening faster than you could possibly imagine. I had a crash bug in Bespoke on some strange things I needed to build into my main music making procedures, and Ryan had a fix the next MORNING. It’s daunting and wonderful to hang out with these earnest, motivated people.You’ll see more on that, sooner than you think.But right now… enjoy a cleaner, smoother Biquad, plus zipper noise suppression! I needed to come up with my Airwindows way to accomplish this, and got it done. More to come.Here’s another utility plugin! This is the same as BiquadDouble, except it’s three stacked biquads. Otherwise the same: you get to set the frequency, Q etc. for all the filters in parallel. This saves time over doing three instances of Biquad, plus they’re run in series inside Console inside the plugin, so there will be a deeper tone out of this one.It’s for if you wanted to get steeper filter slopes without the cutoff going resonant. This, like BiquadDouble, is very much ‘tools to make tools’, for if you’re mocking up some kind of effect and know you’re going to be using filters. You can test out what you’ll use in your effect, this way. Note: seems like the inv/dry/wet is really just wet/dry/wet, and I have too many new and more worthwhile plugins to put out to fuss over it so I’d advise just taking BiquadTriple as it is. I’d be using it as full-wet anyway though you can bleed in dry for more of a shelf effect if you’re doing a steep rolloff of some kind.I expect to be doing some more variations on this as there are known ways to stack filters where the Q factors are very specific and staggered: however, those will be more aimed at the mixer/end user, and probably will be called isolators (that being a common term for this type of steep crossover-like filter). And again, I’ve got other things to put out that are more interesting, so more will be revealed as I do those experiments. I think an Airwindows isolator-style effect would be good. Bear in mind the biquads can be fussy about extreme settings and moving the sliders abruptly."
};
constexpr std::string_view k_tags{
    "biquads"
};

template <typename T>
class Biquad final : public Effect<T>
{
    double biquad[11]; // note that this stereo form doesn't require L and R forms!
    // This is because so much of it is coefficients etc. that are the same on both channels.
    // So the stored samples are in 7-8 and 9-10, and freq/res/coefficients serve both.
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    Biquad()
    {
        for (int x = 0; x < 11; x++) {
            biquad[x] = 0.0;
        }
        A = 1.0;
        B = 0.5;
        C = 0.5;
        D = 1.0;
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
        kParamfor (int x = 0,
kParamA = 1,
kParamB = 2,
kParamC = 3,
kNumParameters = 4

    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: for (int x = value; break;
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
        case kParamfor (int x: return for (int x;
case kParamA: return A;
case kParamB: return B;
case kParamC: return C;

        default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return 0;
        case kParamA: return 1.0;
        case kParamB: return 0.5;
        case kParamC: return 0.5;

        default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "type";
        case kParamA: return "freq";
        case kParamB: return "q";
        case kParamC: return "invwet";

        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "Type";
        case kParamA: return "Freq";
        case kParamB: return "Q";
        case kParamC: return "Inv/Wet";

        default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return std::to_string((A*3.999)+0.00001));
        case kParamA: return std::to_string((B * B * B * 0.9999) + 0.0001);
        case kParamB: return std::to_string((C * C * C * 29.99) + 0.01);
        case kParamC: return std::to_string((D * 2.0) - 1.0);

        default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        case kParamfor (int x: return "";
        case kParamA: return "";
        case kParamB: return "";
        case kParamC: return "";
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
        int type = ceil((A * 3.999) + 0.00001);
        biquad[0] = ((B * B * B * 0.9999) + 0.0001) * 0.499;
        if (biquad[0] < 0.0001) {
            biquad[0] = 0.0001;
        }
        biquad[1] = (C * C * C * 29.99) + 0.01;
        if (biquad[1] < 0.0001) {
            biquad[1] = 0.0001;
        }
        double wet = (D * 2.0) - 1.0;
        // biquad contains these values:
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        //[2] is a0 but you need distinct ones for additional biquad instances so it's here
        //[3] is a1 but you need distinct ones for additional biquad instances so it's here
        //[4] is a2 but you need distinct ones for additional biquad instances so it's here
        //[5] is b1 but you need distinct ones for additional biquad instances so it's here
        //[6] is b2 but you need distinct ones for additional biquad instances so it's here
        //[7] is LEFT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[8] is LEFT stored delayed sample (you have to include the coefficient making code if you do that)
        //[9] is RIGHT stored delayed sample (freq and res are stored so you can move them sample by sample)
        //[10] is RIGHT stored delayed sample (you have to include the coefficient making code if you do that)
        // to build a dedicated filter, rename 'biquad' to whatever the new filter is, then
        // put this code either within the sample buffer (for smoothly modulating freq or res)
        // or in this 'read the controls' area (for letting you change freq and res with controls)
        // or in 'reset' if the freq and res are absolutely fixed (use GetSampleRate to define freq)
        if (type == 1) { // lowpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K * K * norm;
            biquad[3] = 2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 2) { // highpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = norm;
            biquad[3] = -2.0 * biquad[2];
            biquad[4] = biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 3) { // bandpass
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = K / biquad[1] * norm;
            biquad[3] = 0.0; // bandpass can simplify the biquad kernel: leave out this multiply
            biquad[4] = -biquad[2];
            biquad[5] = 2.0 * (K * K - 1.0) * norm;
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
        if (type == 4) { // notch
            double K = tan(M_PI * biquad[0]);
            double norm = 1.0 / (1.0 + K / biquad[1] + K * K);
            biquad[2] = (1.0 + K * K) * norm;
            biquad[3] = 2.0 * (K * K - 1) * norm;
            biquad[4] = biquad[2];
            biquad[5] = biquad[3];
            biquad[6] = (1.0 - K / biquad[1] + K * K) * norm;
        }
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
            inputSampleL = sin(inputSampleL);
            inputSampleR = sin(inputSampleR);
            // encode Console5: good cleanness
            /*
             double mid = inputSampleL + inputSampleR;
             double side = inputSampleL - inputSampleR;
             //assign mid and side.Between these sections, you can do mid/side processing
             double tempSampleM = (mid * biquad[2]) + biquad[7];
             biquad[7] = (mid * biquad[3]) - (tempSampleM * biquad[5]) + biquad[8];
             biquad[8] = (mid * biquad[4]) - (tempSampleM * biquad[6]);
             mid = tempSampleM; //like mono AU, 7 and 8 store mid channel
             double tempSampleS = (side * biquad[2]) + biquad[9];
             biquad[9] = (side * biquad[3]) - (tempSampleS * biquad[5]) + biquad[10];
             biquad[10] = (side * biquad[4]) - (tempSampleS * biquad[6]);
             inputSampleR = tempSampleS; //note: 9 and 10 store the side channel
             inputSampleL = (mid+side)/2.0;
             inputSampleR = (mid-side)/2.0;
             //unassign mid and side
             */
            double tempSampleL = (inputSampleL * biquad[2]) + biquad[7];
            biquad[7] = (inputSampleL * biquad[3]) - (tempSampleL * biquad[5]) + biquad[8];
            biquad[8] = (inputSampleL * biquad[4]) - (tempSampleL * biquad[6]);
            inputSampleL = tempSampleL; // like mono AU, 7 and 8 store L channel
            double tempSampleR = (inputSampleR * biquad[2]) + biquad[9];
            biquad[9] = (inputSampleR * biquad[3]) - (tempSampleR * biquad[5]) + biquad[10];
            biquad[10] = (inputSampleR * biquad[4]) - (tempSampleR * biquad[6]);
            inputSampleR = tempSampleR; // note: 9 and 10 store the R channel
            if (inputSampleL > 1.0) {
                inputSampleL = 1.0;
            }
            if (inputSampleL < -1.0) {
                inputSampleL = -1.0;
            }
            if (inputSampleR > 1.0) {
                inputSampleR = 1.0;
            }
            if (inputSampleR < -1.0) {
                inputSampleR = -1.0;
            }
            // without this, you can get a NaN condition where it spits out DC offset at full blast!
            inputSampleL = asin(inputSampleL);
            inputSampleR = asin(inputSampleR);
            // amplitude aspect
            if (wet < 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - fabs(wet)));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - fabs(wet)));
                // inv/dry/wet lets us turn LP into HP and band into notch
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
} // namespace airwindohhs::biquad
