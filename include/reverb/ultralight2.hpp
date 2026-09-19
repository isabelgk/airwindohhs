#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::ultralight2 {

constexpr std::string_view k_name{ "Ultralight2" };
constexpr std::string_view k_short_description{
    "Ultralight2 is more of the most low-CPU matrix reverb I can make."
};
constexpr std::string_view k_long_description{
    "This plugin continues work I'm doing, trying to make working demos of ideas for the Godot game engine.Much like FastDistance is a filter that (when not pushed to really heavy attenuation of brightness) is way more efficient than using a DF1 biquad filter, Ultralight2 is a reverb that's way more efficient than using any of my other reverbs. Why? Because it's extremely limited. There's nearly nothing to it, just a 4x4 Householder matrix and some tone shaping and a teeny allpass chain that runs in parallel to the predelay.Why? Because I want to put dozens, maybe hundreds of them, into a game… and have the game still run and have room for graphics and gameplay.This would involve placing them as ultrasimple mono instances, each with its own delay times and settings, in a scene. They'd all get fed all of the sounds in the scene, and then your environment would be composed of all the distinct spaces sounding off at once.The controls have changed a bit! I had to do a bugfix on Ultralight anyway (redownload it if you want the tone shaping to work fully as intended). Now, rather than making part of the first delays turn into allpasses, I did further experimenting. No wonder I never liked allpasses, they sound awful unless the delay time is so quick it's inside the 'Haas effect' region where really rapid echoes blur and merge… without that, they're useless. Now all the matrix delays are normal reverb delays (which also means they can run even faster in SIMD) and then there's another thing that goes in front…Turns out you can use exactly the same sequence of operations to do an allpass, and a simple delay line. THAT means it can be SIMD (single instruction, multiple data). And it turns out if you set these things up right (never mind loops, just do things in fours) compilers really do optimize for them if they can. I learned that when some AMD users on retro machines got wildly worse performance than they should: your processor has to at least be able to do vector processing on 32-bit floats to run Ultralight or Ultralight2. Anything half recent ought to do it, though I'm interested in exceptions (I don't think stuff compiled on the Pi will be using SIMD).So now there's Damping (same as RT60, but more damping means less sustain), but if you turn Damping to zero, rather than getting infinite sustain, now you get zero reverb… just only the predelay, dry/wet, and the allpasses. So Ultralight2 can also be used as a blurry slapback.Then there's unSolid and unRflec, which are the highpass and lowpass like they are in Ultralight. But there are changes here too… unSolid is the highpass, where if you turn it up the decay gets brighter and you lose bass through the walls. However, the frequencies are a bit different than they are in Ultralight. unRflec is the lowpass, like in Ultralight, where I've fixed a simple bug and re-uploaded it (it's just the next week, be aware that's happened and consider it an Ultralight Redux). However, with Ultralight, these things go from 0 to 4, incrementally adding stages of the feedback path into the filtering, and using the same paths: for Ultralight they count down from 4 which is the least filtered, and they pick the same paths so choosing 3 on both means that path is both highpassed and lowpassed.For Ultralight2, you set it to 0 for no filtering, and 1 to filter only one path… but they come from opposite directions. So, if you set the filters to 2 and 2, you've just put a filter on every path, but only one. If you pick 1 and 3, or 3 and 1, same deal: you've affected all the feedback, but in different ways. 4 and 4 gives you full highpassing and lowpassing on every path, and so on. I just found that it sounded better that way.You can also set the predelay to 0 and then there's no allpassing. Each part of the reverb can be individually bypassed, just in case.Dry/Wet is the reverb (there are no early reflections) and works as usual, except that in Ultralight2…Distnce is the distance high frequency attentuation, just like in FastDistance, and runs AFTER dry/wet so you can hear what a sound, its predelay, and its reverb, sound like in a game engine with all of them far away from you. That's because, like my earlier plugin VerbSixes, this plugin is the tool I use to audition possible 4x4 reverbs to add to the game engine. I paste in the new values in the header file, build it, and do a sound example to hear what it did. Especially when dialing in settings, that's helpful. Again, this is NOT really meant for musician or mix use, though if you must run a reverb on impossibly potato hardware (that can do SIMD), this might save you.I think this will hold me with low-CPU verbs for now. I've got big things in store for the flagship reverbs: turns out there's a kind of matrix for 6x6es that's way better than Householder, which is optimal for stuff like these 4x4s. So this work has led to some nice advances in Airwindows-land :)"
};
constexpr std::string_view k_tags{
    "reverb"
};

