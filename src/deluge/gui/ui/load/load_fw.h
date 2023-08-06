#include "gui/ui/browser/browser.h"

class LoadFirmwareUI final : public Browser {
public:
	LoadFirmwareUI();
	bool opened();
	void enterKeyPress();
	int32_t getCurrentFilePath(String* path) override;
};


extern LoadFirmwareUI loadFirmwareUI;
