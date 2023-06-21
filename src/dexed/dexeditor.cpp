#include <new>
#include "processing/sound/sound_instrument.h"
#include "model/song/song.h"
#include "model/clip/instrument_clip.h"
#include "model/note/note_row.h"
#include "model/model_stack.h"
#include "model/drum/kit.h"
#include "gui/ui/keyboard_screen.h"
#include "hid/buttons.h"
#include "memory/general_memory_allocator.h"
#include "gui/ui_timer_manager.h"
#include "hid/display/oled.h"
#include "gui/ui/root_ui.h"
#include "dexed/engine.h"
#include "dexed/PluginData.h"
#include "dexed/dexeditor.h"
#include "extern.h"

Dx7UI dx7ui;
static Dx7SyxBrowser dx7SyxBrowser;

void Dx7UI::renderUI() {
#if HAVE_OLED
  renderUIsForOled();
#endif
}

static void popupText(const char *text, bool persist) {
	numericDriver.displayPopup(text, persist ? 0 : 3);
}

void Dx7UI::openFile(const char *path) {
  const int xx = 4104;

  FILINFO fno;
  int result = f_stat(path, &fno);
  FSIZE_t fileSize = fno.fsize;
  if (fileSize < xx) {
    popupText("too small!", true);
  }

  FIL file;
  // Open the file
  result = f_open(&file, path, FA_READ);
  if (result != FR_OK) {
    popupText("read error 1", true);
    return;
  }

  int status;
  UINT numBytesRead;
  int readsize = min((int)fileSize, 8192);
  uint8_t* buffer = (uint8_t*)generalMemoryAllocator.alloc(readsize, NULL, false, true);
  if (!buffer) {
    popupText("out of memory", true);
	f_close(&file);
    return;
  }
  result = f_read(&file, buffer, fileSize, &numBytesRead);
  if (numBytesRead < xx) {
    popupText("read error 2", true);
    goto close;
  }

  status = cart.load(buffer, numBytesRead);
  if (status) {
    popupText("load error", true);
    goto close;
  }

  cartValid = true;
  cartPos = 0;
  doLoad(0);

close:
  f_close(&file);
free:
  generalMemoryAllocator.dealloc(buffer);
}

void Dx7UI::doLoad(int delta) {
  if (!patch || !cartValid) return;
  state = kStateLoading;
  uiTimerManager.unsetTimer(TIMER_SHORTCUT_BLINK);

  cartPos += delta;
  if (cartPos < 0) cartPos = 0;
  if (cartPos >= 32) cartPos = 31;

  cart.unpackProgram(patch->currentPatch, cartPos);
  Cartridge::normalizePgmName(progName, (const char *)&patch->currentPatch[145]);
  renderUI();
  uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
}

bool Dx7UI::opened() {
	state = kStateNone;
	loadPending = false;
	focusRegained();
	return true;
}

void Dx7UI::focusRegained() {
    uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);

    if (dexedEditedSource) {
      if (dexedEditedSource->dx7Patch == NULL) {
        void* memory = generalMemoryAllocator.alloc(sizeof(Dx7Patch), NULL, false, true);
        dexedEditedSource->dx7Patch = new(memory) Dx7Patch;
      }
      patch = dexedEditedSource->dx7Patch;
    } else {
      patch = NULL;
    }
}

int Dx7UI::padAction(int x, int y, int on) {
  if (x == displayWidth && on && y > 1) {
    int op = 8-y-1; // op 0-5
    char* val = &patch->opSwitch[op];
    *val = (*val == '0' ? '1' : '0');
    uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
  }

  if (x >= displayWidth) return ACTION_RESULT_DEALT_WITH;

		int result = potentialShortcutPadAction(x, y, on);
		if (result != ACTION_RESULT_NOT_DEALT_WITH) return result;

	if (getRootUI() == &keyboardScreen) {
		if (x < displayWidth) {
			keyboardScreen.padAction(x, y, on);
			return ACTION_RESULT_DEALT_WITH;
		}
	}

  if (sdRoutineLock && !allowSomeUserActionsEvenWhenInCardRoutine) {
    return ACTION_RESULT_REMIND_ME_OUTSIDE_CARD_ROUTINE; // Allow some of the time when in card routine.
  }

  return ACTION_RESULT_NOT_DEALT_WITH;
}

