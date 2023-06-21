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

#include "dexed_global_params.h"
#include "dsp/dexed/engine.h"
#include "gui/menu_item/dexed_operator_params.h"
#include "gui/menu_item/dexed_param.h"
#include "gui/ui/browser/dexed_browser.h"
#include "gui/ui/sound_editor.h"
#include "gui/ui_timer_manager.h"
#include "memory/general_memory_allocator.h"
#include "processing/source.h"
#include "storage/dexed/PluginData.h"
#include "util/container/static_vector.hpp"

namespace deluge::gui::menu_item {

DexedGlobalParams dexedGlobalParams{l10n::String::STRING_FOR_DEXED_GLOBAL_PARAMS};

void DexedGlobalParams::beginSession(MenuItem* navigatedBackwardFrom) {
	readValueAgain();
}

void DexedGlobalParams::readValueAgain() {
	if (display->haveOLED()) {
		renderUIsForOled();
	}
	else {
		drawValue();
	}
}

struct {
	const char *name;
	int index;
} items[] = {
    {"algorithm", 134},
	{"feedback", 135},
	{"sync", 136},
	{"operator 1", -1},
	{"operator 2", -2},
	{"operator 3", -3},
	{"operator 4", -4},
	{"operator 5", -5},
	{"operator 6", -6},
	{"pitch env.", 6*21},
	{"lfo", 137},
};

constexpr int numValues = sizeof(items) / sizeof(items[0]);

void DexedGlobalParams::drawPixelsForOled() {
	static_vector<std::string_view, numValues> itemNames = {};
	for (auto & item : items) {
		itemNames.push_back(item.name);
	}
	drawItemsForOled(itemNames, currentValue - scrollPos, scrollPos);
}

void DexedGlobalParams::drawValue() {
	display->setScrollingText(items[currentValue].name);
}

void DexedGlobalParams::selectEncoderAction(int32_t offset) {
	int32_t newValue = currentValue + offset;

	if (display->haveOLED()) {
		if (newValue >= numValues || newValue < 0) {
			return;
		}
	}
	else {
		if (newValue >= numValues) {
			newValue %= numValues;
		}
		else if (newValue < 0) {
			newValue = (newValue % numValues + numValues) % numValues;
		}
	}

	currentValue = newValue;

	if (display->haveOLED()) {
		if (currentValue < scrollPos) {
			scrollPos = currentValue;
		}
		else if (currentValue >= scrollPos + kOLEDMenuNumOptionsVisible) {
			scrollPos++;
		}
	}

	readValueAgain();
}

MenuItem* DexedGlobalParams::selectButtonPress() {
	int index = items[currentValue].index;
	if (index >= 0) {
		dexedParam.param = index;
		return &dexedParam;
	} else if (index >= -6) {
		dexedOperatorParams.op = 6+index;
		dexedOperatorParams.format(-index);
		return &dexedOperatorParams;
	}
	return nullptr;
}

} // namespace deluge::gui::menu_item
