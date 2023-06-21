#include "stdint.h"

#include "memory/general_memory_allocator.h"
#include "storage/storage_manager.h"
#include "sin.h"
#include "exp2.h"
#include "dx7note.h"
#include "EngineMkI.h"
#include "PluginData.h"
#include "freqlut.h"
#include "engine.h"
#include <new>
#include "dexed/dexeditor.h"

EngineMkI Dexed::engineMkI;
FmCore Dexed::engineModern;
	Dx7Patch Dexed::globalPatch;
int32_t Dexed::lfo_unit;

void Dexed::init(void) {
  Exp2::init();
  Tanh::init();
  Sin::init();
  Freqlut::init(44100);
  PitchEnv::init(44100);
  Env::init_sr(44100);
  lfo_unit = (int32_t)(25190424 / (float)44100 + 0.5);
}

DxVoice dxVoices[NUM_VOICE_SAMPLES_STATIC];
DxVoice* firstUnassignedDxVoice = dxVoices;

DxVoice* Dexed::solicitDxVoice() {
	if (firstUnassignedDxVoice) {
		DxVoice* toReturn = firstUnassignedDxVoice;
		firstUnassignedDxVoice = firstUnassignedDxVoice->nextUnassigned;
		return toReturn;
	}
	else {
		void* memory = generalMemoryAllocator.alloc(sizeof(DxVoice), NULL, false, true);
		if (!memory) return NULL;

		return new (memory) DxVoice();
	}
}

void Dexed::dxVoiceUnassigned(DxVoice* dxVoice) {
	if (dxVoice >= dxVoices && dxVoice < &dxVoices[NUM_VOICE_SAMPLES_STATIC]) {
		dxVoice->nextUnassigned = firstUnassignedDxVoice;
		firstUnassignedDxVoice = dxVoice;
	}
	else {
		generalMemoryAllocator.dealloc(dxVoice);
	}
}

int standardNoteToFreq(int note) {
    const int base = 50857777;  // (1 << 24) * (log(440) / log(2) - 69/12)
    const int step = (1 << 24) / 12;
    return base + step * note;
}

void Dexed::writeDxPatch(Dx7Patch *patch) {
	storageManager.writeAttributeHexBytes("dx7patch", patch->currentPatch, 155);
}

void Dexed::readDxPatch(Dx7Patch **patch) {
  if (*patch == NULL) {
	void* memory = generalMemoryAllocator.alloc(sizeof(Dx7Patch), NULL, false, true);
	*patch = new(memory) Dx7Patch;
  }

	storageManager.readTagOrAttributeValueHexBytes((*patch)->currentPatch, 155);
}

