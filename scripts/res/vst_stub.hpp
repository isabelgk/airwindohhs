// Synthetic stand-in for the (proprietary, not vendored) Steinberg VST2 SDK
// headers that upstream airwindows plugin sources #include.
//
// Real audioeffectx.h isn't available to this generator, and libclang treats
// declarations built on an unresolved base class as invalid, which silently
// drops statements from the AST (e.g. `_canDo.insert(...)` calls vanish
// entirely). Providing a structurally-equivalent stub keeps every plugin's
// constructor/methods well-formed so libclang's AST reflects the real
// source. Only declarations grab's extractors need to resolve are included;
// method bodies are irrelevant since this generator never compiles or runs
// this code, only parses it to locate byte ranges in the original source.
#pragma once

typedef int VstInt32;
typedef long VstIntPtr;
typedef VstIntPtr (*audioMasterCallback)(void* effect, VstInt32 opcode, VstInt32 index,
                                          VstIntPtr value, void* ptr, float opt);

enum VstPlugCategory {
    kPlugCategUnknown = 0,
    kPlugCategEffect,
    kPlugCategSynth,
    kPlugCategAnalysis,
    kPlugCategMastering,
    kPlugCategSpacializer,
    kPlugCategRoomFx,
    kPlugSurroundFx,
    kPlugCategRestoration,
    kPlugCategOfflineProcess,
    kPlugCategShell,
    kPlugCategGenerator,
};

const int kVstMaxProgNameLen = 24;
const int kVstMaxParamStrLen = 8;
const int kVstMaxVendorStrLen = 64;
const int kVstMaxProductStrLen = 64;
const int kVstMaxEffectNameLen = 32;

class AudioEffect {
public:
    AudioEffect(audioMasterCallback audioMaster, VstInt32 numPrograms, VstInt32 numParams) {}
    virtual ~AudioEffect() {}

    void setNumInputs(VstInt32) {}
    void setNumOutputs(VstInt32) {}
    void setUniqueID(unsigned long) {}
    void canProcessReplacing() {}
    void canDoubleReplacing() {}
    void programsAreChunks(bool = true) {}
    void isSynth(bool = true) {}
    void noTail(bool = true) {}
    void hasVu(bool = true) {}
    void hasClip(bool = true) {}
    void setParameterAutomated(VstInt32, float) {}
    float getSampleRate() { return 0.0f; }
};

class AudioEffectX : public AudioEffect {
public:
    AudioEffectX(audioMasterCallback audioMaster, VstInt32 numPrograms, VstInt32 numParams)
        : AudioEffect(audioMaster, numPrograms, numParams) {}

    void vst_strncpy(char* dst, const char* src, VstInt32 n) {}
    void float2string(float value, char* text, VstInt32 n) {}
    void double2string(double value, char* text, VstInt32 n) {}
    void int2string(VstInt32 value, char* text, VstInt32 n) {}
    void dB2string(float value, char* text, VstInt32 n) {}
    float string2float(char* text) { return 0.0f; }
    bool getSpeakerArrangement(void*, void*) { return false; }
};

extern "C++" AudioEffect* createEffectInstance(audioMasterCallback audioMaster);
