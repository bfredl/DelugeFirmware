#pragma once

#include "EngineMkI.h"
#include "dx7note.h"
#include "definitions_cxx.hpp"

// seems to cause crashes... maybe YAGNI
// #define DX_PREALLOC

class DxEngine {
public:

	DxEngine();
#define EXP2_LG_N_SAMPLES 10
#define EXP2_N_SAMPLES (1 << EXP2_LG_N_SAMPLES)
	int32_t exp2tab[EXP2_N_SAMPLES << 1];

#define TANH_LG_N_SAMPLES 10
#define TANH_N_SAMPLES (1 << TANH_LG_N_SAMPLES)
	int32_t tanhtab[TANH_N_SAMPLES << 1];

// Use twice as much RAM for the LUT but avoid a little computation
#define SIN_DELTA

#define SIN_LG_N_SAMPLES 10
#define SIN_N_SAMPLES (1 << SIN_LG_N_SAMPLES)
#ifdef SIN_DELTA
	int32_t sintab[SIN_N_SAMPLES << 1];
#else
	int32_t sintab[SIN_N_SAMPLES + 1];
#endif

#define FREQ_LG_N_SAMPLES 10
#define FREQ_N_SAMPLES (1 << FREQ_LG_N_SAMPLES)
	int32_t freq_lut[FREQ_N_SAMPLES + 1];

#ifdef DX_PREALLOC
DxVoice dxVoices[kNumVoiceSamplesStatic];
DxVoice* firstUnassignedDxVoice;
#endif

#ifdef USE_DEXED_MK1
	EngineMkI engineMkI;
#endif
	FmCore engineModern;

	DxVoice* solicitDxVoice();
	void dxVoiceUnassigned(DxVoice* dxVoice);
	DxPatch* newPatch();
};

extern DxEngine* dxEngine;

DxEngine *getDxEngine();

DxPatch* newPatch();
