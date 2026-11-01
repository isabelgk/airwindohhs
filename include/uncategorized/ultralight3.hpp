#pragma once
#include "effect.hpp"
#include <cmath>
#include <cstdint>
#include <cstdlib>

namespace airwindohhs::ultralight3 {

constexpr std::string_view k_name{ "Ultralight3" };
constexpr std::string_view k_short_description{
    ""
};
constexpr std::string_view k_long_description{
    ""
};
constexpr std::string_view k_tags{
    "uncategorized"
};

template <typename T>
class Ultralight3 final : public Effect<T>
{
    static constexpr int d4A = 1626;
    static constexpr int d4B = 650;
    static constexpr int d4C = 470;
    static constexpr int d4D = 47;
    static constexpr int d4E = 1149;
    static constexpr int d4F = 575;
    static constexpr int d4G = 456;
    static constexpr int d4H = 52;
    static constexpr int d4I = 1756;
    static constexpr int d4J = 888;
    static constexpr int d4K = 173;
    static constexpr int d4L = 49;
    static constexpr int d4M = 1798;
    static constexpr int d4N = 776;
    static constexpr int d4O = 152;
    static constexpr int d4P = 21;
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
    static constexpr unsigned long kUniqueId = 'utl3';
    float A;
    float B;
    float C;
    float D;
    float E;
    float F;
    float G;
    float H;
    float aA[d4A + 5]; // begin Ultralight reverb node
    float aB[d4B + 5];
    float aC[d4C + 5];
    float aD[d4D + 5];
    float aE[d4E + 5];
    float aF[d4F + 5];
    float aG[d4G + 5];
    float aH[d4H + 5];
    float aI[d4I + 5];
    float aJ[d4J + 5];
    float aK[d4K + 5];
    float aL[d4L + 5];
    float aM[d4M + 5];
    float aN[d4N + 5];
    float aO[d4O + 5];
    float aP[d4P + 5];
    float aQ[d4Q + 5];
    float aR[d4R + 5];
    float aS[d4S + 5];
    float aT[d4T + 5];
    int cA, cB, cC, cD, cE, cF, cG, cH;
    int cI, cJ, cK, cL, cM, cN, cO, cP;
    int cQ, cR, cS, cT; // QRST are the allpasses taking the place of predelay
    float oQ, oR, oS, oT; // oQRST are the outputs of the allpasses
    float fA, fB, fC, fD;
    float fhA, fhB, fhC, fhD;
    float flA, flB, flC, flD;
    float hA, hB, hC, hD, hE, hF, hG, hH, hI; // end Ultralight reverb node
    // FastDistance2 on each sound source position, independently.
    // If using Ultralight in a game engine, it can produce four
    // locations from one reverb node, and rather than mix them
    // before outputting to the engine, you get to place them
    // in a square and if you're inside the square, rotating will
    // pivot the spatialization of the single reverb node.
    // Combined with using many nodes this is a dense sound field.
    float fdLA, fdLB, fdLC, fdLD, fdLE, fdLF, fdLG, fdLH;
    float fdRA, fdRB, fdRC, fdRD, fdRE, fdRF, fdRG, fdRH;
    float pointCycle, pointAL, pointBL, pointAR, pointBR;
    // this is a simple linear interpolation sample rate routine for the plugin version
    uint32_t fpdL;
    uint32_t fpdR;
    // default stuff

