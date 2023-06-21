#pragma once

// #define USE_DEXED_MK1

#include "EngineMkI.h"
#include "dx7note.h"

typedef Dx7Note DxVoice;
namespace Dexed {
#ifdef USE_DEXED_MK1
extern EngineMkI engineMkI;
#endif
extern FmCore engineModern;
extern int32_t lfo_unit;

DxVoice* solicitDxVoice();
void dxVoiceUnassigned(DxVoice* dxVoice);

Dx7Patch* newPatch();
}; // namespace Dexed
