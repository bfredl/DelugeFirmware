/*
 * Copyright © 2014-2023 Synthstrom Audible Limited
 *
 * This file is part of The Synthstrom Audible Deluge Firmware.
 *
 * The Synthstrom Audible Deluge Firmware is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with this program.
 * If not, see <https://www.gnu.org/licenses/>.
 */

#include "processing/sound/sound_instrument.h"
#include "definitions_cxx.hpp"
#include "dsp/dx/dx7note.h"
#include "dsp/stereo_sample.h"
#include "gui/views/view.h"
#include "model/clip/instrument_clip.h"
#include "model/model_stack.h"
#include "model/note/note_row.h"
#include "model/song/song.h"
#include "model/voice/voice.h"
#include "model/voice/voice_vector.h"
#include "modulation/arpeggiator.h"
#include "modulation/params/param_manager.h"
#include "modulation/params/param_set.h"
#include "modulation/patch/patch_cable_set.h"
#include "playback/playback_handler.h"
#include "processing/engines/audio_engine.h"
#include "storage/audio/audio_file_manager.h"
#include "storage/storage_manager.h"
#include "util/misc.h"

namespace params = deluge::modulation::params;

SoundInstrument::SoundInstrument() : MelodicInstrument(OutputType::SYNTH) {
}

bool SoundInstrument::writeDataToFile(Serializer& writer, Clip* clipForSavingOutputOnly, Song* song) {

	// MelodicInstrument::writeDataToFile(writer, clipForSavingOutputOnly, song); // Nope, this gets called within the
	// below call
	writeMelodicInstrumentAttributesToFile(writer, clipForSavingOutputOnly, song);

	ParamManager* paramManager;

	// If saving Output only...
	if (clipForSavingOutputOnly) {
		paramManager = &clipForSavingOutputOnly->paramManager;

		// Or if saving Song...
	}
	else {

		// If no activeClip, that means no Clip has this Output, so there should be a backedUpParamManager that we
		// should use
		if (!activeClip) {
			paramManager = song->getBackedUpParamManagerPreferablyWithClip(this, NULL);
		}
		else {
			paramManager = nullptr;
		}
	}

	Sound::writeToFile(writer, clipForSavingOutputOnly == nullptr, paramManager,
	                   clipForSavingOutputOnly ? &((InstrumentClip*)clipForSavingOutputOnly)->arpSettings : nullptr,
	                   NULL);

	MelodicInstrument::writeMelodicInstrumentTagsToFile(writer, clipForSavingOutputOnly, song);

	return true;
}

// arpSettings optional - no need if you're loading a new V2.0 song where Instruments are all separate from Clips and
// won't store any arp stuff
Error SoundInstrument::readFromFile(Deserializer& reader, Song* song, Clip* clip, int32_t readAutomationUpToPos) {

	char modelStackMemory[MODEL_STACK_MAX_SIZE];
	ModelStackWithModControllable* modelStack =
	    setupModelStackWithSong(modelStackMemory, song)->addTimelineCounter(clip)->addModControllableButNoNoteRow(this);

	return Sound::readFromFile(reader, modelStack, readAutomationUpToPos, &defaultArpSettings);
}

void SoundInstrument::cutAllSound() {
	Sound::unassignAllVoices();
}

