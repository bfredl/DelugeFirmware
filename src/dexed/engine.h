#pragma once
#include "controllers.h"
#include "dx7note.h"

typedef Dx7Note DxVoice;
namespace Dexed {
	void init();
	void controller_prepare(Controllers * controllers);

	DxVoice* solicitDxVoice();
	void dxVoiceUnassigned(DxVoice* dxVoice);

	extern Dx7Patch globalPatch;
	extern Controllers dummy_controller;
};
