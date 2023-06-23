#include "soundeditor.h"
#include "dexed/engine.h"
#include "dexed/PluginData.h"

class Dx7UI final : public UI {
public:
  Dx7UI() {};
	void focusRegained();
  int padAction(int x, int y, int velocity);
  int buttonAction(int x, int y, bool on, bool inCardRoutine);
	void selectEncoderAction(int8_t offset);


	bool renderMainPads(uint32_t whichRows, uint8_t image[][displayWidth + sideBarWidth][3],
	                    uint8_t occupancyMask[][displayWidth + sideBarWidth], bool drawUndefinedArea = false);
	bool renderSidebar(uint32_t whichRows, uint8_t image[][displayWidth + sideBarWidth][3],
	                   uint8_t occupancyMask[][displayWidth + sideBarWidth]);
#if HAVE_OLED
  void renderOLED(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]);
#endif

  void openFile();
  void doLoad(int delta);

  int potentialShortcutPadAction(int x, int y, int on);

  bool did_button = false;
  enum State {
    kStateNone,
    kStateEditing,
    kStateLoading,
  };
  State state = kStateNone;
  int param = 0;
  Dx7Patch *patch;

  // TODO: this is huge, use alloc
  Cartridge cart;
  bool cartValid = false;
  int cartPos = 0;

  bool loadPending = false;
  char progName[11];
};
extern Dx7UI dx7ui;

