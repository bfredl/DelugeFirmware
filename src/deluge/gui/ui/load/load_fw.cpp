#include "load_fw.h"
#include "hid/display/numeric_driver.h"
#include "memory/general_memory_allocator.h"
#include "util/chainload.h"
#include "hid/display/oled.h"

LoadFirmwareUI::LoadFirmwareUI() {
#if HAVE_OLED
	fileIcon = OLED::rightArrowIcon;
	title = "firmware files";
#else
	shouldWrapFolderContents = false;
#endif
  }

char const* allowedFileExtensionsSyx[] = {"BIN", NULL};
bool LoadFirmwareUI::opened() {

	bool success = Browser::opened();
	if (!success) return false;

	allowedFileExtensions = allowedFileExtensionsSyx;

	allowFoldersSharingNameWithFile = true;
	instrumentTypeToLoad = InstrumentType::NONE;
	qwertyVisible = false;

#if HAVE_OLED
	fileIndexSelected = 0;
#endif

	int error = storageManager.initSD();
	if (error) goto sdError;

  currentDir.set("IMAGES");

  // TODO: fill in last used name!
  error = arrivedInNewFolder(1, "", "IMAGES");
	if (error) goto sdError;

  return true;
sdError:
		numericDriver.displayError(error);
		return false;
}

// TODO: this is identical to SampleBrowser, move to parent class?
int32_t LoadFirmwareUI::getCurrentFilePath(String* path) {
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

void load_firmware(const char *path) {
	FILINFO fno;

	int result = f_stat(path, &fno);
	FSIZE_t fileSize = fno.fsize;

	FIL currentFile;
	// Open the file
	result = f_open(&currentFile, path, FA_READ);
	if (result != FR_OK) {
		numericDriver.displayPopup(HAVE_OLED ? "FAILE" : "NONE");
		return;
	}

	UINT numBytesRead;
	uint8_t* buffer = (uint8_t*)GeneralMemoryAllocator::get().alloc(fileSize, NULL, false, true);
	if (!buffer) {
		numericDriver.displayPopup("VERY fAIL");
		return;
	}

	int status = f_read(&currentFile, buffer, fileSize, &numBytesRead);
	if (status) {
		numericDriver.displayPopup("read fAIL");
		goto free_ret;
	}

	chainload_from_buf(buffer, fileSize);

free_ret:
	GeneralMemoryAllocator::get().dealloc(buffer);
}

void LoadFirmwareUI::enterKeyPress() {
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

      load_firmware(path.get());
    }
}

LoadFirmwareUI loadFirmwareUI;