void SoundInstrument::renderOutput(ModelStack* modelStack, std::span<StereoSample> output, int32_t* reverbBuffer,
                                   int32_t reverbAmountAdjust, int32_t sideChainHitPending,
                                   bool shouldLimitDelayFeedback, bool isClipActive) {
	// this should only happen in the rare case that this is called while replacing an instrument but after the clips
	// have been cleared
	if (!activeClip) [[unlikely]] {
		return;
	}
	ModelStackWithThreeMainThings* modelStackWithThreeMainThings =
	    modelStack->addTimelineCounter(activeClip)
	        ->addOtherTwoThingsButNoNoteRow(this, getParamManager(modelStack->song));

	if (skippingRendering) {
		compressor.reset();
		compressor.gainReduction = 0;
	}
	else {
		Sound::render(modelStackWithThreeMainThings, output, reverbBuffer, sideChainHitPending, reverbAmountAdjust,
		              shouldLimitDelayFeedback, kMaxSampleValue, recorder);
	}

	if (playbackHandler.isEitherClockActive() && !playbackHandler.ticksLeftInCountIn && isClipActive) {

		// No time to call the proper function and do error checking, sorry.
		ParamCollectionSummary* patchedParamsSummary = &modelStackWithThreeMainThings->paramManager->summaries[1];
		bool anyInterpolating = false;
		if constexpr (params::kNumParams > 64) {
			anyInterpolating = patchedParamsSummary->whichParamsAreInterpolating[0]
			                   || patchedParamsSummary->whichParamsAreInterpolating[1]
			                   || patchedParamsSummary->whichParamsAreInterpolating[2];
		}
		else {
			anyInterpolating = patchedParamsSummary->whichParamsAreInterpolating[0]
			                   || patchedParamsSummary->whichParamsAreInterpolating[1];
		}
		if (anyInterpolating) {
yesTickParamManagerForClip:
			modelStackWithThreeMainThings->paramManager->toForTimeline()->tickSamples(output.size(),
			                                                                          modelStackWithThreeMainThings);
		}
		else {

			// Try other options too.

			// No time to call the proper function and do error checking, sorry.
			ParamCollectionSummary* unpatchedParamsSummary = &modelStackWithThreeMainThings->paramManager->summaries[0];
			if constexpr (params::UNPATCHED_SOUND_MAX_NUM > 32) {
				if (unpatchedParamsSummary->whichParamsAreInterpolating[0]
				    || unpatchedParamsSummary->whichParamsAreInterpolating[1]) {
					goto yesTickParamManagerForClip;
				}
			}
			else {
				if (unpatchedParamsSummary->whichParamsAreInterpolating[0]) {
					goto yesTickParamManagerForClip;
				}
			}

			// No time to call the proper function and do error checking, sorry.
			ParamCollectionSummary* patchCablesSummary = &modelStackWithThreeMainThings->paramManager->summaries[2];
			if constexpr (kMaxNumPatchCables > 32) {
				if (patchCablesSummary->whichParamsAreInterpolating[0]
				    || patchCablesSummary->whichParamsAreInterpolating[1]) {
					goto yesTickParamManagerForClip;
				}
			}
			else {
				if (patchCablesSummary->whichParamsAreInterpolating[0]) {
					goto yesTickParamManagerForClip;
				}
			}

			// No time to call the proper function and do error checking, sorry.
			ParamCollectionSummary* expressionParamsSummary =
			    &modelStackWithThreeMainThings->paramManager->summaries[3];
			if constexpr (kNumExpressionDimensions > 32) {
				if (expressionParamsSummary->whichParamsAreInterpolating[0]
				    || expressionParamsSummary->whichParamsAreInterpolating[1]) {
					goto yesTickParamManagerForClip;
				}
			}
			else {
				if (expressionParamsSummary->whichParamsAreInterpolating[0]) {
					goto yesTickParamManagerForClip;
				}
			}
		}

		// Do the ParamManagers of each NoteRow, too
		for (int32_t i = 0; i < ((InstrumentClip*)activeClip)->noteRows.getNumElements(); i++) {
			NoteRow* thisNoteRow = ((InstrumentClip*)activeClip)->noteRows.getElement(i);
			// No time to call the proper function and do error checking, sorry.
			ParamCollectionSummary* expressionParamsSummary = &thisNoteRow->paramManager.summaries[0];
			bool result = false;
			if constexpr (kNumExpressionDimensions > 32) {
				result = expressionParamsSummary->whichParamsAreInterpolating[0]
				         || expressionParamsSummary->whichParamsAreInterpolating[1];
			}
			else {
				result = expressionParamsSummary->whichParamsAreInterpolating[0];
			}
			if (result) {
				modelStackWithThreeMainThings->setNoteRow(thisNoteRow, thisNoteRow->y);
				modelStackWithThreeMainThings->paramManager = &thisNoteRow->paramManager;
				thisNoteRow->paramManager.tickSamples(output.size(), modelStackWithThreeMainThings);
			}
		}
	}
}

