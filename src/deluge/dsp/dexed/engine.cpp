#include "definitions_cxx.hpp"
#include "stdint.h"

#include "EngineMkI.h"
#include "dx7note.h"
#include "engine.h"
#include "math_lut.h"
#include "memory/memory_allocator_interface.h"
#include <new>

#ifdef USE_DEXED_MK1
EngineMkI *Dexed::engineMkI;
#endif
// static data is not safe
FmCore *Dexed::engineModern;

int32_t Dexed::lfo_unit;

static bool did_init_engine = false;

// TODO: does not work. maybe YAGNI?
// #define DEXED_PREALLOC
#ifdef DEXED_PREALLOC
//DxVoice dxVoices[kNumVoiceSamplesStatic];
DxVoice *dxVoices;
DxVoice* firstUnassignedDxVoice;
#endif

const uint8_t init_voice[] = {
    99, 99, 99, 99, 99, 99, 99, 00, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  7,  99, 99, 99, 99, 99,
    99, 99, 00, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  7,  99, 99, 99, 99, 99, 99, 99, 00, 0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  7,  99, 99, 99, 99, 99, 99, 99, 00, 0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  1,  0,  7,  99, 99, 99, 99, 99, 99, 99, 00, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,
    7,  99, 99, 99, 99, 99, 99, 99, 00, 0,  0,  0,  0,  0,  0,  0,  0,  99, 0,  1,  0,  7,  99, 99, 99, 99,
    50, 50, 50, 50, 0,  0,  1,  35, 0,  0,  0,  1,  0,  3,  24, 73, 78, 73, 84, 32, 86, 79, 73, 67, 69, 0};

static void init_engine(void) {
	dexed_init_lut_data(); // TODO: later
	PitchEnv::init(44100);
	Env::init_sr(44100);
	Dexed::lfo_unit = (int32_t)(25190424 / (float)44100 + 0.5);

#ifdef DEXED_PREALLOC
	void *voicemem = allocMaxSpeed(kNumVoiceSamplesStatic*sizeof(DxVoice));
	dxVoices = new (voicemem) DxVoice[kNumVoiceSamplesStatic];
	firstUnassignedDxVoice = dxVoices;

	for (int i = 0; i < kNumVoiceSamplesStatic-1; i++) {
		dxVoices[i].nextUnassigned = &dxVoices[i+1];
	}
#endif


	// TODO: these are not large in memory. but static construction in dsp library
	// is currently not safe
	void *engineMem = allocMaxSpeed(sizeof(FmCore));
	Dexed::engineModern = new (engineMem) FmCore();

#ifdef USE_DEXED_MK1
	engineMem = allocMaxSpeed(sizeof(EngineMkI));
	Dexed::engineMkI = new (engineMem) EngineMkI();
#endif

}

DxVoice* Dexed::solicitDxVoice() {
#ifdef DEXED_PREALLOC
	if (firstUnassignedDxVoice) {
		DxVoice* toReturn = firstUnassignedDxVoice;
		firstUnassignedDxVoice = firstUnassignedDxVoice->nextUnassigned;
		toReturn->preallocated = true;
		return toReturn;
	}
#endif
	void* memory = allocMaxSpeed(sizeof(DxVoice));
	if (!memory)
		return nullptr;

	return new (memory) DxVoice();
}

void Dexed::dxVoiceUnassigned(DxVoice* dxVoice) {
#ifdef DEXED_PREALLOC
	if (dxVoice->preallocated) {
		dxVoice->nextUnassigned = firstUnassignedDxVoice;
		firstUnassignedDxVoice = dxVoice;
		return;
	}
#endif
	 delugeDealloc(dxVoice);
}

int  standardNoteToFreq(int note) {
	 const int base = 50857777; // (1 << 24) * (log(440) / log(2) - 69/12)
	const int step = (1 << 24) / 12;
	return base + step * note;
}

Dx7Patch* Dexed::newPatch(void) {
	if (!did_init_engine) {
		init_engine();
		did_init_engine = true;
	}

	void* memory = allocMaxSpeed(sizeof(Dx7Patch));
	return new (memory) Dx7Patch;
}
