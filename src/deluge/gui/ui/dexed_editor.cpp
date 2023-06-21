#include "gui/views/instrument_clip_view.h"
#include "model/clip/instrument_clip.h"
#include "model/model_stack.h"
#include "model/note/note_row.h"
#include "model/song/song.h"
#include "processing/sound/sound_instrument.h"
#include <cstring>
#include <new>
// #include "model/drum/kit.h"
#include "dexed_editor.h"
#include "dsp/dexed/engine.h"
#include "extern.h"
#include "gui/ui/keyboard/keyboard_screen.h"
#include "gui/ui/root_ui.h"
#include "gui/ui_timer_manager.h"
#include "hid/buttons.h"
#include "hid/display/oled.h"
#include "hid/led/indicator_leds.h"
#include "hid/led/pad_leds.h"
#include "memory/general_memory_allocator.h"

#ifdef DEXED_EDITOR

using deluge::hid::display::OLED;

DexedEditor dexedEditor;

void DexedEditor::renderUI() {
	if (display->haveOLED()) {
		renderUIsForOled();
	}
	else {
		show7Seg();
	}
}

static void popupText(const char* text, bool persist) {
	display->displayPopup(text, persist ? 0 : 3);
}

bool DexedEditor::opened() {
	state = kStateNone;
	focusRegained();
	return true;
}

void DexedEditor::focusRegained() {
	uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);

	// after patch has been selekt:
	PadLEDs::reassessGreyout();
}

ActionResult DexedEditor::padAction(int32_t x, int32_t y, int32_t on) {
	if (x == kDisplayWidth && on && y > 1) {
		int op = 8 - y - 1; // op 0-5
		char* val = &patch->opSwitch[op];
		*val = (*val == '0' ? '1' : '0');
		uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
		PadLEDs::reassessGreyout();
	}

	if (x >= kDisplayWidth)
		return ActionResult::DEALT_WITH;

	ActionResult result = potentialShortcutPadAction(x, y, on);
	if (result != ActionResult::NOT_DEALT_WITH)
		return result;

	if (getRootUI() == &keyboardScreen) {
		if (x < kDisplayWidth) {
			keyboardScreen.padAction(x, y, on);
			return ActionResult::DEALT_WITH;
		}
	}

	if (sdRoutineLock && !allowSomeUserActionsEvenWhenInCardRoutine) {
		return ActionResult::REMIND_ME_OUTSIDE_CARD_ROUTINE; // Allow some of the time when in card routine.
	}

	return ActionResult::NOT_DEALT_WITH;
}

ActionResult DexedEditor::potentialShortcutPadAction(int x, int y, int on) {
	if (!on || x >= kDisplayWidth || !Buttons::isShiftButtonPressed()) {
		return ActionResult::NOT_DEALT_WITH;
	}

	state = kStateNone;
	param = -1;
	paramExtra = -1;

	uplim = 127;

	if (y > 1) {
		uplim = 99;
		int op = 8 - y - 1; // op 0-5
		int ip = 0;
		if (isScale && x < 5) {
			ip = 8 + x;
		}
		else if (x < 8) {
			ip = x;
		}
		else if (x < 13) {
			ip = (x - 8) + 16;
		}
		else if (x < 16) {
			ip = (x - 13) + 13;
		}
		param = 21 * op + ip;
		if (ip == 17) {
			uplim = 1;
		}
		else if (ip == 11 || ip == 12) {
			uplim = 3;
		}
		else if (ip == 13 || ip == 15) {
			uplim = 7;
		}
		else if (ip == 14) {
			uplim = 3;
		}
		else if (ip == 20) {
			uplim = 14;
		}
	}
	else if (y == 0) {
		if (x < 11) {
			param = 6 * 21 + x;
			if (x < 8) {
				uplim = 99;
			}
			else if (x == 8) {
				uplim = 31;
			}
			else if (x == 9) {
				uplim = 7;
			}
			else if (x == 10) {
				uplim = 1;
			}
		}
		else if (x == 11) {
			paramExtra = 1;
		}
	}
	else if (y == 1) {
		if (x == 0) {
			isScale = !isScale;
		}
		else if (x == 2) {
			// TODO: these options are not yet saved!
#ifdef USE_DEXED_MK1
			patch->setEngineVintage();
			popupText("Vintage engine", false);
#else
			popupText("disabled (no vintage engine)", false);
#endif
		}
		else if (x == 3) {
			patch->setEngineModern(false);
			popupText("Modern engine (-)", false);
		}
		else if (x == 4) {
			patch->setEngineModern(true);
			popupText(display->haveOLED() ? "Modern engine (NEON)" : "NEON", false);
		}
		else if (x >= 8 && x < 16) {
			int order[8] = {0, 1, 4, 5, 2, 6, 3, 7};
			param = 137 + order[x - 8];

			if (param == 141) {
				uplim = 1;
			}
			else if (param == 142) {
				uplim = 5;
			}
			else if (param == 143) {
				uplim = 7;
			}
			else {
				uplim = 99;
			}
		}
	}

	if (param >= 0 or paramExtra >= 0) {
		state = kStateEditing;
		soundEditor.setupShortcutBlink(x, y, 1);
		soundEditor.blinkShortcut();
	}
	else {
		uiTimerManager.unsetTimer(TIMER_SHORTCUT_BLINK);
	}

	if (param >= 0 && !display->haveOLED()) {
		flash7SegParam();
	}
	else {
		renderUI();
	}

	uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
	return ActionResult::DEALT_WITH;
}

