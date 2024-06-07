#pragma once
#include "effect.hpp"
#include <cstdlib>

namespace airwindohhs::brightambience3 {

constexpr std::string_view k_name{ "BrightAmbience3" };
constexpr std::string_view k_short_description{
    "BrightAmbience3 adds undersampling for high sample rates, and better feedback."
};
constexpr std::string_view k_long_description{
    "BrightAmbience is a very old plugin. The original was all about taking sounds coming in, and transforming them into lengths of extruded and very bright reverb. BrightAmbience2 transformed that, in turn, into a more adaptable creation that used inter-aural delays to create a subtle stereo effect like an aura around mono content.BrightAmbience3 adds undersampling. Now high sample rate mixes retain a consistent tone and reverb length to what the CD-rate plugin would do… and it’s more CPU-efficient running at the elevated rates… and the subtle darkening in tone makes it worth a re-listen.But now, on top of all that, we’ve got a new way to apply feedback at the ‘wider’ reverb settings, which allows you to feed THOSE back too. And that means, BrightAmbience3 has just taken on a new life for a variety of vibey, distinctly analog-feeling blurred delay effects. Even the really wide reverb settings will still feed back at full crank (though they just give you a sort of droney resonant quality) and the medium settings produce a variety of unusual sounds that are a little bit like when you have a crummy old antique echo effect, and it has no clarity, but when you turn up the feedback strong retro flavors begin to take over… you can’t get clean infinite regeneration that way, but tune it to taste and dial back the feedback control until you have enough echo for your purposes.Or, ignore the feedback and just use it as BrightAmbience, but with a greater range of effect at higher sample rates, and a richer tone thanks to the undersampling."
};
constexpr std::string_view k_tags{
    "ambience"
};

static int primeL[] = { 5, 5, 13, 17, 37, 41, 61, 67, 89, 109, 131, 157, 181, 191, 223, 241, 281, 283, 337, 353, 373, 401, 433, 461, 521, 547, 569, 587, 601, 617, 719, 739, 787, 797, 863, 877, 929, 967, 997, 1031, 1069, 1087, 1163, 1171, 1213, 1217, 1301, 1409, 1439, 1447, 1481, 1499, 1531, 1597, 1627, 1669, 1733, 1741, 1789, 1823, 1861, 1913, 2029, 2063, 2083, 2099, 2237, 2269, 2347, 2351, 2383, 2417, 2503, 2549, 2617, 2647, 2687, 2719, 2753, 2803, 2903, 2909, 3011, 3019, 3079, 3109, 3181, 3229, 3271, 3299, 3323, 3407, 3491, 3517, 3571, 3593, 3643, 3733, 3767, 3911, 3947, 4027, 4093, 4133, 4157, 4217, 4283, 4339, 4409, 4421, 4481, 4517, 4561, 4567, 4673, 4759, 4789, 4801, 4889, 4933, 4951, 5021, 5077, 5107, 5197, 5281, 5387, 5441, 5507, 5557, 5639, 5651, 5711, 5749, 5807, 5851, 5879, 6037, 6121, 6217, 6247, 6311, 6329, 6353, 6367, 6469, 6607, 6653, 6673, 6691, 6827, 6841, 6869, 6899, 7069, 7109, 7207, 7283, 7369, 7417, 7487, 7523, 7621, 7649, 7703, 7753, 7853, 7883, 8017, 8059, 8111, 8117, 8231, 8233, 8291, 8377, 8419, 8513, 8537, 8581, 8731, 8747, 8779, 8807, 8861, 8923, 9001, 9041, 9109, 9293, 9323, 9403, 9463, 9539, 9623, 9661, 9743, 9833, 9871, 9923, 10007, 10009, 10091, 10169, 10271, 10433, 10459, 10487, 10567, 10589, 10639, 10663, 10691, 10723, 10859, 10861, 10937, 11257, 11317, 11369, 11467, 11633, 11777, 11867, 11923, 11927, 11959, 12007, 12101, 12113, 12149, 12203, 12323, 12409, 12433, 12457, 12487, 12503, 12553, 12647, 12781, 12841, 12967, 13003, 13043, 13103, 13177, 13217, 13307, 13331, 13477, 13513, 13597, 13613, 13669, 13693, 13711, 13757, 13873, 14051, 14143, 14159, 14197, 14437, 14489, 14503, 14593, 14713, 14731, 14783, 14869, 14923, 14983, 15061, 15233, 15271, 15307, 15313, 15427, 15511, 15643, 15683, 15859, 15973, 16063, 16073, 16097, 16127, 16183, 16253, 16417, 16451, 16529, 16693, 16729, 16901, 16927, 17117, 17191, 17291, 17341, 17377, 17389, 17417, 17489, 17539, 17657, 17659, 17783, 17911, 17989, 18049, 18169, 18181, 18223, 18229, 18313, 18433, 18451, 18617, 18671, 18719, 18773, 18787, 18919, 19013, 19219, 19433, 19469, 19501, 19583, 19759, 19793, 19819, 19919, 20047, 20071, 20107, 20173, 20231, 20323, 20341, 20443, 20477, 20731, 20759, 20789, 20873, 20903, 20959, 21101, 21149, 21187, 21191, 21277, 21317, 21383, 21493, 21557, 21587, 21737, 21757, 21821, 21937, 22031, 22067, 22109, 22367, 22567, 22651, 22727, 22751, 22817, 22853, 22921, 23087, 23227, 23251, 23447, 23539, 23567, 23669, 23813, 23887, 23909, 23929, 24023, 24071, 24109, 24137, 24151, 24203, 24251, 24391, 24419, 24443, 24509, 24659, 24671, 24793, 24859, 24919, 25057, 25169, 25301, 25309, 25357, 25411, 25423, 25603, 25733, 25771, 25841, 25931, 25969, 26017, 26189, 26267, 26371, 26431, 26489, 26597, 26693, 26801, 26921, 26959, 27017, 27077, 27091, 27449, 27457, 27583, 27689, 27737, 27809, 27851, 27943, 28069, 28109, 28283, 28307, 28403, 28573, 28649, 28657, 28813, 29101, 29147, 29153, 29287, 29333, 29387, 29483, 29573, 29641, 29717, 29803, 30089, 30091, 30119, 30133, 30259, 30557, 30593, 30661, 30713, 30781, 30839, 30869, 30893, 31033, 31079, 31181, 31193, 31267, 31307, 31489, 31517, 31667, 31741, 32003, 32159, 32233, 32297, 32299, 32327, 32341, 32537, 32603, 32749 };
static int primeR[] = { 3, 7, 11, 19, 31, 43, 59, 71, 83, 113, 127, 163, 179, 193, 211, 251, 277, 293, 331, 359, 367, 409, 431, 463, 509, 557, 563, 593, 599, 619, 709, 743, 773, 809, 859, 881, 919, 971, 991, 1033, 1063, 1091, 1153, 1181, 1201, 1223, 1297, 1423, 1433, 1451, 1471, 1511, 1523, 1601, 1621, 1693, 1723, 1747, 1787, 1831, 1847, 1931, 2027, 2069, 2081, 2111, 2221, 2273, 2341, 2357, 2381, 2423, 2477, 2551, 2609, 2657, 2683, 2729, 2749, 2819, 2897, 2917, 3001, 3023, 3067, 3119, 3169, 3251, 3259, 3301, 3319, 3413, 3469, 3527, 3559, 3607, 3637, 3739, 3761, 3917, 3943, 4049, 4091, 4139, 4153, 4219, 4273, 4349, 4397, 4423, 4463, 4519, 4549, 4583, 4663, 4783, 4787, 4813, 4877, 4937, 4943, 5023, 5059, 5113, 5189, 5297, 5381, 5443, 5503, 5563, 5623, 5653, 5701, 5779, 5801, 5857, 5869, 6043, 6113, 6221, 6229, 6317, 6323, 6359, 6361, 6473, 6599, 6659, 6661, 6701, 6823, 6857, 6863, 6907, 7057, 7121, 7193, 7297, 7351, 7433, 7481, 7529, 7607, 7669, 7699, 7757, 7841, 7901, 8011, 8069, 8101, 8123, 8221, 8237, 8287, 8387, 8389, 8521, 8527, 8597, 8719, 8753, 8761, 8819, 8849, 8929, 8999, 9043, 9103, 9311, 9319, 9413, 9461, 9547, 9619, 9677, 9739, 9839, 9859, 9929, 9973, 10037, 10079, 10177, 10267, 10453, 10457, 10499, 10559, 10597, 10631, 10667, 10687, 10729, 10853, 10867, 10909, 11261, 11311, 11383, 11447, 11657, 11743, 11887, 11909, 11933, 11953, 12011, 12097, 12119, 12143, 12211, 12301, 12413, 12421, 12473, 12479, 12511, 12547, 12653, 12763, 12853, 12959, 13007, 13037, 13109, 13171, 13219, 13297, 13337, 13469, 13523, 13591, 13619, 13649, 13697, 13709, 13759, 13859, 14057, 14107, 14173, 14177, 14447, 14479, 14519, 14591, 14717, 14723, 14797, 14867, 14929, 14969, 15073, 15227, 15277, 15299, 15319, 15413, 15527, 15641, 15727, 15823, 15991, 16061, 16087, 16091, 16139, 16141, 16267, 16411, 16453, 16519, 16699, 16703, 16903, 16921, 17123, 17189, 17293, 17333, 17383, 17387, 17419, 17483, 17551, 17627, 17669, 17761, 17921, 17987, 18059, 18149, 18191, 18217, 18233, 18311, 18439, 18443, 18637, 18661, 18731, 18757, 18793, 18917, 19031, 19213, 19441, 19463, 19507, 19577, 19763, 19777, 19841, 19913, 20051, 20063, 20113, 20161, 20233, 20297, 20347, 20441, 20479, 20719, 20771, 20773, 20879, 20899, 20963, 21089, 21157, 21179, 21193, 21269, 21319, 21379, 21499, 21529, 21589, 21727, 21767, 21817, 21943, 22027, 22073, 22093, 22369, 22549, 22669, 22721, 22769, 22811, 22859, 22907, 23099, 23209, 23269, 23431, 23549, 23563, 23671, 23801, 23893, 23899, 23957, 24019, 24077, 24107, 24133, 24169, 24197, 24281, 24379, 24421, 24439, 24517, 24631, 24677, 24781, 24877, 24917, 25073, 25163, 25303, 25307, 25367, 25409, 25439, 25601, 25741, 25763, 25847, 25919, 25981, 26003, 26203, 26263, 26387, 26423, 26497, 26591, 26699, 26783, 26927, 26953, 27031, 27073, 27103, 27437, 27479, 27581, 27691, 27733, 27817, 27847, 27947, 28057, 28111, 28279, 28309, 28393, 28579, 28643, 28661, 28807, 29123, 29137, 29167, 29269, 29339, 29383, 29501, 29569, 29663, 29683, 29819, 30071, 30097, 30113, 30137, 30253, 30559, 30577, 30671, 30707, 30803, 30829, 30871, 30881, 31039, 31069, 31183, 31189, 31271, 31277, 31511, 31513, 31687, 31729, 32009, 32143, 32237, 32261, 32303, 32323, 32353, 32533, 32609, 32719 };
// these arrays go from 0 to primeL[489] = 32719 which is almost 32767

template <typename T>
class BrightAmbience3 final : public Effect<T>
{
    int gcount;
    double pL[32768];
    double pR[32768];
    double feedbackA;
    double feedbackB;
    double figureL[9];
    double figureR[9];
    double lastRefL[10];
    double lastRefR[10];
    int cycle;
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff
    float A;
    float B;
    float C;
    float D;

