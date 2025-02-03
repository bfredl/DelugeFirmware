/*
 * Copyright © 2017-2023 Synthstrom Audible Limited
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

#pragma once

#include "definitions_cxx.hpp"
#include "model/sync.h"
#include "storage/storage_manager.h"
#include "util/container/array/ordered_resizeable_array.h"
#include "util/container/array/resizeable_array.h"

#include <array>
#include <cstddef>
#include <cstdint>

class PostArpTriggerable;
class ParamManagerForTimeline;

constexpr uint32_t RANDOMIZER_LOCK_MAX_SAVED_VALUES = 16;
constexpr uint32_t ARP_MAX_INSTRUCTION_NOTES = 4;
constexpr uint32_t PATTERN_MAX_BUFFER_SIZE = 16;

constexpr uint32_t ARP_NOTE_NONE = 32767;

enum class ArpType : uint8_t {
	SYNTH,
	DRUM,
};

class ArpeggiatorSettings {
public:
	ArpeggiatorSettings();

	void cloneFrom(ArpeggiatorSettings const* other);

	bool readCommonTagsFromFile(Deserializer& reader, char const* tagName, Song* songToConvertSyncLevel);

	bool readNonAudioTagsFromFile(Deserializer& reader, char const* tagName);

	void writeCommonParamsToFile(Serializer& writer, Song* songToConvertSyncLevel);

	void writeNonAudioParamsToFile(Serializer& writer);

	void generateNewNotePattern();

	void updatePresetFromCurrentSettings();

	void updateSettingsFromCurrentPreset();

	uint32_t getPhaseIncrement(int32_t arpRate);

	// Main settings
	ArpPreset preset{ArpPreset::OFF};
	ArpMode mode{ArpMode::OFF};

	// Sequence settings
	ArpOctaveMode octaveMode{ArpOctaveMode::UP};
	ArpNoteMode noteMode{ArpNoteMode::UP};

	// Octave settings
	uint8_t numOctaves{2};

	// Number of repetitions of each step
	uint8_t numStepRepeats{1};

	// Chord type (only for kit arpeggiators)
	uint8_t chordTypeIndex{0};

	// Sync settings
	SyncLevel syncLevel;
	SyncType syncType;

	// Arp randomizer lock
	bool randomizerLock{false};

	// MPE settings
	ArpMpeModSource mpeVelocity{ArpMpeModSource::OFF};

	// Spread last lock
	uint32_t lastLockedNoteProbabilityParameterValue{0};
	uint32_t lastLockedBassProbabilityParameterValue{0};
	uint32_t lastLockedChordProbabilityParameterValue{0};
	uint32_t lastLockedRatchetProbabilityParameterValue{0};
	uint32_t lastLockedSpreadVelocityParameterValue{0};
	uint32_t lastLockedSpreadGateParameterValue{0};
	uint32_t lastLockedSpreadOctaveParameterValue{0};

	// Pre-calculated randomized values for each parameter
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedNoteProbabilityValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedBassProbabilityValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedChordProbabilityValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedRatchetProbabilityValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedSpreadVelocityValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedSpreadGateValues;
	std::array<int8_t, RANDOMIZER_LOCK_MAX_SAVED_VALUES> lockedSpreadOctaveValues;

	// Order for the pattern note mode
	std::array<int8_t, PATTERN_MAX_BUFFER_SIZE> notePattern;

	// Temporary flags
	bool flagForceArpRestart{false};

	// Automatable params
	int32_t rate{0}; // Default to 25 if not set in XML
	int32_t gate{0}; // Default to 25 if not set in XML
	uint32_t rhythm{0};
	uint32_t sequenceLength{0};
	uint32_t chordPolyphony{0};
	uint32_t ratchetAmount{0};
	uint32_t noteProbability{4294967295u}; // Default to 25 if not set in XML
	uint32_t bassProbability{0};
	uint32_t chordProbability{0};
	uint32_t ratchetProbability{0};
	uint32_t spreadVelocity{0};
	uint32_t spreadGate{0};
	uint32_t spreadOctave{0};
};

struct ArpNote {
	ArpNote() {
		outputMemberChannel.fill(MIDI_CHANNEL_NONE);
		noteCodeOnPostArp.fill(ARP_NOTE_NONE);
	}
	int16_t inputCharacteristics[2]; // Before arpeggiation. And applying to MIDI input if that's happening. Or, channel
	                                 // might be MIDI_CHANNEL_NONE.
	int16_t mpeValues[kNumExpressionDimensions];
	uint8_t velocity;
	uint8_t baseVelocity;

	// For note-ons
	std::array<uint8_t, ARP_MAX_INSTRUCTION_NOTES> outputMemberChannel;
	std::array<int16_t, ARP_MAX_INSTRUCTION_NOTES> noteCodeOnPostArp;
};

struct ArpJustNoteCode {
	int16_t noteCode; // Before arpeggiation
};

class ArpReturnInstruction {
public:
	ArpReturnInstruction() {
		sampleSyncLengthOn = 0;
		arpNoteOn = nullptr;
		outputMIDIChannelOff.fill(MIDI_CHANNEL_NONE);
		noteCodeOffPostArp.fill(ARP_NOTE_NONE);
	}

	// These are only valid if doing a note-on, or when releasing the most recently played with the arp off when other
	// notes are still playing (e.g. for mono note priority)
	uint32_t sampleSyncLengthOn; // This defaults to zero, or may be overwritten by the caller to the Arp - and then
	                             // the Arp itself may override that.
	ArpNote* arpNoteOn;

	// And these are only valid if doing a note-off
	std::array<uint8_t, ARP_MAX_INSTRUCTION_NOTES> outputMIDIChannelOff; // For MPE
	std::array<int16_t, ARP_MAX_INSTRUCTION_NOTES> noteCodeOffPostArp;
};

class ArpeggiatorBase {
public:
	ArpeggiatorBase() {
		noteCodeCurrentlyOnPostArp.fill(ARP_NOTE_NONE);
		outputMIDIChannelForNoteCurrentlyOnPostArp.fill(0);
	}
	virtual void noteOn(ArpeggiatorSettings* settings, int32_t noteCode, int32_t velocity,
	                    ArpReturnInstruction* instruction, int32_t fromMIDIChannel, int16_t const* mpeValues) = 0;
	virtual void noteOff(ArpeggiatorSettings* settings, int32_t noteCodePreArp, ArpReturnInstruction* instruction) = 0;
	void render(ArpeggiatorSettings* settings, ArpReturnInstruction* instruction, int32_t numSamples,
	            uint32_t gateThreshold, uint32_t phaseIncrement);
	int32_t doTickForward(ArpeggiatorSettings* settings, ArpReturnInstruction* instruction, uint32_t ClipCurrentPos,
	                      bool currentlyPlayingReversed);
	void calculateRandomizerAmounts(ArpeggiatorSettings* settings);
	virtual bool hasAnyInputNotesActive() = 0;
	virtual void reset() = 0;
	virtual ArpType getArpType() = 0;

	bool gateCurrentlyActive = false;
	uint32_t gatePos = 0;

	bool playedFirstArpeggiatedNoteYet = false;
	uint8_t lastVelocity = 0;
	std::array<int16_t, ARP_MAX_INSTRUCTION_NOTES> noteCodeCurrentlyOnPostArp;
	std::array<uint8_t, ARP_MAX_INSTRUCTION_NOTES> outputMIDIChannelForNoteCurrentlyOnPostArp;

	// Playing state
	uint32_t notesPlayedFromSequence = 0;
	uint32_t randomNotesPlayedFromOctave = 0;

	// Sequence state
	int16_t whichNoteCurrentlyOnPostArp; // As in, the index within our list
	int8_t currentOctave = 0;
	int8_t currentDirection = 1;
	int8_t currentOctaveDirection = 1;

	// Rhythm state
	uint32_t notesPlayedFromRhythm = 0;
	uint32_t lastNormalNotePlayedFromRhythm = 0;

	// Locked randomizer state
	uint32_t notesPlayedFromLockedRandomizer = 0;

	// Note probability state
	bool lastNormalNotePlayedFromNoteProbability = true;

	// Bass probability state
	bool lastNormalNotePlayedFromBassProbability = true;

	// Chord probability state
	bool lastNormalNotePlayedFromChordProbability = true;

	// Step repeat state
	int32_t stepRepeatIndex = 0;

	// Ratcheting state
	uint32_t ratchetNotesIndex = 0;
	uint32_t ratchetNotesMultiplier = 0;
	uint32_t ratchetNotesCount = 0;
	bool isRatcheting = false;

	// Chord state
	uint32_t chordNotesCount = 0;

	// Calculated spread amounts
	bool isPlayNoteForCurrentStep = true;
	bool isPlayBassForCurrentStep = false;
	bool isPlayChordForCurrentStep = false;
	bool isPlayRatchetForCurrentStep = false;
	int32_t spreadVelocityForCurrentStep = 0;
	int32_t spreadGateForCurrentStep = 0;
	int32_t spreadOctaveForCurrentStep = 0;
	bool resetLockedRandomizerValuesNextTime = false;

protected:
	void calculateNextNoteAndOrOctave(ArpeggiatorSettings* settings, uint8_t numActiveNotes);
	void setInitialNoteAndOctave(ArpeggiatorSettings* settings, uint8_t numActiveNotes);
	void resetBase();
	void resetRatchet();
	void executeArpStep(ArpeggiatorSettings* settings, uint8_t numActiveNotes, bool isRatchet,
	                    uint32_t maxSequenceLength, uint32_t rhythm, bool shouldCarryOnRhythmNote, bool shouldPlayNote,
	                    bool shouldPlayBassNote, bool shouldPlayChordNote);
	void increasePatternIndexes(uint8_t numStepRepeats);
	void increaseSequenceIndexes(uint32_t maxSequenceLength, uint32_t rhythm);
	void maybeSetupNewRatchet(ArpeggiatorSettings* settings);
	bool evaluateRhythm(uint32_t rhythm, bool isRatchet);
	bool evaluateNoteProbability(bool isRatchet);
	bool evaluateBassProbability(bool isRatchet);
	bool evaluateChordProbability(bool isRatchet);
	uint32_t calculateSpreadVelocity(uint8_t velocity, int32_t spreadVelocityForCurrentStep);
	int32_t getOctaveDirection(ArpeggiatorSettings* settings);
	virtual void switchNoteOn(ArpeggiatorSettings* settings, ArpReturnInstruction* instruction, bool isRatchet) = 0;
	void switchAnyNoteOff(ArpReturnInstruction* instruction);
	bool getRandomProbabilityResult(uint32_t value);
	int8_t getRandomBipolarProbabilityAmount(uint32_t value);
	int8_t getRandomWeighted2BitsAmount(uint32_t value);
};

class ArpeggiatorForDrum final : public ArpeggiatorBase {
public:
	ArpeggiatorForDrum();
	void noteOn(ArpeggiatorSettings* settings, int32_t noteCode, int32_t velocity, ArpReturnInstruction* instruction,
	            int32_t fromMIDIChannel, int16_t const* mpeValues) override;
	void noteOff(ArpeggiatorSettings* settings, int32_t noteCodePreArp, ArpReturnInstruction* instruction) override;
	void reset() override;
	ArpType getArpType() override { return ArpType::DRUM; }
	ArpNote arpNote; // For the one note. noteCode will always be 60. velocity will be 0 if off.
	int16_t noteForDrum;

protected:
	void switchNoteOn(ArpeggiatorSettings* settings, ArpReturnInstruction* instruction, bool isRatchet) override;
	bool hasAnyInputNotesActive() override;
};

class Arpeggiator final : public ArpeggiatorBase {
public:
	Arpeggiator();

	void reset() override;
	ArpType getArpType() override { return ArpType::SYNTH; }

	void noteOn(ArpeggiatorSettings* settings, int32_t noteCode, int32_t velocity, ArpReturnInstruction* instruction,
	            int32_t fromMIDIChannel, int16_t const* mpeValues) override;
	void noteOff(ArpeggiatorSettings* settings, int32_t noteCodePreArp, ArpReturnInstruction* instruction) override;
	bool hasAnyInputNotesActive() override;

	// This array tracks the notes ordered by noteCode
	OrderedResizeableArray notes;
	// This array tracks the notes as they were played by the user
	ResizeableArray notesAsPlayed;
	// This array tracks the notes as ordered by the Pattern note mode
	ResizeableArray notesByPattern;

protected:
	void rearrangePatterntArpNotes(ArpeggiatorSettings* settings);
	void switchNoteOn(ArpeggiatorSettings* settings, ArpReturnInstruction* instruction, bool isRatchet) override;
};
