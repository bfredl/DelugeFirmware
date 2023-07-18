#pragma once
#include "dx7note.h"
#include "EngineMkI.h"

class Source;
typedef Dx7Note DxVoice;
namespace Dexed {
	void init();

	extern EngineMkI engineMkI;
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