void DexedEditor::selectEncoderAction(int8_t offset) {
	if (patch && state == kStateEditing) {

		int current_val;
		if (param >= 0) {
			current_val = patch->currentPatch[param];
		}
		else if (paramExtra >= 0) {
			current_val = patch->random_detune;
		}

		bool scaleable = (param != 134 && param != 135);
		int scale = (scaleable && Buttons::isShiftButtonPressed()) ? 10 : 1;

		if (param >= 0 && current_val == uplim && Buttons::isShiftButtonPressed()) {
			if (param == 135) {
				uplim = 9; // why not a bit more feedback..
			}
			else if (param == 134) {
				uplim = 127; // MAYHEM MODE
			}
		}

		int newval = current_val + offset * scale;
		if (newval > uplim)
			newval = uplim;
		if (newval < 0)
			newval = 0;
		if (param >= 0) {
			patch->currentPatch[param] = newval;
		}
		else if (paramExtra >= 0) {
			patch->random_detune = newval;
		}
		renderUI();
		uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
	}
}

// TODO: overall we integrate quite poorly with instrumentClipView (no audition)
ActionResult DexedEditor::horizontalEncoderAction(int32_t offset) {
	if (getRootUI() == &keyboardScreen) {
		return getRootUI()->horizontalEncoderAction(offset);
	}
	return ActionResult::DEALT_WITH;
}

ActionResult DexedEditor::verticalEncoderAction(int32_t offset, bool inCardRoutine) {
	if (getRootUI() == &keyboardScreen) {
		return getRootUI()->verticalEncoderAction(offset, inCardRoutine);
	}
	return ActionResult::DEALT_WITH;
}

const char* desc_op_long[] = {"env rate1",   "env rate2",  "env rate3",  "env rate4",     "env level1",  "env level2",
                              "env level3",  "env level4", "breakpoint", "left depth",    "right depth", "left curve",
                              "right curve", "rate scale", "ampmod",     "velocity sens", "level",       "mode",
                              "coarse",      "fine",       "detune"};

const char* desc_op_short[] = {"er1", "er2", "er3", "er4", "el1", "er2", "er3", "er4", "brk", "led", "rid",
                               "lec", "ric", "rsc", "amd", "vel", "lvl", "trk", "coa", "fin", "det"};

const char* desc_global_long[]{"pitch rate1",  "pitch rate2",  "pitch rate3",   "pitch rate4",     "pitch level1",
                               "pitch level2", "pitch level3", "pitch level4",  "algoritm",        "feedback",
                               "oscSync",      "LFO rate",     "LFO delay",     "LFO pitch depth", "LPO amp depth",
                               "LFO sync",     "LFO waveform", "LFO pitch sens"};

const char* desc_global_short[]{"per1", "per2", "per3", "per4", "pel1", "pel2", "pel3", "pel4", "algo",
                                "fdbk", "sync", "lrat", "ldel", "lpde", "lamp", "lsyn", "lwav", "lpsn"};

const char* curves[]{"lin-", "exp-", "exp+", "lin+", "????"};
const char* shapes_long[]{"tri", "saw down", "saw up", "square", "sin", "s-hold"};
const char* shapes_short[]{"tri", "sawd", "sawu", "sqre", "sin", "shld"};

// change the coleur
// TODO: delenda est, this was from before "RGB" was a thing
void color(RGB* colour, int r, int g, int b) {
	colour->r = r;
	colour->g = g;
	colour->b = b;
}