template <typename T>
class Ultralight2 final : public Effect<T>
{
    static constexpr int d4A = 1317;
    static constexpr int d4B = 40;
    static constexpr int d4C = 71;
    static constexpr int d4D = 74;
    static constexpr int d4E = 2139;
    static constexpr int d4F = 65;
    static constexpr int d4G = 350;
    static constexpr int d4H = 2025;
    static constexpr int d4I = 654;
    static constexpr int d4J = 686;
    static constexpr int d4K = 384;
    static constexpr int d4L = 257;
    static constexpr int d4M = 1743;
    static constexpr int d4N = 1764;
    static constexpr int d4O = 217;
    static constexpr int d4P = 1751;
    static constexpr int d4Q = 1880;
    static constexpr int d4R = 910;
    static constexpr int d4S = 620;
    static constexpr int d4T = 7920;
    static constexpr int d4U = 5;
    static constexpr int d4V = 4;
    static constexpr int d4W = 4;
    static constexpr int prime[] = { 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997, 1009, 1013, 1019, 1021, 1031, 1033, 1039, 1049, 1051, 1061, 1063, 1069, 1087, 1091, 1093, 1097, 1103, 1109, 1117, 1123, 1129, 1151, 1153, 1163, 1171, 1181, 1187, 1193, 1201, 1213, 1217, 1223, 1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291, 1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373, 1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451, 1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511, 1523, 1531, 1543, 1549, 1553, 1559, 1567, 1571, 1579, 1583, 1597, 1601, 1607, 1609, 1613, 1619, 1621, 1627, 1637, 1657, 1663, 1667, 1669, 1693, 1697, 1699, 1709, 1721, 1723, 1733, 1741, 1747, 1753, 1759, 1777, 1783, 1787, 1789, 1801, 1811, 1823, 1831, 1847, 1861, 1867, 1871, 1873, 1877, 1879, 1889, 1901, 1907, 1913, 1931, 1933, 1949, 1951, 1973, 1979, 1987, 1993, 1997, 1999, 2003, 2011, 2017, 2027, 2029, 2039, 2053, 2063, 2069, 2081, 2083, 2087, 2089, 2099, 2111, 2113, 2129, 2131, 2137, 2141, 2143, 2153, 2161, 2179, 2203, 2207, 2213, 2221, 2237, 2239, 2243, 2251, 2267, 2269, 2273, 2281, 2287, 2293, 2297, 2309, 2311, 2333, 2339, 2341, 2347, 2351, 2357, 2371, 2377, 2381, 2383, 2389, 2393, 2399, 2411, 2417, 2423, 2437, 2441, 2447, 2459, 2467, 2473, 2477, 2503, 2521, 2531, 2539, 2543, 2549, 2551, 2557, 2579, 2591, 2593, 2609, 2617, 2621, 2633, 2647, 2657, 2659, 2663, 2671, 2677, 2683, 2687, 2689, 2693, 2699, 2707, 2711, 2713, 2719, 2729, 2731, 2741, 2749, 2753, 2767, 2777, 2789, 2791, 2797, 2801, 2803, 2819, 2833, 2837, 2843, 2851, 2857, 2861, 2879, 2887, 2897, 2903, 2909, 2917, 2927, 2939, 2953, 2957, 2963, 2969, 2971, 2999, 3001, 3011, 3019, 3023, 3037, 3041, 3049, 3061, 3067, 3079, 3083, 3089, 3109, 3119, 3121, 3137, 3163, 3167, 3169, 3181, 3187, 3191, 3203, 3209, 3217, 3221, 3229, 3251, 3253, 3257, 3259, 3271, 3299, 3301, 3307, 3313, 3319, 3323, 3329, 3331, 3343, 3347, 3359, 3361, 3371, 3373, 3389, 3391, 3407, 3413, 3433, 3449, 3457, 3461, 3463, 3467, 3469, 3491, 3499, 3511, 3517, 3527, 3529, 3533, 3539, 3541, 3547, 3557, 3559, 3571, 3581, 3583, 3593, 3607, 3613, 3617, 3623, 3631, 3637, 3643, 3659, 3671, 3673, 3677, 3691, 3697, 3701, 3709, 3719, 3727, 3733, 3739, 3761, 3767, 3769, 3779, 3793, 3797, 3803, 3821, 3823, 3833, 3847, 3851, 3853, 3863, 3877, 3881, 3889, 3907, 3911, 3917, 3919, 3923, 3929, 3931, 3943, 3947, 3967, 3989, 4001, 4003, 4007, 4013, 4019, 4021, 4027, 4049, 4051, 4057, 4073, 4079, 4091, 4093, 4099, 4111, 4127, 4129, 4133, 4139, 4153, 4157, 4159, 4177, 4201, 4211, 4217, 4219, 4229, 4231, 4241, 4243, 4253, 4259, 4261, 4271, 4273, 4283, 4289, 4297, 4327, 4337, 4339, 4349, 4357, 4363, 4373, 4391, 4397, 4409, 4421, 4423, 4441, 4447, 4451, 4457, 4463, 4481, 4483, 4493, 4507, 4513, 4517, 4519, 4523, 4547, 4549, 4561, 4567, 4583, 4591, 4597, 4603, 4621, 4637, 4639, 4643, 4649, 4651, 4657, 4663, 4673, 4679, 4691, 4703, 4721, 4723, 4729, 4733, 4751, 4759, 4783, 4787, 4789, 4793, 4799, 4801, 4813, 4817, 4831, 4861, 4871, 4877, 4889, 4903, 4909, 4919, 4931, 4933, 4937, 4943, 4951, 4957, 4967, 4969, 4973, 4987, 4993, 4999, 5003, 5009, 5011, 5021, 5023, 5039, 5051, 5059, 5077, 5081, 5087, 5099, 5101, 5107, 5113, 5119, 5147, 5153, 5167, 5171, 5179, 5189, 5197, 5209, 5227, 5231, 5233, 5237, 5261, 5273, 5279, 5281, 5297, 5303, 5309, 5323, 5333, 5347, 5351, 5381, 5387, 5393, 5399, 5407, 5413, 5417, 5419, 5431, 5437, 5441, 5443, 5449, 5471, 5477, 5479, 5483, 5501, 5503, 5507, 5519, 5521, 5527, 5531, 5557, 5563, 5569, 5573, 5581, 5591, 5623, 5639, 5641, 5647, 5651, 5653, 5657, 5659, 5669, 5683, 5689, 5693, 5701, 5711, 5717, 5737, 5741, 5743, 5749, 5779, 5783, 5791, 5801, 5807, 5813, 5821, 5827, 5839, 5843, 5849, 5851, 5857, 5861, 5867, 5869, 5879, 5881, 5897, 5903, 5923, 5927, 5939, 5953, 5981, 5987, 6007, 6011, 6029, 6037, 6043, 6047, 6053, 6067, 6073, 6079, 6089, 6091, 6101, 6113, 6121, 6131, 6133, 6143, 6151, 6163, 6173, 6197, 6199, 6203, 6211, 6217, 6221, 6229, 6247, 6257, 6263, 6269, 6271, 6277, 6287, 6299, 6301, 6311, 6317, 6323, 6329, 6337, 6343, 6353, 6359, 6361, 6367, 6373, 6379, 6389, 6397, 6421, 6427, 6449, 6451, 6469, 6473, 6481, 6491, 6521, 6529, 6547, 6551, 6553, 6563, 6569, 6571, 6577, 6581, 6599, 6607, 6619, 6637, 6653, 6659, 6661, 6673, 6679, 6689, 6691, 6701, 6703, 6709, 6719, 6733, 6737, 6761, 6763, 6779, 6781, 6791, 6793, 6803, 6823, 6827, 6829, 6833, 6841, 6857, 6863, 6869, 6871, 6883, 6899, 6907, 6911, 6917, 6947, 6949, 6959, 6961, 6967, 6971, 6977, 6983, 6991, 6997, 7001, 7013, 7019, 7027, 7039, 7043, 7057, 7069, 7079, 7103, 7109, 7121, 7127, 7129, 7151, 7159, 7177, 7187, 7193, 7207, 7211, 7213, 7219, 7229, 7237, 7243, 7247, 7253, 7283, 7297, 7307, 7309, 7321, 7331, 7333, 7349, 7351, 7369, 7393, 7411, 7417, 7433, 7451, 7457, 7459, 7477, 7481, 7487, 7489, 7499, 7507, 7517, 7523, 7529, 7537, 7541, 7547, 7549, 7559, 7561, 7573, 7577, 7583, 7589, 7591, 7603, 7607, 7621, 7639, 7643, 7649, 7669, 7673, 7681, 7687, 7691, 7699, 7703, 7717, 7723, 7727, 7741, 7753, 7757, 7759, 7789, 7793, 7817, 7823, 7829, 7841, 7853, 7867, 7873, 7877, 7879, 7883, 7901, 7907, 7919 };
    static constexpr int kNumPrograms = 0;
    static constexpr int kNumInputs = 2;
    static constexpr int kNumOutputs = 2;
    static constexpr unsigned long kUniqueId = 'utl2';
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float lA[d4A + 5];
    float lB[d4B + 5];
    float lC[d4C + 5];
    float lD[d4D + 5];
    float lE[d4E + 5];
    float lF[d4F + 5];
    float lG[d4G + 5];
    float lH[d4H + 5];
    float lI[d4I + 5];
    float lJ[d4J + 5];
    float lK[d4K + 5];
    float lL[d4L + 5];
    float lM[d4M + 5];
    float lN[d4N + 5];
    float lO[d4O + 5];
    float lP[d4P + 5];
    float lQ[d4Q + 5];
    float lR[d4R + 5];
    float lS[d4S + 5];
    float lT[d4T + 5];
    float rA[d4A + 5];
    float rB[d4B + 5];
    float rC[d4C + 5];
    float rD[d4D + 5];
    float rE[d4E + 5];
    float rF[d4F + 5];
    float rG[d4G + 5];
    float rH[d4H + 5];
    float rI[d4I + 5];
    float rJ[d4J + 5];
    float rK[d4K + 5];
    float rL[d4L + 5];
    float rM[d4M + 5];
    float rN[d4N + 5];
    float rO[d4O + 5];
    float rP[d4P + 5];
    float rQ[d4Q + 5];
    float rR[d4R + 5];
    float rS[d4S + 5];
    float rT[d4T + 5];
    int cA, cB, cC, cD, cE, cF, cG, cH;
    int cI, cJ, cK, cL, cM, cN, cO, cP;
    int cQ, cR, cS, cT; // QRST are the allpasses taking the place of predelay
    float oQL, oRL, oSL, oTL; // oQRST are the outputs of the allpasses
    float fAL, fBL, fCL, fDL;
    float fhAL, fhBL, fhCL, fhDL;
    float flAL, flBL, flCL, flDL;
    float hAL, hBL, hCL, hDL, hEL; // do not allocate in audio code
    float oQR, oRR, oSR, oTR; // oQRST are the outputs of the allpasses
    float fAR, fBR, fCR, fDR;
    float fhAR, fhBR, fhCR, fhDR;
    float flAR, flBR, flCR, flDR;
    float hAR, hBR, hCR, hDR, hER; // do not allocate in audio code
    float firstDryL, firstAvgL;
    float lsA, lsB, lsC, lsD, lsE, lsF, lsG, lsH, lsI, lsJ, lsK, lsL, lsM;
    float lsN, lsO, lsP, lsQ, lsR, lsS, lsT, lsU, lsV, lsW, lsX, lsY, lsZ;
    float lsa, lsb, lsc, lsd, lse, lsf, lsg, lsh, lsi, lsj, lsk, lsl, lsm;
    float lsn, lso, lsp, lsq, lsr, lss, lst, lsu, lsv, lsw, lsx, lsy, lsz;
    float firstDryR, firstAvgR;
    float rsA, rsB, rsC, rsD, rsE, rsF, rsG, rsH, rsI, rsJ, rsK, rsL, rsM;
    float rsN, rsO, rsP, rsQ, rsR, rsS, rsT, rsU, rsV, rsW, rsX, rsY, rsZ;
    float rsa, rsb, rsc, rsd, rse, rsf, rsg, rsh, rsi, rsj, rsk, rsl, rsm;
    float rsn, rso, rsp, rsq, rsr, rss, rst, rsu, rsv, rsw, rsx, rsy, rsz;
    int prevDistance;
    float pointCycle, pointAL, pointBL, pointAR, pointBR;
    // this is a simple linear interpolation sample rate routine for the plugin version
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Ultralight2()
    {
        A = 0.65;
        B = 0.75;
        C = 0.75;
        D = 0.54;
        E = 0.5;
        F = 0.0;
        for (int x = 0; x < d4A + 2; x++) {
            lA[x] = 0.0f;
            rA[x] = 0.0f;
        }
        for (int x = 0; x < d4B + 2; x++) {
            lB[x] = 0.0f;
            rB[x] = 0.0f;
        }
        for (int x = 0; x < d4C + 2; x++) {
            lC[x] = 0.0f;
            rC[x] = 0.0f;
        }
        for (int x = 0; x < d4D + 2; x++) {
            lD[x] = 0.0f;
            rD[x] = 0.0f;
        }
        for (int x = 0; x < d4E + 2; x++) {
            lE[x] = 0.0f;
            rE[x] = 0.0f;
        }
        for (int x = 0; x < d4F + 2; x++) {
            lF[x] = 0.0f;
            rF[x] = 0.0f;
        }
        for (int x = 0; x < d4G + 2; x++) {
            lG[x] = 0.0f;
            rG[x] = 0.0f;
        }
        for (int x = 0; x < d4H + 2; x++) {
            lH[x] = 0.0f;
            rH[x] = 0.0f;
        }
        for (int x = 0; x < d4I + 2; x++) {
            lI[x] = 0.0f;
            rI[x] = 0.0f;
        }
        for (int x = 0; x < d4J + 2; x++) {
            lJ[x] = 0.0f;
            rJ[x] = 0.0f;
        }
        for (int x = 0; x < d4K + 2; x++) {
            lK[x] = 0.0f;
            rK[x] = 0.0f;
        }
        for (int x = 0; x < d4L + 2; x++) {
            lL[x] = 0.0f;
            rL[x] = 0.0f;
        }
        for (int x = 0; x < d4M + 2; x++) {
            lM[x] = 0.0f;
            rM[x] = 0.0f;
        }
        for (int x = 0; x < d4N + 2; x++) {
            lN[x] = 0.0f;
            rN[x] = 0.0f;
        }
        for (int x = 0; x < d4O + 2; x++) {
            lO[x] = 0.0f;
            rO[x] = 0.0f;
        }
        for (int x = 0; x < d4P + 2; x++) {
            lP[x] = 0.0f;
            rP[x] = 0.0f;
        }
        for (int x = 0; x < d4Q + 2; x++) {
            lQ[x] = 0.0f;
            rQ[x] = 0.0f;
        }
        for (int x = 0; x < d4R + 2; x++) {
            lR[x] = 0.0f;
            rR[x] = 0.0f;
        }
        for (int x = 0; x < d4S + 2; x++) {
            lS[x] = 0.0f;
            rS[x] = 0.0f;
        }
        for (int x = 0; x < d4T + 2; x++) {
            lT[x] = 0.0f;
            rT[x] = 0.0f;
        }
        cA = cB = cC = cD = cE = cF = cG = cH = 1;
        cI = cJ = cK = cL = cM = cN = cO = cP = 1;
        cQ = cR = cS = cT = 1;
        oQL = oRL = oSL = oTL = 0.0f;
        fAL = fBL = fCL = fDL = 0.0f;
        fhAL = fhBL = fhCL = fhDL = 0.0f;
        flAL = flBL = flCL = flDL = 0.0f;
        hAL = hBL = hCL = hDL = hEL = 0.0f;
        oQR = oRR = oSR = oTR = 0.0f;
        fAR = fBR = fCR = fDR = 0.0f;
        fhAR = fhBR = fhCR = fhDR = 0.0f;
        flAR = flBR = flCR = flDR = 0.0f;
        hAR = hBR = hCR = hDR = hER = 0.0f;
        firstDryL = firstAvgL = 0.0f;
        lsA = lsB = lsC = lsD = lsE = lsF = lsG = lsH = lsI = lsJ = lsK = lsL = lsM = 0.0f;
        lsN = lsO = lsP = lsQ = lsR = lsS = lsT = lsU = lsV = lsW = lsX = lsY = lsZ = 0.0f;
        lsa = lsb = lsc = lsd = lse = lsf = lsg = lsh = lsi = lsj = lsk = lsl = lsm = 0.0f;
        lsn = lso = lsp = lsq = lsr = lss = lst = lsu = lsv = lsw = lsx = lsy = lsz = 0.0f;
        firstDryR = firstAvgR = 0.0f;
        rsA = rsB = rsC = rsD = rsE = rsF = rsG = rsH = rsI = rsJ = rsK = rsL = rsM = 0.0f;
        rsN = rsO = rsP = rsQ = rsR = rsS = rsT = rsU = rsV = rsW = rsX = rsY = rsZ = 0.0f;
        rsa = rsb = rsc = rsd = rse = rsf = rsg = rsh = rsi = rsj = rsk = rsl = rsm = 0.0f;
        rsn = rso = rsp = rsq = rsr = rss = rst = rsu = rsv = rsw = rsx = rsy = rsz = 0.0f;
        prevDistance = 0;
        pointCycle = pointAL = pointBL = pointAR = pointBR = 0.0f;
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
        kParamE = 4,
        kParamF = 5,
        kNumParameters = 6
    };