int Dx7UI::potentialShortcutPadAction(int x, int y, int on) {
  if (!on || x >= displayWidth || !Buttons::isShiftButtonPressed()) {
    return ACTION_RESULT_NOT_DEALT_WITH;
  }

  state = kStateNone;
  param = -1;

  uplim = 127;

  if (y > 1) {
    uplim = 99;
    int op = 8-y-1; // op 0-5
    int ip = 0;
    if (isScale && x < 5) {
      ip = 8+x;
    } else if (x < 8) {
      ip = x;
    } else if (x < 13) {
      ip = (x-8)+16;
    } else if (x < 16) {
      ip = (x-13)+13;
    }
    param = 21*op+ip;
    if (ip == 17) {
      uplim = 1;
    } else if (ip == 11 || ip == 12) {
      uplim = 3;
    } else if (ip == 13 || ip == 15) {
      uplim = 7;
    } else if (ip == 14) {
      uplim = 3;
    } else if (ip == 20) {
      uplim = 14;
    }
  } else  if (y==0 && x < 10) {
    param = 6*21+x;
    if (x < 8) {
      uplim = 99;
    } else if (x == 9) {
      uplim = 7;
    }
  } else if (y == 1) {
    if (x == 0) {
      isScale = !isScale;
    } else if (x>=8 && x <16) {
      int order[8] = {0, 1, 4, 5, 2, 6, 3, 7};
      param = 137+order[x-8];

      if (param == 141) {
        uplim = 1;
      } else if (param == 142) {
        uplim = 5;
      } else if (param == 143) {
        uplim = 7;
      } else {
        uplim = 99;
      }
    }
  }

  if (param >= 0) {
    state = kStateEditing;
    loadPending = false;
    soundEditor.setupShortcutBlink(x, y, 1);
    soundEditor.blinkShortcut();
  } else {
    uiTimerManager.unsetTimer(TIMER_SHORTCUT_BLINK);
  }

  renderUI();
  uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
  return ACTION_RESULT_DEALT_WITH;
}

void Dx7UI::selectEncoderAction(int8_t offset) {
  if (patch && state == kStateEditing) {

    int newval = patch->currentPatch[param]+offset;
    if (newval > uplim) newval = uplim;
    if (newval < 0) newval = 0;
    patch->currentPatch[param] = newval;
    renderUI();
    uiNeedsRendering(this, 0xFFFFFFFF, 0xFFFFFFFF);
  } else if (state == kStateLoading) {
    doLoad(offset);
  }

}

const char *desc[] = {
  "env rate1", "env rate2", "env rate3", "env rate4",
  "env level1", "env level2", "env level3", "env level4",
  "breakpoint", "left depth", "right depth", "left curve", "right curve",
  "rate scale", "ampmod", "velocity sens",
  "level", "mode", "coarse", "fine", "detune"
};

const char *desc_global[] {
  "pitch rate1", "pitch rate2", "pitch rate3", "pitch rate4",
  "pitch level1", "pitch level2", "pitch level3", "pitch level4",
  "algoritm", "feedback", "oscSync",
  "LFO rate", "LFO delay", "LFO pitch depth", "LPO amp depth", "LFO sync", "LFO waveform", "LFO pitch sens"
};

const char *curves[] {"lin-", "exp-", "exp+", "lin+", "?????"};
const char *shapes[] {"tri", "saw down", "saw up", "square", "sin", "s-hold"};

// change the coleur
void color(uint8_t *colour, int r, int g, int b) {
  colour[0] = r;
  colour[1] = g;
  colour[2] = b;
}

bool Dx7UI::renderSidebar(uint32_t whichRows, uint8_t image[][displayWidth + sideBarWidth][3],
                                       uint8_t occupancyMask[][displayWidth + sideBarWidth]) {
	if (!image) return true;

  int algid = patch ? patch->currentPatch[134] : 0;
  FmAlgorithm a = FmCore::algorithms[algid];

	for (int i = 0; i < displayHeight; i++) {
		if (!(whichRows & (1 << i))) continue;
    uint8_t* thisColour = image[i][displayWidth];
    int op = 8-i-1; // op 0-5

    bool muted = false;
    if (op < 6) {
      char* val = &patch->opSwitch[op];
      if (*val == '0') {
        color(thisColour, 255, 0, 0);
      } else if (a.ops[op] & (OUT_BUS_ONE | OUT_BUS_TWO))  {
        color(thisColour, 0, 128, 255);  // modulator
      } else {
        color(thisColour, 0, 255, 0);  // carrier
      }
    } else {
        color(thisColour, 0, 0, 0);
    }
	}

	return true;
}