bool DexedEditor::renderSidebar(uint32_t whichRows, RGB image[][kDisplayWidth + kSideBarWidth],
                                uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth]) {
	if (!image)
		return true;

	int algid = patch ? patch->currentPatch[134] : 0;
	FmAlgorithm a = FmCore::algorithms[algid];

	for (int i = 0; i < kDisplayHeight; i++) {
		if (!(whichRows & (1 << i)))
			continue;
		RGB* thisColour = &image[i][kDisplayWidth];
		int op = 8 - i - 1; // op 0-5

		bool muted = false;
		if (op < 6) {
			char* val = &patch->opSwitch[op];
			if (*val == '0') {
				color(thisColour, 255, 0, 0);
			}
			else if (a.ops[op] & (OUT_BUS_ONE | OUT_BUS_TWO)) {
				color(thisColour, 0, 128, 255); // modulator
			}
			else {
				color(thisColour, 0, 255, 0); // carrier
			}
		}
		else {
			color(thisColour, 0, 0, 0);
		}
	}

	return true;
}

bool DexedEditor::renderMainPads(uint32_t whichRows, RGB image[][kDisplayWidth + kSideBarWidth],
                                 uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth], bool drawUndefinedArea) {
	if (!image)
		return true;

	int editedOp = -1;
	if (state == kStateEditing && param >= 0 && param < 6 * 21) {
		editedOp = param / 21;
	}

	for (int i = 0; i < kDisplayHeight; i++) {
		if (!(whichRows & (1 << i)))
			continue;
		memset(image[i], 0, 3 * 16);
		int op = 8 - i - 1; // op 0-5
		if (op < 6) {
			char* val = &patch->opSwitch[op];
			for (int x = 0; x < 4; x++) {
				color(&image[i][x], 0, 120, 0);
				color(&image[i][x + 4], 0, 40, 110);
			}
			if (isScale) {
				for (int x = 0; x < 5; x++) {
					color(&image[i][x], 100, 150, 0);
				}
			}
			color(&image[i][8], 70, 70, 70);
			for (int x = 0; x < 3; x++) {
				color(&image[i][x + 10], 150, 0, 150);
			}
			color(&image[i][14], 120, 40, 40);
			color(&image[i][15], 150, 125, 0);
			if (op == editedOp && *val != '0') {
				for (int x = 0; x < 16; x++) {
					for (int c = 0; c < 3; c++) {
						image[i][x][c] = image[i][x][c] + (image[i][x][c] >> 2);
						if (image[i][x][c] < 30)
							image[i][x][c] = 30;
					}
				}
			}
		}
		else if (i == 0) {
			for (int x = 0; x < 4; x++) {
				color(&image[i][x], 40, 240, 40);
				color(&image[i][x + 4], 40, 140, 240);
			}
			color(&image[i][8], 255, 0, 0);
			color(&image[i][9], 100, 100, 100);
			color(&image[i][10], 100, 100, 100);
		}
		else if (i == 1) {
			if (isScale) {
				color(&image[i][0], 0, 120, 0);
			}
			else {
				color(&image[i][0], 100, 150, 0);
			}
			for (int x = 0; x < 3; x++) {
				color(&image[i][x + 2], 120, 10, 10);
			}
			for (int x = 0; x < 7; x++) {
				color(&image[i][x + 8], 120, 40, 40);
			}
		}
	}
	return true;
}

bool DexedEditor::getGreyoutRowsAndCols(uint32_t* cols, uint32_t* rows) {
	if (!patch) {
		return false;
	}

	for (int i = 0; i < 6; i++) {
		int row = 8 - i - 1;
		char* val = &patch->opSwitch[i];
		if (*val == '0') {
			*rows |= (1 << row);
		}
	}

	return true;
}

