/*
 * Copyright © 2018-2023 Synthstrom Audible Limited
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

#include "model/drum/midi_drum.h"
#include "gui/views/automation_view.h"
#include "gui/views/instrument_clip_view.h"
#include "io/midi/midi_engine.h"
#include "model/drum/non_audio_drum.h"
#include "storage/storage_manager.h"
#include <string.h>

MIDIDrum::MIDIDrum() : NonAudioDrum(DrumType::MIDI) {
	channel = 0;
	note = 0;
}

void MIDIDrum::noteOn(ModelStackWithThreeMainThings* modelStack, uint8_t velocity, Kit* kit, int16_t const* mpeValues,
                      int32_t fromMIDIChannel, uint32_t sampleSyncLength, int32_t ticksLate, uint32_t samplesLate) {
	ArpeggiatorSettings* arpSettings = getArpSettings();
	ArpReturnInstruction instruction;
	// Run everything by the Arp...
	arpeggiator.noteOn(arpSettings, note, velocity, &instruction, fromMIDIChannel, mpeValues);
	if (instruction.arpNoteOn != nullptr) {
		for (int32_t n = 0; n < ARP_MAX_INSTRUCTION_NOTES; n++) {
			if (instruction.arpNoteOn->noteCodeOnPostArp[n] == ARP_NOTE_NONE) {
				break;
			}
			noteOnPostArp(instruction.arpNoteOn->noteCodeOnPostArp[n], instruction.arpNoteOn, n);
		}
	}
}

void MIDIDrum::noteOff(ModelStackWithThreeMainThings* modelStack, int32_t velocity) {
	ArpeggiatorSettings* arpSettings = getArpSettings();
	ArpReturnInstruction instruction;
	// Run everything by the Arp...
	arpeggiator.noteOff(arpSettings, note, &instruction);
	for (int32_t n = 0; n < ARP_MAX_INSTRUCTION_NOTES; n++) {
		if (instruction.noteCodeOffPostArp[n] == ARP_NOTE_NONE) {
			break;
		}
		noteOffPostArp(instruction.noteCodeOffPostArp[n]);
	}
}

void MIDIDrum::noteOnPostArp(int32_t noteCodePostArp, ArpNote* arpNote, int32_t noteIndex) {
	lastVelocity = arpNote->velocity;
	midiEngine.sendNote(this, true, noteCodePostArp, arpNote->velocity, channel, kMIDIOutputFilterNoMPE);
	state = true;
}

void MIDIDrum::noteOffPostArp(int32_t noteCodePostArp) {
	midiEngine.sendNote(this, false, noteCodePostArp, kDefaultNoteOffVelocity, channel, kMIDIOutputFilterNoMPE);
	state = false;
}

void MIDIDrum::unassignAllVoices() {
	if (hasAnyVoices()) {
		noteOff(nullptr);
	}
}

void MIDIDrum::writeToFile(Serializer& writer, bool savingSong, ParamManager* paramManager) {
	writer.writeOpeningTagBeginning("midiOutput", true);

	writer.writeAttribute("channel", channel, false);
	writer.writeAttribute("note", note, false);
	writer.writeOpeningTagEnd();

	NonAudioDrum::writeArpeggiatorToFile(writer);

	if (savingSong) {
		Drum::writeMIDICommandsToFile(writer);
	}
	writer.writeClosingTag("midiOutput", true, true);
}

Error MIDIDrum::readFromFile(Deserializer& reader, Song* song, Clip* clip, int32_t readAutomationUpToPos) {
	char const* tagName;

	while (*(tagName = reader.readNextTagOrAttributeName())) {
		if (!strcmp(tagName, "note")) {
			note = reader.readTagOrAttributeValueInt();
			reader.exitTag("note");
		}
		else if (NonAudioDrum::readDrumTagFromFile(reader, tagName)) {}
		else {
			reader.exitTag(tagName);
		}
	}

	return Error::NONE;
}

void MIDIDrum::getName(char* buffer) {

	int32_t channelToDisplay = channel + 1;

	if (channelToDisplay < 10 && note < 100) {
		strcpy(buffer, " ");
	}
	else {
		buffer[0] = 0;
	}

	// If can't fit everything on display, show channel as hexadecimal
	if (channelToDisplay >= 10 && note >= 100) {
		buffer[0] = 'A' + channelToDisplay - 10;
		buffer[1] = 0;
	}
	else {
		intToString(channelToDisplay, &buffer[strlen(buffer)]);
	}

	strcat(buffer, ".");

	if (note < 10 && channelToDisplay < 100) {
		strcat(buffer, " ");
	}

	intToString(note, &buffer[strlen(buffer)]);
}

int8_t MIDIDrum::modEncoderAction(ModelStackWithThreeMainThings* modelStack, int8_t offset, uint8_t whichModEncoder) {

	NonAudioDrum::modEncoderAction(modelStack, offset, whichModEncoder);

	if ((getCurrentUI() == &instrumentClipView
	     || (getCurrentUI() == &automationView
	         && automationView.getAutomationSubType() == AutomationSubType::INSTRUMENT))
	    && currentUIMode == UI_MODE_AUDITIONING) {
		if (whichModEncoder == 1) {
			modChange(modelStack, offset, &noteEncoderCurrentOffset, &note, 128);
		}
	}

	return -64;
}

void MIDIDrum::expressionEvent(int32_t newValue, int32_t expressionDimension) {

	// Aftertouch only
	if (expressionDimension == 2) {
		int32_t value7 = newValue >> 24;
		// Note: use the note code currently on post-arp, because this drum supports "Chord Simulator" and "Octaves" and
		// the note code could be different
		midiEngine.sendPolyphonicAftertouch(this, channel, value7, arpeggiator.arpNote.noteCodeOnPostArp[0],
		                                    kMIDIOutputFilterNoMPE);
	}
}

void MIDIDrum::polyphonicExpressionEventOnChannelOrNote(int32_t newValue, int32_t expressionDimension,
                                                        int32_t channelOrNoteNumber,
                                                        MIDICharacteristic whichCharacteristic) {
	// Because this is a Drum, we disregard the noteCode (which is what channelOrNoteNumber always is in our case - but
	// yeah, that's all irrelevant.
	expressionEvent(newValue, expressionDimension);
}