    void set_parameter_value(int index, float value)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;

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
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return 0.65; break;
            case kParamB: return 0.75; break;
            case kParamC: return 0.75; break;
            case kParamD: return 0.54; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "damping"; break;
            case kParamB: return "unsolid"; break;
            case kParamC: return "unreflc"; break;
            case kParamD: return "predlay"; break;
            case kParamE: return "dry/wet"; break;
            case kParamF: return "distnce"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index))
        {
            case kParamA: return "Damping"; break;
            case kParamB: return "UnSolid"; break;
            case kParamC: return "UnReflc"; break;
            case kParamD: return "Predlay"; break;
            case kParamE: return "Dry/Wet"; break;
            case kParamF: return "Distnce"; break;

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
            case kParamD: return std::to_string(D); break;
            case kParamE: return std::to_string(E); break;
            case kParamF: return std::to_string(F); break;

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
            case kParamD: return ""; break;
            case kParamE: return ""; break;
            case kParamF: return ""; break;

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

        float overallscale = 1.0f;
        overallscale /= 48000.0f;
        overallscale *= Effect<T>::getSampleRate();
        float pointDerez = 1.0f / overallscale; // forcing it to be simple
        bool bypassPre = (((int)(D * 998.0)) < 1); // plugin only
        // the hardcoded verb objects use set matrix delay sizes, then the following four extras:
        float X = D * 998.0f;
        int dpT = prime[(int)X]; // only a delay, not an allpass
        X = sqrt(X * 81.408f);
        int dpQ = prime[(int)X]; // final allpass
        X = sqrt(X * 81.408f);
        int dpR = prime[(int)X]; // second allpass
        X = sqrt(X * 81.408f);
        int dpS = prime[(int)X]; // first allpass
        // this produces four ints that declare arrays like the matrix delays,
        // which replaces the plugin versions set up to scale up to the maximum predelay
        // so, here's how we gnerate const int d4Q-R-S-T with T first, then QRS
        // X = the number in Param D; d4T = prime[(int)X];
        // X = sqrt(X*81.408); d4Q = prime[(int)X];
        // X = sqrt(X*81.408); d4R = prime[(int)X];
        // X = sqrt(X*81.408); d4S = prime[(int)X];
        bool bypassVerb = (((int)(A * 9.99f)) < 1); // plugin only
        // and then the following three adjustments for RT60 and tone shaping.
        float damping = 0.0625f - (((int)(A * 9.99f)) * 0.003125f); // 0 bypass, 1-9 valid
        int unSolid = 4 - (int)(B * 4.99f); // 0 through 4 are valid
        int unReflect = 4 - (int)(C * 4.99f); // 0 through 4 are valid
        // lastly, the verb object gets three more ints, d4U-V-W, damping-unsolid-unreflect.
        // it must generate its damping value from infinite sustain minus the damping int,
        // then unsolid/unreflect just selects which path to highpass/lowpass, or not.
        // damping = 0.0625f-(d4U*0.003125f); //will be 1-9
        // use d4V for unSolid, use d4W for unReflect
        float wet = E;
        // this plugin applies distance filter to dry AND wet, so that when testing
        // out possible spaces for the game engine, you can hear what it'll sound like
        // in practice when both the source sound and the verb are distant.
        float distance = pow(F * 7.28f, 2.0f); // 0 to 52.9984
        int distanceSteps = (int)distance; // 52 maximum
        distance -= (float)distanceSteps; // 0.9984
        // distance is not a property of the reverb node, it's a property of
        // the game audio engine, where every source can be localized
        while (--sampleFrames >= 0)
        {
            float inputSampleL = *in1;
            float inputSampleR = *in2;
            if (fabs(inputSampleL) < 1.18e-23) {
                inputSampleL = fpdL * 1.18e-17;
            }
            if (fabs(inputSampleR) < 1.18e-23) {
                inputSampleR = fpdR * 1.18e-17;
            }
            pointCycle += pointDerez;
            if (pointCycle > 0.9999999f) {
                // this is sample rate handling for the plugin, so that
                // a more primitive reverb node can run at any rate.
                float drySampleL = inputSampleL;
                float drySampleR = inputSampleR;
                // predelay for plugin: not suitable for game engine,
                // as it doesn't handle changing delays gracefully.
                // However, we are going to run a FIXED predelay for
                // each verb node, on the grounds that if you're right next to it,
                // you'll hear maximum delay before the sound bounces off the first wall.
                // As such it'd always be the same delay, and so it'd not pose a problem,
                // even for sound events that are moving around in the verb area.
                // This is it, combined with three inline allpasses that are tuned
                // independently to the predelay, set up so the compiler can
                // vectorize the lot of them as SIMD operations.
                // Note that some old AMD processors may perform worse here.
                oQL = oRL - (lQ[(cQ + 1) - ((cQ + 1 > dpQ) ? dpQ + 1 : 0)] * 0.5f);
                oRL = oSL - (lR[(cR + 1) - ((cR + 1 > dpR) ? dpR + 1 : 0)] * 0.5f);
                oSL = oTL - (lS[(cS + 1) - ((cS + 1 > dpS) ? dpS + 1 : 0)] * 0.5f);
                oTL = inputSampleL; // SIMDable, but lT[] is just a simple delay
                oQR = oRR - (rQ[(cQ + 1) - ((cQ + 1 > dpQ) ? dpQ + 1 : 0)] * 0.5f);
                oRR = oSR - (rR[(cR + 1) - ((cR + 1 > dpR) ? dpR + 1 : 0)] * 0.5f);
                oSR = oTR - (rS[(cS + 1) - ((cS + 1 > dpS) ? dpS + 1 : 0)] * 0.5f);
                oTR = inputSampleR; // SIMDable, but rT[] is just a simple delay
                lQ[cQ] = oQL;
                lR[cR] = oRL;
                lS[cS] = oSL;
                lT[cT] = oTL;
                rQ[cQ] = oQR;
                rR[cR] = oRR;
                rS[cS] = oSR;
                rT[cT] = oTR;
                cQ++;
                cR++;
                cS++;
                cT++;
                if (cQ > dpQ) {
                    cQ = 0;
                }
                if (cR > dpR) {
                    cR = 0;
                }
                if (cS > dpS) {
                    cS = 0;
                }
                if (cT > dpT) {
                    cT = 0;
                }
                oQL = fma(oQL, 0.5f, lQ[cQ]);
                oRL = fma(oRL, 0.5f, lR[cR]);
                oSL = fma(oSL, 0.5f, lS[cS]);
                oTL = fma(oTL, 0.0f, lT[cT]); // not allpass but can use vector fma()
                oQR = fma(oQR, 0.5f, rQ[cQ]);
                oRR = fma(oRR, 0.5f, rR[cR]);
                oSR = fma(oSR, 0.5f, rS[cS]);
                oTR = fma(oTR, 0.0f, rT[cT]); // not allpass but can use vector fma()
                if (bypassPre) {
                    oQL = inputSampleL;
                    oQR = inputSampleR;
                } // plugin only
                lA[cA] = fma(fAL, damping, oQL);
                lB[cB] = fma(fBL, damping, oQL);
                lC[cC] = fma(fCL, damping, oQL);
                lD[cD] = fma(fDL, damping, oQL);
                rA[cA] = fma(fAR, damping, oQR);
                rB[cB] = fma(fBR, damping, oQR);
                rC[cC] = fma(fCR, damping, oQR);
                rD[cD] = fma(fDR, damping, oQR);
                cA++;
                cB++;
                cC++;
                cD++;
                if (cA > d4A) {
                    cA = 0;
                }
                if (cB > d4B) {
                    cB = 0;
                }
                if (cC > d4C) {
                    cC = 0;
                }
                if (cD > d4D) {
                    cD = 0;
                }
                hAL = lA[cA];
                hBL = lB[cB];
                hCL = lC[cC];
                hDL = lD[cD];
                hAR = rA[cA];
                hBR = rB[cB];
                hCR = rC[cC];
                hDR = rD[cD];
                hEL = -(hAL + hBL + hCL + hDL);
                hER = -(hAR + hBR + hCR + hDR);
                lE[cE] = fma(hAL, 2.0f, hEL);
                lF[cF] = fma(hBL, 2.0f, hEL);
                lG[cG] = fma(hCL, 2.0f, hEL);
                lH[cH] = fma(hDL, 2.0f, hEL);
                rE[cE] = fma(hAR, 2.0f, hER);
                rF[cF] = fma(hBR, 2.0f, hER);
                rG[cG] = fma(hCR, 2.0f, hER);
                rH[cH] = fma(hDR, 2.0f, hER);
                cE++;
                cF++;
                cG++;
                cH++;
                if (cE > d4E) {
                    cE = 0;
                }
                if (cF > d4F) {
                    cF = 0;
                }
                if (cG > d4G) {
                    cG = 0;
                }
                if (cH > d4H) {
                    cH = 0;
                }
                hAL = lE[cE];
                hBL = lF[cF];
                hCL = lG[cG];
                hDL = lH[cH];
                hAR = rE[cE];
                hBR = rF[cF];
                hCR = rG[cG];
                hDR = rH[cH];
                hEL = -(hAL + hBL + hCL + hDL);
                hER = -(hAR + hBR + hCR + hDR);
                lI[cI] = fma(hAL, 2.0f, hEL);
                lJ[cJ] = fma(hBL, 2.0f, hEL);
                lK[cK] = fma(hCL, 2.0f, hEL);
                lL[cL] = fma(hDL, 2.0f, hEL);
                rI[cI] = fma(hAR, 2.0f, hER);
                rJ[cJ] = fma(hBR, 2.0f, hER);
                rK[cK] = fma(hCR, 2.0f, hER);
                rL[cL] = fma(hDR, 2.0f, hER);
                cI++;
                cJ++;
                cK++;
                cL++;
                if (cI > d4I) {
                    cI = 0;
                }
                if (cJ > d4J) {
                    cJ = 0;
                }
                if (cK > d4K) {
                    cK = 0;
                }
                if (cL > d4L) {
                    cL = 0;
                }
                hAL = lI[cI];
                hBL = lJ[cJ];
                hCL = lK[cK];
                hDL = lL[cL];
                hAR = rI[cI];
                hBR = rJ[cJ];
                hCR = rK[cK];
                hDR = rL[cL];
                hEL = -(hAL + hBL + hCL + hDL);
                hER = -(hAR + hBR + hCR + hDR);
                lM[cM] = fma(hAL, 2.0f, hEL);
                lN[cN] = fma(hBL, 2.0f, hEL);
                lO[cO] = fma(hCL, 2.0f, hEL);
                lP[cP] = fma(hDL, 2.0f, hEL);
                rM[cM] = fma(hAR, 2.0f, hER);
                rN[cN] = fma(hBR, 2.0f, hER);
                rO[cO] = fma(hCR, 2.0f, hER);
                rP[cP] = fma(hDR, 2.0f, hER);
                cM++;
                cN++;
                cO++;
                cP++;
                if (cM > d4M) {
                    cM = 0;
                }
                if (cN > d4N) {
                    cN = 0;
                }
                if (cO > d4O) {
                    cO = 0;
                }
                if (cP > d4P) {
                    cP = 0;
                }
                hAL = lM[cM];
                hBL = lN[cN];
                hCL = lO[cO];
                hDL = lP[cP];
                hAR = rM[cM];
                hBR = rN[cN];
                hCR = rO[cO];
                hDR = rP[cP];
                hEL = -(hAL + hBL + hCL + hDL);
                hER = -(hAR + hBR + hCR + hDR);
                fAL = fma(hAL, 2.0f, hEL);
                fBL = fma(hBL, 2.0f, hEL);
                fCL = fma(hCL, 2.0f, hEL);
                fDL = fma(hDL, 2.0f, hEL);
                fAR = fma(hAR, 2.0f, hER);
                fBR = fma(hBR, 2.0f, hER);
                fCR = fma(hCR, 2.0f, hER);
                fDR = fma(hDR, 2.0f, hER);
                switch (unSolid)
                {
                    case 0:
                        fhAL *= 0.25f;
                        fAL -= fhAL * 0.125f;
                        fhAL += fAL;
                        fhAR *= 0.25f;
                        fAR -= fhAR * 0.125f;
                        fhAR += fAR;
                    case 1:
                        fhBL *= 0.25f;
                        fBL -= fhBL * 0.125f;
                        fhBL += fBL;
                        fhBR *= 0.25f;
                        fBR -= fhBR * 0.125f;
                        fhBR += fBR;
                    case 2:
                        fhCL *= 0.25f;
                        fCL -= fhCL * 0.125f;
                        fhCL += fCL;
                        fhCR *= 0.25f;
                        fCR -= fhCR * 0.125f;
                        fhCR += fCR;
                    case 3:
                        fhDL *= 0.25f;
                        fDL -= fhDL * 0.125f;
                        fhDL += fDL;
                        fhDR *= 0.25f;
                        fDR -= fhDR * 0.125f;
                        fhDR += fDR;
                    case 4:
                        break;
                }
                switch (unReflect)
                {
                    case 0:
                        fDL = (fDL + flDL) * 0.5f;
                        flDL = fDL;
                        fDR = (fDR + flDR) * 0.5f;
                        flDR = fDR;
                    case 1:
                        fCL = (fCL + flCL) * 0.5f;
                        flCL = fCL;
                        fCR = (fCR + flCR) * 0.5f;
                        flCR = fCR;
                    case 2:
                        fBL = (fBL + flBL) * 0.5f;
                        flBL = fBL;
                        fBR = (fBR + flBR) * 0.5f;
                        flBR = fBR;
                    case 3:
                        fAL = (fAL + flAL) * 0.5f;
                        flAL = fAL;
                        fAR = (fAR + flAR) * 0.5f;
                        flAR = fAR;
                    case 4:
                        break;
                }
                inputSampleL = (hAL + hBL + hCL + hDL) * 0.0625f;
                inputSampleR = (hAR + hBR + hCR + hDR) * 0.0625f;
                // end of just the reverb node part
                if (bypassVerb) {
                    inputSampleL = oQL;
                    inputSampleR = oQR;
                } // plugin only
                // dry/wet is not part of the game engine reverb node
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f - wet));
                // plugin only: game engine one would have just the reverb,
                // and every sound source could have the following Distance filter,
                // including the localized reverb nodes. The reason it's here
                // is so the plugin can preview how dry+reverb, both distant, will sound
                // begin just the distance filter L
                firstDryL = inputSampleL; // start by doing the interpolation
                inputSampleL += firstAvgL;
                inputSampleL *= 0.5f;
                firstAvgL = inputSampleL;
                inputSampleL = (firstDryL * (1.0f - distance)) + (inputSampleL * distance);
                // having done at least one interpolation we can now do the integer number of stages
                if (prevDistance < distanceSteps) {
                    switch (prevDistance)
                    {
                        case 0: lsA = inputSampleL; // if we've changed the setting
                        case 1: lsB = inputSampleL; // reset the ones being put back into play
                        case 2: lsC = inputSampleL; // this won't be happening constantly,
                        case 3: lsD = inputSampleL; // but since we don't have an array
                        case 4: lsE = inputSampleL; // this is the structure by which we
                        case 5: lsF = inputSampleL; // get the new averagings going
                        case 6: lsG = inputSampleL; // without pops or clicks
                        case 7: lsH = inputSampleL;
                        case 8: lsI = inputSampleL;
                        case 9: lsJ = inputSampleL;
                        case 10: lsK = inputSampleL;
                        case 11: lsL = inputSampleL;
                        case 12: lsM = inputSampleL;
                        case 13: lsN = inputSampleL;
                        case 14: lsO = inputSampleL;
                        case 15: lsP = inputSampleL;
                        case 16: lsQ = inputSampleL;
                        case 17: lsR = inputSampleL;
                        case 18: lsS = inputSampleL;
                        case 19: lsT = inputSampleL;
                        case 20: lsU = inputSampleL;
                        case 21: lsV = inputSampleL;
                        case 22: lsW = inputSampleL;
                        case 23: lsX = inputSampleL;
                        case 24: lsY = inputSampleL;
                        case 25: lsZ = inputSampleL;
                        case 26: lsa = inputSampleL;
                        case 27: lsb = inputSampleL;
                        case 28: lsc = inputSampleL;
                        case 29: lsd = inputSampleL;
                        case 30: lse = inputSampleL;
                        case 31: lsf = inputSampleL;
                        case 32: lsg = inputSampleL;
                        case 33: lsh = inputSampleL;
                        case 34: lsi = inputSampleL;
                        case 35: lsj = inputSampleL;
                        case 36: lsk = inputSampleL;
                        case 37: lsl = inputSampleL;
                        case 38: lsm = inputSampleL;
                        case 39: lsn = inputSampleL;
                        case 40: lso = inputSampleL;
                        case 41: lsp = inputSampleL;
                        case 42: lsq = inputSampleL;
                        case 43: lsr = inputSampleL;
                        case 44: lss = inputSampleL;
                        case 45: lst = inputSampleL;
                        case 46: lsu = inputSampleL;
                        case 47: lsv = inputSampleL;
                        case 48: lsw = inputSampleL;
                        case 49: lsx = inputSampleL;
                        case 50: lsy = inputSampleL;
                        case 51: lsz = inputSampleL;
                        case 52: break; // reset prevDistance after R channel
                    }
                }
                switch (52 - distanceSteps)
                { // apply the stack of filter steps to produce the distance filter
                    case 0:
                        inputSampleL += lsz;
                        inputSampleL *= 0.5f;
                        lsz = inputSampleL;
                    case 1:
                        inputSampleL += lsy;
                        inputSampleL *= 0.5f;
                        lsy = inputSampleL;
                    case 2:
                        inputSampleL += lsx;
                        inputSampleL *= 0.5f;
                        lsx = inputSampleL;
                    case 3:
                        inputSampleL += lsw;
                        inputSampleL *= 0.5f;
                        lsw = inputSampleL;
                    case 4:
                        inputSampleL += lsv;
                        inputSampleL *= 0.5f;
                        lsv = inputSampleL;
                    case 5:
                        inputSampleL += lsu;
                        inputSampleL *= 0.5f;
                        lsu = inputSampleL;
                    case 6:
                        inputSampleL += lst;
                        inputSampleL *= 0.5f;
                        lst = inputSampleL;
                    case 7:
                        inputSampleL += lss;
                        inputSampleL *= 0.5f;
                        lss = inputSampleL;
                    case 8:
                        inputSampleL += lsr;
                        inputSampleL *= 0.5f;
                        lsr = inputSampleL;
                    case 9:
                        inputSampleL += lsq;
                        inputSampleL *= 0.5f;
                        lsq = inputSampleL;
                    case 10:
                        inputSampleL += lsp;
                        inputSampleL *= 0.5f;
                        lsp = inputSampleL;
                    case 11:
                        inputSampleL += lso;
                        inputSampleL *= 0.5f;
                        lso = inputSampleL;
                    case 12:
                        inputSampleL += lsn;
                        inputSampleL *= 0.5f;
                        lsn = inputSampleL;
                    case 13:
                        inputSampleL += lsm;
                        inputSampleL *= 0.5f;
                        lsm = inputSampleL;
                    case 14:
                        inputSampleL += lsl;
                        inputSampleL *= 0.5f;
                        lsl = inputSampleL;
                    case 15:
                        inputSampleL += lsk;
                        inputSampleL *= 0.5f;
                        lsk = inputSampleL;
                    case 16:
                        inputSampleL += lsj;
                        inputSampleL *= 0.5f;
                        lsj = inputSampleL;
                    case 17:
                        inputSampleL += lsi;
                        inputSampleL *= 0.5f;
                        lsi = inputSampleL;
                    case 18:
                        inputSampleL += lsh;
                        inputSampleL *= 0.5f;
                        lsh = inputSampleL;
                    case 19:
                        inputSampleL += lsg;
                        inputSampleL *= 0.5f;
                        lsg = inputSampleL;
                    case 20:
                        inputSampleL += lsf;
                        inputSampleL *= 0.5f;
                        lsf = inputSampleL;
                    case 21:
                        inputSampleL += lse;
                        inputSampleL *= 0.5f;
                        lse = inputSampleL;
                    case 22:
                        inputSampleL += lsd;
                        inputSampleL *= 0.5f;
                        lsd = inputSampleL;
                    case 23:
                        inputSampleL += lsc;
                        inputSampleL *= 0.5f;
                        lsc = inputSampleL;
                    case 24:
                        inputSampleL += lsb;
                        inputSampleL *= 0.5f;
                        lsb = inputSampleL;
                    case 25:
                        inputSampleL += lsa;
                        inputSampleL *= 0.5f;
                        lsa = inputSampleL;
                    case 26:
                        inputSampleL += lsZ;
                        inputSampleL *= 0.5f;
                        lsZ = inputSampleL;
                    case 27:
                        inputSampleL += lsY;
                        inputSampleL *= 0.5f;
                        lsY = inputSampleL;
                    case 28:
                        inputSampleL += lsX;
                        inputSampleL *= 0.5f;
                        lsX = inputSampleL;
                    case 29:
                        inputSampleL += lsW;
                        inputSampleL *= 0.5f;
                        lsW = inputSampleL;
                    case 30:
                        inputSampleL += lsV;
                        inputSampleL *= 0.5f;
                        lsV = inputSampleL;
                    case 31:
                        inputSampleL += lsU;
                        inputSampleL *= 0.5f;
                        lsU = inputSampleL;
                    case 32:
                        inputSampleL += lsT;
                        inputSampleL *= 0.5f;
                        lsT = inputSampleL;
                    case 33:
                        inputSampleL += lsS;
                        inputSampleL *= 0.5f;
                        lsS = inputSampleL;
                    case 34:
                        inputSampleL += lsR;
                        inputSampleL *= 0.5f;
                        lsR = inputSampleL;
                    case 35:
                        inputSampleL += lsQ;
                        inputSampleL *= 0.5f;
                        lsQ = inputSampleL;
                    case 36:
                        inputSampleL += lsP;
                        inputSampleL *= 0.5f;
                        lsP = inputSampleL;
                    case 37:
                        inputSampleL += lsO;
                        inputSampleL *= 0.5f;
                        lsO = inputSampleL;
                    case 38:
                        inputSampleL += lsN;
                        inputSampleL *= 0.5f;
                        lsN = inputSampleL;
                    case 39:
                        inputSampleL += lsM;
                        inputSampleL *= 0.5f;
                        lsM = inputSampleL;
                    case 40:
                        inputSampleL += lsL;
                        inputSampleL *= 0.5f;
                        lsL = inputSampleL;
                    case 41:
                        inputSampleL += lsK;
                        inputSampleL *= 0.5f;
                        lsK = inputSampleL;
                    case 42:
                        inputSampleL += lsJ;
                        inputSampleL *= 0.5f;
                        lsJ = inputSampleL;
                    case 43:
                        inputSampleL += lsI;
                        inputSampleL *= 0.5f;
                        lsI = inputSampleL;
                    case 44:
                        inputSampleL += lsH;
                        inputSampleL *= 0.5f;
                        lsH = inputSampleL;
                    case 45:
                        inputSampleL += lsG;
                        inputSampleL *= 0.5f;
                        lsG = inputSampleL;
                    case 46:
                        inputSampleL += lsF;
                        inputSampleL *= 0.5f;
                        lsF = inputSampleL;
                    case 47:
                        inputSampleL += lsE;
                        inputSampleL *= 0.5f;
                        lsE = inputSampleL;
                    case 48:
                        inputSampleL += lsD;
                        inputSampleL *= 0.5f;
                        lsD = inputSampleL;
                    case 49:
                        inputSampleL += lsC;
                        inputSampleL *= 0.5f;
                        lsC = inputSampleL;
                    case 50:
                        inputSampleL += lsB;
                        inputSampleL *= 0.5f;
                        lsB = inputSampleL;
                    case 51:
                        inputSampleL += lsA;
                        inputSampleL *= 0.5f;
                        lsA = inputSampleL;
                    case 52: break;
                }
                // end distance filter L
                // begin just the distance filter R
                firstDryR = inputSampleR; // start by doing the interpolation
                inputSampleR += firstAvgR;
                inputSampleR *= 0.5f;
                firstAvgR = inputSampleR;
                inputSampleR = (firstDryR * (1.0f - distance)) + (inputSampleR * distance);
                // having done at least one interpolation we can now do the integer number of stages
                if (prevDistance < distanceSteps) {
                    switch (prevDistance)
                    {
                        case 0: rsA = inputSampleR; // if we've changed the setting
                        case 1: rsB = inputSampleR; // reset the ones being put back into play
                        case 2: rsC = inputSampleR; // this won't be happening constantly,
                        case 3: rsD = inputSampleR; // but since we don't have an array
                        case 4: rsE = inputSampleR; // this is the structure by which we
                        case 5: rsF = inputSampleR; // get the new averagings going
                        case 6: rsG = inputSampleR; // without pops or clicks
                        case 7: rsH = inputSampleR;
                        case 8: rsI = inputSampleR;
                        case 9: rsJ = inputSampleR;
                        case 10: rsK = inputSampleR;
                        case 11: rsL = inputSampleR;
                        case 12: rsM = inputSampleR;
                        case 13: rsN = inputSampleR;
                        case 14: rsO = inputSampleR;
                        case 15: rsP = inputSampleR;
                        case 16: rsQ = inputSampleR;
                        case 17: rsR = inputSampleR;
                        case 18: rsS = inputSampleR;
                        case 19: rsT = inputSampleR;
                        case 20: rsU = inputSampleR;
                        case 21: rsV = inputSampleR;
                        case 22: rsW = inputSampleR;
                        case 23: rsX = inputSampleR;
                        case 24: rsY = inputSampleR;
                        case 25: rsZ = inputSampleR;
                        case 26: rsa = inputSampleR;
                        case 27: rsb = inputSampleR;
                        case 28: rsc = inputSampleR;
                        case 29: rsd = inputSampleR;
                        case 30: rse = inputSampleR;
                        case 31: rsf = inputSampleR;
                        case 32: rsg = inputSampleR;
                        case 33: rsh = inputSampleR;
                        case 34: rsi = inputSampleR;
                        case 35: rsj = inputSampleR;
                        case 36: rsk = inputSampleR;
                        case 37: rsl = inputSampleR;
                        case 38: rsm = inputSampleR;
                        case 39: rsn = inputSampleR;
                        case 40: rso = inputSampleR;
                        case 41: rsp = inputSampleR;
                        case 42: rsq = inputSampleR;
                        case 43: rsr = inputSampleR;
                        case 44: rss = inputSampleR;
                        case 45: rst = inputSampleR;
                        case 46: rsu = inputSampleR;
                        case 47: rsv = inputSampleR;
                        case 48: rsw = inputSampleR;
                        case 49: rsx = inputSampleR;
                        case 50: rsy = inputSampleR;
                        case 51: rsz = inputSampleR;
                        case 52: prevDistance = distanceSteps;
                    }
                }
                switch (52 - distanceSteps)
                { // apply the stack of filter steps to produce the distance filter
                    case 0:
                        inputSampleR += rsz;
                        inputSampleR *= 0.5f;
                        rsz = inputSampleR;
                    case 1:
                        inputSampleR += rsy;
                        inputSampleR *= 0.5f;
                        rsy = inputSampleR;
                    case 2:
                        inputSampleR += rsx;
                        inputSampleR *= 0.5f;
                        rsx = inputSampleR;
                    case 3:
                        inputSampleR += rsw;
                        inputSampleR *= 0.5f;
                        rsw = inputSampleR;
                    case 4:
                        inputSampleR += rsv;
                        inputSampleR *= 0.5f;
                        rsv = inputSampleR;
                    case 5:
                        inputSampleR += rsu;
                        inputSampleR *= 0.5f;
                        rsu = inputSampleR;
                    case 6:
                        inputSampleR += rst;
                        inputSampleR *= 0.5f;
                        rst = inputSampleR;
                    case 7:
                        inputSampleR += rss;
                        inputSampleR *= 0.5f;
                        rss = inputSampleR;
                    case 8:
                        inputSampleR += rsr;
                        inputSampleR *= 0.5f;
                        rsr = inputSampleR;
                    case 9:
                        inputSampleR += rsq;
                        inputSampleR *= 0.5f;
                        rsq = inputSampleR;
                    case 10:
                        inputSampleR += rsp;
                        inputSampleR *= 0.5f;
                        rsp = inputSampleR;
                    case 11:
                        inputSampleR += rso;
                        inputSampleR *= 0.5f;
                        rso = inputSampleR;
                    case 12:
                        inputSampleR += rsn;
                        inputSampleR *= 0.5f;
                        rsn = inputSampleR;
                    case 13:
                        inputSampleR += rsm;
                        inputSampleR *= 0.5f;
                        rsm = inputSampleR;
                    case 14:
                        inputSampleR += rsl;
                        inputSampleR *= 0.5f;
                        rsl = inputSampleR;
                    case 15:
                        inputSampleR += rsk;
                        inputSampleR *= 0.5f;
                        rsk = inputSampleR;
                    case 16:
                        inputSampleR += rsj;
                        inputSampleR *= 0.5f;
                        rsj = inputSampleR;
                    case 17:
                        inputSampleR += rsi;
                        inputSampleR *= 0.5f;
                        rsi = inputSampleR;
                    case 18:
                        inputSampleR += rsh;
                        inputSampleR *= 0.5f;
                        rsh = inputSampleR;
                    case 19:
                        inputSampleR += rsg;
                        inputSampleR *= 0.5f;
                        rsg = inputSampleR;
                    case 20:
                        inputSampleR += rsf;
                        inputSampleR *= 0.5f;
                        rsf = inputSampleR;
                    case 21:
                        inputSampleR += rse;
                        inputSampleR *= 0.5f;
                        rse = inputSampleR;
                    case 22:
                        inputSampleR += rsd;
                        inputSampleR *= 0.5f;
                        rsd = inputSampleR;
                    case 23:
                        inputSampleR += rsc;
                        inputSampleR *= 0.5f;
                        rsc = inputSampleR;
                    case 24:
                        inputSampleR += rsb;
                        inputSampleR *= 0.5f;
                        rsb = inputSampleR;
                    case 25:
                        inputSampleR += rsa;
                        inputSampleR *= 0.5f;
                        rsa = inputSampleR;
                    case 26:
                        inputSampleR += rsZ;
                        inputSampleR *= 0.5f;
                        rsZ = inputSampleR;
                    case 27:
                        inputSampleR += rsY;
                        inputSampleR *= 0.5f;
                        rsY = inputSampleR;
                    case 28:
                        inputSampleR += rsX;
                        inputSampleR *= 0.5f;
                        rsX = inputSampleR;
                    case 29:
                        inputSampleR += rsW;
                        inputSampleR *= 0.5f;
                        rsW = inputSampleR;
                    case 30:
                        inputSampleR += rsV;
                        inputSampleR *= 0.5f;
                        rsV = inputSampleR;
                    case 31:
                        inputSampleR += rsU;
                        inputSampleR *= 0.5f;
                        rsU = inputSampleR;
                    case 32:
                        inputSampleR += rsT;
                        inputSampleR *= 0.5f;
                        rsT = inputSampleR;
                    case 33:
                        inputSampleR += rsS;
                        inputSampleR *= 0.5f;
                        rsS = inputSampleR;
                    case 34:
                        inputSampleR += rsR;
                        inputSampleR *= 0.5f;
                        rsR = inputSampleR;
                    case 35:
                        inputSampleR += rsQ;
                        inputSampleR *= 0.5f;
                        rsQ = inputSampleR;
                    case 36:
                        inputSampleR += rsP;
                        inputSampleR *= 0.5f;
                        rsP = inputSampleR;
                    case 37:
                        inputSampleR += rsO;
                        inputSampleR *= 0.5f;
                        rsO = inputSampleR;
                    case 38:
                        inputSampleR += rsN;
                        inputSampleR *= 0.5f;
                        rsN = inputSampleR;
                    case 39:
                        inputSampleR += rsM;
                        inputSampleR *= 0.5f;
                        rsM = inputSampleR;
                    case 40:
                        inputSampleR += rsL;
                        inputSampleR *= 0.5f;
                        rsL = inputSampleR;
                    case 41:
                        inputSampleR += rsK;
                        inputSampleR *= 0.5f;
                        rsK = inputSampleR;
                    case 42:
                        inputSampleR += rsJ;
                        inputSampleR *= 0.5f;
                        rsJ = inputSampleR;
                    case 43:
                        inputSampleR += rsI;
                        inputSampleR *= 0.5f;
                        rsI = inputSampleR;
                    case 44:
                        inputSampleR += rsH;
                        inputSampleR *= 0.5f;
                        rsH = inputSampleR;
                    case 45:
                        inputSampleR += rsG;
                        inputSampleR *= 0.5f;
                        rsG = inputSampleR;
                    case 46:
                        inputSampleR += rsF;
                        inputSampleR *= 0.5f;
                        rsF = inputSampleR;
                    case 47:
                        inputSampleR += rsE;
                        inputSampleR *= 0.5f;
                        rsE = inputSampleR;
                    case 48:
                        inputSampleR += rsD;
                        inputSampleR *= 0.5f;
                        rsD = inputSampleR;
                    case 49:
                        inputSampleR += rsC;
                        inputSampleR *= 0.5f;
                        rsC = inputSampleR;
                    case 50:
                        inputSampleR += rsB;
                        inputSampleR *= 0.5f;
                        rsB = inputSampleR;
                    case 51:
                        inputSampleR += rsA;
                        inputSampleR *= 0.5f;
                        rsA = inputSampleR;
                    case 52: break;
                }
                // end distance filter R
                pointAL = pointBL;
                pointBL = inputSampleL;
                pointAR = pointBR;
                pointBR = inputSampleR;
                pointCycle = 0.0f; // interpolation point values
                // this is the sample rate adjustment for the plugin version
                // game engine may well always run at 44.1k or whatever
            }
            inputSampleL = pointAL + ((pointBL - pointAL) * pointCycle);
            inputSampleR = pointAR + ((pointBR - pointAR) * pointCycle);
            // this is the linear interpolation between reverb points
            // governed by sample rate, to work as a plugin
            // begin 64 bit stereo floating point dither
            // int expon; frexp((double)inputSampleL, &expon);
            fpdL ^= fpdL << 13;
            fpdL ^= fpdL >> 17;
            fpdL ^= fpdL << 5;
            // inputSampleL += ((double(fpdL)-uint32_t(0x7fffffff)) * 3.553e-44l * pow(2,expon+62));
            // frexp((double)inputSampleR, &expon);
            fpdR ^= fpdR << 13;
            fpdR ^= fpdR >> 17;
            fpdR ^= fpdR << 5;
            if (fpdL - fpdR < 1073741824 || fpdR - fpdL < 1073741824) {
                fpdR ^= fpdR << 13;
                fpdR ^= fpdR >> 17;
                fpdR ^= fpdR << 5;
            }
            // inputSampleR += ((double(fpdR)-uint32_t(0x7fffffff)) * 3.553e-44l  * pow(2,expon+62));
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
} // namespace airwindohhs::ultralight2