Error SoundInstrument::loadAllAudioFiles(bool mayActuallyReadFiles) {

	bool doingAlternatePath =
	    mayActuallyReadFiles && (audioFileManager.alternateLoadDirStatus == AlternateLoadDirStatus::NONE_SET);
	if (doingAlternatePath) {
		Error error = setupDefaultAudioFileDir();
		if (error != Error::NONE) {
			return error;
		}
	}

	Error error = Sound::loadAllAudioFiles(mayActuallyReadFiles);

	if (doingAlternatePath) {
		audioFileManager.thingFinishedLoading();
	}

	return error;
}

void SoundInstrument::resyncLFOs() {
	resyncGlobalLFO();
}

ModControllable* SoundInstrument::toModControllable() {
	return this;
}

void SoundInstrument::setupPatching(ModelStackWithTimelineCounter* modelStack) {

	InstrumentClip* clip = (InstrumentClip*)modelStack->getTimelineCounterAllowNull();
	ParamManagerForTimeline* paramManager;
	if (clip) {
		paramManager = &clip->paramManager;

		ModelStackWithThreeMainThings* modelStackWithThreeMainThings =
		    modelStack->addOtherTwoThingsButNoNoteRow(this, paramManager);

		ensureInaccessibleParamPresetValuesWithoutKnobsAreZero(modelStackWithThreeMainThings);
	}
	else {
		paramManager =
		    (ParamManagerForTimeline*)modelStack->song->getBackedUpParamManagerPreferablyWithClip(this, NULL);
		ensureInaccessibleParamPresetValuesWithoutKnobsAreZeroWithMinimalDetails(paramManager);
	}

	ModelStackWithParamCollection* modelStackWithParamCollection =
	    paramManager->getPatchCableSet(modelStack->addOtherTwoThingsButNoNoteRow(this, paramManager));

	PatchCableSet* patchCableSet = (PatchCableSet*)modelStackWithParamCollection->paramCollection;

	patchCableSet->setupPatching(modelStackWithParamCollection);
}

bool SoundInstrument::setActiveClip(ModelStackWithTimelineCounter* modelStack, PgmChangeSend maySendMIDIPGMs) {

	bool clipChanged = MelodicInstrument::setActiveClip(modelStack, maySendMIDIPGMs);

	if (clipChanged) {
		AudioEngine::mustUpdateReverbParamsBeforeNextRender = true;

		if (modelStack) {
			ParamManager* paramManager = &modelStack->getTimelineCounter()->paramManager;
			patcher.performInitialPatching(this, paramManager);

			// Grab mono expression params
			ExpressionParamSet* expressionParams = paramManager->getExpressionParamSet();
			if (expressionParams) {
				for (int32_t i = 0; i < kNumExpressionDimensions; i++) {
					monophonicExpressionValues[i] = expressionParams->params[i].getCurrentValue();
				}
			}
			else {
				for (int32_t i = 0; i < kNumExpressionDimensions; i++) {
					monophonicExpressionValues[i] = 0;
				}
			}
			expressionSourcesChangedAtSynthLevel.set();
		}
	}
	return clipChanged;
}

void SoundInstrument::setupWithoutActiveClip(ModelStack* modelStack) {

	ModelStackWithTimelineCounter* modelStackWithTimelineCounter = modelStack->addTimelineCounter(nullptr);

	setupPatching(modelStackWithTimelineCounter);

	ParamManager* paramManager =
	    modelStackWithTimelineCounter->song->getBackedUpParamManagerPreferablyWithClip(this, NULL);
	if (!paramManager) {
		FREEZE_WITH_ERROR("E173");
	}
	patcher.performInitialPatching(this, paramManager);

	// Clear mono expression params
	for (int32_t i = 0; i < kNumExpressionDimensions; i++) {
		monophonicExpressionValues[i] = 0;
	}
	expressionSourcesChangedAtSynthLevel.set();

	Instrument::setupWithoutActiveClip(modelStack);
}