  public:
    Ultralight3()
    {
        A = 0.3;
        B = 0.0;
        C = 0.0;
        D = 0.0;
        E = 0.5;
        F = 0.0;
        G = 0.0;
        H = 0.0;
        for (int x = 0; x < d4A + 2; x++) {
            aA[x] = 0.0f;
        }
        for (int x = 0; x < d4B + 2; x++) {
            aB[x] = 0.0f;
        }
        for (int x = 0; x < d4C + 2; x++) {
            aC[x] = 0.0f;
        }
        for (int x = 0; x < d4D + 2; x++) {
            aD[x] = 0.0f;
        }
        for (int x = 0; x < d4E + 2; x++) {
            aE[x] = 0.0f;
        }
        for (int x = 0; x < d4F + 2; x++) {
            aF[x] = 0.0f;
        }
        for (int x = 0; x < d4G + 2; x++) {
            aG[x] = 0.0f;
        }
        for (int x = 0; x < d4H + 2; x++) {
            aH[x] = 0.0f;
        }
        for (int x = 0; x < d4I + 2; x++) {
            aI[x] = 0.0f;
        }
        for (int x = 0; x < d4J + 2; x++) {
            aJ[x] = 0.0f;
        }
        for (int x = 0; x < d4K + 2; x++) {
            aK[x] = 0.0f;
        }
        for (int x = 0; x < d4L + 2; x++) {
            aL[x] = 0.0f;
        }
        for (int x = 0; x < d4M + 2; x++) {
            aM[x] = 0.0f;
        }
        for (int x = 0; x < d4N + 2; x++) {
            aN[x] = 0.0f;
        }
        for (int x = 0; x < d4O + 2; x++) {
            aO[x] = 0.0f;
        }
        for (int x = 0; x < d4P + 2; x++) {
            aP[x] = 0.0f;
        }
        for (int x = 0; x < d4Q + 2; x++) {
            aQ[x] = 0.0f;
        }
        for (int x = 0; x < d4R + 2; x++) {
            aR[x] = 0.0f;
        }
        for (int x = 0; x < d4S + 2; x++) {
            aS[x] = 0.0f;
        }
        for (int x = 0; x < d4T + 2; x++) {
            aT[x] = 0.0f;
        }
        cA = cB = cC = cD = cE = cF = cG = cH = 1;
        cI = cJ = cK = cL = cM = cN = cO = cP = 1;
        cQ = cR = cS = cT = 1;
        oQ = oR = oS = oT = 0.0f;
        fA = fB = fC = fD = 0.0f;
        fhA = fhB = fhC = fhD = 0.0f;
        flA = flB = flC = flD = 0.0f;
        hA = hB = hC = hD = hE = hF = hG = hH = hI = 0.0f;
        fdLA = fdLB = fdLC = fdLD = fdLE = fdLF = fdLG = fdLH = 0.0;
        fdRA = fdRB = fdRC = fdRD = fdRE = fdRF = fdRG = fdRH = 0.0;
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
        kParamG = 6,
        kParamH = 7,
        kNumParameters = 8
    };

    void set_parameter_value(int index, float value)
    {
        switch (index) {
            case kParamA: A = value; break;
            case kParamB: B = value; break;
            case kParamC: C = value; break;
            case kParamD: D = value; break;
            case kParamE: E = value; break;
            case kParamF: F = value; break;
            case kParamG: G = value; break;
            case kParamH: H = value; break;
            default: break; // unknown parameter, shouldn't happen!
        }
    }

    float get_parameter_value(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return A; break;
            case kParamB: return B; break;
            case kParamC: return C; break;
            case kParamD: return D; break;
            case kParamE: return E; break;
            case kParamF: return F; break;
            case kParamG: return G; break;
            case kParamH: return H; break;

            default: break;
        }
        return 0.0;
    }

