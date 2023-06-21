#include "definitions_cxx.hpp"
#include "stdint.h"

#include "EngineMkI.h"
#include "dx7note.h"
#include "engine.h"
#include "math_lut.h"
#include "memory/memory_allocator_interface.h"
#include <new>


DxEngine* dxEngine = nullptr;

static void init_engine(void) {
	void *engineMem = allocMaxSpeed(sizeof(DxEngine));
	dxEngine = new (engineMem) DxEngine();

	dx_init_lut_data();

	PitchEnv::init(44100);
	Env::init_sr(44100);
}


DxEngine::DxEngine () {
#ifdef DX_PREALLOC
	firstUnassignedDxVoice = &dxVoices[0];

	for (int i = 0; i < kNumVoiceSamplesStatic-1; i++) {
		dxVoices[i].nextUnassigned = &dxVoices[i+1];
	}
#endif
}

DxVoice* DxEngine::solicitDxVoice() {
#ifdef DX_PREALLOC
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

void DxEngine::dxVoiceUnassigned(DxVoice* dxVoice) {
#ifdef DX_PREALLOC
	if (dxVoice->preallocated) {
		dxVoice->nextUnassigned = firstUnassignedDxVoice;
		firstUnassignedDxVoice = dxVoice;
		return;
	}
#endif
	 delugeDealloc(dxVoice);
}

static int standardNoteToFreq(int note) {
	 const int base = 50857777; // (1 << 24) * (log(440) / log(2) - 69/12)
	const int step = (1 << 24) / 12;
	return base + step * note;
}

DxPatch* DxEngine::newPatch(void) {
	void* memory = allocMaxSpeed(sizeof(DxPatch));
	return new (memory) DxPatch;
}

// TODO: static initialization should be allowed.
DxEngine *getDxEngine() {
	if (dxEngine == nullptr) {
		init_engine();
	}
	return dxEngine;
}