void SoundInstrument::prepareForHibernationOrDeletion() {
	Sound::prepareForHibernation();
}

void SoundInstrument::setupPatchingForAllParamManagers(Song* song) {
	song->setupPatchingForAllParamManagersForInstrument(this);
}

void SoundInstrument::deleteBackedUpParamManagers(Song* song) {
	song->deleteBackedUpParamManagersForModControllable(this);
}

extern bool expressionValueChangesMustBeDoneSmoothly;

void SoundInstrument::monophonicExpressionEvent(int32_t newValue, int32_t expressionDimension) {
	expressionSourcesChangedAtSynthLevel[expressionDimension] = true;
	monophonicExpressionValues[expressionDimension] = newValue;
}

// Alternative to what's in the NonAudioInstrument:: implementation, which would almost work here, but we cut corner for
// Sound by avoiding going through the Arp and just talk directly to the Voices. (Despite my having made it now actually
// need to talk to the Arp too, as below...) Note, this virtual function actually overrides/implements from two base
// classes - MelodicInstrument and ModControllable.
void SoundInstrument::polyphonicExpressionEventOnChannelOrNote(int32_t newValue, int32_t expressionDimension,
                                                               int32_t channelOrNoteNumber,
                                                               MIDICharacteristic whichCharacteristic) {
	int32_t s = expressionDimension + util::to_underlying(PatchSource::X);

	// sourcesChanged |= 1 << s; // We'd ideally not want to apply this to all voices though...

	int32_t ends[2];
	AudioEngine::activeVoices.getRangeForSound(this, ends);
	for (int32_t v = ends[0]; v < ends[1]; v++) {
		Voice* thisVoice = AudioEngine::activeVoices.getVoice(v);
		if (thisVoice->inputCharacteristics[util::to_underlying(whichCharacteristic)] == channelOrNoteNumber) {
			if (expressionValueChangesMustBeDoneSmoothly) {
				thisVoice->expressionEventSmooth(newValue, s);
			}
			else {
				thisVoice->expressionEventImmediate(*this, newValue, s);
			}
		}
	}

	// Must update MPE values in Arp too - useful either if it's on, or if we're in true monophonic mode - in either
	// case, we could need to suddenly do a note-on for a different note that the Arp knows about, and need these MPE
	// values.
	int32_t n, nEnd;
	if (whichCharacteristic == MIDICharacteristic::NOTE) {
		n = arpeggiator.notes.search(channelOrNoteNumber, GREATER_OR_EQUAL);
		if (n < arpeggiator.notes.getNumElements()) {
			nEnd = 0;
			goto lookAtArpNote;
		}
		return;
	}
	nEnd = arpeggiator.notes.getNumElements();
	for (n = 0; n < nEnd; n++) {
lookAtArpNote:
		ArpNote* arpNote = (ArpNote*)arpeggiator.notes.getElementAddress(n);
		if (arpNote->inputCharacteristics[util::to_underlying(whichCharacteristic)] == channelOrNoteNumber) {
			arpNote->mpeValues[expressionDimension] = newValue >> 16;
		}
	}

	// Let the Sound know about this polyphonic expression event
	// The Sound class will use it to send MIDI out (if enabled in the sound config)
	Sound::polyphonicExpressionEventOnChannelOrNote(newValue, expressionDimension, channelOrNoteNumber,
	                                                whichCharacteristic);
}

void SoundInstrument::sendNote(ModelStackWithThreeMainThings* modelStack, bool isOn, int32_t noteCode,
                               int16_t const* mpeValues, int32_t fromMIDIChannel, uint8_t velocity,
                               uint32_t sampleSyncLength, int32_t ticksLate, uint32_t samplesLate) {

	if (!inValidState) {
		return;
	}

	if (isOn) {
		noteOn(modelStack, &arpeggiator, noteCode, mpeValues, sampleSyncLength, ticksLate, samplesLate, velocity,
		       fromMIDIChannel);
	}
	else {
		noteOff(modelStack, &arpeggiator, noteCode);
	}
}

