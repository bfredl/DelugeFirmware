#include "stdint.h"

#include "GeneralMemoryAllocator.h"
#include "storagemanager.h"
#include "sin.h"
#include "exp2.h"
#include "lfo.h"
#include "controllers.h"
#include "dx7note.h"
#include "EngineMkI.h"
#include "PluginData.h"
#include "freqlut.h"
#include "engine.h"
#include <new>
#include "dexed/dexeditor.h"

EngineMkI engineMkI;

	Dx7Patch Dexed::globalPatch;
	Controllers Dexed::dummy_controller;
void Dexed::init(void) {
  Exp2::init();
  Tanh::init();
  Sin::init();
  Freqlut::init(44100);
  PitchEnv::init(44100);
  Env::init_sr(44100);
  new (&dx7ui) Dx7UI;

  new (&engineMkI) EngineMkI;

  new (&globalPatch) Dx7Patch;

  static uint8_t fin_voice[155] = {
99,48,1,24,94,79,0,0,60,54,0,1,0,7,0,3,99,0,15,0,8,95,35,8,28,99,70,0,0,52,0,35,3,0,4,0,4,79,0,3,0,11,95,32,25,33,99,70,0,0,0,0,0,0,0,3,3,5,99,0,1,0,3,85,50,2,10,99,70,0,0,58,0,37,0,0,3,0,0,79,0,9,0,1,75,46,16,12,99,70,0,0,52,8,0,3,0,3,0,3,79,0,2,0,5,74,30,27,31,99,70,0,0,60,0,0,0,0,3,3,5,99,0,1,0,12,94,67,95,60,50,50,50,50,2,6,1,34,33,0,0,1,4,1,12,67,104,114,111,109,97,32,53,32,92,
};

  globalPatch.update(init_voice);

  controller_prepare(&dummy_controller);
}


void Dexed::controller_prepare(Controllers * controllers) {
    controllers->masterTune = 0;
    controllers->modwheel_cc = 0;
    controllers->foot_cc = 0;
    controllers->breath_cc = 0;
    controllers->aftertouch_cc = 0;
    controllers->refresh();

    controllers->core = (FmCore *)&engineMkI;
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
	memcpy((*patch)->currentPatch, Dexed::globalPatch.currentPatch, sizeof Dexed::globalPatch.currentPatch);
  }

	storageManager.readTagOrAttributeValueHexBytes((*patch)->currentPatch, 155);
}

#if 0
int not_main() {
  engine_init();

  Controllers controllers;
  controller_prepare(&controllers);

  Cartridge cart;

  uint8_t bufer[4104];
  int size;
  //FILE *syx = fopen(argv[1], "rb");
  // int size = fread(bufer, 1, 4104, syx);
  // fprintf(stderr, "siz: %d\n", size);
  int status = cart.load(bufer, size);
  //fprintf(stderr, "lesa: %d\n", status);

  char dest[32][11];
  cart.getProgramNames(dest);
  for (int i = 0; i <32; i++) {
    // fprintf(stderr, "prog %d: %s\n", i, dest[i]);
  }

  Dx7Patch patch;
  Dx7Note note;
  Lfo lfo;

  uint8_t data[161];
  int num = 0;
  cart.unpackProgram(data, num);

  int velo = 100;
  int midinote = 60;
  int pitch = standardNoteToFreq(midinote);
  // memcpy(data, init_voice, 155);

  patch.update(data);
  note.init(patch, midinote, pitch, velo);
  if (data[136] ) {
    note.oscSync();
  }
  lfo.reset(data + 137);
  // note.update(data, midinote, pitch, velo);

  const int N = 37;

  const int numSamples = N*4410;
  int16_t audiobuf[numSamples];
  for (int i = 0; i < numSamples; i += N) {
    int32_t computebuf[N];
    memset(computebuf, 0, sizeof computebuf);
    note.compute(computebuf, N, 0, 0, 0, &controllers);
    for (int j = 0; j < N; j++) {
        int32_t lfovalue = lfo.getsample(N);
        int32_t lfodelay = lfo.getdelay(N);
        int val = computebuf[j] >> 4;
        int clip_val = val < -(1 << 24) ? -0x8000 : val >= (1 << 24) ? 0x7fff : val >> 9;
        audiobuf[i+j] = clip_val;
    }
  }
}
#endif
