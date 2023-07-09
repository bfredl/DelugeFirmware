/*
 * Copyright © 2021-2023 Synthstrom Audible Limited
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

#include "midi_test.h"

#include "gui/menu_item/midi/devices.h"
#include "io/midi/midi_device.h"

namespace menu_item::midi {

void MidiTest::beginSession(MenuItem* navigatedBackwardFrom) {
	soundEditor.currentValue = 0;
}

static const int numValues = 6;
static const char *itemNames[] = {"short sysex", "long sysex", "tetra rep", "tetra ask", "size override", "send mode"};

void MidiTest::drawValue() {
#if HAVE_OLED
	renderUIsForOled();
#else
	numericDriver.setScrollingText(itemNames[soundEditor.currentValue]);
#endif
}

#if HAVE_OLED
void MidiTest::drawPixelsForOled() {
	int selectedRow = soundEditor.currentValue;
	int off = selectedRow - 1;
	if (off > numValues-3) off = numValues-3;
	// no else!
	if (off < 0) off = 0;
	drawItemsForOled(itemNames+off, selectedRow-off);
}
#endif

void MidiTest::selectEncoderAction(int offset) {
	int newValue = soundEditor.currentValue + offset;
	if (newValue >= numValues) {
		newValue = 0;
	} else if (newValue < 0) {
		newValue = numValues -1;
	}
	soundEditor.currentValue = newValue;
	drawValue();
}

MenuItem* MidiTest::selectButtonPress() {
	soundEditor.currentMIDIDevice->doSysexTest(soundEditor.currentValue);

	return (MenuItem*)0xFFFFFFFF;
}

}