ArpeggiatorSettings* SoundInstrument::getArpSettings(InstrumentClip* clip) {
	return MelodicInstrument::getArpSettings(clip);
}

bool SoundInstrument::readTagFromFile(Deserializer& reader, char const* tagName) {
	return MelodicInstrument::readTagFromFile(reader, tagName);
}

void SoundInstrument::compensateInstrumentVolumeForResonance(ModelStackWithThreeMainThings* modelStack) {
	Sound::compensateVolumeForResonance(modelStack);
}

void SoundInstrument::loadCrucialAudioFilesOnly() {
	loadAllAudioFiles(true);
}

// Any time it gets edited, we want to grab the default arp settings from the activeClip
void SoundInstrument::beenEdited(bool shouldMoveToEmptySlot) {
	if (activeClip) {
		defaultArpSettings.cloneFrom(&((InstrumentClip*)activeClip)->arpSettings);
	}
	Instrument::beenEdited(shouldMoveToEmptySlot);
}

// Returns num ticks til next arp event
int32_t SoundInstrument::doTickForwardForArp(ModelStack* modelStack, int32_t currentPos) {
	if (!activeClip) {
		return 2147483647;
	}

	ModelStackWithThreeMainThings* modelStackWithThreeMainThings =
	    modelStack->addTimelineCounter(activeClip)
	        ->addOtherTwoThingsButNoNoteRow(this, getParamManager(modelStack->song));

	UnpatchedParamSet* unpatchedParams = modelStackWithThreeMainThings->paramManager->getUnpatchedParamSet();

	ArpeggiatorSettings* arpSettings = getArpSettings();
	arpSettings->rhythm = (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_RHYTHM) + 2147483648;
	arpSettings->sequenceLength =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_SEQUENCE_LENGTH) + 2147483648;
	arpSettings->chordPolyphony =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_CHORD_POLYPHONY) + 2147483648;
	arpSettings->ratchetAmount = (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_RATCHET_AMOUNT) + 2147483648;
	arpSettings->noteProbability =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_NOTE_PROBABILITY) + 2147483648;
	arpSettings->bassProbability =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_BASS_PROBABILITY) + 2147483648;
	arpSettings->chordProbability =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_CHORD_PROBABILITY) + 2147483648;
	arpSettings->ratchetProbability =
	    (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_RATCHET_PROBABILITY) + 2147483648;
	arpSettings->spreadVelocity = (uint32_t)unpatchedParams->getValue(params::UNPATCHED_SPREAD_VELOCITY) + 2147483648;
	arpSettings->spreadGate = (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_SPREAD_GATE) + 2147483648;
	arpSettings->spreadOctave = (uint32_t)unpatchedParams->getValue(params::UNPATCHED_ARP_SPREAD_OCTAVE) + 2147483648;

	ArpReturnInstruction instruction;

	int32_t ticksTilNextArpEvent =
	    arpeggiator.doTickForward(arpSettings, &instruction, currentPos, activeClip->currentlyPlayingReversed);

	ModelStackWithSoundFlags* modelStackWithSoundFlags = modelStackWithThreeMainThings->addSoundFlags();

	for (int32_t n = 0; n < ARP_MAX_INSTRUCTION_NOTES; n++) {
		if (instruction.noteCodeOffPostArp[n] == ARP_NOTE_NONE) {
			break;
		}
		noteOffPostArpeggiator(modelStackWithSoundFlags, instruction.noteCodeOffPostArp[n]);
	}
	if (instruction.arpNoteOn != nullptr) {
		for (int32_t n = 0; n < ARP_MAX_INSTRUCTION_NOTES; n++) {
			if (instruction.arpNoteOn->noteCodeOnPostArp[n] == ARP_NOTE_NONE) {
				break;
			}
			noteOnPostArpeggiator(
			    modelStackWithSoundFlags,
			    instruction.arpNoteOn->inputCharacteristics[util::to_underlying(MIDICharacteristic::NOTE)],
			    instruction.arpNoteOn->noteCodeOnPostArp[n], instruction.arpNoteOn->velocity,
			    instruction.arpNoteOn->mpeValues, instruction.sampleSyncLengthOn, 0, 0,
			    instruction.arpNoteOn->inputCharacteristics[util::to_underlying(MIDICharacteristic::CHANNEL)]);
		}
	}

	return ticksTilNextArpEvent;
}

