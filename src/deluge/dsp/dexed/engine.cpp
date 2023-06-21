#include "stdint.h"

#include "EngineMkI.h"
#include "dx7note.h"
#include "engine.h"
#include "math_lut.h"
#include "memory/general_memory_allocator.h"
#include <new>

#ifdef USE_DEXED_MK1
EngineMkI Dexed::engineMkI;
#endif
FmCore Dexed::engineModern;
int32_t Dexed::lfo_unit;

static bool did_init_engine = false;

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
}

DxVoice dxVoices[kNumVoiceSamplesStatic];
DxVoice* firstUnassignedDxVoice = dxVoices;

DxVoice* Dexed::solicitDxVoice() {
	if (firstUnassignedDxVoice) {
		DxVoice* toReturn = firstUnassignedDxVoice;
		firstUnassignedDxVoice = firstUnassignedDxVoice->nextUnassigned;
		return toReturn;
	}
	else {
		void* memory = GeneralMemoryAllocator::get().allocMaxSpeed(sizeof(DxVoice));
		if (!memory)
			return nullptr;

		return new (memory) DxVoice();
	}
}

void Dexed::dxVoiceUnassigned(DxVoice* dxVoice) {
	if (dxVoice >= dxVoices && dxVoice < &dxVoices[kNumVoiceSamplesStatic]) {
		dxVoice->nextUnassigned = firstUnassignedDxVoice;
		firstUnassignedDxVoice = dxVoice;
	}
	else {
		GeneralMemoryAllocator::get().dealloc(dxVoice);
	}
}

int standardNoteToFreq(int note) {
	const int base = 50857777; // (1 << 24) * (log(440) / log(2) - 69/12)
	const int step = (1 << 24) / 12;
	return base + step * note;
}

Dx7Patch* Dexed::newPatch(void) {
	if (!did_init_engine) {
		init_engine();
		did_init_engine = true;
	}

	void* memory = GeneralMemoryAllocator::get().allocMaxSpeed(sizeof(Dx7Patch));
	return new (memory) Dx7Patch;
}
