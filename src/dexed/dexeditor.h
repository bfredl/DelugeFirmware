#include "gui/ui/sound_editor.h"
#include "dexed/engine.h"
#include "dexed/PluginData.h"
#include "gui/ui/browser/browser.h"

// #define DEXED_EDITOR
#ifdef DEXED_EDITOR

class Dx7UI final : public UI {
public:
	Dx7UI() {};
	bool opened() override;
	void focusRegained() override;
	ActionResult padAction(int32_t x, int32_t y, int32_t velocity) override;
	ActionResult buttonAction(deluge::hid::Button b, bool on, bool inCardRoutine) override;
	void selectEncoderAction(int8_t offset) override;
	ActionResult verticalEncoderAction(int32_t offset, bool inCardRoutine) override;
	ActionResult horizontalEncoderAction(int32_t offset) override;

	bool renderMainPads(uint32_t whichRows, uint8_t image[][kDisplayWidth + kSideBarWidth][3],
	                    uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth], bool drawUndefinedArea = false) override;
	bool renderSidebar(uint32_t whichRows, uint8_t image[][kDisplayWidth + kSideBarWidth][3],
	                   uint8_t occupancyMask[][kDisplayWidth + kSideBarWidth]) override;
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

  void openFile(const char* path);
  void doLoad(int delta);

  void renderUI();

  ActionResult potentialShortcutPadAction(int x, int y, int on);

  bool did_button = false;
  enum State {
    kStateNone,
    kStateEditing,
    kStateLoading,
  };
  State state = kStateNone;
  int param = 0;
  int paramExtra = -1;
  Dx7Patch *patch;
  bool isScale;

  // TODO: this is huge, use alloc
  Cartridge cart;
  bool cartValid = false;
  int cartPos = 0;
  int uplim = 127;

  bool loadPending = false;
  char progName[11];
};
extern Dx7UI dx7ui;

class Dx7SyxBrowser final : public Browser {
public:
	Dx7SyxBrowser();
	bool opened();
	void enterKeyPress();
	int32_t getCurrentFilePath(String* path) override;
};

#endif