void SoundInstrument::getThingWithMostReverb(Sound** soundWithMostReverb, ParamManager** paramManagerWithMostReverb,
                                             GlobalEffectableForClip** globalEffectableWithMostReverb,
                                             int32_t* highestReverbAmountFound) {
	if (activeClip) {
		Sound::getThingWithMostReverb(soundWithMostReverb, paramManagerWithMostReverb, globalEffectableWithMostReverb,
		                              highestReverbAmountFound, &activeClip->paramManager);
	}
}

ArpeggiatorBase* SoundInstrument::getArp() {
	return &arpeggiator;
}

bool SoundInstrument::noteIsOn(int32_t noteCode, bool resetTimeEntered) {

	ArpeggiatorSettings* arpSettings = getArpSettings();

	if (arpSettings) {
		if (arpSettings->mode != ArpMode::OFF || polyphonic == PolyphonyMode::LEGATO
		    || polyphonic == PolyphonyMode::MONO) {

			int32_t n = arpeggiator.notes.search(noteCode, GREATER_OR_EQUAL);
			if (n >= arpeggiator.notes.getNumElements()) {
				return false;
			}
			ArpNote* arpNote = (ArpNote*)arpeggiator.notes.getElementAddress(n);
			return (arpNote->inputCharacteristics[util::to_underlying(MIDICharacteristic::NOTE)] == noteCode);
		}
	}

	if (!numVoicesAssigned) {
		return false;
	}

	int32_t ends[2];
	AudioEngine::activeVoices.getRangeForSound(this, ends);
	for (int32_t v = ends[0]; v < ends[1]; v++) {
		Voice* thisVoice = AudioEngine::activeVoices.getVoice(v);
		if ((thisVoice->noteCodeAfterArpeggiation == noteCode)
		    && thisVoice->envelopes[0].state < EnvelopeStage::RELEASE) { // Ignore releasing notes. Is this right?
			if (resetTimeEntered) {
				thisVoice->envelopes[0].resetTimeEntered();
			}
			return true;
		}
	}
	return false;
}

void SoundInstrument::offerReceivedYamahaSysex(MIDICable& fromDevice, int32_t channel, uint8_t* data, int32_t len) {
	MIDIMatchType match = midiInput.checkMatch(&fromDevice, channel);

	if (match != MIDIMatchType::NO_MATCH) {
		receivedYamahaSysex(fromDevice, match, channel, data, len);
	}
}

void SoundInstrument::receivedYamahaSysex(MIDICable& fromDevice, MIDIMatchType match, int32_t channel, uint8_t* data,
                                          int32_t len) {
	if (sources[0].oscType != OscType::DX7 || sources[0].dxPatch == nullptr) {
		return;
	}

	int32_t substatus = data[2] >> 4;

	if (substatus == 1) {
		if (data[3] >> 2 == 0) { // g=0: voice parameter
			int32_t param = (data[3] << 7) + data[4];
			uint8_t value = data[5];

			if (param > 155) {
				return;
			}

			sources[0].dxPatch->params[param] = value;
			sources[0].dxPatchChanged = true;
		}
	}
	else if (substatus == 0) {
		if (data[3] == 0 && len >= 156) { // single voice dump
			memcpy(sources[0].dxPatch->params, data + 6, 155);
			sources[0].dxPatchChanged = true;
		}
	}
}