void DexedEditor::renderOLED(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	char buffer[12];
	if (patch && state == kStateEditing && param >= 0) {
		intToString(param, buffer, 3);
		OLED::drawString(buffer, 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);

		int op = param / 21;
		int idx = param % 21;
		if (param < 6 * 21) {

			buffer[0] = 'o';
			buffer[1] = 'p';
			buffer[2] = '6' - op;
			buffer[3] = ' ';
			buffer[4] = 0;

			OLED::drawString(buffer, 4 * kTextSpacingX, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS,
			                 kTextSpacingX, kTextSizeYUpdated);

			OLED::drawString(desc_op_long[idx], 8 * kTextSpacingX, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS,
			                 kTextSpacingX, kTextSizeYUpdated);
		}
		else if (param < 6 * 21 + 18) {
			OLED::drawString(desc_global_long[param - 6 * 21], 4 * kTextSpacingX, 5, OLED::oledMainImage[0],
			                 OLED_MAIN_WIDTH_PIXELS, kTextSpacingX, kTextSizeYUpdated);
		}

		int val = patch->currentPatch[param];
		if (param == 6 * 21 + 8) {
			val += 1; // algorithms start at one
		}
		else if (param < 6 * 21 && (idx == 20)) {
			val -= 7; // detuning -7 - 7
		}
		int ybel = 5 + kTextSizeYUpdated + 2;
		intToString(val, buffer, 3);
		OLED::drawString(buffer, 0, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);

		const char* extra = "";
		if (param < 6 * 21 && (idx == 11 || idx == 12)) {
			int kurva = std::min(val, 4);
			extra = curves[kurva];
		}
		else if (param < 6 * 21 && (idx == 17)) {
			;
			extra = val ? "fixed" : "ratio";
		}
		OLED::drawString(extra, 4 * kTextSpacingX, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);

		if (param < (6 * 21 + 8) && idx < 8) {
			renderEnvelope(image, op, idx); // op== 6 for pitch envelope
		}
		else if (param < 6 * 21 && idx < 13) {
			renderScaling(image, op, idx);
		}
		else if (param < 6 * 21 && idx < 16) {
			// :P
		}
		else if (param < 6 * 21 && idx < 21) {
			renderTuning(image, op, idx);
		}
		else if (param == 134) {
			renderAlgorithm(image);
		}
		else if (param >= 137 && param < 144) {
			renderLFO(image, param);
		}
	}
	else if (patch && state == kStateEditing && paramExtra >= 0) {
		OLED::drawString("--- random detune", 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);
		int ybel = 5 + kTextSizeYUpdated + 2;
		intToString(patch->random_detune, buffer, 3);
		OLED::drawString(buffer, 0, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
		                 kTextSizeYUpdated);
	}
	else {
		const char* text = "DX EDITOR";
		OLED::drawString(text, 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX, kTextSizeYUpdated);
	}
}

void show(const char* text, int r, int c, bool inv = false) {
	int ybel = 7 + (2 + r) * (kTextSizeYUpdated + 2);
	int xpos = 4 + c * kTextSpacingX;
	OLED::drawString(text, xpos, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
	                 kTextSizeYUpdated);
	if (inv) {
		int width = strlen(text);
		OLED::invertArea(xpos - 1, kTextSpacingX * width + 1, ybel - 1, ybel + kTextSizeYUpdated, OLED::oledMainImage);
	}
}

void show(int val, int r, int c, bool inv = false) {
	char buffer[12];
	intToString(val, buffer, 2);
	show(buffer, r, c, inv);
}

void DexedEditor::renderEnvelope(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
	for (int i = 0; i < 4; i++) {
		int val = patch->currentPatch[op * 21 + i];
		show(val, 0, i * 3, (i == param));
		val = patch->currentPatch[op * 21 + 4 + i];
		show(val, 1, i * 3, (i + 4 == param));
	}
}

void DexedEditor::renderScaling(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
	char buffer[12];

	for (int i = 0; i < 2; i++) {
		int val = patch->currentPatch[op * 21 + 9 + i];
		show(val, 0, 1 + i * 11, (9 + i == param));

		val = patch->currentPatch[op * 21 + 11 + i];
		int kurva = std::min(val, 4);
		show(curves[kurva], 1, 1 + i * 11, (11 + i == param));
	}

	int ybelmid = 7 + 2 * (kTextSizeYUpdated + 2) + ((kTextSizeYUpdated + 1) >> 1);
	int val = patch->currentPatch[op * 21 + 8];
	intToString(val, buffer, 2);
	int xpos = 14 + 6 * kTextSpacingX;
	OLED::drawString(buffer, xpos, ybelmid, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, kTextSpacingX,
	                 kTextSizeYUpdated);
	if (8 == param) {
		OLED::invertArea(xpos - 1, kTextSpacingX * 2 + 1, ybelmid - 1, ybelmid + kTextSizeYUpdated,
		                 OLED::oledMainImage);
	}

	int noteCode = val + 17;
	if (getRootUI() == &keyboardScreen) {
		InstrumentClip* clip = (InstrumentClip*)currentSong->currentClip;
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
		}
	}
}

void DexedEditor::renderTuning(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
	int mode = patch->currentPatch[op * 21 + 17];
	const char* text = mode ? "fixed" : "ratio";
	show(text, 0, 1, (17 == param));

	for (int i = 0; i < 3; i++) {
		int val = patch->currentPatch[op * 21 + i + 18];
		if (i + 18 == 20)
			val -= 7;
		show(val, 0, 7 + i * 3, (i + 18 == param));
	}

	show("level", 1, 1);

	int val = patch->currentPatch[op * 21 + 16];
	show(val, 1, 7, (16 == param));
}

