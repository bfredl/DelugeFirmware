#include "dsp/dexed/engine.h"
#include "gui/ui/browser/browser.h"
#include "gui/ui/sound_editor.h"

#pragma once

#define DEXED_EDITOR
#ifdef DEXED_EDITOR

class DexedEditor final : public UI {
public:
	DexedEditor(){};
	bool opened() override;
	void focusRegained() override;
	ActionResult padAction(int32_t x, int32_t y, int32_t velocity) override;
	ActionResult buttonAction(deluge::hid::Button b, bool on, bool inCardRoutine) override;
	void selectEncoderAction(int8_t offset) override;
	ActionResult verticalEncoderAction(int32_t offset, bool inCardRoutine) override;
	ActionResult horizontalEncoderAction(int32_t offset) override;

	bool renderMainPads(uint32_t whichRows, RGB image[][kDisplayWidth + kSideBarWidth],
	                    uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth], bool drawUndefinedArea = true) override;
	bool renderSidebar(uint32_t whichRows, RGB image[][kDisplayWidth + kSideBarWidth] = NULL,
	                   uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth] = NULL) override;
	bool getGreyoutRowsAndCols(uint32_t* cols, uint32_t* rows) override;
	void renderOLED(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) override;
	void renderEnvelope(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param);
	void renderScaling(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param);
	void renderTuning(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param);
	void renderLFO(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int param);
	void renderAlgorithm(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]);
	void show7Seg();
	void flash7SegParam();

	ActionResult timerCallback() override;

	bool timerRedraw;

	void renderUI();

	UIType getUIType() final { return UIType::BROWSER; }

	ActionResult potentialShortcutPadAction(int x, int y, int on);

	bool did_button = false;
	enum State {
		kStateNone,
		kStateEditing,
	};
	State state = kStateNone;
	int param = 0;
	int paramExtra = -1;
	Dx7Patch* patch;
	bool isScale;

	int uplim = 127;
};
extern DexedEditor dexedEditor;

#endif
