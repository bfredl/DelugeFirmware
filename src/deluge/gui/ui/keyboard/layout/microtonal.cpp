/*
 * Copyright © 2016-2024 Synthstrom Audible Limited
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

#include "gui/ui/keyboard/layout/microtonal.h"
#include "gui/ui/audio_recorder.h"
#include "gui/ui/browser/sample_browser.h"
#include "gui/ui/sound_editor.h"
#include "hid/display/display.h"
#include "model/scale/note_set.h"
#include "model/settings/runtime_feature_settings.h"
#include "util/functions.h"

namespace deluge::gui::ui::keyboard::layout {

void KeyboardLayoutMicrotonal::evaluatePads(PressedPad presses[kMaxNumKeyboardPadPresses]) {
	currentNotesState = NotesState{}; // Erase active notes

	for (int32_t idxPress = 0; idxPress < kMaxNumKeyboardPadPresses; ++idxPress) {
		auto pressed = presses[idxPress];
		if (pressed.active && pressed.x < kDisplayWidth) {
			enableNote(noteFromCoords(pressed.x, pressed.y), velocity);
		}
	}

	// Should be called last so currentNotesState can be read
	ColumnControlsKeyboard::evaluatePads(presses);
}

void KeyboardLayoutMicrotonal::handleVerticalEncoder(int32_t offset, bool shiftEnabled) {
	if (verticalEncoderHandledByColumns(offset)) {
		return;
	}
	if (shiftEnabled) {
		KeyboardStateMicrotonal& state = getState().microtonal;
		state.colInterval += offset;
		state.colInterval = std::clamp(state.colInterval, kMinIsomorphicRowInterval, kMaxIsomorphicRowInterval);

		char buffer[13] = "Col step:   ";
		auto displayOffset = (display->haveOLED() ? 10 : 0);
		intToString(state.colInterval, buffer + displayOffset, 1);
		display->displayPopup(buffer);

		offset = 0; // Reset offset variable for processing scroll calculation without actually shifting
	}

	offsetPads(offset * getState().microtonal.rowInterval, false);
}

void KeyboardLayoutMicrotonal::handleHorizontalEncoder(int32_t offset, bool shiftEnabled) {
	if (horizontalEncoderHandledByColumns(offset, shiftEnabled)) {
		return;
	}
	offsetPads(offset, shiftEnabled);
}

void KeyboardLayoutMicrotonal::offsetPads(int32_t offset, bool shiftEnabled) {
	KeyboardStateMicrotonal& state = getState().microtonal;

	if (shiftEnabled) {
		state.rowInterval += offset;
		state.rowInterval = std::clamp(state.rowInterval, kMinIsomorphicRowInterval, kMaxIsomorphicRowInterval);

		char buffer[13] = "Row step:   ";
		auto displayOffset = (display->haveOLED() ? 10 : 0);
		intToString(state.rowInterval, buffer + displayOffset, 1);
		display->displayPopup(buffer);

		offset = 0; // Reset offset variable for processing scroll calculation without actually shifting
	}

	// Calculate highest possible displayable note with current rowInterval
	int32_t highestScrolledNote =
	    (getHighestClipNote() - ((kDisplayHeight - 1) * state.rowInterval + kDisplayWidth - 1));

	// Make sure current value is in bounds
	state.scrollOffset = std::clamp(state.scrollOffset, getLowestClipNote(), highestScrolledNote);

	// Offset if still in bounds (reject if the next row can not be shown completely)
	int32_t newOffset = state.scrollOffset + offset;
	if (newOffset >= getLowestClipNote() && newOffset <= highestScrolledNote) {
		state.scrollOffset = newOffset;
	}

	precalculate();
}

void KeyboardLayoutMicrotonal::precalculate() {
	KeyboardStateMicrotonal& state = getState().microtonal;

	// Pre-Buffer colours for next renderings
	for (int32_t i = 0; i < (kDisplayHeight * state.rowInterval + kDisplayWidth); ++i) {
		noteColours[i] = getNoteColour(state.scrollOffset + i);
	}
}

void KeyboardLayoutMicrotonal::renderPads(RGB image[][kDisplayWidth + kSideBarWidth]) {
	const int kMaxOctaveSize = 50;
	int octave_size = 31;
	int8_t octaveScaleNotes[kMaxOctaveSize] = {0};
	int8_t baseNotes[] = {0, 5, 10, 13, 18, 23, 28};
	if (getScaleModeEnabled()) {
		for (auto note : baseNotes) {
			octaveScaleNotes[note] = 1;
			octaveScaleNotes[note+2] = 2;
			octaveScaleNotes[(note+29)%31] = 3;
		}
	}

	RGB scaleColours[4] { colours::black, colours::green, colours::blue, colours::red};

	// Precreate list of all active notes per octave
	bool octaveActiveNotes[kMaxOctaveSize] = {0};
	for (uint8_t idx = 0; idx < currentNotesState.count; ++idx) {
		octaveActiveNotes[((currentNotesState.notes[idx].note + octave_size) - getRootNote()) % octave_size] = true;
	}


	// Iterate over grid image
	for (int32_t y = 0; y < kDisplayHeight; ++y) {
		int32_t noteCode = noteFromCoords(0, y);
		int32_t normalizedPadOffset = noteCode - getState().microtonal.scrollOffset;
		int32_t noteWithinOctave = (uint16_t)((noteCode + octave_size) - getRootNote()) % octave_size;
		int32_t colInterval = getState().microtonal.colInterval;

		for (int32_t x = 0; x < kDisplayWidth; x++) {

			RGB base = scaleColours[octaveScaleNotes[noteWithinOctave]];
			// Full colour for every octaves root and active notes
			if (octaveActiveNotes[noteWithinOctave] || noteWithinOctave == 0) {
				image[y][x] = base;
			}
			// If highlighting notes is active, do it
			else if (runtimeFeatureSettings.get(RuntimeFeatureSettingType::HighlightIncomingNotes)
			             == RuntimeFeatureStateToggle::On
			         && getHighlightedNotes()[noteCode] != 0) {
				image[y][x] = base.adjust(getHighlightedNotes()[noteCode], 1);
			}

			// Or, if this note is just within the current scale, show it dim
			else if (octaveScaleNotes[noteWithinOctave]) {
				image[y][x] = base.forTail();
			}

			// turn off other pads
			else {
				image[y][x] = colours::black;
			}

			// TODO: In a future revision it would be nice to add this to the API
			//  Dim note pad if a browser is open with the note highlighted
			if (getCurrentUI() == &sampleBrowser || getCurrentUI() == &audioRecorder
			    || (getCurrentUI() == &soundEditor && soundEditor.getCurrentMenuItem()->isRangeDependent())) {
				if (soundEditor.isUntransposedNoteWithinRange(noteCode)) {
					for (int32_t colour = 0; colour < 3; colour++) {
						int32_t value = (int32_t)image[y][x][colour] + 35;
						image[y][x][colour] = std::min<int32_t>(value, std::numeric_limits<uint8_t>::max());
					}
				}
			}

			noteCode += colInterval;
			++normalizedPadOffset;
			noteWithinOctave = (noteWithinOctave + colInterval) % octave_size;
		}
	}
}

bool KeyboardLayoutMicrotonal::drawNoteCode(int32_t noteCode) {
	int notenum = noteCode % 31;
	int octave = noteCode / 31;
	int notevals[] = {0, 5, 10, 13, 18, 23, 28};
	char notenames[] = {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
	int baseval = 0;
	int delta = 0;
	for (int note: notevals) {
		delta = notenum-note;
		if (-2 <= delta && delta <= 2) {
			break;
		}
		baseval++;
	}

	char deltaname[] = {'b', '\\', ' ', '|', '#'};
	char notestr[4] = {notenames[baseval], deltaname[delta+2], (char)('0'+octave), 0};

	display->popupTextTemporary(notestr);
	return true;
}

} // namespace deluge::gui::ui::keyboard::layout
