#include "stdint.h"

#include "GeneralMemoryAllocator.h"
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

  new (&engineMkI) EngineMkI;

  new (&globalPatch) Dx7Patch;
  globalPatch.update((const uint8_t *)init_voice);

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
