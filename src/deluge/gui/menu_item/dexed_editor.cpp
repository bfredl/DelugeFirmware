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

#include "dexed_editor.h"
#include "gui/ui/dexed_editor.h"
#include "gui/ui_timer_manager.h"
#include "processing/source.h"

namespace deluge::gui::menu_item {

DexedEditorMenu dexedEditorMenu{l10n::String::STRING_FOR_DEXED_EDITOR};

void DexedEditorMenu::beginSession(MenuItem* navigatedBackwardFrom) {
#ifdef DEXED_EDITOR
	soundEditor.shouldGoUpOneLevelOnBegin = true;
	dexedEditor.patch = soundEditor.currentSource->ensureDx7Patch();
	bool success = openUI(&dexedEditor);
	if (!success) {
		uiTimerManager.unsetTimer(TimerName::SHORTCUT_BLINK);
	}
#endif
}

} // namespace deluge::gui::menu_item
