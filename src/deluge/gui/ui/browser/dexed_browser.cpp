
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

#include "definitions_cxx.hpp"
#include "gui/ui/browser/dexed_browser.h"
#include "hid/display/oled.h"

DexedSyxBrowser::DexedSyxBrowser() {
	fileIcon = deluge::hid::display::OLED::waveIcon;
	title = "DX7 syx files";
	shouldWrapFolderContents = false;
}

static char const* allowedFileExtensionsSyx[] = {"SYX", NULL};
bool DexedSyxBrowser::opened() {

	bool success = Browser::opened();
	if (!success) return false;

	allowedFileExtensions = allowedFileExtensionsSyx;

	allowFoldersSharingNameWithFile = true;
	outputTypeToLoad = OutputType::NONE;
	qwertyVisible = false;

	fileIndexSelected = 0;

	int error = storageManager.initSD();
	if (error) goto sdError;

  currentDir.set("DX7");

  // TODO: fill in last used name!
  error = arrivedInNewFolder(1, "", "DX7");
	if (error) goto sdError;

  return true;
sdError:
		display->displayError(error);
		return false;
}

// TODO: this is identical to SampleBrowser, move to parent class?
int32_t DexedSyxBrowser::getCurrentFilePath(String* path) {
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

void DexedSyxBrowser::enterKeyPress() {
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
				display->displayError(error);
				close(); // Don't use goBackToSoundEditor() because that would do a left-scroll
				return;
			}
    } else {
      // TODO: c.f. slotbrowser, we might just be able to pass a file pointer to the FAT loader
      String path;
      getCurrentFilePath(&path);
      int len = path.getLength();
      close();

      //dx7ui.openFile(path.get());
	  display->displayPopup(path.get());
    }
}

DexedSyxBrowser dexedBrowser{};