  public:
    BrightAmbience3()
    {
        A = 1.0;
        B = 0.0;
        C = 0.0;
        D = 1.0;
        for (int count = 0; count < 32767; count++) {
            pL[count] = 0.0;
            pR[count] = 0.0;
        }
        feedbackA = feedbackB = 0.0;
        for (int x = 0; x < 9; x++) {
            figureL[x] = 0.0;
            figureR[x] = 0.0;
        }
        for (int c = 0; c < 9; c++) {
            lastRefL[c] = 0.0;
            lastRefR[c] = 0.0;
        }
        cycle = 0;
        gcount = 0;
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
        kParamD = 3,
        kNumParameters = 4
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            A = value;
            break;
        kParamB:
            B = value;
            break;
        kParamC:
            C = value;
            break;
        kParamD:
            D = value;
            break;

            default: break;
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return A;
            break;
        kParamB:
            return B;
            break;
        kParamC:
            return C;
            break;
        kParamD:
            return D;
            break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return 1.0;
            break;
        kParamB:
            return 0.0;
            break;
        kParamC:
            return 0.0;
            break;
        kParamD:
            return 1.0;
            break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "start";
            break;
        kParamB:
            return "length";
            break;
        kParamC:
            return "feedback";
            break;
        kParamD:
            return "dry/wet";
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "Start";
            break;
        kParamB:
            return "Length";
            break;
        kParamC:
            return "Feedback";
            break;
        kParamD:
            return "Dry/Wet";
            break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return std::to_string(A);
            break;
        kParamB:
            return std::to_string(B);
            break;
        kParamC:
            return std::to_string(C);
            break;
        kParamD:
            return std::to_string(D);
            break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_label(int index) const
    {
        switch (static_cast<params>(index))
        {
        kParamA:
            return "";
            break;
        kParamB:
            return "";
            break;
        kParamC:
            return "";
            break;
        kParamD:
            return "";
            break;

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
        int cycleEnd = floor(overallscale);
        if (cycleEnd < 1) {
            cycleEnd = 1;
        }
        if (cycleEnd > 4) {
            cycleEnd = 4;
        }
        // this is going to be 2 for 88.1 or 96k, 3 for silly people, 4 for 176 or 192k
        if (cycle > cycleEnd - 1) {
            cycle = cycleEnd - 1; // sanity check
        }
        int start = (int)(A * 400) + 88;
        int length = (int)(pow(B, 2) * 487) + 1;
        if (start + length > 488) {
            start = 488 - length;
        }
        double feedbackAmount = C * 0.25;
        double wet = D;
        //[0] is frequency: 0.000001 to 0.499999 is near-zero to near-Nyquist
        //[1] is resonance, 0.7071 is Butterworth. Also can't be zero
        figureL[0] = figureR[0] = 1000.0 / Effect<T>::getSampleRate(); // fixed frequency, 3.515775k
        figureL[1] = figureR[1] = pow(length * 0.037 * feedbackAmount, 2) + 0.01; // resonance
        double K = tan(M_PI * figureR[0]);
        double norm = 1.0 / (1.0 + K / figureR[1] + K * K);
        figureL[2] = figureR[2] = K / figureR[1] * norm;
        figureL[4] = figureR[4] = -figureR[2];
        figureL[5] = figureR[5] = 2.0 * (K * K - 1.0) * norm;
        figureL[6] = figureR[6] = (1.0 - K / figureR[1] + K * K) * norm;
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
            cycle++;
            if (cycle == cycleEnd) { // hit the end point and we do an Air sample
                double tempL = 0.0;
                double tempR = 0.0;
                if (gcount < 0 || gcount > 32767) {
                    gcount = 32767;
                }
                int count = gcount;
                pL[count] = inputSampleL + feedbackB;
                pR[count] = inputSampleR + feedbackA;
                for (int offset = start; offset < start + length; offset++) {
                    tempL += pL[count + primeL[offset] - ((count + primeL[offset] > 32767) ? 32768 : 0)];
                    tempR += pR[count + primeR[offset] - ((count + primeR[offset] > 32767) ? 32768 : 0)];
                }
                inputSampleL = tempL / cbrt(length);
                inputSampleR = tempR / cbrt(length);
                double tempSample = (inputSampleL * figureL[2]) + figureL[7];
                figureL[7] = -(tempSample * figureL[5]) + figureL[8];
                figureL[8] = (inputSampleL * figureL[4]) - (tempSample * figureL[6]);
                feedbackA = sin(tempSample) * feedbackAmount;
                tempSample = (inputSampleR * figureR[2]) + figureR[7];
                figureR[7] = -(tempSample * figureR[5]) + figureR[8];
                figureR[8] = (inputSampleR * figureR[4]) - (tempSample * figureR[6]);
                feedbackB = sin(tempSample) * feedbackAmount;
                gcount--;
                if (cycleEnd == 4) {
                    lastRefL[0] = lastRefL[4]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[1] = (lastRefL[0] + lastRefL[2]) / 2; // one quarter
                    lastRefL[3] = (lastRefL[2] + inputSampleL) / 2; // three quarters
                    lastRefL[4] = inputSampleL; // full
                    lastRefR[0] = lastRefR[4]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[1] = (lastRefR[0] + lastRefR[2]) / 2; // one quarter
                    lastRefR[3] = (lastRefR[2] + inputSampleR) / 2; // three quarters
                    lastRefR[4] = inputSampleR; // full
                }
                if (cycleEnd == 3) {
                    lastRefL[0] = lastRefL[3]; // start from previous last
                    lastRefL[2] = (lastRefL[0] + lastRefL[0] + inputSampleL) / 3; // third
                    lastRefL[1] = (lastRefL[0] + inputSampleL + inputSampleL) / 3; // two thirds
                    lastRefL[3] = inputSampleL; // full
                    lastRefR[0] = lastRefR[3]; // start from previous last
                    lastRefR[2] = (lastRefR[0] + lastRefR[0] + inputSampleR) / 3; // third
                    lastRefR[1] = (lastRefR[0] + inputSampleR + inputSampleR) / 3; // two thirds
                    lastRefR[3] = inputSampleR; // full
                }
                if (cycleEnd == 2) {
                    lastRefL[0] = lastRefL[2]; // start from previous last
                    lastRefL[1] = (lastRefL[0] + inputSampleL) / 2; // half
                    lastRefL[2] = inputSampleL; // full
                    lastRefR[0] = lastRefR[2]; // start from previous last
                    lastRefR[1] = (lastRefR[0] + inputSampleR) / 2; // half
                    lastRefR[2] = inputSampleR; // full
                }
                if (cycleEnd == 1) {
                    lastRefL[0] = inputSampleL;
                    lastRefR[0] = inputSampleR;
                }
                cycle = 0; // reset
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
            }
            else {
                inputSampleL = lastRefL[cycle];
                inputSampleR = lastRefR[cycle];
                // we are going through our references now
            }
            switch (cycleEnd) // multi-pole average using lastRef[] variables
            {
                case 4:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[7]) * 0.5;
                    lastRefL[7] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[7]) * 0.5;
                    lastRefR[7] = lastRefR[8]; // continue, do not break
                case 3:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[6]) * 0.5;
                    lastRefL[6] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[6]) * 0.5;
                    lastRefR[6] = lastRefR[8]; // continue, do not break
                case 2:
                    lastRefL[8] = inputSampleL;
                    inputSampleL = (inputSampleL + lastRefL[5]) * 0.5;
                    lastRefL[5] = lastRefL[8]; // continue, do not break
                    lastRefR[8] = inputSampleR;
                    inputSampleR = (inputSampleR + lastRefR[5]) * 0.5;
                    lastRefR[5] = lastRefR[8]; // continue, do not break
                case 1:
                    break; // no further averaging
            }
            if (wet != 1.0) {
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0 - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0 - wet));
            }
            // Dry/Wet control, defaults to the last slider
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
            in1++;
            in2++;
            out1++;
            out2++;
        }
    }
};
} // namespace airwindohhs::brightambience3