void DexedEditor::renderLFO(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int param) {
	char buffer[12];
	int ybel = 5 + 2 * (kTextSizeYUpdated + 2) + 2;
	int ybel2 = ybel + (kTextSizeYUpdated + 2);

	for (int i = 0; i < 2; i++) {
		int val = patch->currentPatch[137 + i];
		show(val, 0, 1 + i * 3, (137 + i == param));
	}

	for (int i = 0; i < 2; i++) {
		int val = patch->currentPatch[139 + i];
		intToString(val, buffer, 2);
		int xpre = 10 + (i * 9) * kTextSpacingX;
		int xpos = xpre + 6 * kTextSpacingX;
		const char* text = (i == 0) ? "pitch" : "  amp";
		show(text, 1, 1 + i * 9);
		show(val, 1, 1 + i * 9 + 6, (i + 139 == param));
	}

	const char* sync = (patch->currentPatch[141] ? "sync" : "    ");
	show(sync, 0, 7, (141 == param));
	int shap = std::min((int)patch->currentPatch[142], 5);
	show(shapes_long[shap], 0, 12, (142 == param));

	int val = patch->currentPatch[143];
	intToString(val, buffer, 1);
	show(buffer, 1, 10, (143 == param));
}

void DexedEditor::renderAlgorithm(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
	FmAlgorithm a = FmCore::algorithms[patch->currentPatch[134]];

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

void DexedEditor::show7Seg() {
	int op = param / 21;
	int idx = param % 21;
	if (patch && state == kStateEditing) {
		const char* text = NULL;
		int val = patch->currentPatch[param];
		if (param == 6 * 21 + 8) {
			val += 1; // algorithms start at one
		}
		else if (param < 6 * 21 && (idx == 20)) {
			val -= 7; // detuning -7 - 7
		}
		else if (param < 6 * 21 && (idx == 17)) {
			text = val ? "fixd" : "rati";
		}
		else if (param < 6 * 21 && (idx == 11 || idx == 12)) {
			int kurva = std::min(val, 4);
			text = curves[kurva];
		}
		else if (param == 142) {
			int shap = std::min((int)patch->currentPatch[142], 5);
			text = shapes_short[shap];
		}

		if (text) {
			display->setText(text);
		}
		else {
			display->setTextAsNumber(val);
		}
	}
	else {
		display->setText("dexe");
	}
}

void DexedEditor::flash7SegParam() {
	char buffer[12];
	int op = param / 21;
	int idx = param % 21;
	if (param < 6 * 21) {
		buffer[0] = '1' + op;
		strncpy(buffer + 1, desc_op_short[idx], (sizeof buffer) - 1);
		display->setText(buffer);
	}
	else {
		if (param < 6 * 21 + 18) {
			display->setText(desc_global_short[param - 6 * 21]);
		}
	}
	timerRedraw = true;
	uiTimerManager.setTimer(TIMER_UI_SPECIFIC, 700);
}

ActionResult DexedEditor::timerCallback() {
	if (timerRedraw) {
		renderUI();
		timerRedraw = false;
	}
	return ActionResult::DEALT_WITH;
}

ActionResult DexedEditor::buttonAction(hid::Button b, bool on, bool inCardRoutine) {
	using namespace hid::button;
	if (b == CLIP_VIEW) {
		did_button = true;
		uiTimerManager.unsetTimer(TIMER_SHORTCUT_BLINK);
		state = kStateNone;
		renderUI();
	}
	else if (b == LEARN) {
		// TODO: this is not useful yet, we need to actually handle incoming sysex/CC to change parameter values
		char buffer[12];
		buffer[0] = 'p';
		intToString(param, buffer + 1, 3);
		display->setText(buffer);
	}
	else if (b == KEYBOARD && on) {
		// TODO: this is quite duplicate with soundEditor
		if (inCardRoutine) {
			return ActionResult::REMIND_ME_OUTSIDE_CARD_ROUTINE;
		}

		if (getRootUI() == &keyboardScreen) {
			swapOutRootUILowLevel(&instrumentClipView);
			instrumentClipView.openedInBackground();
		}
		else if (getRootUI() == &instrumentClipView) {
			swapOutRootUILowLevel(&keyboardScreen);
			keyboardScreen.openedInBackground();
		}

		indicator_leds::setLedState(IndicatorLED::KEYBOARD, getRootUI() == &keyboardScreen);
	}
	else if (b == BACK) {
		close();
	}
	return ActionResult::NOT_DEALT_WITH;
}

#endif