    T get_parameter_default(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return 0.3; break;
            case kParamB: return 0.0; break;
            case kParamC: return 0.0; break;
            case kParamD: return 0.0; break;
            case kParamE: return 0.5; break;
            case kParamF: return 0.0; break;
            case kParamG: return 0.0; break;
            case kParamH: return 0.0; break;

            default: break;
        }
        return 0.0;
    }

    constexpr std::string_view get_parameter_name(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "damping"; break;
            case kParamB: return "unsolid"; break;
            case kParamC: return "unreflc"; break;
            case kParamD: return "predlay"; break;
            case kParamE: return "drywet"; break;
            case kParamF: return "frntbk"; break;
            case kParamG: return "bottop"; break;
            case kParamH: return "distnce"; break;

            default: break;
        }
        return {};
    }

    constexpr std::string_view get_parameter_title(int index)
    {
        switch (static_cast<params>(index)) {
            case kParamA: return "Damping"; break;
            case kParamB: return "UnSolid"; break;
            case kParamC: return "UnReflc"; break;
            case kParamD: return "Predlay"; break;
            case kParamE: return "Dry/Wet"; break;
            case kParamF: return "Frnt/Bk"; break;
            case kParamG: return "Bot/Top"; break;
            case kParamH: return "Distnce"; break;

            default: break;
        }
        return {};
    }

    std::string get_parameter_display(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: int2string((int)(A * 9.99), text, kVstMaxParamStrLen); break;
            case kParamB: int2string((int)(B * 4.99), text, kVstMaxParamStrLen); break;
            case kParamC: int2string((int)(C * 4.99), text, kVstMaxParamStrLen); break;
            case kParamD: int2string((int)(D * 998.0), text, kVstMaxParamStrLen); break;
            case kParamE: float2string(E, text, kVstMaxParamStrLen); break;
            case kParamF: float2string(F, text, kVstMaxParamStrLen); break;
            case kParamG: float2string(G, text, kVstMaxParamStrLen); break;
            case kParamH: float2string(H, text, kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        } // this displays the values and handles 'popups' where it's discrete choices

        return text;
    }

    std::string get_parameter_label(int index) const
    {
        std::string text;
        switch (index) {
            case kParamA: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamB: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamC: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamD: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamE: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamF: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamG: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            case kParamH: vst_strncpy(text, "", kVstMaxParamStrLen); break;
            default: break; // unknown parameter, shouldn't happen!
        }

        return text;
    }

    void process(T** inputs, T** outputs, long sampleFrames)
    {
        T* in1 = inputs[0];
        T* in2 = inputs[1];
        T* out1 = outputs[0];
        T* out2 = outputs[1];

        double overallscale = 1.0;
        overallscale /= 48000.0;
        overallscale *= Effect<T>::getSampleRate();
        float pointDerez = 1.0f / overallscale; // forcing it to be simple
        bool bypassPre = (((int)(D * 998.0f)) < 1); // plugin only
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
        float wet = E; // plugin only
        float back = F; // plugin only
        float ceiling = G; // plugin only
        // this plugin applies distance filter to dry AND wet, so that when testing
        // out possible spaces for the game engine, you can hear what it'll sound like
        // in practice when both the source sound and the verb are distant.
        float distance = pow(1.0f - H, overallscale + 1.0f);
        // distance is not a property of the reverb node, it's a property of
        // the game audio engine, where every source can be localized
        while (--sampleFrames >= 0) {
            double inputSampleL = *in1;
            double inputSampleR = *in2;
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
                float inputSample = (inputSampleL + inputSampleR) * 0.5f;
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
                oQ = oR - (aQ[(cQ + 1) - ((cQ + 1 > dpQ) ? dpQ + 1 : 0)] * 0.5f);
                oR = oS - (aR[(cR + 1) - ((cR + 1 > dpR) ? dpR + 1 : 0)] * 0.5f);
                oS = oT - (aS[(cS + 1) - ((cS + 1 > dpS) ? dpS + 1 : 0)] * 0.5f);
                oT = inputSample; // SIMDable, but aT[] is just a simple delay
                aQ[cQ] = oQ;
                aR[cR] = oR;
                aS[cS] = oS;
                aT[cT] = oT;
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
                oQ = oQ * 0.5f + aQ[cQ];
                oR = oR * 0.5f + aR[cR];
                oS = oS * 0.5f + aS[cS];
                oT = oT * 0.0f + aT[cT]; // not allpass but can use vector fma
                if (bypassPre) {
                    oQ = inputSample; // plugin only
                }
                aA[cA] = fA * damping + oQ;
                aB[cB] = fB * damping + oQ;
                aC[cC] = fC * damping + oQ;
                aD[cD] = fD * damping + oQ;
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
                hA = aA[cA];
                hB = aB[cB];
                hC = aC[cC];
                hD = aD[cD];
                hI = -(hA + hB + hC + hD);
                aE[cE] = hA * 2.0f + hI;
                aF[cF] = hB * 2.0f + hI;
                aG[cG] = hC * 2.0f + hI;
                aH[cH] = hD * 2.0f + hI;
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
                hA = aE[cE];
                hB = aF[cF];
                hC = aG[cG];
                hD = aH[cH];
                hI = -(hA + hB + hC + hD);
                aI[cI] = hA * 2.0f + hI;
                aJ[cJ] = hB * 2.0f + hI;
                aK[cK] = hC * 2.0f + hI;
                aL[cL] = hD * 2.0f + hI;
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
                hA = aI[cI];
                hB = aJ[cJ];
                hC = aK[cK];
                hD = aL[cL];
                hI = -(hA + hB + hC + hD); // we have our ground level 4 outputs
                aM[cM] = hA * 2.0f + hI; // function of the reverb is unaffected,
                aN[cN] = hB * 2.0f + hI; // but on the first pass through, this
                aO[cO] = hC * 2.0f + hI; // will be slightly quicker and produce
                aP[cP] = hD * 2.0f + hI; // fewer early reflections.
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
                hE = aM[cM];
                hF = aN[cN];
                hG = aO[cO];
                hH = aP[cP];
                hI = -(hE + hF + hG + hH); // we have our overhead 4 outputs
                fA = hE * 2.0f + hI; // using these as the added four outputs means
                fB = hF * 2.0f + hI; // there's a whole other layer of delays in there
                fC = hG * 2.0f + hI; // that are distinct delays which don't match
                fD = hH * 2.0f + hI; // the other four. Verb still unaffected.
                hA *= 0.25f;
                hB *= 0.25f;
                hC *= 0.25f;
                hD *= 0.25f;
                hE *= 0.125f;
                hF *= 0.125f;
                hG *= 0.125f;
                hH *= 0.125f;
                // hA through hD are now the floor reverb outputs. These are
                // fewer echoes and earlier reflections than the final ones.
                // hE through hH are now the ceiling outputs. These are
                // the final outputs with more reflections, added delays.
                // They all use the full matrix to get to feedback, so it ends
                // up as the same reverb tail no matter where you tap in.
                // The difference in gain is because of level differences from
                // different places in the matrix.
                // In stereo, you can monitor either pair, or fold them down
                // to left and right in the plugin. In an actual game engine,
                // these are positioned in a square, so spatialization is what
                // folds them down to stereo or binaural or whatever you need.
                // end of just the reverb node part
                switch (unSolid) {
                    case 0:
                        fhA *= 0.25f;
                        fA -= fhA * 0.125f;
                        fhA += fA;
                    case 1:
                        fhB *= 0.25f;
                        fB -= fhB * 0.125f;
                        fhB += fB;
                    case 2:
                        fhC *= 0.25f;
                        fC -= fhC * 0.125f;
                        fhC += fC;
                    case 3:
                        fhD *= 0.25f;
                        fD -= fhD * 0.125f;
                        fhD += fD;
                    case 4:
                        break;
                }
                switch (unReflect) {
                    case 0:
                        fD = (fD + flD) * 0.5f;
                        flD = fD;
                    case 1:
                        fC = (fC + flC) * 0.5f;
                        flC = fC;
                    case 2:
                        fB = (fB + flB) * 0.5f;
                        flB = fB;
                    case 3:
                        fA = (fA + flA) * 0.5f;
                        flA = fA;
                    case 4:
                        break;
                }
                if (bypassVerb) {
                    inputSampleL = inputSampleR = oQ; // plugin only
                }
                inputSampleL = (hC * back * (1.0f - ceiling)) + (hA * (1.0f - ceiling) * (1.0f - back));
                inputSampleR = (hD * back * (1.0f - ceiling)) + (hB * (1.0f - ceiling) * (1.0f - back));
                inputSampleL += (hG * back * ceiling) + (hE * ceiling * (1.0f - back));
                inputSampleR += (hH * back * ceiling) + (hF * ceiling * (1.0f - back));
                // dry/wet is not part of the game engine reverb node
                inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f - wet));
                inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f - wet));
                // plugin only: game engine one would have just the reverb,
                // and every sound source could have the FastDistance2 filter,
                // including the localized reverb nodes. The reason it's here
                // is so the plugin can preview how dry+reverb, both distant, will sound.
                pointAL = pointBL;
                pointBL = inputSampleL;
                pointAR = pointBR;
                pointBR = inputSampleR;
                pointCycle = 0.0f; // interpolation point values
                // this is the sample rate adjustment for the plugin version
            }
            inputSampleL = pointAL + ((pointBL - pointAL) * pointCycle);
            inputSampleR = pointAR + ((pointBR - pointAR) * pointCycle);
            // this is the linear interpolation between reverb points
            // governed by sample rate, to work as a plugin.
            // For use in a game engine, I suggest running the whole reverb
            // system undersampled in the event of high sample rate, all the nodes and
            // each node's FastDistance2 filter. Even simple linear interpolation will
            // handle this just fine, or the gane engine can run one instance of my
            // bezier undersampling like in my plugin reverbs. In pretty much every
            // case, the game system will NOT be running high sample rate, and it's
            // super wasted on distance-filtered reverbs.
            // begin FastDistance2
            inputSampleL *= 0.5f; // because of the ending fdC+fdD, output is
            inputSampleR *= 0.5f; // 6dB up from input. In game this can be built
            // into the reverb probe's input as that will have a distance attenuation
            // already as part of its normal operation.
            // for FastDistance2, the alterations from a pure cascading stack
            // of SVFs is that the first one's at a slightly different frequency
            // to more gracefully go from very close to medium distance, and the
            // output is the sum of the last two stages, which causes a subtle roll-off
            // right up at the very top of the frequency range while also altering
            // the shape of the filter to be more like original FastDistance.
            fdLE = distance * 0.875f * (inputSampleL - fdLA - fdLE) + fdLE;
            fdLF = distance * (fdLA - fdLB - fdLF) + fdLF;
            fdLG = distance * (fdLB - fdLC - fdLG) + fdLG;
            fdLH = distance * (fdLC - fdLD - fdLH) + fdLH;
            fdLA = distance * 0.875f * fdLE + fdLA;
            fdLB = distance * fdLF + fdLB;
            fdLC = distance * fdLG + fdLC;
            fdLD = distance * fdLH + fdLD;
            inputSampleL = fdLC + fdLD;
            fdRE = distance * 0.875f * (inputSampleR - fdRA - fdRE) + fdRE;
            fdRF = distance * (fdRA - fdRB - fdRF) + fdRF;
            fdRG = distance * (fdRB - fdRC - fdRG) + fdRG;
            fdRH = distance * (fdRC - fdRD - fdRH) + fdRH;
            fdRA = distance * 0.875f * fdRE + fdRA;
            fdRB = distance * fdRF + fdRB;
            fdRC = distance * fdRG + fdRC;
            fdRD = distance * fdRH + fdRD;
            inputSampleR = fdRC + fdRD;
            // end FastDistance2
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
} // namespace airwindohhs::ultralight3
