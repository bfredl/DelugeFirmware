/*
 * Copyright © 2015-2023 Synthstrom Audible Limited
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

#include "param.h"
#include "dsp/dexed/dx7note.h"
#include "gui/ui/keyboard/keyboard_screen.h"
#include "gui/ui/sound_editor.h"
#include "gui/ui_timer_manager.h"
#include "hid/buttons.h"
#include "hid/display/oled.h"
#include "model/song/song.h"
#include "processing/source.h"
#include <cstring>

namespace deluge::gui::menu_item {

DxParam dxParam{l10n::String::STRING_FOR_DX_PARAM};

void DxParam::beginSession(MenuItem* navigatedBackwardFrom) {
	readValueAgain();
}

#define MAX_PARAM_IDX 143

int DxParam::getValue() {
	if (param >= 0 && param <= MAX_PARAM_IDX) {
		return patch->params[param];
	} else if (param == -1) {
		return patch->random_detune;
	} else {
		return 0;
	}
}

void DxParam::setValue(int val) {
	if (param >= 0 && param <= MAX_PARAM_IDX) {
		patch->params[param] = val;
	} else if (param == -1) {
		patch->random_detune = val;
	}

	soundEditor.currentSource->dxPatchChanged = true;
}

void DxParam::readValueAgain() {
	patch = soundEditor.currentSource->ensureDxPatch();
	displayValue = getValue();

	int x = -1, y = -1;

	upper_limit = 99;
	int op = param / 21;
	int idx = param % 21;
	if (param < 6 * 21) {
		if (idx == 11 || idx == 12) {
			upper_limit = 3;
		}
		else if (idx == 13 || idx == 15) {
			upper_limit = 7;
		}
		else if (idx == 14) {
			upper_limit = 3;
		}
		else if (idx == 20) {
			upper_limit = 14;
		}

		y = 7 - op;
		if (idx < 8) {
			x = idx;
		}
		else if (idx >= 16) {
			x = (idx - 16) + 8;
		}
		else if (idx == 16) {
			x = 13;
		}
	}
	else if (param == 6 * 21 + 8) {
		upper_limit = 31;
	}
	else if (param == 6 * 21 + 9) {
		upper_limit = 7;
	}
	else if (param == 6 * 21 + 10) {
		upper_limit = 1;
	}

	if (param >= 6 * 21 && param < 6 * 21 + 11) {
		y = 1;
		x = param - 6 * 21;
	}

	drawValue();
	if (x >= 0 && y >= 0) {
		// TODO: different color!
		soundEditor.setupShortcutBlink(x, y, 1);
		soundEditor.blinkShortcut();
	}
	else {
		uiTimerManager.unsetTimer(TimerName::SHORTCUT_BLINK);
	}
}

void DxParam::selectEncoderAction(int32_t offset) {
	int value = getValue();

	int newval = value + offset;
	if (newval > upper_limit) {
		newval = upper_limit;
	}
	else if (newval < 0) {
		newval = 0;
	}

	setValue(newval);
	displayValue = newval;

	drawValue();
}

void DxParam::horizontalEncoderAction(int32_t offset) {
	if (Buttons::isShiftButtonPressed()) {
		if (param > 6 * 21 + 8)
			return; // TODO: remember last OP param?
		int newVal = param + 21 * offset;
		if (newVal >= 0 and newVal < 6 * 21 + 8) {
			param = newVal;
		}
	}
	else {
		param = param + offset;
		if (param < -1) {
			param = 143;
		}
		else if (param >= 143) {
			param = -1;
		}
	}

	readValueAgain();
}

bool DxParam::potentialShortcutPadAction(int32_t x, int32_t y, bool on) {
	int found_param = -1;
	if (y > 1 && x <= 13) {
		int op = 8 - y - 1; // op 0-5
		int idx;
		if (x < 8) {
			idx = x;
		}
		else if (x < 13) {
			idx = (x - 8) + 16;
		}
		else if (x == 13) {
			idx = 15;
		}
		found_param = 21 * op + idx;
	}
	else if (y == 1) {
		if (x < 11) {
			found_param = 6 * 21 + x;
		}
	}

	if (found_param >= 0) {
		param = found_param;
		readValueAgain();
		return true;
	}

	return false;
}

const char* desc_op_long[] = {"env rate1",   "env rate2",  "env rate3",  "env rate4",     "env level1",  "env level2",
                              "env level3",  "env level4", "breakpoint", "left depth",    "right depth", "left curve",
                              "right curve", "rate scale", "ampmod",     "velocity sens", "level",       "mode",
                              "coarse",      "fine",       "detune"};

const char* desc_global_long[]{"DX7 pitch R1", "DX7 pitch R2",     "DX7 pitch R3",  "DX7 pitch R4",  "DX7 pitch l1",
                               "DX7 pitch l2", "DX7 pitch l3",     "DX7 pitch l4",  "DX7 algoritm",  "DX7 feedback",
                               "DX7 oscSync",  "DX7 LFO rate",     "DX7 LFO delay", "DX7 LFO pitch", "DX7 LFO amp",
                               "DX7 LFO sync", "DX7 LFO waveform", "DX7 pitch sens"};

const char* curves[]{"lin-", "exp-", "exp+", "lin+", "????"};
const char* shapes_long[]{"tri", "saw down", "saw up", "square", "sin", "s-hold"};
const char* shapes_short[]{"tri", "sawd", "sawu", "sqre", "sin", "shld"};

[[nodiscard]] std::string_view DxParam::getTitle() const {
	static char buffer[25];

	if (param < 0) {
		return "random detune";
	}

	int op = param / 21;
	int idx = param % 21;
	if (param < 6 * 21) {

		strcpy(buffer, "op0 ");
		buffer[2] = '6' - op;

		const char* desc;
		if (idx < 8) {
			desc = "envelope";
		}
		else if (idx < 13) {
			desc = "scaling";
		}
		else if (idx < 16) {
			desc = "params";
		}
		else {
			desc = "tune/level";
		}

		strcpy(buffer + 4, desc);

		return buffer;
	}
	else if (param < 6 * 21 + 8) {
		return "dx7 pitch env";
	}
	else if (param >= 137 && param < 144) {
		return "dx7 LFO";
	}
	else if (param < 6 * 21 + 18) {
		return desc_global_long[param - 6 * 21];
	}

	return "DX7 PARAM";
}

using deluge::hid::display::OLED;
static void show(const char* text, int r, int c, bool inv = false) {
	int ybel = 7 + (2 + r) * (kTextSizeYUpdated + 2);
	int xpos = 5 + c * kTextSpacingX;
	OLED::drawString(text, xpos, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
	                 kTextSizeYUpdated);
	if (inv) {
		int width = strlen(text);
		OLED::invertArea(xpos - 1, kTextSpacingX * width + 1, ybel - 1, ybel + kTextSizeYUpdated, OLED::oledMainImage);
	}
}

static void show(int val, int r, int c, bool inv = false) {
	char buffer[12];
	intToString(val, buffer, 2);
	show(buffer, r, c, inv);
}

static void renderEnvelope(uint8_t* patch, int op, int idx) {
	show("rate", 0, 0, false);
	show("levl", 1, 0, false);
	for (int i = 0; i < 4; i++) {
		int val = patch[op * 21 + i];
		show(val, 0, 5 + i * 3, (i == idx));
		val = patch[op * 21 + 4 + i];
		show(val, 1, 5 + i * 3, (i + 4 == idx));
	}
}

static void renderScaling(uint8_t* patch, int op, int idx) {
	char buffer[12];

	for (int i = 0; i < 2; i++) {
		int val = patch[op * 21 + 9 + i];
		show(val, 0, 1 + i * 11, (9 + i == idx));

		val = patch[op * 21 + 11 + i];
		int k = std::min(val, 4);
		show(curves[k], 1, 1 + i * 11, (11 + i == idx));
	}

	int ybelmid = 7 + 2 * (kTextSizeYUpdated + 2) + ((kTextSizeYUpdated + 1) >> 1);
	int val = patch[op * 21 + 8];
	intToString(val, buffer, 2);
	int xpos = 14 + 6 * kTextSpacingX;
	OLED::drawString(buffer, xpos, ybelmid, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
	                 kTextSizeYUpdated);
	if (8 == idx) {
		OLED::invertArea(xpos - 1, kTextSpacingX * 2 + 1, ybelmid - 1, ybelmid + kTextSizeYUpdated,
		                 OLED::oledMainImage);
	}

	int noteCode = val + 17;
	if (getRootUI() == &keyboardScreen) {
		// TODO: borked
		InstrumentClip* clip = getCurrentInstrumentClip();
		int notedisp = 0; // noteCode - clip->yScrollKeyboardScreen;
		int x = notedisp;
		int y = 0;
		while (x > 10 && y < 7) {
			x -= 5; // fUBBIGT
			y += 1;
		}
		if (x >= 0 && x < 16) {
			// TODO: different color, do not override the main shortcut blink
			soundEditor.setupShortcutBlink(x, y, 2);
			soundEditor.blinkShortcut();
		}
	}
}

static void renderSensParams(uint8_t* patch, int op, int idx) {
	show("rate scale", 0, 1);
	show(patch[op * 21 + 13], 0, 12, idx == 13);

	show("ampmod", 1, 0);
	show(patch[op * 21 + 14], 1, 7, idx == 14);

	show("velo", 1, 11);
	show(patch[op * 21 + 15], 1, 16, idx == 15);
}

static void renderTuning(uint8_t* patch, int op, int param) {
	int mode = patch[op * 21 + 17];
	const char* text = mode ? "fixed" : "ratio";
	show(text, 0, 1, (17 == param));

	for (int i = 0; i < 3; i++) {
		int val = patch[op * 21 + i + 18];
		if (i + 18 == 20)
			val -= 7;
		show(val, 0, 7 + i * 3, (i + 18 == param));
	}

	show("level", 1, 1);

	int val = patch[op * 21 + 16];
	show(val, 1, 7, (16 == param));
}

static void renderLFO(uint8_t* patch, int param) {
	char buffer[12];
	int ybel = 5 + 2 * (kTextSizeYUpdated + 2) + 2;
	int ybel2 = ybel + (kTextSizeYUpdated + 2);

	for (int i = 0; i < 2; i++) {
		int val = patch[137 + i];
		show(val, 0, 1 + i * 3, (137 + i == param));
	}

	for (int i = 0; i < 2; i++) {
		int val = patch[139 + i];
		intToString(val, buffer, 2);
		int xpre = 10 + (i * 9) * kTextSpacingX;
		int xpos = xpre + 6 * kTextSpacingX;
		const char* text = (i == 0) ? "pitch" : "  amp";
		show(text, 1, 1 + i * 9);
		show(val, 1, 1 + i * 9 + 6, (i + 139 == param));
	}

	const char* sync = (patch[141] ? "sync" : "    ");
	show(sync, 0, 7, (141 == param));
	int shap = std::min((int)patch[142], 5);
	show(shapes_long[shap], 0, 12, (142 == param));

	int val = patch[143];
	intToString(val, buffer, 1);
	show(buffer, 1, 10, (143 == param));
}

static void renderAlgorithm(uint8_t* patch) {

	char buffer[12];
	intToString(patch[134] + 1, buffer, 2);
	OLED::drawString(buffer, 113, 7, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX, kTextSizeYUpdated);

	FmAlgorithm a = FmCore::algorithms[patch[134]];
	for (int i = 0; i < 6; i++) {
		int f = a.ops[i];
		char buffer[12];
		int inbus = (f >> 4) & 3;
		int outbus = f & 3;
		const char ib[] = {'.', 'x', 'y', 'z'};
		const char ob[] = {'c', 'x', 'y', 'q'};
		buffer[0] = '1' + i;
		buffer[1] = ':';
		buffer[2] = ib[inbus];
		buffer[3] = (f & OUT_BUS_ADD) ? '+' : '>';
		buffer[4] = ob[outbus];
		buffer[5] = (f & (FB_IN | FB_OUT)) ? 'f' : ' ';
		buffer[6] = 0;

		int r = i / 3, c = i % 3;
		show(buffer, r, c * 7);
	}
}

void DxParam::drawPixelsForOled() {
	const int y0 = 20;
	char buffer[12];

	if (param < 0 || param == 135 || param == 136) {
		int val = getValue();
		intToString(val, buffer, param < 0 ? 2 : 1);
		OLED::drawString(buffer, 50, y0, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextHugeSpacingX,
		                 kTextHugeSizeY);
		return;
	}

	int op = param / 21;
	int idx = param % 21;
	if (param < (6 * 21 + 8) && idx < 8) {
		renderEnvelope(patch->params, op, idx); // op== 6 for pitch envelope
	}
	else if (param < 6 * 21 && idx < 13) {
		renderScaling(patch->params, op, idx);
	}
	else if (param < 6 * 21 && idx < 16) {
		renderSensParams(patch->params, op, idx);
	}
	else if (param < 6 * 21 && idx < 21) {
		renderTuning(patch->params, op, idx);
	}
	else if (param == 134) {
		renderAlgorithm(patch->params);
	}
	else if (param >= 137 && param < 144) {
		renderLFO(patch->params, param);
	}
	else {
		/*
		TODO: useful for "learn" popup
		intToString(param, buffer, 3);
		OLED::drawString(buffer, 0, y0, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);

		OLED::drawString(desc_global_long[param - 6 * 21], 4 * kTextSpacingX, y0, OLED::oledMainImage[0],
		                 OLED_MAIN_WIDTH_PIXELS, kTextSpacingX, kTextSizeYUpdated);

		int val = getValue();
		if (param == 6 * 21 + 8) {
			val += 1; // algorithms start at one
		}
		else if (param < 6 * 21 && (idx == 20)) {
			val -= 7; // detuning -7 - 7
		}
		int ybel = y0 + kTextSizeYUpdated + 2;
		intToString(val, buffer, 3);
		OLED::drawString(buffer, 0, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);
		*/
	}
}

void DxParam::drawValue() {
	if (display->haveOLED()) {
		renderUIsForOled();
	}
	else {
		// 7seg here
	}
}

void DxParam::openForOp(int op) {
	bool was_focused = true;  // was already in DxParam
	if (!isUIOpen(&soundEditor) || soundEditor.getCurrentMenuItem() != this) {
		bool success = soundEditor.setup(getCurrentClip(), this, 0);
		if (!success) {
			return;
		}
		soundEditor.enterOrUpdateSoundEditor(true);
		was_focused = false;
	}


	int new_param = op*21+16;
	if (was_focused) {
		if (param == new_param) {
			new_param = op*21+18;
		} else if (0 <= param && param < 6*21) {
			int param_for_op = op*21+(param % 21);
			if (param != param_for_op) {
				new_param = param_for_op;
			}
		}
	}
	param = new_param;
	readValueAgain();
}

} // namespace deluge::gui::menu_item