bool Dx7UI::renderMainPads(uint32_t whichRows, uint8_t image[][displayWidth + sideBarWidth][3],
                                    uint8_t occupancyMask[][displayWidth + sideBarWidth], bool drawUndefinedArea) {
	if (!image) return true;

  int editedOp = -1;
  if (state == kStateEditing && param < 6*21) {
    editedOp = param/21;
  }

	for (int i = 0; i < displayHeight; i++) {
		if (!(whichRows & (1 << i))) continue;
    memset(image[i], 0, 3*16);
    int op = 8-i-1; // op 0-5
    if (op < 6) {
      char* val = &patch->opSwitch[op];
      for (int x = 0; x < 4; x++) {
        color(image[i][x], 0, 120, 0);
        color(image[i][x+4], 0, 40, 110);
      }
      if (isScale) {
        for (int x = 0; x < 5; x++) {
          color(image[i][x], 100, 150, 0);
        }
      }
      color(image[i][8], 70, 70, 70);
      for (int x = 0; x < 3; x++) {
        color(image[i][x+10], 150, 0, 150);
      }
      color(image[i][14], 120, 40, 40);
      color(image[i][15], 150, 125, 0);
      if (*val == '0') {
        for (int x = 0; x < 16; x++) {
          for (int c = 0; c < 16; c++) {
            image[i][x][c] = (image[i][x][c] >> 1) + (image[i][x][c] >> 2);
          }
        }
      } else if (op == editedOp) {
        for (int x = 0; x < 16; x++) {
          for (int c = 0; c < 3; c++) {
            image[i][x][c] = image[i][x][c] + (image[i][x][c] >> 2);
            if (image[i][x][c] < 30) image[i][x][c] = 30;
          }
        }
      }
    } else if (i == 0) {
      for (int x = 0; x < 4; x++) {
        color(image[i][x], 40, 240, 40);
        color(image[i][x+4], 40, 140, 240);
      }
      color(image[i][8], 255, 0, 0);
      color(image[i][9], 100, 100, 100);
    } else if (i == 1) {
      if(isScale) {
        color(image[i][0], 0, 120, 0);
      } else {
        color(image[i][0], 100, 150, 0);
      }
      for (int x = 0; x < 7; x++) {
        color(image[i][x+8], 120, 40, 40);
      }
    }

  }
	return true;
}

#if HAVE_OLED

