#pragma once

// #define USE_DEXED_MK1

#include "dx7note.h"
#include "EngineMkI.h"

class Source;
typedef Dx7Note DxVoice;
namespace Dexed {
	void init();

#ifdef USE_DEXED_MK1
	extern EngineMkI engineMkI;
#endif
	extern FmCore engineModern;
	extern int32_t lfo_unit;

	DxVoice* solicitDxVoice();
	void dxVoiceUnassigned(DxVoice* dxVoice);

	void writeDxPatch(Dx7Patch *patch);
	void readDxPatch(Dx7Patch **patch);
	Dx7Patch * ensurePatch(Source *s);
    Dx7Patch *newPatch();
	extern Dx7Patch globalPatch;
};