void Dx7UI::renderOLED(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
  if (patch && state == kStateEditing) {
    char buffer[12];
    intToString(param, buffer, 3);
    OLED::drawString(buffer, 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);

    int op = param/21;
    int idx = param%21;
    if (param < 6*21) {

      buffer[0] = 'o';
      buffer[1] = 'p';
      buffer[2] = '1' + op;
      buffer[3] = ' ';
      buffer[4] = 0;

    OLED::drawString(buffer, 4*TEXT_SPACING_X, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);

    OLED::drawString(desc[idx], 8*TEXT_SPACING_X, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
    } else if (param < 6*21+18) {
    OLED::drawString(desc_global[param-6*21], 4*TEXT_SPACING_X, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
    }

    int val = patch->currentPatch[param];
    if (param == 6*21+8) {
      val += 1; // algorithms start at one
    }
    int ybel = 5+TEXT_SIZE_Y_UPDATED+2;
    intToString(val, buffer, 3);
    OLED::drawString(buffer, 0, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);

    const char *extra = "";
    if (param < 6*21 && (idx == 11 || idx == 12)) {
      int kurva = min(val,4);
      extra = curves[kurva];
    } else if (param < 6*21 && (idx == 17)) {;
      extra = val ? "fixed" : "keyboard";
    }
    OLED::drawString(extra, 4*TEXT_SPACING_X, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);

    if (param < (6*21+8) && idx < 8) {
      renderEnvelope(image, op, idx); // op== 6 for pitch envelope
    } else if (param < 6*21 && idx < 13) {
      renderScaling(image, op, idx);
    } else if (param < 6*21 && idx < 16) {
      // :P
    } else if (param < 6*21 && idx < 21) {
      renderTuning(image, op, idx);
    } else if (param == 134) {
      renderAlgorithm(image);
    } else if (param >= 137 && param < 144) {
      renderLFO(image, param);
    }

  } else if (state == kStateLoading) {
    char buffer[12];
    intToString(cartPos+1, buffer, 3);
    OLED::drawString(buffer, 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
    progName[10] = 0; // just checking
    OLED::drawString(progName, 0, 5+TEXT_SIZE_Y_UPDATED+2, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
  } else {
    const char *text = "DX EDITOR";
    OLED::drawString(text, 0, 5, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
  }
}

void show(const char *text, int r, int c, bool inv = false) {
  int ybel = 7+(2+r)*(TEXT_SIZE_Y_UPDATED+2);
  int xpos = 4+c*TEXT_SPACING_X;
  OLED::drawString(text, xpos, ybel, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
  if (inv) {
    int width = strlen(text);
    OLED::invertArea(xpos-1, TEXT_SPACING_X*width+1, ybel-1, ybel + TEXT_SIZE_Y_UPDATED, OLED::oledMainImage);
  }
}

void show(int val, int r, int c, bool inv = false) {
  char buffer[12];
  intToString(val, buffer, 2);
  show(buffer, r, c, inv);
}

void Dx7UI::renderEnvelope(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
  for (int i = 0; i < 4; i++) {
    int val = patch->currentPatch[op*21+i];
    show(val, 0, i*3, (i == param));
    val = patch->currentPatch[op*21+4+i];
    show(val, 1, i*3, (i+4 == param));
  }
}

void Dx7UI::renderScaling(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
  char buffer[12];

  for (int i = 0; i < 2; i++) {
    int val = patch->currentPatch[op*21+9+i];
    show(val, 0, 1+i*11, (9+i == param));

    val = patch->currentPatch[op*21+11+i];
    int kurva = min(val,4);
    show(curves[kurva], 1, 1+i*11, (11+i == param));
  }

  int ybelmid = 7+2*(TEXT_SIZE_Y_UPDATED+2)+((TEXT_SIZE_Y_UPDATED+1)>>1);
  int val = patch->currentPatch[op*21+8];
  intToString(val, buffer, 2);
  int xpos = 14+6*TEXT_SPACING_X;
  OLED::drawString(buffer, xpos, ybelmid, OLED::oledMainImage[0], OLED_MAIN_WIDTH_PIXELS, TEXT_SPACING_X, TEXT_SIZE_Y_UPDATED);
  if (8 == param) {
    OLED::invertArea(xpos-1, TEXT_SPACING_X*2+1, ybelmid-1, ybelmid + TEXT_SIZE_Y_UPDATED, OLED::oledMainImage);
  }

  int noteCode = val + 17;
	if (getRootUI() == &keyboardScreen) {
    InstrumentClip *clip = (InstrumentClip*)currentSong->currentClip;
    int notedisp = noteCode - clip->yScrollKeyboardScreen;
    int x = notedisp;
    int y = 0;
    while (x>10 && y < 7) {
      x -= 5;  // fUBBIGT
      y += 1;
    }
    if (x >= 0 && x < 16) {
      // TODO: different color, do not override the main shortcut blink
      soundEditor.setupShortcutBlink(x, y, 2);
    }
  }
}

void Dx7UI::renderTuning(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int op, int param) {
  int mode = patch->currentPatch[op*21+17];
  const char *text = mode ? "fixed" : "pitch";
  show(text, 0, 1, (17 == param));

  for (int i = 0; i < 3; i++) {
    int val = patch->currentPatch[op*21+i+18];
    show(val, 0, 7+i*3, (i+18 == param));
  }

  show("level", 1, 1);

  int val = patch->currentPatch[op*21+16];
  show(val, 1, 7, (16 == param));
}

void Dx7UI::renderLFO(uint8_t image[][OLED_MAIN_WIDTH_PIXELS], int param) {
  char buffer[12];
  int ybel = 5+2*(TEXT_SIZE_Y_UPDATED+2)+2;
  int ybel2 = ybel+(TEXT_SIZE_Y_UPDATED+2);

  for (int i = 0; i < 2; i++) {
    int val = patch->currentPatch[137+i];
    show(val, 0, 1+i*3, (137+i == param));
  }

  for (int i = 0; i < 2; i++) {
    int val = patch->currentPatch[139+i];
    intToString(val, buffer, 2);
    int xpre = 10+(i*9)*TEXT_SPACING_X;
    int xpos = xpre+6*TEXT_SPACING_X;
    const char *text = (i == 0) ? "pitch" : "  amp";
    show(text, 1, 1+i*9);
    show(val, 1, 1+i*9+6, (i+139==param));
  }

  const char* sync = (patch->currentPatch[141] ? "sync" : "    ");
  show(sync, 0, 7, (141 == param));
  int shap = min((int)patch->currentPatch[142],5);
  show(shapes[shap], 0, 12, (142 == param));

  int val = patch->currentPatch[143];
  intToString(val, buffer, 1);
  show(buffer, 1, 10, (143 == param));
}

void Dx7UI::renderAlgorithm(uint8_t image[][OLED_MAIN_WIDTH_PIXELS]) {
  FmAlgorithm a = FmCore::algorithms[patch->currentPatch[134]];

  for (int i = 0; i < 6; i++) {
    int f = a.ops[i];
    char buffer[12];
    int inbus = (f >> 4) & 3;
    int outbus = f & 3;
    const char ib[] = {'.', 'x', 'y', 'z'};
    const char ob[] = {'c', 'x', 'y', 'q'};
    buffer[0] = '1'+i;
    buffer[1] = ':';
    buffer[2] = ib[inbus];
    buffer[3] = (f & OUT_BUS_ADD) ? '+': '>';
    buffer[4] = ob[outbus];
    buffer[5] = (f & (FB_IN|FB_OUT)) ? 'f' : ' ';
    buffer[6] = 0;

    int r = i/3, c = i%3;
    show(buffer, r, c*7);
  }
}
#endif

int Dx7UI::buttonAction(int x, int y, bool on, bool inCardRoutine) {
  if (on && loadPending) {
    if (x == loadButtonX && y == loadButtonY) {
      loadPending = false;

      if (!cartValid) {
        openUI(&dx7SyxBrowser);  // find a syx cartridge first
      } else {
        doLoad(0);
      }
      return ACTION_RESULT_DEALT_WITH;
    }
  }

  if (x == loadButtonX && y == loadButtonY && on) {
    if (state != kStateLoading) {
      popupText("overwrite? press again", false);
      loadPending = true;
    } else {
      // already in loading, reopen browser
      openUI(&dx7SyxBrowser);
    }
  } else if (x == clipViewButtonX && y == clipViewButtonY) {
    did_button = true;
    uiTimerManager.unsetTimer(TIMER_SHORTCUT_BLINK);
    state = kStateNone;
    renderUI();
  } else if (x == backButtonX && y == backButtonY) {
    close();
  }
  return ACTION_RESULT_DEALT_WITH;
}


Dx7SyxBrowser::Dx7SyxBrowser() {
#if HAVE_OLED
	fileIcon = OLED::waveIcon;
	title = "DX7 syx files";
#else
	shouldWrapFolderContents = false;
#endif
  }

char const* allowedFileExtensionsSyx[] = {"SYX", NULL};
bool Dx7SyxBrowser::opened() {

	bool success = Browser::opened();
	if (!success) return false;

  allowedFileExtensions = allowedFileExtensionsSyx;

  allowFoldersSharingNameWithFile = true;
  instrumentTypeToLoad = 255;
  qwertyVisible = false;

#if HAVE_OLED
  fileIndexSelected = 0;
#endif

	int error = storageManager.initSD();
	if (error) goto sdError;

  currentDir.set("DX7");

  // TODO: fill in last used name!
  error = arrivedInNewFolder(1, "", "DX7");
	if (error) goto sdError;

  return true;
sdError:
		numericDriver.displayError(error);
		return false;
}

// TODO: this is identical to SampleBrowser, move to parent class?
int Dx7SyxBrowser::getCurrentFilePath(String* path) {
	int error;

	path->set(&currentDir);
	int oldLength = path->getLength();
	if (oldLength) {
		error = path->concatenateAtPos("/", oldLength);
		if (error) {
gotError:
			path->clear();
			return error;
		}
	}

	FileItem* currentFileItem = getCurrentFileItem();

	error = path->concatenate(&currentFileItem->filename);
	if (error) goto gotError;

	return NO_ERROR;
}

void Dx7SyxBrowser::enterKeyPress() {
	FileItem* currentFileItem = getCurrentFileItem();
	if (!currentFileItem) {
    return;
  }


		if (currentFileItem->isFolder) {
      // [SIC]
			char const* filenameChars =
			    currentFileItem->filename
			        .get(); // Extremely weirdly, if we try to just put this inside the parentheses in the next line,
			                // it returns an empty string (&nothing). Surely this is a compiler error??

			int error = goIntoFolder(filenameChars);
			if (error) {
				numericDriver.displayError(error);
				close(); // Don't use goBackToSoundEditor() because that would do a left-scroll
				return;
			}
    } else {
      // TODO: c.f. slotbrowser, we might just be able to pass a file pointer to the FAT loader
      String path;
      getCurrentFilePath(&path);
      int len = path.getLength();
      close();
      dx7ui.openFile(path.get());
    }
}

